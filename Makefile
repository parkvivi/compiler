LEX      = flex
BISON    = bison
CC       = gcc
CFLAGS   = -Wall -Wextra -Isrc/ast -Isrc/dot -Isrc/gci -Isrc/semantico -Isrc/main
DOT      = dot

LEX_SRC   = src/lexico/lexico.l
BISON_SRC = src/sintatico/sintatico.y
BUILD_DIR = build
INPUT_FILE = entrada/gcd.txt
MODULE_SRCS = src/ast/ast.c src/dot/dot.c src/gci/gci.c src/semantico/semantico.c src/semantico/tabsimbolos.c src/main/main.c

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
endif

all: $(PARSER_BIN)

parser: $(PARSER_BIN)

run: $(PARSER_BIN)
	$(PARSER_BIN) $(INPUT_FILE)
	$(DOT) -Tpng $(BUILD_DIR)/arvore.dot -o $(BUILD_DIR)/arvore.png

ifeq ($(OS),Windows_NT)
regression: $(PARSER_BIN)
	powershell -NoProfile -Command "$${cases} = @( \
	    @{f='entrada/codigo.txt'; e=0}, \
	    @{f='entrada/gcd.txt'; e=0}, \
	    @{f='entrada/sort.txt'; e=0}, \
	    @{f='entrada/lex2.c'; e=1}, \
	    @{f='entrada/par1.c'; e=1}, \
	    @{f='entrada/sem10.c'; e=1}, \
	    @{f='entrada/sem17.c'; e=1}, \
	    @{f='entrada/tac1.c'; e=0} \
	); \
	$${ok} = $$true; \
	foreach ($${c} in $${cases}) { \
	    & '$(PARSER_BIN)' $${c}.f | Out-Null; \
	    $${got} = $$LASTEXITCODE; \
	    if ($${got} -ne $${c}.e) { \
	        Write-Host ('FALHOU: ' + $${c}.f + ' esperado=' + $${c}.e + ' obtido=' + $${got}); \
	        $${ok} = $$false; \
	    } else { \
	        Write-Host ('OK: ' + $${c}.f + ' (exit=' + $${got} + ')'); \
	    } \
	}; \
	if (-not $${ok}) { exit 1 }"
else
regression: $(PARSER_BIN)
	@set -e; \
	for c in \
	  "entrada/codigo.txt:0" \
	  "entrada/gcd.txt:0" \
	  "entrada/sort.txt:0" \
	  "entrada/lex2.c:1" \
	  "entrada/par1.c:1" \
	  "entrada/sem10.c:1" \
	  "entrada/sem17.c:1" \
	  "entrada/tac1.c:0"; do \
	  file=$${c%:*}; expected=$${c#*:}; \
	  set +e; $(PARSER_BIN) $$file >/dev/null 2>&1; got=$$?; set -e; \
	  if [ $$got -ne $$expected ]; then \
	    echo "FALHOU: $$file esperado=$$expected obtido=$$got"; \
	    exit 1; \
	  else \
	    echo "OK: $$file (exit=$$got)"; \
	  fi; \
	done
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

.PHONY: all clean parser scanner run regression
