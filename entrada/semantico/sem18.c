void func(int a, int b) {
    int x;
    x = a + b;
    output(x);
}

void main(void) {
    int m;

    m = 3;

    func(m);
}

/* esperado: erro semantico; func() chamada com argumentos insuficientes */