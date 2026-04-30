void func(int a, int b) {
    int x;
    x = a + b;
    output(x);
}

void main(void) {
    int m;
    int n;
    int p;

    m = 3;
    n = 2;
    p = 1;

    func(m, n, p);
}

/* esperado: erro semantico; func() chamada com argumentos a mais */