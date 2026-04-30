int func(void f) {
    int x;
    x = f + 4;
    return x;
}

void main(void) {
    output(func(3));
}

/* esperado: erro semantico; parametro f do tipo void */