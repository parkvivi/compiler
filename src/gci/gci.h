#ifndef GCI_H
#define GCI_H

#include "ast.h"

typedef struct Temporario {
    bool ocupado;
    bool representaVariavel;
    Simbolo* variavel; // ponteiro para no na tab de simbolos que representa a variavel em questao (deixar?)
} Temporario;

typedef struct Quadrupla {
    char *operacao;
    char *argumento1;
    char *argumento2;
    char *argumento3;
    struct Quadrupla* prox;
} Quadrupla;

extern int temp_cont;
extern int label_cont;

void inicializaTemporarios();
bool ehTemporario(const char* candidato);
char* novoTemporario();
void liberarTemporario(const char* temporario);
int temporarioParaNumero(const char* temporario);
char* novoLabel();
void emitirQuadrupla(const char *op, const char *a1, const char *a2, const char *a3);
void liberarQuadruplas();
char* gerarQuadruplas(AST* no, char* nomeFuncaoAtual, bool percorrerIrmaos);
void imprimirQuadruplas(FILE *out);
void gerarCodigoIntermediario();

#endif