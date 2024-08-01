#include "ros/ros.h"
#include <ros/time.h>
#include <ros/package.h>

// Opencv 4.5.5
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Eigen
#include <Eigen/Dense>

// 消息过滤器
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/exact_time.h>
#include <message_filters/sync_policies/approximate_time.h>

// msg
#include "robot_msgs/usb_camera.h"
#include "robot_msgs/omni_perception.h"
#include "std_msgs/Float32.h"


// 最大ID查询
#define MAX_ID(a,b) a >= b ? a : b

// 全向方向决策
typedef enum
{
    Omni_init = 0,  // 决策无方向
    Omni_L = 1,     // 决策左方向
    Omni_R = 2      // 决策右方向
}Omni_direction;

// 世界坐标点
struct World_coordinates
{
    float x;
    float y;
    float z;
    void Init(float pos_x,float pos_y,float pos_z){
        x = pos_x;
        y = pos_y;
        z = pos_z;
    }
};


float main_yaw;
ros::Publisher Decision_L_pub;
ros::Publisher Decision_R_pub;

// 全向决策
void callback(const robot_msgs::usb_camera::ConstPtr &PTZ_O, const robot_msgs::usb_camera::ConstPtr &PTZ_F, const robot_msgs::usb_camera::ConstPtr &PTZ_R, const robot_msgs::usb_camera::ConstPtr &PTZ_L){

    // 左右摄像头分给左右云台部分
    int Decision_O_direction = Omni_init;
    int Decision_F_direction = Omni_init;

    // 决策数据
    double Decision_L_yaw;
    int Decision_L_target_id;
    int Decision_L_target_lock;
    World_coordinates Decision_L_World;
    double Decision_R_yaw;
    int Decision_R_target_id;
    int Decision_R_target_lock;
    World_coordinates Decision_R_World;

/**********************************角度范围判断*****************************************/

    // 判断前置摄像头识别范围
    double O_relative_yaw = (PTZ_O->yaw - main_yaw);
    // 左云台判断范围 | -20 ~ 175
    if(O_relative_yaw > -20 && O_relative_yaw < 175 && PTZ_O->target_lock == 0x31) Decision_O_direction = Omni_L;
    // 右云台判断范围 | 20 ~ -175
    else if(O_relative_yaw < 20 && O_relative_yaw > -175 && PTZ_O->target_lock == 0x31) Decision_O_direction = Omni_R;

    // 判断后置摄像头识别范围
    double F_relative_yaw = (PTZ_F->yaw - main_yaw);
    // 左云台判断范围 | -20 ~ 175
    if(F_relative_yaw > -20 && F_relative_yaw < 175 && PTZ_F->target_lock == 0x31) Decision_F_direction = Omni_L;
    // 右云台判断范围 | 20 ~ -175
    else if(F_relative_yaw < 20 && F_relative_yaw > -175 && PTZ_F->target_lock == 0x31) Decision_F_direction = Omni_R;

//END

    // ID对应优先级 1:英雄 2:工程 3|4:步兵 5:备用步兵 6:哨兵 7:前哨站 8:基地
    int ID_priority[10] = {-1,1,0,2,2,2,3,0,0};
    // Yaw轴数据
    int ID_Yaw[10] = {0};       
    // 世界坐标点
    World_coordinates Points[10];          

    // 所有情况的二进制表达数组 | 第三位为前置摄像头 第二位为后置摄像头 第一位为左右摄像头
    int16_t State_machine[10] = {0x111,0x101,0x011,0x001,0x110,0x100,0x010,0x000};
    int16_t Final_state,USB_O_sign,USB_F_sign,USB_RL_sign;
    uint8_t Decision_O,Decision_F,Decision_RL;
/**********************************左云台*****************************************/
    // 状态表达
    if(Decision_O_direction == Omni_L) USB_O_sign = 0x100;
    else USB_O_sign = 0x000;
    if(Decision_F_direction == Omni_L) USB_F_sign = 0x010;
    else USB_F_sign = 0x000;
    if(PTZ_L->target_lock == 0x31) USB_RL_sign = 0x001;
    else USB_RL_sign = 0x000;
    Final_state = USB_O_sign | USB_F_sign | USB_RL_sign;

    // 获取三个摄像头状态
    Decision_O = (Final_state >> 8) & 0x00F;
    Decision_F = (Final_state >> 4) & 0x00F;
    Decision_RL = (Final_state >> 0) & 0x00F;
    
    // 存储Yaw轴数据
    ID_Yaw[PTZ_O->track_id*Decision_O] = PTZ_O->yaw;
    ID_Yaw[PTZ_F->track_id*Decision_F] = PTZ_F->yaw;
    ID_Yaw[PTZ_L->track_id*Decision_RL] = PTZ_L->yaw;
    // 存储世界坐标数据
    Points[PTZ_O->track_id*Decision_O].Init(PTZ_O->x,PTZ_O->y,PTZ_O->z);
    Points[PTZ_F->track_id*Decision_F].Init(PTZ_F->x,PTZ_F->y,PTZ_F->z);
    Points[PTZ_L->track_id*Decision_RL].Init(PTZ_L->x,PTZ_L->y,PTZ_L->z);

    // ID优先判断
    // 左云台
    Decision_L_target_id = MAX_ID(ID_priority[PTZ_O->track_id*Decision_O],ID_priority[PTZ_F->track_id*Decision_F]);
    Decision_L_target_id = MAX_ID(Decision_L_target_id,ID_priority[PTZ_L->track_id*Decision_RL]);
    if(Final_state == 0x000){
        Decision_L_target_lock = 0x32;
    }else{
        Decision_L_target_lock = 0x31;
    }
    Decision_L_yaw = ID_Yaw[Decision_L_target_id];
    Decision_L_World = Points[Decision_L_target_id];
/**********************************右云台*****************************************/

    // 状态表达
    if(Decision_O_direction == Omni_R) USB_O_sign = 0x100;
    else USB_O_sign = 0x000;
    if(Decision_F_direction == Omni_R) USB_F_sign = 0x010;
    else USB_F_sign = 0x000;
    if(PTZ_R->target_lock == 0x31) USB_RL_sign = 0x001;
    else USB_RL_sign = 0x000;
    Final_state = USB_O_sign | USB_F_sign | USB_RL_sign;

    // 获取三个摄像头状态
    Decision_O = (Final_state >> 8) & 0x00F;
    Decision_F = (Final_state >> 4) & 0x00F;
    Decision_RL = (Final_state >> 0) & 0x00F;
    
    // 存储Yaw轴数据
    ID_Yaw[PTZ_O->track_id*Decision_O] = PTZ_O->yaw;
    ID_Yaw[PTZ_F->track_id*Decision_F] = PTZ_F->yaw;
    ID_Yaw[PTZ_R->track_id*Decision_RL] = PTZ_R->yaw;
    // 存储世界坐标数据
    Points[PTZ_O->track_id*Decision_O].Init(PTZ_O->x,PTZ_O->y,PTZ_O->z);
    Points[PTZ_F->track_id*Decision_F].Init(PTZ_F->x,PTZ_F->y,PTZ_F->z);
    Points[PTZ_R->track_id*Decision_RL].Init(PTZ_R->x,PTZ_R->y,PTZ_R->z);

    // ID优先判断
    // 右云台
    Decision_R_target_id = MAX_ID(ID_priority[PTZ_O->track_id*Decision_O],ID_priority[PTZ_F->track_id*Decision_F]);
    Decision_R_target_id = MAX_ID(Decision_R_target_id,ID_priority[PTZ_R->track_id*Decision_RL]);
    if(Final_state == 0x000){
        Decision_R_target_lock = 0x32;
    }else{
        Decision_R_target_lock = 0x31;
    }
    Decision_R_yaw = ID_Yaw[Decision_R_target_id];
    Decision_R_World = Points[Decision_L_target_id];

/**********************************END*****************************************/

    // 发送左右数据 | msg数据待定 | omni_perception.msg
    // 创建发送数据
    robot_msgs::omni_perception Omni_L_t;
    robot_msgs::omni_perception Omni_R_t;

    // 填充数据
    Omni_L_t.track_id = Decision_L_target_id;
    Omni_L_t.target_lock = Decision_L_target_lock;
    Omni_L_t.yaw = Decision_L_yaw;
    Omni_L_t.x = Decision_L_World.x;
    Omni_L_t.y = Decision_L_World.y;
    Omni_L_t.z = Decision_L_World.z;

    Omni_R_t.track_id = Decision_R_target_id;
    Omni_R_t.target_lock = Decision_R_target_lock;
    Omni_R_t.yaw = Decision_R_yaw;
    Omni_R_t.x = Decision_R_World.x;
    Omni_R_t.y = Decision_R_World.y;
    Omni_R_t.z = Decision_R_World.z;

    // 发送数据
    Decision_L_pub.publish(Omni_L_t);
    Decision_R_pub.publish(Omni_R_t);
}

// 获取大yaw轴数据
void Robot_Main_Yaw(const std_msgs::Float32ConstPtr &Yaw){
    main_yaw = Yaw->data;
}

int main(int argc, char *argv[]){

    setlocale(LC_ALL,"");

    // 初始化节点
    ros::init(argc, argv, "Omnidirectional_decision");

    // 创建句柄
    ros::NodeHandle nh;

    // 创建订阅对象
    ros::Subscriber main_yaw_sub = nh.subscribe<std_msgs::Float32>("/main_yaw",1,Robot_Main_Yaw);   

    // 发送数据
    Decision_L_pub = nh.advertise<robot_msgs::omni_perception>("/Omni_Decision/PTZ_L",1);
    Decision_R_pub = nh.advertise<robot_msgs::omni_perception>("/Omni_Decision/PTZ_R",1);

    // 建立需要订阅的消息对应的订阅器
    message_filters::Subscriber<robot_msgs::usb_camera> USB_Camera_O_sub(nh, "/USB_Camera/PTZ_O", 1);  
    message_filters::Subscriber<robot_msgs::usb_camera> USB_Camera_F_sub(nh, "/USB_Camera/PTZ_F", 1);  
    message_filters::Subscriber<robot_msgs::usb_camera> USB_Camera_R_sub(nh, "/USB_Camera/PTZ_R", 1);  
    message_filters::Subscriber<robot_msgs::usb_camera> USB_Camera_L_sub(nh, "/USB_Camera/PTZ_L", 1);  

    // 同步ROS消息
    typedef message_filters::sync_policies::ApproximateTime<robot_msgs::usb_camera, robot_msgs::usb_camera, robot_msgs::usb_camera, robot_msgs::usb_camera> MySyncPolicy;

    // 创建同步器对象
    message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), USB_Camera_O_sub, USB_Camera_F_sub, USB_Camera_R_sub, USB_Camera_L_sub);

    ROS_INFO("--Omnidirectional_decision Start--");

    // 注册同步回调函数
    sync.registerCallback(boost::bind(&callback, _1, _2, _3, _4));


    ros::spin();
    return 0;
}