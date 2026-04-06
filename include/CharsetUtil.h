#ifndef __WD_CharsetUtil_HPP__
#define __WD_CharsetUtil_HPP__
#include <string>

namespace CharsetUtil {
    // 将GBK编码字符串转为UTF-8
    std::string gbk_to_utf8(const std::string& gbk);
    // 将UTF-8编码字符串转为GBK
    std::string utf8_to_gbk(const std::string& utf8);
}

#endif
