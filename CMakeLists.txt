cmake_minimum_required(VERSION 3.16.3)
project(RM_Detection)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

#链接OpenCV和Eigen3
find_package(OpenCV 4.5 REQUIRED)
find_package(Eigen3 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIRS})
#链接相机驱动
set(CAMERA_LIB  ${PROJECT_SOURCE_DIR}/MVS_Drive/libMVSDK.so
                ${PROJECT_SOURCE_DIR}/MVS_Drive/libMvCameraControl.so
                ${PROJECT_SOURCE_DIR}/MVS_Drive/libMvCameraControl.so.3.2.2.1
        )
###############功能包动态库#################
#set(ALL_Package)                    # !!!不能删除!!!
FILE(GLOB ALL_Package "${PROJECT_SOURCE_DIR}/Dynamic_library/*.so")
#########################################

include_directories(${PROJECT_SOURCE_DIR}/Dynamic_library/include)    #功能包头文件
include_directories(${PROJECT_SOURCE_DIR}/include)                    #公共头文件

add_executable(RM_Detection main.cpp)
target_link_libraries(RM_Detection ${ALL_Package})   #链接功能包动态库
target_link_libraries(RM_Detection ${OpenCV_LIBS} ${CAMERA_LIB}) #链接相机驱动和OpenCV
#git