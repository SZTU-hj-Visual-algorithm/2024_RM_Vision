//
// Created by hj on 2024/3/20.
//
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <unistd.h>   // 用于获取当前工作目录路径
#include <vector>

class Logger {
private:
    std::ofstream outputFile;

public:
    Logger() {

        // 构造 "log" 文件夹路径
        std::string logDir = "/home/hj/RM_Detection_ROS/src/robot_detection/src/Buff_Detection/buff_logs";

        // 获取当前日期时间
        std::string dateTime = getCurrentDateTime();

        // 构造文件名
        std::string filename = logDir + "/" + dateTime + ".txt";

        // 打开文件
        outputFile.open(filename);

        if (!outputFile.is_open()) {
            std::cerr << "无法打开文件：" << filename << std::endl;
            return;
        }
    }

    ~Logger() {
        // 关闭文件
        outputFile.close();
    }

    // 获取当前日期时间并格式化为字符串
    std::string getCurrentDateTime() {
        time_t now = time(0);
        struct tm timeinfo;
        char buffer[80];
        localtime_r(&now, &timeinfo);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &timeinfo);
        return std::string(buffer);
    }

    // 写入数据到文件
    void logData(const std::string& data) {
        if (outputFile.is_open()) {
            outputFile << data << std::endl;
        } else {
            std::cerr << "文件未打开。" << std::endl;
        }
    }
};



