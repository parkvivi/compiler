int func(int h) {
    return h;
}

void main(void) {
    int x;
    int j[2];

    x = func(j[0]);
}

/* esperado: passar; j eh vetor, mas j[0] eh valor inteiro */