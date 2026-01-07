# CloudDisk 项目文档

## 项目概述

CloudDisk 是一个基于 C++ 实现的轻量级云盘服务器，提供基本的文件存储与用户管理功能，适用于学习和小型部署场景。该项目旨在解决本地文件无法远程访问、用户身份验证和安全传输等基础云存储需求。

## 系统功能

### 主要功能

1. **用户注册与登录**
   - 通过 `/user/signup` 接口进行用户注册
   - 通过 `/user/signin` 接口进行用户登录

2. **文件上传**
   - 支持 HTTP POST 到 `/file/upload`
   - 支持多部分表单数据上传

3. **文件下载**
   - 通过 `/file/downloadurl` 获取下载链接

4. **文件查询与管理**
   - 通过 `/file/query` 接口查询用户文件列表

5. **用户信息管理**
   - 通过 `/user/info` 接口获取用户信息

6. **静态资源服务**
   - HTML/CSS/JS 页面在 `static/` 目录下
   - 提供注册、登录、主页和上传页面

7. **安全功能**
   - SHA1 哈希生成（用于文件去重或校验）
   - Token 生成（用于会话认证）

### 关键特性

- 使用 C++11 编写，依赖高性能 HTTP 框架 wfrest 和 workflow
- 支持 HTTPS（依赖 OpenSSL 库）
- 日志系统集成（log4cpp）
- 可扩展的模块化设计

## 技术架构

### 设计模式

- **单例模式**：可能用于日志组件 MyLogger
- **工具类模式**：Hash 和 Token 类为无状态工具类，提供静态功能

### 组件交互

1. **main.cc** 启动程序，初始化并运行 `CloudiskServer`
2. **CloudiskServer.cc** 处理所有 HTTP 请求路由
3. **Hash.cc** 提供 SHA1 计算服务，用于文件指纹
4. **Token.cc** 负责生成和解析认证 token
5. **MyLogger.cc** 封装日志输出，供各模块调用
6. 静态页面由服务器直接响应，前端使用 jQuery + Bootstrap 实现交互

### 技术选型

#### 前端
- jQuery v3.2.1
- Bootstrap 4.x（通过 bootstrap.min.css/js）
- FileInput 插件用于上传界面

#### 后端
- C++11
- wfrest（RESTful HTTP 服务器框架）
- workflow（异步任务调度库）

#### 数据库
- MySQL（需手动导入 `database/tbl_sql.sql` 创建表结构）

#### 安全与加密
- OpenSSL（libssl, libcrypto）用于 HTTPS 和加密操作
- crypt（密码哈希）

#### 日志
- log4cpp（liblog4cpp）

#### 构建工具
- Makefile 控制编译流程

## 开发环境

### 必需工具

- GCC（支持 C++11）
- Make
- wfrest、workflow、OpenSSL、log4cpp 等库已安装
- MySQL 数据库服务

### 环境搭建

1. **安装依赖库（以 Ubuntu 为例）**
   ```bash
   sudo apt-get install libssl-dev liblog4cpp5-dev
   ```

2. **编译 wfrest 和 workflow 并正确链接**（需提前构建并配置 LD_LIBRARY_PATH）

3. **导入数据库结构**
   ```bash
   mysql -u root -p < database/tbl_sql.sql
   ```

4. **配置 `conf/file.conf` 文件**（包含数据库连接信息、端口等）

### 运行环境

- **构建命令**:
  ```bash
  make
  ```

- **本地开发**:
  ```bash
  ./CloudiskServer
  ```
  （默认监听 1234 端口）

- **清理构建**:
  ```bash
  make clean
  ```

## 部署说明

1. 部署前确保 `conf/file.conf` 正确配置
2. 将 `static/` 目录与可执行文件一同部署
3. 使用 systemd 或后台进程方式运行服务
4. 建议配合 Nginx 做反向代理和静态资源缓存

## API 接口文档

### 用户注册
- **接口**: `POST /user/signup`
- **功能**: 注册新用户
- **参数**: username, password (表单数据)
- **返回**: SUCCESS 或错误信息

### 用户登录
- **接口**: `POST /user/signin`
- **功能**: 用户登录
- **参数**: username, password (表单数据)
- **返回**: JSON 格式的 token 信息

### 获取用户信息
- **接口**: `GET /user/info`
- **功能**: 获取用户信息
- **参数**: username, token (查询参数)
- **返回**: JSON 格式的用户信息

### 文件查询
- **接口**: `POST /file/query`
- **功能**: 查询用户文件列表
- **参数**: username, token, limit (表单数据)
- **返回**: JSON 格式的文件列表

### 文件上传
- **接口**: `POST /file/upload`
- **功能**: 上传文件
- **参数**: username, token (查询参数), file (表单数据)
- **返回**: 上传结果

### 文件下载链接
- **接口**: `GET /file/downloadurl`
- **功能**: 获取文件下载链接
- **参数**: filename (查询参数)
- **返回**: 下载链接

## 项目结构

```
.
├── conf                    # 配置文件
│   └── file.conf
├── database                # 数据库文件
│   └── tbl_sql.sql
├── doc                     # 项目文档
├── include                 # 头文件
│   ├── CloudiskServer.h
│   ├── Hash.h
│   ├── MyLogger.hpp
│   ├── Token.h
│   └── unixHeader.h
├── src                     # 源代码
│   ├── CloudiskServer.cc
│   ├── Hash.cc
│   ├── MyLogger.cc
│   ├── Token.cc
│   └── main.cc
├── static                  # 静态资源
│   ├── js
│   └── view
├── tests                   # 测试文件
├── Makefile                # 构建脚本
├── README.en.md
└── README.md
```

## 技术约束与限制

### 代码规范
- 未提供具体规范，但命名清晰，头文件分离

### 性能要求
- 适用于中小规模并发请求，未提及高可用或集群支持

### 安全要求
- 用户密码应加密存储（推测使用 SHA1 + salt）
- Token 机制保障会话安全
- 依赖 OpenSSL 实现通信加密

### 已知问题
- 未提供 API 文档和错误码说明
- 缺少单元测试覆盖率报告
- 无自动部署脚本或 Docker 支持
- 第三方 JS 库版本较旧（如 jQuery 3.2.1 存在潜在安全风险）

### 技术限制
- 必须手动管理第三方库依赖（wfrest/workflow 需自行编译安装）
- 不支持热更新或动态加载模块
- 日志级别和路径依赖 `conf/file.conf`，配置不灵活