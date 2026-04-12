#include <stdio.h>
#include <string.h>
#include "tabsimbolos.h"

extern int linha_atual;

Escopo* topo_escopo = NULL;

void entrarEscopo() {

    Escopo* novoEscopo = (Escopo*) malloc(sizeof(Escopo));

    novoEscopo->simbolos = NULL;
    novoEscopo->prox = topo_escopo;

    topo_escopo = novoEscopo;
}

void sairEscopo() {

    if (topo_escopo == NULL) return;

    Simbolo* s = topo_escopo->simbolos;

    while (s) {
        Simbolo* sAux = s;
        s = s->prox;
        
        free(sAux->nome);
        free(sAux);
    }

    Escopo* eAux = topo_escopo;
    topo_escopo = topo_escopo->prox;

    free(eAux);
}

Simbolo* buscarSimboloEscopoAtual(const char* nome, CategoriaSimbolo categoria, TipoSimbolo tipo, bool ehVetor) {

    if (!topo_escopo) return NULL;

    Simbolo* s = topo_escopo->simbolos;

    while (s) {
        if (strcmp(s->nome, nome) == 0 && s->categoria == categoria && s->tipo == tipo && s->ehVetor == ehVetor) {
            return s;
        }

        s = s->prox;
    }

    return NULL;
}

Simbolo* buscarSimboloTodosEscopos(const char* nome, CategoriaSimbolo categoria, TipoSimbolo tipo, bool ehVetor) {

    Escopo* e = topo_escopo;

    while (e) {
        Simbolo* s = e->simbolos;

        while (s) {
            if (strcmp(s->nome, nome) == 0 && s->categoria == categoria && s->tipo == tipo && s->ehVetor == ehVetor) {
                return s;
            }

            s = s->prox;
        }

        e = e->prox;
    }

    return NULL;
}

void declararSimbolo(const char *nome, CategoriaSimbolo categoria, TipoSimbolo tipo, bool ehVetor, int tamanhoVetor) {

    // Verificações de uso correto da função declararSimbolo(); não verificam semântica
    if (categoria == Funcao && (ehVetor == true || tamanhoVetor != 0)) {
        fprintf(stderr, "Para a declaracao de funcoes, ehVetor tem que ser false e tamanhoVetor tem que ser 0");
    }

    if (categoria == Variavel && ehVetor == false && tamanhoVetor != 1) {
        fprintf(stderr, "Para a declaracao de variaveis que não são vetores, tamanho tem que ser 1");
    }

    Simbolo* s = (Simbolo*) malloc(sizeof(Simbolo));

    s->nome = strdup(nome);
    s->categoria = categoria;
    s->tipo = tipo;

    s->ehVetor = ehVetor;
    s->tamanhoVetor = tamanhoVetor;
}