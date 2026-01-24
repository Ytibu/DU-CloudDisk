#include "MyLogger.hpp"

// 模板函数定义
template <typename... T>
void MyLogger::warn(const char *msg, const T &...args)
{
    _root.warn(format_message(msg, args...));
}

template <typename... T>
void MyLogger::debug(const char *msg, const T &...args)
{
    _root.debug(format_message(msg, args...));
}

template <typename... T>
void MyLogger::info(const char *msg, const T &...args)
{
    _root.info(format_message(msg, args...));
}

template <typename... T>
void MyLogger::error(const char *msg, const T &...args)
{
    _root.error(format_message(msg, args...));
}
