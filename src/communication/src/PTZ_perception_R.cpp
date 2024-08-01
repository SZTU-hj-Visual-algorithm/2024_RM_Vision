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

ros::Publisher Robot_R_ctrl_pub;
ros::Publisher Decision_pub;
ros::Publisher Track_reset_pub;

static int lose_num = 0; 
float main_yaw;

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
    robot_msgs::robot_ctrl Robot_R_ctrl_t;
    robot_msgs::Yaw_Decision Decision_t;
    
    // 填充数据
    Decision_t.header.frame_id = "Decision_Yaw_R";
    Decision_t.header.seq++;
    Decision_t.header.stamp = ros::Time::now();
    Decision_t.yaw = PTZ->yaw;
    Decision_t.target_lock = PTZ->target_lock;
    
    Robot_R_ctrl_t.pitch = PTZ->pitch;                  // pitch轴数据
    Robot_R_ctrl_t.yaw = PTZ->yaw;                      // yaw轴数据 | 绝对角
    // Robot_R_ctrl_t.yaw = pre[0];                        // yaw轴数据 | 绝对角 | 滤波测试
    Robot_R_ctrl_t.is_follow = PTZ->target_lock;        // 跟踪情况
    Robot_R_ctrl_t.fire_mode = PTZ->fire_mode;          // 开火模式
    Robot_R_ctrl_t.fire_command = PTZ->fire_command;    // 开火命令

    // 发送数据
    Robot_R_ctrl_pub.publish(Robot_R_ctrl_t);
    
}

// void callback(const robot_msgs::barrel::ConstPtr &omni, const robot_msgs::PTZ_perception::ConstPtr &PTZ){

//     float Decision_pitch;           // 决策pitch
//     float Decision_yaw;             // 决策yaw
//     float Decision_score;           // 决策开火
//     float Decision_fire_command;    // 决策开火
//     float Decision_fire_mode;       // 决策开火模式[连发|三连发]

//     // ptich轴数据只使用云台数据
//     Decision_pitch = PTZ->pitch;
//     Decision_fire_command = PTZ->fire_command;
//     Decision_fire_mode = PTZ->fire_mode;


//     // 判断云台|全向跟踪模式
//     if(PTZ->target_lock == 0x31 && omni->target_lock == 1){
//         // 根据ID和分数决定进行决策处理

//         // ID判断是否为同一目标
//         if(PTZ->track_id == omni->track_id){
//             Decision_yaw = PTZ->yaw;
//             Decision_score = PTZ->score;
//             lose_num = 0;
//         }
//         else {
//             // 根据分数进行丢失处理,达到一定阈值进行跟踪重置
//             if(PTZ->score >= omni->grade){
//                 // 选择云台
//                 Decision_yaw = PTZ->yaw;
//                 Decision_score = PTZ->score;
//             }else{
//                 // 丢失分数叠加,超出一定阈值重新进入跟踪模式
//                 lose_num++;
//             }
//         }
     
//     }
//     else{
//         lose_num = 0;
//         // 分数决策
//         if(PTZ->score >= omni->grade){
//             // 选择云台
//             Decision_yaw = PTZ->yaw;
//             Decision_score = PTZ->score;
//         }else{
//             // 选择全向感知
//             Decision_yaw = omni->yaw;
//             Decision_score = omni->grade;
//         }
//     }

//     // 陀螺状态下选择相信云台
//     if(PTZ->spin_state != UNKNOWN){
//         Decision_yaw = PTZ->yaw;
//         Decision_score = PTZ->score;
//     }

//     // 创建发送数据
//     robot_msgs::PTZ_perception PTZ_Yaw_t;
    
//     //! 还需要判断跟踪状态
//     // 填充数据
//     PTZ_Yaw_t.header.frame_id = "PTZ_Yaw_R";
//     PTZ_Yaw_t.header.seq++;
//     PTZ_Yaw_t.header.stamp = ros::Time::now();

//     PTZ_Yaw_t.pitch = Decision_pitch;                   // pitch轴
//     PTZ_Yaw_t.yaw = Decision_yaw;                       // yaw轴
//     PTZ_Yaw_t.track_id = PTZ->track_id;               // 跟踪装甲板ID
//     PTZ_Yaw_t.target_lock = PTZ->target_lock;         // 跟踪模式
//     PTZ_Yaw_t.score = Decision_score;                   // 装甲板分数
//     PTZ_Yaw_t.fire_command = Decision_fire_command;     // 开火指令
//     PTZ_Yaw_t.fire_mode = Decision_fire_mode;           // 开火模式

//     PTZ_Yaw_t.x = PTZ->x;                               // 世界坐标x
//     PTZ_Yaw_t.y = PTZ->y;                               // 世界坐标y
//     PTZ_Yaw_t.z = PTZ->z;                               // 世界坐标z

//     // 发送数据
//     Decision_pub.publish(PTZ_Yaw_t);

//     // 丢失阈值判断
//     if(lose_num == 5){
//         robot_msgs::Track_reset Track_reset_t;
        
//         // 填充数据
//         Track_reset_t.header.frame_id = "Track_reset_R";
//         Track_reset_t.header.seq++;
//         Track_reset_t.header.stamp = ros::Time::now();
        
//         // 更新跟踪ID
//         Track_reset_t.Track_id = omni->track_id;
//         Track_reset_t.Reset = 1;

//         // 发送跟踪重置指令
//         Track_reset_pub.publish(Track_reset_t);
//         lose_num = 0;
//     }


// }

// #define omni_mode       // 全向感知开关
int main(int argc, char *argv[]){

    // 设置语言运行环境
    setlocale(LC_ALL,"");  

    // 初始化节点
    ros::init(argc, argv, "PTZ_perception_R");

    // 创建句柄
    ros::NodeHandle nh;

    Decision_pub = nh.advertise<robot_msgs::Yaw_Decision>("/PTZ_R/Main_Yaw",1);
    Track_reset_pub = nh.advertise<robot_msgs::Track_reset>("/PTZ_R/Track_Reset",1);
    Robot_R_ctrl_pub = nh.advertise<robot_msgs::robot_ctrl>("/robot_right_gimble_ctrl",1);
    
    // 卡尔曼初始化
    KF_yaw.Initial();
#ifdef omni_mode
    // 同步左云台|全向感知锁定信息,进行分数比较决策(可能还得添加一个模式的同步)
    message_filters::Subscriber<robot_msgs::usb_camera> omni_R_sub(nh, "/Robot_right_imu", 1);  
    message_filters::Subscriber<robot_msgs::PTZ_perception> PTZ_R_sub(nh, "/PTZ_perception_R", 1);  

    // 同步ROS消息
    typedef message_filters::sync_policies::ApproximateTime<robot_msgs::usb_camera, robot_msgs::PTZ_perception> MySyncPolicy;

    // 创建同步器对象
    message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), omni_R_sub, PTZ_R_sub);

    ROS_INFO("--PTZ_perception_R Start--");
    // 注册同步回调函数
    sync.registerCallback(boost::bind(&callback, _1, _2));
#else
    // 创建订阅对象
    ros::Subscriber auto_pub = nh.subscribe<robot_msgs::PTZ_perception>("/PTZ_perception_L",1,Auto);   

#endif //omni_mode

    ros::spin();

}