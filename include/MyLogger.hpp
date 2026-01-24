#ifndef __MYLOGGER_H__
#define __MYLOGGER_H__

#include <log4cpp/Category.hh>
#include <string>
#include <sstream>
#include <cstdarg>
using std::string;

/**
 * 日志类
 * 单例模式：确保只有一个实例，并且提供全局访问点，使用静态方法获取实例
 * 宏定义：日志打印宏定义，方便调用
 */

inline std::string format_message(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return std::string(buffer);
}


#define LOG_WARN(format, ...) MyLogger::getInstance().warn(format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) MyLogger::getInstance().error(format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) MyLogger::getInstance().info(format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) MyLogger::getInstance().debug(format, ##__VA_ARGS__)

class MyLogger
{
public:
    // 添加接受std::string参数的重载版本，以兼容现有调用方式
    void warn(const std::string& msg) { _root.warn(msg); }
    void debug(const std::string& msg) { _root.debug(msg); }
    void info(const std::string& msg) { _root.info(msg); }
    void error(const std::string& msg) { _root.error(msg); }

    // 模板函数声明
    template <typename... T>
    void warn(const char *msg, const T &...args);
    
    template <typename... T>
    void debug(const char *msg, const T &...args);
    
    template <typename... T>
    void info(const char *msg, const T &...args);
    
    template <typename... T>
    void error(const char *msg, const T &...args);


    static MyLogger &getInstance();

private:
    MyLogger(); 
    ~MyLogger();
    MyLogger(const MyLogger &) = delete;    
    MyLogger &operator=(const MyLogger&)=delete;    

private:
    log4cpp::Category &_root;
};

#include "MyLogger.tpp"

#endif