int func(int h[]) {
    return 6;
}

void main(void) {
    int x;
    int j;

    x = func(j);
}

/* esperado: erro semantico; funcao func chamada com inteiro, espera vetor*/