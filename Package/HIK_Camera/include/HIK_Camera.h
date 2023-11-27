//
// Created by steins_xin on 23-9-16.
//

#ifndef MSV_SDK_HIK_CAMERA_H
#define MSV_SDK_HIK_CAMERA_H

#include <opencv2/opencv.hpp>
//外部头文件
#include "HIK_Camera_H/MvCameraControl.h"
#include "robot_status.h"

class HIK_Camera {
public:
    cv::Mat Bayer;                              // 原始图片(Bayer格式)
    cv::Mat src;                                // 处理后图片(RGB格式)
    bool Camera_OK = true;                      // 相机运行状态

    //========================= 相机使用函数 =========================
    HIK_Camera();                               // 相机构造函数(读取相机参数)
    bool HIKCamera_Init();                      // 海康工业相机初始化
    bool HIKCamera_SetParam(int WhiteAuto);     // 设置海康工业相机参数(是否开启白平衡)
    bool HIKCamera_startGrabbing();             // 相机开启取流
    void HIKCamera_read();                      // 读取相机图像(多线程)(是否录像)
    void HIKCamera_conversion();                // 海康相机图像转换
    bool HIKCamera_close();                     // 相机关闭

    //========================= 相机调试函数 =========================
    bool HIKCamera_outParam();                  // 输出相机参数
    bool HIKCamera_FPS();                       // 输出相机帧率
    //========================= 相机录像函数 =========================
    bool HIKCamera_StartRecord();               // 相机启动录像
    bool HIKCamera_Record();                    // 录像数据读取
    bool HIKCamera_StopRecord();                // 相机关闭录像
private:
    //========================= 初始化部分 =========================
    int nRet = MV_OK;                           // 初始化 nRet
    void* handle = NULL;                        // 初始化相机句柄
    unsigned int nIndex = 0;                    // 默认选择序列号为0的相机
    MV_CC_DEVICE_INFO_LIST stDeviceList = {0};  // 初始化枚举设备列表
    MV_FRAME_OUT stOutFrame = {0};              // 初始化输出图像缓存
    //========================= 相机参数部分 ========================
    int BalanceRatio_value[3];                  // 白平衡参数 RGB
    float ExposureTime;                         // 相机曝光时间
    float Gain;                                 // 相机增益
    //========================= 录像参数部分 ========================
    MV_CC_RECORD_PARAM Record;                  // 录像结构体
    MV_CC_INPUT_FRAME_INFO RecordFrame;         // 录像数据
    char Video_path[100];                       // 视频保存路径
    float Video_FPS;                            // 视频帧率
    bool Video_Record = false;                  // 是否录像
};


#endif //MSV_SDK_HIK_CAMERA_H
