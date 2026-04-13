void mouse(void){
    return 3;
}

void main(void){
    mouse();
}

/* esperado: erro semantico; funcao mouse() eh void mas retorna inteiro */