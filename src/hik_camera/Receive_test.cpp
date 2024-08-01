#include "ros/ros.h"
#include <opencv2/opencv.hpp>

#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/CompressedImage.h>

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  try
  {
    // 展示图片
    ROS_INFO("RX_YES");
    // cv::Mat image = cv::imdecode(cv::Mat(msg->data), cv::IMREAD_COLOR);
    // cv::imshow("view",image);
    cv::Mat image = cv_bridge::toCvShare(msg, "bgr8")->image;
    cv::imshow("HIK_Camera_Receive", image);
    cv::waitKey(1);

  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}


int main(int argc, char *argv[]) {

    // 初始化节点
    ros::init(argc, argv, "image_listener");
    // 创建句柄
    ros::NodeHandle nh;

    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("HIK_Camera/image", 1, imageCallback);
    
    ros::spin();
    return 0;
}