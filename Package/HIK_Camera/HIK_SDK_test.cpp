//
// Created by steins_xin on 23-9-16.
//

#include "HIK_Camera.h"
#include "robot_status.h"

//  海康工业相机使用Demo,请使用多线程为相机单独使用一个线程
int main(){
    HIK_Camera HIK;
    //相机初始化
    if(HIK.HIKCamera_Init()){
        HIK.HIKCamera_SetParam(Robot::HIK_ON);        // 设置相机参数(自动白平衡) | ON:自动白平衡  OFF:手动白平衡
        HIK.HIKCamera_startGrabbing();              // 相机开始取流
        HIK.HIKCamera_outParam();                   // 相机输出参数
        HIK.HIKCamera_FPS();                        // 相机实际帧率
        while (HIK.Camera_OK){
            HIK.HIKCamera_read();           //读取相机图像
            cv::imshow("src", HIK.src);
            HIK.Bayer.release();            //释放资源
        }
        HIK.HIKCamera_close();           //关闭相机
    }
}