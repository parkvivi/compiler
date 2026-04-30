void helloworld(void){

}

int func(void) {
    return helloworld();
}

void main(void) {
    int x;

    x = func();
}

/* esperado: erro semantico; funcao func eh de tipo int mas retorna tipo void */