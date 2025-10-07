# Makefile for bignum-lib aggregator

# --- Tools ---
AR = ar
ARFLAGS = rcs

# --- Directories ---
BUILD_DIR = build
DIST_DIR = dist
INCLUDE_DIR = include
LIBS_DIR = libs

# --- Files ---
TARGET_LIB = $(DIST_DIR)/libbignum.a
# Собираем список всех объектных файлов, которые должны войти в библиотеку
OBJECTS = $(LIBS_DIR)/bignum-shift-left/build/bignum_shift_left.o
# (в будущем здесь будут и другие .o файлы)

.PHONY: all build install clean help

all: build

# --- Main Targets ---

# Главная цель: собрать библиотеку
build: $(TARGET_LIB)

# Цель "install" копирует библиотеку и заголовки в папку dist/
install: $(TARGET_LIB)
	@echo "Installing library and headers to $(DIST_DIR)/..."
	cp -r $(INCLUDE_DIR)/* $(DIST_DIR)/
	strip --strip-all $(TARGET_LIB)
	@echo "Installation complete."

# --- Compilation Rules ---

# Правило для создания библиотеки из объектных файлов
$(TARGET_LIB): $(OBJECTS) | $(DIST_DIR)
	@echo "Creating static library $(TARGET_LIB)..."
	$(AR) $(ARFLAGS) $(TARGET_LIB) $(OBJECTS)

# Правило для сборки объектных файлов: рекурсивно вызываем make в сабмодулях
$(OBJECTS):
	@echo "Building submodules..."
	$(MAKE) -C $(LIBS_DIR)/bignum-shift-left build

# --- Utility Targets ---

$(BUILD_DIR) $(DIST_DIR):
	mkdir -p $@

clean:
	@echo "Cleaning up main project..."
	rm -rf $(BUILD_DIR) $(DIST_DIR)
	@echo "Cleaning up submodules..."
	$(MAKE) -C $(LIBS_DIR)/bignum-shift-left clean

help:
	@echo "Available targets:"
	@echo "  all/build  - Build the static library libbignum.a."
	@echo "  install    - Copy library and headers to dist/ directory."
	@echo "  clean      - Remove all generated files from main project and submodules."