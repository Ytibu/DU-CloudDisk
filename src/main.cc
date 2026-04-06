#include "CloudDiskServer.h"
#include <iostream>

int main()
{
    CloudDiskServer cloudDiskServer(1);
    cloudDiskServer.loadModules();
    cloudDiskServer.start(1234);
    return 0;
}

