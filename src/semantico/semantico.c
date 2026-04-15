#include "semantico.h"

#define ARQUIVO_TABELA_SIMBOLOS "build/tabsimbolos.txt"

TipoSimbolo retornaTipo(AST* no) {

    // Nó tem que ser do tipo TipoTipoEspecificador!
    if (strcmp(no->dado.nome, "void") == 0) {
        return TipoVoid;
    }

    return TipoInt;
}

void criarTabSimbolosInterno(AST* no, FILE* arquivo, int nivelEscopo, int* proximoIdEscopo) {

    if (no == NULL) return;

    bool escopoCriado = false;
    int idEscopoAtual = -1;

    switch(no->tipo) {
        case TipoEscopo:
        case TipoDeclaracaoFuncao:
            if (no->tipo == TipoDeclaracaoFuncao) {
                Simbolo* funcDeclarada = buscarSimboloEscopoAtual(no->dado.nome);
                if (funcDeclarada != NULL) {
                    char buffer[50];
                    sprintf(buffer, "Identificador '%s' em uso", no->dado.nome);
                    erroSemantico(buffer);
                } else {
                    TipoSimbolo tipo = retornaTipo(no->filhos[0]);
                    Simbolo* s = declararSimbolo(no->dado.nome, Funcao, tipo, false, -1);

                    int quantidadeParametros = 0;
                    Parametro* listaParametros = NULL;
                    Parametro* ultimoParametro = NULL;
                    for (AST* param=no->filhos[1]; param!=NULL; param=param->irmao, quantidadeParametros++) {
                        Parametro* p = (Parametro*) malloc(sizeof(Parametro));
                        p->tipoParam = retornaTipo(param->filhos[0]);
                        p->paramVetor = param->filhos[1] == NULL ? false: true;
                        p->prox = NULL;
                        if (ultimoParametro == NULL) {
                            listaParametros = p;
                            ultimoParametro = p;
                        } else {
                            ultimoParametro->prox = p;
                            ultimoParametro = p;
                        }
                    }
                    s->quantidadeParametros = quantidadeParametros;
                    s->parametros = listaParametros;

                    no->simbolo = s;
                }
            }

            entrarEscopo();
            escopoCriado = true;
            idEscopoAtual = *proximoIdEscopo;
            *proximoIdEscopo += 1;

            break;

        case TipoVar:
            Simbolo* varProcurada = buscarSimboloTodosEscopos(no->dado.nome, Variavel, true);
            if (varProcurada == NULL) {
                char buffer[50];
                sprintf(buffer, "Variavel '%s' nao declarada", no->dado.nome);
                erroSemantico(buffer);
            }
            no->simbolo = varProcurada;

            break;

        case TipoDeclaracaoVariavel:
        case TipoParametro:
            Simbolo* varDeclarada = buscarSimboloEscopoAtual(no->dado.nome);
            if (varDeclarada != NULL) {
                char buffer[50];
                sprintf(buffer, "Identificador '%s' em uso", no->dado.nome);
                erroSemantico(buffer);
            }

            TipoSimbolo tipo = retornaTipo(no->filhos[0]);
            bool ehVetor = no->filhos[1] == NULL ? false : true;
            int tamanhoVetor;

            if (ehVetor == true) {
                if (no->filhos[1]->tipo == TipoNum) {
                    tamanhoVetor = no->filhos[1]->dado.valor;
                } else {
                    tamanhoVetor = -1;
                }
            } else {
                tamanhoVetor = -1;
            }

            Simbolo* s = declararSimbolo(no->dado.nome, Variavel, tipo, ehVetor, tamanhoVetor);
            no->simbolo = s;

            break;
        
        case TipoChamadaFuncao:
            Simbolo* funcProcurada = buscarSimboloTodosEscopos(no->dado.nome, Funcao, true);
            if (funcProcurada == NULL) {
                char buffer[50];
                sprintf(buffer, "Funcao '%s' nao declarada", no->dado.nome);
                erroSemantico(buffer);
            }

            int quantidadeArgumentos = 0;
            for (AST* arg=no->filhos[0]; arg!=NULL; arg=arg->irmao, quantidadeArgumentos++);
            if (quantidadeArgumentos != funcProcurada->quantidadeParametros) {
                char buffer[100];
                sprintf(buffer, "Funcao '%s' chamada com argumentos a mais ou insuficientes", no->dado.nome);
                erroSemantico(buffer);
            }

            no->simbolo = funcProcurada;

            break;
        
        default:
            break;
    }

    for (int i=0; i<MAXFILHOS; i++) {
        AST* filho = no->filhos[i];
        if (filho == NULL) {
            continue;
        }

        criarTabSimbolosInterno(filho, arquivo, escopoCriado ? nivelEscopo + 1 : nivelEscopo, proximoIdEscopo);
    }

    if (escopoCriado == true) {
        imprimirEscopoAtual(arquivo, idEscopoAtual, nivelEscopo);
        sairEscopo();
    }

    criarTabSimbolosInterno(no->irmao, arquivo, nivelEscopo, proximoIdEscopo);
}

void criarTabSimbolos(AST* no, FILE* arquivo) {

    int proximoIdEscopo = 1;
    criarTabSimbolosInterno(no, arquivo, 1, &proximoIdEscopo);
}

TipoSimbolo verificarTipos(AST* no) {

    if (no == NULL) return TipoVoid;

    TipoSimbolo tipo;

    switch(no->tipo) {
        case TipoNum:
            tipo = TipoInt;
            break;
            
        case TipoTipoEspecificador:
            tipo = retornaTipo(no);
            break;

        case TipoDeclaracaoVariavel:
        case TipoParametro:
            TipoSimbolo tipoVarParam = verificarTipos(no->filhos[0]); 

            if (tipoVarParam != TipoInt) {
                erroSemantico("Variavel/parametro declarado como void");
            }

            tipo = TipoVoid;
            break;

        case TipoVar:
            if (no->simbolo->tipo != TipoInt) {
                erroSemantico("Variavel/parametro eh void");
            }

            TipoSimbolo tipoIndiceAcessado = verificarTipos(no->filhos[0]);
            if (no->filhos[0] != NULL && tipoIndiceAcessado != TipoInt) {
                char buffer[50];
                sprintf(buffer, "Indice nao inteiro acessado no vetor '%s'", no->dado.nome);
                erroSemantico(buffer);
            }

            tipo = no->simbolo->tipo;
            break;

        case TipoSoma:
        case TipoSubtracao:
        case TipoMultiplicacao:
        case TipoDivisao:
        case TipoMenorIgual:
        case TipoMenor:
        case TipoMaiorIgual:
        case TipoMaior:
        case TipoIgual:
        case TipoDiferente:
        case TipoAtribuicao:
            TipoSimbolo tipoEsquerdo = verificarTipos(no->filhos[0]);
            TipoSimbolo tipoDireito = verificarTipos(no->filhos[1]);

            if (tipoEsquerdo == TipoVoid || tipoDireito == TipoVoid) {
                erroSemantico("Expressoes so aceitam inteiros");
            }

            AST* filhoEsquerdo = no->filhos[0];
            if (filhoEsquerdo->tipo == TipoVar && filhoEsquerdo->simbolo->ehVetor == true && filhoEsquerdo->filhos[0] == NULL) {
                char buffer[50];
                sprintf(buffer, "Vetor '%s' sendo usando como variavel inteira", filhoEsquerdo->dado.nome);
                erroSemantico(buffer);
            }

            AST* filhoDireito = no->filhos[1];
            if (filhoDireito->tipo == TipoVar && filhoDireito->simbolo->ehVetor == true && filhoDireito->filhos[0] == NULL) {
                char buffer[50];
                sprintf(buffer, "Vetor '%s' sendo usando como variavel inteira", filhoDireito->dado.nome);
                erroSemantico(buffer);
            }

            tipo = TipoInt;
            break;

        case TipoChamadaFuncao:
            AST* argumento = no->filhos[0];
            Parametro* parametro = no->simbolo->parametros;

            while (argumento != NULL) {
                TipoSimbolo tipo = verificarTipos(argumento);

                if (tipo != parametro->tipoParam || (argumento->tipo == TipoVar && argumento->simbolo->ehVetor != parametro->paramVetor)) {
                    char buffer[50];
                    sprintf(buffer, "Funcao '%s' chamada com argumentos incompativeis", no->dado.nome);
                    erroSemantico(buffer);
                }

                argumento = argumento->irmao;
                parametro = parametro->prox;
            }

            tipo = no->simbolo->tipo;
            break;

        case TipoDeclaracaoFuncao:
            TipoSimbolo tipoFuncao = verificarTipos(no->filhos[0]);
            if (tipoFuncao == TipoInt) {
                // tem que ter return int
            } else {
                // nao pode ter return int
            }
            
            // mudar análise dos parâmetros pra cá???

            verificarTipos(no->filhos[1]);
            verificarTipos(no->filhos[2]);
            
            tipo = TipoVoid;
            break;
        
        case TipoDeclaracaoRetorno:
            AST* expressaoRetornada = no->filhos[0];
            if (expressaoRetornada != NULL && expressaoRetornada->tipo == TipoVar && expressaoRetornada->simbolo->ehVetor == true && expressaoRetornada->filhos[0] == NULL) {
                char buffer[50];
                sprintf(buffer, "Vetor '%s' sendo usando como variavel inteira", expressaoRetornada->dado.nome);
                erroSemantico(buffer);
            }

            TipoSimbolo tipoRetorno = verificarTipos(no->filhos[0]);

            if (expressaoRetornada != NULL && tipoRetorno != TipoInt) {
                erroSemantico("Retorno de tipo incompativel");
            }

            tipo = tipoRetorno;
            break;

        default:
            for (int i=0; i<MAXFILHOS; i++) {
                verificarTipos(no->filhos[i]);
            }

            tipo = TipoVoid;
            break;
    }

    verificarTipos(no->irmao);
    return tipo;
}

void analisarSemantica() {
    
    FILE* saida = fopen(ARQUIVO_TABELA_SIMBOLOS, "w");

    if (saida == NULL) {
        fprintf(stderr, "Nao foi possivel criar o arquivo auxiliar de escopos em '%s'\n", ARQUIVO_TABELA_SIMBOLOS);
        return;
    }

    fprintf(saida, "Relatorio de escopos e simbolos\n\n");

    entrarEscopo();

    Simbolo* s;
    s = declararSimbolo("input", Funcao, TipoInt, false, -1);
    s->quantidadeParametros = 0;
    s = declararSimbolo("output", Funcao, TipoVoid, false, -1);
    s->quantidadeParametros = 1;

    criarTabSimbolos(raizAST, saida);

    imprimirEscopoAtual(saida, 0, 0);
    sairEscopo();

    verificarTipos(raizAST);

    liberarTabSimbolos();

    fclose(saida);
}

void erroSemantico(const char* mensagem) {
	fprintf(stderr, "ERRO SEMANTICO :: \"%s\"\n", mensagem);
	exit(1);
}