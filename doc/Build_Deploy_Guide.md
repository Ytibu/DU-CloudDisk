# CloudDisk 构建与部署指南

## 项目简介

CloudDisk 是一个基于 C++ 实现的轻量级云盘服务器，提供基本的文件存储与用户管理功能。本文档详细说明如何构建和部署 CloudDisk 项目。

## 环境要求

### 操作系统
- Linux (推荐 Ubuntu 18.04 或更高版本)

### 硬件要求
- CPU: 1 核或以上
- 内存: 1GB 或以上
- 存储: 10GB 可用空间

### 软件依赖
- GCC (支持 C++11)
- Make
- MySQL
- OpenSSL
- log4cpp
- wfrest
- workflow

## 依赖安装

### Ubuntu/Debian 系统

```bash
# 安装基础编译工具
sudo apt-get update
sudo apt-get install build-essential cmake git

# 安装 OpenSSL 和 log4cpp 开发库
sudo apt-get install libssl-dev liblog4cpp5-dev

# 安装 MySQL 客户端和服务器
sudo apt-get install mysql-server mysql-client
```

### 安装 wfrest 和 workflow

```bash
# 克隆 workflow 仓库
git clone https://github.com/sogou/workflow.git
cd workflow
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install

# 克隆 wfrest 仓库
git clone https://github.com/wfrest/wfrest.git
cd wfrest
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DWFREST_EXAMPLES=OFF
make -j$(nproc)
sudo make install
```

## 项目构建

### 1. 克隆项目

```bash
git clone <your-repository-url>
cd CloudDisk
```

### 2. 配置数据库

首先创建数据库表结构：

```bash
mysql -u root -p < database/tbl_sql.sql
```

### 3. 修改配置文件

编辑 `conf/file.conf` 文件，配置数据库连接信息：

```ini
# 数据库连接配置
user root;
password <your-mysql-password>;
host localhost;
port 3306;
database CloudDisk;
```

### 4. 编译项目

```bash
# 确保 build 目录存在
make prepare

# 编译主程序
make
```

编译成功后会生成 `CloudDiskServer` 可执行文件。

### 5. 编译测试程序

如果需要运行测试，可以编译测试程序：

```bash
make testHash
make testToken
```

## 运行服务

### 1. 启动服务

```bash
./CloudDiskServer
```

服务器将在 1234 端口启动。

### 2. 验证服务

打开浏览器访问 `http://localhost:1234/user/signup`，如果能正常显示注册页面，则服务启动成功。

### 3. 停止服务

使用 `Ctrl+C` 组合键停止服务。

## 配置说明

### 服务器配置

在 `conf/file.conf` 文件中可以配置服务器参数：

- 端口：默认为 1234
- 数据库连接信息
- 日志级别和路径

### MySQL 配置

项目使用 MySQL 数据库存储用户信息和文件信息，需要确保：
- 数据库服务正常运行
- 用户名密码正确
- 数据库表结构已创建

## 部署说明

### 1. 服务器环境准备

确保服务器上安装了所有依赖项，包括：
- MySQL 服务
- OpenSSL 库
- log4cpp 库
- wfrest 和 workflow 框架

### 2. 上传文件

将以下文件上传到服务器：
- `CloudDiskServer` (可执行文件)
- `conf/` 目录
- `static/` 目录
- `database/` 目录（用于备份）

### 3. 创建运行目录

```bash
mkdir -p /opt/cloudisk
cd /opt/cloudisk
```

### 4. 设置权限

```bash
chmod +x CloudDiskServer
chown cloudisk:cloudisk -R /opt/cloudisk
```

### 5. 启动服务

```bash
# 前台运行
./CloudDiskServer

# 或后台运行
nohup ./CloudDiskServer > cloudisk.log 2>&1 &
```

### 6. 配置系统服务（可选）

创建系统服务文件 `/etc/systemd/system/cloudisk.service`：

```ini
[Unit]
Description=CloudDisk Server
After=network.target

[Service]
Type=simple
User=cloudisk
WorkingDirectory=/opt/cloudisk
ExecStart=/opt/cloudisk/CloudDiskServer
Restart=always

[Install]
WantedBy=multi-user.target
```

启用并启动服务：

```bash
sudo systemctl daemon-reload
sudo systemctl enable cloudisk
sudo systemctl start cloudisk
```

## 测试

### 单元测试

运行已编译的测试程序：

```bash
./testHash
./testToken
```

### 功能测试

使用 curl 或 Postman 测试 API 接口：

```bash
# 测试服务器状态
curl http://localhost:1234/

# 用户注册
curl -X POST http://localhost:1234/user/signup \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "username=testuser&password=testpass"

# 用户登录
curl -X POST http://localhost:1234/user/signin \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "username=testuser&password=testpass"
```

## 故障排除

### 1. 编译错误

如果遇到编译错误，请检查：
- 所有依赖库是否正确安装
- 头文件路径是否正确
- 库链接是否正确

### 2. 运行时错误

如果服务无法启动，请检查：
- MySQL 服务是否运行
- 数据库连接参数是否正确
- 端口是否被占用

### 3. 访问错误

如果无法访问服务，请检查：
- 防火墙设置
- 网络连接
- 服务是否正常运行

## 维护

### 日志管理

服务器日志保存在配置文件指定的位置，定期清理日志文件以节省磁盘空间。

### 备份

定期备份数据库和重要配置文件：

```bash
mysqldump -u root -p CloudDisk > backup_$(date +%Y%m%d).sql
```

### 更新

更新服务时：
1. 停止当前服务
2. 备份配置文件
3. 替换可执行文件
4. 重新启动服务