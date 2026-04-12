#ifndef TABSIMBOLOS_H
#define TABSIMBOLOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum { Variavel, Funcao } CategoriaSimbolo;     // Categoria do simbolo -> variavel ou funcao
typedef enum { TipoVoid, TipoInt } TipoSimbolo;         // Tipo do simbolo -> void ou int

typedef struct Simbolo {
    char* nome;

    CategoriaSimbolo categoria;
    TipoSimbolo tipo;

    bool ehVetor;
    int tamanhoVetor;

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
void declararSimbolo(const char *nome, CategoriaSimbolo categoria, TipoSimbolo tipo, bool ehVetor, int tamanhoVetor);
void imprimirEscopoAtual(FILE* saida, int idEscopo, int nivelEscopo);

#endif