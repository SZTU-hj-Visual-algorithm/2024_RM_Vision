#! /bin/bash

###############################
# Package Install 通用编译部分
# Builder: steins_xin
# Version: 1.0 
# Function: 编译功能包
###############################

Package_Name=$1
# 查看功能包状态
Package_state=`cat .package.cfg | grep "${Package_Name}" | awk -F ":" '{print $3}'`

# 清除build文件夹
if [ -d Package/${Package_Name}/build ];then
    rm -r Package/${Package_Name}/build
fi

#########################
#       Cmake 部分
#########################

# 开始构建build
mkdir Package/${Package_Name}/build
cmake_build=`cmake -S Package/${Package_Name}/ -B Package/${Package_Name}/build/ 2>.error.txt`
cmake_error=`cat .error.txt | grep -i "Error"`      # 错误信息日志

# 输出错误日志
if [ ! "${cmake_error}" = "" ];then
    OLDIFS=$IFS                         # 保存原始状态
    IFS=$'\n'                           # 设置以\n为分隔符
    for i in `cat .error.txt`;do
        if [ ! "`echo "$i" | grep -i "Error"`" = "" ];then
            echo -e "\033[1;31m$i\033[0m"
        else
            echo "$i"
        fi
    done
    printf "\n\033[1;31mError: Cmake Build fail!!!\033[0m\n\n"
    IFS=$OLDIFS                         # 恢复状态
    exit -1
fi

#########################
#       make 部分
#########################

# 初始化.error.txt
> .error.txt
# 子线程监控进度
{
    echo -ne "\e[?25l"                  # 隐藏光标
    echo -e "\033[1;35m|- Start Installation...\033[0m"
    Shell/install_progress.sh
    echo -ne "\e[?25h"                  # 恢复光标
} &

# make编译
make_build=`cmake --build Package/${Package_Name}/build/ 1>.package_make.log 2>.error.txt `
# cmake --build Package/${Package_Name}/build/ 2>.error.txt
make_error=`cat .error.txt | grep -i "Error"`


# 输出错误日志
if [ ! "${make_error}" = "" ];then
    OLDIFS=$IFS                         # 保存原始状态
    IFS=$'\n'                           # 设置以\n为分隔符
    for i in `cat .error.txt`;do
        if [ ! "`echo "$i" | grep -i "Error"`" = "" ];then
            echo -e "\033[1;31m$i\033[0m"
        elif [ ! "`echo "$i" | grep -i "|"`" = "" ];then
            echo -e "\033[1;31m$i\033[0m"
        else
            echo "$i"
        fi
    done
    printf "\n\033[1;31mError: make Build fail!!!\033[0m\n\n"
    IFS=$OLDIFS                         # 恢复状态
    exit -1
else
    echo -e "\033[1;32mInstall Successful!!!\033[0m"
    echo ""
fi

# 修改功能包安装状态
rows=`cat .package.cfg | grep -n "${Package_Name}" | awk -F ":" '{print $1}'`
sed -i ''$rows'c ['$rows']:'${Package_Name}':Install' .package.cfg
