#include "ros/ros.h"
#include <ros/time.h>
#include <ros/package.h>

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
#include "robot_msgs/Robot_ctrl.h"
#include "robot_msgs/Vision.h"

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
#include "Outpost_Observe.h"

// 能量机关识别与预测
#include "BuffDetection.h"
#include "BuffPrediction.h"

// 数据部分
cv::Mat src;
form Vision_data;
ros::Publisher Vision_pub;
ros::Time Time;             // 自瞄时间阈值
ros::Time Buff_Time;             // Buff时间阈值

// robot_status.h里面定义开关
#ifdef Neural_Mode
NeuralArmorDetector Neural_Detector;
#else
ArmorDetector Detector;
#endif //Neural_Mode
AngleSolve AS;
ArmorTrack Tracker;
ArmorObserve AO;
Outpost_Observe PO;
std::vector<double> vdata(4);


BuffDetection* BD = new BuffDetection;
BuffPrediction BP;
BuffTarget Target;
bool buff_init = false;

// 模式切换
typedef enum
{
    Auto = 1,               // 自瞄模式
    Outpost = 2,            // 前哨站模式
    Buff = 3                // Buff模式
}Attack_Mode;

int Mode = Auto;                            // 自瞄模式:1 | 前哨站:2 
ros::Time Duration;                         // 模式持续时间 

/** 火控参数 */
ros::Time Spin_Fire_time;                   // 时间间隔
ros::Time Outpost_Fire_time;
double Spin_Time_accumulation;              // 时间间隔累积
double Outpost_Time_accumulation;
bool Spin_Fire_control_Init = false;        // 火控初始化
bool Outpost_Fire_control_Init = false;

void callback(const sensor_msgs::ImageConstPtr &src_msg, const robot_msgs::VisionConstPtr &Vision_msg){
    
    // ROS_INFO("---------------------------");
    // Armor容器
    std::vector<Armor> Targets;

    // 读取图片
    src = cv_bridge::toCvShare(src_msg, "bgr8")->image;

    // 读取IMU数据
    Vision_data.enemy_color = Vision_msg->id; // 颜色 | 107:蓝方 7:红方
    Vision_data.data[0] = Vision_msg->pitch;  // pitch
    Vision_data.data[1] = Vision_msg->yaw;    // yaw
    Vision_data.data[2] = Vision_msg->shoot;  // shoot
    Vision_data.mode = Vision_msg->mode;      // 0x21
    // 四元数
    for (size_t i = 0; i < 4; i++)
    {
      Vision_data.quat[i] = Vision_msg->quaternion[i];
    }

    // 初始化四元数 | 需要特别注意,每一个使用到AS的类都需要四元数初始化
    BP.AS.Init(Vision_data.data, Vision_data.quat);
    BD->AS.Init(Vision_data.data, Vision_data.quat);
    Tracker.AS.Init(Vision_data.data, Vision_data.quat);
    AO.AS.Init(Vision_data.data, Vision_data.quat);
    PO.AS.Init(Vision_data.data, Vision_data.quat);

    // 选择击打颜色
#ifdef Neural_Mode
    if(Vision_data.enemy_color == 107) Neural_Detector.enemy_color = RED;
    else if(Vision_data.enemy_color == 7) Neural_Detector.enemy_color = BLUE;
#else 
    if(Vision_data.enemy_color == 107) Detector.enemy_color = RED;
    else if(Vision_data.enemy_color == 7) Detector.enemy_color = BLUE;
#endif //Neural_Mode

    double OK = false;
#ifndef DEBUG
if(Vision_data.mode == 0x21)
{
#endif //DEBUG

    // 判断时间
    double dt = (ros::Time::now() - Time).toSec();    // 计算时间间隔
    if(dt > 1) {
        Tracker.Reset();
        PO.Reset();
    }
    // 更新时间
    Time = ros::Time::now();

    // 进行识别处理
#ifdef Neural_Mode
    Targets = Neural_Detector.detect(src);
#else 
    Targets = Detector.Detection(src);
#endif //Neural_Mode
    // 获取最终装甲板
    Tracker.Track(src,Targets,std::chrono::high_resolution_clock::now());
    circle(src,PO.cir,4,CV_RGB(0, 255, 0),-1);
    Tracker.show();
    cv::waitKey(1);

    // 前哨站功能 | 进入前哨站模式,持续一段时间后退出
    if(Tracker.tracking_id == 7 || Tracker.tracking_id == 8) {
        PO.Outpost_Track(Tracker.enemy_armor,std::chrono::high_resolution_clock::now());
        Duration = ros::Time::now();    // 更新时间
    }

    // 模式判断
    double Duration_time;
    Duration_time = (ros::Time::now() - Duration).toSec();

    // 模式丢失时间大于1s 退出前哨站模式,返回自瞄模式
    if(Duration_time > 1) Mode = Auto;
    else Mode = Outpost;

    // 创建发送数据
    robot_msgs::Robot_ctrl Robot_ctrl_t;

    // 开火判断
    double fire;
    if(OK) fire = 1;
    else fire = 0;

    // 填充数据
    if(Tracker.tracker_state == TRACKING){
        Robot_ctrl_t.pitch = Tracker.Solve_pitch;
        Robot_ctrl_t.yaw = Tracker.Solve_yaw;
        Robot_ctrl_t.fire_command = fire;
        Robot_ctrl_t.target_lock = 0x31;
    }else {
        Robot_ctrl_t.pitch = Tracker.AS.Robot_msg.Controller_pitch;
        Robot_ctrl_t.yaw = Tracker.AS.Robot_msg.Controller_yaw;
        Robot_ctrl_t.fire_command = 0;
        Robot_ctrl_t.target_lock = 0x32;
    }

    // 发送数据
    // Vision_pub.publish(Robot_ctrl_t);
       
    // 初始化火控 
    Spin_Fire_control_Init = false;
    Outpost_Fire_control_Init = false;

#ifndef DEBUG
}
#endif //DEBUG
if (Vision_data.mode == 0x23 || Vision_data.mode == 0x22)
    {  

        Mode = Buff;
        // 初始化判断
        double dt = (ros::Time::now() - Buff_Time).toSec();    // 计算时间间隔
        ROS_INFO("dt:%f",dt);
        if(dt > 0.2) BD = new BuffDetection;
        Buff_Time = ros::Time::now(); 

        // TODO： 注意能量机关是击打自己颜色的！
        if (Vision_data.enemy_color == 107){
            BD->our_color = BLUE;
        }
        else if (Vision_data.enemy_color == 7){
            BD->our_color = RED;
        }
        // std::cout<<BD.our_color<<std::endl;
        if (buff_init == false)
        {
            BP.Solve_OK = false;
            BD->Detection_Init();   //
            BP.Prediction_Init();
            buff_init = true;
            ROS_INFO("INIT");
         }

        // 0x23 c键    0x22 x键  BIGBUFF
        if (Vision_data.mode == 0x22)
        {
           
            BP.Buff_Mode = SMALL;
            BD->buff_mode = SMALL;
        }
        else if (Vision_data.mode == 0x23)
        {
            BP.Buff_Mode = BIG;
            BD->buff_mode = BIG;  
        }


        // FIXEME
        //  NeuralBuffDetector neuralBuff;
        //  std::vector<NBDerection> myBuff = neuralBuff.detect(src);

        // 进行识别处理
 
        std::vector<BuffTarget> finalBuffTarget;
        finalBuffTarget = BD->Detection(src);
        if(!finalBuffTarget.empty()){
            Target  =finalBuffTarget[0];
            BP.Prediction(src, Target);
        }

 
        cv::imshow("src", src);
        cv::waitKey(1);
   
        // 创建发送数据
        robot_msgs::Robot_ctrl Robot_ctrl_t;

        // 填充数据
        if (BP.Solve_OK && abs(BP.Solve_pitch)<100 && abs(BP.Solve_yaw)<100)
        {
            Robot_ctrl_t.pitch = BP.Solve_pitch;
            Robot_ctrl_t.yaw = BP.Solve_yaw;
            Robot_ctrl_t.fire_command = BP.fire_Ok;
            Robot_ctrl_t.target_lock = 0x31;
        }
        else
        {
            Robot_ctrl_t.pitch = BP.AS.Robot_msg.Controller_pitch;
            Robot_ctrl_t.yaw = BP.AS.Robot_msg.Controller_yaw;
            Robot_ctrl_t.fire_command = BP.fire_Ok;
            Robot_ctrl_t.target_lock = 0x32;
        }

        // 发送数据
        Vision_pub.publish(Robot_ctrl_t);
    }
    else
    {       
        buff_init = false;  //不进入程序就初始化改成flase ，这样每次进入模式都是重新初始化
    }
}

// 反陀螺优化方向:通过补帧的方法去加强击打灵敏度,以毫秒级进行补帧,避免每一次识别时跳过击打范围,导致火控失效
// C板的接送频率在300HZ左右? 为了以毫米级来更新数据,需要控制发送频率,不能过快? 过快会导致其他数据接送出现问题 
// 如果出现控制问题,那就将火控单独独立处理,电控那边将火控的接受包含0x32,即火控完全独立

// 绝对角限幅函数
void Angle_limit(double *Angle){
    if(*Angle >= 180) *Angle -= 360;
    else if(*Angle <= -180) *Angle += 360;
}

// 反陀螺火控
void Spin_Fire_control(){

    // 判断时间
    double dt = (ros::Time::now() - Time).toSec();    // 计算时间间隔
    if(dt > 0.2) {
        Tracker.Reset();
    }

    double Fire_OK = false;
    double select_angle_left = -15;
    double select_angle_right = -35;

    // 记录角度 
    double spin_angle = AO.spin_angle*(180.0f/CV_PI);
    double Left_Armor_angle = AO.Left_Armor_angle;
    double Right_Armor_angle = AO.Right_Armor_angle;
    double O_Armor_angle = AO.O_Armor_angle;
    double Angle_Speed = AO.Angle_Speed;

    // 时间的累积 | 在自瞄更新时清空
    Spin_Time_accumulation += (ros::Time::now() - Spin_Fire_time).toSec() * 1000.0f;
    // ROS_INFO("Spin_Fire_time: %f ms  Spin_accumulation: %f ms",(ros::Time::now() - Spin_Fire_time).toSec() * 1000.0f , Spin_Time_accumulation);

    // 角度累加 | 分顺逆
    if(Tracker.Spin_State() == COUNTER_CLOCKWISE){
        spin_angle -= Spin_Time_accumulation*Angle_Speed;
        Left_Armor_angle -= Spin_Time_accumulation*Angle_Speed;
        Right_Armor_angle -= Spin_Time_accumulation*Angle_Speed;
        O_Armor_angle -= Spin_Time_accumulation*Angle_Speed;
    }
    else if(Tracker.Spin_State() == CLOCKWISE){
        spin_angle += Spin_Time_accumulation*Angle_Speed;
        Left_Armor_angle += Spin_Time_accumulation*Angle_Speed;
        Right_Armor_angle += Spin_Time_accumulation*Angle_Speed;
        O_Armor_angle += Spin_Time_accumulation*Angle_Speed;
    }

    // 对绝对角进行限制 
    Angle_limit(&spin_angle);
    Angle_limit(&Left_Armor_angle);
    Angle_limit(&Right_Armor_angle);
    Angle_limit(&O_Armor_angle);

    // 火控独立 | 只做火控处理? | 先做逆时针
    // 可能要根据当前角度重新计算装甲板 | 会稳定吗?
    if(Tracker.Spin_State() == COUNTER_CLOCKWISE){
            // std::cout << spin_angle << "   " << spin_angle << std::endl;
            if(spin_angle < select_angle_left && spin_angle > select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.spin_Aromor);
                Fire_OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            }
            else if(Left_Armor_angle < select_angle_left && Left_Armor_angle > select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.Left_Armor);
                Fire_OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            } 
            else if(Right_Armor_angle < select_angle_left && Right_Armor_angle > select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.Right_Armor);
                Fire_OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            } 
            else if(O_Armor_angle < select_angle_left && O_Armor_angle > select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.O_Armor);
                Fire_OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            }
            // 固定点位需要修改
            else {
                Tracker.Solve_pitch = Tracker.AS.Robot_msg.Controller_pitch;
                Tracker.Solve_yaw = Tracker.AS.Robot_msg.Controller_yaw;
            }
    }
    // 顺时针[R] 
    else if (Tracker.Spin_State() == CLOCKWISE){
            if(spin_angle > -select_angle_left && spin_angle < -select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.spin_Aromor);
                Fire_OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            }         
               else if(Left_Armor_angle > -select_angle_left && Left_Armor_angle < -select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.Left_Armor);
                Fire_OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            } 
            else if(Right_Armor_angle > -select_angle_left && Right_Armor_angle < -select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.Right_Armor);
                Fire_OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            } 
            else if(O_Armor_angle > -select_angle_left && O_Armor_angle < -select_angle_right){
                Eigen::Vector3d rpy = AS.Barrel_Solve(AO.O_Armor);
                Fire_OK = true;
                Tracker.Solve_pitch = rpy[1];
                Tracker.Solve_yaw = rpy[2];
            }
            // 固定点位需要修改
            else {
                Tracker.Solve_pitch = Tracker.AS.Robot_msg.Controller_pitch;
                Tracker.Solve_yaw = Tracker.AS.Robot_msg.Controller_yaw;
            }
    }
    

    // 创建发送数据
    robot_msgs::Robot_ctrl Robot_ctrl_t;

    // 开火判断
    double fire;
    if(Fire_OK) fire = 1;
    else fire = 0;

    // 填充数据
    if(Tracker.tracker_state == TRACKING){
        Robot_ctrl_t.pitch = Tracker.Solve_pitch;
        Robot_ctrl_t.yaw = Tracker.Solve_yaw;
        Robot_ctrl_t.fire_command = fire;
        Robot_ctrl_t.target_lock = 0x31;
    }else {
        Robot_ctrl_t.pitch = Tracker.AS.Robot_msg.Controller_pitch;
        Robot_ctrl_t.yaw = Tracker.AS.Robot_msg.Controller_yaw;
        Robot_ctrl_t.fire_command = 0;
        Robot_ctrl_t.target_lock = 0x32;
    }
    Vision_pub.publish(Robot_ctrl_t);

    // 更新时间
    Spin_Fire_time = ros::Time::now();
}

// 前哨站火控
void Outpost_Fire_control(){

    double Fire_OK = false;
    double select_angle_left = -15;
    double select_angle_right = -20;

    // 记录角度 
    double spin_angle = PO.spin_angle*(180.0f/CV_PI);
    double Left_Armor_angle = PO.Left_Armor_angle;
    double Right_Armor_angle = PO.Right_Armor_angle;
    double Angle_Speed = 0.144;

    // 时间的累积 | 在自瞄更新时清空
    Outpost_Time_accumulation += (ros::Time::now() - Outpost_Fire_time).toSec() * 1000.0f;
    // ROS_INFO("Outpost_Fire_time: %f ms  Outpost_accumulation: %f ms",(ros::Time::now() - Outpost_Fire_time).toSec() * 1000.0f , Outpost_Time_accumulation);

    // 角度累加 | 分顺逆 | 暂待测试
    // if(PO.Spin_State() == COUNTER_CLOCKWISE){
    //     spin_angle -= Outpost_Time_accumulation*Angle_Speed;    
    //     Left_Armor_angle -= Outpost_Time_accumulation*Angle_Speed;
    //     Right_Armor_angle -= Outpost_Time_accumulation*Angle_Speed;
    // }
    // else if(PO.Spin_State() == CLOCKWISE){
    //     spin_angle += Outpost_Time_accumulation*Angle_Speed;
    //     Left_Armor_angle += Outpost_Time_accumulation*Angle_Speed;
    //     Right_Armor_angle += Outpost_Time_accumulation*Angle_Speed;
    // }
    
    // printf("spin_angle: %f  Left_Armor_angle:%f  Right_Armor_angle:%f \n",spin_angle,Left_Armor_angle,Right_Armor_angle);

    // 对绝对角进行限制 
    Angle_limit(&spin_angle);
    Angle_limit(&Left_Armor_angle);
    Angle_limit(&Right_Armor_angle);

    // 火控部分 | 瞄中心？还是前哨站中心 | 目前还是选择瞄装甲板
    if(PO.Spin_State() == COUNTER_CLOCKWISE){
        if(spin_angle < select_angle_left && spin_angle > select_angle_right){
            Eigen::Vector3d rpy = AS.Barrel_Solve(PO.O_Armor);
            Fire_OK = true;
            Tracker.Solve_pitch = rpy[1];
            Tracker.Solve_yaw = rpy[2];
        }
        else if(Left_Armor_angle < select_angle_left && Left_Armor_angle > select_angle_right){
            Eigen::Vector3d rpy = AS.Barrel_Solve(PO.L_Armor);
            Fire_OK = true;
            Tracker.Solve_pitch = rpy[1];
            Tracker.Solve_yaw = rpy[2];
        }
        else if(Right_Armor_angle < select_angle_left && Right_Armor_angle > select_angle_right){
            Eigen::Vector3d rpy = AS.Barrel_Solve(PO.R_Armor);
            Fire_OK = true;
            Tracker.Solve_pitch = rpy[1];
            Tracker.Solve_yaw = rpy[2];
        }
        else {
            Tracker.Solve_pitch = Tracker.AS.Robot_msg.Controller_pitch;
            Tracker.Solve_yaw = Tracker.AS.Robot_msg.Controller_yaw;
        }

    }
    else if(PO.Spin_State() == CLOCKWISE){
        if(spin_angle > -select_angle_left && spin_angle < -select_angle_right){
            Eigen::Vector3d rpy = AS.Barrel_Solve(PO.O_Armor);
            Fire_OK = true;
            Tracker.Solve_pitch = rpy[1];
            Tracker.Solve_yaw = rpy[2];
        }
        else if(Left_Armor_angle > -select_angle_left && Left_Armor_angle < -select_angle_right){
            Eigen::Vector3d rpy = AS.Barrel_Solve(PO.L_Armor);
            Fire_OK = true;
            Tracker.Solve_pitch = rpy[1];
            Tracker.Solve_yaw = rpy[2];
        }
        else if(Right_Armor_angle > -select_angle_left && Right_Armor_angle < -select_angle_right){
            Eigen::Vector3d rpy = AS.Barrel_Solve(PO.R_Armor);
            Fire_OK = true;
            Tracker.Solve_pitch = rpy[1];
            Tracker.Solve_yaw = rpy[2];
        }
        else {
            Tracker.Solve_pitch = Tracker.AS.Robot_msg.Controller_pitch;
            Tracker.Solve_yaw = Tracker.AS.Robot_msg.Controller_yaw;
        }
    }

    // 创建发送数据
    robot_msgs::Robot_ctrl Robot_ctrl_t;

    // 开火判断
    double fire;
    if(Fire_OK) fire = 1;
    else fire = 0;

    // 填充数据
    if(Tracker.tracker_state == TRACKING){
        Robot_ctrl_t.pitch = Tracker.Solve_pitch;
        Robot_ctrl_t.yaw = Tracker.Solve_yaw;
        Robot_ctrl_t.fire_command = fire;
        Robot_ctrl_t.target_lock = 0x31;
    }else {
        Robot_ctrl_t.pitch = Tracker.AS.Robot_msg.Controller_pitch;
        Robot_ctrl_t.yaw = Tracker.AS.Robot_msg.Controller_yaw;
        Robot_ctrl_t.fire_command = 0;
        Robot_ctrl_t.target_lock = 0x32;
    }
    Vision_pub.publish(Robot_ctrl_t);

    // 更新时间
    Outpost_Fire_time = ros::Time::now();
}

int main(int argc, char *argv[]){

    setlocale(LC_ALL,"");

    // 初始化节点
    ros::init(argc, argv, "Robot_Detection");

    // 创建句柄
    ros::NodeHandle nh;

    Vision_pub = nh.advertise<robot_msgs::Robot_ctrl>("Robot_ctrl_data",1);

    // 建立需要订阅的消息对应的订阅器
    message_filters::Subscriber<sensor_msgs::Image> HIK_Camera_sub(nh, "/HIK_Camera/image", 1);  
    message_filters::Subscriber<robot_msgs::Vision> Imu_sub(nh, "/Vision_data", 1);  

    // 同步ROS消息
    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, robot_msgs::Vision> MySyncPolicy;

    // 创建同步器对象
    message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), HIK_Camera_sub, Imu_sub);
    
    ROS_INFO("--Robot_Detection Start--");
    // 注册同步回调函数
    sync.registerCallback(boost::bind(&callback, _1, _2));


    ros::Rate rate(300);    //指定频率 | 待调整 100HZ:10ms 200HZ: 5ms 300HZ:3.3ms 500HZ:2ms
    // 经测试,在300HZ下不影响自瞄程序的运行 | 其他未测试
    // 使用时间间隔进行测试
    while (ros::ok())
    {
if(Mode != Buff){
        ROS_INFO("Auto");
        /**  反陀螺  */
        if(Mode == Auto){
            // 火控处理 
            if(!Spin_Fire_control_Init) {
                Spin_Fire_time = ros::Time::now();
                Spin_Time_accumulation = 0;
                Spin_Fire_control_Init = true;
            }
            Spin_Fire_control();
        }
        else if(Mode == Outpost){
            /**  前哨站  */    
            // 火控处理 
            if(!Outpost_Fire_control_Init) {
                Outpost_Fire_time = ros::Time::now();
                Outpost_Time_accumulation = 0;
                Outpost_Fire_control_Init = true;
            }
            Outpost_Fire_control();
        }
}else{
    ROS_INFO("Buff");

}
        

        // 休眠部分
        rate.sleep();
        ros::spinOnce();
    }
    
    ros::spin();

    return 0;

}