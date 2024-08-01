#! /bin/bash

###############################
# RM_Detection 功能包安装脚本
# Builder: steins_xin
# Version: 1.0 
# Function: 选择安装功能包
###############################

# 读取配置文件
Init_OK=`cat .Init.cfg | grep "Init" | awk -F ":" '{print $2}'`
if [ $Init_OK = 'OFF' ];then
    echo -e "\033[31mError: 请先运行Init.sh进行初始化!\033[0m"
    exit -1
fi

clear
echo ""
echo -e "\033[35m############################################\033[0m"
echo -e "\033[35m|        RM_Detection Package Install      |\033[0m"
echo -e "\033[35m|-Builder: steins_xin                      |\033[0m"
echo -e "\033[35m|-Date: 2024/11/3                          |\033[0m"
echo -e "\033[35m|-Version: 1.0                             |\033[0m"
echo -e "\033[35m############################################\033[0m"
echo ""
sleep 0.1

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

# 输出功能包状态
OLDIFS=$IFS                         # 保存原始状态
IFS=$'\n'                           # 设置以\n为分隔符
for i in $(cat .package.cfg);do
    package_Serial_number=`echo $i | awk -F ":" '{print $1}'`
    package_name=`echo $i | awk -F ":" '{print $2}'`_Package
    package_state=`echo $i | awk -F ":" '{print $3}'`
    if [ ${package_state} = "Not Install" ];then
        printf "\033[32m%-4s\033[0m \033[36m%-35s\033[0m \033[31m:%-10s\033[0m \n" ${package_Serial_number} ${package_name} ${package_state}
    else
        printf "\033[32m%-4s\033[0m \033[36m%-35s\033[0m \033[32m:%-10s\033[0m \n" ${package_Serial_number} ${package_name} ${package_state}
    fi
 
done
IFS=$OLDIFS

printf "\n\033[35m[Ctrl+C 退出] Or 选择安装功能包: \033[0m"
read -p "" Serial_number

# 判断是否为[1~Package_number]
if ! echo "$Serial_number" | grep -q "^[1-${Package_number}]$";then
    echo -e "\033[1;31mError: 非法输入! 请输入正确的序号! \033[0m"
    exit -1
fi

# 获取选中功能包的安装信息
Serial=[$Serial_number]
choose_package=`grep "${Serial}" .package.cfg`
choose_package_name=`echo ${choose_package} | awk -F ":" '{print $2}'`
choose_package_state=`echo ${choose_package} | awk -F ":" '{print $3}'`


# 判断是否可以安装
if [ "${choose_package_state}" = "Not Install" ];then
    echo -ne "\033[1;35m是否要安装\033[0m \033[1;36m[${choose_package_name}]\033[0m \033[1;35m功能包? [y/n] \033[0m"
else
    echo -ne "\033[1;36m[${choose_package_name}]\033[0m \033[1;35m功能包已安装,是否重新安装? [y/n] \033[0m"
fi

read -p "" install_OK
echo ""
if [ $install_OK = "y" ];then
    if [ -f Package/"${choose_package_name}"/.setup.sh ];then
        Package/"${choose_package_name}"/.setup.sh
    else
        Shell/package_install.sh ${choose_package_name}
    fi
fi