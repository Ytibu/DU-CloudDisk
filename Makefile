INCLUDES:=./  ./include ./src
SRCS:=$(wildcard src/*.cc) 
LIBS:=  -lwfrest -lworkflow -lssl -lcrypto -lcrypt -llog4cpp

# 提取src目录下的源文件并映射到build目录下的.o文件
BUILD_DIR:=build
SOURCES:=$(wildcard src/*.cc)
OBJECTS:=$(SOURCES:src/%.cc=$(BUILD_DIR)/%.o)

SERVER:= CloudiskServer

# 主程序目标
$(SERVER): $(OBJECTS)
	g++ $^ -o $@ $(LIBS) -g

# 编译src目录下的源文件到build目录
$(BUILD_DIR)/%.o: src/%.cc
	g++ -c $< -o $@ $(LIBS) $(addprefix -I, $(INCLUDES)) -g

# 定义测试目标
HASH:=testHash
TOKEN:=testToken

$(HASH): $(BUILD_DIR)/Hash.o $(BUILD_DIR)/testHash.o $(BUILD_DIR)/MyLogger.o
	g++ $^ -o $@ $(LIBS) $(addprefix -I, $(INCLUDES)) -g

$(TOKEN): $(BUILD_DIR)/Token.o $(BUILD_DIR)/testToken.o $(BUILD_DIR)/MyLogger.o
	g++ $^ -o $@ $(LIBS) $(addprefix -I, $(INCLUDES)) -g

# 编译测试文件
$(BUILD_DIR)/testHash.o: tests/testHash.cc
	g++ -c $< -o $@ $(LIBS) $(addprefix -I, $(INCLUDES)) -g

$(BUILD_DIR)/testToken.o: tests/testToken.cc
	g++ -c $< -o $@ $(LIBS) $(addprefix -I, $(INCLUDES)) -g

# 显示变量值
echo:
	echo $(INCLUDES)
	echo $(SOURCES)
	echo $(OBJECTS)

# 清理构建文件
clean:
	rm -rf $(BUILD_DIR)/*.o $(SERVER) $(HASH) $(TOKEN)

# 创建build目录（如果不存在）
prepare:
	@mkdir -p $(BUILD_DIR)