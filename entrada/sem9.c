int func(void) {
    return 3;
}

void main(void) {
    int y;
    y = func();
}

int teste(void) {
    return 3;
}

/* esperado: erro semantico; funcao func() declarada depois de main() */