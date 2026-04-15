#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "ast.h"
#include "tabsimbolos.h"

TipoSimbolo retornaTipo(AST* no);
void criarTabSimbolosInterno(AST* no, FILE* arquivo, int nivelEscopo, int* proximoIdEscopo);
void criarTabSimbolos(AST* no, FILE* arquivo);
void analisarSemantica();
void avisoSemantico(const char*);
void erroSemantico(const char*);

#endif