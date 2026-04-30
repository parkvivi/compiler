int mouse(void){
    return;
}

void main(void){
    int x;
    x = mouse();
}

/* esperado: erro semantico; funcao mouse() eh int mas nao retorna nada */