#ifndef GCI_H
#define GCI_H

#include "ast.h"

typedef enum { Livre, ValorTemporario, AssociadoVariavel } EstadoTemporario;

typedef struct Temporario {
    EstadoTemporario estado;
    int nivelEscopo;
    Simbolo* variavel;
} Temporario;

typedef struct Quadrupla {
    char *operacao;
    char *argumento1;
    char *argumento2;
    char *argumento3;
    struct Quadrupla* prox;
} Quadrupla;

extern int label_cont;

void inicializaTemporarios();
char* novoTemporario(Simbolo* variavel, int nivelEscopo);
void reassociarTemporario(const char* temporario, int nivelEscopo);
int liberarTemporario(const char* temporario, bool liberarTemporario);
int temporarioParaNumero(const char* temporario);
char* novoLabel();
void emitirQuadrupla(const char *op, const char *a1, const char *a2, const char *a3);
void liberarQuadruplas();
char* gerarQuadruplas(AST* no, char* nomeFuncaoAtual, bool percorrerIrmaos, int nivelEscopo);
void imprimirQuadruplas(FILE *out);
void gerarCodigoIntermediario();

#endif