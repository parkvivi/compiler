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
                    fprintf(stderr, "ERRO SEMANTICO :: Identificador '%s' em uso\n", no->dado.nome);
                    break;
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
                fprintf(stderr, "ERRO SEMANTICO :: Variavel '%s' nao declarada\n", no->dado.nome);
                break;
            }
            no->simbolo = varProcurada;

            break;

        case TipoDeclaracaoVariavel:
        case TipoParametro:
            Simbolo* varDeclarada = buscarSimboloEscopoAtual(no->dado.nome);
            if (varDeclarada != NULL) {
                fprintf(stderr, "ERRO SEMANTICO :: Identificador '%s' em uso\n", no->dado.nome);
                break;
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
                fprintf(stderr, "ERRO SEMANTICO :: Funcao '%s' nao declarada\n", no->dado.nome);
                break;
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

    // verificação de tipos
    switch(no->tipo) {
        case TipoTipoEspecificador:
            return retornaTipo(no);

        case TipoDeclaracaoVariavel:
        case TipoParametro:
            if (tiposFilhos[0] != TipoInt) {
                fprintf(stderr, "ERRO SEMANTICO :: Variavel/parametro declarado como void");
                return TipoVoid;
            }

            return TipoVoid;

        case TipoVar:
            if (no->simbolo->tipo != TipoInt) {
                fprintf(stderr, "ERRO SEMANTICO :: Variavel/parametro eh void");
                return TipoVoid;
            }

            return no->simbolo->tipo;

        case TipoSoma:
        case TipoSubtracao:
        case TipoMultiplicacao:
        case TipoDivisao:
        case TipoAtribuicao:
        case TipoMenorIgual:
        case TipoMenor:
        case TipoMaiorIgual:
        case TipoMaior:
        case TipoIgual:
        case TipoDiferente:
            if (tiposFilhos[0] == TipoVoid || tiposFilhos[1] == TipoVoid) {
                fprintf(stderr, "ERRO SEMANTICO :: Expressoes so aceitam inteiros");
                return TipoVoid;
            }

            return TipoInt;

        case TipoChamadaFuncao:
            return no->simbolo->tipo;

        default:
            return TipoVoid;
    }

    verificarTipos(no->irmao);
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
    liberarTabSimbolos();

    fclose(saida);
}

// TODO: Incorporar no código
void erroSemantico(char* mensagem) {
	fprintf(stderr, "ERRO SEMANTICO :: \"%s\"\n", mensagem);
	exit(1);
}