/* media aritmetica */

int somaValores(int n) {
    int i;
    int soma;
    
    soma = 0;
    i = 0;
    while (i < n) {
        soma = soma + input();
        i = i + 1;
    }

    return soma;
}

void main(void) {
    int n;
    int soma;

    n = input();
    soma = somaValores(n);
    
    output(soma/n);
}