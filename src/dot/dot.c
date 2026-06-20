#include "dot.h" 
#include "caminho_saida.h"

#include <stdlib.h>
#include <string.h>

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
        case TipoDeclaracaoSelecao:     return  "if";
        case TipoDeclaracaoIteracao:    return  "while";
        case TipoDeclaracaoRetorno:     return  "return";
        case TipoEscopo:                return  "{}";
        case TipoVetor:                 return  "[]";
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

const char* obterEstiloNo(AST* no) {

    switch (no->tipo) {
        case TipoDeclaracaoFuncao:
            return "shape=box, style=\"filled,rounded\", fillcolor=\"#D1E8E2\", color=\"#116466\"";
            
        case TipoDeclaracaoVariavel:
        case TipoParametro:
            return "shape=box, style=\"filled,rounded\", fillcolor=\"#ffddc4\", color=\"#ff9c55\"";

        case TipoDeclaracaoSelecao:
        case TipoDeclaracaoIteracao:
        case TipoDeclaracaoRetorno:
            return "shape=diamond, style=filled, fillcolor=\"#ffd1ff\", color=\"#ff68ff\"";

        case TipoChamadaFuncao:
            return "shape=ellipse, style=filled, fillcolor=\"#E8D7FF\", color=\"#9B5DE5\"";

        case TipoAtribuicao:
        case TipoMenorIgual:
        case TipoMenor:
        case TipoMaiorIgual:
        case TipoMaior:
        case TipoIgual:
        case TipoDiferente:
        case TipoSoma:
        case TipoSubtracao:
        case TipoMultiplicacao:
        case TipoDivisao:
            return "shape=circle, style=filled, fillcolor=\"#FFF3CD\", color=\"#FFC107\", width=0.4, fixedsize=true";

        case TipoNum:
            return "shape=square, style=filled, fillcolor=\"#d6ffd7\", color=\"#2dbc30\", fontcolor=\"#2B2D42\"";

        case TipoVar:
            return "shape=square, style=filled, fillcolor=\"#d1e6ff\", color=\"#114b8d\", fontcolor=\"#2B2D42\"";

        case TipoVetor:
            return "shape=square, style=filled, fillcolor=\"#ffe3e3\", color=\"#ff8c8c\"";

        default:
            return "shape=ellipse, style=filled, fillcolor=\"#F4F4F9\", color=\"#949496\"";
    }
}

void imprimirNoDOT(AST* no, FILE* file) {

    if (!no) return;

    AST* atual = no;
    while (atual) {
        fprintf(file, "  node%p [label=\"%s\", %s];\n", (void*)atual, nomeTipoNo(atual), obterEstiloNo(atual));
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
            fprintf(file, "  node%p -> node%p [color=\"#888888\", style=dashed, weight=1, dir=none];\n", (void*)atual, (void*)atual->irmao);
            atual = atual->irmao;
        }
    }

    atual = no;
    while (atual) {
        AST** ordemFilhos = NULL;
        int qtdFilhos = 0;

        for (int i=0; i<MAXFILHOS; i++) {
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
            for (int i=0; i<qtdFilhos; i++) {
                fprintf(file, "node%p; ", (void*)ordemFilhos[i]);
            }

            fprintf(file, "}\n");

            for (int i=0; i<qtdFilhos-1; i++) {
                fprintf(file, "  node%p -> node%p [style=invis, weight=1];\n", (void*)ordemFilhos[i], (void*)ordemFilhos[i + 1]);
            }
        }

        free(ordemFilhos);

        atual = atual->irmao;
    }
}



void gerarDOT(AST* raiz) {

    FILE *file;
    char caminhoSaida[260];
    montarCaminhoSaida(caminhoSaida, sizeof(caminhoSaida), "arvore.dot");

    file = fopen(caminhoSaida, "w");
    if (file == NULL) {
        printf("Erro ao abrir arquivo para gerar arquivo.dot em '%s'\n", caminhoSaida);
        return;
    }
    
    fprintf(file, "digraph AST {\n");
    fprintf(file, "  ordering=out;\n");
    fprintf(file, "  fontname=\"Courier New Bold\";\n");
    fprintf(file, "  background=\"#e4e4e4\";\n");
    fprintf(file, "  node [fontname=\"Courier New Bold\", fontsize=11, penwidth=1.5];\n");
    fprintf(file, "  edge [fontname=\"Courier New Bold\", fontsize=10, color=\"#4A4A4A\", penwidth=1.2];\n");
    fprintf(file, "  nodesep=0.4;\n");
    fprintf(file, "  ranksep=0.5;\n");

    imprimirNoDOT(raiz, file);

    fprintf(file, "}\n");
    fclose(file);
}