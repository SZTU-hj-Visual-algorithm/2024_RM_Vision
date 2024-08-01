cmake_minimum_required(VERSION 3.0.2)
project(omni_perception)

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
  message_filters
  sensor_msgs
  roslib
)
# 链接Opencv和Eigen3
find_package(OpenCV 4.5 REQUIRED)
find_package(Eigen3 REQUIRED)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES omni_perception
 CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
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
include_directories(${PROJECT_SOURCE_DIR}/package/AngleSolve/include)
FILE(GLOB AngleSolve ${PROJECT_SOURCE_DIR}/package/AngleSolve/src/*.cpp)

# Armor_Detection功能包
include_directories(${PROJECT_SOURCE_DIR}/package/Armor_Detection/include)
FILE(GLOB Armor_Detection ${PROJECT_SOURCE_DIR}/package/Armor_Detection/src/*.cpp)

# Armor_Track功能包
include_directories(${PROJECT_SOURCE_DIR}/package/Armor_Track/include)
FILE(GLOB Armor_Track ${PROJECT_SOURCE_DIR}/package/Armor_Track/src/*.cpp)

# Neural_Armor_Detection功能包
include_directories(${PROJECT_SOURCE_DIR}/package/Neural_Armor_Detection/include)
FILE(GLOB Neural_Armor_Detection ${PROJECT_SOURCE_DIR}/package/Neural_Armor_Detection/src/*.cpp)

add_executable(${PROJECT_NAME}_O_node src/camera_O_node.cpp
  ${AngleSolve}
  ${Armor_Detection}
  ${Armor_Track}
  ${Neural_Armor_Detection}
)
add_executable(${PROJECT_NAME}_F_node src/camera_F_node.cpp
  ${AngleSolve}
  ${Armor_Detection}
  ${Armor_Track}
  ${Neural_Armor_Detection}
)
add_executable(${PROJECT_NAME}_R_node src/camera_R_node.cpp
  ${AngleSolve}
  ${Armor_Detection}
  ${Armor_Track}
  ${Neural_Armor_Detection}
)
add_executable(${PROJECT_NAME}_L_node src/camera_L_node.cpp
  ${AngleSolve}
  ${Armor_Detection}
  ${Armor_Track}
  ${Neural_Armor_Detection}
)

add_executable(${PROJECT_NAME}_Decision_node Omnidirectional_decision.cpp

)

target_link_libraries(${PROJECT_NAME}_O_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  /opt/intel/openvino/runtime/lib/intel64/libopenvino.so
)

target_link_libraries(${PROJECT_NAME}_F_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  /opt/intel/openvino/runtime/lib/intel64/libopenvino.so
)

target_link_libraries(${PROJECT_NAME}_R_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  /opt/intel/openvino/runtime/lib/intel64/libopenvino.so
)

target_link_libraries(${PROJECT_NAME}_L_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  /opt/intel/openvino/runtime/lib/intel64/libopenvino.so
)

target_link_libraries(${PROJECT_NAME}_Decision_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)

