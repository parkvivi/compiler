#include "semantico.h"
#include "caminho_saida.h"

#include <stdlib.h>

TipoSimbolo retornaTipo(AST* no) {

    // Nó tem que ser do tipo TipoTipoEspecificador!
    if (strcmp(no->dado.nome, "void") == 0) {
        return TipoVoid;
    }

    return TipoInt;
}

void criarTabSimbolos(AST* no, FILE* arquivo, int nivelEscopo, int* proximoIdEscopo) {

    if (no == NULL) return;

    bool escopoCriado = false;
    int idEscopoAtual = -1;

    switch(no->tipo) {
        case TipoEscopo:
        case TipoDeclaracaoFuncao:
            if (no->tipo == TipoDeclaracaoFuncao) {
                Simbolo* funcDeclarada = buscarSimboloEscopoAtual(no->dado.nome);
                if (funcDeclarada != NULL) {
                    char buffer[80];
                    sprintf(buffer, "Identificador '%s' em uso", no->dado.nome);
                    erroSemantico(buffer);
                } else {
                    TipoSimbolo tipo = retornaTipo(no->filhos[0]);
                    Simbolo* s = declararSimbolo(no->dado.nome, Funcao, tipo, false, -1);
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
                char buffer[80];
                sprintf(buffer, "Variavel '%s' nao declarada", no->dado.nome);
                erroSemantico(buffer);
            }
            no->simbolo = varProcurada;

            break;

        case TipoDeclaracaoVariavel:
        case TipoParametro:
            Simbolo* varDeclarada = buscarSimboloEscopoAtual(no->dado.nome);
            if (varDeclarada != NULL) {
                char buffer[80];
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
                char buffer[80];
                sprintf(buffer, "Funcao '%s' nao declarada", no->dado.nome);
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

        criarTabSimbolos(filho, arquivo, (escopoCriado ? (nivelEscopo + 1) : nivelEscopo), proximoIdEscopo);
    }

    if (escopoCriado == true) {
        imprimirEscopoAtual(arquivo, idEscopoAtual, nivelEscopo);
        sairEscopo();
    }

    criarTabSimbolos(no->irmao, arquivo, nivelEscopo, proximoIdEscopo);
}

TipoSimbolo verificarTipos(AST* no, bool* encontrouReturnInt, bool* encontrouReturnVoid) {

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
            TipoSimbolo tipoVarParam = verificarTipos(no->filhos[0], encontrouReturnInt, encontrouReturnVoid); 

            if (tipoVarParam != TipoInt) {
                erroSemantico("Variavel/parametro declarado como void");
            }

            tipo = TipoVoid;
            break;

        case TipoVar:
            if (no->simbolo->tipo != TipoInt) {
                erroSemantico("Variavel/parametro eh void");
            }

            TipoSimbolo tipoIndiceAcessado = verificarTipos(no->filhos[0], encontrouReturnInt, encontrouReturnVoid);
            if (no->filhos[0] != NULL && tipoIndiceAcessado != TipoInt) {
                char buffer[80];
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
            TipoSimbolo tipoEsquerdo = verificarTipos(no->filhos[0], encontrouReturnInt, encontrouReturnVoid);
            TipoSimbolo tipoDireito = verificarTipos(no->filhos[1], encontrouReturnInt, encontrouReturnVoid);

            if (tipoEsquerdo == TipoVoid || tipoDireito == TipoVoid) {
                erroSemantico("Expressoes so aceitam inteiros");
            }

            AST* filhoEsquerdo = no->filhos[0];
            if (filhoEsquerdo->tipo == TipoVar && filhoEsquerdo->simbolo->ehVetor == true && filhoEsquerdo->filhos[0] == NULL) {
                char buffer[80];
                sprintf(buffer, "Vetor '%s' sendo usando como variavel inteira", filhoEsquerdo->dado.nome);
                erroSemantico(buffer);
            }

            AST* filhoDireito = no->filhos[1];
            if (filhoDireito->tipo == TipoVar && filhoDireito->simbolo->ehVetor == true && filhoDireito->filhos[0] == NULL) {
                char buffer[80];
                sprintf(buffer, "Vetor '%s' sendo usando como variavel inteira", filhoDireito->dado.nome);
                erroSemantico(buffer);
            }

            tipo = TipoInt;
            break;

        case TipoChamadaFuncao:
            AST* argumento = no->filhos[0];
            Parametro* parametro = no->simbolo->parametros;

            while (argumento != NULL && parametro != NULL) {
                TipoSimbolo tipo = verificarTipos(argumento, encontrouReturnInt, encontrouReturnVoid);
                bool argumentoEhVetor = (argumento->tipo == TipoVar && argumento->simbolo->ehVetor == true && argumento->filhos[0] == NULL);

                if (tipo != parametro->tipoParam || argumentoEhVetor != parametro->paramVetor) {
                    char buffer[80];
                    sprintf(buffer, "Funcao '%s' chamada com argumentos incompativeis", no->dado.nome);
                    erroSemantico(buffer);
                }

                argumento = argumento->irmao;
                parametro = parametro->prox;
            }

            if (argumento != NULL || parametro != NULL) {
                char buffer[80];
                sprintf(buffer, "Funcao '%s' chamada com argumentos a mais ou insuficientes", no->dado.nome);
                erroSemantico(buffer);
            }

            tipo = no->simbolo->tipo;
            break;

        case TipoDeclaracaoFuncao:
            TipoSimbolo tipoFuncao = verificarTipos(no->filhos[0], encontrouReturnInt, encontrouReturnVoid);
            
            Simbolo* s = no->simbolo;
            Parametro* listaParametros = NULL;
            Parametro* ultimoParametro = NULL;
            int quantidadeParametros = 0;
            for (AST* param=no->filhos[1]; param!=NULL; param=param->irmao) {
                Parametro* p = (Parametro*) malloc(sizeof(Parametro));
                p->tipoParam = retornaTipo(param->filhos[0]);
                p->paramVetor = param->filhos[1] == NULL ? false: true;
                p->prox = NULL;
                quantidadeParametros++;
                if (ultimoParametro == NULL) {
                    listaParametros = p;
                    ultimoParametro = p;
                } else {
                    ultimoParametro->prox = p;
                    ultimoParametro = p;
                }
            }
            s->parametros = listaParametros;
            s->quantidadeParametros = quantidadeParametros;

            verificarTipos(no->filhos[1], encontrouReturnInt, encontrouReturnVoid);

            *encontrouReturnInt = false;
            *encontrouReturnVoid = false;
        
            verificarTipos(no->filhos[2], encontrouReturnInt, encontrouReturnVoid);

            if ((tipoFuncao == TipoInt && *encontrouReturnVoid == true) || (tipoFuncao == TipoVoid && *encontrouReturnInt == true)) {
                char buffer[80];
                sprintf(buffer, "Funcao '%s' possui retorno de tipo incompativel em seu escopo", no->dado.nome);
                erroSemantico(buffer);
            }

            if (tipoFuncao == TipoInt && *encontrouReturnInt == false) {
                char buffer[80];
                sprintf(buffer, "Funcao '%s' eh de tipo int mas nao retorna nada", no->dado.nome);
                erroSemantico(buffer);
            }

            if (tipoFuncao == TipoInt) {
                char buffer[200];
                sprintf(buffer, "Certifique-se de que todos os caminhos da funcao '%s' incluem retorno", no->dado.nome);
                avisoSemantico(buffer);
            }
            
            tipo = TipoVoid;
            break;
        
        case TipoDeclaracaoRetorno:
            AST* expressaoRetornada = no->filhos[0];
            if (expressaoRetornada != NULL && expressaoRetornada->tipo == TipoVar && expressaoRetornada->simbolo->ehVetor == true && expressaoRetornada->filhos[0] == NULL) {
                char buffer[80];
                sprintf(buffer, "Vetor '%s' sendo usando como variavel inteira", expressaoRetornada->dado.nome);
                erroSemantico(buffer);
            }

            TipoSimbolo tipoRetorno = verificarTipos(no->filhos[0], encontrouReturnInt, encontrouReturnVoid);
            if (expressaoRetornada != NULL) {
                if (tipoRetorno != TipoInt) {
                    erroSemantico("Retorno de tipo incompativel");
                }
                *encontrouReturnInt = true;
            } else {
                *encontrouReturnVoid = true;
            }

            tipo = tipoRetorno;
            break;

        default:
            for (int i=0; i<MAXFILHOS; i++) {
                AST* filho = no->filhos[i];
                if (filho == NULL) {
                    continue;
                }

                verificarTipos(filho, encontrouReturnInt, encontrouReturnVoid);
            }

            tipo = TipoVoid;
            break;
    }

    verificarTipos(no->irmao, encontrouReturnInt, encontrouReturnVoid);
    return tipo;
}

void analisarMain() {

    Escopo* e;
    for (e=lista_escopos; e!=NULL && e->prox!=NULL; e=e->prox);

    Simbolo* main = e->simbolos;
    if (main == NULL || strcmp(main->nome, "main") != 0 || main->tipo != TipoVoid || main->parametros != NULL) {
        erroSemantico("Funcao 'main' nao declarada ou declarada incorretamente");
    }
}

void analisarSemantica() {

    char caminhoSaida[260];
    montarCaminhoSaida(caminhoSaida, sizeof(caminhoSaida), "tabsimbolos.txt");

    FILE* saida = fopen(caminhoSaida, "w");

    if (saida == NULL) {
        fprintf(stderr, "Nao foi possivel criar o arquivo auxiliar de escopos em '%s'\n", caminhoSaida);
        return;
    }

    fprintf(saida, "Relatorio de escopos e simbolos\n\n");

    entrarEscopo();

    Simbolo* s;
    s = declararSimbolo("input", Funcao, TipoInt, false, -1);

    s = declararSimbolo("output", Funcao, TipoVoid, false, -1);
    Parametro* pOutput = (Parametro*) malloc(sizeof(Parametro));
    pOutput->tipoParam = TipoInt;
    pOutput->paramVetor = false;
    pOutput->prox = NULL;
    s->parametros = pOutput;

    // Funções reservadas do SO
    s = declararSimbolo("carregarPrograma", Funcao, TipoVoid, false, -1);
    // 1 parâmetro do tipo inteiro (índice do programa no HD)
    Parametro* pCarregarPrograma = (Parametro*) malloc(sizeof(Parametro));
    pCarregarPrograma->tipoParam = TipoInt;
    pCarregarPrograma->paramVetor = false;
    pCarregarPrograma->prox = NULL;
    s->parametros = pCarregarPrograma;

    declararSimbolo("preservarContexto", Funcao, TipoVoid, false, -1);
    declararSimbolo("executarPrograma", Funcao, TipoVoid, false, -1);
    declararSimbolo("restaurarContexto", Funcao, TipoVoid, false, -1);
    
    s = declararSimbolo("configurarBase", Funcao, TipoVoid, false, -1);
    // 1 parâmetro do tipo inteiro (endereco base do programa na memória de dados)
    Parametro* pConfigurarBase = (Parametro*) malloc(sizeof(Parametro));
    pConfigurarBase->tipoParam = TipoInt;
    pConfigurarBase->paramVetor = false;
    pConfigurarBase->prox = NULL;
    s->parametros = pConfigurarBase;

    int proximoIdEscopo = 1;
    criarTabSimbolos(raizAST, saida, 1, &proximoIdEscopo);

    imprimirEscopoAtual(saida, 0, 0);
    sairEscopo();

    bool encontrouReturnInt = false;
    bool encontrouReturnVoid = false;
    verificarTipos(raizAST, &encontrouReturnInt, &encontrouReturnVoid);

    analisarMain();

    fclose(saida);
}

void avisoSemantico(const char* mensagem) {
    fprintf(stderr, "AVISO SEMANTICO :: \"%s\"\n", mensagem);
}

void erroSemantico(const char* mensagem) {
	fprintf(stderr, "ERRO SEMANTICO :: \"%s\"\n", mensagem);
	exit(1);
}