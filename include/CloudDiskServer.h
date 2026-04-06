#ifndef __WD_CloudDiskServer_HPP__ 
#define __WD_CloudDiskServer_HPP__ 

#include <workflow/WFFacilities.h>
#include <wfrest/HttpServer.h>

class CloudDiskServer
{
public:
    CloudDiskServer(int cnt) 
    : _waitGroup(cnt)
    {}

    ~CloudDiskServer() {}

    void start(unsigned short port);

    void loadModules();

private:
    //模块化的思维方式写代码
    void loadStaticResourceModule();
    void loadUserRegisterModule();
    void loadUserLoginModule();
    void loadUserInfoModule();
    void loadFileQueryModule();
    void loadFileUploadModule();
    void loadFileDownloadModule();

private:
    WFFacilities::WaitGroup _waitGroup;
    wfrest::HttpServer _httpserver;
};

#endif

