#include "Token.h"
#include "unixHeader.h"
#include "MyLogger.hpp"

#include <openssl/md5.h>

#include <iostream>

string Token::genToken() const
{
    string tmp = _salt + _username;
    unsigned char md[16] = {0};
    MD5((const unsigned char *)tmp.c_str(), tmp.size(), md);
    char fragment[3] = {0};
    string result;
    for(int i = 0; i < 16; ++i) {
        sprintf(fragment, "%02x", md[i]);
        result += fragment;
    }

    time_t secs = time(nullptr);
    struct tm  * ptm = localtime(&secs);
    char buff[15] = {0};
    sprintf(buff, "%04d%02d%02d%02d%02d%02d", 
            ptm->tm_year + 1900,
            ptm->tm_mon + 1,
            ptm->tm_mday,
            ptm->tm_hour,
            ptm->tm_min,
            ptm->tm_sec);
    // 记录token生成事件
    LOG_INFO("Generated token for user %s", _username.c_str());
    return result + buff;
}