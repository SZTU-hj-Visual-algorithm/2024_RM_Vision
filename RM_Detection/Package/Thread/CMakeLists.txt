cmake_minimum_required(VERSION 3.16.3)
project(Thread)
#CMAKE_ARCHIVE_OUTPUT_DIRECTORY: 默认存放静态库的文件夹位置;
#CMAKE_LIBRARY_OUTPUT_DIRECTORY: 默认存放动态库的文件夹位置;
# =======================多线程封装库=========================
set(CMAKE_CXX_STANDARD 11)
# UNIX下需要添加这个编译选项
if(UNIX)
    message("--------UNIX--------")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

set(Thread src/Thread.cpp)
#测试运行代码
add_executable(Thread_test Thread_test.cpp)