#ifndef GCI_H
#define GCI_H

#include "ast.h"

typedef enum { Livre, ValorTemporario, AssociadoVariavel } EstadoTemporario;

typedef enum OperacaoQuadrupla {
    ADD,
    SUB,
    MUL,
    DIV,
    ASSIGN,
    LT,
    LE,
    GT,
    GE,
    EQ,
    NEQ,
    FUNCTION,
    CALL,
    ALLOC,
    PARAM,
    ARG,
    RETURN,
    END,
    IFF,
    GOTO,
    LABEL,
    LOAD,
    STORE,
    HALT,
    PUSH,
    POP,
    MOVE,
    NONE
} OperacaoQuadrupla;

typedef struct Temporario {
    EstadoTemporario estado;
    int nivelEscopo;
    Simbolo* variavel;
    bool preservado;
} Temporario;

typedef struct Quadrupla {
    OperacaoQuadrupla operacao;
    char *argumento1;
    char *argumento2;
    char *argumento3;
    struct Quadrupla* prox;
} Quadrupla;

extern int label_cont;
extern Quadrupla* lista_quadruplas;

void inicializaTemporarios();
char* novoTemporario(Simbolo* variavel, int nivelEscopo);
void reassociarTemporario(const char* temporario, Simbolo* variavel, int nivelEscopo);
int liberarTemporario(const char* temporario, bool liberarTemporario);
int temporarioParaNumero(const char* temporario);
char* novoLabel();
void emitirQuadrupla(OperacaoQuadrupla op, const char *a1, const char *a2, const char *a3);
void liberarQuadruplas();
char* gerarQuadruplas(AST* no, char* nomeFuncaoAtual, bool percorrerIrmaos, int nivelEscopo, bool reutilizaAtribuicao);
void imprimirQuadruplas(FILE *out);
void gerarCodigoIntermediario();

#endif