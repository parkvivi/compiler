#include "gci.h"
#include <stdlib.h>
#include <string.h>

#define MAX_TEMPORARIOS 28

static void montarCaminhoSaida(char* destino, size_t tamanho, const char* nomeArquivo) {

    const char* diretorioSaida = getenv("COMPILER_OUTPUT_DIR");
    if (diretorioSaida == NULL || diretorioSaida[0] == '\0') {
        diretorioSaida = "build";
    }

    snprintf(destino, tamanho, "%s/%s", diretorioSaida, nomeArquivo);
}

Quadrupla* lista_quadruplas = NULL;
Quadrupla* ultima_quadrupla = NULL;
Temporario temporarios[MAX_TEMPORARIOS];

int label_cont = 0;

bool ehTemporario(const char* temporario) {

    if (temporario != NULL && temporario[0] == 't' && (temporario[1] >= '0' && temporario[1] <= '9')) {
        // temporários são da forma t[0-9]...
        return true;
    }

    return false;
}

void inicializaTemporarios() {

    for (int i=0; i<MAX_TEMPORARIOS; i++) {
        temporarios[i].estado = Livre;
        temporarios[i].nivelEscopo = -1;
        temporarios[i].variavel = NULL;
        temporarios[i].preservado = false;
    }
}

char* novoTemporario(Simbolo* variavel, int nivelEscopo) {

    for (int i=0; i<MAX_TEMPORARIOS; i++) {
        if (temporarios[i].estado == Livre && temporarios[i].preservado == false) {
            char* novoTemporario = malloc(sizeof(char)*20);
            sprintf(novoTemporario, "t%d", i + 1);

            temporarios[i].nivelEscopo = nivelEscopo;
            temporarios[i].variavel = variavel;

            if (variavel != NULL) {
                temporarios[i].estado = AssociadoVariavel;
                variavel->temporario = strdup(novoTemporario);
            } else {
                temporarios[i].estado = ValorTemporario;
            }

            return novoTemporario;
        }
    }

    fprintf(stderr, "GCI :: Erro ao criar novos temporarios\n");
    return NULL;
}

void preservarTemporarios() {

    for (int i=0; i<MAX_TEMPORARIOS; i++) {
        if (temporarios[i].estado != Livre) {
            char buffer[20];
            sprintf(buffer, "t%d", i + 1);

            temporarios[i].preservado = true;
            emitirQuadrupla(GCI_PUSH, NULL, NULL, buffer, NULL);
        }
    }
}

void restaurarTemporarios() {

    for (int i=MAX_TEMPORARIOS-1; i>=0; i--) {
        if (temporarios[i].preservado == true) {
            char buffer[20];
            sprintf(buffer, "t%d", i + 1);

            emitirQuadrupla(GCI_POP, NULL, NULL, buffer, NULL);
            temporarios[i].preservado = false;
        }
    }
}

void reassociarTemporario(const char* temporario, Simbolo* variavel, int nivelEscopo) {

    if (temporario == NULL || variavel == NULL) return;

    if (ehTemporario(temporario) == false || strcmp(temporario, "t0") == 0) return;

    int indiceTemporario = temporarioParaNumero(temporario);
    if (indiceTemporario < 0 || indiceTemporario >= MAX_TEMPORARIOS) {
        fprintf(stderr, "GCI :: Erro ao reassociar temporarios\n");
        return;
    }

    for (int i=0; i<MAX_TEMPORARIOS; i++) {
        if (temporarios[i].variavel == variavel) {
            temporarios[i].estado = Livre;
            temporarios[i].nivelEscopo = -1;
            temporarios[i].variavel = NULL;
        }
    }

    if (variavel->temporario != NULL) {
        free(variavel->temporario);
    }

    variavel->temporario = strdup(temporario);
    temporarios[indiceTemporario].estado = AssociadoVariavel;
    temporarios[indiceTemporario].nivelEscopo = nivelEscopo;
    temporarios[indiceTemporario].variavel = variavel;
}

int liberarTemporario(const char* temporario, bool liberarTemporario) {

    if (temporario == NULL) return 0;

    if (ehTemporario(temporario) == false || strcmp(temporario, "t0") == 0) return 0;

    int indiceTemporario = temporarioParaNumero(temporario);
    if (indiceTemporario < 0 || indiceTemporario >= MAX_TEMPORARIOS) {
        return 0;
    }

    EstadoTemporario estado = temporarios[indiceTemporario].estado;

    // verificacao de casos de liberacao de temporarios para tipos aritmeticos e relacionais
    if (estado == AssociadoVariavel && liberarTemporario == false) {
        return 1;
    }

    if (estado == ValorTemporario && liberarTemporario == false) {
        return 2;
    }

    if (temporarios[indiceTemporario].preservado == true) {
        return 0;
    }

    temporarios[indiceTemporario].estado = Livre;
    temporarios[indiceTemporario].nivelEscopo = -1;

    Simbolo* s = temporarios[indiceTemporario].variavel;
    if (s != NULL && s->temporario != NULL) {
        free(s->temporario);
        s->temporario = NULL;
        temporarios[indiceTemporario].variavel = NULL;
    }

    return 0;
}

void liberarTemporariosEscopoAtual(int nivelEscopo) {

    for (int i=0; i<MAX_TEMPORARIOS; i++) {
        if (temporarios[i].nivelEscopo == nivelEscopo) {
            if (temporarios[i].preservado == true) {
                continue;
            }

            temporarios[i].estado = Livre;
            temporarios[i].nivelEscopo = -1;

            Simbolo* s = temporarios[i].variavel;
            if (s != NULL && s->temporario != NULL) {
                free(s->temporario);
                s->temporario = NULL;
                temporarios[i].variavel = NULL;
            }
        }
    }
}

int temporarioParaNumero(const char* temporario) {

    int tamanhoTemporario = strlen(temporario);
    int dezena = 0, unidade = 0;
    int numero = 0;

    if (tamanhoTemporario == 2) {
        unidade = temporario[1] - 48;
    } else if (tamanhoTemporario == 3) {
        unidade = temporario[2] - 48;
        dezena = 10 * (temporario[1] - 48);
    } else {
        return -1;
    }

    numero = dezena + unidade;
    if (numero < 1 || numero > MAX_TEMPORARIOS) {
        return -1;
    }

    return numero - 1;
}

char* novoLabel() {

    char* novoLabel = malloc(sizeof(char)*20);
    sprintf(novoLabel, "L%d", label_cont++);

    return novoLabel;
}

OperacaoQuadrupla verificarOperacao(TipoNo tipo) {

    switch (tipo) {
        case TipoMenorIgual:    return GCI_LE;
        case TipoMenor:         return GCI_LT;
        case TipoMaiorIgual:    return GCI_GE;
        case TipoMaior:         return GCI_GT;
        case TipoIgual:         return GCI_EQ;
        case TipoDiferente:     return GCI_NEQ;
        case TipoMultiplicacao: return GCI_MUL;
        case TipoDivisao:       return GCI_DIV;
        case TipoSoma:          return GCI_ADD;
        case TipoSubtracao:     return GCI_SUB;
        default:                return GCI_NONE;
    }
}

char* operacaoQuadrupla(OperacaoQuadrupla tipo) {
    
    switch (tipo) {
        case GCI_ADD:       return  "ADD";
        case GCI_SUB:       return  "SUB";
        case GCI_MUL:       return  "MUL";
        case GCI_DIV:       return  "DIV";
        case GCI_ASSIGN:    return  "ASSIGN";
        case GCI_LT:        return  "LT";
        case GCI_LE:        return  "LE";
        case GCI_GT:        return  "GT";
        case GCI_GE:        return  "GE";
        case GCI_EQ:        return  "EQ";
        case GCI_NEQ:       return  "NEQ";
        case GCI_FUNCTION:  return  "FUNCTION";
        case GCI_CALL:      return  "CALL";
        case GCI_ALLOC:     return  "ALLOC";
        case GCI_PARAM:     return  "PARAM";
        case GCI_ARG:       return  "ARG";
        case GCI_RETURN:    return  "RETURN";
        case GCI_END:       return  "END";
        case GCI_IFF:       return  "IFF";
        case GCI_GOTO:      return  "GOTO";
        case GCI_LABEL:     return  "LABEL";
        case GCI_LOAD:      return  "LOAD";
        case GCI_STORE:     return  "STORE";
        case GCI_HALT:      return  "HALT";
        case GCI_PUSH:      return  "PUSH";
        case GCI_POP:       return  "POP";
        case GCI_MOVE:      return  "MOVE";
        default:            return  NULL;
    }
}

void emitirQuadrupla(OperacaoQuadrupla op, const char *a1, const char *a2, const char *a3, Simbolo* simbolo) {

    Quadrupla* novaQuadrupla = (Quadrupla*) malloc(sizeof(Quadrupla));

    novaQuadrupla->operacao   = op;
    novaQuadrupla->argumento1 = ((a1 != NULL) ? strdup(a1) : NULL);
    novaQuadrupla->argumento2 = ((a2 != NULL) ? strdup(a2) : NULL);
    novaQuadrupla->argumento3 = ((a3 != NULL) ? strdup(a3) : NULL);
    novaQuadrupla->simbolo = simbolo;
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

    emitirQuadrupla(GCI_HALT, NULL, NULL, NULL, NULL);

    Quadrupla* quad=lista_quadruplas;

    while (quad != NULL) {
        fprintf(arquivo, "(%s, %s, %s, %s)\n",
                ((quad->operacao   != GCI_NONE) ? operacaoQuadrupla(quad->operacao)   : "-"),
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

char* gerarQuadruplas(AST* no, char* nomeFuncaoAtual, bool percorrerIrmaos, int nivelEscopo, bool reutilizaAtribuicao) {

    if (!no) return NULL;

    char* arg = NULL;

    switch (no->tipo) {
        case TipoDeclaracaoFuncao:
            char* nomeFuncaoDeclarada = no->dado.nome;
            char* tipoFuncaoDeclarada = ((no->simbolo->tipo == TipoInt) ? "int" : "void");

            emitirQuadrupla(GCI_FUNCTION, nomeFuncaoDeclarada, tipoFuncaoDeclarada, NULL, no->simbolo);

            char* argParametrosFuncao = gerarQuadruplas(no->filhos[1], nomeFuncaoDeclarada, true, nivelEscopo, false);
            liberarTemporario(argParametrosFuncao, true);

            char* argCorpoFuncao = gerarQuadruplas(no->filhos[2], nomeFuncaoDeclarada, true, nivelEscopo + 1, false);
            liberarTemporario(argCorpoFuncao, true);

            emitirQuadrupla(GCI_END, nomeFuncaoDeclarada, NULL, NULL, no->simbolo);
            
            liberarTemporariosEscopoAtual(nivelEscopo + 1);
            break;

        case TipoDeclaracaoVariavel:
            char* nomeVariavel = no->dado.nome;
            char* tamanhoVetor = NULL;

            if (no->filhos[1] != NULL) {
                tamanhoVetor = malloc(20*sizeof(char));
                sprintf(tamanhoVetor, "%d", no->filhos[1]->dado.valor);
            }

            emitirQuadrupla(GCI_ALLOC, nomeFuncaoAtual, nomeVariavel, tamanhoVetor, no->simbolo);

            if (tamanhoVetor != NULL) {
                free(tamanhoVetor);
            }

            break;

        case TipoDeclaracaoIteracao:
            char* labelComecoIteracao = novoLabel();
            char* labelFinalIteracao = novoLabel();
            emitirQuadrupla(GCI_LABEL, NULL, NULL, labelComecoIteracao, NULL);

            char* condicaoIteracao = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true, nivelEscopo, true);
            emitirQuadrupla(GCI_IFF, condicaoIteracao, NULL, labelFinalIteracao, NULL);
            liberarTemporario(condicaoIteracao, true);

            char* argEscopoIteracao = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true, nivelEscopo + 1, false);
            liberarTemporario(argEscopoIteracao, true);

            emitirQuadrupla(GCI_GOTO, NULL, NULL, labelComecoIteracao, NULL);
            emitirQuadrupla(GCI_LABEL, NULL, NULL, labelFinalIteracao, NULL);

            liberarTemporariosEscopoAtual(nivelEscopo + 1);

            free(labelComecoIteracao);
            free(labelFinalIteracao);

            break;
        
        case TipoDeclaracaoSelecao:
            char* condicaoSelecao = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true, nivelEscopo, true);
            char* labelElse = ((no->filhos[2] == NULL) ? NULL : novoLabel());
            char* labelFinalSelecao = novoLabel();

            emitirQuadrupla(GCI_IFF, condicaoSelecao, NULL, ((labelElse == NULL) ? labelFinalSelecao : labelElse), NULL);
            liberarTemporario(condicaoSelecao, true);

            char* argEscopoSelecao = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true, nivelEscopo + 1, false);
            liberarTemporario(argEscopoSelecao, true);
            
            if (labelElse != NULL) {
                emitirQuadrupla(GCI_GOTO, NULL, NULL, labelFinalSelecao, NULL);
            }

            emitirQuadrupla(GCI_LABEL, NULL, NULL, ((labelElse == NULL) ? labelFinalSelecao : labelElse), NULL);

            liberarTemporariosEscopoAtual(nivelEscopo + 1);

            if (labelElse != NULL) {
                char* argElse = gerarQuadruplas(no->filhos[2], nomeFuncaoAtual, true, nivelEscopo + 1, false);
                liberarTemporario(argElse, true);
                
                emitirQuadrupla(GCI_LABEL, NULL, NULL, labelFinalSelecao, NULL);

                liberarTemporariosEscopoAtual(nivelEscopo + 1);

                free(labelElse);
            }

            free(labelFinalSelecao);

            break;
        
        case TipoDeclaracaoRetorno:
            char* operandoRetornado = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true, nivelEscopo, true);

            emitirQuadrupla(GCI_RETURN, operandoRetornado, NULL, NULL, NULL);
            liberarTemporario(operandoRetornado, true);

            break;

        case TipoVar:
            if (no->filhos[0] == NULL) {
                if (no->simbolo->temporario != NULL) {
                    arg = no->simbolo->temporario;
                    break;
                }

                char* nomeVariavel = no->dado.nome;
                char* destinoConteudoVariavel = novoTemporario(no->simbolo, nivelEscopo);

                emitirQuadrupla(GCI_LOAD, nomeVariavel, NULL, destinoConteudoVariavel, no->simbolo);

                arg = destinoConteudoVariavel;
            } else {
                char* nomeVetor = no->simbolo->temporario;

                if (nomeVetor == NULL) {
                    nomeVetor = novoTemporario(no->simbolo, nivelEscopo);
                    emitirQuadrupla(GCI_LOAD, no->dado.nome, NULL, nomeVetor, no->simbolo);
                }
                
                char* indiceAcessado = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true, nivelEscopo, true);
                
                // significa que indiceAcessado eh um temporario
                if (no->filhos[0]->tipo != TipoNum) {
                    char* novo = ((liberarTemporario(indiceAcessado, false) == 2) ? indiceAcessado : novoTemporario(NULL, nivelEscopo));

                    emitirQuadrupla(GCI_ADD, nomeVetor, indiceAcessado, novo, NULL);

                    if (liberarTemporario(indiceAcessado, false) == 2) {
                        liberarTemporario(indiceAcessado, true);
                    }

                    nomeVetor = novo;
                    indiceAcessado = NULL;
                }
                
                char* destinoConteudoAcessado = novoTemporario(NULL, nivelEscopo);

                emitirQuadrupla(GCI_LOAD, nomeVetor, indiceAcessado, destinoConteudoAcessado, no->simbolo);

                if (liberarTemporario(nomeVetor, false) == 2) {
                    liberarTemporario(nomeVetor, true);
                }

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
            char* destinoIntermediario = ((no->simbolo->tipo == TipoInt) ? "t0" : NULL);

            bool ehOutput = (strcmp(nomeFuncaoChamada, "output") == 0);
            bool mudarContexto = (strcmp(nomeFuncaoChamada, "input") != 0 && ehOutput == false);

            if (mudarContexto == true) {
                preservarTemporarios();
            }

            if (ehOutput == true) {
                char* origemArgumento = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, false, nivelEscopo, true);
                emitirQuadrupla(GCI_MOVE, origemArgumento, NULL, "t0", NULL);

                liberarTemporario(origemArgumento, true);
            } else {
                for (AST* argumento=no->filhos[0]; argumento!=NULL; argumento=argumento->irmao) {
                    char* origemArgumento = gerarQuadruplas(argumento, nomeFuncaoAtual, false, nivelEscopo + 1, true);
                    emitirQuadrupla(GCI_ARG, origemArgumento, NULL, NULL, NULL);

                    if (liberarTemporario(origemArgumento, false) == 2) {
                        liberarTemporario(origemArgumento, true);
                    }
                }

                liberarTemporariosEscopoAtual(nivelEscopo + 1);
            }

            char quantidadeArgumentos[20];
            sprintf(quantidadeArgumentos, "%d", no->simbolo->quantidadeParametros);

            emitirQuadrupla(GCI_CALL, nomeFuncaoChamada, quantidadeArgumentos, destinoIntermediario, no->simbolo);

            if (mudarContexto == true) {
                restaurarTemporarios();
            }

            char* destinoValorRetornado = ((no->simbolo->tipo == TipoInt) ? novoTemporario(NULL, nivelEscopo) : NULL);

            if (no->simbolo->tipo == TipoInt) {
                emitirQuadrupla(GCI_MOVE, "t0", NULL, destinoValorRetornado, NULL);
            }

            arg = destinoValorRetornado;
            break;
        
        case TipoAtribuicao:
            char* operando = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true, nivelEscopo, true);

            AST* filhoEsquerdo = no->filhos[0];
            if (filhoEsquerdo->tipo == TipoVar) {
                char* nomeVariavel = filhoEsquerdo->dado.nome;

                if (filhoEsquerdo->filhos[0] != NULL) {
                    if (filhoEsquerdo->simbolo->temporario == NULL) {
                        char* novo = novoTemporario(filhoEsquerdo->simbolo, nivelEscopo);
                        emitirQuadrupla(GCI_LOAD, nomeVariavel, NULL, novo, filhoEsquerdo->simbolo);
                        nomeVariavel = novo;
                    } else {
                        nomeVariavel = filhoEsquerdo->simbolo->temporario;
                    }
                }

                if (no->filhos[1]->tipo == TipoNum) {
                    char* novo = novoTemporario(NULL, nivelEscopo);
                    emitirQuadrupla(GCI_LOAD, operando, NULL, novo, NULL);
                    operando = novo;
                }

                char* indiceAcessado = gerarQuadruplas(filhoEsquerdo->filhos[0], nomeFuncaoAtual, true, nivelEscopo, true);

                // significa que indiceAcessado eh um temporario
                if (filhoEsquerdo->filhos[0] != NULL && filhoEsquerdo->filhos[0]->tipo != TipoNum) {
                    char* novo = ((liberarTemporario(indiceAcessado, false) == 2) ? indiceAcessado : novoTemporario(NULL, nivelEscopo));

                    emitirQuadrupla(GCI_ADD, nomeVariavel, indiceAcessado, novo, NULL);

                    if (liberarTemporario(indiceAcessado, false) == 2) {
                        liberarTemporario(indiceAcessado, true);
                    }

                    nomeVariavel = novo;
                    indiceAcessado = NULL;
                }

                emitirQuadrupla(GCI_STORE, nomeVariavel, indiceAcessado, operando, filhoEsquerdo->simbolo);

                if (filhoEsquerdo->filhos[0] == NULL && liberarTemporario(operando, false) != 1) {
                    reassociarTemporario(operando, filhoEsquerdo->simbolo, nivelEscopo);
                }
                
                if (liberarTemporario(nomeVariavel, false) == 2) {
                    liberarTemporario(nomeVariavel, true);
                }

                arg = operando;

                if (liberarTemporario(operando, false) == 2 && reutilizaAtribuicao == false) {
                    liberarTemporario(operando, true);
                    arg = NULL;
                }
            } else {
                char* destinoAtribuicao = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true, nivelEscopo, false);
                
                emitirQuadrupla(GCI_ASSIGN, operando, NULL, destinoAtribuicao, NULL);
                liberarTemporario(operando, true);

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
            OperacaoQuadrupla operacao = verificarOperacao(no->tipo);
            char* operandoEsquerdo = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true, nivelEscopo, true);
            char* operandoDireito = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true, nivelEscopo, true);
            char* destinoOperacao = NULL;
            bool liberarDireito = false;

            if (no->filhos[0]->tipo == TipoNum) {
                char* novo = novoTemporario(NULL, nivelEscopo);
                emitirQuadrupla(GCI_LOAD, operandoEsquerdo, NULL, novo, NULL);
                operandoEsquerdo = novo;
            }

            // aqui, liberarTemporario() inicialmente so verifica se argumento eh temporario
            if (liberarTemporario(operandoEsquerdo, false) == 2) {
                destinoOperacao = operandoEsquerdo;
                liberarDireito = true;
            } else if (liberarTemporario(operandoDireito, false) == 2) {
                destinoOperacao = operandoDireito;
            } else {
                destinoOperacao = novoTemporario(NULL, nivelEscopo);
            }

            emitirQuadrupla(operacao, operandoEsquerdo, operandoDireito, destinoOperacao, NULL);
            
            if (liberarDireito == true) {
                liberarTemporario(operandoDireito, true);
            }

            arg = destinoOperacao;  
            break;
        
        default:
            for (int i=0; i<MAXFILHOS; i++) {
                AST* filho = no->filhos[i];
                if (filho == NULL) {
                    continue;
                }

                char* argFilhos = gerarQuadruplas(filho, nomeFuncaoAtual, true, nivelEscopo, false);
                liberarTemporario(argFilhos, true);
            }

            break;
    }

    if (percorrerIrmaos == true) {
        char* argIrmao = gerarQuadruplas(no->irmao, nomeFuncaoAtual, true, nivelEscopo, false);
        liberarTemporario(argIrmao, true);
    }

    return arg;
}

void gerarCodigoIntermediario() {

    char caminhoSaida[260];
    montarCaminhoSaida(caminhoSaida, sizeof(caminhoSaida), "codigointermediario.txt");

    FILE* saida = fopen(caminhoSaida, "w");

    if (saida == NULL) {
        fprintf(stderr, "Nao foi possivel criar o arquivo '%s'\n", caminhoSaida);
        return;
    }

    inicializaTemporarios();
    gerarQuadruplas(raizAST, NULL, true, 0, false);

    imprimirQuadruplas(saida);
}