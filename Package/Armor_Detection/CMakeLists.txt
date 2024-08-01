cmake_minimum_required(VERSION 3.16.3)
project(Armor_Detection)

# =======================装甲板识别封装库=========================

set(CMAKE_CXX_STANDARD 11)
#链接OpenCV和Eigen3
find_package(OpenCV 4.5 REQUIRED)
find_package(Eigen3 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/../../include)                    #公共头文件
include_directories(${PROJECT_SOURCE_DIR}/include)                          #私有头文件

#源文件
set(SOURCE src/Armor_detection.cpp src/Number_DNN.cpp)

#生成动态库(放置在Dynamic_library下)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../Dynamic_library)
add_library(Armor_Detection SHARED ${SOURCE})

#将头文件一起复制过去
file(COPY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION ${PROJECT_SOURCE_DIR}/../../Dynamic_library/include/)
