#include "main.h"

int main(int argc, char **argv){
    
    if(argc > 1){
        yyin = fopen(argv[1], "r");
        if(yyin == NULL){
            fprintf(stderr, "Problema na leitura do arquivo!");
            exit(1);
        }
    }

    yyparse();
    analisarSemantica();

    gerarDOT(raizAST);
    gerarCodigoIntermediario();
    gerarAssembly();

    liberarTabSimbolos();
    liberaAST(raizAST);
    liberarQuadruplas();
    liberarInstrucoes();
    
    fclose(yyin);

    return 0;
}