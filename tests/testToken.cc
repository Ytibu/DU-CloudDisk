#include "Token.h"
#include "MyLogger.hpp"
#include <iostream>
using std::cout;
using std::endl;

int main()
{
    Token token("Jackie", "12345678");
    //cout << "token:" << token.genToken() << endl;
    LOG_INFO("token: %s", token.genToken().c_str());
    return 0;
}