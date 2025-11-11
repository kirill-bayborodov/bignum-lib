# Makefile for bignum-lib aggregator

LIB_NAME     := bignum
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

# --- Directories ---
BUILD_DIR    := build
DIST_DIR     := dist
INCLUDE_DIR  := include
LIBS_DIR     := libs
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
install: $(TARGET_LIB)
	@echo "Installing library and headers to $(DIST_DIR)/..."
	# 1. Создаем нужную структуру папок в dist/
	mkdir -p $(DIST_DIR)/common
	mkdir -p $(DIST_DIR)/bignum-shift-left
	mkdir -p $(DIST_DIR)/bignum-shift-right
	# 2. Копируем главный заголовочный файл
	cp $(INCLUDE_DIR)/bignum.h $(DIST_DIR)/
	# 3. Копируем зависимые заголовочные файлы, сохраняя структуру
	cp $(LIBS_DIR)/common/include/bignum.h $(DIST_DIR)/common/
	cp $(LIBS_DIR)/bignum-shift-left/include/bignum_shift_left.h $(DIST_DIR)/bignum-shift-left/
	cp $(LIBS_DIR)/bignum-shift-right/include/bignum_shift_right.h $(DIST_DIR)/bignum-shift-right/
	@echo "Installing library and headers to $(DIST_DIR)/..."
	cp -r $(INCLUDE_DIR)/* $(DIST_DIR)/
	@echo "Installation complete."

# Цель для запуска интеграционных тестов
test: install | $(BUILD_DIR)
	@echo "Running integration tests..."
	$(CC) tests/test_integration.c -I$(DIST_DIR) -L$(DIST_DIR) -l$(LIB_NAME) -o $(BUILD_DIR)/test_runner -no-pie
	./$(BUILD_DIR)/test_runner

# --- Compilation Rules ---

# Правило для создания библиотеки из объектных файлов
$(TARGET_LIB): $(OBJECTS) | $(DIST_DIR)
	@echo "Stripping object files, keeping symbol $(LIB_NAME)..."
	@$(STRIP) --strip-debug $(OBJECTS) || true; 
	@$(STRIP) --strip-unneeded $(OBJECTS) || true; 
	@echo "   symbols now:"; $(NM) -g --defined-only $(OBJECTS) || true; 
	@echo "";		
	@echo "Creating static library $(TARGET_LIB)..."	
	$(AR) $(ARFLAGS) $(TARGET_LIB) $(OBJECTS)
	@echo "Indexing static library..."
	ranlib $(TARGET_LIB)
	@echo "   symbols now:"; $(NM) -g --defined-only $(TARGET_LIB) || true; 		

# Правило для сборки объектных файлов: рекурсивно вызываем make в сабмодулях
$(OBJECTS):
	@echo "Building submodules..."
	$(MAKE) -C $(LIBS_DIR)/bignum-shift-left build
	$(MAKE) -C $(LIBS_DIR)/bignum-shift-right build

# --- Utility Targets ---

$(BUILD_DIR) $(DIST_DIR):
	mkdir -p $@

clean:
	@echo "Cleaning up main project..."
	rm -rf $(BUILD_DIR) $(DIST_DIR)
	@echo "Cleaning up submodules..."
	$(MAKE) -C $(LIBS_DIR)/bignum-shift-left clean
	$(MAKE) -C $(LIBS_DIR)/bignum-shift-right clean

help:
	@echo "Available targets:"
	@echo "  all/build  - Build the static library libbignum.a."
	@echo "  install    - Copy library and headers to dist/ directory."
	@echo "  clean      - Remove all generated files from main project and submodules."