#! /bin/bash

###############################
# Armor_Detection 功能包安装脚本
###############################


# 环境检测
profile=True
if [ ! -d Model ];then
	echo -e "\033[31mError: Model目录丢失!!!"
	profile=False
fi
if [ ! -f SetParam_File/Detect_data.yaml ];then
	echo -e "\033[31mError: SetParam_File/Detect_data.yaml 丢失!!!"
	profile=False
fi
if [ ! -f SetParam_File/Dnn_data.yaml ];then
	echo -e "\033[31mError: SetParam_File/Dnn_data.yaml 丢失!!!"
	profile=False
fi
if [ $profile = "False" ];then
	exit -1
fi

# 修改 Dnn_data.yaml 内 onnx路径

Route=`pwd`
ONNX=`cat SetParam_File/Dnn_data.yaml | grep -o "Model/.*.onnx"`
new_path=$Route/$ONNX

rows=`cat SetParam_File/Dnn_data.yaml | grep -n "net_path" | awk -F ":" '{print $1}'`
sed -i ''$rows'c net_path: '$new_path'' SetParam_File/Dnn_data.yaml

# 安装部分
Dirname=$(cd $(dirname $0);pwd)
package_name=`echo "$Dirname" | grep -oP "(?<=Package/).*"`
Shell/package_install.sh $package_name