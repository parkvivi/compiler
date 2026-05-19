#include "assembly.h"

Instrucao* lista_instrucoes = NULL;
Instrucao* ultima_instrucao = NULL;

static void montarCaminhoSaida(char* destino, size_t tamanho, const char* nomeArquivo) {

    const char* diretorioSaida = getenv("COMPILER_OUTPUT_DIR");
    if (diretorioSaida == NULL || diretorioSaida[0] == '\0') {
        diretorioSaida = "build";
    }

    snprintf(destino, tamanho, "%s/%s", diretorioSaida, nomeArquivo);
}

char* operacaoInstrucao(OperacaoInstrucao operacao) {
    switch (operacao) {
        case OP_ADD:    return "ADD";
        case OP_ADDS:   return "ADDS";
        case OP_SUB:    return "SUB";
        case OP_SUBS:   return "SUBS";
        case OP_RSB:    return "RSB";
        case OP_MUL:    return "MUL";
        case OP_UDIV:   return "UDIV";
        case OP_NOT:    return "NOT";
        case OP_AND:    return "AND";
        case OP_ORR:    return "ORR";
        case OP_EOR:    return "EOR";
        case OP_MOV:    return "MOV";
        case OP_CMP:    return "CMP";
        case OP_B:      return "B";
        case OP_BX:     return "BX";
        case OP_BL:     return "BL";
        case OP_LDR:    return "LDR";
        case OP_STR:    return "STR";
        case OP_NOP:    return "NOP";
        case OP_HLT:    return "HLT";
        case OP_IN:     return "IN";
        case OP_OUT:    return "OUT";
        default:        return NULL;
    }
}

char* sufixoInstrucao(SufixoCondicional sufixo) {
    switch (sufixo) {
        case SUFIXO_EQ: return "EQ";
        case SUFIXO_NE: return "NE";
        case SUFIXO_CS: return "CS";
        case SUFIXO_CC: return "CC";
        case SUFIXO_MI: return "MI";
        case SUFIXO_PL: return "PL";
        case SUFIXO_VS: return "VS";
        case SUFIXO_VC: return "VC";
        case SUFIXO_HI: return "HI";
        case SUFIXO_LS: return "LS";
        case SUFIXO_GE: return "GE";
        case SUFIXO_LT: return "LT";
        case SUFIXO_GT: return "GT";
        case SUFIXO_LE: return "LE";
        default:        return NULL;
    }
}

void imprimirInstrucoes(FILE *arquivo) {

    for (Instrucao* instrucao=lista_instrucoes; instrucao!=NULL; instrucao=instrucao->prox) {

        if (instrucao->label != NULL) {
            fprintf(arquivo, "%s:\n", instrucao->label);
        }

        char* operacao = operacaoInstrucao(instrucao->operacao);
        char* sufixo = sufixoInstrucao(instrucao->sufixo);

        fprintf(arquivo, "%s", operacao);

        if (sufixo != NULL) {
            fprintf(arquivo, "%s", sufixo);
        }

        switch (instrucao->operacao) {
            case OP_ADD:
            case OP_ADDS:
            case OP_SUB:
            case OP_SUBS:
            case OP_RSB:
            case OP_MUL:
            case OP_UDIV:
            case OP_AND:
            case OP_ORR:
            case OP_EOR:
                fprintf(arquivo, " %s, %s, %s\n",
                    instrucao->argumento1,
                    instrucao->argumento2,
                    instrucao->argumento3
                );
                continue;
            
            case OP_NOT:
            case OP_MOV:
                fprintf(arquivo, " %s, %s\n",
                    instrucao->argumento1,
                    instrucao->argumento3
                );
                continue;
            
            case OP_CMP:
                fprintf(arquivo, " %s, %s\n",
                    instrucao->argumento2,
                    instrucao->argumento3
                );
                continue;
            
            case OP_B:
            case OP_BL:
                fprintf(arquivo, " %s\n",
                    instrucao->argumento3
                );
                continue;

            case OP_BX:
                fprintf(arquivo, " %s\n",
                    instrucao->argumento2
                );
                continue;

            case OP_LDR:
            case OP_STR:
                if (instrucao->argumento1 == NULL) {
                    fprintf(arquivo, " %s, %s\n",
                        instrucao->argumento1,
                        instrucao->argumento3
                    );
                } else if (instrucao->argumento3 == NULL) {
                    fprintf(arquivo, " %s, %s\n",
                        instrucao->argumento1,
                        instrucao->argumento2
                    );
                } else {
                    fprintf(arquivo, " %s, [%s, %s]\n",
                        instrucao->argumento1,
                        instrucao->argumento2,
                        instrucao->argumento3
                    );
                }
                continue;

            case OP_IN:
                fprintf(arquivo, " %s\n",
                    instrucao->argumento1
                );
                continue;

            case OP_OUT:
                fprintf(arquivo, " %s\n",
                    instrucao->argumento2
                );
                continue;

            default:
                fprintf(arquivo, "\n");
                continue;

        }
    }
}

void liberarInstrucoes() {

    Instrucao* instrucao = lista_instrucoes;

    while (instrucao != NULL) {
        Instrucao* instrAux = instrucao;
        instrucao=instrucao->prox;

        if (instrAux->argumento1 != NULL) {
            free(instrAux->argumento1);
        }

        if (instrAux->argumento2 != NULL) {
            free(instrAux->argumento2);
        }

        if (instrAux->argumento3 != NULL) {
            free(instrAux->argumento3);
        }

        free(instrAux);
    }
}

void emitirInstrucao(OperacaoInstrucao op, SufixoCondicional suf, const char* l, const char *a1, const char *a2, const char *a3) {

    Instrucao* novaInstrucao = (Instrucao*) malloc(sizeof(Instrucao));

    novaInstrucao->operacao   = op;
    novaInstrucao->sufixo     = suf;
    novaInstrucao->label      = ((l != NULL) ? strdup(l) : NULL);
    novaInstrucao->argumento1 = ((a1 != NULL) ? strdup(a1) : NULL);
    novaInstrucao->argumento2 = ((a2 != NULL) ? strdup(a2) : NULL);
    novaInstrucao->argumento3 = ((a3 != NULL) ? strdup(a3) : NULL);
    novaInstrucao->prox = NULL;

    if (ultima_instrucao == NULL) {
        lista_instrucoes = novaInstrucao;
        ultima_instrucao = novaInstrucao;
    } else {
        ultima_instrucao->prox = novaInstrucao;
        ultima_instrucao = novaInstrucao;
    }
}

void quadruplasParaAssembly() {

    SufixoCondicional sufixoAtual = SUFIXO_AL;
    char* labelAtual = NULL;

    Quadrupla* quad=lista_quadruplas;

    while (quad != NULL) {
        switch (quad->operacao) {
            case GCI_ADD:
                emitirInstrucao(OP_ADD, sufixoAtual, labelAtual, quad->argumento3, quad->argumento1, quad->argumento2);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_SUB:
                emitirInstrucao(OP_SUB, sufixoAtual, labelAtual, quad->argumento3, quad->argumento1, quad->argumento2);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_MUL:
                emitirInstrucao(OP_MUL, sufixoAtual, labelAtual, quad->argumento3, quad->argumento1, quad->argumento2);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_DIV:
                emitirInstrucao(OP_UDIV, sufixoAtual, labelAtual, quad->argumento3, quad->argumento1, quad->argumento2);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_ASSIGN:
                emitirInstrucao(OP_MOV, sufixoAtual, labelAtual, quad->argumento3, NULL, quad->argumento1);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_LT:     
                emitirInstrucao(OP_CMP, sufixoAtual, labelAtual, NULL, quad->argumento1, quad->argumento2);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_GE;
                break;

            case GCI_LE:     
                emitirInstrucao(OP_CMP, sufixoAtual, labelAtual, NULL, quad->argumento1, quad->argumento2);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_GT;
                break;

            case GCI_GT:     
                emitirInstrucao(OP_CMP, sufixoAtual, labelAtual, NULL, quad->argumento1, quad->argumento2);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_LE;
                break;

            case GCI_GE:     
                emitirInstrucao(OP_CMP, sufixoAtual, labelAtual, NULL, quad->argumento1, quad->argumento2);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_LT;
                break;

            case GCI_EQ:     
                emitirInstrucao(OP_CMP, sufixoAtual, labelAtual, NULL, quad->argumento1, quad->argumento2);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_NE;
                break;

            case GCI_NEQ:
                emitirInstrucao(OP_CMP, sufixoAtual, labelAtual, NULL, quad->argumento1, quad->argumento2);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_EQ;
                break;

            case GCI_FUNCTION:
                // percorrer parametros e allocs
                labelAtual = quad->argumento1;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_CALL:
                if (strcmp(quad->argumento1, "input") == 0) {
                    emitirInstrucao(OP_IN, sufixoAtual, labelAtual, quad->argumento3, NULL, NULL);
                } else if (strcmp(quad->argumento1, "output") == 0) {
                    emitirInstrucao(OP_OUT, sufixoAtual, labelAtual, NULL, quad->argumento3, NULL);
                } else {
                    emitirInstrucao(OP_B, sufixoAtual, labelAtual, NULL, NULL, quad->argumento1);
                }
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_ALLOC:
                // reservar espaço na memória
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_PARAM:
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_ARG:
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_RETURN:
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_END:
                emitirInstrucao(OP_BX, sufixoAtual, labelAtual, NULL, "R31", NULL);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_IFF:
                emitirInstrucao(OP_B, sufixoAtual, labelAtual, NULL, NULL, quad->argumento3);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_GOTO:
                emitirInstrucao(OP_B, sufixoAtual, labelAtual, NULL, NULL, quad->argumento3);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_LABEL:
                labelAtual = quad->argumento3;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_LOAD:
                emitirInstrucao(OP_LDR, sufixoAtual, labelAtual, quad->argumento3, quad->argumento1, quad->argumento2);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_STORE:
                emitirInstrucao(OP_STR, sufixoAtual, labelAtual, quad->argumento3, quad->argumento1, quad->argumento2);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_HALT:
                emitirInstrucao(OP_HLT, sufixoAtual, labelAtual, NULL, NULL, NULL);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_PUSH:
                emitirInstrucao(OP_STR, sufixoAtual, labelAtual, quad->argumento3, "R30", NULL);
                emitirInstrucao(OP_ADD, sufixoAtual, labelAtual, "R30", "R30", "#1");
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_POP:
                emitirInstrucao(OP_SUB, sufixoAtual, labelAtual, "R30", "R30", "#1");
                emitirInstrucao(OP_LDR, sufixoAtual, labelAtual, "R30", quad->argumento3, NULL);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_MOVE:
                emitirInstrucao(OP_MOV, sufixoAtual, labelAtual, quad->argumento3, NULL, quad->argumento1);
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            default:
                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;
        }

        quad=quad->prox;
    }
}

void gerarAssembly() {

    emitirInstrucao(OP_NOP, SUFIXO_AL, NULL, NULL, NULL, NULL);
    emitirInstrucao(OP_B, SUFIXO_AL, NULL, NULL, NULL, "main");
    quadruplasParaAssembly();

    char caminhoSaida[260];
    montarCaminhoSaida(caminhoSaida, sizeof(caminhoSaida), "assembly.txt");

    FILE* saida = fopen(caminhoSaida, "w");
    imprimirInstrucoes(saida);
}