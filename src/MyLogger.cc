#include "MyLogger.hpp"
#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <fstream>

// 日志文件流
static std::ofstream logFile;

MyLogger::MyLogger()
    : _root(log4cpp::Category::getRoot())
{
    auto ptn = new log4cpp::PatternLayout();
    ptn->setConversionPattern("%d %c [%p] - %m%n");

    /*创建控制台输出器*/
    auto appenderOut = new log4cpp::OstreamAppender("console", &std::cout);
    appenderOut->setLayout(ptn);

    /*打开日志文件 - 使用绝对路径*/
    std::string logPath = "/root/workSpace/myDev/CloudDisk/log/rollingfile.log";
    std::cerr << "Log path: " << logPath << std::endl;
    logFile.open(logPath, std::ios::app);
    if (!logFile.is_open()) {
        perror("Failed to open log file");
        std::cerr << "Failed to open log file" << std::endl;
    } else {
        std::cerr << "Log file opened successfully" << std::endl;
    }

    /*设置日志级别并添加输出器*/
    _root.setPriority(log4cpp::Priority::DEBUG);
    _root.addAppender(appenderOut);
}

MyLogger::~MyLogger()
{
    // 关闭日志文件
    if (logFile.is_open()) {
        logFile.close();
    }
    log4cpp::Category::shutdown();
}

// 使用C++11局部静态变量方式实现线程安全的单例
MyLogger &MyLogger::getInstance()
{
    static MyLogger instance;
    return instance;
}
