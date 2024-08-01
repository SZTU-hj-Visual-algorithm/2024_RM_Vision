cmake_minimum_required(VERSION 3.0.2)
project(communication)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

# 重定向自己编译的opencv
set(OpenCV_DIR ~/opencv/cmake)
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  message_filters
  sensor_msgs
)
# 链接Opencv和Eigen3
find_package(OpenCV 4.5 REQUIRED)
find_package(Eigen3 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIRS})
## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)

###################################
## catkin specific configuration ##
###################################
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES communication
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
  ${PROJECT_SOURCE_DIR}/../../devel/include
  ${PROJECT_SOURCE_DIR}/../public_include
  ${PROJECT_SOURCE_DIR}/Kalman/
)


# 三云台同步计算
add_executable(${PROJECT_NAME}_PTZ_Yaw_node src/PTZ_Yaw_Receive.cpp Kalman/Kalman.cpp)
# 左云台全向感知处理
add_executable(${PROJECT_NAME}_PTZ_perception_L_node src/PTZ_perception_L.cpp Kalman/Kalman.cpp)
# 右云台全向感知处理
add_executable(${PROJECT_NAME}_PTZ_perception_R_node src/PTZ_perception_R.cpp Kalman/Kalman.cpp)
# 云台数据模拟
add_executable(${PROJECT_NAME}_Test_node test/PTZ_test.cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(${PROJECT_NAME}_PTZ_Yaw_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)
target_link_libraries(${PROJECT_NAME}_PTZ_perception_L_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)
target_link_libraries(${PROJECT_NAME}_PTZ_perception_R_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)
target_link_libraries(${PROJECT_NAME}_Test_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)
