#include "ros/ros.h"
#include <ros/time.h>

// 结构体
#include "robot_status.h"

// msg
#include "robot_msgs/usb_camera.h"
#include "robot_msgs/PTZ_perception.h"
#include "robot_msgs/Track_reset.h"


// 开火模式切换
typedef enum
{
    Continuous_shoot = 1,       // 连发模式
    Three_shoot = 2,            // 三连发模式
}Fire_Mode;

// 开火模式切换
typedef enum
{
    Fire_ON = 1,                // 开火命令
    Fire_OFF = 0,               // 关火命令
}Fire;

// 左右云台模拟数据发送 | 需要进行选择
#define PTZ_L_TEST                  // 发送左云台数据
// #define PTZ_R_TEST                  // 发送右云台数据
int main(int argc, char *argv[]){
     // 设置语言运行环境
    setlocale(LC_ALL,"");  

    // 初始化节点
    ros::init(argc, argv, "PTZ_Test");

    // 创建句柄
    ros::NodeHandle nh;

    // 发送模拟数据
#ifdef PTZ_L_TEST                  // 发送左云台数据
    ros::Publisher Decision_pub = nh.advertise<robot_msgs::PTZ_perception>("/PTZ_R/PTZ_perception",1);
#endif //PTZ_L_TEST

#ifdef PTZ_R_TEST                  // 发送右云台数据
    ros::Publisher Decision_pub = nh.advertise<robot_msgs::PTZ_perception>("/PTZ_R/PTZ_perception",1);
#endif //PTZ_R_TEST


    // 创建发送数据
    robot_msgs::PTZ_perception PTZ_Yaw_t;

    while (ros::ok()){
        // 填充数据
        PTZ_Yaw_t.header.frame_id = "PTZ_Yaw_Test";
        PTZ_Yaw_t.header.seq++;
        PTZ_Yaw_t.header.stamp = ros::Time::now();

        PTZ_Yaw_t.pitch = 0;                   // pitch轴
        PTZ_Yaw_t.yaw = 0;                       // yaw轴
        PTZ_Yaw_t.track_id = 3;               // 跟踪装甲板ID
        PTZ_Yaw_t.target_lock = 0x32;         // 跟踪模式
        PTZ_Yaw_t.score = 50;                   // 装甲板分数
        PTZ_Yaw_t.fire_command = Continuous_shoot;     // 开火指令
        PTZ_Yaw_t.fire_mode = Fire_OFF;           // 开火模式

        ROS_INFO("Test_YES");
        // 发送数据
        Decision_pub.publish(PTZ_Yaw_t);
        ros::spinOnce();
    }
}
