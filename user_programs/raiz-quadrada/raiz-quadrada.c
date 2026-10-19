/* raiz quadrada, retorna inteiro mais proximo pelo limite inferior */

int raiz(int n) {
    int anterior;
    int i;

    anterior = 0;
    i = 0;

    while (n > i*i) {
        anterior = i;
        i = i + 1;
    }

    if (n == i*i) {
        return i;
    }

    return anterior;
}

void main(void) {
    int n;

    n = input();
    output(raiz(n));
}