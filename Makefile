# ================================================================
# Makefile for Water Reminder Application
# Author: zcg
# Description: 喝水提醒终端应用构建系统
# ================================================================

# 编译器设置
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2
DEBUG_CFLAGS = -Wall -Wextra -std=c99 -pedantic -g -DDEBUG
LIBS = -lm

# 目录设置
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INSTALL_DIR = /usr/local/bin

# 源文件和目标文件
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TARGET = $(BIN_DIR)/water_reminder
DEBUG_TARGET = $(BIN_DIR)/water_reminder_debug

# 颜色定义
BOLD = \033[1m
GREEN = \033[32m
YELLOW = \033[33m
BLUE = \033[34m
RED = \033[31m
RESET = \033[0m

# 默认目标
.PHONY: all clean debug install uninstall help test

all: $(TARGET)

# 创建必要的目录
$(BUILD_DIR):
	@echo "$(BLUE)Creating build directory...$(RESET)"
	@mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	@echo "$(BLUE)Creating bin directory...$(RESET)"
	@mkdir -p $(BIN_DIR)

# 编译目标文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

# 链接生成可执行文件
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	@echo "$(GREEN)Linking $(TARGET)...$(RESET)"
	@$(CC) $(OBJECTS) $(LIBS) -o $(TARGET)
	@echo "$(BOLD)$(GREEN)✅ Build completed successfully!$(RESET)"
	@echo "$(BLUE)Run with: ./$(TARGET)$(RESET)"

# 调试版本
debug: CFLAGS = $(DEBUG_CFLAGS)
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(OBJECTS) | $(BIN_DIR)
	@echo "$(GREEN)Linking debug version...$(RESET)"
	@$(CC) $(OBJECTS) $(LIBS) -o $(DEBUG_TARGET)
	@echo "$(BOLD)$(GREEN)✅ Debug build completed!$(RESET)"

# 清理构建文件
clean:
	@echo "$(RED)Cleaning build files...$(RESET)"
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "$(GREEN)✅ Clean completed!$(RESET)"

# 深度清理（包括配置和数据文件）
distclean: clean
	@echo "$(RED)Cleaning all generated files...$(RESET)"
	@rm -rf config data logs
	@echo "$(GREEN)✅ Distribution clean completed!$(RESET)"

# 安装到系统
install: $(TARGET)
	@echo "$(BLUE)Installing to $(INSTALL_DIR)...$(RESET)"
	@sudo cp $(TARGET) $(INSTALL_DIR)/
	@sudo chmod +x $(INSTALL_DIR)/water_reminder
	@echo "$(BOLD)$(GREEN)✅ Installation completed!$(RESET)"
	@echo "$(BLUE)You can now run 'water_reminder' from anywhere$(RESET)"

# 从系统卸载
uninstall:
	@echo "$(RED)Uninstalling from $(INSTALL_DIR)...$(RESET)"
	@sudo rm -f $(INSTALL_DIR)/water_reminder
	@echo "$(GREEN)✅ Uninstallation completed!$(RESET)"

# 运行程序
run: $(TARGET)
	@echo "$(BLUE)Running Water Reminder...$(RESET)"
	@./$(TARGET)

# 运行调试版本
run-debug: $(DEBUG_TARGET)
	@echo "$(BLUE)Running debug version...$(RESET)"
	@./$(DEBUG_TARGET)

# 代码格式化
format:
	@echo "$(BLUE)Formatting source code...$(RESET)"
	@find $(SRC_DIR) -name "*.c" -o -name "*.h" | xargs clang-format -i
	@echo "$(GREEN)✅ Code formatting completed!$(RESET)"

# 静态代码分析
analyze:
	@echo "$(BLUE)Running static analysis...$(RESET)"
	@cppcheck --enable=all --std=c99 $(SRC_DIR)/
	@echo "$(GREEN)✅ Static analysis completed!$(RESET)"

# 内存检查
memcheck: $(DEBUG_TARGET)
	@echo "$(BLUE)Running memory check...$(RESET)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(DEBUG_TARGET)

# 快速测试
test: $(TARGET)
	@echo "$(BLUE)Running quick tests...$(RESET)"
	@echo "$(YELLOW)Testing binary exists...$(RESET)"
	@test -f $(TARGET) && echo "$(GREEN)✅ Binary exists$(RESET)" || echo "$(RED)❌ Binary not found$(RESET)"
	@echo "$(YELLOW)Testing binary is executable...$(RESET)"
	@test -x $(TARGET) && echo "$(GREEN)✅ Binary is executable$(RESET)" || echo "$(RED)❌ Binary not executable$(RESET)"
	@echo "$(YELLOW)Testing help output...$(RESET)"
	@timeout 3s ./$(TARGET) --help >/dev/null 2>&1; \
	if [ $$? -eq 124 ]; then \
		echo "$(YELLOW)⚠️  Program doesn't support --help flag$(RESET)"; \
	else \
		echo "$(GREEN)✅ Help output test passed$(RESET)"; \
	fi

# 创建发布包
package: clean all
	@echo "$(BLUE)Creating release package...$(RESET)"
	@mkdir -p release/water_reminder
	@cp -r $(BIN_DIR) $(SRC_DIR) Makefile README.md release/water_reminder/
	@cd release && tar -czf water_reminder-v1.0.tar.gz water_reminder/
	@rm -rf release/water_reminder
	@echo "$(BOLD)$(GREEN)✅ Package created: release/water_reminder-v1.0.tar.gz$(RESET)"

# 显示项目信息
info:
	@echo "$(BOLD)$(BLUE)Water Reminder Application$(RESET)"
	@echo "$(BLUE)Author: zcg$(RESET)"
	@echo "$(BLUE)Version: 1.0$(RESET)"
	@echo "$(BLUE)Description: 炫酷的喝水提醒终端应用$(RESET)"
	@echo ""
	@echo "$(YELLOW)Source files:$(RESET)"
	@ls -la $(SRC_DIR)/
	@echo ""
	@echo "$(YELLOW)Compiler: $(CC)$(RESET)"
	@echo "$(YELLOW)Flags: $(CFLAGS)$(RESET)"
	@echo "$(YELLOW)Libraries: $(LIBS)$(RESET)"

# 帮助信息
help:
	@echo "$(BOLD)$(BLUE)Water Reminder - Makefile Help$(RESET)"
	@echo ""
	@echo "$(YELLOW)Available targets:$(RESET)"
	@echo "  $(GREEN)all$(RESET)         - Build the application (default)"
	@echo "  $(GREEN)debug$(RESET)       - Build debug version with sanitizers"
	@echo "  $(GREEN)clean$(RESET)       - Remove build files"
	@echo "  $(GREEN)distclean$(RESET)   - Remove all generated files"
	@echo "  $(GREEN)install$(RESET)     - Install to system (requires sudo)"
	@echo "  $(GREEN)uninstall$(RESET)   - Remove from system (requires sudo)"
	@echo "  $(GREEN)run$(RESET)         - Build and run the application"
	@echo "  $(GREEN)run-debug$(RESET)   - Build and run debug version"
	@echo "  $(GREEN)format$(RESET)      - Format source code with clang-format"
	@echo "  $(GREEN)analyze$(RESET)     - Run static code analysis"
	@echo "  $(GREEN)memcheck$(RESET)    - Run memory leak detection"
	@echo "  $(GREEN)test$(RESET)        - Run quick tests"
	@echo "  $(GREEN)package$(RESET)     - Create release package"
	@echo "  $(GREEN)info$(RESET)        - Show project information"
	@echo "  $(GREEN)help$(RESET)        - Show this help message"
	@echo ""
	@echo "$(YELLOW)Examples:$(RESET)"
	@echo "  make           - Build the application"
	@echo "  make debug     - Build debug version"
	@echo "  make run       - Build and run"
	@echo "  make install   - Install system-wide"
	@echo "  make clean     - Clean build files"

# 依赖关系
$(BUILD_DIR)/main.o: $(SRC_DIR)/water_reminder.h
$(BUILD_DIR)/core.o: $(SRC_DIR)/water_reminder.h
$(BUILD_DIR)/ui.o: $(SRC_DIR)/water_reminder.h 