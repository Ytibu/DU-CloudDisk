#include "Hash.h"
#include "MyLogger.hpp"
#include <iostream>
using std::cout;
using std::endl;

int main()
{
    Hash hash("Makefile");
    //cout << hash.sha1() << " Makefile " << endl;
    LOG_INFO("Hash of Makefile: %s", hash.sha1().c_str());
    return 0;
}