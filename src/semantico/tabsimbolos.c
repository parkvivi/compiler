#include <stdio.h>
#include <string.h>
#include "tabsimbolos.h"

extern int linha_atual;

Escopo* topo_escopo = NULL;

static const char* categoriaParaString(CategoriaSimbolo categoria) {
    if (categoria == Funcao) {
        return "funcao";
    }

    return "variavel";
}

static const char* tipoParaString(TipoSimbolo tipo) {
    if (tipo == TipoVoid) {
        return "void";
    }

    return "int";
}

void entrarEscopo() {

    Escopo* novoEscopo = (Escopo*) malloc(sizeof(Escopo));

    novoEscopo->simbolos = NULL;
    novoEscopo->prox = topo_escopo;

    topo_escopo = novoEscopo;
}

void sairEscopo() {

    if (topo_escopo == NULL) return;
    
    Simbolo* s = topo_escopo->simbolos;

    if (topo_escopo->prox == NULL && s != NULL && strcmp(s->nome, "main") != 0) {
        fprintf(stderr, "ERRO SEMANTICO :: funcao \"main\" nao declarada ou declarada incorretamente");
        return;
    }

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

Simbolo* buscarSimboloEscopoAtual(const char* nome) {

    if (!topo_escopo) return NULL;

    Simbolo* s = topo_escopo->simbolos;

    while (s) {
        if (strcmp(s->nome, nome) == 0) {
            return s;
        }

        s = s->prox;
    }

    return NULL;
}

Simbolo* buscarSimboloTodosEscopos(const char* nome, CategoriaSimbolo categoria, bool verificarCategoria) {

    Escopo* e = topo_escopo;

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

void declararSimbolo(const char *nome, CategoriaSimbolo categoria, TipoSimbolo tipo, bool ehVetor, int tamanhoVetor) {

    // Verificações de uso correto da função declararSimbolo(); não verificam semântica
    if (categoria == Funcao && (ehVetor == true || tamanhoVetor != 0)) {
        fprintf(stderr, "Para a declaração de funções, ehVetor tem que ser false e tamanhoVetor tem que ser 0");
    }

    if (categoria == Variavel && ehVetor == false && tamanhoVetor != -1) {
        // -1 indica que tamanhoVetor não importa
        fprintf(stderr, "Para a declaração de variáveis que não são vetores, tamanho tem que ser -1");
    }

    Simbolo* s = (Simbolo*) malloc(sizeof(Simbolo));

    s->nome = strdup(nome);
    s->categoria = categoria;
    s->tipo = tipo;

    s->ehVetor = ehVetor;
    s->tamanhoVetor = tamanhoVetor;
    s->prox = NULL;

    if (topo_escopo == NULL) {
        fprintf(stderr, "ERRO :: Nenhum escopo ativo para declarar simbolo '%s'", nome);
        free(s->nome);
        free(s);
        return;
    }

    s->prox = topo_escopo->simbolos;
    topo_escopo->simbolos = s;
}

void imprimirEscopoAtual(FILE* saida, int idEscopo, int nivelEscopo) {

    if (saida == NULL || topo_escopo == NULL) {
        return;
    }

    fprintf(saida, "Escopo #%d (nivel %d)\n", idEscopo, nivelEscopo);

    Simbolo* simbolo = topo_escopo->simbolos;

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