#! /bin/bash

###############################
# Serial_Device 功能包安装脚本
###############################


# 环境检测

# 规则文件检测并自动安装
if [ ! -f /etc/udev/rules.d/robomaster_usb.rules ];then
    echo -e "\033[33mWarning: 未检测到robomaster_usb.rules!\033[0m"
    printf "\033[33m是否要安装robomaster_usb.rules [y/n] \033[0m"
    read -p "" usb_install
fi  
# 安装规则文件
if [ "$usb_install" = 'y' ];then
    sudo cp robomaster_usb.rules /etc/udev/rules.d/
fi

# 安装部分
Dirname=$(cd $(dirname $0);pwd)
package_name=`echo "$Dirname" | grep -oP "(?<=Package/).*"`
Shell/package_install.sh $package_name