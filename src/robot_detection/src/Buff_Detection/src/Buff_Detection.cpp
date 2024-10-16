#include "BuffDetection.h"

// TODO:展示处理后的图像;

// #define  SHOW_IMAGE

// TODO: 画出所有符叶的矩形

// #define DRAW_ALL_LEAF_RECT

// TODO: 画出目标符叶矩形

// #define DRAW_LEAF_RECT
 

 

 

BuffDetection::BuffDetection()
{
    std::string package_path = ros::package::getPath("robot_detection");
    cv::FileStorage fs(package_path + "/SetParam_File/Buff_data.yaml", cv::FileStorage::READ); 

    //通道分离参数
    br_blue_threshold = (int) fs["br_blue_threshold"];
    br_red_threshold = (int) fs["br_red_threshold"];

    rb_red_threshold = (int) fs["rb_red_threshold"];
    rb_blue_threshold = (int) fs["rb_blue_threshold"];

    //矩形 轮廓筛选部分参数
    r_min_area = (int) fs["r_min_area"];
    r_max_area = (int) fs["r_max_area"];

    leaf_min_area = (int) fs["leaf_min_area"];
    leaf_max_area = (int) fs["leaf_max_area"];

    //R标信息参数
    r_max_hb_ratio = (float) fs["r_max_hb_ratio"];
    r_min_hb_ratio = (float) fs["r_min_hb_ratio"];

    r_max_hw_ratio = (float) fs["r_max_hw_ratio"];
    r_min_hw_ratio = (float) fs["r_min_hw_ratio"];

    R_ReInited_flag = (int) fs["R_ReInited_flag"];

    r_max_are_ratio = (double) fs["r_max_are_ratio"];
    r_min_are_ratio = (double) fs["r_min_are_ratio"];

    //leaf信息筛选参数
    leaf_max_hw_ratio = (float) fs["leaf_max_hw_ratio"];
    leaf_min_hw_ratio = (float) fs["leaf_min_hw_ratio"];

    leaf_r_max_distance = (int) fs["leaf_r_max_distance"];
    leaf_r_min_distance = (int) fs["leaf_r_min_distance"];


    //图像分类置信度评判
    leaf_classifier_confidence = (int) fs["leaf_classifier_confidence"];

    //R标图像匹配路径
    RModelPath = (std::string) fs["RModelPath"];
 

    LeafModelPath = (std::string) fs["LeafModelPath"];
    HandleModelPath = (std::string) fs["HandleModelPath"];
 
 
 
    fs.release();
}



//TODO:原本想作为初始化使用，但是ROS也可以自动杀死程序并清空对应的内存就可以舍弃。
void BuffDetection::Detection_Init()
{
     
    //  First_R_Inited = true;  // R的第一次识别开关
    //  GET_FIRST_R_POS = false;  // 获取R正确第一次正确识别的R坐标
    //  RInit = false;
    //  R_ReInited = 0 ;

     
}




std::vector<BuffTarget> BuffDetection::Detection(cv::Mat &frame_) {

    /**
    * @brief 对图像初步增强、采用图像分割、膨胀和均值滤波 等
    * @param  时间消耗长_较慢
    * @return  返回二值化图像
    */



    SetImage(frame_);
 

    /**
     * @brief 寻找初始轮廓 并将符合条件的轮廓 进入R 与 leaf 的筛选中
     * @param candidateRect_Buff结构体是在robot_struct定义的_将识别信息传到预测类里面
     * @return  返回容器finalBuff
     */
    findCandidateRect(binary);

    

    /**
       * @brief 应对可能出现两个目标的解决方法
       * @param 判断对于的位置与R标的距离
       * @return  Nullptr
       */
    Get_final_buff();

    /**
     * @brief 可视化数据
     */
    show(frame_);


#ifdef SHOW_IMAGE
 
    cv::imshow("binary",binary);

#endif

    // 两个容器都进行了用前清空操作！
    //判断是否为空；
    
    
    return final_buff_target;

}
 
/**
* @brief 对图像初步增强、采用图像分割、膨胀和均值滤波 等
* @param  时间消耗长_较慢
* @return  返回二值化图像
*/

void BuffDetection::SetImage(const cv::Mat &frame_) {


//    std::cout<<rb_red_threshold<<std::endl;
    frame_.copyTo(src_);
    //---------------------------------------通道分离处理速度更快！----------------
    if (our_color ==RED) {
        std::vector<cv::Mat> channels;
        cv::split(frame_, channels);
        cv::threshold(channels[2], channels[2], rb_red_threshold, 255, cv::THRESH_BINARY);
        cv::threshold(channels[0], channels[0], rb_blue_threshold, 255, cv::THRESH_BINARY);
        binary = channels[2] - channels[0];
        // cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
        // cv::dilate(binary, binary, kernel);
        cv::medianBlur(binary, binary, 3);

    } else if (our_color == BLUE) {
        std::vector<cv::Mat> channels;
        cv::split(frame_, channels);
        cv::threshold(channels[0], channels[0], br_blue_threshold, 255, cv::THRESH_BINARY);
        cv::threshold(channels[2], channels[2], br_red_threshold, 255, cv::THRESH_BINARY);
        binary = channels[0] - channels[2];
        cv::medianBlur(binary, binary, 3);
        // cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
        // cv::dilate(binary, binary, kernel);
    }  
}

/**
* @brief 寻找初始轮廓 并将符合条件的轮廓 进入R 与 leaf 的筛选中_分流器
* @param candidateRect_Buff结构体是在robot_struct定义的_将识别信息传到预测类里面
* @return  返回容器finalBuff
*/



void BuffDetection::findCandidateRect(cv::Mat &images) {
    //用前清空 不用清空 每次都新的
    std::vector<std::vector<cv::Point>> all_contours;
    
//        std::vector<cv::Vec4i> all_hierarchy;

    //寻找轮廓
    findContours(images, all_contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    //轮廓为空就返回
    if (all_contours.empty()) {
        // std::printf("no contour\n");
        return;
    }
    first_buff_target.clear();
    //根据所有轮廓进行筛选
    for (auto &contour: all_contours) {

        
        cv::RotatedRect rect = cv::minAreaRect(contour);
        double area = cv::contourArea(contour);
        cv::Point2f Vertex[4];
        rect.points(Vertex);




//************************************************  R标 TODO：下面代码额外的功能开发仓促，没有实现简洁、不具有可观性************************************************ 
        //r标的初筛选
        float r_hw_ratio = rect.size.height / rect.size.width;
        bool r_hw_ratio_ok = (r_hw_ratio > (r_min_hw_ratio) && r_hw_ratio < (r_max_hw_ratio));
        bool is_r = (area > r_min_area && area < r_max_area);
 
   
        //判断总和
        /**
        1、首先操作手对准R标，会根据以准心为圆心、半径为50的圆范围内搜寻所需要的目标
        2、锁定好目标会开启用世界坐标转换图像坐标的方式获取R标信息
        3、由于世界坐标会发生偏移，所以要定时刷新校准
        
        **/
        
      //判断总和

     if(!RUpdate)  // 如果不需要刷新
      {
        // std::cout<<"World to Pixel\n";
        
        // FIRST_R_PIXEL= AS.imu2pixel(FIRST_R_WORLD_POS);  //重新覆盖第一次R的转换坐标
        R_Point = AS.imu2pixel(R_world_pos); //获取图像坐标
        // std::cout<<R_world_pos[0]<<" "<<R_world_pos[1]<<" "<<R_world_pos[2]<<std::endl;
        if(GET_FIRST_R_POS)  //第一次识别到 GET_FIRST_R_POS = True 。 然后进入累加程序（计时-->更新R标）
        {
            R_ReInited++;
        }
        if (R_ReInited > R_ReInited_flag ) //到时间点需要重新更新识别R标（不更新的话，R的三维坐标会漂移），更新R的三维坐标
        {
            R_ReInited = 0;
            RInit = false;
            RUpdate = true;
            printf("update\n");
        }
      }

      if (is_r && r_hw_ratio_ok && !RInit)  //RInit 就是需要再一次初始化  !RInit就是没有初始化。：1:程序刚开始没有对R初始化（即没有识别到R）需要识别才能算初始化。 2：更新的时候，R需要重新识别，重新初始化
      { 
        cv::Point2f image_center = cv::Point2f(images.cols * 0.5, images.rows * 0.5);  // 相机中心对准R标的
        
        if(RUpdate)    //  更新与 第一次识别不一样的。  更新是在上一次R标的位置附近筛选    第一次是在相机中心附近筛选目标R标  TODO：不懂可以看这个BUFF分支Readme文件
        {
            if(POINT_DIST(rect.center,FIRST_R_PIXEL) < 40) // 筛选最近的，并更新三维坐标
            {
                std::cout<<"Read to Update\n";
                // cv::circle(src_, rect.center, 20, cv::Scalar(255, 0, 255), 2);
                // cv::imshow("src_" , src_);
                R_Point = rect.center;
                R_world_pos = AS.pixel2imu_buff( Vertex, 5);
                // R_world_pos = FIRST_R_WORLD_POS;
                RUpdate = false ;
            }
        }
           
        if( POINT_DIST(image_center,rect.center) < 40 && First_R_Inited)  // 筛选最近的，并更新三维坐标
        {
                 printf("No update\n");
                 R_candidateRect_ = candidateRect_Buff(area, rect, Vertex);
                findR(R_candidateRect_);
                First_R_Inited = false;
        }
        
      }

    
        // double FIRST_R_TO_NOW_R_DIS = std::sqrt((FIRST_R_WORLD_POS[1]-R_world_pos[1])*(FIRST_R_WORLD_POS[1]-R_world_pos[1])+(FIRST_R_WORLD_POS[2]-R_world_pos[2])*(FIRST_R_WORLD_POS[2]-R_world_pos[2]));
        // std::cout<<FIRST_R_TO_NOW_R_DIS<<std::endl;
    //   std::cout<<R_world_pos[1]<<"   "<<R_world_pos[2]<<"   "<<R_world_pos[0]<<std::endl;
     

//************************************************  R标  ************************************************ 

//************************************************  大符  ************************************************ 
        

        //大符叶子初筛选
        bool is_leaf = (area > leaf_min_area && area < leaf_max_area);
        float leaf_hw_ratio;
        if (rect.size.width > rect.size.height) {
            leaf_hw_ratio = rect.size.height / rect.size.width;
        } else { leaf_hw_ratio = rect.size.width / rect.size.height; }
        bool lear_hw_ratio_ok = (leaf_hw_ratio > leaf_min_hw_ratio && leaf_hw_ratio < leaf_max_hw_ratio);


        if (is_leaf && lear_hw_ratio_ok) {
            leaf_candidateRect_ = candidateRect_Buff(area, rect, Vertex);
            findLeaf(leaf_candidateRect_);

        }
//************************************************  大符  ************************************************ 

#ifdef DRAW_ALL_LEAF_RECT   
        for (int i = 0; i < 4; i++) {
            cv::line( , Vertex[i], Vertex[(i + 1) % 4], cv::Scalar(0, 255, 0), 3);
        }
#endif
    }
}

/**
   * @brief 筛选信息判断是否为R
   * @param candidateRect_Buff_&R_rect
   * @return  返回布尔值
   */
bool BuffDetection::is_R(candidateRect_Buff &R_rect) const {

    // 加入颜色判断 应为R太小了 误识别率太高
    cv::Rect boundingRect = R_rect.rect.boundingRect();
    cv::Scalar sum;
    if (0 <= boundingRect.x && 0 <= boundingRect.width && boundingRect.x + boundingRect.width <= src_.cols &&
        0 <= boundingRect.y && 0 <= boundingRect.height && boundingRect.y + boundingRect.height <= src_.rows)
    {
        cv::Mat roi = src_(boundingRect);
        cv::Mat mask = binary(boundingRect);
        sum = cv::mean(roi, mask);
    }

    bool color_correct = false;

    if(our_color == RED)    // 1 红  2蓝   RGB   BGR
    {
        color_correct =  sum[0] < sum[2] ? true : false;
    }
    else if(our_color ==BLUE)
    {   
        color_correct =  sum[2] < sum[0]? true : false;
    }
    

    // 斜边和底边的比率   TODO: R标的物理筛选条件想不到别的了-----------> 斜边底边比  +  轮廓面积  +  长宽比   +（轮廓面积 / 物理面积？）
    float hypotenuse = POINT_DIST(R_rect.Vertex[0], R_rect.Vertex[2]);
    float base = POINT_DIST(R_rect.Vertex[0], R_rect.Vertex[3]);
    float hb_ratio = base / hypotenuse;
    bool hb_ratio_ok = (hb_ratio > (r_min_hb_ratio) && hb_ratio < (r_max_hb_ratio));

    // 轮廓面积比物理面积
    float are_ratio = R_rect.area / (R_rect.rect.size.width * R_rect.rect.size.height);
    bool are_ratio_ok = (are_ratio > (r_min_are_ratio) && are_ratio < (r_max_are_ratio));

    //总判断
    if (hb_ratio_ok && are_ratio_ok && color_correct) { return true; } else { return false; }
}


/**
   * @brief 根据is_R初步筛选后 再次通过分类器判断
   * @param candidateRect_Buff_&R_rect classification分类器
   * @return  返回R的坐标信息
   */

void BuffDetection::findR(candidateRect_Buff &R_rect) {

    /*
    * 第一次找R 初始化为 false  然后 改为ture 无需再次识别R
    */
    if (is_R(R_rect)) {

        /////将其变成边缘矩形 进入模型判断

        // cv::Rect boundingRect = R_rect.rect.boundingRect();
 
        // std::vector<cv::Point2f> Rect_points(4);   //限制容器容量
        // Rect_points[1] = cv::Point2f(boundingRect.x + boundingRect.height, boundingRect.y);
        // Rect_points[3] = cv::Point2f(boundingRect.x, boundingRect.y + boundingRect.width);
        // Rect_points[2] = cv::Point2f(boundingRect.x, boundingRect.y);
        // Rect_points[0] = cv::Point2f(boundingRect.x + boundingRect.height, boundingRect.y + boundingRect.width);

 
        
        // cv::Mat outputImage = warpAndConvertToGray(src_ , Rect_points);
        //分类器识别判断
        // if (classifier(outputImage , 2 , RModelPath)) 
        // { 
            //初始化改为true
            RInit = true;
            R_Point = R_rect.rect.center;
 
            // printf("-----awd-----");
            R_world_pos = AS.pixel2imu_buff(R_rect.Vertex, 5);
            if (!GET_FIRST_R_POS) 
            {
                FIRST_R_WORLD_POS = R_world_pos;
                GET_FIRST_R_POS = true;
            }

//            std::cout << R_world_pos[0] << "  " << R_world_pos[1] << "  " << R_world_pos[2] << std::endl;
//            double diff = sqrt(pow(R_world_pos[0], 2) + pow(R_world_pos[1], 2));

//
//            std::cout << "R_rect: " << R_rect.Vertex[0] << "  " << R_rect.Vertex[1] << "  " << R_rect.Vertex[2] << "  "
//                      << R_rect.Vertex[3] << std::endl;
//            cv::circle(show_src, R_pixel, 15, cv::Scalar(255, 0, 255), -1);
//
//            cv::putText(show_src, "R_world_pos_y: " + std::to_string(R_world_pos[1]), cv::Point2f(500, 300),
//                        cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 0), 1);
//            cv::putText(show_src, "R_world_pos_z: " + std::to_string(R_world_pos[2]), cv::Point2f(500, 340),
//                        cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 0), 1);
//            cv::putText(show_src, "R_pixel_x: " + std::to_string(R_pixel.x), cv::Point2f(500, 380),
//                        cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 0), 1);
//            cv::putText(show_src, "R_pixel_y: " + std::to_string(R_pixel.y), cv::Point2f(500, 420),
//                        cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 0), 1);
//
//            cv::putText(show_src, "diff: " + std::to_string(diff), cv::Point2f(500, 460), cv::FONT_HERSHEY_SIMPLEX, 1.0,
//                        cv::Scalar(255, 255, 0), 1);

        // }
 

    }
  
}

 


/**
   * @brief 筛选信息判断是否为Leaf
   * @param candidateRect_Buff_Leaf____classification分类器_____注意由于leaf时刻旋转需要固定角标信息
   * @return  返回Leaf的坐标||四点坐标 信息
   */

bool BuffDetection::isLeaf(candidateRect_Buff &Leaf) const {


    //------------------------------------------------方案二：旋转矩形-------------------------------
    //TODO 定义宽为最长的！ 高为最短的

    float distance = POINT_DIST(Leaf.rect.center, R_Point);
    bool dis_ok = (distance > leaf_r_min_distance && distance < leaf_r_max_distance);
    if (dis_ok) { return true; } else { return false; }
}


void BuffDetection::findLeaf(candidateRect_Buff &Leaf) {

//--------------------固定旋转矩形的四点坐标（左上角为0 顺时针增大） 具体顺序后期再转换成需要的--------------------------------------
    if (isLeaf(Leaf)) {

        // 获取Leaf 中心点的选择角度 360度模式
        leaf_.angle = getAngle(Leaf.rect.center, cv::Point2f(3000.0f, R_Point.y), R_Point, "360");
 
        leaf_.leaf_RotatedRect = Leaf.rect;

        //获取leaf的四个点坐标
        cv::Point2f leaf_target[4];
        cv::Point2f Vertex[4];

        //参考装甲板灯条信息 可以将leaf划分成中部的上下点
        cv::Point2f leaf_bottom;
        cv::Point2f leaf_top;

        for (size_t i = 0; i < 4; i++) {
            Vertex[i] = Leaf.Vertex[i];
        }

        ////////可以继续优化一下算法，降低复杂度  （为什么不是0-90 90-180 因为画面不是正方形。）
        if (leaf_.angle >= 0 && leaf_.angle < 91 || leaf_.angle >= 353 && leaf_.angle <= 360) {
            leaf_target[0] = Vertex[0];
            leaf_target[1] = Vertex[1];
            leaf_target[2] = Vertex[2];
            leaf_target[3] = Vertex[3];

        } else if (leaf_.angle >= 91 && leaf_.angle < 181) {
            leaf_target[0] = Vertex[3];
            leaf_target[1] = Vertex[0];
            leaf_target[2] = Vertex[1];
            leaf_target[3] = Vertex[2];

        } else if (leaf_.angle >= 181 && leaf_.angle < 266) {
            leaf_target[0] = Vertex[2];
            leaf_target[1] = Vertex[3];
            leaf_target[2] = Vertex[0];
            leaf_target[3] = Vertex[1];

        } else {
            leaf_target[0] = Vertex[1];
            leaf_target[1] = Vertex[2];
            leaf_target[2] = Vertex[3];
            leaf_target[3] = Vertex[0];
        }


        // 3 1 和 0 2 有时候经常互换 加入信息判断————强化固定
        if (POINT_DIST(leaf_target[3], R_Point) > POINT_DIST(leaf_target[1] , R_Point)) {
            std::swap(leaf_target[1], leaf_target[3]);

        }
        if (POINT_DIST(leaf_target[1], leaf_target[0]) < POINT_DIST(leaf_target[1], leaf_target[2])) {
            std::swap(leaf_target[2], leaf_target[0]);
        }

        leaf_top = ((leaf_target[0] + leaf_target[1]) / 2);
        leaf_bottom = ((leaf_target[3] + leaf_target[2]) / 2);


      #ifdef DRAW_LEAF_RECT
        cv::circle(src_, leaf_bottom, 3, cv::Scalar(245, 26, 188), 2);
        cv::circle(src_, leaf_top, 3, cv::Scalar(245, 26, 188), 2);

        for (int i = 0; i < 4; i++)
        {
            cv::line(src_, leaf_target[i], leaf_target[(i + 1) % 4], cv::Scalar(0, 255, 0), 3);
        }
        cv::circle(src_, leaf_bottom, 3, cv::Scalar(245, 26, 188), 2);
        cv::circle(src_, leaf_top, 3, cv::Scalar(245, 26, 188), 2);
        line(src_, leaf_target[1], leaf_top, cv::Scalar(214, 112, 218), 2);
        line(src_, leaf_target[1], leaf_target[2], cv::Scalar(214, 112, 218), 2);
        line(src_, leaf_top, leaf_bottom, cv::Scalar(214, 112, 218), 2);
        line(src_, leaf_target[1], leaf_bottom, cv::Scalar(214, 112, 218), 2);
        line(src_, leaf_target[2], leaf_bottom, cv::Scalar(214, 112, 218), 2);
        line(src_, leaf_target[2], leaf_top, cv::Scalar(214, 112, 218), 2);
        cv::putText(src_, "0", leaf_target[0], cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(214, 112, 218), 2);
        cv::putText(src_, "1", leaf_target[1], cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(214, 112, 218), 2);
        cv::putText(src_, "2", leaf_target[2], cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(214, 112, 218), 2);
        cv::putText(src_, "3", leaf_target[3], cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(214, 112, 218), 2);
        cv::imshow("src_", src_);

#endif
 
        //将四点储存到容器、方便后续的透视变换

        std::vector<cv::Point2f> points(4);
        points[0] = leaf_target[1];
        points[1] = leaf_top;
        points[2] = leaf_bottom;
        points[3] = leaf_target[2];

        cv::Point2f handle_top = 0.5*(leaf_target[0]+leaf_top);
        cv::Point2f handle_bottom = 0.5*(leaf_target[3]+leaf_bottom);


        //因为出现固定点又乱序的情况，直接筛掉；

        float height = POINT_DIST(leaf_top, leaf_bottom);
        float wight = POINT_DIST(leaf_bottom, leaf_target[2]);
        float hw_ok = height / wight;
        if (hw_ok > 1.5 || hw_ok < 0.7) return;


        cv::Mat outputImage = warpAndConvertToGray(src_ , points);

        //分类器
         if (classifier(outputImage , TrueLeafClass , LeafModelPath)) {
            /*
             * 将需要的信息打包进结构体，放入容器。
             */
            std::vector<cv::Point2f> val_points(4);
            val_points[0] = handle_top;
            val_points[1] = leaf_target[0];
            val_points[2] = leaf_target[3];
            val_points[3] = handle_bottom;
            cv::RotatedRect buff = cv::minAreaRect(points);
            cv::Rect buff_rect = buff.boundingRect();




            buff_target_.Val_points = val_points;
            buff_target_.boundingRect = buff_rect;
            buff_target_.r_center = R_Point;
            buff_target_.buff_rect = buff;
            buff_target_.buff_center = buff.center;
            buff_target_.AS_center_input[3] = leaf_top;
            buff_target_.AS_center_input[2] = leaf_bottom;
            buff_target_.AS_center_input[1] = leaf_target[2];
            buff_target_.AS_center_input[0] = leaf_target[1];
            buff_target_.target_angle = getAngle(Leaf.rect.center, cv::Point2f(3000.0f, R_Point.y), R_Point, "180");
            buff_target_.leaf_angle = leaf_.angle;
            //放入最初目标容器
            first_buff_target.emplace_back(buff_target_);


            //TODO:好像没有用到过。
            now_Point = buff.center;
            //判断是否切换了目标
                // 先判断有没有切换目标，如果没有切换则为 True
            if (POINT_DIST(now_Point, last_Point) > 100 && (fabs(now_Point.x - last_Point.x) > 60 || abs(now_Point.y - last_Point.y) > 60))
            {
                std::printf("Change!\n");
                sameLeaf = false;
            }
            else
            {
                sameLeaf = true;
            }
            last_Point =  now_Point;
  
        }
    }
}


/**
    * @brief 应对可能出现两个目标的解决方法
    * @param 判断对于的位置与R标的距离
    * @return  Nullptr
    */

void BuffDetection::Get_final_buff() {
    final_buff_target.clear();

    if (first_buff_target.size() > 1) {
         
                // 定义输出点（希望将输入的四边形区域映射到的矩形区域）
       
        for(auto x: first_buff_target)
        {
           
            cv::Mat outputImage = warpAndConvertToGray(src_ , x.Val_points);
            
           if(classifier(outputImage, TrueHandleClass , HandleModelPath))  // 符柄分类  Handle
            { 
                
                final_buff_target.emplace_back(x);
                
                break;
            }
 
        }
 
    } else if (first_buff_target.size() == 1) {

        final_buff_target=first_buff_target;
       
    } else {
        return;
    }

}


/**
   * @brief LeNetV5 根据图案判断是否是Target标
   * @param  耗时不高_与数字识别差不多
   * @return  返回布尔值
   */
bool BuffDetection::classifier(cv::Mat &src  , size_t id, std::string&ModePath) {
    double confidence;
    size_t classId;
    // 加载ONNX模型
 
    cv::dnn::Net net = cv::dnn::readNetFromONNX(ModePath);

    // 将图像转换为blob
    cv::Mat blob = cv::dnn::blobFromImage(src, 1.0, cv::Size(30, 30), cv::Scalar(), true, false);

    // 设置网络的输入
    net.setInput(blob);

    // 进行前向传播以获取输出
    cv::Mat prob = net.forward();

    // 找到概率最高的类别
    cv::Point classIdPoint;
    minMaxLoc(prob.reshape(1, 1), nullptr, &confidence, nullptr, &classIdPoint);

    classId = classIdPoint.x;

    if (classId == id && confidence * 100 > leaf_classifier_confidence) {
        blob.release();
        prob.release();
        return true;
    } else {

        blob.release();
        prob.release();
        return false;
    }
}

cv::Mat BuffDetection::warpAndConvertToGray(const cv::Mat& src, const std::vector<cv::Point2f>& Points)
{
// 定义输出点
    std::vector<cv::Point2f> outputPoints;
    outputPoints.emplace_back(0, 0);             // 左下角
    outputPoints.emplace_back(0, 30);            // 左上角
    outputPoints.emplace_back(30, 30);           // 右上角
    outputPoints.emplace_back(30, 0);            // 右下角

    // 计算透视变换矩阵
    cv::Mat perspectiveMatrix = cv::getPerspectiveTransform(Points, outputPoints);

    // 创建输出图像，与源图像具有相同的类型和大小
    cv::Mat outputImage;
    
    // 应用透视变换
    cv::warpPerspective(src, outputImage, perspectiveMatrix, cv::Size(30, 30));

    // 转灰度降低运算量
    cv::cvtColor(outputImage, outputImage, cv::COLOR_BGR2GRAY);

    return outputImage; 

}


/**
* @brief  可视化数据
* @return  Nullptr
*/
void BuffDetection::show(cv::Mat &show_src) {

    cv::putText(show_src, "angle: " + std::to_string(leaf_.angle), cv::Point(11, 120), cv::FONT_HERSHEY_SIMPLEX, 1.0,
                cv::Scalar(123, 255, 22), 1);
 
 
    cv::circle(show_src, R_Point, 20, cv::Scalar(255, 0, 255), 2);

    cv::circle(show_src, buff_target_.buff_center, 5, cv::Scalar(255, 255, 255), 3);

    cv::line(show_src, cv::Point2f(show_src.cols / 2, show_src.rows / 2 -50), cv::Point2f(show_src.cols / 2, show_src.rows/2 + 50),
             cv::Scalar(255, 255, 255), 1.5);

    cv::line(show_src, cv::Point2f(show_src.cols / 2 - 50, show_src.rows / 2), cv::Point2f(show_src.cols / 2 + 50 , show_src.rows / 2),
             cv::Scalar(255, 255, 255), 1.5);

    cv::circle(show_src ,cv::Point2f(show_src.cols * 0.5, show_src.rows * 0.5), 60 ,cv::Scalar(0, 198, 0) ,3);

    cv::circle(show_src ,FIRST_R_PIXEL, 20 ,cv::Scalar(123, 198, 11) ,2);
    cv::putText(show_src, "FIRST_R_PIXEL"  , FIRST_R_PIXEL, cv::FONT_HERSHEY_SIMPLEX, 1.0,
                cv::Scalar(123, 198, 11), 1);
}

 

///////////////////////////--------------------------工具函数-------------------------------///////////////////////////
/**
* @brief 计算角度函数
* @param  提供了两种模式_180_360的计算
* @return  根据模式返回对应的角度
*/

float BuffDetection::getAngle(cv::Point2f pt1, cv::Point2f pt2, cv::Point2f pt0, std::string mode) {
    float _circleAngle360, _circleAngle180;
    float dx1 = pt1.x - pt0.x;
    float dy1 = pt1.y - pt0.y;
    float dx2 = pt2.x - pt0.x;
    float dy2 = pt2.y - pt0.y;
    float angle_line = (dx1 * dx2 + dy1 * dy2) / sqrtf((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10f);
    _circleAngle180 = acosf(angle_line) * 180.0f / 3.141592653f;

    if (pt0.y < pt1.y) {
        _circleAngle360 = 360.0f - _circleAngle180;
        _circleAngle180 = -_circleAngle180;
    } else {
        _circleAngle360 = _circleAngle180;
    }
    if (mode == "360") { return _circleAngle360; }
    else { return _circleAngle180; }
}


/**
   * @brief 保存图片
   */

void BuffDetection::ImWrite_frame(cv::Mat &Write_image) {   //  帮助保存图片作为模型训练集
    if (frameCount % 1 == 0) {

        // 构造文件名，包含完整的路径
        std::string filename = "/home/hj/images/" + std::to_string(frameCount) + ".jpg";
        // 保存当前帧为图像文件
        cv::imwrite(filename, Write_image);
    }
    // 增加帧计数
    frameCount++;
}
