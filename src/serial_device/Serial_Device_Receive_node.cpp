#include "ros/ros.h"
#include "serial_main.h"
// 外部头文件
#include "robot_status.h"
#include "robot_struct.h"
// msg
#include "robot_msgs/Robot_ctrl.h"

SerialMain serial; 
std::vector<double> vdata(4);

// 发送数据函数
void Robot_ctrl_Send(const robot_msgs::Robot_ctrl::ConstPtr& Robot_ctrl_t){
    // 向下位机发送数据
    double fire = (float)Robot_ctrl_t->fire_command;
    double mode = (float)Robot_ctrl_t->target_lock;
    vdata = {Robot_ctrl_t->pitch,Robot_ctrl_t->yaw,fire,mode};
    serial.SenderMain(vdata);   // 启动跟踪自瞄

    /* Test */
    // ROS_INFO("Pitch:%.2f Yaw: %.2f Fire:%.2f Mode: %.2f",
    // Robot_ctrl_t->pitch,Robot_ctrl_t->yaw,fire,mode);
}

int main(int argc, char *argv[]){

    setlocale(LC_ALL,"");

    // 初始化 ROS 节点
    ros::init(argc,argv,"Robot_ctrl_sub");

    // 创建 ROS 句柄
    ros::NodeHandle nh;

    // 创建订阅对象
    ros::Subscriber sub = nh.subscribe<robot_msgs::Robot_ctrl>("Robot_ctrl_data",1,Robot_ctrl_Send);   

    ROS_INFO("-- Robot_ctrl_sub Start--");

    ros::spin();  

    return 0;
}