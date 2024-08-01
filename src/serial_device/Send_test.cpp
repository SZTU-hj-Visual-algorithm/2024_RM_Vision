#include "ros/ros.h"
// msg头文件
#include "robot_msgs/Robot_ctrl.h"

int main(int argc, char *argv[]){

    setlocale(LC_ALL,"");

    // 初始化 ROS 节点
    ros::init(argc,argv,"Robot_ctrl_pub");

    // 创建 ROS 句柄
    ros::NodeHandle nh;

    // 创建发布者对象
    ros::Publisher pub = nh.advertise<robot_msgs::Robot_ctrl>("Robot_ctrl_data",10);

    // 创建发送数据
    robot_msgs::Robot_ctrl Robot_ctrl_t;

    Robot_ctrl_t.pitch = 10;
    Robot_ctrl_t.yaw = 10;
    Robot_ctrl_t.fire_command = 0;
    Robot_ctrl_t.target_lock = 0x31;

    ros::Rate r(1);
    while (ros::ok()){
        pub.publish(Robot_ctrl_t);
        Robot_ctrl_t.pitch++;
        ROS_INFO("Send_data");
        r.sleep();
        ros::spinOnce();
    }
    return 0;
}