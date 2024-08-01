#include "../include/Neural_Armor_Detection.h"

using namespace cv;
using namespace std;
using namespace dnn;
using namespace Robot;

/**
 * B:蓝色   R:红色  N:熄灭  P:紫色(在后续模型中废弃,合并为红色)
 * G:哨兵   1:1号   2:2号   3:3号   4:4号   5:5号
 * O:前哨站     Bs:基地     Bb:基地大装甲
*/
const std::vector<std::string> coconame = {
        "BG", "B1", "B2", "B3", "B4", "B5", "BO", "BBs", "BBb", // 1 2 3 4 5 6->0
        "RG", "R1", "R2", "R3", "R4", "R5", "RO", "RBs", "RBb", // 10 11 12 所以 - 9
        "NG", "N1", "N2", "N3", "N4", "N5", "NO", "NBs", "NBb",
        "PG", "P1", "P2", "P3", "P4", "P5", "PO", "PBs", "PBb"
};

// 类别颜色
const std::vector<cv::Scalar> CLASS_COLORS = {
        cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255),
        cv::Scalar(255, 255, 0), cv::Scalar(0, 255, 255), cv::Scalar(255, 0, 255),
        cv::Scalar(192, 192, 192), cv::Scalar(128, 128, 128), cv::Scalar(128, 0, 0),
        cv::Scalar(128, 128, 0), cv::Scalar(0, 128, 0), cv::Scalar(128, 0, 128),
        cv::Scalar(0, 128, 128), cv::Scalar(0, 0, 128), cv::Scalar(72, 61, 139),
        cv::Scalar(47, 79, 79), cv::Scalar(47, 79, 47), cv::Scalar(0, 100, 0),
        cv::Scalar(85, 107, 47), cv::Scalar(139, 69, 19), cv::Scalar(160, 82, 45),
        cv::Scalar(255, 140, 0), cv::Scalar(255, 165, 0), cv::Scalar(255, 215, 0),
        cv::Scalar(184, 134, 11), cv::Scalar(218, 165, 32), cv::Scalar(238, 232, 170),
        cv::Scalar(189, 183, 107), cv::Scalar(0, 128, 128), cv::Scalar(0, 139, 139),
        cv::Scalar(25, 25, 112), cv::Scalar(70, 130, 180), cv::Scalar(100, 149, 237),
        cv::Scalar(123, 104, 238), cv::Scalar(106, 90, 205), cv::Scalar(176, 196, 222)
};

// 参数读取部分
NeuralArmorDetector::NeuralArmorDetector() {
    //============================= 参数文件路径 =============================
    std::string package_path = ros::package::getPath("robot_detection");
    cv::FileStorage fs(package_path + "/SetParam_File/Neural_Detect_data.yaml", cv::FileStorage::READ);

    /** 置信度阈值 */
    this->confThreshold = (float)fs["confThreshold"];
    this->nmsThreshold = (float)fs["nmsThreshold"];
    this->scoreThreshold = (float)fs["scoreThreshold"];

    /** 模型参数 */
    this->detectClassStartIndex = (int)fs["detectClassStartIndex"];
    this->detectRange = (int)fs["detectRange"];
    this->detectPointIndex = (int)fs["detectPointIndex"];

    /** 图像resize大小 */
    this->inpWidth = 640;
    this->inpHeight = 640;

    /** 模型加载缓存路径 */
    this->model_cache = (string)fs["model_cache"];
 
    this->onnx_path = (string)fs["onnx_path"];

    /** */
    this->initial_model();
}


NeuralArmorDetector::~NeuralArmorDetector(){}

/**
 * 对传入的画面进行装甲板检测.
 * 现在的返回类型是void,
 * 再将postprocess_img()函数修改为返回装甲板后,detect的数据1返回类型也应该修改
 *
 * @param frame 输入的原始图像。
 */
std::vector<Armor> NeuralArmorDetector::detect(Mat & frame) {
    preprocess_img_letterBox(frame);

    infer_request.infer();

    //异步推理
//    infer_request.start_async();
//    infer_request.wait();
    const ov::Tensor& output_tensor = infer_request.get_output_tensor();
    ov::Shape output_shape = output_tensor.get_shape();
    float* detections = output_tensor.data<float>();
    this->frame_w = frame.cols;
    this->frame_h = frame.rows;

    return this->postprocess_img(frame, detections, output_shape);
}

/**
 * 初始化模型以供后续使用。
 *
 * 本函数执行以下主要任务：
 * 1. 创建OpenVINO核心对象并设置模型缓存目录。
 * 2. 读取ONNX模型文件。
 * 3. 设置模型的输入和输出预处理步骤，包括数据类型、布局和颜色格式转换等。
 * 4. 编译模型，优化性能，并创建推理请求对象。
 */
void NeuralArmorDetector::initial_model() {
    ov::Core core; // 创建OpenVINO核心对象。
    core.set_property(ov::cache_dir(this->model_cache)); // 启用模型缓存功能，并指定缓存目录。

    // 读取模型文件，`this->onnx_path`是模型文件的路径。
    std::shared_ptr<ov::Model> model = core.read_model(this->onnx_path);

    // 创建预处理和后处理配置对象。
    ov::preprocess::PrePostProcessor ppp = ov::preprocess::PrePostProcessor(model);

    // 配置模型输入的预处理步骤。
    // 设置输入数据的元素类型为无符号8位整数，布局为NHWC，颜色格式为RGB。
    ppp.input().tensor().set_element_type(ov::element::u8).set_layout("NHWC").set_color_format(ov::preprocess::ColorFormat::RGB);

    // 配置模型输出的后处理步骤。
    // 设置输出数据的元素类型为32位浮点数。
    ppp.output().tensor().set_element_type(ov::element::f32);

    // 配置进一步的输入预处理操作。
    // 转换元素类型为32位浮点数，颜色格式转换为BGR，并进行缩放。
    ppp.input().preprocess().convert_element_type(ov::element::f32).convert_color(ov::preprocess::ColorFormat::BGR).scale({ 255, 255, 255 });

    // 设置模型的输入布局。
    ppp.input().model().set_layout("NCHW");

    // 应用预处理和后处理配置。
    model = ppp.build();

    // 编译模型，设置设备为自动选择（"AUTO"），优化性能模式为低延迟，推理精度为16位浮点数。
    this->compiled_model = core.compile_model(model, "GPU", ov::hint::performance_mode(ov::hint::PerformanceMode::LATENCY), ov::hint::inference_precision(ov::element::f16));

    // 创建推理请求对象，用于执行推理。
    this->infer_request = compiled_model.create_infer_request();
}


/**
 * 对输入图像进行预处理，包括缩放和添加边框，以适应模型的输入尺寸。
 *
 * 此函数首先将图像缩放到接近模型输入尺寸的大小，然后在必要的边缘添加黑色边框，
 * 以确保图像完整地填充模型的输入尺寸，同时保持原始图像的宽高比不变。
 *
 * @param frame 输入的原始图像。
 */
void NeuralArmorDetector::preprocess_img_letterBox(cv::Mat &frame) {
    try {
        // 计算图像缩放比例，以确保缩放后的图像能够保持原始宽高比并适应目标尺寸。
        // 使用 static_cast<float>() 来确保 inpWidth/frame.cols 和 inpHeight/frame.rows 的计算结果为浮点数，避免整数除法导致的精度丢失。
        float scale_ratio = std::min(static_cast<float>(inpWidth) / frame.cols, static_cast<float>(inpHeight) / frame.rows);

        // 根据计算出的缩放比例，确定缩放后图像的新尺寸。
        // 这里将缩放比例应用于原图像的宽度和高度，并将结果转换为整数，因为像素的数量不能是小数。
        cv::Size scaled_size(int(frame.cols * scale_ratio), int(frame.rows * scale_ratio));

        // 使用 cv::resize 函数对原始图像进行缩放操作，得到缩放后的图像 scaled_frame。参数说明：
        // 0, 0：缩放时的x轴和y轴的比例，这里不使用这两个参数，因为缩放的大小已经通过 scaled_size 指定。
        // cv::INTER_LINEAR：缩放时使用的插值方法，INTER_LINEAR 表示双线性插值，适用于缩放操作，可以在保证速度的同时获得较好的视觉效果。
        cv::Mat scaled_frame;
        cv::resize(frame, scaled_frame, scaled_size, 0, 0, cv::INTER_LINEAR);

        // 计算边框的大小
        int top_border = (inpHeight - scaled_size.height) / 2;
        int bottom_border = inpHeight - scaled_size.height - top_border;
        int left_border = (inpWidth - scaled_size.width) / 2;
        int right_border = inpWidth - scaled_size.width - left_border;

        // 添加黑色边框以生成最终的输入图像
        cv::Mat letterbox_frame;
        cv::copyMakeBorder(scaled_frame, letterbox_frame, top_border, bottom_border, left_border, right_border, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

        // 更新缩放比例和偏移量，供后续处理模型推理结果时,将结果映射到原画面上时使用
        this->rx = static_cast<float>(frame.cols) / scaled_size.width;
        this->ry = static_cast<float>(frame.rows) / scaled_size.height;
        this->dx = left_border; // 水平偏移量
        this->dy = top_border;  // 垂直偏移量

        // 准备模型输入数据
        float* input_data = reinterpret_cast<float*>(letterbox_frame.data);
        ov::Tensor input_tensor = ov::Tensor(compiled_model.input().get_element_type(), compiled_model.input().get_shape(), input_data);
        infer_request.set_input_tensor(input_tensor);

    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "未知异常" << std::endl;
    }
}

/**
 * 对模型的检测结果进行后处理，包括解析检测结果、应用非极大值抑制（NMS）和在图像上绘制检测框和关键点。
 * 这是后处理函数的第二版, 第一版将模型输出转化为Mat类型的数据, 通过使用Mat的相关方法进行索引
 * 而此第二版使用的是指针在数据上直接进行操作, 减少了Mat对象的创建和引用, 延迟降低了 1ms~2ms.
 *
 * 对于模型输出的解释: 模型的输出是连续的8400x50个数, 可以相当于一个行(row)数位50,列(col)数为8400的矩阵,每一列都是模型的一个检测结果的完整检测数据,
 * 也就是说每一行都是每一个检测结果的相同属性,比如这个矩阵的第一行,是每个检测结果的bbox的cx,二第二行是每个检测结果的cy.
 * 又因为模型的输出结果是连续的,相当于
 * 矩阵[1, 2
 *      3, 4]表达为连续的 1, 2, 3, 4.
 * 所以由此可推从第0位到第8399位是8400个检测结果的bbox的cx值,接下来从8400到16799是8400个检测结果的bbox的cy值
 *
 * @param frame 输入的图像，将在其上绘制检测结果。
 * @param detections 模型的原始输出，包含检测到的对象的信息。
 * @param output_shape 模型输出的维度信息。
 */

std::vector<Armor> NeuralArmorDetector::postprocess_img(cv::Mat &frame, float *detections, ov::Shape &output_shape) {
    // 模型输出的维度，其中行数为属性数量，列数为检测结果数量
    //int num_attributes = output_shape[1]; // 属性数量，这个模型是50个, 为什么用不上注释了呢?因为下面所有属性的提取我们都是写好的,用不上这个值来循环
    int num_detections = output_shape[2]; // 检测结果数量，这个模型是8400个检测结果

    // 定义用于存储检测结果的容器
    std::vector<cv::Rect> boxes;
    vector<int> class_ids;
    vector<float> confidences;
    std::vector<std::vector<cv::Point2f>> keyPointS;

    /** 根据电控ing传来的颜色赋值装甲板的筛选范围 */
    if (enemy_color == RED) {
        this->detectClassStartIndex = 9;
        this->detectRange = 18;
    }
    else if (enemy_color == BLUE) {
        this->detectClassStartIndex = 0;
        this->detectRange = 9;
    }

    //TODO 下面这段注释,
    // 采用分块求和的方式，即对每一个检测结果的置信度进行累加
    // 累加一定数量的类别置信度后就检查当前的和是否已经超过阈值，如果超过则提前终止求和，进一步处理这个检测结果。
    // 如果没有超过阈值就不寻找其中的的最大值, 这是否是一种优化的方式? 因为求和比用条件判断更快!使用以下筛选需要在 postprocess的最后面补上1个 }
//    float sum_threshold = 0.7f; // 设置一个求和阈值
//
//    for (int i = 0; i < num_detections; ++i) {
//        float sum_score = 0.0;
//
//        // 对每个检测结果的类别置信度求和
//        for (int j = 0; j < 36; ++j) { // 假设有36个类别
//            sum_score += detections[i + (4 + j) * num_detections];
//            if (sum_score > sum_threshold) {
//                break; // 如果求和结果已超过阈值，则提前终止求和
//            }
//        }
//        float max_score = 0.0;
//        if (sum_score <= sum_threshold) {
//            continue; // 如果求和结果未超过阈值，则跳过这个检测结果
//        } else {
//            int class_id = -1;
//            for (int j = 0; j < 36; ++j) { // 假设有36个类别
//                float score = detections[i + (4 + j) * num_detections];
//                if (score > max_score) {
////                if (score > 0.7) cout << "score" << j << " : " << score << endl;
//                    max_score = score;
//                    class_id = j;
//                }
//            }

    //TODO 这个是常规的筛选方式, 对每个检测结果,寻找其最大的score, 然后用max_Score去和confThreshold进行比较
    // 相比上面要少个}
//    enemyColor = ....
    for (int i = 0; i < num_detections; ++i) {
        // 遍历每一个检测结果
        // 从检测结果中提取最大的类别分数和对应的类别ID

        //TODO 如果enemyColor为2也就是蓝色,且i在0到8之间，或者如果enemyColor为1也就是红色且i在9到17之间，则跳过当前循环迭代
//        if ((enemyColor == 2 && i >= 0 && i <= 8) || (enemyColor == 1 && i >= 9 && i <= 17)) {
//            continue;
//        }

        float max_score = 0.0;
        int class_id = -1;
        for (int j = detectClassStartIndex; j < this->detectRange; ++j) {
            // 这里36指遍历36个类别的置信度,如果只要BLUE和RED的置信度,可以将其改为18, 直接拦腰截断了一半,
            // 这里可以根据敌方的眼神开选择j的赋值和区间, 来提高推理速度和容错率, 比赛上不同方的机器人和前哨站,基地,颜色都是不一样的.
            float score = detections[i + (4 + j) * num_detections];
            if (score > max_score) {
                max_score = score;
                class_id = j;
            }
        }

        // 如果最大分数大于置信度阈值，则记录该检测结果
        if (max_score > confThreshold) {
            float cx = detections[i]; // 第0行到第8399行是cx
            float cy = detections[i + num_detections]; // 第8400行到第16799行是cy
            float ow = detections[i + 2 * num_detections]; // 宽 16800 ~ 25199
            float oh = detections[i + 3 * num_detections]; // 高
            // 计算边界框的左上角点和宽高
            cv::Rect box(static_cast<int>(cx - 0.5 * ow), static_cast<int>(cy - 0.5 * oh), static_cast<int>(ow), static_cast<int>(oh));
            boxes.push_back(box);
            class_ids.push_back(class_id);
            confidences.push_back(max_score);

            // 提取关键点, 10位数 也就是五组xy.
            // 关键点的顺序为: 中心, 左上, 左下, 右下, 右上
            vector<cv::Point2f> kpts;
            for (int k = 0; k < 5; ++k) {
                float kpt_x = ((detections[i + (detectPointIndex + k * 2) * num_detections] - this->dx) * this->rx);
                float kpt_y = ((detections[i + ((detectPointIndex + 1) + k * 2) * num_detections] - this->dy) * this->ry);
                kpts.push_back(cv::Point2f(kpt_x, kpt_y));
            }
            keyPointS.push_back(kpts);
        }
    }

    // 非极大值抑制（NMS）
    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, nms_result);


    // 存储最终的检测结果
    std::vector<Detection> output;
    for (int idx : nms_result) {
        Detection result;
        result.class_id = class_ids[idx];
        result.confidence = confidences[idx];
        result.box = boxes[idx];
        result.keyPoint = keyPointS[idx];
        output.emplace_back(result);
    }
//    cout << "frame.col: " << this->frame_w << "  frame.row: " << this->frame_h << endl;
//    drawDetections(frame, output);
    std::vector<Armor> finalArmors = neuralArmorGrade(output);
    drawArmors(frame ,neuralArmorGrade(output));
    return finalArmors;
//    for (auto armor : finalArmors) {
//        cout << "id: " << armor.id << endl
//        << "grade" << armor.grade << endl
//        << "confi" << armor.confidence << endl;
//    }
}


/**
 * 对模型推理结果进行处理
 * 进行装甲板的打分和判断,返回最终装甲板
 * 
*/
// TODO: 打分机制需要优化,在这里进行装甲板的颜色判断和击打选择
std::vector<Armor> NeuralArmorDetector::neuralArmorGrade(const std::vector<Detection>& candidateArmors) {
    std::vector<Armor> finalArmors;
    for (const auto &candidateArmor: candidateArmors) {
        Armor armor;
        //TODO 计算装甲板宽度,并赋值大小装甲板 要注意测试,能否正确检测大装甲板, 如果不行就求两个的最小值
        double armorWidth = (POINT_DIST(candidateArmor.keyPoint[2], candidateArmor.keyPoint[3]) + POINT_DIST(candidateArmor.keyPoint[1], candidateArmor.keyPoint[4])) / 2;
        double armorHeight = (POINT_DIST(candidateArmor.keyPoint[2], candidateArmor.keyPoint[1]) + POINT_DIST(candidateArmor.keyPoint[4], candidateArmor.keyPoint[3])) / 2;
        // 宽高比筛选条件
//        cout << "box w:" << static_cast<int>(candidateArmor.box.width * this->rx) << "  box h:" << static_cast<int>(candidateArmor.box.height * this->ry) << endl;
//        cout << "armor w:" << armorWidth << "  armow h:" << armorHeight << endl;
        bool small_wh_ratio_ok = armor_small_min_wh_ratio < armorWidth/armorHeight && armorWidth/armorHeight < armor_small_max_wh_ratio;
//        bool big_wh_ratio_ok = armor_big_min_wh_ratio < armorWidth/armorHeight && armorWidth/armorHeight < armor_big_max_wh_ratio;
//        cout << "snallOK:" << small_wh_ratio_ok << "  bigOK:" << big_wh_ratio_ok << endl;


        // 图像中心、打分
        int near_grade;
        double pts_distance = POINT_DIST(candidateArmor.keyPoint[0], Point2f(this->frame_w * 0.5, this->frame_h * 0.5));
        near_grade = pts_distance/near_standard < 1 ? 100 : (near_standard/pts_distance) * 100;

        //TODO id 打分, 后面如果根据颜色来nms, 这里应该改进一下
        int id_grade = ((candidateArmor.class_id == 1) || (candidateArmor.class_id == 10)) ? 100 : 80;

        // 最大装甲板，用面积，找一个标准值（固定距离（比如3/4米），装甲板大小（Armor.area）大约是多少，分大小装甲板）
        // 这段代码的作用是给最大装甲板打分，根据装甲板的高度与预设的标准高度之间的比值来计算分数。
        // 如果装甲板是唯一的候选装甲板，那么分数将为100。否则，分数将根据比值乘以一个权重来计算，可能得出的值会很小。
        int height_grade;
        double hRotation = armorHeight / height_standard;
        if(candidateArmors.size()==1)  hRotation=1;
        height_grade = hRotation * 60;

        // 下面的系数得详细调节；
        int final_grade = id_grade * id_grade_ratio +
                          height_grade  * height_grade_ratio +
                          near_grade  * near_grade_ratio;

        if (final_grade > grade_standard)
        {
            armor.armor_pt4[0] = candidateArmor.keyPoint[2]; // 左下
            armor.armor_pt4[1] = candidateArmor.keyPoint[3]; // 右下
            armor.armor_pt4[2] = candidateArmor.keyPoint[4]; // 右上
            armor.armor_pt4[3] = candidateArmor.keyPoint[1]; // 左上
            if(small_wh_ratio_ok) armor.type = SMALL;
            else armor.type = BIG;
            if (candidateArmor.class_id < 18) {
                // only use RED and BLUE
                armor.id = candidateArmor.class_id;
                if (armor.id > 8) armor.id -= 9;
                if (armor.id == 6) armor.id = 9; // index 6 is armor class B0

                // 颜色判别 | 此处为简单应付,后续将颜色判断搬到上面
                // std::string color = coconame[candidateArmor.class_id];
                // int enemyColor;
                // if(color[0] == 'R') enemyColor = RED;
                // else if(color[0] == 'B') enemyColor = BLUE;
                // if(enemyColor != BLUE) continue;
            }
            armor.grade = final_grade;
            armor.confidence = candidateArmor.confidence;
            finalArmors.emplace_back(armor);
        }
    }
    return finalArmors;
}




/**
 * 将模型的检测结果在画面中可视化出来,包括文本,bbox,关键点
 *
 * @param frame 输入的图像，将在其上绘制检测结果。
 * @param detections 经过postprocess处理后的输出，是可用的装甲板的信息。
 */
void NeuralArmorDetector::drawDetections(cv::Mat &frame, const std::vector<Detection> &detections) {

    for (const auto &detection: detections) {
        auto box = detection.box;
        int class_id = detection.class_id;
        cv::Scalar color = CLASS_COLORS[class_id % CLASS_COLORS.size()];

        //因为使用了 letterBox 来处理图像,所以bbox的正确属参数需要映射回去!
        box.x = static_cast<int>((box.x - this->dx) * this->rx);
        box.y = static_cast<int>((box.y - this->dy) * this->ry);
        box.width = static_cast<int>(box.width * this->rx);
        box.height = static_cast<int>(box.height * this->ry);

        float xmax = box.x + box.width;
        float ymax = box.y + box.height;

        // 绘制边界框-
        cv::rectangle(frame, cv::Point(box.x, box.y), cv::Point(xmax, ymax), color, 3);

        // 绘制边界框上的文本
        std::string classString =
                coconame[detection.class_id] + ' ' + std::to_string(detection.confidence).substr(0, 4);
        cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
        cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);
        cv::rectangle(frame, textBox, color, cv::FILLED);
        cv::putText(frame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1,
                    cv::Scalar(0, 0, 0), 2);


        // 绘制关键点 因为使用letterBox来处理了图像,所以要把推理得到的点映射回原来的图像上
        for (const auto &kpt: detection.keyPoint) {
            // 调整坐标以映射回原始图像, 提取坐标的时候已经映射了
//            int kpt_x = static_cast<int>((kpt.x - this->dx) * this->rx);
//            int kpt_y = static_cast<int>((kpt.y - this->dy) * this->ry);

            cv::circle(frame, cv::Point(int(kpt.x), int(kpt.y)), 3, cv::Scalar(0, 0, 255), -1);
        }

    }
}

void NeuralArmorDetector::drawArmors(cv::Mat &frame, const std::vector<Armor> &armors) {
    for (const auto &armor : armors) {
        // 绘制装甲板的四边形轮廓
        for (int i = 0; i < 4; i++) {
            cv::line(frame, armor.armor_pt4[i], armor.armor_pt4[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
        }

        // 突出显示四个角点
        for (const auto &point : armor.armor_pt4) {
            cv::circle(frame, point, 3, cv::Scalar(0, 0, 255), -1);
        }

        // 文本基础位置（基于第一个点）
        cv::Point textBasePos = armor.armor_pt4[0] + cv::Point2f(15, -1);

        // 显示 ID
        std::string idText = "ID: " + std::to_string(armor.id);
        cv::putText(frame, idText, textBasePos, cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 0), 1);

        // 计算第二行文本的位置（基于第一行文本的位置稍微向下偏移）
        int textHeight = 15; // 假设文本高度大约为 15 像素
        cv::Point secondLinePos = textBasePos + cv::Point(5, textHeight);

        // 显示 Grade 和 Conf
        std::string gcText = "Grade: " + std::to_string(armor.grade) + " Conf: " + std::to_string(armor.confidence);
        cv::putText(frame, gcText, secondLinePos, cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 1);
    }
}