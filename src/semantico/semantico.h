#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "ast.h"
#include "tabsimbolos.h"

TipoSimbolo retornaTipo(AST* no);
void criarTabSimbolos(AST* no);
void verificarTipos();
void analisarSemantica();

#endif