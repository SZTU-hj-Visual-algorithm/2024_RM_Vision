//
// Created by steins_xin on 23-10-31.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "serial_main.h"
// 外部头文件
#include "robot_status.h"
#include "robot_struct.h"


form _send_data;                        // 公共数据
int main(){
    int first_get;
    SerialMain serial;  // usb
    std::cout << first_get << std::endl;
    cv::Mat img = cv::Mat(640,640,CV_8UC3);
    cv::Mat src;


    chrono_time time_temp;
    time_temp = std::chrono::high_resolution_clock::now();
    _send_data = {0x21,first_get, //! 0x21应该换成serial.vision_msg_.mode赋值，方便调试默认给的0x21
                  {serial.vision_msg_.pitch,
                   serial.vision_msg_.yaw,
                   serial.vision_msg_.shoot},
                  {serial.vision_msg_.quaternion[0],
                   serial.vision_msg_.quaternion[1],
                   serial.vision_msg_.quaternion[2],
                   serial.vision_msg_.quaternion[3]},
                   std::vector<Armor>(),time_temp};
    //---------------------serial_main  data------------
    while (1){
        src = img.clone();
        first_get = serial.ReceiverMain();                                          // usb
        cv::putText(src,"mode:"+std::to_string(serial.vision_msg_.mode),cv::Point(0,40),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(src,"pitch:"+std::to_string(serial.vision_msg_.pitch),cv::Point(0,80),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(src,"yaw:"+std::to_string(serial.vision_msg_.yaw),cv::Point(0,120),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(src,"quat0:"+std::to_string(serial.vision_msg_.quaternion[0]),cv::Point(0,160),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(src,"quat1:"+std::to_string(serial.vision_msg_.quaternion[1]),cv::Point(0,200),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(src,"quat2:"+std::to_string(serial.vision_msg_.quaternion[2]),cv::Point(0,240),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::putText(src,"quat3:"+std::to_string(serial.vision_msg_.quaternion[3]),cv::Point(0,280),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        cv::imshow("src",src);
        cv::waitKey(1);
    }

}

//        std::cout << "-----serial_main  data------" << std::endl;
//        std::cout << "mode:"  << serial.vision_msg_.mode << std::endl;
//        std::cout << "pitch:" << serial.vision_msg_.pitch << std::endl;
//        std::cout << "yaw:"   << serial.vision_msg_.yaw << std::endl;
//        std::cout << "quat0:" << serial.vision_msg_.quaternion[0] << std::endl;
//        std::cout << "quat1:" << serial.vision_msg_.quaternion[1] << std::endl;
//        std::cout << "quat2:" << serial.vision_msg_.quaternion[2] << std::endl;
//        std::cout << "quat3:" << serial.vision_msg_.quaternion[3] << std::endl;