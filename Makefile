# Makefile for bignum-lib aggregator

CONFIG       ?= release

# --- Calculated Variables --
REPOSITORY_NAME := $(notdir $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
FAMILY_NAME := $(firstword $(subst -, ,$(REPOSITORY_NAME)))
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
BUILD_DIR = build
BIN_DIR = bin
LIBS_DIR = libs
TESTS_DIR = tests
INCLUDE_DIR = include
DIST_DIR = dist

COMMON_NAME := $(FAMILY_NAME)-common
COMMON_DIR  := $(LIBS_DIR)/$(COMMON_NAME)

# --- Submodules Logic (Universal) ---
SUBMODULES  := $(patsubst $(LIBS_DIR)/%/,%,$(filter %/,$(wildcard $(LIBS_DIR)/*/)))
SUBMODULES_INCLUDE_DIR := $(foreach d,$(SUBMODULES),$(LIBS_DIR)/$(d)/$(INCLUDE_DIR))
SUBMODULES_INCLUDE_DIR += $(foreach d,$(SUBMODULES),$(LIBS_DIR)/$(d)/$(DIST_DIR))
OBJ_LIST    := $(patsubst $(LIBS_DIR)/%/,%,$(filter %/,$(wildcard $(LIBS_DIR)/*/)))

# Отделяем сабмодули с исходниками от вендорных дистрибутивов
SRC_SUBMODULES  := $(foreach d,$(OBJ_LIST),$(if $(wildcard $(LIBS_DIR)/$(d)/Makefile),$(d),))
DIST_SUBMODULES := $(filter-out $(SRC_SUBMODULES),$(OBJ_LIST))

# Собираем OBJECTS только для тех сабмодулей, у которых реально есть исходники
OBJECTS := $(foreach d,$(SRC_SUBMODULES),$(if $(wildcard $(LIBS_DIR)/$(d)/src/$(subst -,_,$(d)).c $(LIBS_DIR)/$(d)/src/$(subst -,_,$(d)).asm),$(LIBS_DIR)/$(d)/build/$(subst -,_,$(d)).o,))

# Собираем все заголовочные файлы сабмодулей
SUBMODULES_HEADERS_RAW := $(foreach dir,$(SUBMODULES_INCLUDE_DIR),$(wildcard $(dir)/*.h))

# Выносим bignum.h на первое место, а затем добавляем все остальные файлы
SUBMODULES_HEADERS := $(filter $(COMMON_DIR)/$(INCLUDE_DIR)/$(FAMILY_NAME).h, $(SUBMODULES_HEADERS_RAW)) \
                      $(filter-out $(COMMON_DIR)/$(INCLUDE_DIR)/$(FAMILY_NAME).h, $(SUBMODULES_HEADERS_RAW))

# --- Target Files ---
STATIC_LIB = $(DIST_DIR)/lib$(LIB_NAME).a
SINGLE_HEADER = $(DIST_DIR)/$(LIB_NAME).h
TEST_NAME = $(DIST_DIR)/test_$(LIB_NAME)_runner.c

# --- Flags ---
CFLAGS_BASE = -std=c11 -Wall -Wextra -pedantic $(addprefix -I , $(SUBMODULES_INCLUDE_DIR))
LDFLAGS = -no-pie -lm

ifeq ($(CONFIG), release)
    CFLAGS = $(CFLAGS_BASE) -O2 -march=native
else
    CFLAGS = $(CFLAGS_BASE) -g
endif

CFLAGS += -Wl,-z,noexecstack

.PHONY: all build generate-header dist lint test clean help show-calc

all: build

build: $(OBJECTS)

$(OBJECTS):
	@echo "Building source submodules... (CONFIG=$(CONFIG))... "
	@$(foreach d,$(SRC_SUBMODULES), \
	(echo "\tBuild for $(d) ..." && $(MAKE) -C $(LIBS_DIR)/$(d) -s build CONFIG=release USE_ASM=auto CFLAGS+=-Wl,-z,noexecstack) || echo "\n\t\t⚠️  $(d) no rule build\n"; \
	)


generate-header:
	@$(MKDIR) $(DIST_DIR)
	@printf "%s" "Generating single-file header..."
	@echo "#ifndef $(UPPER_LIB_NAME)_SINGLE_H" > $(SINGLE_HEADER)
	@echo "#define $(UPPER_LIB_NAME)_SINGLE_H" >> $(SINGLE_HEADER)
	@echo "" >> $(SINGLE_HEADER)
	if [ -n "$(strip $(SUBMODULES_HEADERS))" ]; then \
	    echo "\n\tConcatenate submodules headers..."; \
		sed -e '/#include "$(FAMILY_NAME).h"/d' -e '/#include <$(FAMILY_NAME).h>/d' $(SUBMODULES_HEADERS) >> $(SINGLE_HEADER); \
	else \
		echo "\n\tSubmodules is empty. Use family header"; \
		sed -e '/#include "$(FAMILY_NAME).h"/d' -e '/#include <$(FAMILY_NAME).h>/d' $(FAMILY_HEADER) >> $(SINGLE_HEADER); \
	fi
	#echo "\n/* --- Included from include/$(LIB_NAME).h --- */" >> $(SINGLE_HEADER)
	#sed -e '/$(UPPER_LIB_NAME)_H/d' -e '/#include <$(FAMILY_NAME).h>/d' -e '/#include "$(FAMILY_NAME).h"/d' $(HEADER) >> $(SINGLE_HEADER)	
	@echo "" >> $(SINGLE_HEADER)
	@echo "#endif // $(UPPER_LIB_NAME)_SINGLE_H" >> $(SINGLE_HEADER)
	@echo "\n\tStep 1: Removing duplicate code blocks..."
	@awk ' \
	BEGIN { in_guard = 0; depth = 0; } \
	{ \
		stripped = $$0; sub(/^[[:space:]]*/, "", stripped); \
		if (in_guard == 1) { \
			if (stripped ~ /^#(if|ifndef|ifdef)/) { depth++; } \
			else if (stripped ~ /^#endif/) { \
				depth--; \
				if (depth == 0) in_guard = 0; \
			} \
			next; \
		} \
		if (stripped ~ /^#ifndef[[:space:]]+[A-Za-z0-9_]+/) { \
			split(stripped, p, /[[:space:]]+/); name = p[2]; \
			if (seen[name]) { in_guard = 1; depth = 1; next; } \
			seen[name] = 1; \
		} \
		print $$0; \
	}' $(SINGLE_HEADER) > $(SINGLE_HEADER)_tmp.h
	@echo "\tStep 2: Removing duplicate Doxygen blocks..."
	@awk ' \
	BEGIN { in_comment = 0; } \
	{ \
		stripped = $$0; sub(/^[[:space:]]*/, "", stripped); \
		if (in_comment == 1) { \
			if (stripped ~ /\*\//) { in_comment = 0; } \
			next; \
		} \
		if (stripped ~ /^\/\*\*/) { \
			# Мы нашли Doxygen-блок. Проверяем, видели ли мы его раньше. \
			# Чтобы идентифицировать блок, мы создаем хеш из первых двух строк. \
			block_id = stripped; \
			getline next_line; \
			block_id = block_id " " next_line; \
			\
			if (seen_doc[block_id]) { \
				in_comment = 1; \
				# Нужно пропустить текущую строку, так как она уже в block_id \
				# Но мы должны проверить, не закрылся ли комментарий сразу \
				if (next_line ~ /\*\//) { in_comment = 0; } \
				next; \
			} \
			seen_doc[block_id] = 1; \
			print stripped; \
			print next_line; \
			next; \
		} \
		print $$0; \
	}' $(SINGLE_HEADER)_tmp.h > $(SINGLE_HEADER)_final.h
	@rm -f $(SINGLE_HEADER)_tmp.h
	@mv $(SINGLE_HEADER)_final.h $(SINGLE_HEADER)
	@echo "Done. Result saved to $(SINGLE_HEADER)"		
	@echo "Ok"

dist: clean
	@echo "Creating single-file header distribution in $(DIST_DIR)/ (CONFIG=$(CONFIG))..."
	@$(MKDIR) $(DIST_DIR)
	@$(MAKE) -s build CONFIG=release
	@printf "%s" "Stripping object files..."
	@$(STRIP) --strip-debug $(OBJECTS) || true;
	@$(STRIP) --strip-unneeded $(OBJECTS) || true;
	@echo "Ok"
	@printf "%s" "Create static library lib$(LIB_NAME).a ..."
	@$(AR) rcs $(STATIC_LIB) $(OBJECTS)
	@$(foreach d,$(DIST_SUBMODULES), \
	$(MKDIR) $(BUILD_DIR)/tmp_$(d) && \
	cd $(BUILD_DIR)/tmp_$(d) && \
	$(AR) x ../../$(LIBS_DIR)/$(d)/dist/lib$(subst -,_,$(d)).a && \
	$(AR) r ../../$(STATIC_LIB) *.o && \
	cd ../..; \
	)
	@$(RL) $(STATIC_LIB)
	@echo "Ok"
	@$(NM) -g --defined-only $(STATIC_LIB)
	@$(MAKE) -s generate-header
	@cp README.md $(DIST_DIR)/ 2>/dev/null || true
	@cp LICENSE $(DIST_DIR)/ 2>/dev/null || true
	@cp $(TESTS_DIR)/test_$(LIB_NAME)_runner.c $(DIST_DIR)/
	@$(CC) $(DIST_DIR)/test_$(LIB_NAME)_runner.c -L$(DIST_DIR) -l$(LIB_NAME) -o $(DIST_DIR)/test_$(LIB_NAME)_runner -no-pie
	@$(DIST_DIR)/test_$(LIB_NAME)_runner
	@$(RM) $(DIST_DIR)/test_$(LIB_NAME)_runner
	@echo "Distribution created successfully in $(DIST_DIR)/ "
	@ls -l $(DIST_DIR)

lint: generate-header
	@echo "Lint submodules... (CONFIG=$(CONFIG))... "
	@$(foreach d,$(SRC_SUBMODULES), \
	  (for sub in $(LIBS_DIR)/$(d)/libs/*; do [ -d "$$sub" ] && $(MKDIR) "$$sub/dist"; done; \
	  echo "\nLint for $(d) ...\n\t" && $(MAKE) -C $(LIBS_DIR)/$(d) -s lint CONFIG=release ) || echo "\n\t\t⚠️  $(d) no rule lint\n"; \
	)
	@echo "Ok"
	@echo "Running static analysis on integration tests..."
	@$(CPPCHECK) --std=c11 --enable=all --error-exitcode=1 --suppress=missingIncludeSystem \
	    --inline-suppr --inconclusive --check-config \
	    -I$(DIST_DIR) \
	    $(TESTS_DIR)/
	@echo "Ok"

test: dist
	@echo "Running integration tests..."
	@$(CC) $(DIST_DIR)/test_$(LIB_NAME)_runner.c -L$(DIST_DIR) -l$(LIB_NAME) -o $(DIST_DIR)/test_$(LIB_NAME)_runner -no-pie
	@$(DIST_DIR)/test_$(LIB_NAME)_runner
	@$(RM) $(DIST_DIR)/test_$(LIB_NAME)_runner
	@echo "Integration tests passed."

clean:
	@echo "Cleaning up build artifacts (build/, bin/, dist/)..."
	@$(RM) $(BUILD_DIR) $(BIN_DIR) $(DIST_DIR)
	@echo "Cleaning up submodule artifacts:" ;
	@$(foreach d,$(OBJ_LIST), \
	if [ -f $(LIBS_DIR)/$(d)/Makefile ]; then \
	(printf "%s" "Clean for $(d) : " && $(MAKE) -C $(LIBS_DIR)/$(d) -s clean) || echo "\n\t\t⚠️  $(d) has no rule clean\n"; \
	else \
	echo "Skipping clean for $(d) (no Makefile found)"; \
	fi; \
	)

help:
	@echo "Usage: make <target> [CONFIG=release]"
	@echo ""
	@echo "Main Targets:"
	@echo "  all/build      Builds the submodules."
	@echo "  lint           Static analysis on C sources."
	@echo "  test           Builds and runs all unit tests."	
	@echo "  dist           Builds a single-header + static-lib distribution in dist/."
	@echo "  clean          Removes build/, bin/, dist/."
	@echo "  help           Shows this help message."

show-calc:
	@echo "REPOSITORY_NAME = '$(REPOSITORY_NAME)'"
	@echo "FAMILY_NAME = '$(FAMILY_NAME)'"
	@echo "OPERATION_NAME = '$(OPERATION_NAME)'"	
	@echo "LIB_NAME = '$(LIB_NAME)'"
	@echo "UPPER_LIB_NAME = '$(UPPER_LIB_NAME)'"
	@echo "NP = '$(NP)'"
	@echo "ASM_LABELS = '$(ASM_LABELS)'"
	@echo "Количество меток: $(words $(subst |, ,$(ASM_LABELS)))"
	@echo "OBJ = '$(OBJ)'"
	@echo "OBJECTS = '$(OBJECTS)'"
	@echo "C_SRC = '$(C_SRC)'"
	@echo "HEADER = '$(HEADER)'"
	@echo "FAMILY_HEADER = '$(FAMILY_HEADER)'"
	@echo "HEADERS = '$(HEADERS)'"
	@echo "SINGLE_HEADER = '$(SINGLE_HEADER)'"
	@echo "SRC_EXT = '$(SRC_EXT)'"
	@echo "USE_ASM = '$(USE_ASM)'"
	@echo "ASM_SRC = '$(ASM_SRC)'"
	@echo "SUBMODULES = '$(SUBMODULES)'"
	@echo "COMMON_NAME = '$(COMMON_NAME)'"	
	@echo "SUBMODULES_INCLUDE_DIR = '$(SUBMODULES_INCLUDE_DIR)'"
	@echo "SUBMODULES_DIST_DIR = '$(SUBMODULES_DIST_DIR)'"
	@echo "SUBMODULES_DIST_LIB = '$(SUBMODULES_DIST_LIB)'"	
	@echo "SUBMODULES_HEADERS_RAW = '$(SUBMODULES_HEADERS_RAW)'"	
	@echo "SUBMODULES_HEADERS = '$(SUBMODULES_HEADERS)'"
	@echo "TEST_BINS_MT = '$(TEST_BINS_MT)'"
	@echo "TEST_BINS = '$(TEST_BINS)'"
	@echo "SAN = $(SAN) ($(SAN_LABEL))"
	@echo "HELGRIND = $(HELGRIND)"
	@echo "SRC_SUBMODULES = '$(SRC_SUBMODULES)'"	
	@echo "DIST_SUBMODULES = '$(DIST_SUBMODULES)'"