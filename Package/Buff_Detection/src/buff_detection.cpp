#include "buff_detection2.h"

// TODO:展示处理后的图像;

#define  SHOW_IMAGE

// TODO: 画出所有符叶的矩形

//#define DRAW_ALL_LEAF_RECT

// TODO: 画出目标符叶矩形

//#define DRAW_LEAF_RECT

// TODO: R的ROI

#define SHOW_R_IMAGE

// TODO: 大符的ROI

#define DRAW_BUFF_IMAGE

// TODO: 大符的点

//#define DRAW_LEAF_POINTS

// TODO: 符叶柄

//#define SHOW_HANDLE_IMAGE

// TODO: 虚拟大符

//#define virtual_buff

namespace robot_detection {

    buff_detection::buff_detection() {
        cv::FileStorage fs("D:/clion tect/Wind_Completon/other/buff_data.yaml", cv::FileStorage::READ);

        color = buff_RED;

        //通道分离参数
        br_blue_threshold = (int) fs["br_blue_threshold"];
        br_red_threshold = (int) fs["br_red_threshold"];

        rb_red_threshold = (int) fs["rb_red_threshold"];
        rb_blue_threshold = (int) fs["rb_blue_threshold"];

        more_threshold = (int) fs["more_threshold"];

        //矩形 轮廓筛选部分参数
        r_min_area = (int) fs["r_min_area"];
        r_max_area = (int) fs["r_max_area"];

        leaf_min_area = (int) fs["leaf_min_area"];
        leaf_max_area = (int) fs["leaf_max_area"];

        //R标信息参数
        r_max_hb_ratio = (float) fs ["r_max_hb_ratio"];
        r_min_hb_ratio = (float) fs ["r_min_hb_ratio"];

        r_max_hw_ratio = (float) fs ["r_max_hw_ratio"];
        r_min_hw_ratio = (float) fs ["r_min_hw_ratio"];

        R_classifier_confidence = (double ) fs["R_classifier_confidence"];

        r_max_are_ratio = (double ) fs["r_max_are_ratio"];
        r_min_are_ratio = (double ) fs["r_min_are_ratio"];



        //leaf信息筛选参数
        leaf_max_hw_ratio = (float) fs ["leaf_max_hw_ratio"];
        leaf_min_hw_ratio = (float) fs ["leaf_min_hw_ratio"];

        leaf_r_max_distance = (int) fs ["leaf_r_max_distance"];
        leaf_r_min_distance = (int) fs ["leaf_r_min_distance"];

        //判断旋转方向计分
        knowDirection_total_pass_score = (int) fs["knowDirection_total_pass_score"];

        //计数
        recording_buff_area_ratio= (int) fs["recording_buff_area"];

        //图像分类置信度评判
        leaf_classifier_confidence = (int) fs["leaf_classifier_confidence"];

        //视频路径
        Video_PATH = (std::string) fs["Video_PATH"];

        //R标图像匹配路径
        r_classifier_model_PATH = (std::string) fs["r_classifier_model_PATH"];

        //保存图片地址路径
        image_write_PATH = (std::string) fs["image_write_PATH"];

        //图像分类模型路径
        buff_classifier_model_PATH = (std::string) fs["buff_classifier_model_PATH"];

        //waitKey
        waitKey = (int) fs["waitKey"];

        fs.release();
    }

    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ////*
    ///*@brief  ：流程函数
    ///*@param  ：无
    ///*@return：无
    ///*@exception 无
    ///*@author  ：  Glb
    ///*/
    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////


    void buff_detection::Procedure() {

        cv::VideoCapture cap(Video_PATH);
        if (!cap.isOpened()) {
            std::cout << "Can not open the video" << std::endl;
            return;
        }


        while (cap.read(frame)) {
            if(find_first_BigBuff){  BigBuff_startTime = std::chrono::high_resolution_clock::now();}
            unsigned int startTime = cv::getTickCount();

            //处理图像
            SetImage(frame);

            //找轮廓
            findCandidateRect(binary);

            //处理矩形
             if(!SetRect_classification()){ std::printf("SetRect_classification----------error\n");};

             Get_final_buff();
            //处理大符
            final_buff();


            unsigned int elapsedTime = cv::getTickCount() - startTime;
            fps = cv::getTickFrequency() / elapsedTime;
#ifdef SHOW_IMAGE
//            show();
//            cv::imshow("frame", show_src);
//            cv::imshow("binary",binary);
#endif
            if (cv::waitKey(waitKey) == 27) {
                return;
            }


        }
        cap.release();
    }


    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ////*
    ///*@brief  ：数据可视化函数
    ///*@param  ：无
    ///*@return：无
    ///*@exception 无
    ///*@author  ：  Glb
    ///*/
    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    void buff_detection::show()
    {
        cv::putText(show_src,"speed : " +std::to_string(AS.bullet_speed),cv::Point2f(11,350),cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0),1,3);

        cv::putText(show_src,"pitch : "+std::to_string(AS.ab_pitch),cv::Point2f(11,400),cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0),1,3);

        cv::putText(show_src,"yaw   : "+std::to_string(AS.ab_yaw),cv::Point2f(11,450),cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 0),1,3);

        cv::putText(show_src,"roll  : "+std::to_string(AS.ab_roll),cv::Point2f(11,500),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,255,0),1,3);

        cv::putText(show_src,"distance: "+std::to_string(R.distance)+"m",cv::Point2f(11,550),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,255,0),1,3);

        cv::putText(show_src, "the last target : "+std::to_string(hit_num),cv::Point2f(11,300),cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(255,255,0), 1,3);

        cv::putText(show_src, "fps:" + std::to_string(fps), cv::Point(11, 170), cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(255,255,0), 1);

        cv::putText(show_src, "angle: " + std::to_string(angle), cv::Point(11, 120), cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(255,255,0), 1);

        if (know_direction){cv::putText(show_src, "Direction: 1 " , cv::Point(11, 80), cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(255,255,0), 1);}else cv::putText(show_src, "Direction: 0 " , cv::Point(11, 80), cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(255,255,0), 1);

        cv::putText(show_src, "confidence: "+std::to_string(confidence *100)+" %",cv::Point2f(11,220),cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(255,255,0), 1);

        cv::putText(show_src, "Id: "+std::to_string(classId),cv::Point2f(11,260),cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(255,255,0), 1);

        cv::line(show_src , cv::Point2f(show_src.cols/2,0),cv::Point2f(show_src.cols/2,show_src.rows),cv::Scalar(212,255,212),1);
        cv::line(show_src , cv::Point2f(0,show_src.rows/2),cv::Point2f(show_src.cols,show_src.rows/2),cv::Scalar(212,255,212),1);
//        cv::circle(show_src , cv::Point2f(show_src.cols/2,show_src.rows/2) ,5 ,cv::Scalar(0,255,0),-1);
    }



    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
     ////*
     ///*@brief  ：图像处理函数
     ///*@param  ：相机图像（input)
     ///*@return： binary 二值化  (output)
     ///*@exception ： 阈值问题 （蓝色通道图没试过）
     ///*@author  ：  Glb
     ///*/
     ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////


    void buff_detection::SetImage(const cv::Mat& frame_)
    {


        frame_.copyTo(show_src);
        //---------------------------------------通道分离处理速度更快！----------------
        if (color == buff_RED) {
            std::vector<cv::Mat> channels;
            cv::split(frame_, channels);
            cv::threshold(channels[2], channels[2], rb_red_threshold-50, 255, cv::THRESH_BINARY);
            cv::threshold(channels[0], channels[0], rb_blue_threshold, 255, cv::THRESH_BINARY);
            binary = channels[2] - channels[0];
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
    //      cv::erode(binary, binary, kernel);
//          cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
            cv::dilate(binary, binary, kernel);
            cv::medianBlur(binary, binary, 3);

        } else if (color == buff_BLUE) {
            std::vector<cv::Mat> channels;
            cv::split(frame_, channels);
            cv::threshold(channels[0], channels[0], br_blue_threshold, 255, cv::THRESH_BINARY);
            cv::threshold(channels[2], channels[2], br_red_threshold, 255, cv::THRESH_BINARY);
            binary = channels[0] - channels[2];
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
            cv::dilate(binary, binary, kernel);
        }
        else {
            cv::Mat gray;
            cv::cvtColor(frame_, gray, cv::COLOR_BGR2GRAY);
            cv::threshold(gray, binary, more_threshold, 255, cv::THRESH_BINARY);
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
            cv::dilate(binary, binary, kernel);
//          cv::medianBlur(binary, binary, 5);


        }
    }


///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ////*
    ///*@brief  ：找候选矩形
    ///*@param  ：images二值化图  &    candidateRect自定义结构体  储存旋转矩形+像素面积  ——> vector
    ///*@return： 候选矩形的容器
    ///*@exception ： 无
    ///*@author  ：  Glb
    ///*/
///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////


    void buff_detection::findCandidateRect(const cv::Mat &images) {
        //用前清空
        all_contours.clear();
        all_hierarchy.clear();

        findContours(images, all_contours, all_hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        if (all_contours.empty()) {
            std::printf("no contour\n");
            return;
        }
        for (int i = 0; i < all_contours.size(); i++) {

            cv::RotatedRect rect = cv::minAreaRect(all_contours[i]);
            float area = cv::contourArea(all_contours[i]);
            if (all_hierarchy[i][3] != -1) {continue;}
            candidateRect_buff.area=area;
            candidateRect_buff.rect=rect;
            candidateRect.emplace_back(candidateRect_buff);
#ifdef DRAW_ALL_LEAF_RECT
            cv::Point2f Vertex[4];
            rect.points(Vertex);
            for(int i = 0 ; i < 4 ; i++)
            {
                line(show_src , Vertex[i] , Vertex[(i + 1) % 4] , cv::Scalar(0,255,0) ,3);
            }
#endif
        }
    }
///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ////*
    ///*@brief  ：矩形筛选
    ///*@param  ：根据不同的轮廓面积进入不同的矩形筛选函数
    ///*@return：无
    ///*@exception ： 无
    ///*@author  ：  Glb
    ///*/
///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    bool buff_detection::SetRect_classification()
    {
        /*
         * 思路是筛子。从一个大容器里面筛选，面积符合要求的  分别储存到 各自的容器  再进行对应的二次筛选   TODO:记得清空
         */
        if(candidateRect.size()<1){return false;}
        for(size_t i =0;i<candidateRect.size();i++)
        {
             bool find_r = (candidateRect[i].area > r_min_area && candidateRect[i].area < r_max_area  );
            bool find_leaf = (candidateRect[i].area > leaf_min_area && candidateRect[i].area < leaf_max_area);
            if  (find_r)    {findR(candidateRect[i] );}
            if (find_leaf)  {findLeaf(candidateRect[i].rect);}
         }
        candidateRect.clear();
        return true;

    }

///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ////*
    ///*@brief  ：判断是否为R函数
    ///*@param  ：传入为旋转矩形R_rect  （根据 斜边底边比  长宽比  轮廓面积  筛选）
    ///*@return： 布尔值
    ///*@exception ： 是否有其他条件约束?
    ///*@author  ：  Glb
    ///*/
///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////

    bool buff_detection::isR(candidateRect_Buff &R_rect) {

        cv::Point2f Vertex[4];
        R_rect.rect.points(Vertex);

        //斜边和底边的比率   TODO: R标的物理筛选条件想不到别的了-----------> 斜边底边比  +  轮廓面积  +  长宽比   +（轮廓面积 / 物理面积？）
        float hypotenuse = POINT_DIST(Vertex[0],Vertex[2]);
        float base = POINT_DIST(Vertex[0],Vertex[3]);
        float hb_ratio = base / hypotenuse;
        float hw_ratio = R_rect.rect.size.height / R_rect.rect.size.width;
        float are_ratio = R_rect.area/(R_rect.rect.size.width*R_rect.rect.size.height);
        bool hb_ratio_ok = (hb_ratio > r_min_hb_ratio && hb_ratio < r_max_hb_ratio);
        bool hw_ratio_ok = (hw_ratio > r_min_hw_ratio && hw_ratio < r_max_hw_ratio);
        bool are_ratio_ok = ( are_ratio > r_min_are_ratio && are_ratio < r_max_are_ratio);
        if(hb_ratio_ok && hw_ratio_ok && are_ratio_ok){return true;}else {return false;}
    }


///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ////*
    ///*@brief  ：寻找R函数
    ///*@param  ：传入为旋转矩形 rect  判断函数 isr
    ///*@return： R的坐标
    ///*@exception ：  暂时没发现
    ///*@author  ：  Glb
    ///*/
///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////

    void buff_detection::findR(candidateRect_Buff &rect  ) {
        if (isR(rect)){

              cv::Rect  rrect = rect.rect.boundingRect();
              std::vector<cv::Point2f> Rect_points(4);
              Rect_points[1] = cv::Point2f (rrect.x +  rrect.height  , rrect.y  );
              Rect_points[3] = cv::Point2f (rrect.x   , rrect.y + rrect.width );
              Rect_points[2] = cv::Point2f (rrect.x     , rrect.y );
              Rect_points[0] = cv::Point2f (rrect.x +  rrect.height , rrect.y + rrect.width );
//            cv::circle(show_src,Rect_points[0],5,cv::Scalar(255,255,255),-1);
//            cv::circle(show_src,Rect_points[1],5,cv::Scalar(255,255,255),-1);
//            cv::circle(show_src,Rect_points[2],5,cv::Scalar(255,255,255),-1);
//            cv::circle(show_src,Rect_points[3],5,cv::Scalar(255,255,255),-1);
//            cv::circle(show_src,cv::Point2f (rrect.x,rrect.y),5,cv::Scalar(255,255,255),-1);
//
//            cv::putText(show_src,"0",Rect_points[0],cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,255,0),1);
//            cv::putText(show_src,"1",Rect_points[1],cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,255,0),1);
//            cv::putText(show_src,"2",Rect_points[2],cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,255,0),1);
//            cv::putText(show_src,"3",Rect_points[3],cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,255,0),1);
//            cv::cvtColor(roi,roi,cv::COLOR_BGR2GRAY);


                // 将角点存储在一个 vector 中

                 std::vector<cv::Point2f> new_Points;
                new_Points.clear();
                new_Points.emplace_back(30 , 30);
                new_Points.emplace_back(30, 0);
                new_Points.emplace_back(0, 0);
                new_Points.emplace_back(0, 30);

                // 计算透视变换矩阵
                cv::Mat R_perspectiveMatrix = cv::getPerspectiveTransform(Rect_points, new_Points);
                // 应用透视变换
                cv::Mat R_outputImage;
                 cv::warpPerspective(show_src, R_outputImage, R_perspectiveMatrix, cv::Size(30, 30));
//                cv::cvtColor(R_outputImage,R_outputImage_hsv,cv::COLOR_BGR2HLS);
                cv::cvtColor(R_outputImage,R_outputImage,cv::COLOR_BGR2GRAY);

                if(R_classifier(R_outputImage))
                {
//                     ImWrite_frame(1,R_outputImage);
                    R_point = rect.rect.center;
//                    std::cout<<POINT_DIST(R_point,cv::Point2f(show_src.cols/2,show_src.rows/2))<<std::endl;
                    if(!findR_ok){last_buff_target_center = rect.rect.center;    findR_ok = true;}else {findR_ok = false;}
                    cv::Point2f Vertex[4];
                    rect.rect.points(Vertex);
                    for (int i = 0; i < 4; i++)
                    {
                        line(show_src, Vertex[i], Vertex[(i + 1) % 4], cv::Scalar(0, 0, 255), 3);
                    }
                }
#ifdef SHOW_R_IMAGE

                cv::imshow("r",R_outputImage);

#endif
            } else return;

    }

///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ////*
    ///*@brief  ：判断是否为R标
    ///*@param  ：cv::Mat &src ,string &R_PATH
    ///*@return： 布尔值
    ///*@exception ：
    ///*@author  ：  Glb
    ///*/
///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    bool buff_detection::R_classifier(cv::Mat &src  ) {
        // 获取当前时间点
//        auto start = std::chrono::high_resolution_clock::now();
        double r_confidence=0;
        // 加载ONNX模型
        std::string modelPath = r_classifier_model_PATH;
        cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath);

        // 将图像转换为blob
        cv::Mat blob = cv::dnn::blobFromImage(src, 1.0, cv::Size(30, 30), cv::Scalar(), true, false);

        // 设置网络的输入
        net.setInput(blob);

        // 进行前向传播以获取输出
        cv::Mat prob = net.forward();

        // 找到概率最高的类别
        cv::Point classIdPoint;
        minMaxLoc(prob.reshape(1, 1), 0, &r_confidence, 0, &classIdPoint);
        classId = classIdPoint.x;

//        // 获取当前时间点
//        auto end = std::chrono::high_resolution_clock::now();
//
//        // 计算时间差（以毫秒为单位）
//        auto duration = std::chrono::duration<float, std::milli>(end - start);

        if (classId == 1 && r_confidence * 100 > leaf_classifier_confidence) {
            blob.release();
            prob.release();
            return true;
        }
        else {
            blob.release();
            prob.release();
            return false;
        }
    }
///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ////*
    ///*@brief  ：判断是否为符叶
    ///*@param  ：传入为旋转矩形 rect
    ///*@return： 布尔值
    ///*@exception ： 符叶相似性
    ///*@author  ：  Glb
    ///*/
///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    bool buff_detection::isLeaf(cv::RotatedRect &leaf) const {


        //------------------------------------------------方案二：旋转矩形-------------------------------
        //TODO 定义宽为最长的！ 高为最短的
        float hw_ratio;
        if (leaf.size.width > leaf.size.height) { hw_ratio = leaf.size.height / leaf.size.width; }
        else {
            hw_ratio = leaf.size.width / leaf.size.height;
        }
        bool hw_ok = (hw_ratio > leaf_min_hw_ratio && hw_ratio < leaf_max_hw_ratio);

        float distance = POINT_DIST(leaf.center,  R_point);
        bool dis_ok = (distance > leaf_r_min_distance && distance < leaf_r_max_distance);
        if (hw_ok && dis_ok) { return true; } else { return false; }
    }

    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
        ////*
        /// *@brief  ：找符叶函数
        /// *@param  ：传入为旋转矩形 leaf
        /// *@return： 目标符叶各种信息
        /// *@exception ： 符叶相似性 + 图像分类稳定性
        /// *@author  ：  Glb
        /// */
    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////

    void buff_detection::findLeaf(cv::RotatedRect &leaf) {

//--------------------固定旋转矩形的四点坐标（左上角为0 顺时针增大）--------------------------------------
        if (isLeaf(leaf)) {
            angle = atan2((leaf.center.y - R_point.y), (leaf.center.x - R_point.x));
            angle = -angle / CV_PI * 180;
            if (angle < 0) {
                angle += 360;
            }
             //根据上下帧的角度差值 判断顺逆时针
            float angle_ratio = angle - last_angle;
            if ( angle_ratio < 1 && angle_ratio > -1 && angle_ratio !=0  &&  know_direction_ONOFF )KnowDirection(angle_ratio);
            last_angle = angle;  //TODO:记得重新赋值


            cv::Point2f leaf_target[4];
            cv::Point2f Vertex[4];
            cv::Point2f leaf_bottom;
            cv::Point2f leaf_top;
            leaf.points(Vertex);



            ////////可以继续优化一下算法，降低复杂度
            if (angle >= 0 && angle < 91  ||angle >= 353 && angle <= 360)
            {
                leaf_target[0] = Vertex[0];
                leaf_target[1] = Vertex[1];
                leaf_target[2] = Vertex[2];
                leaf_target[3] = Vertex[3];

            } else if (angle >= 91 && angle < 181)
            {
                leaf_target[0] = Vertex[3];
                leaf_target[1] = Vertex[0];
                leaf_target[2] = Vertex[1];
                leaf_target[3] = Vertex[2];

            } else if (angle >= 181 && angle < 266)
            {
                leaf_target[0] = Vertex[2];
                leaf_target[1] = Vertex[3];
                leaf_target[2] = Vertex[0];
                leaf_target[3] = Vertex[1];

            }else
            {
                leaf_target[0] = Vertex[1];
                leaf_target[1] = Vertex[2];
                leaf_target[2] = Vertex[3];
                leaf_target[3] = Vertex[0];
            }

            if (POINT_DIST(leaf_target[3], R_point) > 200)
            {
                std::swap(leaf_target[1], leaf_target[3]);

            }
            if (POINT_DIST(leaf_target[0], R_point) > 200)
            {
                std::swap(leaf_target[2], leaf_target[0]);
            }

            leaf_top = ((leaf_target[0] + leaf_target[1]) / 2);
            leaf_bottom = ((leaf_target[3] + leaf_target[2]) / 2);

            std::vector<cv::Point2f> points(4);
            points[0] = leaf_target[1];
            points[1] = leaf_top;
            points[2] = leaf_bottom;
            points[3] = leaf_target[2];


            // 定义输出点（希望将输入的四边形区域映射到的矩形区域）
            std::vector<cv::Point2f> outputPoints;
            outputPoints.clear();
            outputPoints.emplace_back(0, 0);
            outputPoints.emplace_back(0, 30);
            outputPoints.emplace_back(30 , 30);
            outputPoints.emplace_back(30, 0);
            // 计算透视变换矩阵
            cv::Mat perspectiveMatrix = cv::getPerspectiveTransform(points, outputPoints);
            // 应用透视变换
            cv::warpPerspective(show_src, outputImage, perspectiveMatrix, cv::Size(30, 30));
            cv::cvtColor(outputImage,outputImage,cv::COLOR_BGR2GRAY);


            if(classifier(outputImage))
            {
                 first_buff_target.clear();
                buff = cv::minAreaRect(points);
                buff_target_.rect=buff;
                buff_target_.angle=angle;
                buff_target_.center=buff.center;
                buff_target_.AS_input[0]=leaf_top;
                buff_target_.AS_input[1]=leaf_bottom;
                buff_target_.AS_input[2]=leaf_target[1];
                buff_target_.AS_input[3]=leaf_target[2];
                first_buff_target.emplace_back(buff_target_);

                #ifdef DRAW_BUFF_IMAGE
                                cv::imshow("output",outputImage);
                #endif

                #ifdef DRAW_LEAF_RECT
                            cv::circle(show_src, leaf_bottom, 3, cv::Scalar(245, 26, 188), 2);
                            cv::circle(show_src, leaf_top, 3, cv::Scalar(245, 26, 188), 2);
                            line(show_src, leaf_target[1], leaf_top, cv::Scalar(214, 112, 218), 2);
                            line(show_src, leaf_target[1], leaf_target[2], cv::Scalar(214, 112, 218), 2);
                            line(show_src, leaf_top, leaf_bottom, cv::Scalar(214, 112, 218), 2);
                            line(show_src, leaf_target[1], leaf_bottom, cv::Scalar(214, 112, 218), 2);
                            line(show_src, leaf_target[2], leaf_bottom, cv::Scalar(214, 112, 218), 2);
                            line(show_src, leaf_target[2], leaf_top, cv::Scalar(214, 112, 218), 2);
                #endif

                #ifdef SHOW_HANDLE_IMAGE
                                //----------------------------------------------------------加一个条件判断(截取它的柄)------------------------------------------------
                            // 定义输出点（希望将输入的四边形区域映射到的矩形区域）
                            std::vector<cv::Point2f> handle_points(4);
                            for (int i = 0; i < 4; ++i) {
                                handle_points[0] = leaf_target[0];
                                handle_points[1] = leaf_top;
                                handle_points[2] = leaf_bottom;
                                handle_points[3] = leaf_target[3];
                            }

                            cv::RotatedRect _buffTarget = cv::minAreaRect(handle_points);
                            true_buff = _buffTarget;
                            buff_target.emplace_back(_buffTarget);
                //          Second_detection(buff_target, handle_points);
                            buff_target.clear();



                            buffNo.angle=angle;
                            buffNo.Diff_angle =   buffNo.last_angle - buffNo.angle;
                            buffNo.last_angle=angle;
                            std::vector<cv::Point2f> new_Points2;
                            new_Points.clear();
                            new_Points.emplace_back(0, 0);
                            new_Points.emplace_back(0, 200);
                            new_Points.emplace_back(200 , 200);
                            new_Points.emplace_back(200, 0);
                            // 计算透视变换矩阵
                            cv::Mat handle_perspectiveMatrix = cv::getPerspectiveTransform(handle_points, new_Points2);
                            // 应用透视变换
                            cv::Mat handle_outputImage;
                            cv::warpPerspective(show_src, handle_outputImage, handle_perspectiveMatrix, cv::Size(200, 200));
                            cv::imshow("handle_outputImage",handle_outputImage);
                #endif

                #ifdef DRAW_LEAF_POINTS

                                for(int i = 0 ; i < 4 ; i++)
                            {
                                line(show_src , leaf_target[i] , leaf_target[(i + 1) % 4] , cv::Scalar(0,255,0) ,3);
                                putText(show_src, "0",leaf_target[0],cv::FONT_HERSHEY_SIMPLEX,1.0,cv::Scalar(255,28,166),2);
                                putText(show_src, "1",leaf_target[1],cv::FONT_HERSHEY_SIMPLEX,1.0,cv::Scalar(255,28,166),2);
                                putText(show_src, "2",leaf_target[2],cv::FONT_HERSHEY_SIMPLEX,1.0,cv::Scalar(255,28,166),2);
                                putText(show_src, "3",leaf_target[3],cv::FONT_HERSHEY_SIMPLEX,1.0,cv::Scalar(255,28,166),2);
                            }
                #endif

            }else{return;}
        }
    }

    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ////*
    /// *@brief  ：获得最终大符
    /// *@param  ：
    /// *@return：
    /// *@exception ：
    /// *@author  ：  Glb
    /// */
///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////

    void buff_detection::Get_final_buff()
        {
             if(first_buff_target.size()>1){
                final_buff_target.clear();
                //如何目标 不止一个 对其进行判断：通过图形分类后 一般会出现两个目标的情况   -------》大多是 大符本身响应问题
                //判断的依据  ：判断上一帧目标 与 下一帧两个大符目标坐标距离比大小  。大的 说明是新的   TODO:记得赋值dis与储存目标点
                if(POINT_DIST(last_buff_target_center, first_buff_target[0].center)<=POINT_DIST(last_buff_target_center, first_buff_target[1].center))
                {
                    dis = POINT_DIST(last_buff_target_center, first_buff_target[0].center);
                    buff_target_points.emplace_back(first_buff_target[0].center);
                    last_buff_target_center = first_buff_target[0].center;
                    final_buff_target.emplace_back(first_buff_target[0]);
                    cv::circle(show_src, first_buff_target[0].center,10,cv::Scalar(0,255,0),-1);
                    first_buff_target.clear();
                }
                else
                {
                    dis = POINT_DIST(last_buff_target_center, first_buff_target[1].center);
                    buff_target_points.emplace_back(first_buff_target[1].center);
                    last_buff_target_center = first_buff_target[1].center;
                    final_buff_target.emplace_back(first_buff_target[1]);
                    cv::circle(show_src, first_buff_target[1].center,10,cv::Scalar(0,255,0),-1);
                    first_buff_target.clear();
                }
            }else if(first_buff_target.size() ==1)
            {

                final_buff_target.emplace_back(first_buff_target[0]);
                first_buff_target.clear();
            }
            else
            {
                  return;
             }
            /*
            * emplace_back()通常比push_back()更高效，因为它避免了临时对象的创建和复制。由于emplace_back()允许直接传递构造参数，它可以在vector中直接构造元素，而不需要额外的拷贝或移动操作。
            */
            if(find_first_BigBuff && dis>0 && dis<15)
            {
                bigBuff_endTime = std::chrono::high_resolution_clock::now();
                // 计算时间间隔（以毫秒为单位）
                auto duration_buff = std::chrono::duration_cast<std::chrono::milliseconds>(bigBuff_endTime - BigBuff_startTime).count();
                if(duration_buff<100)time += duration_buff;

            }
            R_B_dis = POINT_DIST(R_point,buff.center);
            //对速度没啥影响
    //        if(buff_target_points.size()>50){fitCircleDirect(buff_target_points,R_point ,buff.center);}
            get_pre();
            GetNumber_Recording_buff(R_point);

        }
    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ////*
    ///*@brief  ：判断最终目标
    ///*@param  ：击打数 和 目标的点集
    ///*@return： 完整击打数 和 清空点集
    ///*@exception ： 清空和击打数不稳定
    ///*@author  ：  Glb
    ///*/
    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////


    bool buff_detection::final_buff()
    {
        if(number_id_0 == 1 && final_buff_target.size()>0){find_first_BigBuff = true;}
        //计数操作：对目标进行针对性处理计数问题
        hit_num = number_id_0 + number_id_1+ number_id_2 + number_id_3 + number_id_4 ;

        if(hit_num ==5)
        {
            KnowDirection_Total_score = 0;
            know_direction_ONOFF= true;
            loss_frame_count =0;

        }
          //对最终目标进行最后一次筛查。
         if(hit_num > 0 && final_buff_target.size()<1)
        {
            //进行数帧  丢失再识别到也得清零
            loss_frame_count ++;
             if(loss_frame_count >9 ){number_id_0 = 1;  buff_target_points.clear();}
            return -1;
        }
        else if(hit_num > 0 &&final_buff_target.size()>0)
        {
            loss_frame_count=0;
            //目标只有一种的情况
            buff_target_points.emplace_back(final_buff_target[0].center);
            dis = POINT_DIST(last_buff_target_center, final_buff_target[0].center);
            last_buff_target_center = final_buff_target[0].center;
            number_id_0 = 0;
        }
         else {final_buff_target.clear();return -1;}
        final_buff_target.clear();
        return true;
    }

    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ///*
    ///*@brief  ：拟合圆
    ///*@param  ：传入点集
    ///*@return： 完整击打数 和 清空点集
    ///*@exception ： 清空和击打数不稳定
    ///*@author  ：  Glb
    ///*/
    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////


    // 最小二乘法拟合圆的函数
    bool buff_detection::fitCircleDirect(const std::vector<cv::Point2f>& points, cv::Point2f& center , cv::Point2f &buff_center) {
        float center_x ;
        float center_y ;
        if (points.size() < 50) {
            return false;
        }

        float sum_x = 0.0f, sum_y = 0.0f;
        float sum_x2 = 0.0f, sum_y2 = 0.0f;
        float sum_x3 = 0.0f, sum_y3 = 0.0f;
        float sum_xy = 0.0f, sum_x1y2 = 0.0f, sum_x2y1 = 0.0f;

        size_t N = points.size();
        for (int i = 0; i < N; i++) {
            float x = points[i].x;
            float y = points[i].y;
            float x2 = x * x;
            float y2 = y * y;
            sum_x += x;
            sum_y += y;
            sum_x2 += x2;
            sum_y2 += y2;
            sum_x3 += x2 * x;
            sum_y3 += y2 * y;
            sum_xy += x * y;
            sum_x1y2 += x * y2;
            sum_x2y1 += x2 * y;
        }
        float C, D, E, G, H;
        float a, b, c;
        float radius;
        C = N * sum_x2 - sum_x * sum_x;
        D = N * sum_xy - sum_x * sum_y;
        E = N * sum_x3 + N * sum_x1y2 - (sum_x2 + sum_y2) * sum_x;
        G = N * sum_y2 - sum_y * sum_y;
        H = N * sum_x2y1 + N * sum_y3 - (sum_x2 + sum_y2) * sum_y;


        //x² + y² + Dx + Ey + F = 0   其实 a = D  b=E   c=F   x = -D/2 + R * cos(θ)   y = -E/2 + R * sin(θ)  圆上一点坐标
        a = (H * D - E * G) / (C * G - D * D);
        b = (H * C - E * D) / (D * D - G * C);
        c = -(a * sum_x + b * sum_y + sum_x2 + sum_y2) / N;

        cv::Point2f R_fit;
        center_x = a / (-2);
        center_y = b / (-2);
        radius = sqrt(a * a + b * b - 4 * c) / 2;
        R_fit = cv::Point2f(  center_x/1.003 ,   center_y/1.018);
        cv::circle(show_src,R_fit,20,cv::Scalar(255,255,255),2);

        return true;
    }

    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ///*
    ///*@brief  ：获取预测点
    ///*@param  ：
    ///*@return：
    ///*@exception ：
    ///*@author  ：  Glb
    ///*/
    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////

    void buff_detection::get_pre()
    {
        cv::Point2f pre;
        if(BigBuff)
        {
            if(know_direction)
            {
                pre.x = R_B_dis * cos((-final_buff_target[0].angle + BP.BIG_buff_PredictionAngle( time/100 )) * CV_PI / 180);
                pre.y = R_B_dis * sin((-final_buff_target[0].angle + BP.BIG_buff_PredictionAngle( time /100 )) * CV_PI / 180);
            }
            else
            {
                pre.x = R_B_dis * cos((-final_buff_target[0].angle - BP.BIG_buff_PredictionAngle(time /100)) * CV_PI / 180);
                pre.y = R_B_dis * sin((-final_buff_target[0].angle - BP.BIG_buff_PredictionAngle(time /100)) * CV_PI / 180);
            }
        }else
        {
            if(know_direction)
            {
                pre.x = R_B_dis * cos((-final_buff_target[0].angle + BP.SMALL_buff_PredictionAngle()) * CV_PI / 180);
                pre.y = R_B_dis * sin((-final_buff_target[0].angle + BP.SMALL_buff_PredictionAngle()) * CV_PI / 180);
            }
            else
            {
                pre.x = R_B_dis * cos((-final_buff_target[0].angle - BP.SMALL_buff_PredictionAngle()) * CV_PI / 180);
                pre.y = R_B_dis * sin((-final_buff_target[0].angle - BP.SMALL_buff_PredictionAngle()) * CV_PI / 180);
            }
        }
        pre.x = pre.x + R_point.x;
        pre.y = pre.y + R_point.y;



        cv::circle(show_src,R_point,R_B_dis,cv::Scalar(255,255,255),1);
        cv::circle(show_src,pre,7,cv::Scalar(0,255,0),-1);
         cv::circle(show_src,final_buff_target[0].center,7,cv::Scalar(255,255,255),-1);
        cv::putText(show_src,"pre",pre,cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0),2,3);
        cv::putText(show_src,"center",final_buff_target[0].center,cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0),2,3);

    }

    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ///*
    ///*@brief  ：计数
    ///*@param  ：
    ///*@return：
    ///*@exception ：
    ///*@author  ：  Glb
    ///*/
    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////

    void buff_detection::GetNumber_Recording_buff(cv::Point2f& center)
    {
        float a, b;
        a=-2*center.x;
        b=-2*center.y;
        buff_id_0.x =  R_B_dis * cos((-final_buff_target[0].angle  ) * CV_PI / 180);
        buff_id_0.y =  R_B_dis * sin((-final_buff_target[0].angle  ) * CV_PI / 180);
        buff_id_0.x = buff_id_0.x - a/2;
        buff_id_0.y = buff_id_0.y - b/2;

        buff_id_1.x =  R_B_dis * cos((-final_buff_target[0].angle + 75) * CV_PI / 180);
        buff_id_1.y =  R_B_dis * sin((-final_buff_target[0].angle + 75) * CV_PI / 180);
        buff_id_1.x = buff_id_1.x - a/2;
        buff_id_1.y = buff_id_1.y - b/2;


        buff_id_2.x =  R_B_dis * cos((-final_buff_target[0].angle + 146) * CV_PI / 180);
        buff_id_2.y =  R_B_dis * sin((-final_buff_target[0].angle + 146) * CV_PI / 180);
        buff_id_2.x = buff_id_2.x - a/2;
        buff_id_2.y = buff_id_2.y - b/2;

        buff_id_3.x =  R_B_dis * cos((-final_buff_target[0].angle + 217) * CV_PI / 180);
        buff_id_3.y =  R_B_dis * sin((-final_buff_target[0].angle + 217) * CV_PI / 180);
        buff_id_3.x = buff_id_3.x - a/2;
        buff_id_3.y = buff_id_3.y - b/2;

        buff_id_4.x =  R_B_dis * cos((-final_buff_target[0].angle + 288) * CV_PI / 180);
        buff_id_4.y =  R_B_dis * sin((-final_buff_target[0].angle + 288) * CV_PI / 180);
        buff_id_4.x = buff_id_4.x - a/2;
        buff_id_4.y = buff_id_4.y - b/2;

         cv::Rect rect_id_0(buff_id_0.x - buff.size.width / (ratio*2), buff_id_0.y - buff.size.height / (ratio*2), buff.size.width /ratio, buff.size.height /ratio);
        cv::Rect rect_id_1(buff_id_1.x - buff.size.width / (ratio*2), buff_id_1.y - buff.size.height / (ratio*2), buff.size.width /ratio, buff.size.height /ratio);
        cv::Rect rect_id_2(buff_id_2.x - buff.size.width / (ratio*2), buff_id_2.y - buff.size.height / (ratio*2), buff.size.width /ratio, buff.size.height /ratio);
        cv::Rect rect_id_3(buff_id_3.x - buff.size.width / (ratio*2), buff_id_3.y - buff.size.height / (ratio*2), buff.size.width /ratio, buff.size.height /ratio);
        cv::Rect rect_id_4(buff_id_4.x - buff.size.width / (ratio*2), buff_id_4.y - buff.size.height / (ratio*2), buff.size.width /ratio, buff.size.height /ratio);

        rect_id_0 =rect_id_0 & cv::Rect(0, 0, binary.cols, binary.rows);
        rect_id_1 =rect_id_1 & cv::Rect(0, 0, binary.cols, binary.rows);
        rect_id_2 =rect_id_2 & cv::Rect(0, 0, binary.cols, binary.rows);
        rect_id_3 =rect_id_3 & cv::Rect(0, 0, binary.cols, binary.rows);
        rect_id_4 =rect_id_4 & cv::Rect(0, 0, binary.cols, binary.rows);


        if (Recording_buff(rect_id_1)) { number_id_1 = 1; } else { number_id_1 = 0; }
        if (Recording_buff(rect_id_2)) { number_id_2 = 1; } else { number_id_2 = 0; }
        if (Recording_buff(rect_id_3)) { number_id_3 = 1; } else { number_id_3 = 0; }
        if (Recording_buff(rect_id_4)) { number_id_4 = 1; } else { number_id_4 = 0; }

    #ifdef virtual_buff


            cv::circle(show_src,buff_id_1,10,cv::Scalar(0,255,0),3);
            cv::putText(show_src, "id_1",buff_id_1,cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(255, 255, 255), 4);

            cv::circle(show_src,buff_id_2,10,cv::Scalar(0,255,0),3);
            cv::putText(show_src, "id_2",buff_id_2,cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(255, 255, 255), 4);

            cv::circle(show_src,buff_id_3,10,cv::Scalar(0,255,0),3);
            cv::putText(show_src, "id_3",buff_id_3,cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(255, 255, 255), 4);

            cv::circle(show_src,buff_id_4,10,cv::Scalar(0,255,0),3);
            cv::putText(show_src, "id_4",buff_id_4,cv::FONT_HERSHEY_SIMPLEX, 1.0,cv::Scalar(255, 255, 255), 4);


             cv::rectangle(show_src, rect_id_0, cv::Scalar(0, 255, 125), 1);
            cv::rectangle(show_src, rect_id_1, cv::Scalar(0, 255, 125), 1);
            cv::rectangle(show_src, rect_id_2, cv::Scalar(0, 255, 125), 1);
            cv::rectangle(show_src, rect_id_3, cv::Scalar(0, 255, 125), 1);
            cv::rectangle(show_src, rect_id_4, cv::Scalar(0, 255, 125), 1);

    //        cv::Point2f preCenter;
    //        float preAngle;
    //        preAngle = CV_PI / 66 ;
    //        double x = buff_center.x - center.x;
    //        double y = buff_center.y - center.y;
    //        preCenter.x = x * cos(preAngle) + y * sin(preAngle) + center.x;
    //        preCenter.y = -x * sin(preAngle) + y * cos(preAngle) + center.y;
    //        cv::circle(show_src,preCenter,10,cv::Scalar(255,255,255),3);
    #endif
    }
    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ///*
    ///*@brief  ：分类器
    ///*@param  ：roi
    ///*@return：布尔值
    ///*@exception ：稳定性与速度
    ///*@author  ：  Glb
    ///*/
    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////

    bool buff_detection::classifier(cv::Mat &src)
    {
        // 获取当前时间点
        auto start = std::chrono::high_resolution_clock::now();

        // 加载ONNX模型
        std::string modelPath = buff_classifier_model_PATH;
        cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath);

        // 将图像转换为blob
        cv::Mat blob = cv::dnn::blobFromImage(src, 1.0, cv::Size(30, 30), cv::Scalar(), true, false);

        // 设置网络的输入
        net.setInput(blob);

        // 进行前向传播以获取输出
        cv::Mat prob = net.forward();

        // 找到概率最高的类别
        cv::Point classIdPoint;
        minMaxLoc(prob.reshape(1, 1), 0, &confidence, 0, &classIdPoint);
        classId = classIdPoint.x;

        // 获取当前时间点
        auto end = std::chrono::high_resolution_clock::now();

        // 计算时间差（以毫秒为单位）
        auto duration = std::chrono::duration<float, std::milli>(end - start);

        // 输出执行时间
        cv::putText(show_src,"forward_time:"+ std::to_string(duration.count())+"ms",cv::Point(11,30),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,255,0),1);

        if ( classId == 1 && confidence*100 > leaf_classifier_confidence){blob.release();prob.release();return true;}else{blob.release();prob.release();return false;}
    }


    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ////*
    ///*@brief  ：判断旋转方向
    ///*@param  ：角度
    ///*@return： 方向h
    ///*@exception ： 逆时针没试过预测
    ///*@author  ：  Glb
    ///*/
    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////

    //用来判断顺逆时针旋转
    bool buff_detection::KnowDirection( float angle)
    {
        //////////////////////////////方案二采用放大计分制度：达到120分即可 认为是顺时针   -120分为逆时针
        KnowDirection_Total_score += -angle * 100;
            if(KnowDirection_Total_score > knowDirection_total_pass_score )
          {
               know_direction = true;
              know_direction_ONOFF= false;

              return true;
          }
          else if (KnowDirection_Total_score < -knowDirection_total_pass_score)
          {
               know_direction = false;
              know_direction_ONOFF= false;
              return true;
          }
        return true;
    }


    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
    ////*
    ///*@brief  ：判断击打
    ///*@param  ：roi 找轮廓
    ///*@return： 击打数
    ///*@exception ： 复杂性高
    ///*@author  ：  Glb
    ///*/
    ///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////

    bool buff_detection::Recording_buff(const cv::Rect &mainRect)
    {
        cv::Mat roi;
        roi = binary(mainRect);
        roi_contours.clear();
        findContours(roi, roi_contours , cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        for (size_t i = 0; i < roi_contours.size(); i++)
        {
            float area = cv::contourArea(roi_contours[i] );
            float hw_are = final_buff_target[0].rect.size.width * final_buff_target[0].rect.size.height;
              if(  hw_are/area > recording_buff_area_ratio )
            {
                  return true;
            }
        }
         return false;
    }

    ///////////////////////////--------------------------工具函数集-------------------------------///////////////////////////
     bool buff_detection::printf(const float &data)
    {
        std::cout<<data<<std::endl;

        return true;
    }

    void buff_detection::ImWrite_frame(int index, cv::Mat &Write_image)
    {
        if (frameCount % index== 0) {

            // 构造文件名，包含完整的路径
            std::string filename =image_write_PATH + std::to_string(frameCount) + ".jpg";
          // 保存当前帧为图像文件
            cv::imwrite(filename, Write_image);
        }
          // 增加帧计数
            frameCount++;
    }

}

///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
////*
///*@brief  ：主函数
///*@param  ：
///*@return：
///*@exception ：
///*@author  ：  Glb
///*/
///////////////////////////--------------------- ///////////////////////////--------------------------///////////////////////////
int main() {
    robot_detection::buff_detection bd;
    bd.Procedure();
    return 0;
}
