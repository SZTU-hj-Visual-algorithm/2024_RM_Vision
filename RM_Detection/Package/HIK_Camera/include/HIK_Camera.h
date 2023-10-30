//
// Created by steins_xin on 23-9-16.
//

#ifndef MSV_SDK_HIK_CAMERA_H
#define MSV_SDK_HIK_CAMERA_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
//外部头文件
#include "robot_status.h"
#include "HIK_Camera_H/MvCameraControl.h"

class HIK_Camera {
public:
    cv::Mat Bayer;                              // 原始图片(Bayer格式)
    cv::Mat src;                                // 处理后图片(RGB格式)
    bool Camera_OK = true;                      // 相机运行状态

    //========================= 相机使用函数 =========================
    HIK_Camera();                               // 相机构造函数(读取相机参数)
    bool HIKCamera_Init();                      // 海康工业相机初始化
    bool HIKCamera_SetParam();                  // 设置海康工业相机参数
    bool HIKCamera_startGrabbing();             // 相机开启取流
    void HIKCamera_read();                      // 读取相机图像(多线程)
    bool HIKCamera_close();                     // 相机关闭

    //========================= 相机调试函数 =========================
    bool HIKCamera_outParam();                  // 输出相机参数
    bool HIKCamera_FPS();                       // 输出相机帧率

private:
    //========================= 初始化部分 =========================
    int nRet = MV_OK;                           // 初始化 nRet
    void* handle = NULL;                        // 初始化相机句柄
    unsigned int nIndex = 0;                    // 默认选择序列号为0的相机
    MV_CC_DEVICE_INFO_LIST stDeviceList = {0};  // 初始化枚举设备列表
    MV_FRAME_OUT stOutFrame = {0};              // 初始化输出图像缓存
    //========================= 相机参数部分 ========================
    float ExposureTime;                         //相机曝光时间
    float Gain;                                 //相机增益
};


#endif //MSV_SDK_HIK_CAMERA_H
