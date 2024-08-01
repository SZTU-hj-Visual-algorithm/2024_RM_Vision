//
// Created by steins_xin on 23-10-31.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "serial_main.h"
#include "ros/ros.h"
#include "ros/package.h"
// 外部头文件
#include "robot_status.h"
#include "robot_struct.h"
// msg
#include "robot_msgs/vision.h"
#include "std_msgs/Float32.h"

int main(int argc, char *argv[]){

    setlocale(LC_ALL,"");

    // 初始化 ROS 节点
    ros::init(argc,argv,"Vision_L_pub");

    // 创建 ROS 句柄
    ros::NodeHandle nh;

    // 创建发布者对象
    ros::Publisher pub = nh.advertise<robot_msgs::vision>("/Serial_Device/Vision_L_data",10);
    ros::Publisher main_yaw_pub = nh.advertise<std_msgs::Float32>("/main_yaw",10);

    // 创建发送数据
    robot_msgs::vision vision_t;
    std_msgs::Float32 yaw_t;


    bool Get_data;
    std::string Universal_Device = "/dev/robomaster";     // 通用设备
    std::string Board_L_Device = "/dev/Steins_Xin_L";     // 左板设备
    std::string Board_R_Device = "/dev/Steins_Xin_R";     // 右板设备
    SerialMain serial(Universal_Device);  // usb

    ROS_INFO("---Vision_L_pub Start---");
    while (ros::ok()){
        Get_data = serial.ReceiverMain();                                          // usb
        if(Get_data){

            // 填充数据
            vision_t.header.frame_id = "vision_L";
            vision_t.header.seq++;
            vision_t.header.stamp = ros::Time::now();
            vision_t.id = serial.vision_msg_.id;
            // vision_t.mode = serial.vision_msg_.mode;
            vision_t.pitch = serial.vision_msg_.pitch;
            vision_t.yaw = serial.vision_msg_.yaw;
            vision_t.roll = serial.vision_msg_.roll;
            // ROS_INFO("yaw: %f",vision_t.yaw);
            
            //! 注意:自定义数组resize设置大小,不然会出现数据错误的情况 
            vision_t.quaternion.resize(4);
            for (int i = 0; i < 4; i++)
            {
                vision_t.quaternion[i] = serial.vision_msg_.quaternion[i];
            }
            vision_t.shoot_spd = serial.vision_msg_.shoot;

            yaw_t.data = serial.vision_msg_.yaw;
            // 发送数据
            pub.publish(vision_t);
            main_yaw_pub.publish(yaw_t);

        }
        ros::spinOnce();
    }
}