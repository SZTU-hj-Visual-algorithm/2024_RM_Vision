#! /bin/bash

# RM_Detection 初始化脚本
# Builder: steins_xin
# Version: 1.0
# Function: 初始化目录结构和功能包配置

# 读取配置文件
Init_OK=`cat .Init.cfg | grep "Init" | awk -F ":" '{print $2}'`
if [ $Init_OK = 'ON' ];then
    echo -e "\033[33mWarning: 所有的配置将会清空!\033[0m"
    printf "\033[33m是否要继续操作 [y/n] \033[0m"
    read -p "" Reset
fi
if [ ! $Reset = 'y' ];then
    exit 0
fi

clear
echo ""
echo -e "\033[35m############################################\033[0m"
echo -e "\033[35m|            RM_Detection Init             |\033[0m"
echo -e "\033[35m|-Builder: steins_xin                      |\033[0m"
echo -e "\033[35m|-Date: 2024/11/3                          |\033[0m"
echo -e "\033[35m|-Version: 1.0                             |\033[0m"
echo -e "\033[35m############################################\033[0m"
echo ""
sleep 0.5

######################
# 目录结构完整性检测
######################
integrity=True
# 动态库目录
if [ ! -d Dynamic_library ];then
    echo -e "\033[1;31mError: Dynamic_library目录丢失!!! \033[0m"
    integrity=False
fi
# 头文件目录
if [ ! -d include ];then
    echo -e "\033[1;31mError: include目录丢失!!! \033[0m"
    integrity=False
fi
# 相机驱动目录
if [ ! -d MVS_Drive ];then
    echo -e "\033[1;31mError: MVS_Drive目录丢失!!! \033[0m"
    integrity=False
fi
# 功能包目录
if [ ! -d Package ];then
    echo -e "\033[1;31mError: Package目录丢失!!! \033[0m"
    integrity=False
fi
# 参数文件目录
if [ ! -d SetParam_File ];then
    echo -e "\033[1;31mError: SetParam_File目录丢失!!! \033[0m"
    integrity=False
fi
# CMakeLists文件
if [ ! -f CMakeLists.txt ];then
    echo -e "\033[1;31mError: CMakeLists.txt丢失!!! \033[0m"
    integrity=False
fi
# 主函数文件
if [ ! -f main.cpp ];then
    echo -e "\033[1;31mError: main.cpp丢失!!! \033[0m"
    integrity=False
fi

if [ "${integrity}" = "False" ];then
    exit -1
fi

######################
# 环境检测             |将错误信息重定向到.error.txt文件中
######################
# opencv4 检测
opencv4=`pkg-config --modversion opencv4 2>.error.txt`
if [ "${opencv4}" = "" ];then
    echo -e "\033[1;31mError: 未检测到opencv4!!! \033[0m"
    integrity=False
fi
# eigen3 检测
eigen3=`pkg-config --modversion eigen3 2>.error.txt`
if [ "${eigen3}" = "" ];then
    echo -e "\033[1;31mError: 未检测到eigen3!!! \033[0m"
    integrity=False
fi
# cmake 检测
cmake=`cmake --version 2>.error.txt | grep "version" | awk '{print $3}'` 
if [ "${cmake}" = "" ];then
    echo -e "\033[1;31mError: 未检测到cmake!!! \033[0m"
    integrity=False
fi

if [ "${integrity}" = "False" ];then
    exit -1
else
    echo -e "\033[1;32m---环境检测---\033[0m"
fi

# 输出版本
printf "\033[1;35m%-8s --Version: %s\033[0m\n" OpenCV4 ${opencv4}
printf "\033[1;35m%-8s --Version: %s\033[0m\n" Eigen3 ${eigen3}
printf "\033[1;35m%-8s --Version: %s\033[0m\n\n" Cmake ${cmake}


######################
# 配置功能包信息
######################
echo -e "\033[1;32m---功能包配置---\033[0m"
# 获取功能包数量
Dir=`ls Package`
Package_number=0
Package_array=()
for i in ${Dir}
do
    package_path=Package/${i}
    if [ -d ${package_path} ];then
        ((Package_number++))
        Package_array[$Package_number]=${i}
    fi
done

# 写入功能包状态
> .package.cfg                  # 初始化文件内容
for ((i=1;i<=$Package_number;i++))
do
    echo "[$i]:${Package_array[$i]}:Not Install" >> .package.cfg
done

######################
# 功能包完整性检测
######################

for ((i=1;i<=$Package_number;i++));do
    # 头文件目录检测
    if [ ! -d Package/${Package_array[$i]}/include ];then
        echo -e "\033[1;33mWarning: [${Package_array[$i]}]功能包未检测到include目录\033[0m"
    fi
    # 源文件目录检测
    if [ ! -d Package/${Package_array[$i]}/src ];then
        echo -e "\033[1;33mWarning: [${Package_array[$i]}]功能包未检测到src目录\033[0m"
    fi
    # CMakeLists.txt文件检测
    if [ ! -f Package/${Package_array[$i]}/CMakeLists.txt ];then
        echo -e "\033[1;33mWarning: [${Package_array[$i]}]功能包未检测到CMakeLists.txt\033[0m"
    fi
done
printf "\033[1;35m%-8s --Number: %d\033[0m\n\n" "Package" ${Package_number}

######################
# 清除功能包遗留配置文件
######################
rm Dynamic_library/*.so 2>.error.txt             # 清除动态库
rm -r Dynamic_library/include/* 2>.error.txt     # 清除头文件


######################
# 修改 robot_status.h 路径
######################

echo -e "\033[1;32m---修改PATH路径---\033[0m"
if [ ! -f include/robot_status.h ];then
	echo -e "\033[1;31mError: robot_status.h 文件丢失!!!\033[0m"
	exit -1
fi

rows=`cat include/robot_status.h | grep -n "#define PATH" | awk -F ":" '{print $1}'`
path=`pwd`
new_path="#define PATH \""${path}"/SetParam_File/\""    # 拼接新路径
sed -i "${rows}c ${new_path}" include/robot_status.h
echo -e "\033[1;35mPATH: ${path}\033[0m"

printf "\n\033[1;32m#! RM_Detection Init Successfully!\033[0m\n\n"

# 初始化标志
Init="Init"
sed -i '1c Init:ON' .Init.cfg