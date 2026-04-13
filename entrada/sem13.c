void func(void) {

}

void main(void) {
    int x;
    x = func();
}

/* esperado: erro semantico; func() eh void, x inteiro  */