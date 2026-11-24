LEX      = flex
BISON    = bison
CC       = gcc
CFLAGS   = -Wall -Wextra -Isrc/ast -Isrc/dot -Isrc/gci -Isrc/semantico -Isrc/main -Isrc/assembly -Isrc/executavel
DOT      = dot

LEX_SRC   = src/lexico/lexico.l
BISON_SRC = src/sintatico/sintatico.y
BUILD_DIR = build
OS_DIR = os
BIOS_DIR = bios
TESTE_DIR = teste
MODULE_SRCS = src/ast/ast.c src/dot/dot.c src/gci/gci.c src/semantico/semantico.c src/semantico/tabsimbolos.c src/assembly/assembly.c src/executavel/executavel.c src/main/caminho_saida.c src/main/main.c


ifeq ($(OS),Windows_NT)
    MKDIR_BUILD = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
    MKDIR_OS = if not exist $(OS_DIR) mkdir $(OS_DIR)
    MKDIR_BIOS = if not exist $(BIOS_DIR) mkdir $(BIOS_DIR)
    MKDIR_TESTE = if not exist $(TESTE_DIR) mkdir $(TESTE_DIR)
    BISON = C:\FLEXWI~1\bison\bin\bison.exe

    LEX_OUTPUT   = $(BUILD_DIR)\lex.yy.c
    BISON_OUTPUT = $(BUILD_DIR)\sintatico.tab.c
    BISON_HEADER = $(BUILD_DIR)\sintatico.tab.h

    PARSER_BIN = $(BUILD_DIR)\parser.exe
    OS_INPUT  = os\os.c
    BIOS_INPUT = bios\bios.c
	TESTE_INPUT = teste\teste.c
else
    MKDIR_BUILD = mkdir -p $(BUILD_DIR)
    MKDIR_OS = mkdir -p $(OS_DIR)
    MKDIR_BIOS = mkdir -p $(BIOS_DIR)
    MKDIR_TESTE = mkdir -p $(TESTE_DIR)

    LEX_OUTPUT   = $(BUILD_DIR)/lex.yy.c
    BISON_OUTPUT = $(BUILD_DIR)/sintatico.tab.c
    BISON_HEADER = $(BUILD_DIR)/sintatico.tab.h

    PARSER_BIN = $(BUILD_DIR)/parser
    OS_INPUT  = os/os.c
    BIOS_INPUT = bios/bios.c
	TESTE_INPUT = teste/teste.c
endif


ifeq ($(OS),Windows_NT)
run: $(PARSER_BIN)
	$(MKDIR_BUILD)
	$(MKDIR_OS)
	set "COMPILER_OUTPUT_DIR=$(OS_DIR)" && $(PARSER_BIN) $(OS_INPUT)
	$(MKDIR_BIOS)
	set "COMPILER_OUTPUT_DIR=$(BIOS_DIR)" && $(PARSER_BIN) $(BIOS_INPUT)
	$(MKDIR_TESTE)
	set "COMPILER_OUTPUT_DIR=$(TESTE_DIR)" && $(PARSER_BIN) $(TESTE_INPUT)

run-os: $(PARSER_BIN)
	$(MKDIR_BUILD)
	$(MKDIR_OS)
	set "COMPILER_OUTPUT_DIR=$(OS_DIR)" && $(PARSER_BIN) $(OS_INPUT)

run-bios: $(PARSER_BIN)
	$(MKDIR_BUILD)
	$(MKDIR_BIOS)
	set "COMPILER_OUTPUT_DIR=$(BIOS_DIR)" && $(PARSER_BIN) $(BIOS_INPUT)

run-teste: $(PARSER_BIN)
	$(MKDIR_BUILD)
	$(MKDIR_TESTE)
	set "COMPILER_OUTPUT_DIR=$(TESTE_DIR)" && $(PARSER_BIN) $(TESTE_INPUT)
else
run: $(PARSER_BIN)
	$(MKDIR_BUILD)
	$(MKDIR_OS)
	COMPILER_OUTPUT_DIR=$(OS_DIR) $(PARSER_BIN) $(OS_INPUT)
	$(MKDIR_BIOS)
	COMPILER_OUTPUT_DIR=$(BIOS_DIR) $(PARSER_BIN) $(BIOS_INPUT)
	$(MKDIR_TESTE)
	COMPILER_OUTPUT_DIR=$(TESTE_DIR) $(PARSER_BIN) $(TESTE_INPUT)

run-os: $(PARSER_BIN)
	$(MKDIR_BUILD)
	$(MKDIR_OS)
	COMPILER_OUTPUT_DIR=$(OS_DIR) $(PARSER_BIN) $(OS_INPUT)

run-bios: $(PARSER_BIN)
	$(MKDIR_BUILD)
	$(MKDIR_BIOS)
	COMPILER_OUTPUT_DIR=$(BIOS_DIR) $(PARSER_BIN) $(BIOS_INPUT)

run-teste: $(PARSER_BIN)
	$(MKDIR_BUILD)
	$(MKDIR_TESTE)
	COMPILER_OUTPUT_DIR=$(TESTE_DIR) $(PARSER_BIN) $(TESTE_INPUT)
endif


$(PARSER_BIN): $(LEX_OUTPUT) $(BISON_OUTPUT) $(MODULE_SRCS)
	$(CC) $(CFLAGS) -o $@ $(LEX_OUTPUT) $(BISON_OUTPUT) $(MODULE_SRCS)

$(LEX_OUTPUT): $(LEX_SRC) $(BISON_HEADER)
	$(MKDIR_BUILD)
	$(LEX) -o$(LEX_OUTPUT) $<

$(BISON_OUTPUT) $(BISON_HEADER) &: $(BISON_SRC)
	$(MKDIR_BUILD)
	$(BISON) -d -o $(BISON_OUTPUT) --defines=$(BISON_HEADER) $(BISON_SRC)

ifeq ($(OS),Windows_NT)
clean:
	if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
	if exist $(OS_DIR)\arvore.dot del /Q $(OS_DIR)\arvore.dot
	if exist $(OS_DIR)\tabsimbolos.txt del /Q $(OS_DIR)\tabsimbolos.txt
	if exist $(OS_DIR)\codigointermediario.txt del /Q $(OS_DIR)\codigointermediario.txt
	if exist $(OS_DIR)\assembly.txt del /Q $(OS_DIR)\assembly.txt
	if exist $(OS_DIR)\executavel.txt del /Q $(OS_DIR)\executavel.txt
	if exist $(BIOS_DIR)\arvore.dot del /Q $(BIOS_DIR)\arvore.dot
	if exist $(BIOS_DIR)\tabsimbolos.txt del /Q $(BIOS_DIR)\tabsimbolos.txt
	if exist $(BIOS_DIR)\codigointermediario.txt del /Q $(BIOS_DIR)\codigointermediario.txt
	if exist $(BIOS_DIR)\assembly.txt del /Q $(BIOS_DIR)\assembly.txt
	if exist $(BIOS_DIR)\executavel.txt del /Q $(BIOS_DIR)\executavel.txt
	if exist $(TESTE_DIR)\arvore.dot del /Q $(TESTE_DIR)\arvore.dot
	if exist $(TESTE_DIR)\tabsimbolos.txt del /Q $(TESTE_DIR)\tabsimbolos.txt
	if exist $(TESTE_DIR)\codigointermediario.txt del /Q $(TESTE_DIR)\codigointermediario.txt
	if exist $(TESTE_DIR)\assembly.txt del /Q $(TESTE_DIR)\assembly.txt
	if exist $(TESTE_DIR)\executavel.txt del /Q $(TESTE_DIR)\executavel.txt
else
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(OS_DIR)/arvore.dot $(OS_DIR)/tabsimbolos.txt $(OS_DIR)/codigointermediario.txt $(OS_DIR)/assembly.txt $(OS_DIR)/executavel.txt
	rm -f $(BIOS_DIR)/arvore.dot $(BIOS_DIR)/tabsimbolos.txt $(BIOS_DIR)/codigointermediario.txt $(BIOS_DIR)/assembly.txt $(BIOS_DIR)/executavel.txt
	rm -f $(TESTE_DIR)/arvore.dot $(TESTE_DIR)/tabsimbolos.txt $(TESTE_DIR)/codigointermediario.txt $(TESTE_DIR)/assembly.txt $(TESTE_DIR)/executavel.txt

endif

.PHONY: all parser run run-os run-bios run-teste clean

all: $(PARSER_BIN)

parser: $(PARSER_BIN)