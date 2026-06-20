#include "executavel.h"
#include "caminho_saida.h"

EnderecoLabel* lista_labels = NULL;
EnderecoLabel* ultimo_label = NULL;

void adicionarLabel(const char* label, int endereco) {
    
    if (!label) return;

    EnderecoLabel* novo = (EnderecoLabel*) malloc(sizeof(EnderecoLabel));
    novo->label = strdup(label);
    novo->endereco = endereco;
    novo->prox = NULL;

    if (lista_labels == NULL) {
        lista_labels = novo;
    } else {
        ultimo_label->prox = novo;
    }

    ultimo_label = novo;
}

void liberarLabels() {
    
    EnderecoLabel* l = lista_labels;

    while (l) {
        EnderecoLabel* lAux = l;
        l = l->prox;

        free(lAux->label);
        free(lAux);
    }
}

int enderecoLabel(const char* label) {

    EnderecoLabel* l;
    for (l=lista_labels; l!=NULL && strcmp(l->label, label)!=0; l=l->prox);

    if (!l) {
        fprintf(stderr, "EXECUTAVEL :: Label '%s' não endereçado", label);
    }

    return l->endereco;
}

char* geraCond(SufixoCondicional suf) {

    switch (suf) {
        case SUFIXO_EQ:  return "0000";
        case SUFIXO_NE:  return "0001";
        case SUFIXO_CS:  return "0010";
        case SUFIXO_CC:  return "0011";
        case SUFIXO_MI:  return "0100";
        case SUFIXO_PL:  return "0101";
        case SUFIXO_VS:  return "0110";
        case SUFIXO_VC:  return "0111";
        case SUFIXO_HI:  return "1000";
        case SUFIXO_LS:  return "1001";
        case SUFIXO_GE:  return "1010";
        case SUFIXO_LT:  return "1011";
        case SUFIXO_GT:  return "1100";
        case SUFIXO_LE:  return "1101";
        case SUFIXO_AL:  return "1110";
        default:    return NULL;
    }
}

char* registradorParaBinario(const char* registrador) {

    if (!registrador) return "00000";

    if (registrador[0] != 'R' || registrador[1] < '0' || registrador[1] > '9') return NULL;

    if (strcmp(registrador, "R0") == 0) {
        return "00000";
    } else if (strcmp(registrador, "R1") == 0) {
        return "00001";
    } else if (strcmp(registrador, "R2") == 0) {
        return "00010";
    } else if (strcmp(registrador, "R3") == 0) {
        return "00011";
    } else if (strcmp(registrador, "R4") == 0) {
        return "00100";
    } else if (strcmp(registrador, "R5") == 0) {
        return "00101";
    } else if (strcmp(registrador, "R6") == 0) {
        return "00110";
    } else if (strcmp(registrador, "R7") == 0) {
        return "00111";
    } else if (strcmp(registrador, "R8") == 0) {
        return "01000";
    } else if (strcmp(registrador, "R9") == 0) {
        return "01001";
    } else if (strcmp(registrador, "R10") == 0) {
        return "01010";
    } else if (strcmp(registrador, "R11") == 0) {
        return "01011";
    } else if (strcmp(registrador, "R12") == 0) {
        return "01100";
    } else if (strcmp(registrador, "R13") == 0) {
        return "01101"; 
    } else if (strcmp(registrador, "R14") == 0) {
        return "01110";
    } else if (strcmp(registrador, "R15") == 0) {
        return "01111";
    } else if (strcmp(registrador, "R16") == 0) {
        return "10000";
    } else if (strcmp(registrador, "R17") == 0) {
        return "10001";
    } else if (strcmp(registrador, "R18") == 0) {
        return "10010";
    } else if (strcmp(registrador, "R19") == 0) {
        return "10011";
    } else if (strcmp(registrador, "R20") == 0) {
        return "10100";
    } else if (strcmp(registrador, "R21") == 0) {
        return "10101";
    } else if (strcmp(registrador, "R22") == 0) {
        return "10110";
    } else if (strcmp(registrador, "R23") == 0) {
        return "10111";
    } else if (strcmp(registrador, "R24") == 0) {
        return "11000";
    } else if (strcmp(registrador, "R25") == 0) {
        return "11001";
    } else if (strcmp(registrador, "R26") == 0) {
        return "11010";
    } else if (strcmp(registrador, "R27") == 0) {
        return "11011";
    } else if (strcmp(registrador, "R28") == 0) {
        return "11100";
    } else if (strcmp(registrador, "R29") == 0) {
        return "11101";
    } else if (strcmp(registrador, "R30") == 0) {
        return "11110";
    } else if (strcmp(registrador, "R31") == 0) {
        return "11111";
    }
    
    return "00000";
}

char* geraOP0(OperacaoInstrucao operacao) {

    switch (operacao) {
        case OP_ADD:
        case OP_ADDS:
        case OP_SUB:
        case OP_SUBS:
        case OP_RSB:
        case OP_MUL:
        case OP_UDIV:
        case OP_NOT:
        case OP_AND:
        case OP_ORR:
        case OP_EOR:
        case OP_MOV:
        case OP_CMP:
            return "00";

        case OP_B:
        case OP_BX:
        case OP_BL:
            return "01";

        case OP_LDR:
        case OP_STR:
            return "10";

        case OP_NOP:
        case OP_HLT:
        case OP_IN:
        case OP_OUT:
            return "11";

        default:
            return NULL;
    }
}

char* geraOP1(OperacaoInstrucao operacao) {

    switch (operacao) {
        // instruções do tipo D
        case OP_ADD:    return "00000";
        case OP_ADDS:   return "00001";
        case OP_SUB:    return "00010";
        case OP_SUBS:   return "00011";
        case OP_RSB:    return "00100";
        case OP_MUL:    return "00101";
        case OP_UDIV:   return "00110";
        case OP_NOT:    return "00111";
        case OP_AND:    return "01000";
        case OP_ORR:    return "01001";
        case OP_EOR:    return "01010";
        case OP_MOV:    return "01011";
        case OP_CMP:    return "01100";

        // instruções do tipo B
        case OP_B:      return "00";
        case OP_BX:     return "01";
        case OP_BL:     return "10";

        // instruções do tipo A
        case OP_NOP:    return "0000";
        case OP_HLT:    return "0001";
        case OP_IN:     return "0010";
        case OP_OUT:    return "0011";

        // instruções do tipo M
        default:        return NULL;
    }
}

char* decimalParaBinario(char* decimalString, int quantidadeBits) {

    char* binario = (char*) malloc((quantidadeBits+1) * sizeof(char));
    unsigned int decimal;

    if (!decimalString) {
        decimal = 0;
    } else {
        decimal = (unsigned int) atoi(decimalString);
    }
    
    for (int i=quantidadeBits-1; i>=0; i--) {
        binario[quantidadeBits - 1 - i] = ((decimal >> i) & 1) + '0';
    }

    binario[quantidadeBits] = '\0';

    return binario;
}

void assemblyParaBinario(FILE* arquivo) {

    for (Instrucao* instrucao=lista_instrucoes; instrucao!=NULL; instrucao=instrucao->prox) {

        // cond
        char* cond = geraCond(instrucao->sufixo);
        fprintf(arquivo, "%s", cond);
        
        // opcode
        char* op0 = geraOP0(instrucao->operacao);
        char bitS = ((instrucao->operacao == OP_STR) ? '1' : '0');
        char bitI = ((instrucao->imediato != NULL) ? '1' : '0');
        char bitR = ((instrucao->rn != NULL) ? '1' : '0');
        char* op1 = geraOP1(instrucao->operacao);

        char* rd = registradorParaBinario(instrucao->rd);
        char* rn = registradorParaBinario(instrucao->rn);
        char* rm = registradorParaBinario(instrucao->rm);

        switch (instrucao->operacao) {
            case OP_ADD:
            case OP_ADDS:
            case OP_SUB:
            case OP_SUBS:
            case OP_RSB:
            case OP_MUL:
            case OP_UDIV:
            case OP_NOT:
            case OP_AND:
            case OP_ORR:
            case OP_EOR:
            case OP_MOV:
            case OP_CMP: 
            {
                fprintf(arquivo, "%s%c%s%s%s", op0, bitI, op1, rn, rd);
                
                if (instrucao->rm != NULL) {
                    fprintf(arquivo, "%s00000", rm);
                } else {
                    char* imediato = decimalParaBinario(instrucao->imediato, TAM_IMEDIATO_TIPO_D);
                    fprintf(arquivo, "%s", imediato);
                    free(imediato);
                }
                
                break;
            }

            case OP_B:
            case OP_BL:
            {   
                int enderecoDecimal = enderecoLabel(instrucao->endereco);
                char buffer[20];
                sprintf(buffer, "%d", enderecoDecimal);

                char* enderecoBinario = decimalParaBinario(buffer, TAM_IMEDIATO_TIPO_B);
                fprintf(arquivo, "%s%s%s", op0, op1, enderecoBinario);
                free(enderecoBinario);

                break;
            }

            case OP_BX:
            {
                fprintf(arquivo, "%s%s%s%s", op0, op1, rn, "0000000000000000000");
                break;
            }
                
            case OP_LDR:
            case OP_STR:
            {
                fprintf(arquivo, "%s%c%c%c%s", op0, bitS, bitI, bitR, rn);

                if (instrucao->operacao == OP_LDR) {
                    fprintf(arquivo, "%s", rd);
                } else {
                    fprintf(arquivo, "%s", rm);
                }

                char* imediato = decimalParaBinario(instrucao->imediato, TAM_IMEDIATO_TIPO_M);
                char* endereco = decimalParaBinario(instrucao->endereco, TAM_IMEDIATO_TIPO_M);

                if (instrucao->imediato == NULL) {
                    fprintf(arquivo, "%s", endereco);
                } else {
                    fprintf(arquivo, "%s", imediato);
                }

                free(imediato);
                free(endereco);

                break;
            }
                
            case OP_NOP:
            case OP_HLT:
            {
                fprintf(arquivo, "%s%s0000000000000000000000", op0, op1);

                break;
            }
                
            case OP_IN:
            case OP_OUT:
            {
                fprintf(arquivo, "%s%s", op0, op1);

                if (instrucao->operacao == OP_IN) {
                    fprintf(arquivo, "%s", rd);
                } else {
                    fprintf(arquivo, "%s", rn);
                }

                fprintf(arquivo, "00000000000000000");

                break;
            }
                
            default:
                break;
        }

        fprintf(arquivo, "\n");
    }
}

void gerarExecutavel() {

    // calcula endereços absolutos dos labels
    int endereco = 0;
    for (Instrucao* i=lista_instrucoes; i!=NULL; i=i->prox) {
        if (i->label != NULL) {
            adicionarLabel(i->label, endereco);
        }

        endereco++;
    }

    char caminhoSaida[260];
    montarCaminhoSaida(caminhoSaida, sizeof(caminhoSaida), "executavel.txt");

    FILE* saida = fopen(caminhoSaida, "w");

    // tradução instrução -> binário
    assemblyParaBinario(saida);
    liberarLabels();
}