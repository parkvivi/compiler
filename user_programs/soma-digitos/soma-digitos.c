/* somar todos os digitos de um numero ate que o resultado tenha somente um digito */

int resto(int a, int b) {
    return a - a/b*b;
}

int somadigitos(int n) {
    int num;
    num = n;

    while (num > 9) {
        int soma;
        int aux;

        soma = 0;
        aux = num;

        while (aux != 0) {
            soma = soma +  resto(aux, 10);
            aux = aux / 10;
        }

        num = soma;
    }

    return num;
}

void main(void) {
    int n;

    n = input();
    output(somadigitos(n));
}