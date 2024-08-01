cmake_minimum_required(VERSION 3.0.2)
project(robot_detection)

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
  message_filters
  sensor_msgs
  roslib
)

# 链接Opencv和Eigen3
find_package(OpenCV 4.5 REQUIRED)
find_package(Eigen3 REQUIRED)
## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES robot_detection
 CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
include_directories(
# include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/../public_include
  ${PROJECT_SOURCE_DIR}/../../devel/include
  # openvino头文件
  /opt/intel/openvino/runtime/include
  /opt/intel/openvino/runtime/include/ie
  /opt/intel/openvino/runtime/include/ngraph
)


# AngleSolve功能包
include_directories(${PROJECT_SOURCE_DIR}/src/AngleSolve/include)
FILE(GLOB AngleSolve ${PROJECT_SOURCE_DIR}/src/AngleSolve/src/*.cpp)

# Neural_Armor_Detection功能包
include_directories(${PROJECT_SOURCE_DIR}/src/Neural_Armor_Detection/include)
FILE(GLOB Neural_Armor_Detection ${PROJECT_SOURCE_DIR}/src/Neural_Armor_Detection/src/*.cpp)

# Armor_Detection功能包
include_directories(${PROJECT_SOURCE_DIR}/src/Armor_Detection/include)
FILE(GLOB Armor_Detection ${PROJECT_SOURCE_DIR}/src/Armor_Detection/src/*.cpp)

# Armor_Observe功能包
include_directories(${PROJECT_SOURCE_DIR}/src/Armor_Observe/include)
FILE(GLOB Armor_Observe ${PROJECT_SOURCE_DIR}/src/Armor_Observe/src/*.cpp)

# Armor_Track功能包
include_directories(${PROJECT_SOURCE_DIR}/src/Armor_Track/include)
FILE(GLOB Armor_Track ${PROJECT_SOURCE_DIR}/src/Armor_Track/src/*.cpp)

# Outpost_Observe功能包
include_directories(${PROJECT_SOURCE_DIR}/src/Outpost_Observe/include)
FILE(GLOB Outpost_Observe ${PROJECT_SOURCE_DIR}/src/Outpost_Observe/src/*.cpp)

add_executable(${PROJECT_NAME}_node Robot_Detection.cpp
  ${AngleSolve}
  ${Armor_Detection}
  ${Neural_Armor_Detection}
  ${Armor_Observe}
  ${Armor_Track}
  ${Outpost_Observe}
)

## Specify libraries to link a library or executable target against
target_link_libraries(${PROJECT_NAME}_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  /opt/intel/openvino/runtime/lib/intel64/libopenvino.so
)
