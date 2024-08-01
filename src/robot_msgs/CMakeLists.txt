cmake_minimum_required(VERSION 3.0.2)
project(robot_msgs)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  message_generation
)


## Generate messages in the 'msg' folder
add_message_files(
  FILES
  vision.msg              # IMU数据
  robot_ctrl.msg          # 发送下位机数据
  PTZ_Yaw.msg             # 大云台数据
  usb_camera.msg              # USB摄像头数据
  PTZ_perception.msg      # 云台|全向感知数据融合
  Track_reset.msg         # 跟踪状态重置
  Yaw_Decision.msg        # 大Yaw决策数据
  omni_perception.msg     # 全向感知决策数据
)

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  std_msgs
)

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################
## Generate dynamic reconfigure parameters in the 'cfg' folder
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES robot_msg
 CATKIN_DEPENDS roscpp rospy std_msgs message_runtime
#  DEPENDS system_lib
)

###################################
## catkin specific configuration ##
###################################
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES robot_msg
 CATKIN_DEPENDS roscpp rospy std_msgs message_runtime
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
# include
${catkin_INCLUDE_DIRS}
)
