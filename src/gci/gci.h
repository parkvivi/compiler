#ifndef GCI_H
#define GCI_H

#include "ast.h"

typedef struct {
    char *operacao;
    char *argumento1;
    char *argumento2;
    char *resultado;
} Quadrupla;

extern Quadrupla *quads;
extern int quads_cont;
extern char *funcao_atual;
extern int temp_cont;
extern int label_cont;

char* novoTemp();
char* novoLabel();
void emitir_quads(const char *op, const char *a1, const char *a2, const char *res);
void limpa_quads();
char* gerar_quads(AST* no);
void print_quads(FILE *out);

#endif