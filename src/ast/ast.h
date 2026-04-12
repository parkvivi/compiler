#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    TipoDeclaracaoFuncao,
    TipoDeclaracaoVariavel,
    TipoTipoEspecificador,
    TipoNum,
    TipoVar,
    TipoEscopo,
    TipoDeclaracaoSelecao,
    TipoDeclaracaoIteracao,
    TipoDeclaracaoRetorno,
    TipoChamadaFuncao,
    TipoParametro,
    TipoParametroVetor,
    TipoAtribuicao,
    TipoMenorIgual,
    TipoMenor,
    TipoMaiorIgual,
    TipoMaior,
    TipoIgual,
    TipoDiferente,
    TipoMultiplicacao,
    TipoDivisao,
    TipoSoma,
    TipoSubtracao
} TipoNo;

#define MAXFILHOS 3

typedef struct AST {
    TipoNo tipo;
    union {
        char* nome;
        int valor;
    } dado;
    struct AST* irmao;
    struct AST* filhos[MAXFILHOS];
} AST;

extern AST *raizAST;
extern char* origem;

AST* criarNo(TipoNo tipo);
AST* anexarIrmao(AST* lista, AST* novoNo);
void liberaAST(AST* no);

#endif