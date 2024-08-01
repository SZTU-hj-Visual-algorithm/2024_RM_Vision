//
// Created by steins_xin on 23-9-27.
//
#include <opencv2/opencv.hpp>
#include "Armor_detection.h"
#include "HIK_Camera.h"

//#define HIK_CAMERA

int main(){
#ifdef HIK_CAMERA
    cv::Mat src;
    HIK_Camera HIK;
    ArmorDetector Detector;
    //相机初始化
    if(HIK.HIKCamera_Init()){
        HIK.HIKCamera_SetParam();        //设置相机参数
        HIK.HIKCamera_startGrabbing();   //相机开始取流
        while (HIK.Camera_OK){
            HIK.HIKCamera_read();        //读取相机图像
            src = HIK.src.clone();       //复制图像
            HIK.Bayer.release();         //释放资源

            // ================识别部分==================
            cv::imshow("src",src);
            Detector.Detection(src);
            cv::waitKey(1);
        }
        HIK.HIKCamera_close();           //关闭相机
    }
#else
    cv::Mat src;
    ArmorDetector Detector;
    //视频帧率为60
    cv::VideoCapture video("/home/steins_xin/lmx_new/images/example_1.mp4");
    while (1) {
        video >> src;
        cv::imshow("src",src);
        Detector.Detection(src);
        cv::waitKey(1);
    }
#endif //HIK_CAMERA
}