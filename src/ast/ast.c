#include "ast.h"

AST *raizAST = NULL;
char* origem = NULL;

AST* criarNo(TipoNo tipo) {

    AST* novoNo = malloc(sizeof(AST));
    novoNo->tipo = tipo;

    novoNo->irmao = NULL;

    for (int i=0; i<MAXFILHOS; i++) {
        novoNo->filhos[i] = NULL;
    }
    
    return novoNo;
}

AST* anexarIrmao(AST* listaIrmaos, AST* novoIrmao) {

    if (listaIrmaos == NULL) return novoIrmao;

    AST* atual;
    for (atual=listaIrmaos; atual->irmao!=NULL; atual=atual->irmao);
    atual->irmao = novoIrmao;

    return listaIrmaos;
}

void liberaAST(AST* no) {

    if (!no) return;
    
    for (int i=0; i<MAXFILHOS; i++) {
        liberaAST(no->filhos[i]);
    }

    if(no->tipo == TipoVar || no->tipo == TipoDeclaracaoFuncao || no->tipo == TipoParametro || no->tipo == TipoChamadaFuncao || no->tipo == TipoTipoEspecificador) {
        free(no->dado.nome);
    }

    liberaAST(no->irmao);
    free(no);
}