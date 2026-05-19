#include "gci.h"

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
    char* argumento1;
    char* argumento2;
    char* argumento3;
    struct Instrucao* prox;
} Instrucao;