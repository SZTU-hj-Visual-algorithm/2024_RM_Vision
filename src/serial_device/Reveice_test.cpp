#include "ros/ros.h"
// msg头文件
#include "robot_msgs/Vision.h"


void doPerson(const robot_msgs::Vision::ConstPtr& vision_t){
    ROS_INFO("Vision_data: Pitch:%.2f, Yaw:%.2f, Roll:%.2f", vision_t->pitch, vision_t->yaw, vision_t->roll);
}

int main(int argc, char *argv[]){
    setlocale(LC_ALL,"");

    //1.初始化 ROS 节点
    ros::init(argc,argv,"Vision_receive_test");
    //2.创建 ROS 句柄
    ros::NodeHandle nh;
    //3.创建订阅对象
    ros::Subscriber sub = nh.subscribe<robot_msgs::Vision>("Vision_data",10,doPerson);   

    ros::spin();  
    return 0;
}