#! /bin/bash

###############################
#   HIK_Camera 功能包安装脚本
###############################


# 环境检测
profile=True
if [ ! -f SetParam_File/HIKCamera_Param.yaml ];then
	echo -e "\033[31mError: SetParam_File/HIKCamera_Param.yaml 丢失!!!"
	profile=False
fi

if [ ! -d /opt/MVS ];then
    echo -e "\033[33mWarning: 未安装MVS!\033[0m"
    echo -e "\033[33m可能导致 HIK_Camera 无法正常运行!\033[0m"
fi

if [ $profile = "False" ];then
	exit -1
fi

# 修改 HIKCamera_Param.yaml 内 录制视频路径
Route=`pwd`
path=`cat SetParam_File/HIKCamera_Param.yaml | grep -o "Video_path"`
if [ ! -d Dynamic_library/Video ];then
	mkdir Dynamic_library/Video
fi
Video_path=$Route/Dynamic_library/Video/HIK_Camera.avi
rows=`cat SetParam_File/HIKCamera_Param.yaml | grep -n "Video_path" | awk -F ":" '{print $1}'`
sed -i ''$rows'c Video_path: '$Video_path'' SetParam_File/HIKCamera_Param.yaml


# 安装部分
Dirname=$(cd $(dirname $0);pwd)
package_name=`echo "$Dirname" | grep -oP "(?<=Package/).*"`
Shell/package_install.sh $package_name