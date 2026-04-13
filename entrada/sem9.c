int func(void) {
    return 3;
}

void main(void) {
    int y;
    y = func();
}

int teste(void) {
}

/* esperado: erro semantico; funcao func() declarada depois de main() */