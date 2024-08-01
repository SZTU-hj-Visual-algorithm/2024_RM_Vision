#include "ros/ros.h"
#include <ros/time.h>

// msg
#include "robot_msgs/robot_ctrl.h"
#include "robot_msgs/PTZ_Yaw.h"
#include "robot_msgs/PTZ_perception.h"
#include "robot_msgs/Yaw_Decision.h"
#include "std_msgs/Float32.h"
#include "std_msgs/UInt8.h"

// 消息过滤器
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/exact_time.h>
#include <message_filters/sync_policies/approximate_time.h>

// Kalman
#include "Kalman.h"

// 大Yaw发送数据
ros::Publisher Robot_main_yaw_pub;


// 击打模式切换
typedef enum
{
    Same_target = 10,          // 相同目标击打
    Different_target = 20,     // 独立目标击打
    Base_Strike = 3           // 基地不击打模式(暂定)
}Strike_Mode;

int Mode = Same_target;       // 等待接口接入
float main_yaw;
float Decision_Yaw;

// 计算yaw轴数据,进行控制处理
void callback(const robot_msgs::Yaw_DecisionConstPtr &PTZ_L, const robot_msgs::Yaw_DecisionConstPtr &PTZ_R){
    // 0x31表示跟踪,0x32表示丢失
    int target_lock_L = PTZ_L->target_lock;
    int target_lock_R = PTZ_R->target_lock;

    bool situation_1 = (target_lock_L == 0x31) && (target_lock_R == 0x31);  // 情况1 
    bool situation_2 = (target_lock_L == 0x31) && (target_lock_R == 0x32);  // 情况2
    bool situation_3 = (target_lock_L == 0x32) && (target_lock_R == 0x31);  // 情况3
    
    // 进行决策
    // 目前决策方案 | 以左云台为主
    if(situation_1){
        Decision_Yaw = PTZ_L->yaw;
    }
    else if(situation_2){
        Decision_Yaw = PTZ_L->yaw;
    }
    else if(situation_3){
        Decision_Yaw = PTZ_R->yaw;
    }
    else {
        Decision_Yaw = main_yaw;
    }
    ROS_INFO("YAW: %f",Decision_Yaw);
    std_msgs::Float32 Robot_main_yaw_t;
    Robot_main_yaw_t.data = Decision_Yaw;                   // 大Yaw轴数据 | 目前以左云台为主,后续接入右云台的判断
    Robot_main_yaw_pub.publish(Robot_main_yaw_t);
}

// 获取大yaw轴数据
void Robot_Main_Yaw(const std_msgs::Float32ConstPtr &Yaw){
    main_yaw = Yaw->data;
}

// 获取模式
void Vision_mode(const std_msgs::UInt8ConstPtr &Vision_Mode){
    Mode = Vision_Mode->data;
}


// 同步左右云台的yaw轴数据和大云台的yaw轴数据,进行数据处理
int main(int argc, char *argv[]){

    // 设置语言运行环境
    setlocale(LC_ALL,"");  

    // 初始化节点
    ros::init(argc, argv, "Yaw_Communication_sync");

    // 创建句柄
    ros::NodeHandle nh;
    
    // 创建订阅对象
    ros::Subscriber main_yaw_sub = nh.subscribe<std_msgs::Float32>("/main_yaw",1,Robot_Main_Yaw);   
    ros::Subscriber mode_sub = nh.subscribe<std_msgs::UInt8>("/attack_mode",1,Vision_mode);   

    // 发送数据
    Robot_main_yaw_pub = nh.advertise<std_msgs::Float32>("/robot_main_ctrl",1);

    // 建立需要订阅的消息对应的订阅器 (可能还得添加一个模式的同步)
    message_filters::Subscriber<robot_msgs::Yaw_Decision> PTZ_L_sub(nh, "/PTZ_L/Main_Yaw", 1);  
    message_filters::Subscriber<robot_msgs::Yaw_Decision> PTZ_R_sub(nh, "/PTZ_R/Main_Yaw", 1);  

    // 同步ROS消息
    typedef message_filters::sync_policies::ApproximateTime<robot_msgs::Yaw_Decision, robot_msgs::Yaw_Decision> MySyncPolicy;

    // 创建同步器对象
    message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(100), PTZ_L_sub, PTZ_R_sub);


    ROS_INFO("[Yaw_Communication_sync]: Start");
    // 注册同步回调函数
    sync.registerCallback(boost::bind(&callback, _1, _2));

    ros::spin();

    return 0;

}