# 云盘服务器项目

这是一个简单的云盘服务器实现，提供文件存储和用户管理的基本功能。

## 功能特性

- 用户注册和登录
- 文件上传和下载
- 文件查询
- 用户信息管理
- 静态资源服务
- SHA1哈希生成
- Token生成

## 目录结构

- `CloudDiskServer.cc` - 主服务器实现
- `Hash.cc` - 哈希生成工具
- `Token.cc` - Token生成工具
- `main.cc` - 程序入口
- `static/` - 静态资源文件（HTML、CSS、JS）
- `tbl_sql.sql` - 数据库表结构

## 编译和运行

1. 确保已安装必要的依赖库
2. 使用Makefile编译项目
3. 运行服务器：`./cloudDisk_server`
4. 服务器默认在8080端口监听

## 使用示例

1. 用户注册：访问`/register`端点
2. 用户登录：访问`/login`端点
3. 文件上传：通过`/upload`端点上传文件
4. 文件下载：通过`/download`端点下载文件

## 测试

- `testHash.cc` - 测试哈希生成功能
- `testToken.cc` - 测试Token生成功能

## 技术栈

- C++11
- HTTP服务器
- MySQL数据库
- Bootstrap前端框架

## 注意事项

- 需要配置`file.conf`文件
- 需要创建数据库并导入`tbl_sql.sql`表结构
- 项目使用了一些第三方JavaScript库（jQuery、Bootstrap等）