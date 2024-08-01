#include "ros/ros.h"

// HIKCamera
#include "HIK_Camera.h"
#include "robot_status.h"

// 图像传输
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/CompressedImage.h>

int main(int argc, char *argv[]){

    // 初始化节点
    ros::init(argc, argv, "L_image_publisher");

    // 创建句柄
    ros::NodeHandle nh;
    
    // 发布图片
    image_transport::ImageTransport it(nh);
    image_transport::Publisher pub = it.advertise("/HIK_Camera_L/image", 1);

    HIK_Camera HIK;
    char Name[20] = {"Steins_Xin_L"};   // 相机自定义名称
    //相机初始化
    //if(HIK.HIKCamera_cUsername_Init(Name)){
    if(HIK.HIKCamera_Init()){
        HIK.HIKCamera_SetParam(Robot::HIK_ON);      // 设置相机参数(自动白平衡) | ON:自动白平衡  OFF:手动白平衡
        HIK.HIKCamera_startGrabbing();              // 相机开始取流
        HIK.HIKCamera_outParam();                   // 相机输出参数
        HIK.HIKCamera_FPS();                        // 相机实际帧率
        ROS_INFO("HIK_L_YES");
        while (HIK.Camera_OK){
            HIK.HIKCamera_read();           //读取相机图像
            cv::imshow("src_L", HIK.src);

            // 填充数据
            cv_bridge::CvImage img_msg;
            img_msg.image = HIK.src.clone();
            img_msg.header.stamp = ros::Time::now();
            img_msg.header.frame_id = "HIK_Camera_L";
            img_msg.encoding = sensor_msgs::image_encodings::BGR8;

            // sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", img).toImageMsg();
            sensor_msgs::ImagePtr msg = img_msg.toImageMsg();

            pub.publish(msg);               // 发送数据
            ros::spinOnce();                // ROS消息回调处理函数

            HIK.Bayer.release();            //释放资源
        }
        HIK.HIKCamera_close();           //关闭相机
    }

    return 0;
}