#include "ros/ros.h"
#include <ros/time.h>

// 结构体
#include "robot_status.h"

// msg
#include "robot_msgs/usb_camera.h"
#include "robot_msgs/PTZ_perception.h"
#include "robot_msgs/Track_reset.h"
#include "robot_msgs/robot_ctrl.h"
#include "robot_msgs/Yaw_Decision.h"
#include "robot_msgs/omni_perception.h"
#include "std_msgs/Float32.h"

// 消息过滤器
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/exact_time.h>
#include <message_filters/sync_policies/approximate_time.h>

// Kalman
#include "Kalman.h"

ros::Publisher Robot_L_ctrl_pub;
ros::Publisher Decision_pub;
ros::Publisher Track_reset_pub;
ros::Publisher Robot_main_yaw_pub;

float main_yaw;                  // 大yaw轴数据
int lock_flag;                   // 锁定位标志

Kalman KF_yaw;                   // 卡尔曼滤波器 | 一阶    
ros::Time t;                     // 时间间隔差
bool Time_init = false;          // 时间初始化标志
// 观测矩阵
Eigen::Matrix<double,2,1> x_k1;  // k-1时刻的滤波值，即是k-1时刻的值
Eigen::Matrix<double,2,1> pre;   // k-1时刻的滤波值，即是k-1时刻的值  

void Auto(const robot_msgs::PTZ_perceptionConstPtr &PTZ){
    
    // 时间初始化
    if(!Time_init) t = ros::Time::now();  // 初始化更新时间
 
    // 卡尔曼计算
    if(Time_init) {
        double time = (ros::Time::now() - t).toSec();
        KF_yaw.setF(time);
        /** 预测步 */
        pre = KF_yaw.predict();
        /** 更新步 */
        double Yaw = PTZ->yaw;
        x_k1 = KF_yaw.update(Yaw);
        // 更新时间
        t = ros::Time::now();
    }

    // 创建发送数据
    robot_msgs::robot_ctrl Robot_L_ctrl_t;
    robot_msgs::Yaw_Decision Decision_t;
    std_msgs::Float32 Robot_main_yaw_t;
    
    // 填充数据
    Decision_t.header.frame_id = "Decision_Yaw_L";
    Decision_t.header.seq++;
    Decision_t.header.stamp = ros::Time::now();
    Decision_t.yaw = PTZ->yaw;
    Decision_t.target_lock = PTZ->target_lock;

    Robot_main_yaw_t.data = PTZ->yaw;                   // 大Yaw轴数据 | 目前以左云台为主,后续接入右云台的判断
    Robot_L_ctrl_t.pitch = PTZ->pitch;                  // pitch轴数据
    Robot_L_ctrl_t.yaw = PTZ->yaw;                      // yaw轴数据 | 绝对角
    // Robot_L_ctrl_t.yaw = pre[0];                        // yaw轴数据 | 绝对角 | 滤波测试
    Robot_L_ctrl_t.is_follow = PTZ->target_lock;        // 跟踪情况
    Robot_L_ctrl_t.fire_mode = PTZ->fire_mode;          // 开火模式
    Robot_L_ctrl_t.fire_command = PTZ->fire_command;    // 开火命令

    lock_flag = PTZ->target_lock;
    // 发送数据
    Robot_L_ctrl_pub.publish(Robot_L_ctrl_t);
    Decision_pub.publish(Decision_t);
}

void Omni(const robot_msgs::omni_perceptionConstPtr &omni){
    // 全向感知 | 0x31
    if(lock_flag != 0x31 && omni->target_lock == 0x31){
        // 计算坐标系下yaw轴数据
        



    }
}


int main(int argc, char *argv[]){

    // 设置语言运行环境
    setlocale(LC_ALL,"");  

    // 初始化节点
    ros::init(argc, argv, "PTZ_perception_L");

    // 创建句柄
    ros::NodeHandle nh;

    // 发送数据
    Decision_pub = nh.advertise<robot_msgs::Yaw_Decision>("/PTZ_L/Main_Yaw",1);
    Track_reset_pub = nh.advertise<robot_msgs::Track_reset>("/PTZ_L/Track_Reset",1);
    Robot_L_ctrl_pub = nh.advertise<robot_msgs::robot_ctrl>("/robot_left_gimble_ctrl",1);
    // Robot_main_yaw_pub = nh.advertise<std_msgs::Float32>("/robot_main_ctrl",1);    

    // 卡尔曼初始化
    KF_yaw.Initial();

    // 创建订阅对象
    ros::Subscriber auto_pub = nh.subscribe<robot_msgs::PTZ_perception>("/PTZ_perception_L",1,Auto);   
    ros::Subscriber omni_pub = nh.subscribe<robot_msgs::omni_perception>("/Omni_Decision/PTZ_L",1,Omni);   
    
    ros::spin();

}