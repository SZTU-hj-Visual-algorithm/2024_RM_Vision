#include "ros/ros.h"
#include <ros/time.h>

// 结构体
#include "protocol.h"
#include "robot_struct.h"

// 图像传输
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/CompressedImage.h>
#include <image_transport/image_transport.h>

// 消息过滤器
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/exact_time.h>
#include <message_filters/sync_policies/approximate_time.h>

// msg
#include "robot_msgs/robot_ctrl.h"
#include "robot_msgs/vision.h"
#include "robot_msgs/PTZ_perception.h"
#include "robot_msgs/Track_reset.h"
#include "std_msgs/UInt8.h"
#include "std_msgs/Int32.h"

// Opencv 4.5.5
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Eigen
#include <Eigen/Dense>

// 识别预测头文件
#include "AngleSolve.h"
#include "Armor_Track.h"
#include "Armor_Observe.h"
#include "Armor_detection.h"
#include "Neural_Armor_Detection.h"


// 数据部分
cv::Mat src;
form Vision_data;
ros::Publisher PTZ_perception_pub;

// #define Neural_Detection    // 神经网络启动开关 
#ifdef Neural_Detection
NeuralArmorDetector Neural_Detector;
#else
ArmorDetector Detector;
#endif //Neural_Detection
AngleSolve AS;
ArmorTrack Tracker;
ArmorObserve AO;
std::vector<double> vdata(4);

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

int Mode = 0;                   // 击打模式
bool Reset = false;             // 重置跟踪标志位
ros::Time reset_time;           // 自瞄重置时间

void callback(const sensor_msgs::ImageConstPtr &src_msg, const robot_msgs::vision::ConstPtr &Vision_msg){
    // Armor容器
    std::vector<Armor> Targets;

    // 读取图片
    src = cv_bridge::toCvShare(src_msg, "bgr8")->image;

    // 读取IMU数据
    Vision_data.data[0] = Vision_msg->pitch;  // pitch
    Vision_data.data[1] = Vision_msg->yaw;    // yaw
    Vision_data.data[2] = Vision_msg->roll;   // roll
    Vision_data.enemy_color = Vision_msg->id; // 颜色 | 107:蓝方 7:红方
    // Vision_data.mode = Vision_msg->mode;      // 0x21(哨兵废弃模式选项)

    // 四元数
    for (size_t i = 0; i < 4; i++)
    {
      Vision_data.quat[i] = Vision_msg->quaternion[i];
    }
    
    // 根据模式选择击打目标
    
    // 初始化四元数
    Tracker.AS.Init(Vision_data.data, Vision_data.quat);
    AO.AS.Init(Vision_data.data, Vision_data.quat);

    // 选择击打颜色
#ifdef Neural_Mode
    if(Vision_data.enemy_color == 107) Neural_Detector.enemy_color = RED;
    else if(Vision_data.enemy_color == 7) Neural_Detector.enemy_color = BLUE;
    // Neural_Detector.enemy_color = Vision_data.enemy_color;
    // Neural_Detector.enemy_color = BLUE;
#else 
    if(Vision_data.enemy_color == 107) Detector.enemy_color = RED;
    else if(Vision_data.enemy_color == 7) Detector.enemy_color = BLUE;
    // Detector.enemy_color = Vision_data.enemy_color;
    // Detector.enemy_color = BLUE;
#endif //Neural_Mode

    // 重置跟踪 | 时间
    double time = (ros::Time::now() - reset_time).toSec();
    // 自瞄时间大于5s重置跟踪目标
    if(time > 5) Reset = true;
    // 重置后更新时间
    if(Reset) {
        reset_time = ros::Time::now();
        Tracker.Reset();
        Reset = false;
    }

    // 进行识别处理
#ifdef Neural_Mode
    Targets = Neural_Detector.detect(src);
#else 
    Detector.Base_Strike = Mode;
    Targets = Detector.Detection(src);
#endif //Neural_Mode

    // 获取最终装甲板
    Tracker.Track(src,Targets,std::chrono::high_resolution_clock::now());
    Tracker.show();
    cv::waitKey(1);
    
    // 弹道速度(后面改成发过来的弹道数据) | 有点问题
    // AS.bullet_speed = 25;

    // 整车观测
    bool AO_OK = (Tracker.tracker_state == TRACKING) && Tracker.OB_Track[Tracker.tracking_id].is_initialized;
    // 开始拟合圆心
    double OK = false;
    if(AO_OK) {
        double angle =  AO.spin_angle*(180.0f/CV_PI);
        // 左右角度[以逆时针为基准]
        // -30 -45
        double select_angle_left = -15;
        double select_angle_right = -25;
    
        AO.Angle_Speed = Tracker.Angle_Speed; 
        AO.Spin_State = Tracker.Spin_State(); 
        AO.Center_fitting(Tracker.enemy_armor,Tracker.OB[Tracker.tracking_id].center_Z,Tracker.OB[Tracker.tracking_id],Tracker.OB_Track[Tracker.tracking_id]);

        // 角度设置以逆时针为主,角度为负 | 陀螺时yaw角度为左正右负
        // 不再进行速度阈值的判断,直接通过角度的判断进行击打
        // 逆时针[L]
        if(Tracker.Spin_State() == COUNTER_CLOCKWISE){
            if(AO.spin_angle*(180.0f/CV_PI) < select_angle_left && AO.spin_angle*(180.0f/CV_PI) > select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.spin_Aromor);
                OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            }
            else if(AO.Left_Armor_angle < select_angle_left && AO.Left_Armor_angle > select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.Left_Armor);
                OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            } 
            else if(AO.O_Armor_angle < select_angle_left && AO.O_Armor_angle > select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.O_Armor);
                OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            }
            // 固定点位需要修改
            else {
                // Eigen::Vector3d rpy = AS.Barrel_Solve(AO.left_target);
                // Tracker.Solve_pitch = rpy[1];
                // Tracker.Solve_yaw = rpy[2];
                Tracker.Solve_pitch = Tracker.AS.Robot_msg.Controller_pitch;
                Tracker.Solve_yaw = Tracker.AS.Robot_msg.Controller_yaw;
            }
        }
        // 顺时针[R] 
        else if (Tracker.Spin_State() == CLOCKWISE)
        {   // 15 20
            if(AO.spin_angle*(180.0f/CV_PI) > -select_angle_left && AO.spin_angle*(180.0f/CV_PI) < -select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.spin_Aromor);
                OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            }
            if(AO.Right_Armor_angle > -select_angle_left && AO.Right_Armor_angle < -select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.Right_Armor);
                OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            } 
            else if(AO.O_Armor_angle > -select_angle_left && AO.O_Armor_angle < -select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.O_Armor);
                OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            }
            // 固定点位需要修改
            else {
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.right_target);
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
                // Tracker.Solve_pitch = Tracker.AS.Robot_msg.Controller_pitch;
                // Tracker.Solve_yaw = Tracker.AS.Robot_msg.Controller_yaw;
            }
        }
        else{
            // 退出陀螺模式
            // Tracker.Angle_Speed = 0;
        }
            
        cv::putText(src,"OK: "+ std::to_string(OK),cv::Point(0,100),cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255, 255, 0),2,3);
        // if(AO.Fit_OK) {AO.ArmorObserve_show(AO.Smooth_position,Tracker.OB[Tracker.tracking_id],Tracker.OB_Track[Tracker.tracking_id]);}
    }

    // 开火指令判断
    double Fire;
    double Fire_mode;
    if(OK) Fire = 1;
    else Fire = 0;

    // 决策框锁定开火[坐标点在左上和右下之间]
    cv::Point2f Hitting_frame[4];
    float img_rows,img_cols;
    img_rows = (float)src.rows;
    img_cols = (float)src.cols;
    // 顺序: 左上 | 右下 
    Hitting_frame[0].x = img_cols*Tracker.wide_ratio;
    Hitting_frame[0].y = img_rows*Tracker.high_ratio;
    Hitting_frame[2].x = img_cols - (img_cols*Tracker.wide_ratio);
    Hitting_frame[2].y = img_rows - (img_rows*Tracker.high_ratio);

    bool Fire_X_OK = (Tracker.enemy_armor.center.x < Hitting_frame[0].x) && (Tracker.enemy_armor.center.x > Hitting_frame[2].x);
    bool Fire_Y_OK = (Tracker.enemy_armor.center.y < Hitting_frame[0].y) && (Tracker.enemy_armor.center.y > Hitting_frame[2].y);
    
    //! 决策框的大小需要根据距离来实际进行大小的调整[重点是左右的距离变化] (需要吗)

    // 判断在决策框内 [需要放在跟踪状态内]
    if(Fire_X_OK && Fire_Y_OK){
        // 处于陀螺状态
        if(Tracker.Spin_State()){
            // 自动开火 | 三连发
            Fire_mode = Three_shoot;
        }
        // 不处于陀螺状态
        else{
            // 进行开火 | 连发
            Fire = Fire_ON;
            Fire_mode = Continuous_shoot;
        }
    }else{
        Fire = Fire_OFF;
    }


    // 创建发送数据
    robot_msgs::PTZ_perception PTZ_perception_t;

    // 开火判断(需要在限定框内进行开火,开火模式的选择: 陀螺模式下(三连发) 正常模式下(连发))[暂定]
    //！一般情况下,需要卡尔曼进行移动预测,陀螺暂时不需要

    // 填充数据
    PTZ_perception_t.header.frame_id = "PTZ_perception_R";
    PTZ_perception_t.header.seq++;
    PTZ_perception_t.header.stamp = ros::Time::now();

    PTZ_perception_t.pitch = Tracker.Solve_pitch;           // pitch轴
    PTZ_perception_t.yaw = Tracker.Solve_yaw;               // yaw轴
    PTZ_perception_t.score = Tracker.enemy_armor.grade;     // 装甲板分数
    PTZ_perception_t.track_id = Tracker.tracking_id;        // 跟踪装甲板ID
    PTZ_perception_t.spin_state = Tracker.Spin_State();     // 陀螺状态
    if(Fire == 1) PTZ_perception_t.fire_command = 1;        // 开火指令
    else PTZ_perception_t.fire_command = 0;      
    PTZ_perception_t.fire_mode = Fire_mode;                 // 开火模式
        
    // 坐标值 
    PTZ_perception_t.x = Tracker.enemy_armor.world_position[0];                         // 世界坐标x
    PTZ_perception_t.y = Tracker.enemy_armor.world_position[1];                         // 世界坐标y
    PTZ_perception_t.z = Tracker.enemy_armor.world_position[2];                         // 世界坐标z

    // 陀螺情况下属于跟踪状态
    if(Tracker.Spin_State()){
        PTZ_perception_t.target_lock = 0x31;                    // 跟踪状态
    }else{
        // 非陀螺状态下
        if(Tracker.tracker_state == TRACKING){
            PTZ_perception_t.target_lock = 0x31;                    // 跟踪状态
        }else{
            PTZ_perception_t.target_lock = 0x32;                    // 跟踪状态
        }
    }
    
    // 发送数据
    PTZ_perception_pub.publish(PTZ_perception_t);
}

// 重置跟踪状态命令
void Track_Reset_CMD(const robot_msgs::Track_reset::ConstPtr &Track_reset_t){
    // 初始化
    Tracker.Track_reset();
    // 重新设置跟踪ID(不进行跟踪状态的重置)
    Tracker.tracking_id = Track_reset_t->Track_id;
}

// 获取模式
void Vision_mode(const std_msgs::Int32::ConstPtr &Vision_Mode){
    Mode = Vision_Mode->data;
}

int main(int argc, char *argv[]){

    setlocale(LC_ALL,"");

    // 初始化节点
    ros::init(argc, argv, "Robot_Detection_R");

    // 创建句柄
    ros::NodeHandle nh;

    PTZ_perception_pub = nh.advertise<robot_msgs::PTZ_perception>("PTZ_perception_R",1);
    reset_time = ros::Time::now();
    
    ros::Subscriber Track_reset_sub = nh.subscribe<robot_msgs::Track_reset>("/PTZ_R/Track_Reset",1,Track_Reset_CMD);   
    ros::Subscriber mode_sub = nh.subscribe<std_msgs::Int32>("is_attack_base_mode",1,Vision_mode);   

    // 建立需要订阅的消息对应的订阅器
    message_filters::Subscriber<sensor_msgs::Image> HIK_Camera_sub(nh, "/HIK_Camera_R/image", 1);  
    // message_filters::Subscriber<robot_msgs::Vision> Imu_sub(nh, "/Serial_Device/Vision_R_data", 1);  
    message_filters::Subscriber<robot_msgs::vision> Imu_sub(nh, "/right_gimbal_vision_data", 1);  


    // 同步ROS消息
    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, robot_msgs::vision> MySyncPolicy;

    // 创建同步器对象
    message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), HIK_Camera_sub, Imu_sub);

    ROS_INFO("--Robot_Detection_R Start--");
    // 注册同步回调函数
    sync.registerCallback(boost::bind(&callback, _1, _2));

    ros::spin();

    return 0;

}