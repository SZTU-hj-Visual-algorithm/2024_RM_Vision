#! /bin/bash

# RM_Detection 功能包管理脚本
# Builder: steins_xin
# Version: 1.0
# Function: 查看所有功能包安装情况和详细信息

# 读取配置文件
Init_OK=`cat .Init.cfg | grep "Init" | awk -F ":" '{print $2}'`
if [ $Init_OK = 'OFF' ];then
    echo -e "\033[31mError: 请先运行Init.sh进行初始化!\033[0m"
    exit -1
fi

clear
echo ""
echo -e "\033[35m############################################\033[0m"
echo -e "\033[35m|        RM_Detection Package Mange        |\033[0m"
echo -e "\033[35m|-Builder: steins_xin                      |\033[0m"
echo -e "\033[35m|-Date: 2024/11/3                          |\033[0m"
echo -e "\033[35m|-Version: 1.0                             |\033[0m"
echo -e "\033[35m############################################\033[0m"
echo -e "\033[32m功能包列表如下:\033[0m"
sleep 0.1
#########################
# 获取功能包的详细信息
#########################
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
IFS=$OLDIFS                         # 恢复状态

#########################
# 查看功能包的详细信息
#########################

printf "\n\033[35m[Ctrl+C 退出] Or 查看功能包: \033[0m"
read -p "" Select_number

# 判断是否为[1~Package_number]
if ! echo "$Select_number" | grep -q "^[1-${Package_number}]$";then
    echo -e "\033[1;31mError: 非法输入! 请输入正确的序号! \033[0m"
    exit -1
fi

Select_package=${Package_array[$Select_number]}
if [ -f Package/$Select_package/.README.md ];then
    OLDIFS=$IFS                         # 保存原始状态
    IFS=$'\n'                           # 设置以\n为分隔符
    for i in `cat Package/$Select_package/.README.md`;do
        # 跳过注释 <!-- -->
        Note=`echo -e "$i" | grep "<!--"`
        if [ "$Note" = "" ];then
            echo -e "$i"
        fi
    done
    IFS=$OLDIFS                         # 恢复状态
else
    printf "\n\033[41m该功能包暂无详细信息\033[0m\n\n" 
fi


