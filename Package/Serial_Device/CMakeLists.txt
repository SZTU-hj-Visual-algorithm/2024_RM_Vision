cmake_minimum_required(VERSION 3.16.3)
project(Serial_Device)
#CMAKE_ARCHIVE_OUTPUT_DIRECTORY: 默认存放静态库的文件夹位置;
#CMAKE_LIBRARY_OUTPUT_DIRECTORY: 默认存放动态库的文件夹位置;
# =======================串口封装库=========================

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
#链接OpenCV和Eigen3
find_package(OpenCV 4.5 REQUIRED)
find_package(Eigen3 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIRS})
find_library(CAMERA_LIB libMvCameraControl.so libMvCameraControl.so.3.1.3.0 ${PROJECT_SOURCE_DIR}/../../MVS_Drive/)


#设置头文件位置
include_directories(${PROJECT_SOURCE_DIR}/../../Dynamic_library/include)    #功能包内部头文件
include_directories(${PROJECT_SOURCE_DIR}/../../include)                    #公共头文件
include_directories(${PROJECT_SOURCE_DIR}/include)                          #私有头文件

#源文件
set(SOURCE src/crc.cpp src/serial_device.cpp src/serial_main.cpp)

#生成动态库(放置在Dynamic_library下)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../Dynamic_library/) #设置路径
add_library(Serial_Device SHARED ${SOURCE})                                     #生成动态库

#将头文件一起复制过去
file(COPY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION ${PROJECT_SOURCE_DIR}/../../Dynamic_library/include/)

#测试运行代码
add_executable(Serial_Device_test Serial_Device_test.cpp ${SOURCE})
target_link_libraries(Serial_Device_test ${CAMERA_LIB} ${OpenCV_LIBS})
target_link_libraries(Serial_Device_test Serial_Device)
