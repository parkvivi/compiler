#include "semantico.h"

#define ARQUIVO_AUXILIAR_ESCOPO "build/escopos_simbolos.txt"

static void criarTabSimbolosInterno(AST* no, FILE* saida, int nivelEscopo, int* proximoIdEscopo) {

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
                    declararSimbolo(no->dado.nome, Funcao, tipo, false, 0);
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

        default:
            break;
    }

    for (int i=0; i<MAXFILHOS; i++) {
        AST* filho = no->filhos[i];
        if (filho == NULL) {
            continue;
        }

        criarTabSimbolosInterno(
            filho,
            saida,
            escopoCriado ? nivelEscopo + 1 : nivelEscopo,
            proximoIdEscopo
        );
    }

    if (escopoCriado == true) {
        imprimirEscopoAtual(saida, idEscopoAtual, nivelEscopo);
        sairEscopo();
    }

    criarTabSimbolosInterno(no->irmao, saida, nivelEscopo, proximoIdEscopo);
}

TipoSimbolo retornaTipo(AST* no) {

    // Antes de usar, ter CERTEZA de que no->tipo é TipoTipoEspecificador
    if (strcmp(no->dado.nome, "void") == 0) {
        return TipoVoid;
    }

    return TipoInt;
}

void criarTabSimbolos(AST* no) {
    int proximoIdEscopo = 1;
    criarTabSimbolosInterno(no, NULL, 1, &proximoIdEscopo);
}

void verificarTipos() {
    // Função que percorre AST em pós-ordem

}

void analisarSemantica() {
    
    FILE* saida = fopen(ARQUIVO_AUXILIAR_ESCOPO, "w");

    if (saida == NULL) {
        fprintf(stderr, "Nao foi possivel criar o arquivo auxiliar de escopos em '%s'\n", ARQUIVO_AUXILIAR_ESCOPO);
        return;
    }

    fprintf(saida, "Relatorio de escopos e simbolos\n\n");

    entrarEscopo();

    int proximoIdEscopo = 1;
    criarTabSimbolosInterno(raizAST, saida, 1, &proximoIdEscopo);

    imprimirEscopoAtual(saida, 0, 0);
    sairEscopo();

    fclose(saida);
}