#include "MyLogger.hpp"
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <stdexcept>
#include <iostream>

MyLogger::MyLogger()
    : _root(log4cpp::Category::getRoot())
{
    try {
        /*从配置文件加载 log4cpp 配置*/
        std::string configFile = "conf/log4cpp.properties";
        log4cpp::PropertyConfigurator::configure(configFile);
        std::cerr << "log4cpp 配置文件加载成功: " << configFile << std::endl;
    } catch (log4cpp::ConfigureFailure& e) {
        std::cerr << "log4cpp 配置失败: " << e.what() << std::endl;
        throw std::runtime_error("Failed to configure log4cpp");
    }
}

MyLogger::~MyLogger()
{
    log4cpp::Category::shutdown();
}

// 使用C++11局部静态变量方式实现线程安全的单例
MyLogger &MyLogger::getInstance()
{
    static MyLogger instance;
    return instance;
}
