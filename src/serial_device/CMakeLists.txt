cmake_minimum_required(VERSION 3.0.2)
project(serial_device)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

# 重定向自己编译的opencv
set(OpenCV_DIR ~/opencv/cmake)
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
)
# 链接Opencv和Eigen3
find_package(OpenCV 4.5 REQUIRED)
find_package(Eigen3 REQUIRED)


###################################
## catkin specific configuration ##
###################################
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES serial_device
 CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
)

###########
## Build ##
###########
include_directories(
  # include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/../public_include
  ${PROJECT_SOURCE_DIR}/../../devel/include
)

add_executable(${PROJECT_NAME}_send_node Serial_Device_Send_node.cpp src/crc.cpp src/serial_device.cpp src/serial_main.cpp)
add_executable(${PROJECT_NAME}_receive_node Serial_Device_Receive_node.cpp src/crc.cpp src/serial_device.cpp src/serial_main.cpp)
add_executable(${PROJECT_NAME}_receive_test_node Reveice_test.cpp)
add_executable(${PROJECT_NAME}_send_test_node Send_test.cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(${PROJECT_NAME}_send_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)
target_link_libraries(${PROJECT_NAME}_receive_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)
target_link_libraries(${PROJECT_NAME}_receive_test_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)
target_link_libraries(${PROJECT_NAME}_send_test_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)