int func(int h[]) {
    return h[0];
}

void main(void) {
    int x;
    int j[2];

    x = func(j);
}

/* esperado: passar; h eh vetor, mas h[0] eh valor inteiro */