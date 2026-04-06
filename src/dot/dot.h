#ifndef DOT_H
#define DOT_H

#include "ast.h"

void gerarDOT(AST* raiz);
const char* nomeTipoNo(AST* no);
void imprimirNoDOT(AST* no, FILE* file);

#endif