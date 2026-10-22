#ifndef GCI_H
#define GCI_H

#include "ast.h"

#define MAX_TEMPORARIOS 28

typedef enum { Livre, ValorTemporario, AssociadoVariavel } EstadoTemporario;

typedef enum OperacaoQuadrupla {
    GCI_ADD,
    GCI_SUB,
    GCI_MUL,
    GCI_DIV,
    GCI_ASSIGN,
    GCI_LT,
    GCI_LE,
    GCI_GT,
    GCI_GE,
    GCI_EQ,
    GCI_NEQ,
    GCI_FUNCTION,
    GCI_CALL,
    GCI_ALLOC,
    GCI_PARAM,
    GCI_ARG,
    GCI_RETURN,
    GCI_END,
    GCI_IFF,
    GCI_GOTO,
    GCI_LABEL,
    GCI_LOAD,
    GCI_STORE,
    GCI_HALT,
    GCI_PUSH,
    GCI_POP,
    GCI_MOVE,
    GCI_NONE,
    GCI_CARREGARPROGRAMA,
    GCI_PRESERVARCONTEXTO,
    GCI_EXECUTARPROGRAMA,
    GCI_RESTAURARCONTEXTO,
    GCI_CONFIGURARBASE
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
    Simbolo* simbolo;
    struct Quadrupla* prox;
} Quadrupla;

extern int label_cont;
extern Quadrupla* lista_quadruplas;

bool ehTemporario(const char* temporario);
void inicializaTemporarios();
char* novoTemporario(Simbolo* variavel, int nivelEscopo);
void reassociarTemporario(const char* temporario, Simbolo* variavel, int nivelEscopo);
int liberarTemporario(const char* temporario, bool liberarTemporario);
int temporarioParaNumero(const char* temporario);
char* novoLabel();
void emitirQuadrupla(OperacaoQuadrupla op, const char *a1, const char *a2, const char *a3, Simbolo* simbolo);
void liberarQuadruplas();
char* gerarQuadruplas(AST* no, char* nomeFuncaoAtual, bool percorrerIrmaos, int nivelEscopo, bool reutilizaAtribuicao);
void imprimirQuadruplas(FILE *out);
void gerarCodigoIntermediario();

#endif