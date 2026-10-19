LEX      = flex
BISON    = bison
CC       = gcc
CFLAGS   = -Wall -Wextra -Isrc/ast -Isrc/dot -Isrc/gci -Isrc/semantico -Isrc/main -Isrc/assembly -Isrc/executavel
DOT      = dot

BUILD_DIR = build
PROGRAMS_DIR = user_programs

LEX_SRC   = src/lexico/lexico.l
BISON_SRC = src/sintatico/sintatico.y
MODULE_SRCS = src/ast/ast.c src/dot/dot.c src/gci/gci.c src/semantico/semantico.c src/semantico/tabsimbolos.c src/assembly/assembly.c src/executavel/executavel.c src/main/caminho_saida.c src/main/main.c
MODULE_HDRS = src/ast/ast.h src/dot/dot.h src/gci/gci.h src/semantico/semantico.h src/semantico/tabsimbolos.h src/assembly/assembly.h src/executavel/executavel.h src/main/caminho_saida.h src/main/main.h

PROGRAM_SRCS := $(wildcard $(PROGRAMS_DIR)/*/*.c)
PROGRAM_DIRS := $(sort $(dir $(PROGRAM_SRCS)))
PROGRAM_NAMES := $(notdir $(patsubst %/,%,$(PROGRAM_DIRS)))
PROGRAM_TARGETS := $(addprefix run-,$(PROGRAM_NAMES))

ifeq ($(OS),Windows_NT)
	BISON = C:\FLEXWI~1\bison\bin\bison.exe
	MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	PROGRAM_OUTPUT_DIR = $(BUILD_DIR)\user_programs
	PARSER_BIN = $(BUILD_DIR)\parser.exe
	LEX_OUTPUT = $(BUILD_DIR)\lex.yy.c
	BISON_OUTPUT = $(BUILD_DIR)\sintatico.tab.c
	BISON_HEADER = $(BUILD_DIR)\sintatico.tab.h
else
	MKDIR = mkdir -p $(BUILD_DIR)
	PROGRAM_OUTPUT_DIR = $(BUILD_DIR)/user_programs
	PARSER_BIN = $(BUILD_DIR)/parser
	LEX_OUTPUT = $(BUILD_DIR)/lex.yy.c
	BISON_OUTPUT = $(BUILD_DIR)/sintatico.tab.c
	BISON_HEADER = $(BUILD_DIR)/sintatico.tab.h
endif

.PHONY: all parser run run-user-programs clean $(PROGRAM_TARGETS)

all: $(PARSER_BIN)

parser: $(PARSER_BIN)

run: run-user-programs

run-user-programs: $(PROGRAM_TARGETS)

$(PARSER_BIN): $(LEX_OUTPUT) $(BISON_OUTPUT) $(MODULE_SRCS) $(MODULE_HDRS)
	$(CC) $(CFLAGS) -o $@ $(LEX_OUTPUT) $(BISON_OUTPUT) $(MODULE_SRCS)

$(LEX_OUTPUT): $(LEX_SRC) $(BISON_HEADER) | $(BUILD_DIR)
	$(LEX) -o$@ $<

$(BISON_OUTPUT) $(BISON_HEADER) &: $(BISON_SRC) | $(BUILD_DIR)
	$(BISON) -d -o $(BISON_OUTPUT) --defines=$(BISON_HEADER) $(BISON_SRC)

$(BUILD_DIR):
	$(MKDIR)

define PROGRAM_template
run-$(1): $(PARSER_BIN)
ifeq ($(OS),Windows_NT)
	if not exist $(PROGRAMS_DIR)\$(1) mkdir $(PROGRAMS_DIR)\$(1)
	set "COMPILER_OUTPUT_DIR=$(PROGRAMS_DIR)\$(1)" && $(PARSER_BIN) $(2)
	$(DOT) -Tpng $(PROGRAMS_DIR)\$(1)\arvore.dot -o $(PROGRAMS_DIR)\$(1)\$(1).png
else
	mkdir -p $(PROGRAMS_DIR)/$(1)
	COMPILER_OUTPUT_DIR=$(PROGRAMS_DIR)/$(1) $(PARSER_BIN) $(2)
	$(DOT) -Tpng $(PROGRAMS_DIR)/$(1)/arvore.dot -o $(PROGRAMS_DIR)/$(1)/$(1).png
endif
endef

$(foreach src,$(PROGRAM_SRCS),$(eval $(call PROGRAM_template,$(notdir $(patsubst %/,%,$(dir $(src)))),$(src))))

clean:
ifeq ($(OS),Windows_NT)
	if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
else
	rm -rf $(BUILD_DIR)
endif
