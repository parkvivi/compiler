int func(void) {
    int x;
    x = 0;
    output(x);
}

void main(void) {
    int x;

    x = func();
}

/* esperado: erro semantico; funcao func nao retorna nenhum inteiro, apesar de ser do tipo int */