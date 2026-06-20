LEX      = flex
BISON    = bison
CC       = gcc
CFLAGS   = -Wall -Wextra -Isrc/ast -Isrc/dot -Isrc/gci -Isrc/semantico -Isrc/main -Isrc/assembly -Isrc/executavel
DOT      = dot

LEX_SRC   = src/lexico/lexico.l
BISON_SRC = src/sintatico/sintatico.y
BUILD_DIR = build
MAIN_INPUT_DIR = entrada/main
MODULE_SRCS = src/ast/ast.c src/dot/dot.c src/gci/gci.c src/semantico/semantico.c src/semantico/tabsimbolos.c src/assembly/assembly.c src/executavel/executavel.c src/main/caminho_saida.c src/main/main.c

ifeq ($(OS),Windows_NT)
    MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
    RM = if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)

    BISON = C:\FLEXWI~1\bison\bin\bison.exe

    LEX_OUTPUT   = $(BUILD_DIR)\lex.yy.c
    BISON_OUTPUT = $(BUILD_DIR)\sintatico.tab.c
    BISON_HEADER = $(BUILD_DIR)\sintatico.tab.h

    MOVE_LEX   = move /Y lex.yy.c $(LEX_OUTPUT)
    MOVE_BISON = move /Y sintatico.tab.c $(BISON_OUTPUT)
    MOVE_HDR   = move /Y sintatico.tab.h $(BISON_HEADER)

    PARSER_BIN = $(BUILD_DIR)\parser.exe
    SCANNER    = $(BUILD_DIR)\scanner.exe
    GCD_INPUT  = $(MAIN_INPUT_DIR)\gcd.txt
    SORT_INPUT = $(MAIN_INPUT_DIR)\sort.txt
    FAT_INPUT  = $(MAIN_INPUT_DIR)\fatorial.txt
    GCD_DIR    = $(BUILD_DIR)\gcd
    SORT_DIR   = $(BUILD_DIR)\sort
    FAT_DIR    = $(BUILD_DIR)\fatorial
    GCD_DOT    = $(GCD_DIR)\arvore.dot
    SORT_DOT   = $(SORT_DIR)\arvore.dot
    FAT_DOT    = $(FAT_DIR)\arvore.dot
else
    MKDIR = mkdir -p $(BUILD_DIR)
    RM = rm -rf $(BUILD_DIR)

    LEX_OUTPUT   = $(BUILD_DIR)/lex.yy.c
    BISON_OUTPUT = $(BUILD_DIR)/sintatico.tab.c
    BISON_HEADER = $(BUILD_DIR)/sintatico.tab.h

    MOVE_LEX   = mv lex.yy.c $(LEX_OUTPUT)
    MOVE_BISON = mv sintatico.tab.c $(BISON_OUTPUT)
    MOVE_HDR   = mv sintatico.tab.h $(BISON_HEADER)

    PARSER_BIN = $(BUILD_DIR)/parser
    SCANNER    = $(BUILD_DIR)/scanner
    GCD_INPUT  = $(MAIN_INPUT_DIR)/gcd.txt
    SORT_INPUT = $(MAIN_INPUT_DIR)/sort.txt
    FAT_INPUT  = $(MAIN_INPUT_DIR)/fatorial.txt
    GCD_DIR    = $(BUILD_DIR)/gcd
    SORT_DIR   = $(BUILD_DIR)/sort
    FAT_DIR    = $(BUILD_DIR)/fatorial
    GCD_DOT    = $(GCD_DIR)/arvore.dot
    SORT_DOT   = $(SORT_DIR)/arvore.dot
    FAT_DOT    = $(FAT_DIR)/arvore.dot
endif

all: $(PARSER_BIN)

parser: $(PARSER_BIN)

run: run-gcd run-sort run-fatorial

ifeq ($(OS),Windows_NT)
run-gcd: $(PARSER_BIN)
	if not exist $(GCD_DIR) mkdir $(GCD_DIR)
	set "COMPILER_OUTPUT_DIR=$(GCD_DIR)" && $(PARSER_BIN) $(GCD_INPUT)
	$(DOT) -Tpng $(GCD_DOT) -o $(GCD_DIR)\gcd.png

run-sort: $(PARSER_BIN)
	if not exist $(SORT_DIR) mkdir $(SORT_DIR)
	set "COMPILER_OUTPUT_DIR=$(SORT_DIR)" && $(PARSER_BIN) $(SORT_INPUT)
	$(DOT) -Tpng $(SORT_DOT) -o $(SORT_DIR)\sort.png

run-fatorial: $(PARSER_BIN)
	if not exist $(FAT_DIR) mkdir $(FAT_DIR)
	set "COMPILER_OUTPUT_DIR=$(FAT_DIR)" && $(PARSER_BIN) $(FAT_INPUT)
	$(DOT) -Tpng $(FAT_DOT) -o $(FAT_DIR)\fatorial.png
else
run-gcd: $(PARSER_BIN)
	mkdir -p $(GCD_DIR)
	COMPILER_OUTPUT_DIR=$(GCD_DIR) $(PARSER_BIN) $(GCD_INPUT)
	$(DOT) -Tpng $(GCD_DOT) -o $(GCD_DIR)/gcd.png

run-sort: $(PARSER_BIN)
	mkdir -p $(SORT_DIR)
	COMPILER_OUTPUT_DIR=$(SORT_DIR) $(PARSER_BIN) $(SORT_INPUT)
	$(DOT) -Tpng $(SORT_DOT) -o $(SORT_DIR)/sort.png

run-fatorial: $(PARSER_BIN)
	mkdir -p $(FAT_DIR)
	COMPILER_OUTPUT_DIR=$(FAT_DIR) $(PARSER_BIN) $(FAT_INPUT)
	$(DOT) -Tpng $(FAT_DOT) -o $(FAT_DIR)/fatorial.png
endif

scanner: CFLAGS += -DLEX_STANDALONE
scanner: $(SCANNER)


$(PARSER_BIN): $(LEX_OUTPUT) $(BISON_OUTPUT) $(MODULE_SRCS)
	$(CC) $(CFLAGS) -o $@ $(LEX_OUTPUT) $(BISON_OUTPUT) $(MODULE_SRCS)

$(SCANNER): $(LEX_OUTPUT)
	$(CC) $(CFLAGS) -o $@ $(LEX_OUTPUT)

$(LEX_OUTPUT): $(LEX_SRC) $(BISON_HEADER)
	$(MKDIR)
	$(LEX) -o$(LEX_OUTPUT) $<

$(BISON_OUTPUT) $(BISON_HEADER): $(BISON_SRC)
	$(MKDIR)
	$(BISON) -d -o $(BISON_OUTPUT) --defines=$(BISON_HEADER) $(BISON_SRC)

clean:
	$(RM)

.PHONY: all clean parser scanner run run-gcd run-sort
