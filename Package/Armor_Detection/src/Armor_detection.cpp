#include "../include/Armor_detection.h"

//#define BINARY_SHOW                 // 展示二值化图片
//#define DRAW_LIGHTS_CONTOURS        // 绘制灯条轮廓
//#define DRAW_LIGHTS_RRT             // 绘制灯条
//#define SHOW_NUMROI                 // 展示数字图像处理图片
//#define DRAW_ARMORS_RRT             // 绘制装甲板
#define DRAW_FINAL_ARMOR_S_CLASS    //展示最终装甲板
//#define SHOW_TIME                   // 输出各个部分花费时间


using namespace cv;
using namespace std;

ArmorDetector::ArmorDetector()
{
    //============================= 参数文件路径 =============================
    FileStorage fs(PATH "Detect_data.yaml", FileStorage::READ);

    /** 二值化阈值*/
    binThresh = (int)fs["binThresh"];

    /** 灯条识别参数读取 */
    light_max_angle = (double)fs["light_max_angle"];
    light_min_hw_ratio = (double)fs["light_min_hw_ratio"];
    light_max_hw_ratio = (double)fs["light_max_hw_ratio"];
    light_min_area_ratio = (double)fs["light_min_area_ratio"];   // contourArea / RotatedRect
    light_max_area_ratio = (double)fs["light_max_area_ratio"];
    light_max_area = (double)fs["light_max_area"];

    /** 装甲板识别参数读取 */
    armor_big_max_wh_ratio = (double)fs["armor_big_max_wh_ratio"];
    armor_big_min_wh_ratio = (double)fs["armor_big_min_wh_ratio"];
    armor_small_max_wh_ratio = (double)fs["armor_small_max_wh_ratio"];
    armor_small_min_wh_ratio = (double)fs["armor_small_min_wh_ratio"]; // 装甲板宽高比真的这么设吗
    armor_max_offset_angle = (double)fs["armor_max_offset_angle"];
    armor_height_offset = (double)fs["armor_height_offset"];
    armor_ij_min_ratio = (double)fs["armor_ij_min_ratio"];
    armor_ij_max_ratio = (double)fs["armor_ij_max_ratio"];
    armor_max_angle = (double)fs["armor_max_angle"];

    /** 装甲板打分参数读取 */
    near_standard = (double)fs["near_standard"];
    height_standard = (int)fs["height_standard"];
    grade_standard = (int)fs["grade_standard"];             // 及格分

    /** 装甲板打分比例参数读取 */
    id_grade_ratio = (double)fs["id_grade_ratio"];
    near_grade_ratio = (double)fs["near_grade_ratio"];
    height_grade_ratio = (double)fs["height_grade_ratio"];

    /** 装甲板种类 (用于推理结果指向下一行) */
    categories = (int)fs["categories"];

    /** 置信度阈值 */
    thresh_confidence = (float)fs["thresh_confidence"];

    fs.release();
}

/**
 *  函数名: setImage
 *  传入: Mat src  (原始图像)
 *  传出: 无
 *  功能: 将传入的原始图像复制给_src并二值化
 */
void ArmorDetector::setImage(const Mat &src)
{
    src.copyTo(_src);
    //二值化
    Mat gray;
    cvtColor(_src,gray,COLOR_BGR2GRAY);
    threshold(gray,_binary,binThresh,255,THRESH_BINARY);
#ifdef BINARY_SHOW
//    namedWindow("_binary",WINDOW_NORMAL);
    imshow("_binary",_binary);
#endif //BINARY_SHOW
}

/**
 *  函数名: isLight
 *  传入: Light src , vector<Point> cnt (灯条结构体,灯条的轮廓点)
 *  传出: bool isLight (布尔变量)
 *  功能: 通过条件判断传入灯条是否为灯条,返回布尔类型
 */
bool ArmorDetector::isLight(Light& light, vector<Point> &cnt)
{
    double height = light.height;
    double width = light.width;

    if(height <= 0 || width <= 0)
        return false;

    // 高一定要大于宽
    bool standing_ok = height > width;

    // 高宽比条件
    double hw_ratio = height / width;
    bool hw_ratio_ok = light_min_hw_ratio < hw_ratio && hw_ratio < light_max_hw_ratio;

    // 外接矩形面积和像素点面积之比条件
    double area_ratio =  contourArea(cnt) / (height * width);
    bool area_ratio_ok = light_min_area_ratio < area_ratio && area_ratio < light_max_area_ratio;

    // 灯条角度条件
    bool angle_ok = fabs(90.0 - light.angle) < light_max_angle;
    // cout<<"angle: "<<light.angle<<endl;

    // 限制面积条件
    bool area_ok = contourArea(cnt) < light_max_area;
    // 灯条判断的条件总集

    bool is_light = hw_ratio_ok && area_ratio_ok && angle_ok && standing_ok && area_ok;

    if(!is_light)
    {
//        cout<<hw_ratio<<"    "<<contourArea(cnt) / light_max_area<<"    "<<light.angle<<endl;
    }

    return is_light;
}

/**
 *  函数名: findLights
 *  传入: 无
 *  传出: 无
 *  功能: _binary寻找轮廓,遍历每个轮廓,传入isLight()判断是否为灯条
 *       是则判断颜色,将符合的灯条放入candidateLights容器
 */
void ArmorDetector::findLights()
{
    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;
    cv::findContours(_binary, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

#ifdef DRAW_LIGHTS_CONTOURS
    cv::Mat showSrc = _src.clone();
    for(int i=0;i< contours.size();i++)
    cv::drawContours(showSrc,contours,i,Scalar(255,0,0),2,LINE_8);
//    namedWindow("showSrc",WINDOW_NORMAL);
    imshow("showSrc",showSrc);
#endif

    if (contours.size() < 2)
    {
//        printf("no 2 contours\n");
        return;
    }

    for (auto & contour : contours)
    {
        RotatedRect r_rect = minAreaRect(contour);
        Light light = Light(r_rect);

        if (isLight(light, contour))
        {
            cv::Rect rect = r_rect.boundingRect();

            if (0 <= rect.x && 0 <= rect.width  && rect.x + rect.width  <= _src.cols &&
                0 <= rect.y && 0 <= rect.height && rect.y + rect.height <= _src.rows)
            {

                cv::Mat roi = _src(rect);
                cv::Mat mask = _binary(rect);

                Scalar sum = cv::mean(roi,mask);

                light.lightColor = sum[2] > sum[0] ? RED : BLUE;

                // 颜色不符合电控发的就不放入 1:RED 2:BLUE
                if(light.lightColor == 2)
                {
                    candidateLights.emplace_back(light);
#ifdef DRAW_LIGHTS_RRT
                    cv::Mat showSrc = _src.clone();
                    Point2f vertice_lights[4];
                    light.points(vertice_lights);
                    for (int i = 0; i < 4; i++) {
                        line(showSrc, vertice_lights[i], vertice_lights[(i + 1) % 4], CV_RGB(255, 0, 0),2,LINE_8);
                    }
                    //circle(showSrc,light.center,5,Scalar(0,0,0),-1);
//                    namedWindow("showSrc",WINDOW_NORMAL);
                    imshow("showSrc", showSrc);
#endif //DRAW_LIGHTS_RRT
                }
            }
        }
    }
}

/**
 *  函数名: matchLights
 *  传入: 无
 *  传出: 无
 *  功能: 将灯条两两匹配,条件判断和传入conTain()判断是否为装甲板
 *       将符合的装甲板放入candidateArmors容器
 */
void ArmorDetector::matchLights()
{
    if(candidateLights.size() < 2)
    {
//        printf("no 2 lights\n");
        return;
    }

    // 将旋转矩形从左到右排序
    sort(candidateLights.begin(), candidateLights.end(),
         [](RotatedRect& a1, RotatedRect& a2) {
             return a1.center.x < a2.center.x; });

    // 双重循环将灯条每对匹配
    for (size_t i = 0; i < candidateLights.size() - 1; i++)
    {
        Light lightI = candidateLights[i];
        Point2f centerI = lightI.center;    // 灯条1中心点

        for (size_t j = i + 1; j < candidateLights.size(); j++)
        {
            Light lightJ = candidateLights[j];
            Point2f centerJ = lightJ.center;     // 灯条2中心点
            double armorWidth = POINT_DIST(centerI,centerJ) - (lightI.width + lightJ.width)/2.0;                    // 计算装甲板宽度
            double armorHeight = (lightI.height + lightJ.height) / 2.0;                                             // 计算装甲板高度
            double armor_ij_ratio = lightI.height / lightJ.height;                                                  // 计算两灯条高度比例
            double armorAngle = atan2((centerI.y - centerJ.y),fabs(centerI.x - centerJ.x))/CV_PI*180.0;    // 计算装甲板倾斜角度

            // 宽高比筛选条件
            bool small_wh_ratio_ok = armor_small_min_wh_ratio < armorWidth/armorHeight && armorWidth/armorHeight < armor_small_max_wh_ratio;
            bool big_wh_ratio_ok = armor_big_min_wh_ratio < armorWidth/armorHeight && armorWidth/armorHeight < armor_big_max_wh_ratio;
            bool wh_ratio_ok = small_wh_ratio_ok || big_wh_ratio_ok;

            // 左右灯条角度差筛选条件
            bool angle_offset_ok = fabs(lightI.angle - lightJ.angle) < armor_max_offset_angle;

            // 左右亮灯条中心点高度差筛选条件
            bool height_offset_ok = fabs(lightI.center.y - lightJ.center.y) / armorHeight < armor_height_offset;

            // 左右灯条的高度比
            bool ij_ratio_ok = armor_ij_min_ratio < armor_ij_ratio && armor_ij_ratio < armor_ij_max_ratio;

            // 候选装甲板角度筛选条件
            bool angle_ok = fabs(armorAngle) < armor_max_angle;

            // 条件集合
            bool is_like_Armor = wh_ratio_ok && angle_offset_ok && height_offset_ok && ij_ratio_ok && angle_ok;

            // 判断为装甲板
            if (is_like_Armor)
            {

                Point2f armorCenter = (centerI + centerJ) / 2.0;
                RotatedRect armor_rrect = RotatedRect(armorCenter,
                                                      Size2f(armorWidth,armorHeight),
                                                      -armorAngle);

                // 装甲板4点坐标 (从左下角开始逆时针)
                Point2f pt4[4] = { lightI.bottom, lightJ.bottom, lightJ.top, lightI.top };

                // conTain()判断两个灯条是否包含了其他灯条
                if (!conTain(armor_rrect,candidateLights,i,j))
                {
                    Armor armor(armor_rrect);

                    for(int index = 0; index < 4; index++)
                        armor.armor_pt4[index] = pt4[index];

                    // 判断大小装甲板
                    if(small_wh_ratio_ok)
                        armor.type = SMALL;
                    else
                        armor.type = BIG;

                    // 判断装甲板的 id
                    preImplement(armor);
                    // 将符合要求的装甲板放入 candidateArmors 容器
                    candidateArmors.emplace_back(armor);
#ifdef DRAW_ARMORS_RRT
                    //cout<<"LightI_angle :   "<<lightI.angle<<"   LightJ_angle :   "<<lightJ.angle<<"     "<<fabs(lightI.angle - lightJ.angle)<<endl;
                    //cout<<"armorAngle   :   "<<armorAngle * 180 / CV_PI <<endl;
                    //cout<<"    w/h      :   "<<armorWidth/armorHeight<<endl;
                    //cout<<"height-offset:   "<<fabs(lightI.height - lightJ.height) / armorHeight<<endl;
                    //cout<<" height-ratio:   "<<armor_ij_ratio<<endl;

                    Mat showSrc = _src.clone();
                    Point2f vertice_armors[4];
                    armor.points(vertice_armors);
                    for (int m = 0; m < 4; m++)
                    {
                        line(showSrc, vertice_armors[m], vertice_armors[(m + 1) % 4], CV_RGB(0, 255, 255),2,LINE_8);
                    }
                    //circle(showSrc,armorCenter,15,Scalar(0,255,255),-1);
//                    namedWindow("showSrc",WINDOW_NORMAL);
                    imshow("showSrc", showSrc);
                    putText(showSrc,to_string(armorAngle),armor.armor_pt4[3],FONT_HERSHEY_COMPLEX,1.0,Scalar(0,255,255),2,8);
#endif //DRAW_ARMORS_RRT
                }
            }

        }
    }
}

/**
 *  函数名: chooseTarget
 *  传入: 无
 *  传出: 无
 *  功能: 循环candidateArmors容器,推理numROIs容器内所有数字
 *       将推理结果,置信度,ID传入get_max(),更新装甲板的置信度和ID
 *       将符合要求的装甲板放入finalArmors容器
 */
void ArmorDetector::chooseTarget()
{

    if(candidateArmors.empty())
    {
        //cout<<"no target!!"<<endl;
        return;
    }
    else if(candidateArmors.size() == 1)
    {
//        cout<<"get 1 target!!"<<endl;

        // DNN推理
        Mat out_blobs = Dnn_Detect.net_forward(numROIs);
        float *outs = (float*)out_blobs.data;

        if (get_max(outs, candidateArmors[0].confidence, candidateArmors[0].id))
        {
#ifdef SHOW_NUMROI
            cv::Mat numDst;
            resize(numROIs[0],numDst,Size(200,300));
            string name = to_string(candidateArmors[0].id) + ":" + to_string(candidateArmors[0].confidence*100) + "%";
//            namedWindow("name",WINDOW_NORMAL);
            imshow("name",numDst);
//            printf("%d",armor.id);
//            std::cout<<"number:   "<<armor.id<<"   type:   "<<armor.type<<std::endl;
//            string file_name = "../data/"+std::to_string(0)+"_"+std::to_string(cnt_count)+".jpg";
//            cout<<file_name<<endl;
//            imwrite(file_name,numDst);
//            cnt_count++;
#endif
            // 一个装甲板分数直接打满
            candidateArmors[0].grade = 100;
            finalArmors.emplace_back(candidateArmors[0]);
        }
    }
    else
    {
        // DNN推理
        Mat out_blobs = Dnn_Detect.net_forward(numROIs);
        float *outs = (float*)out_blobs.data;            //每一行对应numROIs容器行数索引(numROIs[行数])的推理结果

        // 获取每个候选装甲板的id和type
        for(int i=0;i<candidateArmors.size();i++) {
            // 如果当前装甲板不符合要求,直接进入下一循环
            if (!get_max(outs, candidateArmors[i].confidence, candidateArmors[i].id))
            {
                outs+=categories;   // 将指针指向下一行
                continue;           // 直接进入下一次循环
            }
#ifdef SHOW_NUMROI
            cv::Mat numDst;
            resize(numROIs[i],numDst,Size(200,300));
            string name = to_string(candidateArmors[i].id) + ":" + to_string(candidateArmors[i].confidence*100) + "%";
            namedWindow("name",WINDOW_NORMAL);
            imshow("name",numDst);
//            printf("%d",armor.id);
//            std::cout<<"number:   "<<armor.id<<"   type:   "<<armor.type<<std::endl;
//            string file_name = "../data/"+std::to_string(0)+"_"+std::to_string(cnt_count)+".jpg";
//            cout<<file_name<<endl;
//            imwrite(file_name,numDst);
//            cnt_count++;
#endif
            /**
             * 目前的装甲板打分策略 (待改进)
             */
            // 装甲板中心点在屏幕中心部分，在中心部分中又是倾斜最小的，
            // 如何避免频繁切换目标：缩小矩形框就是跟踪到了，一旦陀螺则会目标丢失，
            // UI界面做数字选择，选几就是几号，可能在切换会麻烦，（不建议）

            //打分制筛选装甲板优先级(！！！！最后只保留了优先级条件2和4和id优先级，其他有些冗余！！！！)
            /*最高优先级数字识别英雄1号装甲板，其次3和4号（如果打分的话1给100，3和4给80大概这个比例）
             *1、宽高比（筛选正面和侧面装甲板，尽量打正面装甲板）
             *2、装甲板靠近图像中心
             *3、装甲板倾斜角度最小
             *4、装甲板高最大
             */
            //1、宽高比用一个标准值和当前值做比值（大于标准值默认置为1）乘上标准分数作为得分
            //2、在缩小roi内就给分，不在不给分（分数占比较低）
            //3、90度减去装甲板的角度除以90得到比值乘上标准分作为得分
            //4、在前三步打分之前对装甲板进行高由大到小排序，获取最大最小值然后归一化，用归一化的高度值乘上标准分作为得分

            // 为装甲板进行打分
            candidateArmors[i].grade = armorGrade(candidateArmors[i]);
            // 装甲板分数超过分数阈值放入finalArmors容器
            if (candidateArmors[i].grade > grade_standard)
            {
                finalArmors.emplace_back(candidateArmors[i]);
            }
            outs+=categories; // 将指针指向下一行
        }
    }


#ifdef DRAW_FINAL_ARMOR_S_CLASS
    Mat showSrc;
    _src.copyTo(showSrc);
    for(size_t i = 0; i < finalArmors.size(); i++)
    {
        // 绘制装甲板边框
        for (int j = 0; j < 4; j++)
        {
            line(showSrc, finalArmors[i].armor_pt4[j], finalArmors[i].armor_pt4[(j + 1) % 4], CV_RGB(255, 255, 0), 2);
        }

        // 装甲板分数
//        double Grade = finalArmors[i].grade;
//        putText(final_armors_src,Grade,finalArmors[i].center,FONT_HERSHEY_COMPLEX, 1.0, Scalar(12, 23, 200), 1, 8);

        // 装甲板ID和置信度
        string information = to_string(finalArmors[i].id) + ":" + to_string(finalArmors[i].confidence*100) + "%";
        putText(showSrc, information,finalArmors[i].armor_pt4[3],FONT_HERSHEY_COMPLEX,1,Scalar(255,0,255));
    }
//    namedWindow("showSrc",WINDOW_NORMAL);
    if(!finalArmors.empty())
        imshow("showSrc", showSrc);
#endif //DRAW_FINAL_ARMOR_S_CLASS

}

/**
 *  函数名: conTain
 *  传入: RotatedRect &match_rect,vector<Light> &Lights, size_t &i, size_t &j (匹配灯条形成矩形,灯条容器,当前灯条的索引)
 *  传出: bool (布尔类型)
 *  功能: 判断当前两个匹配灯条形成的矩形内是否包含了其他灯条
 */
bool ArmorDetector::conTain(RotatedRect &match_rect,vector<Light> &Lights, size_t &i, size_t &j)
{
    Rect matchRoi = match_rect.boundingRect();
    for (size_t k=i+1;k<j;k++)
    {
        // 灯条五等份位置的点
        if (matchRoi.contains(Lights[k].top)    ||
            matchRoi.contains(Lights[k].bottom) ||
            matchRoi.contains(Point2f(Lights[k].top.x+Lights[k].height*0.25 , Lights[k].top.y+Lights[k].height*0.25)) ||
            matchRoi.contains(Point2f(Lights[k].bottom.x-Lights[k].height*0.25 , Lights[k].bottom.y-Lights[k].height*0.25)) ||
            matchRoi.contains(Lights[k].center)  )
        {
            return true;
        }
        else
        {
            continue;
        }
    }
    return false;
}


/**
 *  函数名: preImplement
 *  传入: Armor& armor (装甲板结构体)
 *  传出: 无
 *  功能: 获取传入装甲板的数字ROI,放置在numROIs容器内
 */
void ArmorDetector::preImplement(Armor& armor)
{
    Mat numDst;
    Mat num;

    // Light length in image
    const int light_length = 14;     //大致为高的一半
    // Image size after warp
    const int warp_height = 30;
    const int small_armor_width = 32;//为48/3*2
    const int large_armor_width = 44;//约为70/3*2
    // Number ROI size
    const cv::Size roi_size(22, 30);

    const int top_light_y = (warp_height - light_length) / 2;
    const int bottom_light_y = top_light_y + light_length;
    //std::cout<<"type:"<<armor.type<<std::endl;
    // 大小装甲板的宽度不同
    const int warp_width = armor.type == SMALL ? small_armor_width : large_armor_width;

    cv::Point2f target_vertices[4] = {
            cv::Point(0, bottom_light_y),
            cv::Point(warp_width, bottom_light_y),
            cv::Point(warp_width, top_light_y),
            cv::Point(0, top_light_y),
    };
    const Mat& rotation_matrix = cv::getPerspectiveTransform(armor.armor_pt4, target_vertices);
    cv::warpPerspective(_src, numDst, rotation_matrix, cv::Size(warp_width, warp_height));

    // Get ROI
    numDst = numDst(cv::Rect(cv::Point((warp_width - roi_size.width) / 2, 0), roi_size));
    Dnn_Detect.img_processing(numDst, numROIs);
}

/**
 *  函数名: get_max
 *  传入: const float *data, float &confidence, int &id (推理结果,装甲板置信度,装甲板ID)
 *  传出: 无
 *  功能: 配合当前装甲板的数字推理结果,判断数字的置信度是否达到阈值,是否要击打当前ID
 *       符合要求返回 True
 */
bool ArmorDetector::get_max(const float *data, float &confidence, int &id)
{
    // 默认初始化
    confidence = data[0];
    id = 0;
    // 寻找最大置信度的ID
    for (int i=0;i<categories;i++)
    {
        if (data[i] > confidence)
        {
            confidence = data[i];
            id = i;
        }
    }
    // 如果id为0或者id为2(工程) | 置信度小于阈值
    if(id == 0 || id == 2 || confidence < thresh_confidence)
        return false;
    else
        return true;
}

/**
 *  函数名: armorGrade
 *  传入: Armor& checkArmor (装甲板结构体)
 *  传出: 无
 *  功能: 为装甲板打分,和击打策略有关 (待修改)
 */
int ArmorDetector::armorGrade(const Armor& checkArmor)
{
    // 看看裁判系统的通信机制，雷达的制作规范

    // 选择用int截断double

    /////////id优先级打分项目////////////////////////
    int id_grade;
    int check_id = checkArmor.id;
    id_grade = check_id == 1 ? 100 : 80;
    ////////end///////////////////////////////////

    /////////最大装甲板板打分项目/////////////////////
    // 最大装甲板，用面积，找一个标准值（固定距离（比如3/4米），装甲板大小（Armor.area）大约是多少，分大小装甲板）
    // 比标准大就是100，小就是做比例，，，，可能小的得出来的值会很小
    int height_grade;
    double hRotation = checkArmor.size.height / height_standard;
    if(candidateArmors.size()==1)  hRotation=1;
    height_grade = hRotation * 60;
    //////////end/////////////////////////////////

    ////////靠近图像中心打分项目//////////////////////
    // 靠近中心，与中心做距离，设定标准值，看图传和摄像头看到的画面的差异
    int near_grade;
    double pts_distance = POINT_DIST(checkArmor.center, Point2f(_src.cols * 0.5, _src.rows * 0.5));
    near_grade = pts_distance/near_standard < 1 ? 100 : (near_standard/pts_distance) * 100;
    ////////end//////////////////////////////////

    // 下面的系数得详细调节；
    int final_grade = id_grade * id_grade_ratio +
                      height_grade  * height_grade_ratio +
                      near_grade  * near_grade_ratio;

    return final_grade;
}

/**
 *  函数名: Detection
 *  传入: const cv::Mat &src (原始图像)
 *  传出: finalArmors  (最终装甲板)
 *  功能: 识别流程的入口函数,返回最终装甲板容器
 */
vector<Armor> ArmorDetector::Detection(const cv::Mat &src)
{
    //初始化清空所有容器
    if(!numROIs.empty())numROIs.clear();
    if(!finalArmors.empty())finalArmors.clear();
    if(!candidateArmors.empty())candidateArmors.clear();
    if(!candidateLights.empty())candidateLights.clear();

#ifdef SHOW_TIME
    auto start = std::chrono::high_resolution_clock::now();
    setImage(src);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = seconds_duration(end-start).count();
    printf("set_time:%lf\n",duration);
    start = std::chrono::high_resolution_clock::now();
    findLights();
    end = std::chrono::high_resolution_clock::now();
    duration = seconds_duration(end-start).count();
    printf("light_time:%lf\n",duration);
    start = std::chrono::high_resolution_clock::now();
    matchLights();
    end = std::chrono::high_resolution_clock::now();
    duration = seconds_duration(end-start).count();
    printf("match_time:%lf\n",duration);
    start = std::chrono::high_resolution_clock::now();
    chooseTarget();
    end = std::chrono::high_resolution_clock::now();
    duration = seconds_duration(end-start).count();
    printf("choose_time:%lf\n",duration);
#else
    setImage(src);
    findLights();
    matchLights();
    chooseTarget();

#endif
    //返回最终装甲板
    return finalArmors;
}

