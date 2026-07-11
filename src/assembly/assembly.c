#include "assembly.h"
#include "caminho_saida.h"

Instrucao* lista_instrucoes = NULL;
Instrucao* ultima_instrucao = NULL;
ListaSimbolos* lista_simbolos = NULL;

int mapeamentoTemporarios[MAX_TEMPORARIOS];     // mapeamento de t1 a t28
bool registradoresDisponiveis[MAX_TEMPORARIOS];  // disponibilidade de R1 a R28

void inserirListaSimbolos(Simbolo* simbolo) {

    ListaSimbolos *s, *sAnt = lista_simbolos;
    for (s=lista_simbolos; s!=NULL && s->simbolo!=simbolo; sAnt=s, s=s->prox);

    if (s == NULL) {
        ListaSimbolos* novo = (ListaSimbolos*) malloc(sizeof(ListaSimbolos));
        novo->simbolo = simbolo;
        novo->prox = NULL;

        if (sAnt == NULL) {
            lista_simbolos = novo;
        } else {
            sAnt->prox = novo;
        }
    }
}

void liberarListaSimbolos() {

    ListaSimbolos* l = lista_simbolos;

    while (l != NULL) {
        ListaSimbolos* lAux = l;
        l = l->prox;

        if (lAux->simbolo->ehVarGlobal == false) {
            lAux->simbolo->offset = 0;
            lAux->simbolo->offsetNegativo = false;
        }

        free(lAux);
    }

    lista_simbolos = NULL;
}

void inicializarMapeamento() {
    for (int i=0; i<MAX_TEMPORARIOS; i++) {
        mapeamentoTemporarios[i] = -1;
        registradoresDisponiveis[i] = true;
    }
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

char* temporariosParaRegistradores(const char* temporario) {

    if (!ehTemporario(temporario)) return NULL;

    if (strcmp(temporario, "t0") == 0) {
        return "R0";
    }

    int indice = temporarioParaNumero(temporario);
    if (mapeamentoTemporarios[indice] == -1) {
        int novoReg;
        for (novoReg=0; registradoresDisponiveis[novoReg]==false && novoReg<MAX_TEMPORARIOS; novoReg++);

        registradoresDisponiveis[novoReg] = false;
        mapeamentoTemporarios[indice] = novoReg;
    }
    
    char* buffer = malloc(sizeof(char)*20);
    sprintf(buffer, "R%d", mapeamentoTemporarios[indice] + 1);
    return buffer;
}

char* reservarRegistrador() {
    
    int novoReg;
    for (novoReg=0; registradoresDisponiveis[novoReg]==false && novoReg<MAX_TEMPORARIOS; novoReg++);

    registradoresDisponiveis[novoReg] = false;
    
    char* buffer = malloc(sizeof(char)*20);
    sprintf(buffer, "R%d", novoReg + 1);

    return buffer;
}

void liberarRegistrador(char* registrador) {

    int indice = temporarioParaNumero(registrador);
    registradoresDisponiveis[indice] = true;

    free(registrador);
}

void imprimirInstrucoes(FILE *arquivo) {

    for (Instrucao* instrucao=lista_instrucoes; instrucao!=NULL; instrucao=instrucao->prox) {

        if (instrucao->label != NULL) {
            fprintf(arquivo, "%s:\n", instrucao->label);
        }

        char* operacao = operacaoInstrucao(instrucao->operacao);
        char* sufixo = sufixoInstrucao(instrucao->sufixo);

        fprintf(arquivo, "\t%s", operacao);

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
                if (instrucao->rm != NULL) {
                    fprintf(arquivo, " %s, %s, %s\n",
                        instrucao->rd,
                        instrucao->rn,
                        instrucao->rm
                    );
                } else {
                    fprintf(arquivo, " %s, %s, #%s\n",
                        instrucao->rd,
                        instrucao->rn,
                        instrucao->imediato
                    );
                }
                continue;
            
            case OP_NOT:
            case OP_MOV:
                if (instrucao->rm != NULL) {
                    fprintf(arquivo, " %s, %s\n",
                        instrucao->rd,
                        instrucao->rm
                    );
                } else {
                    fprintf(arquivo, " %s, #%s\n",
                        instrucao->rd,
                        instrucao->imediato
                    );
                }
                continue;
            
            case OP_CMP:
                if (instrucao->rm != NULL) {
                    fprintf(arquivo, " %s, %s\n",
                        instrucao->rn,
                        instrucao->rm
                    );
                } else {
                    fprintf(arquivo, " %s, #%s\n",
                        instrucao->rn,
                        instrucao->imediato
                    );
                }
                continue;
            
            case OP_B:
            case OP_BL:
                fprintf(arquivo, " %s\n",
                    instrucao->endereco
                );
                continue;

            case OP_BX:
                fprintf(arquivo, " %s\n",
                    instrucao->rn
                );
                continue;

            case OP_LDR:
                if (instrucao->rn != NULL && instrucao->imediato != NULL) {
                    // LDR deslocamento
                    fprintf(arquivo, " %s, [%s, #%s]\n",
                        instrucao->rd,
                        instrucao->rn,
                        instrucao->imediato
                    );
                } else if (instrucao->rn != NULL) {
                    // LDR indireto
                    fprintf(arquivo, " %s, %s\n",
                        instrucao->rd,
                        instrucao->rn
                    );
                } else if (instrucao->imediato != NULL) {
                    // LDR imediato
                    fprintf(arquivo, " %s, #%s\n",
                        instrucao->rd,
                        instrucao->imediato
                    );
                } else {
                    // LDR direto
                    fprintf(arquivo, " %s, =%s\n",
                        instrucao->rd,
                        instrucao->endereco
                    );
                }
                continue;

            case OP_STR:
                if (instrucao->rn != NULL && instrucao->imediato != NULL) {
                    // STR deslocamento
                    fprintf(arquivo, " %s, [%s, #%s]\n",
                        instrucao->rm,
                        instrucao->rn,
                        instrucao->imediato
                    );
                } else if (instrucao->rn != NULL) {
                    // LDR indireto
                    fprintf(arquivo, " %s, %s\n",
                        instrucao->rm,
                        instrucao->rn
                    );
                } else if (instrucao->endereco != NULL) {
                    // STR direto
                    fprintf(arquivo, " %s, =%s\n",
                        instrucao->rm,
                        instrucao->endereco
                    );
                } 
                continue;

            case OP_IN:
                fprintf(arquivo, " %s\n",
                    instrucao->rd
                );
                continue;

            case OP_OUT:
                fprintf(arquivo, " %s\n",
                    instrucao->rn
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

        if (instrAux->label != NULL) {
            free(instrAux->label);
        }

        if (instrAux->rd != NULL) {
            free(instrAux->rd);
        }

        if (instrAux->rn != NULL) {
            free(instrAux->rn);
        }

        if (instrAux->rm != NULL) {
            free(instrAux->rm);
        }

        if (instrAux->imediato != NULL) {
            free(instrAux->imediato);
        }

        if (instrAux->endereco != NULL) {
            free(instrAux->endereco);
        }

        free(instrAux);
    }
}

void emitirInstrucao(OperacaoInstrucao op, SufixoCondicional suf, const char* l, const char *rd, const char *rn, const char *rm, const char *im, const char *end) {

    Instrucao* novaInstrucao = (Instrucao*) malloc(sizeof(Instrucao));

    novaInstrucao->operacao = op;
    novaInstrucao->sufixo   = suf;
    novaInstrucao->label    = ((l != NULL) ? strdup(l) : NULL);
    novaInstrucao->rd       = ((rd != NULL) ? strdup(rd) : NULL);
    novaInstrucao->rn       = ((rn != NULL) ? strdup(rn) : NULL);
    novaInstrucao->rm       = ((rm != NULL) ? strdup(rm) : NULL);
    novaInstrucao->imediato = ((im != NULL) ? strdup(im) : NULL);
    novaInstrucao->endereco = ((end != NULL) ? strdup(end) : NULL);
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
    char* funcaoAtual = NULL;

    int enderecoLocal = COMECO_VAR_LOCAIS;
    int quantidadeParametros = 0;

    Quadrupla* quad=lista_quadruplas;

    while (quad != NULL) {
        char *rd = NULL, *rn = NULL, *rm = NULL, *imediato = NULL, *endereco = NULL;

        switch (quad->operacao) {
            case GCI_ADD:
            case GCI_SUB:
            case GCI_MUL:
            case GCI_DIV: {
                OperacaoInstrucao operacao;

                if (quad->operacao == GCI_ADD) {
                    operacao = OP_ADD;
                } else if (quad->operacao == GCI_SUB) {
                    operacao = OP_SUB;
                } else if (quad->operacao == GCI_MUL) {
                    operacao = OP_MUL;
                } else {
                    operacao = OP_UDIV;
                }
                
                bool bitI = ehTemporario(quad->argumento2);

                rd = temporariosParaRegistradores(quad->argumento3);
                rn = temporariosParaRegistradores(quad->argumento1);
                rm = (bitI ? temporariosParaRegistradores(quad->argumento2) : NULL);
                imediato = (bitI ? NULL : quad->argumento2);

                emitirInstrucao(operacao, sufixoAtual, labelAtual, rd, rn, rm, imediato, endereco);

                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;
            }

            case GCI_ASSIGN: {
                bool bitI = ehTemporario(quad->argumento1);

                rd = temporariosParaRegistradores(quad->argumento3);
                rm = (bitI ? temporariosParaRegistradores(quad->argumento2) : NULL);
                imediato = (bitI ? NULL : quad->argumento2);

                emitirInstrucao(OP_MOV, sufixoAtual, labelAtual, rd, rn, rm, imediato, endereco);

                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;
            }

            case GCI_LT:
            case GCI_LE:
            case GCI_GT:
            case GCI_GE:
            case GCI_EQ:
            case GCI_NEQ: {
                bool bitI = ehTemporario(quad->argumento2);
                
                rn = temporariosParaRegistradores(quad->argumento1);
                rm = (bitI ? temporariosParaRegistradores(quad->argumento2) : NULL);
                imediato = (bitI ? NULL : quad->argumento2);

                emitirInstrucao(OP_CMP, sufixoAtual, labelAtual, rd, rn, rm, imediato, endereco);

                labelAtual = NULL;

                SufixoCondicional sufixo;
                
                if (quad->operacao == GCI_LT) {
                    sufixo = SUFIXO_GE;
                } else if (quad->operacao == GCI_LE) {
                    sufixo = SUFIXO_GT;
                } else if (quad->operacao == GCI_GT) {
                    sufixo = SUFIXO_LE;
                }  else if (quad->operacao == GCI_GE) {
                    sufixo = SUFIXO_LT;
                } else if (quad->operacao == GCI_EQ) {
                    sufixo = SUFIXO_NE;
                } else {
                    sufixo = SUFIXO_EQ;
                }

                sufixoAtual = sufixo;
                break;
            }

            case GCI_FUNCTION:
                labelAtual = quad->argumento1;
                sufixoAtual = SUFIXO_AL;
                funcaoAtual = quad->argumento1;

                if (strcmp(quad->argumento1, "main") == 0) {
                    emitirInstrucao(OP_MOV, sufixoAtual, labelAtual, FP, NULL, SP, NULL, NULL); // atualiza frame pointer da main
                    labelAtual = NULL;
                    enderecoLocal = 0;
                    break;
                }

                quantidadeParametros = quad->simbolo->quantidadeParametros;

                emitirInstrucao(OP_STR, sufixoAtual, labelAtual, NULL, SP, FP, NULL, NULL);  // STR FP, SP      -> guarda FP antigo

                labelAtual = NULL;

                emitirInstrucao(OP_MOV, sufixoAtual, labelAtual, FP, NULL, SP, NULL, NULL);  // MOV FP, SP      -> atualiza FP
                emitirInstrucao(OP_ADD, sufixoAtual, labelAtual, SP, SP, NULL, "1", NULL);   // ADD SP, SP, #1
                emitirInstrucao(OP_STR, sufixoAtual, labelAtual, NULL, SP, LR, NULL, NULL);  // STR LR, SP      -> guarda LR antigo
                emitirInstrucao(OP_ADD, sufixoAtual, labelAtual, SP, SP, NULL, "1", NULL);   // ADD SP, SP, #1

                break;

            case GCI_CALL:
                if (strcmp(quad->argumento1, "input") == 0) {
                    rd = temporariosParaRegistradores(quad->argumento3);
                    emitirInstrucao(OP_IN, sufixoAtual, labelAtual, rd, NULL, NULL, NULL, NULL);
                    emitirInstrucao(OP_NOP, SUFIXO_AL, NULL, NULL, NULL, NULL, NULL, NULL);
                } else if (strcmp(quad->argumento1, "output") == 0) {
                    emitirInstrucao(OP_OUT, sufixoAtual, labelAtual, NULL, RR, NULL, NULL, NULL);
                    emitirInstrucao(OP_NOP, SUFIXO_AL, NULL, NULL, NULL, NULL, NULL, NULL);
                } else {
                    char decremento[20];
                    sprintf(decremento, "%d", atoi(quad->argumento2));
                    emitirInstrucao(OP_BL, sufixoAtual, labelAtual, NULL, NULL, NULL, NULL, quad->argumento1);

                    labelAtual = NULL;
                    sufixoAtual = SUFIXO_AL;

                    emitirInstrucao(OP_SUB, sufixoAtual, labelAtual, SP, SP, NULL, decremento, NULL);
                }

                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_ALLOC:
                if (funcaoAtual == NULL) {
                    // ignora alocação de variáveis globais
                    labelAtual = NULL;
                    sufixoAtual = SUFIXO_AL;
                    break;
                }

                emitirInstrucao(OP_ADD, sufixoAtual, labelAtual, SP, SP, NULL, "1", NULL);

                inserirListaSimbolos(quad->simbolo);

                quad->simbolo->offset = enderecoLocal;
                enderecoLocal++;

                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_PARAM:
                inserirListaSimbolos(quad->simbolo);
                
                quad->simbolo->offset = quantidadeParametros;
                quad->simbolo->offsetNegativo = true;
                quantidadeParametros--;

                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_ARG:
                rn = SP;
                
                if (ehTemporario(quad->argumento1) == true) {
                    rm = temporariosParaRegistradores(quad->argumento1);

                    emitirInstrucao(OP_STR, sufixoAtual, labelAtual, NULL, rn, rm, NULL, NULL);

                    labelAtual = NULL;
                    sufixoAtual = SUFIXO_AL;
                } else {
                    char* regAux = reservarRegistrador();

                    emitirInstrucao(OP_LDR, sufixoAtual, labelAtual, regAux, NULL, NULL, quad->argumento1, NULL);

                    labelAtual = NULL;
                    sufixoAtual = SUFIXO_AL;

                    emitirInstrucao(OP_STR, sufixoAtual, labelAtual, NULL, rn, regAux, imediato, NULL);
                    
                    liberarRegistrador(regAux);
                }

                emitirInstrucao(OP_ADD, sufixoAtual, labelAtual, SP, SP, NULL, "1", NULL);

                break;

            case GCI_RETURN:
            case GCI_END:
                if (quad->operacao == GCI_RETURN && quad->argumento1 != NULL) {
                    if (ehTemporario(quad->argumento1) == true) {
                        rm = temporariosParaRegistradores(quad->argumento1);
                    } else {
                        imediato = quad->argumento1;
                    }

                    emitirInstrucao(OP_MOV, sufixoAtual, labelAtual, RR, NULL, rm, imediato, NULL);
                    
                    labelAtual = NULL;
                    sufixoAtual = SUFIXO_AL;
                } else if (quad->operacao == GCI_END && strcmp(quad->argumento1, "main") == 0) {
                    enderecoLocal = COMECO_VAR_LOCAIS;
                    funcaoAtual = NULL;
                    liberarListaSimbolos();
                    break;
                } else if (quad->operacao == GCI_END) {
                    enderecoLocal = COMECO_VAR_LOCAIS;
                    quantidadeParametros = 0;
                    funcaoAtual = NULL;
                    liberarListaSimbolos();
                }

                emitirInstrucao(OP_LDR, sufixoAtual, labelAtual, LR, FP, NULL, "1", NULL);      // LDR LR, [FP, #1] -> atualiza endereço de retorno correto

                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;

                emitirInstrucao(OP_MOV, sufixoAtual, labelAtual, SP, NULL, FP, NULL, NULL);     // MOV SP, FP       -> torna topo da pilha = endereço do fp atual (que guarda endereço do fp antigo)
                emitirInstrucao(OP_LDR, sufixoAtual, labelAtual, FP, SP, NULL, NULL, NULL);     // LDR FP, SP       -> atualiza fp para fp antigo
                emitirInstrucao(OP_BX, sufixoAtual, labelAtual, NULL, LR, NULL, NULL, NULL);    // BX LR            -> retorna da função
                break;

            case GCI_IFF:
                emitirInstrucao(OP_B, sufixoAtual, labelAtual, NULL, NULL, NULL, NULL, quad->argumento3);

                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_GOTO:
                emitirInstrucao(OP_B, sufixoAtual, labelAtual, NULL, NULL, NULL, NULL, quad->argumento3);

                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_LABEL:
                labelAtual = quad->argumento3;
                emitirInstrucao(OP_NOP, SUFIXO_AL, labelAtual, NULL, NULL, NULL, NULL, NULL);    // NOP

                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;
            
            case GCI_STORE: // arg3 -> rm ; arg1 -> rn ; arg2 -> endereço/imediato
            case GCI_LOAD:  // arg3 -> rd ; arg1 -> rn ; arg2 -> endereço/imediato
                rd = ((quad->operacao == GCI_LOAD) ? temporariosParaRegistradores(quad->argumento3) : NULL);
                rm = ((quad->operacao == GCI_STORE) ? temporariosParaRegistradores(quad->argumento3) : NULL);
                OperacaoInstrucao operacao = ((quad->operacao == GCI_LOAD) ? OP_LDR : OP_STR);
                
                if (quad->simbolo != NULL && ehTemporario(quad->argumento1) == false) {
                    if (quad->simbolo->ehVarGlobal == true) {
                        // variáveis globais são endereçadas diretamente
                        char* buffer = malloc(sizeof(char)*20);
                        sprintf(buffer, "%d", quad->simbolo->offset);
                        if (quad->simbolo->ehVetor == true) {
                            // endereço da base do vetor é passada como imediato para cálculo da posição no vetor
                            imediato = buffer;
                        } else {
                            // endereço da variável é passada como endereço para acesso direto
                            endereco = buffer;
                        }
                    } else if (quad->simbolo->offsetNegativo == true) {
                        // parâmetro é endereçado indiretamente
                        char buffer[20];
                        sprintf(buffer, "%d", quad->simbolo->offset);

                        char* regAux = reservarRegistrador();
                        emitirInstrucao(OP_SUB, sufixoAtual, labelAtual, regAux, FP, NULL, buffer, NULL);
                        
                        labelAtual = NULL;
                        sufixoAtual = SUFIXO_AL;

                        emitirInstrucao(operacao, sufixoAtual, labelAtual, rd, regAux, rm, imediato, endereco);

                        liberarRegistrador(regAux);
                        break;
                    } else {
                        // variável local é endereçada por deslocamento
                        rn = FP;
                        
                        char buffer[20];
                        sprintf(buffer, "%d", quad->simbolo->offset);

                        emitirInstrucao(operacao, sufixoAtual, labelAtual, rd, rn, rm, buffer, endereco);

                        labelAtual = NULL;
                        sufixoAtual = SUFIXO_AL;
                        break;
                    }
                } else {
                    if (ehTemporario(quad->argumento1) == true) {
                        rn = temporariosParaRegistradores(quad->argumento1);
                    } else {
                        imediato = quad->argumento1;
                    }
                }

                emitirInstrucao(operacao, sufixoAtual, labelAtual, rd, rn, rm, imediato, endereco);

                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_HALT:
                emitirInstrucao(OP_HLT, sufixoAtual, labelAtual, NULL, NULL, NULL, NULL, NULL);

                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;
                break;

            case GCI_PUSH:
                rn = SP;
                rm = temporariosParaRegistradores(quad->argumento3);

                emitirInstrucao(OP_STR, sufixoAtual, labelAtual, NULL, rn, rm, NULL, NULL);

                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;

                emitirInstrucao(OP_ADD, sufixoAtual, labelAtual, SP, SP, NULL, "1", NULL);
                break;

            case GCI_POP:
                emitirInstrucao(OP_SUB, sufixoAtual, labelAtual, SP, SP, NULL, "1", NULL);

                labelAtual = NULL;
                sufixoAtual = SUFIXO_AL;

                rd = temporariosParaRegistradores(quad->argumento3);

                emitirInstrucao(OP_LDR, sufixoAtual, labelAtual, rd, SP, NULL, NULL, NULL);
                break;

            case GCI_MOVE:
                rd = temporariosParaRegistradores(quad->argumento3);

                if (ehTemporario(quad->argumento1) == true) {
                    rm = temporariosParaRegistradores(quad->argumento1);
                } else {
                    imediato = quad->argumento1;
                }

                emitirInstrucao(OP_MOV, sufixoAtual, labelAtual, rd, rn, rm, imediato, endereco);

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

    inicializarMapeamento();

    emitirInstrucao(OP_NOP, SUFIXO_AL, NULL, NULL, NULL, NULL, NULL, NULL);    // NOP

    emitirInstrucao(OP_MOV, SUFIXO_AL, NULL, RR, NULL, NULL, "0", NULL);       // MOV R0, #0
    emitirInstrucao(OP_MOV, SUFIXO_AL, NULL, FP, NULL, NULL, "0", NULL);       // MOV R29, #0
    emitirInstrucao(OP_MOV, SUFIXO_AL, NULL, SP, NULL, NULL, "0", NULL);       // MOV R30, #0
    emitirInstrucao(OP_MOV, SUFIXO_AL, NULL, LR, NULL, NULL, "0", NULL);       // MOV R31, #0

    bool ehGlobal = true;
    int enderecoGlobal = 0;
    for (Quadrupla* q=lista_quadruplas; q!=NULL; q=q->prox) {
        if (q->operacao == GCI_FUNCTION) {
            ehGlobal = false;
        } else if (q->operacao == GCI_END) {
            ehGlobal = true;
        } else if (ehGlobal == true && q->operacao == GCI_ALLOC) {
            char* tamanhoVetor = ((q->simbolo->ehVetor == true) ? q->argumento3 : "1");
            int tamanhoAlocado = tamanhoAlocado = atoi(tamanhoVetor);

            emitirInstrucao(OP_ADD, SUFIXO_AL, NULL, SP, SP, NULL, tamanhoVetor, NULL);

            q->simbolo->ehVarGlobal = true;
            q->simbolo->offset = enderecoGlobal;
            enderecoGlobal += tamanhoAlocado;
        }
    }

    emitirInstrucao(OP_B, SUFIXO_AL, NULL, NULL, NULL, NULL, NULL, "main");    // B main

    quadruplasParaAssembly();

    char caminhoSaida[260];
    montarCaminhoSaida(caminhoSaida, sizeof(caminhoSaida), "assembly.txt");

    FILE* saida = fopen(caminhoSaida, "w");
    imprimirInstrucoes(saida);
}