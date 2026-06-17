#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include "gci.h"

#define RR "R0"  // return register
#define FP "R29" // frame pointer
#define SP "R30" // stack pointer
#define LR "R31" // link register

#define COMECO_VAR_LOCAIS 2

typedef enum {
    OP_ADD,
    OP_ADDS,
    OP_SUB,
    OP_SUBS,
    OP_RSB,
    OP_MUL,
    OP_UDIV,
    OP_NOT,
    OP_AND,
    OP_ORR,
    OP_EOR,
    OP_MOV,
    OP_CMP,
    OP_B,
    OP_BX,
    OP_BL,
    OP_LDR,
    OP_STR,
    OP_NOP,
    OP_HLT,
    OP_IN,
    OP_OUT
} OperacaoInstrucao;

typedef enum {
    SUFIXO_EQ,
    SUFIXO_NE,
    SUFIXO_CS,
    SUFIXO_CC,
    SUFIXO_MI,
    SUFIXO_PL,
    SUFIXO_VS,
    SUFIXO_VC,
    SUFIXO_HI,
    SUFIXO_LS,
    SUFIXO_GE,
    SUFIXO_LT,
    SUFIXO_GT,
    SUFIXO_LE,
    SUFIXO_AL
} SufixoCondicional;

typedef struct Instrucao {
    OperacaoInstrucao operacao;
    SufixoCondicional sufixo;
    char* label;

    char* rd;
    char* rn;
    char* rm;
    char* imediato;
    char* endereco;
    
    struct Instrucao* prox;
} Instrucao;

extern Instrucao* lista_instrucoes;
extern Instrucao* ultima_instrucao;

char* operacaoInstrucao(OperacaoInstrucao operacao);
char* sufixoInstrucao(SufixoCondicional sufixo);
void imprimirInstrucoes(FILE *arquivo);
void liberarInstrucoes();
void emitirInstrucao(OperacaoInstrucao op, SufixoCondicional suf, const char* l, const char *rd, const char *rn, const char *rm, const char *im, const char *end);
void quadruplasParaAssembly();
void gerarAssembly();

#endif