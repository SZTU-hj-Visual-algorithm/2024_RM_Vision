//
// Created by steins_xin on 23-9-28.
//

//#define HIKCAMERA

#include <iostream>
#include <opencv2/opencv.hpp>
#include "Armor_detection.h"
#include "AngleSolve.h"
#include "HIK_Camera.h"
#include "robot_status.h"

int main(){
#ifdef HIKCAMERA
    cv::Mat src;
    HIK_Camera HIK;
    ArmorDetector Detector;
    std::vector<Armor> Targets;
    Eigen::Vector3d imu_pos;
    AngleSolve AS;
    //相机初始化
    if(HIK.HIKCamera_Init()){
        HIK.HIKCamera_SetParam();        //设置相机参数
        HIK.HIKCamera_startGrabbing();   //相机开始取流
        while (HIK.Camera_OK){
            HIK.HIKCamera_read();        //读取相机图像
            src = HIK.src.clone();       //复制图像
            HIK.Bayer.release();         //释放资源

            // ================识别部分==================
            Targets = Detector.Detection(src);                 //获取最终装甲板
            if(!Targets.empty() && Targets[0].id == 3) {
                imu_pos = AS.pixel2imu(Targets[0]);             //得到世界坐标
                std::string Distance = std::to_string(imu_pos[1]);
                cv::putText(src,Distance,cv::Point(50,100),5,5,cv::Scalar(0,0,255));
//            std::cout << "Distance: " << imu_pos[1] << "m" << std::endl;
            }
            cv::imshow("src",src);
            cv::waitKey(1);
        }
        HIK.HIKCamera_close();           //关闭相机
    }
#else
    using namespace std;
    using namespace cv;

    cv::Mat src;
    ArmorDetector Detector;
    std::vector<Armor> Targets;
    Eigen::Vector3d imu_pos;
    AngleSolve AS;

    //视频帧率为60
//    cv::VideoCapture video("/home/steins_xin/lmx_new/images/example_1.mp4");
    cv::VideoCapture video("/home/steins_xin/CLionProjects/RM_Detection/images/Video_2.avi");
    while (1) {
        video >> src;
        Targets = Detector.Detection(src);                          //获取最终装甲板
        AS._src = src.clone();

        bool OK = false;
        Armor Track_Armor;
        for (int i = 0; i < Targets.size(); ++i) {
            if(Targets[i].id == 3) {
                OK = true;
                Track_Armor = Targets[i];
                break;
            }
        }

        if(OK) {
            Track_Armor.world_position = AS.pixel2imu(Track_Armor);
            AS.AngleSolve_show(Track_Armor);
        }
        if(OK)cv::putText(src,"distance:"+ std::to_string(Track_Armor.world_position[1]),cv::Point(0,40),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::imshow("src",src);
        cv::waitKey(0);
    }

//    cv::Mat _src = cv::imread("/home/steins_xin/CLionProjects/RM_Detection/images/1_5.bmp");
//    Targets = Detector.Detection(_src);                          //获取最终装甲板
//    AS._src = _src;
//    imu_pos = AS.pixel2imu(Targets[0]);             //得到世界坐标
//    cv::imshow("_src",_src);
//    cv::waitKey(0);
#endif

}