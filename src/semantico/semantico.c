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
            if(varProcurada == NULL) {
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
            if(funcProcurada == NULL) {
                char buffer[50];
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

    TipoSimbolo tiposFilhos[MAXFILHOS];
    for (int i=0; i<MAXFILHOS; i++) {
        tiposFilhos[i] = verificarTipos(no->filhos[i]);
    }

    TipoSimbolo tipoRetornado;

    switch(no->tipo) {
        case TipoNum:
            tipoRetornado = TipoInt;
            break;
            
        case TipoTipoEspecificador:
            tipoRetornado = retornaTipo(no);
            break;

        case TipoDeclaracaoVariavel:
        case TipoParametro:
            if (tiposFilhos[0] != TipoInt) {
                erroSemantico("Variavel/parametro declarado como void");
                tipoRetornado = TipoVoid;
                break;
            }

            tipoRetornado = TipoVoid;
            break;

        case TipoVar:
            if (no->simbolo->tipo != TipoInt) {
                erroSemantico("Variavel/parametro eh void");
                tipoRetornado = TipoVoid;
                break;
            }

            tipoRetornado = no->simbolo->tipo;
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
            if (tiposFilhos[0] == TipoVoid || tiposFilhos[1] == TipoVoid) {
                //fprint(stderr, "");
                erroSemantico("Expressoes so aceitam inteiros");
                tipoRetornado = TipoVoid;
                break;
            }

            tipoRetornado = TipoInt;
            break;

        case TipoChamadaFuncao:
            tipoRetornado = no->simbolo->tipo;
            break;

        case TipoDeclaracaoFuncao:
            if (tiposFilhos[0] == TipoInt) {
                // tem que ter return int
            } else {
                // nao pode ter return int
            }
            
            tipoRetornado = TipoVoid;
            break;

        default:
            tipoRetornado = TipoVoid;
            break;
    }

    verificarTipos(no->irmao);
    return tipoRetornado;
}

void analisarSemantica() {
    
    FILE* saida = fopen(ARQUIVO_TABELA_SIMBOLOS, "w");

    if (saida == NULL) {
        fprintf(stderr, "Nao foi possivel criar o arquivo auxiliar de escopos em '%s'\n", ARQUIVO_TABELA_SIMBOLOS);
        return;
    }

    fprintf(saida, "Relatorio de escopos e simbolos\n\n");

    entrarEscopo();

    declararSimbolo("input", Funcao, TipoInt, false, -1);
    declararSimbolo("output", Funcao, TipoVoid, false, -1);

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