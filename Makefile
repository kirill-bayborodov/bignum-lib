# Makefile for bignum-lib aggregator

LIB_NAME     := bignum
CONFIG       ?= release
# --- Compiler and Flags ---
CC           := gcc

# --- Tools ---
AR           := ar
STRIP        := strip
RL           := ranlib
CPPCHECK     := cppcheck
OBJCOPY      := objcopy
NM           := nm
ARFLAGS      := rcs
RM           := rm -rf
MKDIR        := mkdir -p

# --- Directories ---
BUILD_DIR    := build
DIST_DIR     := dist
INCLUDE_DIR  := include
LIBS_DIR     := libs
TESTS_DIR    := tests
BIGNUM_DIR   := $(LIBS_DIR)/common/include

# --- Files ---
TARGET_LIB   := $(DIST_DIR)/lib$(LIB_NAME).a
# Собираем список всех объектных файлов, которые должны войти в библиотеку
BIGNUM_SHIFT_LEFT_OBJ := $(LIBS_DIR)/bignum-shift-left/build/bignum_shift_left.o
BIGNUM_SHIFT_RIGHT_OBJ := $(LIBS_DIR)/bignum-shift-right/build/bignum_shift_right.o

OBJECTS      :=  $(BIGNUM_SHIFT_LEFT_OBJ) $(BIGNUM_SHIFT_RIGHT_OBJ)
# (в будущем здесь будут и другие .o файлы)

.PHONY: all build install test clean help

all: build

# --- Main Targets ---

# Главная цель: собрать библиотеку
build: $(TARGET_LIB)

# Цель "install" копирует библиотеку и заголовки в папку dist/
install: build
	@echo "Installing library and headers to $(DIST_DIR)/..."
# 1. Создаем нужную структуру папок в dist/
	@$(MKDIR) $(DIST_DIR)/common
	@$(MKDIR) $(DIST_DIR)/bignum-shift-left
	@$(MKDIR) $(DIST_DIR)/bignum-shift-right
# 2. Копируем главный заголовочный файл
	@cp $(INCLUDE_DIR)/bignum.h $(DIST_DIR)/
# 3. Копируем зависимые заголовочные файлы, сохраняя структуру
	@cp $(LIBS_DIR)/common/include/bignum.h $(DIST_DIR)/common/
	@cp $(LIBS_DIR)/bignum-shift-left/include/bignum_shift_left.h $(DIST_DIR)/bignum-shift-left/
	@cp $(LIBS_DIR)/bignum-shift-right/include/bignum_shift_right.h $(DIST_DIR)/bignum-shift-right/
	@cp -r $(INCLUDE_DIR)/* $(DIST_DIR)/
	@echo "Installation complete."

dist: clean install
	@echo "Creating distribution in $(DIST_DIR)/ (CONFIG=$(CONFIG))..."
	@$(MKDIR) $(DIST_DIR)	
	@cp README.md $(DIST_DIR)/
	@cp LICENSE $(DIST_DIR)/
	@echo "Done."	

# Цель для запуска интеграционных тестов
test: install | $(BUILD_DIR)
	@echo "Running integration tests..."
	@$(CC) tests/test_integration.c -I$(DIST_DIR) -L$(DIST_DIR) -l$(LIB_NAME) -o $(BUILD_DIR)/test_runner -no-pie
	@./$(BUILD_DIR)/test_runner
	@echo "Ok";		

# --- Compilation Rules ---

# Правило для создания библиотеки из объектных файлов
$(TARGET_LIB): $(OBJECTS) 
	@$(MKDIR) $(DIST_DIR)		
	@echo "Stripping object files..."
	@$(STRIP) --strip-debug $(OBJECTS) || true; 
	@$(STRIP) --strip-unneeded $(OBJECTS) || true; 
	@echo "Ok";		
	@echo "Creating static library $(TARGET_LIB)..."	
	@$(AR) $(ARFLAGS) $(TARGET_LIB) $(OBJECTS)
	@echo "Ok";	
	@echo "Indexing static library..."
	@ranlib $(TARGET_LIB)
	@echo "Ok";	

# Правило для сборки объектных файлов: рекурсивно вызываем make в сабмодулях
$(OBJECTS):
	@echo "Building submodules... (CONFIG=$(CONFIG))... "
	@$(MAKE) -s -C $(LIBS_DIR)/bignum-shift-left build CONFIG=release
	@$(MAKE) -s -C $(LIBS_DIR)/bignum-shift-right build CONFIG=release
	@echo "Ok";		

# --- Utility Targets ---

$(BUILD_DIR):
	@$(MKDIR) $@

lint:
	@echo "Lint submodules... (CONFIG=$(CONFIG))... "
	@$(MAKE) -s -C $(LIBS_DIR)/bignum-shift-left lint CONFIG=release
	@$(MAKE) -s -C $(LIBS_DIR)/bignum-shift-right lint CONFIG=release
	@echo "Ok";	
	@echo "Running static analysis on C source files..."		
	@$(CPPCHECK) --std=c11 --enable=all --error-exitcode=1 --suppress=missingIncludeSystem \
	    --inline-suppr --inconclusive --check-config \
	    -I$(INCLUDE_DIR) -I$(BIGNUM_DIR) \
	    $(TESTS_DIR)/  $(DIST_DIR)/ 
	@echo "Ok";		    

clean:
	@echo "Cleaning up main project..."
	@rm -rf $(BUILD_DIR) $(DIST_DIR)
	@echo "Ok";		
	@echo "Cleaning up submodules..."
	@$(MAKE) -s -C $(LIBS_DIR)/bignum-shift-left clean
	@$(MAKE) -s -C $(LIBS_DIR)/bignum-shift-right clean
	@echo "Ok";		

help:
	@echo "Available targets:"
	@echo "  all/build  - Build the static library libbignum.a."
	@echo "  install    - Copy library and headers to dist/ directory."
	@echo "  dist       - Packages the product into the 'dist/' directory for external use." 	
	@echo "  clean      - Remove all generated files from main project and submodules."
	@echo "  lint       - Running static analysis on C source files"
	@echo "  help       - Shows this help message."	