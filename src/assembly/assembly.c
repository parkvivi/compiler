#include "assembly.h"

Instrucao* lista_instrucoes = NULL;
Instrucao* ultima_instrucao = NULL;

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

    emitirInstrucao(OP_HLT, NULL, NULL, NULL);

    // imprimir diferentemente dependendo da instrucao
    // usar operacaoInstrucao e sufixoInstrucao
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

void emitirInstrucao(OperacaoInstrucao op, const char *a1, const char *a2, const char *a3) {

    Instrucao* novaInstrucao = (Instrucao*) malloc(sizeof(Instrucao));

    novaInstrucao->operacao   = op;
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

    for (Quadrupla* quad=lista_quadruplas; quad!=NULL; quad=quad->prox) {
        switch (quad->operacao) {
            case GCI_ADD:
                emitirInstrucao(OP_ADD, quad->argumento3, quad->argumento1, quad->argumento2);
                continue;

            case GCI_SUB:
                emitirInstrucao(OP_SUB, quad->argumento3, quad->argumento1, quad->argumento2);
                continue;

            case GCI_MUL:
                emitirInstrucao(OP_SUB, quad->argumento3, quad->argumento1, quad->argumento2);
                continue;

            case GCI_DIV:
                continue;

            case GCI_ASSIGN:
                continue;

            case GCI_LT:     
                continue;

            case GCI_LE:     
                continue;

            case GCI_GT:     
                continue;

            case GCI_GE:     
                continue;

            case GCI_EQ:     
                continue;

            case GCI_NEQ:
                continue;

            case GCI_FUNCTION:
                continue;

            case GCI_CALL:
                continue;

            case GCI_ALLOC:
                continue;

            case GCI_PARAM:
                continue;

            case GCI_ARG:
                continue;

            case GCI_RETURN:
                continue;

            case GCI_END:
                continue;

            case GCI_IFF:
                continue;

            case GCI_GOTO:
                continue;

            case GCI_LABEL:
                continue;

            case GCI_LOAD:
                continue;

            case GCI_STORE:
                continue;

            case GCI_HALT:
                continue;

            case GCI_PUSH:
                continue;

            case GCI_POP:
                continue;

            case GCI_MOVE:
                continue;

            default:
                continue;
        }
    }
}

void gerarAssembly() {
    quadruplasParaAssembly();
    // liberarInstrucoes();
}