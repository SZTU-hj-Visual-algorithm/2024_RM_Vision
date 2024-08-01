#include "ros/ros.h"
#include <ros/time.h>
#include <ros/package.h>

// Opencv 4.5.5
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Eigen
#include <Eigen/Dense>

// msg
#include "robot_msgs/usb_camera.h"
#include "std_msgs/Float32.h"

// 识别预测头文件
#include "AngleSolve.h"
#include "Armor_Track.h"
#include "Armor_detection.h"
#include "Neural_Armor_Detection.h"

// 参数部分
char Camera_name[100];
double ExposureTime;
double Gain;
double FPS;
double width;
double height;
double AUTO_WB;

float main_yaw;
void Init(){
    std::string package_path = ros::package::getPath("omni_perception");
    cv::FileStorage fs(package_path + "/USB_Camera_Param.yaml", cv::FileStorage::READ);

    ExposureTime = (double)fs["ExposureTime"];
    Gain = (double)fs["Gain"];
    FPS = (double)fs["FPS"];
    width = (double)fs["width"];
    height = (double)fs["height"];
    AUTO_WB = (double)fs["AUTO_WB"];
    strcpy(Camera_name,((std::string)fs["Camera_O"]).data());

    fs.release();
}

// 获取大yaw轴数据
void Robot_Main_Yaw(const std_msgs::Float32ConstPtr &Yaw){
    main_yaw = Yaw->data;
}

// 功能包
cv::Mat src;
ArmorDetector Detector;
AngleSolve AS;
ArmorTrack Tracker;
int main(int argc, char *argv[]){

    setlocale(LC_ALL,"");

    // 初始化节点
    ros::init(argc, argv, "USB_Camera_O");

    // 创建句柄
    ros::NodeHandle nh;
    
    ros::Publisher usb_camera_pub = nh.advertise<robot_msgs::usb_camera>("/USB_Camera/PTZ_O",1);

    // 初始化
    Init();

    // 启动USB相机
    cv::VideoCapture cap;
    cap.open(Camera_name);
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));  // 设置视频编码格式
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);                                   // 宽
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);                                 // 高
    cap.set(cv::CAP_PROP_EXPOSURE, ExposureTime);                               // 曝光
    cap.set(cv::CAP_PROP_FPS, FPS);                                             // 设置最大帧数
    cap.set(cv::CAP_PROP_GAIN, Gain);                                           // 增益
    cap.set(cv::CAP_PROP_AUTO_WB, AUTO_WB);                                     // 设置自动白平衡

    // 初始化四元数 | 改成欧拉角旋转矩阵(固定)
    // Tracker.AS.Init(Vision_data.data, Vision_data.quat);

    while (ros::ok())
    {
        std::vector<Armor> Targets;

        // 读取图片
        cap >> src;
        if (src.empty()){continue;}

        // 识别装甲板
        Targets = Detector.Detection(src);

        Tracker.AS.Init_EulerAngle(0,0,main_yaw);

        // 获取最终装甲板
        Tracker.Track(src,Targets,std::chrono::high_resolution_clock::now());
        Tracker.show();
        cv::waitKey(1);

        // 创建发送数据
        robot_msgs::usb_camera usb_camera_t;
        usb_camera_t.track_id = Tracker.tracking_id; 
        usb_camera_t.yaw = Tracker.Solve_yaw;                   // yaw轴
        usb_camera_t.score = Tracker.enemy_armor.grade;         // 装甲板分数
        // 坐标值 
        usb_camera_t.x = Tracker.enemy_armor.world_position[0];                         // 世界坐标x
        usb_camera_t.y = Tracker.enemy_armor.world_position[1];                         // 世界坐标y
        usb_camera_t.z = Tracker.enemy_armor.world_position[2];                         // 世界坐标z

        if(Tracker.tracker_state == TRACKING){
            usb_camera_t.target_lock = 0x31;                    // 跟踪状态
        }else{
            usb_camera_t.target_lock = 0x32;                    // 跟踪状态
        }

        // 发送数据
        usb_camera_pub.publish(usb_camera_t);

        ros::spinOnce();
    }

    ros::spin();
    return 0;
}