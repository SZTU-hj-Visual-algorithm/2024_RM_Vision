cmake_minimum_required(VERSION 3.16.3)
project(Armor_Observe)

# =======================预测封装库=========================

set(CMAKE_CXX_STANDARD 11)
#链接OpenCV和Eigen3
find_package(OpenCV 4.5 REQUIRED)
find_package(Eigen3 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIRS})
#链接相机驱动
find_library(CAMERA_LIB libMvCameraControl.so libMvCameraControl.so.3.1.3.0 ${PROJECT_SOURCE_DIR}/../../MVS_Drive/)

#Debug 模式
set(Debug ON)
if(DEFINED Debug)
    message("----Debug----")
    FILE(GLOB HIK_Camera "${PROJECT_SOURCE_DIR}/../HIK_Camera/src/*.cpp")
    FILE(GLOB Detection "${PROJECT_SOURCE_DIR}/../Armor_Detection/src/*.cpp")
    FILE(GLOB Solve "${PROJECT_SOURCE_DIR}/../AngleSolve/src/*.cpp")
    FILE(GLOB Track "${PROJECT_SOURCE_DIR}/../Armor_Track/src/*.cpp")
    include_directories(${PROJECT_SOURCE_DIR}/../HIK_Camera/include)
    include_directories(${PROJECT_SOURCE_DIR}/../Armor_Detection/include)
    include_directories(${PROJECT_SOURCE_DIR}/../AngleSolve/include)
    include_directories(${PROJECT_SOURCE_DIR}/../Armor_Track/include)
else()
    #设置海康相机的动态库路径
    set(HIK_Camera ${PROJECT_SOURCE_DIR}/../../Dynamic_library/libHIK_Camera.so)
    #设置识别的动态库路径
    set(Detection ${PROJECT_SOURCE_DIR}/../../Dynamic_library/libArmor_Detection.so)
    #设置坐标系的动态库路径
    set(Solve ${PROJECT_SOURCE_DIR}/../../Dynamic_library/libAngleSolve.so)
    #设置跟踪的动态库路径
    set(Track ${PROJECT_SOURCE_DIR}/../../Dynamic_library/libArmor_Track.so)
    #设置整车观测的动态库路径
    set(Observe ${PROJECT_SOURCE_DIR}/../../Dynamic_library/libArmor_Observe.so)
    #设置头文件位置
    include_directories(${PROJECT_SOURCE_DIR}/../../Dynamic_library/include)    #功能包内部头文件
endif()

include_directories(${PROJECT_SOURCE_DIR}/../../include)                    #公共头文件
include_directories(${PROJECT_SOURCE_DIR}/include)                          #私有头文件

#源文件
set(SOURCE src/Armor_Observe.cpp src/Circle_Kalman.cpp)

#生成动态库(放置在Dynamic_library下)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../Dynamic_library)
add_library(Armor_Observe SHARED ${SOURCE})

#将头文件一起复制过去
file(COPY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION ${PROJECT_SOURCE_DIR}/../../Dynamic_library/include/)

#测试运行代码
if(DEFINED Debug)
    add_executable(Armor_Observe_test Armor_Observe_test.cpp ${SOURCE} ${HIK_Camera} ${Detection} ${Solve} ${Track})
else()
    add_executable(Armor_Observe_test Armor_Observe_test.cpp)
    target_link_libraries(Armor_Observe_test ${HIK_Camera} ${Detection} ${Solve} ${Track} ${Observe})   #链接动态库
endif()
target_link_libraries(Armor_Observe_test  ${OpenCV_LIBS} ${CAMERA_LIB}) #链接相机驱动和OpenCV
target_link_libraries(Armor_Observe_test  Armor_Observe)