#ifndef EXECUTAVEL_H
#define EXECUTAVEL_H

#include "assembly.h"

typedef struct EnderecoLabel {
    char* label;
    int endereco;
    struct EnderecoLabel* prox;
} EnderecoLabel;

extern EnderecoLabel* lista_labels;
extern EnderecoLabel* ultimo_label;

#endif