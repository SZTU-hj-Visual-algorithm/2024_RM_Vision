cmake_minimum_required(VERSION 3.0.2)
project(hik_camera)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

# 重定向自己编译的cv_bridge
set(cv_bridge_DIR ~/catkin_ws/devel/share/cv_bridge/cmake)
# 重定向自己编译的opencv
set(OpenCV_DIR ~/opencv/cmake)
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  cv_bridge
  image_transport
  sensor_msgs
  roslib
)
# 链接Opencv和Eigen3
find_package(OpenCV 4.5 REQUIRED)
find_package(Eigen3 REQUIRED)
set(CAMERA_LIB  
    ${PROJECT_SOURCE_DIR}/MVS_Drive/libMVSDK.so
    ${PROJECT_SOURCE_DIR}/MVS_Drive/libMvCameraControl.so
    ${PROJECT_SOURCE_DIR}/MVS_Drive/libMvCameraControl.so.3.2.2.1      
)


###################################
## catkin specific configuration ##
###################################
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES hik_camera
 CATKIN_DEPENDS roscpp rospy std_msgs
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
  ${OpenCV_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/../public_include
)

# 相机发布节点
add_executable(${PROJECT_NAME}_node HIK_Camera_node.cpp src/HIK_Camera.cpp)
# 相机接受测试节点
add_executable(${PROJECT_NAME}_Receive_test_node Receive_test.cpp)


## Specify libraries to link a library or executable target against
target_link_libraries(${PROJECT_NAME}_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  ${CAMERA_LIB}
)
target_link_libraries(${PROJECT_NAME}_Receive_test_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)

