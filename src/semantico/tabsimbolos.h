#ifndef TABSIMBOLOS_H
#define TABSIMBOLOS_H

#include <stdlib.h>
#include <string.h>

typedef enum { Variavel, Funcao } CategoriaSimbolo;     // Categoria do simbolo -> variavel ou funcao
typedef enum { TipoVoid, TipoInt } TipoSimbolo;         // Tipo do simbolo -> void ou int
typedef enum { TipoInteiro, TipoVetor } TipoVariavel;   // Tipo da variavel -> inteiro ou vetor

typedef struct Simbolo {
    char *nome;
    CategoriaSimbolo categoria;
    struct Simbolo *prox;
    TipoSimbolo tipoSimbolo;
    TipoVariavel tipo;
    int tamanho;
    union {
        int inteiro;
        int *vetor;
    } valor;
} Simbolo;

typedef struct Escopo {
    Simbolo *simbolos;
    int linhaInicio;
    struct Escopo *prox;
} Escopo;

extern Escopo *topo_escopo;

// Prototipos das funcoes para tratar da tabela de simbolos
void entrarEscopo(int linhaInicio);
void sairEscopo();
Simbolo* buscarSimboloEscopoAtual(const char *nome, CategoriaSimbolo categoria, TipoVariavel tipo, int verificarTipos);
Simbolo* buscarSimboloTodosEscopos(const char *nome, CategoriaSimbolo categoria, TipoVariavel tipo, int verificarTipos);
void declararSimbolo(const char *nome, CategoriaSimbolo categoria, TipoSimbolo tipoSimbolo, TipoVariavel tipo, int tamanhoVetor);
void atribuirValorAVariavel(const char *nome, int valorAtribuido, int indiceVetor);
int buscarValorDeVariavel(const char *nome, int indiceVetor);

#endif