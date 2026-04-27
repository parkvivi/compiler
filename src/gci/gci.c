#include "gci.h"
#include <string.h>

#define ARQUIVO_CODIGO_INTERMEDIARIO "build/codigointermediario.txt"
#define MAX_TEMPORARIOS 28

Quadrupla* lista_quadruplas = NULL;
Quadrupla* ultima_quadrupla = NULL;
Temporario temporarios[MAX_TEMPORARIOS];

int label_cont = 0;

void inicializaTemporarios() {

    for (int i=0; i<MAX_TEMPORARIOS; i++) {
        temporarios[i].estado = Livre;
        temporarios[i].nivelEscopo = -1;
        temporarios[i].variavel = NULL;
    }
}

char* novoTemporario(Simbolo* variavel, int nivelEscopo) {

    for (int i=0; i<MAX_TEMPORARIOS; i++) {
        if (temporarios[i].estado == Livre) {
            char* novoTemporario = malloc(sizeof(char)*20);
            sprintf(novoTemporario, "R%d", i + 1);

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

    fprintf(stderr, "GCI :: Erro ao criar novos temporarios");
    return NULL;
}

void preservarTemporarios() {

    for (int i=0; i<MAX_TEMPORARIOS; i++) {
        if (temporarios[i].estado != Livre) {
            char buffer[20];
            sprintf(buffer, "R%d", i + 1);

            emitirQuadrupla("PUSH", NULL, NULL, buffer);
        }
    }
}

void restaurarTemporarios() {

    for (int i=MAX_TEMPORARIOS-1; i>=0; i--) {
        if (temporarios[i].estado != Livre) {
            char buffer[20];
            sprintf(buffer, "R%d", i + 1);

            emitirQuadrupla("POP", NULL, NULL, buffer);
        }
    }
}

void reassociarTemporario(const char* temporario, Simbolo* variavel, int nivelEscopo) {

    if (temporario == NULL || temporario[0] != 'R' || variavel == NULL) return;

    int indiceTemporario = temporarioParaNumero(temporario);
    if (indiceTemporario < 0 || indiceTemporario >= MAX_TEMPORARIOS) {
        fprintf(stderr, "GCI :: Erro ao reassociar temporarios");
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

    if (temporario == NULL || temporario[0] != 'R') return -1;

    int indiceTemporario = temporarioParaNumero(temporario);
    if (indiceTemporario < 0 || indiceTemporario >= MAX_TEMPORARIOS) {
        fprintf(stderr, "GCI :: Erro ao liberar temporarios");
        return -1;
    }

    EstadoTemporario estado = temporarios[indiceTemporario].estado;

    // verificacao de casos de liberacao de temporarios para tipos aritmeticos e relacionais
    if (estado == AssociadoVariavel && liberarTemporario == false) {
        return 1;
    }

    if (estado == ValorTemporario && liberarTemporario == false) {
        return 2;
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

    if (temporario == NULL || temporario[0] != 'R') return -1;

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

char* gerarQuadruplas(AST* no, char* nomeFuncaoAtual, bool percorrerIrmaos, int nivelEscopo){

    if (!no) return NULL;

    char* arg = NULL;

    switch (no->tipo) {
        case TipoDeclaracaoFuncao:
            char* nomeFuncaoDeclarada = no->dado.nome;
            char* tipoFuncaoDeclarada = ((no->simbolo->tipo == TipoInt) ? "int" : "void");

            emitirQuadrupla("FUNCTION", nomeFuncaoDeclarada, tipoFuncaoDeclarada, NULL);

            char* argParametrosFuncao = gerarQuadruplas(no->filhos[1], nomeFuncaoDeclarada, true, nivelEscopo);
            liberarTemporario(argParametrosFuncao, true);

            char* argCorpoFuncao = gerarQuadruplas(no->filhos[2], nomeFuncaoDeclarada, true, nivelEscopo + 1);
            liberarTemporario(argCorpoFuncao, true);

            emitirQuadrupla("END", nomeFuncaoDeclarada, NULL, NULL);
            
            liberarTemporariosEscopoAtual(nivelEscopo + 1);
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

            char* condicaoIteracao = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true, nivelEscopo);
            emitirQuadrupla("IFF", condicaoIteracao, NULL, labelFinalIteracao);
            liberarTemporario(condicaoIteracao, true);

            char* argEscopoIteracao = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true, nivelEscopo + 1);
            liberarTemporario(argEscopoIteracao, true);

            emitirQuadrupla("GOTO", NULL, NULL, labelComecoIteracao);
            emitirQuadrupla("LABEL", NULL, NULL, labelFinalIteracao);

            liberarTemporariosEscopoAtual(nivelEscopo + 1);

            free(labelComecoIteracao);
            free(labelFinalIteracao);

            break;
        
        case TipoDeclaracaoSelecao:
            char* condicaoSelecao = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true, nivelEscopo);
            char* labelElse = ((no->filhos[2] == NULL) ? NULL : novoLabel());
            char* labelFinalSelecao = novoLabel();

            emitirQuadrupla("IFF", condicaoSelecao, NULL, ((labelElse == NULL) ? labelFinalSelecao : labelElse));
            liberarTemporario(condicaoSelecao, true);

            char* argEscopoSelecao = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true, nivelEscopo + 1);
            liberarTemporario(argEscopoSelecao, true);
            
            if (labelElse != NULL) {
                emitirQuadrupla("GOTO", NULL, NULL, labelFinalSelecao);
            }

            emitirQuadrupla("LABEL", NULL, NULL, ((labelElse == NULL) ? labelFinalSelecao : labelElse));

            liberarTemporariosEscopoAtual(nivelEscopo + 1);

            if (labelElse != NULL) {
                char* argElse = gerarQuadruplas(no->filhos[2], nomeFuncaoAtual, true, nivelEscopo + 1);
                liberarTemporario(argElse, true);
                
                emitirQuadrupla("LABEL", NULL, NULL, labelFinalSelecao);

                liberarTemporariosEscopoAtual(nivelEscopo + 1);

                free(labelElse);
            }

            free(labelFinalSelecao);

            break;
        
        case TipoDeclaracaoRetorno:
            char* operandoRetornado = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true, nivelEscopo);

            emitirQuadrupla("RETURN", operandoRetornado, NULL, NULL);
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

                emitirQuadrupla("LOAD", nomeVariavel, NULL, destinoConteudoVariavel);

                arg = destinoConteudoVariavel;
            } else {
                char* nomeVetor = no->simbolo->temporario;

                if (nomeVetor == NULL) {
                    nomeVetor = novoTemporario(no->simbolo, nivelEscopo);
                    emitirQuadrupla("LOAD", no->dado.nome, NULL, nomeVetor);
                }
                
                char* indiceAcessado = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true, nivelEscopo);
                char* destinoConteudoAcessado = novoTemporario(NULL, nivelEscopo);

                emitirQuadrupla("LOAD", nomeVetor, indiceAcessado, destinoConteudoAcessado);

                if (liberarTemporario(indiceAcessado, false) == 2) {
                    liberarTemporario(indiceAcessado, true);
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
            char* destinoIntermediario = ((no->simbolo->tipo == TipoInt) ? "R0" : NULL);

            bool ehOutput = (strcmp(nomeFuncaoChamada, "output") == 0);
            bool mudarContexto = (strcmp(nomeFuncaoChamada, "input") != 0 && ehOutput == false);

            if (mudarContexto == true) {
                preservarTemporarios();
            }

            if (ehOutput == true) {
                char* origemArgumento = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, false, nivelEscopo);
                emitirQuadrupla("MOVE", origemArgumento, NULL, "R0");

                liberarTemporario(origemArgumento, true);
            } else {
                for (AST* argumento=no->filhos[0]; argumento!=NULL; argumento=argumento->irmao) {
                    contaArgumentos++;
                    char* origemArgumento = gerarQuadruplas(argumento, nomeFuncaoAtual, false, nivelEscopo + 1);
                    emitirQuadrupla("ARG", origemArgumento, NULL, NULL);

                    if (liberarTemporario(origemArgumento, false) == 2){
                        liberarTemporario(origemArgumento, true);
                    }
                }

                liberarTemporariosEscopoAtual(nivelEscopo + 1);
            }

            char quantidadeArgumentos[20];
            sprintf(quantidadeArgumentos, "%d", contaArgumentos);

            emitirQuadrupla("CALL", nomeFuncaoChamada, quantidadeArgumentos, destinoIntermediario);
            
            if (mudarContexto == true) {
                restaurarTemporarios();
            }

            char* destinoValorRetornado = ((no->simbolo->tipo == TipoInt) ? novoTemporario(NULL, nivelEscopo) : NULL);
            
            if (no->simbolo->tipo == TipoInt) {
                emitirQuadrupla("MOVE", "R0", NULL, destinoValorRetornado);
            }

            arg = destinoValorRetornado;
            break;

        case TipoParametro:
            char* nomeParametro = no->dado.nome;
            
            emitirQuadrupla("PARAM", nomeFuncaoAtual, nomeParametro, NULL);

            break;
        
        case TipoAtribuicao:
            char* operando = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true, nivelEscopo);

            AST* filhoEsquerdo = no->filhos[0];
            if (filhoEsquerdo->tipo == TipoVar) {
                char* nomeVariavel = filhoEsquerdo->dado.nome;
                if (filhoEsquerdo->filhos[0] != NULL) {
                    if (filhoEsquerdo->simbolo->temporario == NULL) {
                        char* novo = novoTemporario(filhoEsquerdo->simbolo, nivelEscopo);
                        emitirQuadrupla("LOAD", nomeVariavel, NULL, novo);
                        nomeVariavel = novo;
                    } else {
                        nomeVariavel = filhoEsquerdo->simbolo->temporario;
                    }
                }

                char* indiceAcessado = gerarQuadruplas(filhoEsquerdo->filhos[0], nomeFuncaoAtual, true, nivelEscopo);
                emitirQuadrupla("STORE", nomeVariavel, indiceAcessado, operando);

                if (filhoEsquerdo->filhos[0] == NULL && liberarTemporario(operando, false) != 1) {
                    reassociarTemporario(operando, filhoEsquerdo->simbolo, nivelEscopo);
                }
                
                if (liberarTemporario(indiceAcessado, false) == 2) {
                    liberarTemporario(indiceAcessado, true);
                }

                arg = operando;
            } else {
                char* destinoAtribuicao = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true, nivelEscopo);
                
                emitirQuadrupla("ASSIGN", operando, NULL, destinoAtribuicao);
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
            char* operacao = verificarOperacao(no->tipo);
            char* operandoEsquerdo = gerarQuadruplas(no->filhos[0], nomeFuncaoAtual, true, nivelEscopo);
            char* operandoDireito = gerarQuadruplas(no->filhos[1], nomeFuncaoAtual, true, nivelEscopo);
            char* destinoOperacao = NULL;
            bool liberarDireito = false;

            if (no->filhos[0]->tipo == TipoNum) {
                char* novo = novoTemporario(NULL, nivelEscopo);
                emitirQuadrupla("LOAD", operandoEsquerdo, NULL, novo);
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

            emitirQuadrupla(operacao, operandoEsquerdo, operandoDireito, destinoOperacao);
            
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

                char* argFilhos = gerarQuadruplas(filho, nomeFuncaoAtual, true, nivelEscopo);
                liberarTemporario(argFilhos, true);
            }

            break;
    }

    if (percorrerIrmaos == true) {
        char* argIrmao = gerarQuadruplas(no->irmao, nomeFuncaoAtual, true, nivelEscopo);
        liberarTemporario(argIrmao, true);
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
    gerarQuadruplas(raizAST, NULL, true, 0);

    imprimirQuadruplas(saida);
    liberarQuadruplas();
}