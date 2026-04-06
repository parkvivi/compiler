#include <stdio.h>
#include <string.h>
#include "tabsimbolos.h"

extern int linha_atual;

Escopo *topo_escopo = NULL;

void entrarEscopo(int linhaInicio) {

    Escopo *novoEscopo = (Escopo*) malloc(sizeof(Escopo));

    novoEscopo->simbolos = NULL;
    novoEscopo->linhaInicio = linhaInicio;
    novoEscopo->prox = topo_escopo;

    topo_escopo = novoEscopo;
}

void sairEscopo() { // pop
    if (topo_escopo == NULL) return;

    Simbolo *s = topo_escopo->simbolos;
    if (topo_escopo->prox == NULL && strcmp(topo_escopo->simbolos->nome, "main") != 0) {
        // main() não foi declarada como último elemento do escopo global 
        fprintf(stderr, "ERRO SEMANTICO :: funcao \"main\" nao declarada ou declarada incorretamente");
        return;
    }

    if (s) {
        printf("ESCOPO: LINHAS %d a %d\n", topo_escopo->linhaInicio, linha_atual);
    }

    while (s) {
        char *categoria = (s->categoria == Variavel) ? "variavel" : "funcao";
        char *tipoSimbolo = (s->tipoSimbolo == TipoInt) ? "int" : "void";
        char *tipoVariavel = (s->categoria == Variavel) ? (s->tipo == TipoInteiro ? "//inteiro" : "//vetor") : "";
        printf("\t-> %s\t\t[%s//%s%s]\n", s->nome, categoria, tipoSimbolo, tipoVariavel);
        
        Simbolo *tmp = s;
        s = s->prox;
        
        free(tmp->nome);
        
        if (tmp->categoria == Variavel && tmp->tipo == TipoVetor) {
            free(tmp->valor.vetor);
        }
        free(tmp);
    }

    Escopo *tmpE = topo_escopo;
    topo_escopo = topo_escopo->prox;
    free(tmpE);
}

Simbolo* buscarSimboloEscopoAtual(const char *nome, CategoriaSimbolo categoria, TipoVariavel tipoVar, int verificarTipos) {

    if (!topo_escopo) return NULL;

    Simbolo *s = topo_escopo->simbolos;
    while (s) {
        if (strcmp(s->nome, nome) == 0) {
            if (verificarTipos == 1 && s->categoria != categoria && (categoria == Variavel && s->tipo != tipoVar)) {
                s = s->prox;
                continue;
            }
            return s;
        }
        s = s->prox;
    }
    return NULL;
}

Simbolo* buscarSimboloTodosEscopos(const char *nome, CategoriaSimbolo categoria, TipoVariavel tipoVar, int verificarTipos) {

    Escopo *e = topo_escopo;
    while (e) {
        Simbolo *s = e->simbolos;
        while (s) {
            if (strcmp(s->nome, nome) == 0) {
                if (verificarTipos == 1 && s->categoria != categoria && (categoria == Variavel && s->tipo != tipoVar)) {
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

void declararSimbolo(const char *nome, CategoriaSimbolo categoria, TipoSimbolo tipoSimbolo, TipoVariavel tipo, int tamanhoVetor) {

    if (buscarSimboloEscopoAtual(nome, categoria, tipo, 1) != NULL) {
        // Erro de simbolo duplicada no escopo
        fprintf(stderr, "LINHA %d :: ERRO SEMANTICO :: identificador \"%s\" em uso\n", linha_atual, nome);
        return;
    }

    if (strcmp(nome, "output") == 0 || strcmp(nome, "input") == 0) {
        // Erro de tentativa de declaração de função com nome reservado
        fprintf(stderr, "LINHA %d :: ERRO SEMANTICO :: nome \"%s\" reservado para funcao\n", linha_atual, nome);
        return;
    }

    Simbolo *s = (Simbolo*) malloc(sizeof(Simbolo));

    s->nome = strdup(nome);
    s->categoria = categoria;
    s->tipoSimbolo = tipoSimbolo;
    s->prox = topo_escopo->simbolos;
    topo_escopo->simbolos = s;

    if (s->categoria != Variavel) {
        return;
    }
    
    s->tipo = tipo;

    if (tipo == TipoInteiro) {
        s->tamanho = 1;
        s->valor.inteiro = 0; 
    } else if (tipo == TipoVetor) {
        s->tamanho = tamanhoVetor;
        s->valor.vetor = (int*) calloc(tamanhoVetor, sizeof(int));
    }
}