#include "CloudDiskServer.h"
#include "Token.h"
#include "Hash.h"
#include "unixHeader.h"
#include "MyLogger.hpp"
#include <workflow/MySQLMessage.h>
#include <workflow/MySQLResult.h>
#include <wfrest/Json.h>
#include <iostream>

#include "CharsetUtil.h"
#include <cerrno>

using namespace wfrest;
using std::string;
using std::cout;
using std::endl;
using std::vector;

/**
 * CloudDiskServer - 云盘服务器主类
 * 负责处理用户注册、登录、文件上传下载等业务逻辑
 */
void CloudDiskServer::start(unsigned short port)
{
    if(_httpserver.track().start(port) == 0) {
        _httpserver.list_routes();
        _waitGroup.wait();
        _httpserver.stop();
    } else {
        LOG_ERROR("CloudDisk Server Start Failed!");
    }
}

void CloudDiskServer::loadModules()
{
    loadStaticResourceModule();
    loadUserRegisterModule();
    loadUserLoginModule();
    loadUserInfoModule();
    loadFileQueryModule();
    loadFileUploadModule();
    loadFileDownloadModule();
}

/* ============================================================================
 * 静态资源模块
 * ============================================================================ */
void CloudDiskServer::loadStaticResourceModule()
{
    // 注册页面
    _httpserver.GET("/user/signup", [](const HttpReq *, HttpResp * resp){
        resp->File("static/view/signup.html");
    });

    // 登录页面
    _httpserver.GET("/static/view/signin.html", [](const HttpReq *, HttpResp * resp){
        resp->File("static/view/signin.html");
    });

    // 首页
    _httpserver.GET("/static/view/home.html", [](const HttpReq *, HttpResp * resp){
        resp->File("static/view/home.html");
    });

    // JavaScript 工具文件
    _httpserver.GET("/static/js/auth.js", [](const HttpReq *, HttpResp * resp){
        resp->File("static/js/auth.js");
    });

    // 用户头像
    _httpserver.GET("/static/img/avatar.jpeg", [](const HttpReq *, HttpResp * resp){
        resp->File("static/img/avatar.jpeg");
    });

    // 文件上传页面
    _httpserver.GET("/file/upload", [](const HttpReq *, HttpResp * resp){
        resp->File("static/view/index.html");
    });

    // 静态文件目录（上传文件相关）
    _httpserver.Static("/file/upload_files", "static/view/upload_files");
}

/* ============================================================================
 * 用户注册模块
 * ============================================================================ */
void CloudDiskServer::loadUserRegisterModule()
{
    _httpserver.POST("/user/signup", [](const HttpReq *req, HttpResp * resp, SeriesWork * series){
        if(req->content_type() == APPLICATION_URLENCODED) {
            // 解析请求：获取用户名和密码
            auto formKV = req->form_kv();
            string username = formKV["username"];
            string password = formKV["password"];

            LOG_INFO("username:%s, password:%s\n", username.c_str(), password.c_str());

            // 对密码进行加密
            string salt("12345678"); // TODO: 应该随机生成
            const char *encrypted = crypt(password.c_str(), salt.c_str());
            string encodedPassword(encrypted ? encrypted : "");
            LOG_INFO("encodedPassword:%s", encodedPassword.c_str());

            // 将用户信息存储到数据库 MySQL 中
            string mysqlurl("mysql://CloudDisk:cloudDisk123@127.0.0.1/CloudDisk");
            auto mysqlTask = WFTaskFactory::create_mysql_task(mysqlurl, 1,
                [resp](WFMySQLTask * mysqltask){
                    // 0. 对任务的状态进行检测
                    int state = mysqltask->get_state();
                    int error = mysqltask->get_error();
                    if(state != WFT_STATE_SUCCESS) {
                        LOG_WARN("MySQL connection failed: %s\n", WFGlobal::get_error_string(state, error));
                        resp->String("Signup Failed");
                        return;
                    }

                    // 1. 检测 SQL 语句是否存在语法错误
                    auto mysqlResp = mysqltask->get_resp();
                    if(mysqlResp->get_packet_type() == MYSQL_PACKET_ERROR) {
                        LOG_ERROR("MySQL ERROR %d: %s\n", mysqlResp->get_error_code(),
                                  mysqlResp->get_error_msg().c_str());
                        resp->String("Signup Failed");
                        return;
                    }

                    using namespace protocol;
                    MySQLResultCursor cursor(mysqlResp);
                    if(cursor.get_cursor_status() == MYSQL_STATUS_OK) {
                        // 2. 成功写入数据库
                        LOG_INFO("Query OK. %llu row affected.", cursor.get_affected_rows());
                        resp->String("SUCCESS");
                    } else {
                        resp->String("Signup Failed");
                    }
                });

            // 执行 SQL 插入操作
            string sql("INSERT INTO CloudDisk.tbl_user(user_name, user_pwd) VALUES('");
            sql += username + "', '" + encodedPassword + "')";
            LOG_INFO("sql:\n%s", sql.c_str());
            mysqlTask->get_req()->set_query(sql);
            series->push_back(mysqlTask);
        }
    });
}

/* ============================================================================
 * 用户登录模块
 * ============================================================================ */
void CloudDiskServer::loadUserLoginModule()
{
    _httpserver.POST("/user/signin", [](const HttpReq *req, HttpResp * resp, SeriesWork * series){
        if(req->content_type() == APPLICATION_URLENCODED) {
            // 解析请求：获取用户名和密码
            auto formKV = req->form_kv();
            string username = formKV["username"];
            string password = formKV["password"];

            LOG_INFO("username:%s, password:%s\n", username.c_str(), password.c_str());

            // 对密码进行加密
            string salt("12345678"); // TODO: 应该随机生成
            const char *encrypted = crypt(password.c_str(), salt.c_str());
            string encodedPassword(encrypted ? encrypted : "");
            LOG_INFO("encodedPassword:%s", encodedPassword.c_str());

            // 从数据库 MySQL 中读取用户信息进行登录验证
            string mysqlurl("mysql://CloudDisk:cloudDisk123@127.0.0.1/CloudDisk");
            auto mysqlTask = WFTaskFactory::create_mysql_task(mysqlurl, 1,
                [=](WFMySQLTask * mysqltask){
                    // 0. 对任务的状态进行检测
                    int state = mysqltask->get_state();
                    int error = mysqltask->get_error();
                    if(state != WFT_STATE_SUCCESS) {
                        LOG_ERROR("MySQL connection failed: %s\n", WFGlobal::get_error_string(state, error));
                        return;
                    }

                    // 1. 检测 SQL 语句是否存在语法错误
                    auto mysqlResp = mysqltask->get_resp();
                    if(mysqlResp->get_packet_type() == MYSQL_PACKET_ERROR) {
                        LOG_ERROR("MySQL ERROR %d: %s\n", mysqlResp->get_error_code(),
                                      mysqlResp->get_error_msg().c_str());
                        resp->String("Login Failed");
                        return;
                    }

                    using namespace protocol;
                    MySQLResultCursor cursor(mysqlResp);

                    // 2. 查询验证（UPDATE 影响行数表示成功）
                    if(cursor.get_cursor_status() == MYSQL_STATUS_OK) {
                        LOG_INFO("Query OK. %llu row affected.", cursor.get_affected_rows());
                        resp->String("Login Failed");
                    } else if(cursor.get_cursor_status() == MYSQL_STATUS_GET_RESULT) {
                        // 3. 读取数据进行登录验证
                        vector<vector<MySQLCell>> matrix;
                        cursor.fetch_all(matrix);
                        string M = matrix[0][0].as_string();
                        LOG_INFO("M: %s", M.c_str());

                        if(encodedPassword == M) {
                            // 3.1 登录成功的情况，生成 Token 信息
                            Token token(username, salt);
                            string tokenStr = token.genToken();

                            // 3.2 构造 JSON 响应
                            wfrest::Json data = wfrest::Json::Object{
                                {"Token", tokenStr},
                                {"Username", username},
                                {"Location", "/static/view/home.html"}
                            };
                            wfrest::Json msg = wfrest::Json::Object{
                                {"data", data}
                            };
                            resp->String(msg.dump());

                            // 3.3 将 Token 保存到数据库中
                            auto nextTask = WFTaskFactory::create_mysql_task(mysqlurl, 1, nullptr);
                            string sql("REPLACE INTO CloudDisk.tbl_user_token(user_name, user_token) VALUES('");
                            sql += username + "', '" + tokenStr + "')";
                            nextTask->get_req()->set_query(sql);
                            series->push_back(nextTask);
                        } else {
                            // 登录失败
                            resp->String("Login Failed");
                        }
                    }
                });

            // 执行 SQL 查询
            string sql("SELECT user_pwd FROM CloudDisk.tbl_user WHERE user_name = '");
            sql += username + "' LIMIT 1";
            LOG_INFO("sql:\n %s", sql.c_str());
            mysqlTask->get_req()->set_query(sql);
            series->push_back(mysqlTask);
        }
    });
}

/* ============================================================================
 * 用户信息模块
 * ============================================================================ */
void CloudDiskServer::loadUserInfoModule()
{
    _httpserver.GET("/user/info", [](const HttpReq *req, HttpResp * resp, SeriesWork * series){
        // 解析请求参数
        string username = req->query("username");
        string tokenStr = req->query("token");

        LOG_INFO("username:%s, token:%s", username.c_str(), tokenStr.c_str());

        string mysqlurl("mysql://CloudDisk:cloudDisk123@127.0.0.1/CloudDisk");
        auto mysqlTask = WFTaskFactory::create_mysql_task(mysqlurl, 1,
                [=](WFMySQLTask * mysqltask){
                    using namespace protocol;
                    auto mysqlResp = mysqltask->get_resp();
                    MySQLResultCursor cursor(mysqlResp);

                    if(cursor.get_cursor_status() == MYSQL_STATUS_GET_RESULT) {
                        // 读操作：获取用户的注册时间
                        vector<vector<MySQLCell>> matrix;
                        cursor.fetch_all(matrix);
                        string signupAt = matrix.empty() ? "" : matrix[0][0].as_string();

                        // 构造 JSON 响应
                        wfrest::Json data = wfrest::Json::Object{
                            {"Username", username},
                            {"SignupAt", signupAt}
                        };
                        wfrest::Json msg = wfrest::Json::Object{
                            {"data", data}
                        };
                        resp->String(msg.dump());
                    } else {
                        resp->String("error");
                    }
                });

        // 执行 SQL 查询
        string sql("SELECT signup_at FROM CloudDisk.tbl_user WHERE user_name = '");
        sql += username + "'";
        mysqlTask->get_req()->set_query(sql);
        series->push_back(mysqlTask);
    });
}

/* ============================================================================
 * 文件列表查询模块
 * ============================================================================ */
void CloudDiskServer::loadFileQueryModule()
{
    _httpserver.POST("/file/query", [](const HttpReq *req, HttpResp * resp, SeriesWork * series){
        // 解析请求：查询词和用户认证信息
        string username = req->query("username");
        string tokenStr = req->query("token");
        string limitCnt = req->form_kv().count("limit") ? req->form_kv().at("limit") : "10";

        LOG_INFO("username:%s, token:%s\n", username.c_str(), tokenStr.c_str());

        string mysqlurl("mysql://CloudDisk:cloudDisk123@127.0.0.1/CloudDisk");
        auto mysqlTask = WFTaskFactory::create_mysql_task(mysqlurl, 1,
            [=](WFMySQLTask * mysqltask){
                using namespace protocol;
                auto mysqlResp = mysqltask->get_resp();
                MySQLResultCursor cursor(mysqlResp);

                if(cursor.get_cursor_status() == MYSQL_STATUS_GET_RESULT) {
                    // 读操作：获取用户的文件列表
                    vector<vector<MySQLCell>> matrix;
                    cursor.fetch_all(matrix);

                    // 构造 JSON 数组响应
                    Json msgArr = Json::Array{};
                    for(size_t i = 0; i < matrix.size(); ++i) {
                        Json row = Json::Object{};
                        row["FileHash"] = matrix[i][0].as_string();
                        row["FileName"] = matrix[i][1].as_string();
                        row["FileSize"] = matrix[i][2].as_ulonglong();
                        row["UploadAt"] = matrix[i][3].as_datetime();
                        row["LastUpdated"] = matrix[i][4].as_datetime();
                        msgArr.push_back(row);
                    }
                    resp->String(msgArr.dump());
                } else {
                    resp->String("error");
                }
            });

        // 执行 SQL 查询
        string sql("SELECT file_sha1, file_name, file_size, upload_at, last_update FROM CloudDisk.tbl_user_file WHERE user_name = '");
        sql += username + "' LIMIT " + limitCnt;
        LOG_INFO("sql:\n %s", sql.c_str());
        mysqlTask->get_req()->set_query(sql);
        series->push_back(mysqlTask);
    });
}

/* ============================================================================
 * 文件上传模块
 * ============================================================================ */
void CloudDiskServer::loadFileUploadModule()
{
    _httpserver.POST("/file/upload", [](const HttpReq *req, HttpResp * resp, SeriesWork * series){
        // 解析请求参数
        string username = req->query("username");
        string tokenStr = req->query("token");

        LOG_INFO("username:%s, token:%s\n", username.c_str(), tokenStr.c_str());

        // 解析请求：消息体（multipart/form-data）
        if(req->content_type() == MULTIPART_FORM_DATA) {
            auto form = req->form();
            string filename = form.count("file") ? form["file"].first : "";
            string content = form.count("file") ? form["file"].second : "";
            if(filename.empty() || content.empty()) {
                resp->String("file upload error: empty filename or content");
                return;
            }

            // 尝试将文件名从GBK转为UTF-8，避免Windows上传乱码
            try {
                std::string utf8name = CharsetUtil::gbk_to_utf8(filename);
                if(!utf8name.empty() && utf8name != filename) {
                    filename = utf8name;
                }
            } catch (...) {
                // 转换失败则保留原始
            }

            // 将数据写入服务器本地
            mkdir("tmp", 0755);
            string filepath = "tmp/" + filename;
            int fd = open(filepath.c_str(), O_CREAT|O_RDWR|O_TRUNC, 0664);
            if(fd < 0) {
                LOG_ERROR("open %s failed: %s", filepath.c_str(), strerror(errno));
                resp->String("file upload error: open failed");
                return;
            }
            ssize_t wlen = write(fd, content.c_str(), content.size());
            close(fd);
            if(wlen != (ssize_t)content.size()) {
                resp->String("file upload error: write failed");
                return;
            }
            // 生成 SHA1 哈希值
            Hash hash(filepath);
            string filehash = hash.sha1();
            LOG_INFO("filehash:%s", filehash.c_str());

            // 将文件相关信息写入数据库 MySQL 中
            string mysqlurl("mysql://CloudDisk:cloudDisk123@127.0.0.1/CloudDisk");
            auto mysqlTask = WFTaskFactory::create_mysql_task(mysqlurl, 1, nullptr);
            string sql("INSERT INTO CloudDisk.tbl_user_file(user_name,file_sha1,file_size,file_name) VALUES('");
            sql += username + "','" + filehash + "', " + std::to_string(content.size()) + " ,'" + filename + "')";
            LOG_INFO("sql:\n %s", sql.c_str());
            mysqlTask->get_req()->set_query(sql);
            series->push_back(mysqlTask);
            resp->String("upload Success");
        }
    });
}

/* ============================================================================
 * 文件下载模块
 * ============================================================================ */
void CloudDiskServer::loadFileDownloadModule()
{
    _httpserver.GET("/file/downloadurl", [](const HttpReq *req, HttpResp * resp){
        string filename = req->query("filename");

        LOG_INFO("filename: %s", filename.c_str());

        // 将下载业务从服务器中分离出去，之后只需要产生一个下载链接就可以了
        // TODO: 需要部署一个下载服务器
        string downloadURL = "http://192.168.100.100:8080/" + filename;
        resp->String(downloadURL);
    });
}
