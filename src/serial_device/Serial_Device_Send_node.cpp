//
// Created by steins_xin on 23-10-31.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "serial_main.h"
#include "ros/ros.h"
// 外部头文件
#include "robot_status.h"
#include "robot_struct.h"
// msg
#include "robot_msgs/Vision.h"


bool Get_data;
SerialMain serial;  // usb
int main(int argc, char *argv[]){

    setlocale(LC_ALL,"");

    // 初始化 ROS 节点
    ros::init(argc,argv,"Vision_pub");

    // 创建 ROS 句柄
    ros::NodeHandle nh;

    // 创建发布者对象
    ros::Publisher pub = nh.advertise<robot_msgs::Vision>("Vision_data",10);

    // 创建发送数据
    robot_msgs::Vision vision_t;


    ROS_INFO("---Vision_pub Start---");
    while (ros::ok()){
        Get_data = serial.ReceiverMain();                                          // usb
        if(Get_data){

            // 填充数据
            vision_t.header.frame_id = "vision";
            vision_t.header.seq++;
            vision_t.header.stamp = ros::Time::now();
            vision_t.id = serial.vision_msg_.id;
            vision_t.mode = serial.vision_msg_.mode;
            vision_t.pitch = serial.vision_msg_.pitch;
            vision_t.yaw = serial.vision_msg_.yaw;
            vision_t.roll = serial.vision_msg_.roll;    

            //! 注意:自定义数组resize设置大小,不然会出现数据错误的情况 
            vision_t.quaternion.resize(4);
            for (int i = 0; i < 4; i++)
            {
                vision_t.quaternion[i] = serial.vision_msg_.quaternion[i];
            }
            vision_t.shoot = serial.vision_msg_.shoot;

            // Eigen::Matrix3d R_x;
            // // 四元数
            // float w=vision_t.quaternion[0],x=-vision_t.quaternion[1],y=-vision_t.quaternion[2],z=vision_t.quaternion[3];
            // R_x << 1-2*y*y-2*z*z, 2*x*y-2*z*w, 2*x*z+2*y*w,
            //         2*x*y+2*z*w, 1-2*x*x-2*z*z, 2*y*z-2*x*w,
            //         2*x*z-2*y*w, 2*y*z+2*w*x, 1-2*x*x-2*y*y;

            // float roll = atan2(2*y*z + 2*w*x,1 - 2*x*x - 2*y*y)/CV_PI * 180.0f;
            // float pitch = asin(2*w*y - 2*x*z)/CV_PI*180.0f;
            // float yaw = atan2(2*x*y + 2*w*z, 1 - 2*y*y - 2*z*z)/CV_PI*180.0f;
            // std::cout<<"----------[quaternion_euler]-----------"<<std::endl;
            // std::cout<<"[roll:]   |"<<roll<<std::endl;
            // std::cout<<"[pitch:]  |"<<pitch<<std::endl;
            // std::cout<<"[yaw:]    |"<<yaw<<std::endl;

            // 发送数据
            pub.publish(vision_t);
        }
        ros::spinOnce();
    }
}