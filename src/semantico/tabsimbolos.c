#include <stdio.h>
#include <string.h>
#include "tabsimbolos.h"

extern int linha_atual;

Escopo* escopo_atual = NULL;
Escopo* lista_escopos = NULL;

void entrarEscopo() {

    Escopo* novoEscopo = (Escopo*) malloc(sizeof(Escopo));

    novoEscopo->simbolos = NULL;
    novoEscopo->prox = escopo_atual;

    if (escopo_atual != NULL) {
        if (escopo_atual->ant != NULL) {
            escopo_atual->ant->prox = novoEscopo;
            novoEscopo->ant = escopo_atual->ant;
        } else {
            novoEscopo->ant = NULL;
            lista_escopos = novoEscopo;
        }
        escopo_atual->ant = novoEscopo;
    } else {
        if (lista_escopos == NULL) {
            novoEscopo->ant = NULL;
            lista_escopos = novoEscopo;
        } else {
            Escopo* ultimoEscopo = lista_escopos;

            while (ultimoEscopo->prox != NULL) {
                ultimoEscopo = ultimoEscopo->prox;
            }

            ultimoEscopo->prox = novoEscopo;
            novoEscopo->ant = ultimoEscopo;
        }
    }

    escopo_atual = novoEscopo;
}

void sairEscopo() {

    if (escopo_atual == NULL) return;
    
    escopo_atual = escopo_atual->prox;
}

void liberarTabSimbolos() {

    if (lista_escopos == NULL) return;

    Escopo* e = lista_escopos;

    while (e) {
        Simbolo* s = e->simbolos;

        while (s) {
            Simbolo* sAux = s;
            s = s->prox;

            if (sAux->parametros != NULL) {
                Parametro* p = sAux->parametros;

                while (p) {
                    Parametro* pAux = p;
                    p = p->prox;

                    free(pAux);
                }
            }
            
            if (sAux->temporario != NULL) {
                free(sAux->temporario);
            }
            
            free(sAux->nome);
            free(sAux);
        }

        Escopo* eAux = e;
        e = e->prox;

        free(eAux);
    }
}

Simbolo* buscarSimboloEscopoAtual(const char* nome) {

    if (!escopo_atual) return NULL;

    Simbolo* s = escopo_atual->simbolos;

    while (s) {
        if (strcmp(s->nome, nome) == 0) {
            return s;
        }

        s = s->prox;
    }

    return NULL;
}

Simbolo* buscarSimboloTodosEscopos(const char* nome, CategoriaSimbolo categoria, bool verificarCategoria) {

    Escopo* e = escopo_atual;

    while (e) {
        Simbolo* s = e->simbolos;

        while (s) {
            if (strcmp(s->nome, nome) == 0) {
                if (verificarCategoria == true && s->categoria != categoria) {
                    s = s->prox;
                    continue;
                }

                return s;
            }

            s = s->prox;
        }

        e = e->prox;
    }

    return NULL;
}

Simbolo* declararSimbolo(const char *nome, CategoriaSimbolo categoria, TipoSimbolo tipo, bool ehVetor, int tamanhoVetor) {

    // Verificações de uso correto da função declararSimbolo(); não verificam semântica
    if (categoria == Funcao && (ehVetor == true || tamanhoVetor != -1)) {
        fprintf(stderr, "Para a declaração de funções, ehVetor tem que ser false e tamanhoVetor tem que ser -1");
        return NULL;
    }

    if (categoria == Variavel && ehVetor == false && tamanhoVetor != -1) {
        // -1 indica que tamanhoVetor não importa
        fprintf(stderr, "Para a declaração de variáveis que não são vetores, tamanho tem que ser -1");
        return NULL;
    }

    Simbolo* s = (Simbolo*) malloc(sizeof(Simbolo));

    s->nome = strdup(nome);
    s->categoria = categoria;
    s->tipo = tipo;

    s->ehVetor = ehVetor;
    s->tamanhoVetor = tamanhoVetor;
    s->parametros = NULL;
    s->quantidadeParametros = -1;
    s->temporario = NULL;
    s->ehVarGlobal = false;
    s->offset = 0;
    s->prox = NULL;

    if (escopo_atual == NULL) {
        fprintf(stderr, "ERRO :: Nenhum escopo ativo para declarar simbolo '%s'", nome);
        free(s->nome);
        free(s);
        return NULL;
    }

    s->prox = escopo_atual->simbolos;
    escopo_atual->simbolos = s;

    return s;
}

const char* categoriaParaString(CategoriaSimbolo categoria) {
    if (categoria == Funcao) {
        return "funcao";
    }

    return "variavel";
}

const char* tipoParaString(TipoSimbolo tipo) {
    if (tipo == TipoVoid) {
        return "void";
    }

    return "int";
}

void imprimirEscopoAtual(FILE* saida, int idEscopo, int nivelEscopo) {

    if (saida == NULL || escopo_atual == NULL) {
        return;
    }

    fprintf(saida, "Escopo #%d (nivel %d)\n", idEscopo, nivelEscopo);

    Simbolo* simbolo = escopo_atual->simbolos;

    if (simbolo == NULL) {
        fprintf(saida, "  (sem simbolos)\n\n");
        return;
    }

    while (simbolo) {
        fprintf(
            saida,
            "  - %s | categoria=%s | tipo=%s | vetor=%s",
            simbolo->nome,
            categoriaParaString(simbolo->categoria),
            tipoParaString(simbolo->tipo),
            simbolo->ehVetor ? "sim" : "nao"
        );

        if (simbolo->ehVetor && simbolo->tamanhoVetor >= 0) {
            fprintf(saida, " | tamanho=%d", simbolo->tamanhoVetor);
        }

        fprintf(saida, "\n");
        simbolo = simbolo->prox;
    }

    fprintf(saida, "\n");
}