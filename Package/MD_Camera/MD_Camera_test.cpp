//
// Created by steins_xin on 23-10-24.
//
//#define MODIFY_NAME

#include <iostream>
#include <opencv2/opencv.hpp>
#include "MD_Camera.h"
using namespace std;
using namespace cv;

Mat src;

int main(){
    Mat get_src;
    auto MD = new MD_Camera;
    printf("camera_open-\n");
#ifndef MODIFY_NAME
    // 初始化(0: 开启相机名字模式 1: 关闭相机名字模式)
    if (MD->init(Robot::MD_ON,"steins_xin")) {
        MD->MDCamera_outParam();            // 输出参数
        while ((waitKey(1) != 27)){
            MD->read_frame_rgb();       // 读取图片
            get_src = MD->src.clone();  // 复制图片
            get_src.copyTo(src);     // 复制图片
            MD->release_data();         // 释放缓存
            imshow("src",get_src);
        }
        MD->~MD_Camera();
    }
    else
    {
        printf("No camera!!\n");
    }
#else
    //==================相机重命名==================
    char *Name = "steins_xin";
//    MD->Modify_Camera_Name(Name);
//    MD->Camera_Name();
#endif //MODIFY_NAME

}