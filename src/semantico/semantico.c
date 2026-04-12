#include "semantico.h"

#define ARQUIVO_TABELA_SIMBOLOS "build/tabsimbolos.txt"

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
                } else {
                    TipoSimbolo tipo = retornaTipo(no->filhos[0]);
                    declararSimbolo(no->dado.nome, Funcao, tipo, false, -1);
                }
            }

            entrarEscopo();
            escopoCriado = true;
            idEscopoAtual = *proximoIdEscopo;
            *proximoIdEscopo += 1;

            break;

        case TipoVar: {
            Simbolo* varProcurada = buscarSimboloTodosEscopos(no->dado.nome, Variavel, true);
            if(varProcurada == NULL) {
                fprintf(stderr, "ERRO SEMANTICO :: Variavel '%s' nao declarada\n", no->dado.nome);
            }

            break;
        }

        case TipoDeclaracaoVariavel:
        case TipoParametro:
            if (no->tipo == TipoParametro && no->filhos[1] != NULL && no->filhos[1]->tipo == TipoParametroVetor) {
                Simbolo* varDeclarada = buscarSimboloEscopoAtual(no->dado.nome);
                if (varDeclarada != NULL) {
                    fprintf(stderr, "ERRO SEMANTICO :: Identificador '%s' em uso\n", no->dado.nome);
                } else {
                    TipoSimbolo tipo = retornaTipo(no->filhos[0]);
                    declararSimbolo(no->dado.nome, Variavel, tipo, true, -1);
                }

                break;
            }

            {
                Simbolo* varDeclarada = buscarSimboloEscopoAtual(no->dado.nome);
                if (varDeclarada != NULL) {
                    fprintf(stderr, "ERRO SEMANTICO :: Identificador '%s' em uso\n", no->dado.nome);
                } else {
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

                    declararSimbolo(no->dado.nome, Variavel, tipo, ehVetor, tamanhoVetor);
                }
            }

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

TipoSimbolo retornaTipo(AST* no) {

    if (strcmp(no->dado.nome, "void") == 0) {
        return TipoVoid;
    }

    return TipoInt;
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
        case TipoDeclaracaoVariavel:
        case TipoParametro:
            if (tiposFilhos[0] != TipoInt) {
                fprintf(stderr, "ERRO SEMANTICO :: Variavel/parametro declarado como void");
            }

            return TipoVoid;

        case TipoVar:
            // verificar tipo
            return TipoInt;

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
            }

            if (tiposFilhos[0] != tiposFilhos[1]) {
                fprintf(stderr, "ERRO SEMANTICO :: Tipos inconsistentes");
            }

            break;

        case TipoChamadaFuncao:
            // retornar o tipo da função
            break;

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