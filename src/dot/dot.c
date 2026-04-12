#include "dot.h" 

const char* nomeTipoNo(AST* no) {
    static char buffer[50];
    switch (no->tipo) {
        case TipoDeclaracaoVariavel:    return  no->dado.nome;
        case TipoDeclaracaoFuncao:      return  no->dado.nome;
        case TipoTipoEspecificador:     return  no->dado.nome;
        case TipoParametro:             return  no->dado.nome;
        case TipoVar:                   return  no->dado.nome;
        case TipoChamadaFuncao:         return  no->dado.nome;
        case TipoNum:                   sprintf(buffer, "%d", no->dado.valor); return buffer;
        case TipoDeclaracaoSelecao:     return  "IF";
        case TipoDeclaracaoIteracao:    return  "WHILE";
        case TipoDeclaracaoRetorno:     return  "RETURN";
        case TipoEscopo:                return  "ESCOPO";
        case TipoParametroVetor:        return  "[]";
        case TipoAtribuicao:            return  "=";
        case TipoMenorIgual:            return  "<=";
        case TipoMenor:                 return  "<";
        case TipoMaiorIgual:            return  ">=";
        case TipoMaior:                 return  ">";
        case TipoIgual:                 return  "==";
        case TipoDiferente:             return  "!=";
        case TipoMultiplicacao:         return  "*";
        case TipoDivisao:               return  "/";
        case TipoSoma:                  return  "+";
        case TipoSubtracao:             return  "-";
        default:                        return  "???";
    }
}

void imprimirNoDOT(AST* no, FILE* file) {
    if (!no) return;

    AST* atual = no;
    while (atual) {
        fprintf(file, "  node%p[label=\"%s\"];\n", (void*)atual, nomeTipoNo(atual));
        atual = atual->irmao;
    }

    if (no->irmao) {
        fprintf(file, "  { rank=same; ");

        atual = no;
        while (atual) {
            fprintf(file, "node%p; ", (void*)atual);
            atual = atual->irmao;
        }

        fprintf(file, "}\n");

        atual = no;
        while (atual->irmao) {
            fprintf(file, "  node%p -> node%p [color=\"#189d37\", weight=100, dir=none];\n", (void*)atual, (void*)atual->irmao);
            atual = atual->irmao;
        }
    }

    atual = no;
    while (atual) {
        AST** ordemFilhos = NULL;
        int qtdFilhos = 0;

        for (int i = 0; i < MAXFILHOS; i++) {
            AST* filho = atual->filhos[i];
            if (filho) {
                AST* it = filho;
                while (it) {
                    ordemFilhos = realloc(ordemFilhos, sizeof(AST*) * (qtdFilhos + 1));
                    ordemFilhos[qtdFilhos++] = it;
                    it = it->irmao;
                }

                fprintf(file, "  node%p -> node%p;\n", (void*)atual, (void*)filho);
                imprimirNoDOT(filho, file);
            }
        }

        if (qtdFilhos > 1) {
            fprintf(file, "  { rank=same; ");
            for (int i = 0; i < qtdFilhos; i++) {
                fprintf(file, "node%p; ", (void*)ordemFilhos[i]);
            }
            fprintf(file, "}\n");

            // Arestas invisiveis para fixar ordem esquerda->direita dos filhos.
            for (int i = 0; i < qtdFilhos - 1; i++) {
                fprintf(file, "  node%p -> node%p [style=invis, weight=100];\n",
                        (void*)ordemFilhos[i], (void*)ordemFilhos[i + 1]);
            }
        }

        free(ordemFilhos);

        atual = atual->irmao;
    }
}

void gerarDOT(AST* raiz) {
    FILE *file;
    file = fopen("build/arvore.dot", "w");
    if(file==NULL) {
        printf("Erro ao abrir arquivo para gerar arquivo.dot\n");
        return;
    }
    
    fprintf(file, "digraph AST {\n");
    fprintf(file, "  ordering=out;\n");

    imprimirNoDOT(raiz, file);

    fprintf(file, "}\n");
    fclose(file);
}