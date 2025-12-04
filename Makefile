# Makefile for bignum-lib aggregator

#LIB_NAME     := bignum
CONFIG       ?= release

# --- Calculated Variables --
REPOSITORY_NAME := $(notdir $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
FAMILY_NAME := $(firstword $(subst -, ,$(REPOSITORY_NAME)))
LIB_NAME := $(subst -,_,$(notdir $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))))
LIB_NAME := $(FAMILY_NAME)
UPPER_LIB_NAME := $(subst z,Z,$(subst y,Y,$(subst x,X,$(subst w,W,$(subst v,V,$(subst u,U,$(subst t,T,$(subst s,S,$(subst r,R,$(subst q,Q,$(subst p,P,$(subst o,O,$(subst n,N,$(subst m,M,$(subst l,L,$(subst k,K,$(subst j,J,$(subst i,I,$(subst h,H,$(subst g,G,$(subst f,F,$(subst e,E,$(subst d,D,$(subst c,C,$(subst b,B,$(subst a,A,$(LIB_NAME)))))))))))))))))))))))))))
NP := $(shell nproc | awk '{print $$1}')

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
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TESTS_DIR = tests
BENCH_DIR = benchmarks
INCLUDE_DIR = include
LIBS_DIR = libs
DIST_DIR = dist
COMMON_NAME  := $(FAMILY_NAME)-common
COMMON_DIR   := $(LIBS_DIR)/$(COMMON_NAME)
COMMON_INCLUDE_DIR := $(COMMON_DIR)/$(INCLUDE_DIR)

# --- Files ---
TARGET_LIB   := $(DIST_DIR)/lib$(LIB_NAME).a
# Собираем список всех объектных файлов, которые должны войти в библиотеку
SUBMODULES   := $(patsubst $(LIBS_DIR)/%/,%,$(filter %/,$(wildcard $(LIBS_DIR)/*/)))
OBJ_LIST := $(filter-out $(COMMON_NAME),$(patsubst $(LIBS_DIR)/%/,%,$(filter %/,$(wildcard $(LIBS_DIR)/*/))))
OBJECTS  := $(foreach d,$(OBJ_LIST),$(LIBS_DIR)/$(d)/$(BUILD_DIR)/$(subst -,_,$(d)).o)
ASM_SOURCES := $(foreach d,$(OBJ_LIST),$(LIBS_DIR)/$(d)/$(SRC_DIR)/$(subst -,_,$(d)).asm)


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
	@printf "%s" "Stripping object files..."
	@$(STRIP) --strip-debug $(OBJECTS) || true; 
	@$(STRIP) --strip-unneeded $(OBJECTS) || true; 
	@echo "Ok";		
	@printf "%s" "Creating static library $(TARGET_LIB)..."	
	@$(AR) $(ARFLAGS) $(TARGET_LIB) $(OBJECTS)
	@echo "Ok";	
	@printf "%s" "Indexing static library..."
	@ranlib $(TARGET_LIB)
	@echo "Ok";	

# Правило для сборки объектных файлов: рекурсивно вызываем make в сабмодулях
$(OBJECTS): $(ASM_SOURCES)
	@echo "Building submodules... (CONFIG=$(CONFIG))... "
	@$(foreach d,$(OBJ_LIST), \
	  (echo "\tBuild for $(d) ..." && $(MAKE) -C $(LIBS_DIR)/$(d) -s build CONFIG=release ) || echo "\n\t\t⚠️  $(d) не имеет правила build\n"; \
	)
	@echo "Ok";		

# --- Utility Targets ---

$(BUILD_DIR):
	@$(MKDIR) $@

lint:
	@echo "Lint submodules... (CONFIG=$(CONFIG))... "
	#@$(MAKE) -s -C $(LIBS_DIR)/bignum-shift-left lint CONFIG=release
	#@$(MAKE) -s -C $(LIBS_DIR)/bignum-shift-right lint CONFIG=release
	@$(foreach d,$(OBJ_LIST), \
	  (echo "\nLint for $(d) ...\n\t" && $(MAKE) -C $(LIBS_DIR)/$(d) -s lint CONFIG=release COMMON_INCLUDE_DIR=$(LIBS_DIR)/common/$(INCLUDE_DIR) ) || echo "⚠️  $(d) не имеет правила clean"; \
	)	
	@echo "Ok";	
	@echo "Running static analysis on C source files..."		
	@$(CPPCHECK) --std=c11 --enable=all --error-exitcode=1 --suppress=missingIncludeSystem \
	    --inline-suppr --inconclusive --check-config \
	    -I$(INCLUDE_DIR) -I$(COMMON_DIR)/$(INCLUDE_DIR) \
	    $(TESTS_DIR)/  $(DIST_DIR)/ 
	@echo "Ok";		    

clean:
	@echo "Cleaning up main project..."
	@rm -rf $(BUILD_DIR) $(DIST_DIR)
	@echo "Ok";		
	@echo "Cleaning up submodule artifacts:" ;
	@$(foreach d,$(OBJ_LIST), \
	  (printf "%s" "Clean for $(d) : " && $(MAKE) -C $(LIBS_DIR)/$(d) -s clean) || echo "\n\t\t⚠️  $(d) не имеет правила clean\n"; \
	)
	@echo "Ok";		

help:
	@echo "Available targets:"
	@echo "  all/build  - Build the static library libbignum.a."
	@echo "  install    - Copy library and headers to dist/ directory."
	@echo "  dist       - Packages the product into the 'dist/' directory for external use." 	
	@echo "  clean      - Remove all generated files from main project and submodules."
	@echo "  lint       - Running static analysis on C source files"
	@echo "  help       - Shows this help message."	

# Тестовый таргет для вычисляемых переменных
.PHONY: show-calc
show-calc:
	@echo "REPOSITORY_NAME = $(REPOSITORY_NAME)"
	@echo "FAMILY_NAME = $(FAMILY_NAME)"	
	@echo "LIB_NAME = $(LIB_NAME)"
	@echo "UPPER_LIB_NAME = $(UPPER_LIB_NAME)"	
	@echo "NP = $(NP)"	
	@echo "SUBMODULES = $(SUBMODULES)"
	@echo "OBJ_LIST = $(OBJ_LIST)"
	@echo "OBJECTS = $(OBJECTS)"
	@echo "ASM_SOURCES = $(ASM_SOURCES)"			
