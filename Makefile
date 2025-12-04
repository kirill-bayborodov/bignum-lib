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

# --- Tools ---
CC = gcc
AS = yasm
PERF = /usr/local/bin/perf
RM = rm -rf
MKDIR = mkdir -p
AR = ar
STRIP = strip
RL = ranlib
CPPCHECK = cppcheck
OBJCOPY = objcopy
NM = nm
CP = cp

# --- Directories ---
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
LIBS_DIR = libs
TESTS_DIR = tests
BENCH_DIR = benchmarks
INCLUDE_DIR = include
DIST_DIR = dist

COMMON_NAME := $(FAMILY_NAME)-common
COMMON_DIR  := $(LIBS_DIR)/$(COMMON_NAME)
REPORTS_DIR = $(BENCH_DIR)/reports
DIST_INCLUDE_DIR = $(DIST_DIR)/$(INCLUDE_DIR)
DIST_LIB_DIR = $(DIST_DIR)/$(LIBS_DIR)

# Собираем список всех объектных файлов, которые должны войти в библиотеку
SUBMODULES  := $(patsubst $(LIBS_DIR)/%/,%,$(filter %/,$(wildcard $(LIBS_DIR)/*/)))
SUBMODULES_INCLUDE_DIR := $(foreach d,$(SUBMODULES),$(LIBS_DIR)/$(d)/$(INCLUDE_DIR))
OBJ_LIST    := $(filter-out $(COMMON_NAME),$(patsubst $(LIBS_DIR)/%/,%,$(filter %/,$(wildcard $(LIBS_DIR)/*/))))
OBJECTS     := $(foreach d,$(OBJ_LIST),$(LIBS_DIR)/$(d)/$(BUILD_DIR)/$(subst -,_,$(d)).o)
ASM_SOURCES := $(foreach d,$(OBJ_LIST),$(LIBS_DIR)/$(d)/$(SRC_DIR)/$(subst -,_,$(d)).asm)
HEADERS     := $(foreach d,$(OBJ_LIST),$(LIBS_DIR)/$(d)/$(INCLUDE_DIR)/$(subst -,_,$(d)).h)
RUNNERS     := $(foreach d,$(OBJ_LIST),$(LIBS_DIR)/$(d)/$(TESTS_DIR)/test_$(subst -,_,$(d))_runner.c)


# --- Source & Target Files ---
HEADER = $(INCLUDE_DIR)/$(LIB_NAME).h
TEST_BINS = $(patsubst $(TESTS_DIR)/%.c, $(BIN_DIR)/%, $(wildcard $(TESTS_DIR)/*.c))

# --- Target Files ---
# Имя финальной статической библиотеки
STATIC_LIB = $(DIST_DIR)/lib$(LIB_NAME).a
# Имя финального единого заголовочного файла
SINGLE_HEADER = $(INCLUDE_DIR)/$(LIB_NAME).h
# Имя финального единого тест-ранера
TEST_NAME = $(TESTS_DIR)/test_$(LIB_NAME)_runner.c

# --- Flags ---
CFLAGS_BASE = -std=c11 -Wall -Wextra -pedantic -I$(INCLUDE_DIR) $(addprefix -I , $(SUBMODULES_INCLUDE_DIR))
ASFLAGS_BASE = -f elf64
LDFLAGS = -no-pie -lm
AEFLAGS = rcs
NMFLAGS = -g --defined-only

ifeq ($(CONFIG), release)
    CFLAGS = $(CFLAGS_BASE) -O2 -march=native
    ASFLAGS = $(ASFLAGS_BASE)
else
    CFLAGS = $(CFLAGS_BASE) -g
    ASFLAGS = $(ASFLAGS_BASE) -g dwarf2
endif

CFLAGS += -Wl,-z,noexecstack

.PHONY: all build install test clean help

all: build

# --- Main Targets ---

# Главная цель: собрать библиотеку
build: $(OBJECTS) $(STATIC_LIB) $(HEADER)

# Цель "install" копирует библиотеку и заголовки в папку dist/
install: build
	@echo "Installing library and headers to $(DIST_DIR)/..."
	@cp $(INCLUDE_DIR)/bignum.h $(DIST_DIR)/
	@echo "Installation complete."

# Цель "dist" копирует библиотеку и заголовки в папку dist/, добавляет README и LICENSE
dist: clean install test
	@echo "Creating distribution in $(DIST_DIR)/ (CONFIG=$(CONFIG))..."
	@cp README.md $(DIST_DIR)/
	@cp LICENSE $(DIST_DIR)/
	@echo "Done."	
	@ls -l $(DIST_DIR)

# Цель для запуска интеграционных тестов
test: install 
# Компилируем тест-раннер в dist, статически линкуя библиотеку из dist и тестируем сборку с библиотекой
	@$(CP) $(TESTS_DIR)/test_$(LIB_NAME)_runner.c $(DIST_DIR)/
	@$(CC) $(DIST_DIR)/test_$(LIB_NAME)_runner.c -L$(DIST_DIR) -l$(LIB_NAME) -o $(DIST_DIR)/test_$(LIB_NAME)_runner -no-pie
	@$(DIST_DIR)/test_$(LIB_NAME)_runner	
	@$(RM) $(DIST_DIR)/test_$(LIB_NAME)_runner			

# --- Compilation Rules ---

# Правило для создания библиотеки из объектных файлов
$(STATIC_LIB): $(OBJECTS) 
	@$(MKDIR) $(DIST_DIR)		
	@printf "%s" "Stripping object files..."
	@$(STRIP) --strip-debug $(OBJECTS) || true; 
	@$(STRIP) --strip-unneeded $(OBJECTS) || true; 
	@echo "Ok";		
	@printf "%s" "Create static library lib$(LIB_NAME).a ..."
	@$(AR) $(ARFLAGS) $(STATIC_LIB) $(OBJECTS)
	@echo "Ok";	
	@printf "%s" "Indexing static library..."
	@$(RL) $(STATIC_LIB)
	@echo "Ok";	
	@$(NM) $(NMFLAGS)  $(STATIC_LIB)	

# Правило для создания объединенного заголовочного файла
$(HEADER): $(HEADERS) | $(INCLUDE_DIR)
	@echo "Creating single-file header in $(INCLUDE_DIR)/ ..."
# 4. Создаем КОРРЕКТНЫЙ единый заголовочный файл
	@printf "%s"  "Generating single-file header..."
# 4.1. Начинаем с единого include guard
	@echo "#ifndef $(UPPER_LIB_NAME)_SINGLE_H" > $(SINGLE_HEADER)
	@echo "#define $(UPPER_LIB_NAME)_SINGLE_H" >> $(SINGLE_HEADER)
	@echo "" >> $(SINGLE_HEADER)

# 4.2. Вставляем содержимое bignum.h, но БЕЗ его собственных include guards
	@echo "/* --- Included from libs/bignum-common/include/bignum.h --- */" >> $(SINGLE_HEADER)
# sed удаляет строки, содержащие BIGNUM_H
	@sed '/BIGNUM_H/d' $(COMMON_DIR)/$(INCLUDE_DIR)/$(FAMILY_NAME).h >> $(SINGLE_HEADER)
	@echo "" >> $(SINGLE_HEADER)
# 4.3. Вставляем содержимое $(LIB_NAME).h, но БЕЗ его include guards и БЕЗ #include "bignum.h"
# sed удаляет строки с BIGNUM_SHIFT_LEFT_H и #include "bignum.h"
	@$(foreach h,$(HEADERS), \
	   ( echo "/* --- Included from $(h) --- */" >> $(SINGLE_HEADER) && \
	     sed -e '/$(subst z,Z,$(subst y,Y,$(subst x,X,$(subst w,W,$(subst v,V,$(subst u,U,$(subst t,T,$(subst s,S,$(subst r,R,$(subst q,Q,$(subst p,P,$(subst o,O,$(subst n,N,$(subst m,M,$(subst l,L,$(subst k,K,$(subst j,J,$(subst i,I,$(subst h,H,$(subst g,G,$(subst f,F,$(subst e,E,$(subst d,D,$(subst c,C,$(subst b,B,$(subst a,A,$(basename $(notdir $(H)))))))))))))))))))))))))))))_H/d' -e '/#include <$(FAMILY_NAME).h>/d' $(h) >> $(SINGLE_HEADER) && \
	     echo "" >> $(SINGLE_HEADER) ); \
	)	
# 4.4. Закрываем единый include guard
	@echo "#endif // $(UPPER_LIB_NAME)_SINGLE_H" >> $(SINGLE_HEADER)
	@echo "Ok"

$(TEST_NAME): 
	@$(foreach r,$(RUNNERS), \
	   ( echo "/* --- Included from $(r) --- */" >> $(TEST_NAME) && \
	   	 sed -e '' $(r) >> $(TEST_NAME) && \
	     echo "" >> $(TEST_NAME) ); \
	)

# Правило для сборки объектных файлов: рекурсивно вызываем make в сабмодулях
$(OBJECTS): $(ASM_SOURCES)
	@echo "Building submodules... (CONFIG=$(CONFIG))... "
	@$(foreach d,$(OBJ_LIST), \
	  (echo "\tBuild for $(d) ..." && $(MAKE) -C $(LIBS_DIR)/$(d) -s build CONFIG=release CFLAGS+=-Wl,-z,noexecstack) || echo "\n\t\t⚠️  $(d) no rule build\n"; \
	)		

# --- Utility Targets ---

$(BUILD_DIR):
	@$(MKDIR) $@

$(BIN_DIR) $(REPORTS_DIR) $(INCLUDE_DIR) $(LIBS_DIR):
	@$(MKDIR) $@	

lint: $(HEADER) 
	@echo "Lint submodules... (CONFIG=$(CONFIG))... "
	@$(foreach d,$(OBJ_LIST), \
	  (echo "\nLint for $(d) ...\n\t" && $(MAKE) -C $(LIBS_DIR)/$(d) -s lint CONFIG=release ) || echo "\n\t\t⚠️  $(d) no rule lint\n"; \
	)	
	@echo "Ok";	
	@echo "Running static analysis on C source files..."		
	@$(CPPCHECK) --std=c11 --enable=all --error-exitcode=1 --suppress=missingIncludeSystem \
	    --inline-suppr --inconclusive --check-config \
	    -I$(INCLUDE_DIR) \
	    $(TESTS_DIR)/  $(DIST_DIR)/ 
	@echo "Ok";		    

clean:
	@echo "Cleaning up build artifacts (build/, bin/, dist/, include/ single-header-file)..."
	@$(RM) $(BUILD_DIR) $(BIN_DIR) $(DIST_DIR) $(INCLUDE_DIR) $(SINGLE_HEADER) 
	@echo "Cleaning up submodule artifacts:" ; 		
	@$(foreach d,$(OBJ_LIST), \
	  (printf "%s" "Clean for $(d) : " && $(MAKE) -C $(LIBS_DIR)/$(d) -s clean) || echo "\n\t\t⚠️  $(d) has no rule clean\n"; \
	)		

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
	@echo "ASM_LABELS = $(ASM_LABELS)"
	@echo "Количество меток: $(words $(subst |, ,$(ASM_LABELS)))"
	@echo "OBJECTS = $(OBJECTS)"
	@echo "OBJ_LIST = $(OBJ_LIST)"	
	@echo "ASM_SOURCES = $(ASM_SOURCES)"
	@echo "HEADERS = $(HEADERS)"			
	@echo "OBJ = $(OBJ)"
	@echo "SUBMODULES = $(SUBMODULES)"	
	@echo "SUBMODULES_INCLUDE_DIR = $(SUBMODULES_INCLUDE_DIR)"	
	@echo "ALL_HEADERS = $(foreach dir,$(SUBMODULES_INCLUDE_DIR),$(wildcard $(dir)/*.h))	"	
	@echo "RUNNERS = $(RUNNERS) "		

