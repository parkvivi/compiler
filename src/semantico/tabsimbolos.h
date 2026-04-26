#ifndef TABSIMBOLOS_H
#define TABSIMBOLOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum { Variavel, Funcao } CategoriaSimbolo;
typedef enum { TipoVoid, TipoInt } TipoSimbolo;

typedef struct Parametro {
    TipoSimbolo tipoParam;
    bool paramVetor;
    
    struct Parametro* prox;
} Parametro;

typedef struct Simbolo {
    char* nome;

    CategoriaSimbolo categoria;
    TipoSimbolo tipo;

    bool ehVetor;
    int tamanhoVetor;

    Parametro* parametros;

    char* temporario;

    struct Simbolo* prox;
} Simbolo;

typedef struct Escopo {
    Simbolo* simbolos;
    struct Escopo* ant;
    struct Escopo* prox;
} Escopo;

extern Escopo* escopo_atual;
extern Escopo* lista_escopos;

void entrarEscopo();
void sairEscopo();
void liberarTabSimbolos();
Simbolo* buscarSimboloEscopoAtual(const char* nome);
Simbolo* buscarSimboloTodosEscopos(const char* nome, CategoriaSimbolo categoria, bool verificarCategoria);
Simbolo* declararSimbolo(const char *nome, CategoriaSimbolo categoria, TipoSimbolo tipo, bool ehVetor, int tamanhoVetor);
const char* categoriaParaString(CategoriaSimbolo categoria);
const char* tipoParaString(TipoSimbolo tipo);
void imprimirEscopoAtual(FILE* saida, int idEscopo, int nivelEscopo);
void erroSemantico(const char*);

#endif