#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>

#include "dot.h"
#include "gci.h"
#include "semantico.h"

extern FILE* yyin;
extern int linha_atual;
extern int yyparse();

#endif