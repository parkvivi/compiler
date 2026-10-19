LEX      = flex
BISON    = bison
CC       = gcc
CFLAGS   = -Wall -Wextra -Isrc/ast -Isrc/dot -Isrc/gci -Isrc/semantico -Isrc/main -Isrc/assembly -Isrc/executavel
DOT      = dot

LEX_SRC   = src/lexico/lexico.l
BISON_SRC = src/sintatico/sintatico.y
OS_DIR = os
MODULE_SRCS = src/ast/ast.c src/dot/dot.c src/gci/gci.c src/semantico/semantico.c src/semantico/tabsimbolos.c src/assembly/assembly.c src/executavel/executavel.c src/main/caminho_saida.c src/main/main.c


ifeq ($(OS),Windows_NT)
    MKDIR = if not exist $(OS_DIR) mkdir $(OS_DIR)
    BISON = C:\FLEXWI~1\bison\bin\bison.exe

    LEX_OUTPUT   = $(OS_DIR)\lex.yy.c
    BISON_OUTPUT = $(OS_DIR)\sintatico.tab.c
    BISON_HEADER = $(OS_DIR)\sintatico.tab.h

    MOVE_LEX   = move /Y lex.yy.c $(LEX_OUTPUT)
    MOVE_BISON = move /Y sintatico.tab.c $(BISON_OUTPUT)
    MOVE_HDR   = move /Y sintatico.tab.h $(BISON_HEADER)

    PARSER_BIN = $(OS_DIR)\parser.exe
    OS_INPUT  = os\os.c
    OS_DOT    = $(OS_DIR)\arvore.dot
else
    MKDIR = mkdir -p $(OS_DIR)
    LEX_OUTPUT   = $(OS_DIR)/lex.yy.c
    BISON_OUTPUT = $(OS_DIR)/sintatico.tab.c
    BISON_HEADER = $(OS_DIR)/sintatico.tab.h

    MOVE_LEX   = mv lex.yy.c $(LEX_OUTPUT)
    MOVE_BISON = mv sintatico.tab.c $(BISON_OUTPUT)
    MOVE_HDR   = mv sintatico.tab.h $(BISON_HEADER)

    PARSER_BIN = $(OS_DIR)/parser
    OS_INPUT  = os/os.c
    OS_DOT    = $(OS_DIR)/arvore.dot
endif


ifeq ($(OS),Windows_NT)
run: $(PARSER_BIN)
	$(MKDIR)
	set "COMPILER_OUTPUT_DIR=$(OS_DIR)" && $(PARSER_BIN) $(OS_INPUT)
	$(DOT) -Tpng $(OS_DOT) -o $(OS_DIR)\os.png
else
run: $(PARSER_BIN)
	$(MKDIR)
	COMPILER_OUTPUT_DIR=$(OS_DIR) $(PARSER_BIN) $(OS_INPUT)
	$(DOT) -Tpng $(OS_DOT) -o $(OS_DIR)/os.png
endif


$(PARSER_BIN): $(LEX_OUTPUT) $(BISON_OUTPUT) $(MODULE_SRCS)
	$(CC) $(CFLAGS) -o $@ $(LEX_OUTPUT) $(BISON_OUTPUT) $(MODULE_SRCS)

$(LEX_OUTPUT): $(LEX_SRC) $(BISON_HEADER)
	$(MKDIR)
	$(LEX) -o$(LEX_OUTPUT) $<

$(BISON_OUTPUT) $(BISON_HEADER) &: $(BISON_SRC)
	$(MKDIR)
	$(BISON) -d -o $(BISON_OUTPUT) --defines=$(BISON_HEADER) $(BISON_SRC)

.PHONY: all parser run

all: $(PARSER_BIN)

parser: $(PARSER_BIN)