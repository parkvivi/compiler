#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "dot.h"
#include "gci.h"
#include "tabsimbolos.h"

extern FILE* yyin;
extern int linha_atual;
extern int yyparse(void);

#endif