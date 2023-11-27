#! /bin/bash

###############################
#   Armor_Track 功能包安装脚本
###############################


# 环境检测
profile=True
if [ ! -f SetParam_File/Kalman_data.yaml ];then
	echo -e "\033[31mError: SetParam_File/Kalman_data.yaml 丢失!!!"
	profile=False
fi

if [ ! -f SetParam_File/Track_data.yaml ];then
	echo -e "\033[31mError: SetParam_File/Track_data.yaml 丢失!!!"
	profile=False
fi

# 检查AngleSolve是否安装
AngleSolve_Install=`cat .package.cfg | grep "AngleSolve" | awk -F ":" '{print $3}'`
if [ ! "$AngleSolve_Install" = "Install" ];then
	printf "\n\033[31mError: 未安装 AngleSolve!\033[0m\n"
    printf "\n\033[31m     | Armor_Track 需依赖 AngleSolve运行!\033[0m\n\n"
	profile=False
fi

if [ $profile = "False" ];then
	exit -1
fi

# 安装部分
Dirname=$(cd $(dirname $0);pwd)
package_name=`echo "$Dirname" | grep -oP "(?<=Package/).*"`
Shell/package_install.sh $package_name