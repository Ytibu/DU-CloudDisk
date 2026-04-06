#include "CharsetUtil.h"
#include <iconv.h>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace CharsetUtil {
    static std::string convert(const std::string& input, const char* from_charset, const char* to_charset) {
        iconv_t cd = iconv_open(to_charset, from_charset);
        if (cd == (iconv_t)-1) {
            throw std::runtime_error("iconv_open failed");
        }
        size_t inlen = input.size();
        size_t outlen = inlen * 4 + 1;
        std::vector<char> outbuf(outlen);
        char* inptr = const_cast<char*>(input.data());
        char* outptr = outbuf.data();
        size_t outbytesleft = outlen;
        size_t inbytesleft = inlen;
        memset(outbuf.data(), 0, outlen);
        if (iconv(cd, &inptr, &inbytesleft, &outptr, &outbytesleft) == (size_t)-1) {
            iconv_close(cd);
            throw std::runtime_error("iconv conversion failed");
        }
        std::string result(outbuf.data());
        iconv_close(cd);
        return result;
    }

    std::string gbk_to_utf8(const std::string& gbk) {
        return convert(gbk, "GBK", "UTF-8");
    }
    std::string utf8_to_gbk(const std::string& utf8) {
        return convert(utf8, "UTF-8", "GBK");
    }
}
