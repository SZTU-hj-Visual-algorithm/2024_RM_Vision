cmake_minimum_required(VERSION 3.16.3)
project(HIK_SDK)
#CMAKE_ARCHIVE_OUTPUT_DIRECTORY: 默认存放静态库的文件夹位置;
#CMAKE_LIBRARY_OUTPUT_DIRECTORY: 默认存放动态库的文件夹位置;
# =======================相机封装库=========================

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
#链接OpenCV和Eigen3
find_package(OpenCV 4.5 REQUIRED)
find_package(Eigen3 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIRS})

#设置头文件位置
include_directories(${PROJECT_SOURCE_DIR}/../../include)                    #公共头文件
include_directories(${PROJECT_SOURCE_DIR}/include)                          #私有头文件

#源文件
set(SOURCE src/HIK_Camera.cpp)

#生成动态库(放置在Dynamic_library下)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../Dynamic_library/) #设置路径
add_library(HIK_Camera SHARED ${SOURCE})                                         #生成动态库

#将头文件一起复制过去
file(COPY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION ${PROJECT_SOURCE_DIR}/../../Dynamic_library/include/)
