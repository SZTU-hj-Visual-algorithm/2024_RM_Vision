#! /bin/bash

###############################
# MD_Camera 功能包安装脚本
###############################


# 环境检测


# 安装部分
Dirname=$(cd $(dirname $0);pwd)
package_name=`echo "$Dirname" | grep -oP "(?<=Package/).*"`
Shell/package_install.sh $package_name