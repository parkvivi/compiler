#ifndef GCI_H
#define GCI_H

#include "ast.h"

typedef struct Quadrupla {
    char *operacao;
    char *argumento1;
    char *argumento2;
    char *argumento3;
    struct Quadrupla* prox;
} Quadrupla;

extern int temp_cont;
extern int label_cont;

char* novoTemporario();
char* novoLabel();
void emitirQuadrupla(const char *op, const char *a1, const char *a2, const char *a3);
void liberarQuadruplas();
char* gerarQuadruplas(AST* no);
void imprimirQuadruplas(FILE *out);
void gerarCodigoIntermediario();

#endif