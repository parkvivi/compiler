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

    /* Geração de quádruplas a partir da AST */
    /*gerar_quads(raizAST);
    printf("\nQuadruplas geradas:\n");
    print_quads(stdout);
    printf("---------------------\n> ");
    limpa_quads();*/

    gerarDOT(raizAST);

    liberaAST(raizAST);
    fclose(yyin);

    return 0;
}