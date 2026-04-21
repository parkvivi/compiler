#include "gci.h"
#include <string.h>

#define ARQUIVO_CODIGO_INTERMEDIARIO "build/codigointermediario.txt"

Quadrupla* lista_quadruplas = NULL;
Quadrupla* ultima_quadrupla = NULL;
int temp_cont = 0;
int label_cont = 0;

char* novoTemporario() {

    char* t = malloc(sizeof(char)*20);
    sprintf(t, "t%d", temp_cont++);

    return t;
}

char* novoLabel() {

    char* l = malloc(sizeof(char)*20);
    sprintf(l, "L%d", label_cont++);

    return l;
}

char* verificarOperacao(TipoNo tipo) {

    switch (tipo) {
        case TipoMenorIgual:    return "LE";
        case TipoMenor:         return "LT";
        case TipoMaiorIgual:    return "GE";
        case TipoMaior:         return "GT";
        case TipoIgual:         return "EQ";
        case TipoDiferente:     return "NEQ";
        case TipoMultiplicacao: return "MUL";
        case TipoDivisao:       return "DIV";
        case TipoSoma:          return "ADD";
        case TipoSubtracao:     return "SUB";
        default:                return NULL;
    }
}

void emitirQuadrupla(const char *op, const char *a1, const char *a2, const char *a3) {

    Quadrupla* novaQuadrupla = (Quadrupla*) malloc(sizeof(Quadrupla));

    novaQuadrupla->operacao   = op != NULL ? strdup(op) : NULL;
    novaQuadrupla->argumento1 = a1 != NULL ? strdup(a1) : NULL;
    novaQuadrupla->argumento2 = a2 != NULL ? strdup(a2) : NULL;
    novaQuadrupla->argumento3 = a3 != NULL ? strdup(a3) : NULL;
    novaQuadrupla->prox = NULL;

    if (ultima_quadrupla == NULL) {
        lista_quadruplas = novaQuadrupla;
        ultima_quadrupla = novaQuadrupla;
    } else {
        ultima_quadrupla->prox = novaQuadrupla;
        ultima_quadrupla = novaQuadrupla;
    }
}

void imprimirQuadruplas(FILE *arquivo) {

    emitirQuadrupla("HALT", NULL, NULL, NULL);

    Quadrupla* quad=lista_quadruplas;

    while (quad != NULL) {
        fprintf(arquivo, "(%s, %s, %s, %s)\n",
                quad->operacao   != NULL ? quad->operacao   : "-",
                quad->argumento1 != NULL ? quad->argumento1 : "-",
                quad->argumento2 != NULL ? quad->argumento2 : "-",
                quad->argumento3 != NULL ? quad->argumento3 : "-"
            );

        quad=quad->prox;
    }
}

void liberarQuadruplas() {

    Quadrupla* quad = lista_quadruplas;

    while (quad != NULL) {
        Quadrupla* quadAux = quad;
        quad = quad->prox;

        if (quadAux->operacao) {
            free(quadAux->operacao);
        }

        if (quadAux->argumento1) {
            free(quadAux->argumento1);
        }

        if (quadAux->argumento2) {
            free(quadAux->argumento2);
        }

        if (quadAux->argumento3) {
            free(quadAux->argumento3);
        }

        free(quadAux);
    }
}

char* gerarQuadruplas(AST* no, char* nomeFuncaoAtual, bool percorrerIrmaos){

    if (!no) return NULL;

    char* arg = NULL;

    switch (no->tipo) {
        case TipoDeclaracaoFuncao:
            char* nomeFuncaoDeclarada = no->dado.nome;
            char* tipoFuncaoDeclarada = no->simbolo->tipo == TipoInt ? "int" : "void";
            char* labelComecoFuncao   = novoLabel();

            emitirQuadrupla("FUNCTION", nomeFuncaoDeclarada, tipoFuncaoDeclarada, NULL);
            emitirQuadrupla("LABEL", NULL, NULL, labelComecoFuncao);

            gerarQuadruplas(no->filhos[1], nomeFuncaoDeclarada, true);
            gerarQuadruplas(no->filhos[2], nomeFuncaoDeclarada, true);

            emitirQuadrupla("END", nomeFuncaoDeclarada, NULL, NULL);
            break;

        case TipoDeclaracaoVariavel:
            char* nomeVariavel = no->dado.nome;
            char* tamanhoVetor = NULL;

            if (no->filhos[1] != NULL) {
                tamanhoVetor = malloc(20*sizeof(char));
                sprintf(tamanhoVetor, "%d", no->filhos[1]->dado.valor);
            }

            emitirQuadrupla("ALLOC", nomeFuncaoAtual, nomeVariavel, tamanhoVetor);

            if (tamanhoVetor != NULL) {
                free(tamanhoVetor);
            }

            break;

        case TipoDeclaracaoIteracao:
            char* labelComecoIteracao = novoLabel();
            char* labelFinalIteracao = novoLabel();
            emitirQuadrupla("LABEL", NULL, NULL, labelComecoIteracao);

            char* condicaoIteracao = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true);
            emitirQuadrupla("IFF", condicaoIteracao, NULL, labelFinalIteracao);

            gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true);
            emitirQuadrupla("GOTO", NULL, NULL, labelComecoIteracao);

            emitirQuadrupla("LABEL", NULL, NULL, labelFinalIteracao);

            free(labelComecoIteracao);
            free(labelFinalIteracao);

            break;
        
        case TipoDeclaracaoSelecao:
            char* condicaoSelecao = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true);
            char* labelElse = ((no->filhos[2] == NULL) ? NULL : novoLabel());
            char* labelFinalSelecao = novoLabel();

            emitirQuadrupla("IFF", condicaoSelecao, NULL, ((labelElse == NULL) ? labelFinalSelecao : labelElse));
            gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true);
            emitirQuadrupla("GOTO", NULL, NULL, labelFinalSelecao);
            emitirQuadrupla("LABEL", NULL, NULL, ((labelElse == NULL) ? labelFinalSelecao : labelElse));

            if (labelElse != NULL) {
                gerarQuadruplas(no->filhos[2], nomeFuncaoAtual, true);
                emitirQuadrupla("GOTO", NULL, NULL, labelFinalSelecao);
                emitirQuadrupla("LABEL", NULL, NULL, labelFinalSelecao);

                free(labelElse);
            }

            free(labelFinalSelecao);

            break;
        
        case TipoDeclaracaoRetorno:
            char* operandoRetornado = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true);

            emitirQuadrupla("RETURN", operandoRetornado, NULL, NULL);

            break;

        case TipoVar:
            if (no->filhos[0] == NULL) {
                arg = no->dado.nome;
            } else {
                char* nomeVetor = no->dado.nome;
                char* indiceAcessado = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true);
                char* destinoConteudoAcessado = novoTemporario();

                emitirQuadrupla("LOAD", nomeVetor, indiceAcessado, destinoConteudoAcessado);

                arg = destinoConteudoAcessado;
            }
            break;
            
        case TipoNum: 
            char numero[20];
            sprintf(numero, "%d", no->dado.valor);
            
            arg = strdup(numero);
            break;

        case TipoChamadaFuncao:
            char* nomeFuncaoChamada = no->dado.nome;
            int contaArgumentos = 0;
            char* destinoValorRetornado = ((no->simbolo->tipo == TipoInt) ? novoTemporario() : NULL);

            for (AST* argumento=no->filhos[0]; argumento!=NULL; argumento=argumento->irmao) {
                contaArgumentos++;
                char* origemArgumento = gerarQuadruplas(argumento, nomeFuncaoAtual, false);
                emitirQuadrupla("ARG", origemArgumento, NULL, NULL);
            }

            char quantidadeArgumentos[20];
            sprintf(quantidadeArgumentos, "%d", contaArgumentos);

            emitirQuadrupla("CALL", nomeFuncaoChamada, quantidadeArgumentos, destinoValorRetornado);

            arg = destinoValorRetornado;
            break;

        case TipoParametro:
            char* nomeParametro = no->dado.nome;
            char* indiceVetor = ((no->filhos[1] == NULL) ? NULL : "vet");
            
            // TODO: incluir alocacao de memoria aqui???
            emitirQuadrupla("PARAM", nomeFuncaoAtual, nomeParametro, indiceVetor);

            break;
        
        case TipoAtribuicao:
            char* operando = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true);

            AST* filhoEsquerdo = no->filhos[0];
            if (filhoEsquerdo->tipo == TipoVar && filhoEsquerdo->filhos[0] != NULL) {
                char* nomeVetor = filhoEsquerdo->dado.nome;
                char* indiceAcessado = gerarQuadruplas(filhoEsquerdo->filhos[0], nomeFuncaoAtual, true);
                emitirQuadrupla("STORE", nomeVetor, indiceAcessado, operando);
                arg = operando;
            } else {
                char* destinoAtribuicao = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true);
                emitirQuadrupla("ASSIGN", operando, NULL, destinoAtribuicao);
                arg = destinoAtribuicao;
            }
            
            break;

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
            char* operacao          = verificarOperacao(no->tipo);
            char* operandoEsquerdo  = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true);
            char* operandoDireito   = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true);
            char* destinoOperacao   = novoTemporario();

            emitirQuadrupla(operacao, operandoEsquerdo, operandoDireito, destinoOperacao);

            arg = destinoOperacao;  
            break;
        
        default:
            for (int i=0; i<MAXFILHOS; i++) {
                AST* filho = no->filhos[i];
                if (filho == NULL) {
                    continue;
                }

                gerarQuadruplas(filho, nomeFuncaoAtual, true);
            }

            break;
    }

    if (percorrerIrmaos == true) {
        gerarQuadruplas(no->irmao, nomeFuncaoAtual, true);
    }

    return arg;
}

void gerarCodigoIntermediario() {

    FILE* saida = fopen(ARQUIVO_CODIGO_INTERMEDIARIO, "w");

    if (saida == NULL) {
        fprintf(stderr, "Nao foi possivel criar o arquivo '%s'\n", ARQUIVO_CODIGO_INTERMEDIARIO);
        return;
    }

    gerarQuadruplas(raizAST, NULL, true);

    imprimirQuadruplas(saida);
    liberarQuadruplas();
}