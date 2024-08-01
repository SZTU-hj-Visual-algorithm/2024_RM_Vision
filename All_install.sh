#! /bin/bash


Install(){
    Package_Name=$1
    echo -e "\033[32m[$Package_Name] Install...\033[0m"
    if [ -f Package/$Package_Name/.setup.sh ];then
        Package/$Package_Name/.setup.sh
    else
        Shell/package_install.sh $Package_Name
    fi
}   

echo -e "\033[35m一键安装...\033[0m"
Install "HIK_Camera"
Install "AngleSolve"
Install "Armor_Detection"
Install "MD_Camera"
Install "Serial_Device"
Install "Armor_Track"
Install "Armor_Observe"