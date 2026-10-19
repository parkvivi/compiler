/* inverte vetor */

void main(void) {
    int x[10];
    int i;
    int aux;

    i = 0;
    while (i < 10) {
        x[i] = input();
        i = i + 1;
    }

    i = 0;
    while (i < 5) {
        aux = x[i];
        x[i] = x[10-i-1];
        x[10-i-1] = aux;
        i = i + 1;
    }

    i = 0;
    while (i < 10) {
        output(x[i]);
        i = i + 1;
    }
}