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

#include "Neural_Buff_Detection.h" //FIXME

// 能量机关识别与预测
#include "BuffDetection.h"
#include "BuffPrediction.h"




// 数据部分
cv::Mat src;
form Vision_data;
ros::Publisher Vision_pub;
chrono_time Time; // 自瞄时间阈值
chrono_time buff_Time;
ros::Time Buff_Time;             // Buff时间阈值

// robot_status.h里面定义开关
#ifdef Neural_Mode
NeuralArmorDetector Neural_Detector;
#else

ArmorDetector Detector;
#endif // Neural_Mode
AngleSolve AS;
ArmorTrack Tracker;
ArmorObserve AO;
BuffDetection* BD = new BuffDetection;
BuffPrediction BP;
BuffTarget Target;
std::vector<double> vdata(4);
bool buff_init = false;




void callback(const sensor_msgs::ImageConstPtr &src_msg, const robot_msgs::VisionConstPtr &Vision_msg)
{
 

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

    // 初始化四元数

    BP.AS.Init(Vision_data.data, Vision_data.quat);
    BD->AS.Init(Vision_data.data, Vision_data.quat);
    Tracker.AS.Init(Vision_data.data, Vision_data.quat);
    AO.AS.Init(Vision_data.data, Vision_data.quat);
    // 选择击打颜色
#ifdef Neural_Mode
    if (Vision_data.enemy_color == 107)
        Neural_Detector.enemy_color = RED;
    else if (Vision_data.enemy_color == 7)
        Neural_Detector.enemy_color = BLUE;
        // Neural_Detector.enemy_color = Vision_data.enemy_color;
        // Neural_Detector.enemy_color = BLUE;
#else
    if (Vision_data.enemy_color == 107)
        Detector.enemy_color = RED;
    else if (Vision_data.enemy_color == 7)
        Detector.enemy_color = BLUE;
        // Detector.enemy_color = Vision_data.enemy_color;
        // Detector.enemy_color = BLUE;
#endif // Neural_Mode
    // ROS_INFO("Color:%d", Vision_data.enemy_color);

    // 创建发送数据
    robot_msgs::Robot_ctrl Robot_ctrl_t;

 

    // 0x21 鼠标右键
  

 
    
    if (Vision_data.mode == 0x23 || Vision_data.mode == 0x21)
 
    {  

        double dt = (ros::Time::now() - Buff_Time).toSec();    // 计算时间间隔
        // ROS_INFO("dt:%f",dt);
        if(dt > 0.2) BD = new BuffDetection;
        Buff_Time = ros::Time::now(); 

       
        // TODO： 注意能量机关是击打自己颜色的！
        if (Vision_data.enemy_color == 107){
            BD->our_color = BLUE;//BLUE
        }
        else if (Vision_data.enemy_color == 7){
            BD->our_color = RED;
        }
        // std::cout<<BD.our_color<<std::endl;
        if (buff_init == false)
        {
            BP.Solve_OK = false ; 
            BD->Detection_Init();   //
            BP.Prediction_Init();
            buff_init = true;
            ROS_INFO("INIT");
         }

        // 0x23 c键    0x22 x键  BIGBUFF
        if (Vision_data.mode == 0x21)
        {
           
            BP.Buff_Mode = 1;
            BD->buff_mode = 1;
        }
        else if (Vision_data.mode == 0x23)
        {
            BP.Buff_Mode = 2;
            BD->buff_mode = 2;
            // ROS_INFO("BIG\n");
          
        }
        // buff_Time = std::chrono::high_resolution_clock::now();
         std::vector<BuffTarget> finalBuffTarget;
        finalBuffTarget = BD->Detection(src);
        if(!finalBuffTarget.empty()){
            Target  =finalBuffTarget[0];
            BP.Prediction(src, Target);
        }
        // ROS_INFO("%f",buff_dt);
  
      
 
        cv::imshow("src", src);
        cv::waitKey(1);

        // 填充数据
        if (BP.Solve_OK && abs(BP.Solve_pitch)<100 && abs(BP.Solve_yaw)<100)
        {
            Robot_ctrl_t.pitch = BP.Solve_pitch;
            Robot_ctrl_t.yaw = BP.Solve_yaw;
            Robot_ctrl_t.fire_command = BP.fire_Ok;
            Robot_ctrl_t.target_lock = 0x31;
            // std::cout<<"OK   pitch:"<< Robot_ctrl_t.pitch <<"    yaw: "<< Robot_ctrl_t.yaw <<std::endl;
        }
        else
        {
            Robot_ctrl_t.pitch = BP.AS.Robot_msg.Controller_pitch;
            Robot_ctrl_t.yaw = BP.AS.Robot_msg.Controller_yaw;
            Robot_ctrl_t.fire_command = BP.fire_Ok;
            Robot_ctrl_t.target_lock = 0x32;
            // std::cout<<"pitch:  "<< Robot_ctrl_t.pitch <<"    yaw: "<< Robot_ctrl_t.yaw <<std::endl;
        }
        
        // 发送数据
        Vision_pub.publish(Robot_ctrl_t);
    }
    else
    {
            
        buff_init = false;  //不进入程序就初始化改成flase ，这样每次进入模式都是重新初始化
    }
     
}
/**
r_max_area : 1000
r_min_area : 50

leaf_max_area : 15000   # 6-7  13000  和 8 9 m 5000
leaf_min_area : 4700   


leaf_r_max_distance : 170   #计算过 6-7m
leaf_r_min_distance : 70    # 8-9 m

r_min_hb_ratio : 0.40
r_max_hb_ratio : 0.83

r_min_hw_ratio : 0.6       # 3/26
r_max_hw_ratio : 1.15

r_max_are_ratio : 0.95      # 3/26  最大不超0.9

r_min_are_ratio : 0.56       #最小不低于 0.75

leaf_min_hw_ratio : 0.47     # 6-7  0.51   和 8 9 m 一样
leaf_max_hw_ratio : 0.63

1000 表示0.001  400->0.400
*/


int debug_br_blue_threshold = 225;
int debug_br_red_threshold = 254;
int debug_rb_red_threshold = 210;
int debug_rb_blue_threshold = 254;
int debug_r_max_area = 1000;
int debug_r_min_area = 50;
int debug_leaf_max_area = 15000;
int debug_leaf_min_area = 4700;
int debug_leaf_r_max_distance = 170;
int debug_leaf_r_min_distance = 70;

int Dubug_Dubg = 0;

int debug_r_min_hb_ratio = 400;
int debug_r_max_hb_ratio = 830;
int debug_r_min_hw_ratio = 600;
int debug_r_max_hw_ratio = 1150;
int debug_leaf_min_hw_ratio = 470;
int debug_leaf_max_hw_ratio = 630;
int debug_r_min_are_ratio = 950;
int debug_r_max_are_ratio = 560;
int debug_clear = 50;
int debug_SMALL_Pre_Angle_Gain = 23000;
int debug_Delay_Time_BIGBUFF = 412;

 

int debug_To_N = 74 ;

int debug_N = 38 ; 

void on_trackbar(int , void*)
{
     BD->br_blue_threshold = debug_br_blue_threshold;
     BD->br_red_threshold  = debug_br_red_threshold;
     BD->rb_red_threshold  = debug_rb_red_threshold;
     BD->rb_blue_threshold = debug_rb_blue_threshold;
     BD->r_max_area = debug_r_max_area;
     BD->r_min_area = debug_r_min_area;
     BD->leaf_max_area = debug_leaf_max_area;
     BD->leaf_min_area = debug_leaf_min_area;
     BD->leaf_r_max_distance = debug_leaf_r_max_distance;
     BD->leaf_r_min_distance = debug_leaf_r_min_distance;
     BD->r_min_hb_ratio = debug_r_min_hb_ratio / 1000.0f ; 
     BD->r_max_hb_ratio = debug_r_max_hb_ratio / 1000.0f ; 
     BD->r_min_hw_ratio = debug_r_min_hw_ratio / 1000.0f ; 
     BD->r_max_hw_ratio = debug_r_max_hw_ratio / 1000.0f ; 
     BD->leaf_min_hw_ratio = debug_leaf_min_hw_ratio / 1000.0f ; 
     BD->leaf_max_hw_ratio = debug_leaf_max_hw_ratio / 1000.0f ; 
     BD->r_min_are_ratio = debug_r_min_are_ratio / 1000.0f ; 
     BD->r_max_are_ratio = debug_r_max_are_ratio / 1000.0f ; 
     BP.SMALL_Pre_Angle_Gain = debug_SMALL_Pre_Angle_Gain / 1000.0f ; 
     BP.DELAY_TIME_ = -debug_Delay_Time_BIGBUFF / 1000.0f;
     BP.Dubug = Dubug_Dubg;
     BP.T0_N = debug_To_N * 1.0f ;
     BP.N = debug_N * 1.0f ;
    BP.clear = debug_clear;
    }



int main(int argc, char *argv[])
{

    setlocale(LC_ALL, "");

    cv::namedWindow("Debug_Parameter",cv::WINDOW_NORMAL);
 
    cv::namedWindow("Prediction",cv::WINDOW_NORMAL);
    cv::createTrackbar("br_blue_threshold" , "Debug_Parameter" , &debug_br_blue_threshold , 255 , on_trackbar);
    cv::createTrackbar("br_red_threshold" , "Debug_Parameter" , &debug_br_red_threshold , 255 , on_trackbar);
    cv::createTrackbar("rb_red_threshold" , "Debug_Parameter" , &debug_rb_red_threshold , 255 , on_trackbar);
    cv::createTrackbar("rb_blue_threshold" , "Debug_Parameter" , &debug_rb_blue_threshold , 255 , on_trackbar);
    cv::createTrackbar("r_max_area" , "Debug_Parameter" , &debug_r_max_area , 2000 , on_trackbar);
    cv::createTrackbar("r_min_area" , "Debug_Parameter" , &debug_r_min_area , 500 , on_trackbar);
    cv::createTrackbar("leaf_max_area" , "Debug_Parameter" , &debug_leaf_max_area , 25000 , on_trackbar);
    cv::createTrackbar("leaf_min_area" , "Debug_Parameter" , &debug_leaf_min_area , 25000 , on_trackbar);
    cv::createTrackbar("leaf_r_max_distance" , "Debug_Parameter" , &debug_leaf_r_max_distance , 350 , on_trackbar);
    cv::createTrackbar("leaf_r_min_distance" , "Debug_Parameter" , &debug_leaf_r_min_distance , 350 , on_trackbar);

    cv::createTrackbar("r_min_hb_ratio" , "Debug_Parameter" , &debug_r_min_hb_ratio , 1500 , on_trackbar);
    cv::createTrackbar("r_max_hb_ratio" , "Debug_Parameter" , &debug_r_max_hb_ratio , 1500 , on_trackbar);
    cv::createTrackbar("r_min_hw_ratio" , "Debug_Parameter" , &debug_r_min_hw_ratio , 1500 , on_trackbar);
    cv::createTrackbar("r_max_hw_ratio" , "Debug_Parameter" , &debug_r_max_hw_ratio , 1500 , on_trackbar);
    cv::createTrackbar("r_min_are_ratio" , "Debug_Parameter" , &debug_r_min_are_ratio , 1500 , on_trackbar);
    cv::createTrackbar("r_max_are_ratio" , "Debug_Parameter" , &debug_r_max_are_ratio , 1500 , on_trackbar);
    cv::createTrackbar("leaf_min_hw_ratio" , "Debug_Parameter" , &debug_leaf_min_hw_ratio , 1500 , on_trackbar);
    cv::createTrackbar("leaf_max_hw_ratio" , "Debug_Parameter" , &debug_leaf_max_hw_ratio , 1500 , on_trackbar);

    cv::createTrackbar("SMALL_Pre_Angle_Gain" , "Prediction" , &debug_SMALL_Pre_Angle_Gain , 74000 , on_trackbar);
    cv::createTrackbar("Delay_Time_BIGBUFF" , "Prediction" , &debug_Delay_Time_BIGBUFF , 1000 , on_trackbar);
    cv::createTrackbar("Dubug_Dubg" , "Prediction" , &Dubug_Dubg , 1 , on_trackbar);
    cv::createTrackbar("debug_To_N" , "Prediction" , &debug_To_N , 200 , on_trackbar);
    cv::createTrackbar("debug_N" , "Prediction" , &debug_N , 200 , on_trackbar);
    cv::createTrackbar("debug_clear" , "Prediction" , &debug_clear , 200 , on_trackbar);

   

    // 初始化节点
    ros::init(argc, argv, "Buff_Dubug");

    // 创建句柄
    ros::NodeHandle nh;

    Vision_pub = nh.advertise<robot_msgs::Robot_ctrl>("Robot_ctrl_data", 1);

    //建立需要订阅的消息对应的订阅器
    message_filters::Subscriber<sensor_msgs::Image> HIK_Camera_sub(nh, "/HIK_Camera/image", 1);
    message_filters::Subscriber<robot_msgs::Vision> Imu_sub(nh, "/Vision_data", 1);

    // 同步ROS消息
    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, robot_msgs::Vision> MySyncPolicy;

    // 创建同步器对象
    message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), HIK_Camera_sub, Imu_sub);

    ROS_INFO("--Buff_Dubug Start--");
    // 注册同步回调函数
    sync.registerCallback(boost::bind(&callback, _1, _2));

    ros::spin();

    return 0;
}