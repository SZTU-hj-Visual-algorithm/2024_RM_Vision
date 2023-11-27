#! /bin/bash

###############################
# 功能包安装进度监控脚本
# Builder: steins_xin
# Version: 1.0 
# Function: 监控功能包的安装进度
###############################

num=0                   # 异常时间计时器
package_make_rows=0     # 文件行数
#####################
# 监控文件
#####################
while true;do
    sleep 0.001           # 文件读取时间间隔
    # error 监测
    make_error=`cat .error.txt | grep -i "Error"`
    if [ ! "${make_error}" = "" ];then
        exit -1
    fi
    # 监控文件是否有数据读入
    rows=`wc -l .package_make.log | awk '{print $1}'`
    if [ ! ${rows} = ${package_make_rows} ];then
        ((package_make_rows = rows))                # 更新最大行号
        # 正则匹配出百分比
        # Percentage=`tail -n 1 .package_make.log | grep -oE "\[[' ',0-9,%]*]" | grep -o "[0-9]*"`
        Percentage=`tail -n 1 .package_make.log | grep -oP "(?<=[[]).*(?=%)"`
        if [ ! $Percentage = "" ];then
            # 计算 # 数目
            str=''
            Percentage_str=$(($Percentage / 4))
            for ((i=1;i<=$Percentage_str;i++));do
                str+='#'
            done
            if [ ! $Percentage = 100 ];then
                printf "\033[35m|- [%-25s]\033[0m\033[35m[%d%%]\033[0m \r" $str $Percentage
            else
                printf "\033[35m|- [%-25s]\033[0m\033[35m[%d%%]\033[0m \n" $str $Percentage
                exit 0
            fi
        fi 
    fi

    # 异常情况处理(2分钟之后强制退出)
    ((num++))
    if [ $num = 120000 ];then
        exit -1
    fi
done