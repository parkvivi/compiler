#include "main.h"

int main(int argc, char **argv){
    if(argc > 1){
        yyin = fopen(argv[1], "r");
        if(yyin == NULL){
            fprintf(stderr, "Problema na leitura do arquivo!");
            exit(1);
        }
    }
    else{
        yyin = fopen("flex/entrada.txt", "r");        
        if(yyin == NULL){
            fprintf(stderr, "Problema na leitura do arquivo!");
            exit(1);
        }
    }

    yyparse();
    analisarSemantica();

    gerarCodigoIntermediario();

    gerarDOT(raizAST);

    liberaAST(raizAST);
    fclose(yyin);

    return 0;
}