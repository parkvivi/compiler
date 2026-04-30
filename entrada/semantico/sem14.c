void main(void) {
    int y;
    y = func();
}

int func(void) {
    return 3;
}

/* esperado: erro semantico; funcao func() nao declarada */