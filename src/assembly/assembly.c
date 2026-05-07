#include "assembly.h"

void gerarAssembly() {
    for (Quadrupla* quad=lista_quadruplas; quad!=NULL; quad=quad->prox) {
        switch (quad->operacao) {
            case ADD:
                continue;

            case SUB:
                continue;

            case MUL:
                continue;

            case DIV:
                continue;

            case ASSIGN:
                continue;

            case LT:     
                continue;

            case LE:     
                continue;

            case GT:     
                continue;

            case GE:     
                continue;

            case EQ:     
                continue;

            case NEQ:
                continue;

            case FUNCTION:
                continue;

            case CALL:
                continue;

            case ALLOC:
                continue;

            case PARAM:
                continue;

            case ARG:
                continue;

            case RETURN:
                continue;

            case END:
                continue;

            case IFF:
                continue;

            case GOTO:
                continue;

            case LABEL:
                continue;

            case LOAD:
                continue;

            case STORE:
                continue;

            case HALT:
                continue;

            case PUSH:
                continue;

            case POP:
                continue;

            case MOVE:
                continue;

            default:
                continue;
        }
    }
}