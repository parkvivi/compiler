int func(int h[]) {
    return h;
}

void main(void) {
    int x;
    int j[2];

    x = func(j);
}

/* esperado: erro semantico; retorno de vetor*/