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
  roslib
)


# 链接Opencv和Eigen3
find_package(OpenCV 4.5 REQUIRED)
find_package(Eigen3 REQUIRED)
###################################
## catkin specific configuration ##
###################################
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES Serial_Device
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
  ${PROJECT_SOURCE_DIR}/../../devel/include
)

## Declare a C++ executable
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide
set(SRC_DIR src/crc.cpp src/serial_device.cpp src/serial_main.cpp)

# # 左C板
add_executable(${PROJECT_NAME}_Send_L_node Serial_Device_Send_L_node.cpp ${SRC_DIR})
# # 右C板
# add_executable(${PROJECT_NAME}_Send_R_node Serial_Device_Send_R_node.cpp ${SRC_DIR})


## Specify libraries to link a library or executable target against
# # 左板
target_link_libraries(${PROJECT_NAME}_Send_L_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)
# # 右板
# target_link_libraries(${PROJECT_NAME}_Send_R_node
#   ${catkin_LIBRARIES}
#   ${OpenCV_LIBS}
# )