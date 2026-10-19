/* potência, x^n */

int potencia(int x, int n) {
    int i;
    int potencia;

    potencia = 1;
    i = 0;

    while (i < n) {
        potencia = potencia * x;
        i = i + 1;
    }

    return potencia;
}

void main(void) {
    int x;
    int n;

    x = input();
    n = input();

    output(potencia(x,n));
}