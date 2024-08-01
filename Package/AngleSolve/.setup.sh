#! /bin/bash

###############################
#    AngleSolve 功能包安装脚本
###############################


# 环境检测
if [ ! -f SetParam_File/Solve_data.yaml ];then
	echo -e "\033[31mError: SetParam_File/Solve_data.yaml 丢失!!!"
	exit -1
fi


# 安装部分
Dirname=$(cd $(dirname $0);pwd)
package_name=`echo "$Dirname" | grep -oP "(?<=Package/).*"`
Shell/package_install.sh $package_name