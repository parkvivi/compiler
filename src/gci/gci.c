#include "gci.h"
#include <string.h>

#define ARQUIVO_CODIGO_INTERMEDIARIO "build/codigointermediario.txt"
#define MAX_TEMPORARIOS 32
#define LR 31   // link register
#define SP 30   // stack pointer
#define FP 29   // frame pointer
#define RR 0    // return register

Quadrupla* lista_quadruplas = NULL;
Quadrupla* ultima_quadrupla = NULL;
bool temporariosOcupados[MAX_TEMPORARIOS];
int label_cont = 0;

void inicializaTemporarios() {
    for (int i=0; i<MAX_TEMPORARIOS; i++) {
        if (i == FP || i == SP || i == LR) {
            temporariosOcupados[i] = true;
            continue;
        }

        temporariosOcupados[i] = false;
    }
}

bool ehTemporario(const char* candidato) {

    if (candidato != NULL && candidato[0] == '$') {
        return true;
    }

    return false;
}

char* novoTemporario() {

    for (int i=RR+1; i<FP; i++) {
        if (temporariosOcupados[i] == false) {
            char* novoTemporario = malloc(sizeof(char)*20);
            sprintf(novoTemporario, "$R%d", i);

            temporariosOcupados[i] = true;

            return novoTemporario;
        }
    }

    fprintf(stderr, "GCI :: Erro ao criar novos temporarios");
    return NULL;
}

void liberarTemporario(const char* temporario) {

    int indiceTemporario = temporarioParaNumero(temporario);
    if (indiceTemporario < 0 || indiceTemporario >= MAX_TEMPORARIOS) {
        fprintf(stderr, "GCI :: Erro ao liberar temporarios");
        return;
    }

    temporariosOcupados[indiceTemporario] = false;
}

int temporarioParaNumero(const char* temporario) {

    for (int i=0; i<MAX_TEMPORARIOS; i++) {
        char buffer[20];
        sprintf(buffer, "$R%d", i);

        if (strcmp(temporario, buffer) == 0) {
            return i;
        }
    }

    return -1;
}

char* novoLabel() {

    char* novoLabel = malloc(sizeof(char)*20);
    sprintf(novoLabel, "L%d", label_cont++);

    return novoLabel;
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

    novaQuadrupla->operacao   = ((op != NULL) ? strdup(op) : NULL);
    novaQuadrupla->argumento1 = ((a1 != NULL) ? strdup(a1) : NULL);
    novaQuadrupla->argumento2 = ((a2 != NULL) ? strdup(a2) : NULL);
    novaQuadrupla->argumento3 = ((a3 != NULL) ? strdup(a3) : NULL);
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
                ((quad->operacao   != NULL) ? quad->operacao   : "-"),
                ((quad->argumento1 != NULL) ? quad->argumento1 : "-"),
                ((quad->argumento2 != NULL) ? quad->argumento2 : "-"),
                ((quad->argumento3 != NULL) ? quad->argumento3 : "-")
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
            char* tipoFuncaoDeclarada = ((no->simbolo->tipo == TipoInt) ? "int" : "void");

            emitirQuadrupla("FUNCTION", nomeFuncaoDeclarada, tipoFuncaoDeclarada, NULL);

            char* argTipoFuncao = gerarQuadruplas(no->filhos[1], nomeFuncaoDeclarada, true);
            if (ehTemporario(argTipoFuncao) == true) {
                liberarTemporario(argTipoFuncao);
            }

            char* argParametrosFuncao = gerarQuadruplas(no->filhos[2], nomeFuncaoDeclarada, true);
            if (ehTemporario(argParametrosFuncao) == true) {
                liberarTemporario(argParametrosFuncao);
            }

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
            if (ehTemporario(condicaoIteracao) == true) {
                liberarTemporario(condicaoIteracao);
            }

            char* argEscopoIteracao = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true);
            if (ehTemporario(argEscopoIteracao) == true) {
                liberarTemporario(argEscopoIteracao);
            }

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

            if (ehTemporario(condicaoSelecao) == true) {
                liberarTemporario(condicaoSelecao);
            }

            char* argEscopoSelecao = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true);
            if (ehTemporario(argEscopoSelecao) == true) {
                liberarTemporario(argEscopoSelecao);
            }
            emitirQuadrupla("GOTO", NULL, NULL, labelFinalSelecao);
            emitirQuadrupla("LABEL", NULL, NULL, ((labelElse == NULL) ? labelFinalSelecao : labelElse));

            if (labelElse != NULL) {
                char* argElse = gerarQuadruplas(no->filhos[2], nomeFuncaoAtual, true);
                if (ehTemporario(argElse) == true) {
                    liberarTemporario(argElse);
                }
                emitirQuadrupla("GOTO", NULL, NULL, labelFinalSelecao);
                emitirQuadrupla("LABEL", NULL, NULL, labelFinalSelecao);

                free(labelElse);
            }

            free(labelFinalSelecao);

            break;
        
        case TipoDeclaracaoRetorno:
            char* operandoRetornado = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true);

            emitirQuadrupla("RETURN", operandoRetornado, NULL, NULL);

            if (ehTemporario(operandoRetornado) == true) {
                liberarTemporario(operandoRetornado);
            }

            break;

        case TipoVar:
            if (no->filhos[0] == NULL) {
                arg = no->dado.nome;
            } else {
                char* nomeVetor = no->dado.nome;
                char* indiceAcessado = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true);
                char* destinoConteudoAcessado = novoTemporario();

                emitirQuadrupla("LOAD", nomeVetor, indiceAcessado, destinoConteudoAcessado);

                if (ehTemporario(indiceAcessado) == true) {
                    liberarTemporario(indiceAcessado);
                }

                arg = destinoConteudoAcessado;
            }
            break;
            
        case TipoNum: 
            char numero[20];
            sprintf(numero, "#%d", no->dado.valor);
            
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

                if (ehTemporario(origemArgumento) == true) {
                    liberarTemporario(origemArgumento);
                }
            }

            char quantidadeArgumentos[20];
            sprintf(quantidadeArgumentos, "%d", contaArgumentos);

            emitirQuadrupla("CALL", nomeFuncaoChamada, quantidadeArgumentos, destinoValorRetornado);

            arg = destinoValorRetornado;
            break;

        case TipoParametro:
            char* nomeParametro = no->dado.nome;
            char* indiceVetor = ((no->filhos[1] == NULL) ? NULL : "vet");
            
            emitirQuadrupla("PARAM", nomeFuncaoAtual, nomeParametro, indiceVetor);

            break;
        
        case TipoAtribuicao:
            char* operando = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true);

            AST* filhoEsquerdo = no->filhos[0];
            if (filhoEsquerdo->tipo == TipoVar) {
                char* nomeVetor = filhoEsquerdo->dado.nome;
                char* indiceAcessado = gerarQuadruplas(filhoEsquerdo->filhos[0], nomeFuncaoAtual, true);
                emitirQuadrupla("STORE", nomeVetor, indiceAcessado, operando);

                if (ehTemporario(indiceAcessado) == true) {
                    liberarTemporario(indiceAcessado);
                }

                arg = operando;
            } else {
                char* destinoAtribuicao = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true);
                emitirQuadrupla("ASSIGN", operando, NULL, destinoAtribuicao);

                if (ehTemporario(operando) == true) {
                    liberarTemporario(operando);
                }

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
            char* operacao = verificarOperacao(no->tipo);
            char* operandoEsquerdo = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true);
            char* operandoDireito = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true);
            char* destinoOperacao = NULL;

            bool esqEhTemporario = ehTemporario(operandoEsquerdo);
            bool dirEhTemporario = ehTemporario(operandoDireito);

            if (esqEhTemporario == true && dirEhTemporario == true) {
                liberarTemporario(operandoDireito);
                destinoOperacao = operandoEsquerdo;
            } else if (esqEhTemporario == true) {
                destinoOperacao = operandoEsquerdo;
            } else if (dirEhTemporario == true) {
                destinoOperacao = operandoDireito;
            } else {
                destinoOperacao = novoTemporario();
            }

            emitirQuadrupla(operacao, operandoEsquerdo, operandoDireito, destinoOperacao);

            arg = destinoOperacao;  
            break;
        
        default:
            for (int i=0; i<MAXFILHOS; i++) {
                AST* filho = no->filhos[i];
                if (filho == NULL) {
                    continue;
                }

                char* argFilhos = gerarQuadruplas(filho, nomeFuncaoAtual, true);
                if (ehTemporario(argFilhos) == true) {
                    liberarTemporario(argFilhos);
                }
            }

            break;
    }

    if (percorrerIrmaos == true) {
        char* argIrmao = gerarQuadruplas(no->irmao, nomeFuncaoAtual, true);
        if (ehTemporario(argIrmao) == true) {
            liberarTemporario(argIrmao);
        }
    }

    return arg;
}

void gerarCodigoIntermediario() {

    FILE* saida = fopen(ARQUIVO_CODIGO_INTERMEDIARIO, "w");

    if (saida == NULL) {
        fprintf(stderr, "Nao foi possivel criar o arquivo '%s'\n", ARQUIVO_CODIGO_INTERMEDIARIO);
        return;
    }

    inicializaTemporarios();
    gerarQuadruplas(raizAST, NULL, true);

    imprimirQuadruplas(saida);
    liberarQuadruplas();
}