#ifndef EXECUTAVEL_H
#define EXECUTAVEL_H

#include "assembly.h"

#define TAM_IMEDIATO_TIPO_D 10
#define TAM_IMEDIATO_TIPO_B 24
#define TAM_IMEDIATO_TIPO_M 13

typedef struct EnderecoLabel {
    char* label;
    int endereco;
    struct EnderecoLabel* prox;
} EnderecoLabel;

extern EnderecoLabel* lista_labels;
extern EnderecoLabel* ultimo_label;

void gerarExecutavel();

#endif