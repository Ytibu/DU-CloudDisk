INCLUDES:=./  ./include ./src
SRCS:=$(wildcard src/*.cc) 
LIBS:=  -lwfrest -lworkflow -lssl -lcrypto -lcrypt -llog4cpp #-liconv

# 提取src目录下的源文件并映射到build目录下的.o文件
BUILD_DIR:=build
BIN_DIR:=bin
SOURCES:=$(wildcard src/*.cc)
OBJECTS:=$(SOURCES:src/%.cc=$(BUILD_DIR)/%.o)
SERVER:= $(BIN_DIR)/CloudDiskServer

# 主程序目标
$(SERVER): $(OBJECTS) | prepare_bin
	g++ $^ -o $@ $(LIBS) -g

# 创建bin目录（如果不存在）
prepare_bin:
	@mkdir -p $(BIN_DIR)

# 编译src目录下的源文件到build目录
$(BUILD_DIR)/%.o: src/%.cc
	g++ -c $< -o $@ $(LIBS) $(addprefix -I, $(INCLUDES)) -g

# 定义测试目标
HASH:= $(BIN_DIR)/testHash
TOKEN:= $(BIN_DIR)/testToken

$(HASH): $(BUILD_DIR)/Hash.o $(BUILD_DIR)/testHash.o $(BUILD_DIR)/MyLogger.o | prepare_bin
	g++ $^ -o $@ $(LIBS) $(addprefix -I, $(INCLUDES)) -g

$(TOKEN): $(BUILD_DIR)/Token.o $(BUILD_DIR)/testToken.o $(BUILD_DIR)/MyLogger.o | prepare_bin
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
	rm -rf $(BUILD_DIR)/*.o $(BIN_DIR)/*

# 创建build目录（如果不存在）
prepare:
	@mkdir -p $(BUILD_DIR)