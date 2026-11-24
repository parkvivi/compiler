void testePartida(int testeMemoria) {
    int teste;

    /* teste do LCD */
    limparLCD();
    escreverLCD(1, 98);     /* b */
    escreverLCD(2, 105);    /* i */
    escreverLCD(3, 111);    /* o */
    escreverLCD(4, 115);    /* s */

    escreverLCD(6, 114);    /* r */
    escreverLCD(7, 117);    /* u */
    escreverLCD(8, 110);    /* n */
    escreverLCD(9, 110);    /* n */
    escreverLCD(10, 105);   /* i */
    escreverLCD(11, 110);   /* n */
    escreverLCD(12, 103);   /* g */

    escreverLCD(16, 91);    /* [ */
    escreverLCD(17, 108);   /* l */
    escreverLCD(18, 99);    /* c */
    escreverLCD(19, 100);   /* d */
    escreverLCD(20, 93);    /* ] */
    output(0);

    /* teste de output */
    escreverLCD(16, 91);    /* [ */
    escreverLCD(17, 111);   /* o */
    escreverLCD(18, 117);   /* u */
    escreverLCD(19, 116);   /* t */
    escreverLCD(20, 93);    /* ] */
    output(10);

    /* teste de input */
    escreverLCD(16, 91);    /* [ */
    escreverLCD(17, 105);   /* i */
    escreverLCD(18, 110);   /* n */
    escreverLCD(19, 93);    /* ] */
    escreverLCD(20, 32);
    teste = input();
    output(teste);

    /* teste da ULA */
    escreverLCD(16, 91);    /* [ */
    escreverLCD(17, 117);   /* u */
    escreverLCD(18, 108);   /* l */
    escreverLCD(19, 97);    /* a */
    escreverLCD(20, 93);    /* ] */
    teste = 10 * 10;
    output(teste);

    /* teste da memória */
    escreverLCD(16, 91);    /* [ */
    escreverLCD(17, 109);   /* m */
    escreverLCD(18, 101);   /* e */
    escreverLCD(19, 109);   /* m */
    escreverLCD(20, 93);    /* ] */
    teste = testeMemoria;
    output(teste);
}

void main(void) {
    testePartida(6);
    carregarSO(0);
    executarSO();  
}