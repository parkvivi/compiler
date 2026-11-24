int SUCESSO;
int ERRO;
int errLCD;
int errOUT;
int errIN;
int errULA;
int errMEM;

int testeLCD(void) {
    int var;

    escreverLCD(12, 76);    /* L */
    escreverLCD(13, 67);    /* C */
    escreverLCD(14, 68);    /* D */

    escreverLCD(16, 126);   /* -> */
    escreverLCD(17, 105);   /* i */
    escreverLCD(18, 110);   /* n */
    escreverLCD(19, 112);   /* p */
    escreverLCD(20, 117);   /* u */   
    escreverLCD(21, 116);   /* t */

    escreverLCD(23, 49);   /* 1 */
    escreverLCD(24, 48);   /* 0 */

    var = input();
    if (var != 10) {
        return errLCD;
    }

    return SUCESSO;
}

int testeOUT(void) {
    int ack;

    escreverLCD(12, 79);    /* O */
    escreverLCD(13, 85);    /* U */
    escreverLCD(14, 84);    /* T */

    escreverLCD(16, 126);   /* -> */
    escreverLCD(17, 111);   /* o */
    escreverLCD(18, 117);   /* u */
    escreverLCD(19, 116);   /* t */
    escreverLCD(20, 112);   /* p */
    escreverLCD(21, 117);   /* u */   
    escreverLCD(22, 116);   /* t */
    escreverLCD(23, 32);
    escreverLCD(24, 49);    /* 1 */
    escreverLCD(25, 48);    /* 0 */
    
    escreverLCD(27, 32);
    escreverLCD(28, 32);
    escreverLCD(29, 32);

    output(10);

    escreverLCD(16, 126);   /* -> */
    escreverLCD(17, 99);    /* c */
    escreverLCD(18, 111);   /* o */
    escreverLCD(19, 110);   /* n */
    escreverLCD(20, 102);   /* f */
    escreverLCD(21, 105);   /* i */   
    escreverLCD(22, 114);   /* r */
    escreverLCD(23, 109);   /* m */
    escreverLCD(24, 97);    /* a */
    escreverLCD(25, 63);    /* ? */

    escreverLCD(27, 40);    /* ( */
    escreverLCD(28, 49);    /* 1 */
    escreverLCD(29, 41);    /* ) */

    ack = input();
    if (ack != 1) {
        return errOUT;
    }

    return SUCESSO;
}

int testeIN(void) {
    int var;
    int ack;

    escreverLCD(12, 73);    /* I */
    escreverLCD(13, 78);    /* N */
    escreverLCD(14, 32);

    escreverLCD(16, 126);   /* -> */
    escreverLCD(17, 105);   /* i */
    escreverLCD(18, 110);   /* n */
    escreverLCD(19, 112);   /* p */
    escreverLCD(20, 117);   /* u */   
    escreverLCD(21, 116);   /* t */
    escreverLCD(22, 58);    /* : */
    escreverLCD(23, 32);
    escreverLCD(24, 32);
    escreverLCD(25, 32);

    escreverLCD(27, 32);
    escreverLCD(28, 32);
    escreverLCD(29, 32);

    var = input();

    escreverLCD(16, 126);   /* -> */
    escreverLCD(17, 111);   /* o */
    escreverLCD(18, 117);   /* u */
    escreverLCD(19, 116);   /* t */
    escreverLCD(20, 112);   /* p */
    escreverLCD(21, 117);   /* u */   
    escreverLCD(22, 116);   /* t */
    escreverLCD(23, 58);    /* : */

    output(var);

    escreverLCD(16, 126);   /* -> */
    escreverLCD(17, 99);    /* c */
    escreverLCD(18, 111);   /* o */
    escreverLCD(19, 110);   /* n */
    escreverLCD(20, 102);   /* f */
    escreverLCD(21, 105);   /* i */   
    escreverLCD(22, 114);   /* r */
    escreverLCD(23, 109);   /* m */
    escreverLCD(24, 97);    /* a */
    escreverLCD(25, 63);    /* ? */

    escreverLCD(27, 40);    /* ( */
    escreverLCD(28, 49);    /* 1 */
    escreverLCD(29, 41);    /* ) */

    ack = input();
    if (ack != 1) {
        return errIN;
    }

    return SUCESSO;
}

int testeULA(void) {
    int var;
    int ack;

    escreverLCD(12, 85);    /* U */
    escreverLCD(13, 76);    /* L */
    escreverLCD(14, 65);    /* A */

    escreverLCD(16, 126);   /* -> */
    escreverLCD(17, 111);   /* o */
    escreverLCD(18, 117);   /* u */
    escreverLCD(19, 116);   /* t */
    escreverLCD(20, 112);   /* p */
    escreverLCD(21, 117);   /* u */   
    escreverLCD(22, 116);   /* t */
    escreverLCD(23, 32);
    escreverLCD(24, 49);    /* 1 */
    escreverLCD(25, 48);    /* 0 */
    escreverLCD(26, 48);    /* 0 */
    escreverLCD(27, 32);
    escreverLCD(28, 32);
    escreverLCD(29, 32);

    var = 10 * 10;
    output(var);

    escreverLCD(16, 126);   /* -> */
    escreverLCD(17, 99);    /* c */
    escreverLCD(18, 111);   /* o */
    escreverLCD(19, 110);   /* n */
    escreverLCD(20, 102);   /* f */
    escreverLCD(21, 105);   /* i */   
    escreverLCD(22, 114);   /* r */
    escreverLCD(23, 109);   /* m */
    escreverLCD(24, 97);    /* a */
    escreverLCD(25, 63);    /* ? */
    escreverLCD(26, 32);
    escreverLCD(27, 40);    /* ( */
    escreverLCD(28, 49);    /* 1 */
    escreverLCD(29, 41);    /* ) */

    ack = input();
    if (ack != 1) {
        return errULA;
    }

    return SUCESSO;
}

int testeMEM(int var) {
    int ack;
    int aux;

    escreverLCD(12, 77);    /* M */
    escreverLCD(13, 69);    /* E */
    escreverLCD(14, 77);    /* M */

    aux = var;

    escreverLCD(16, 126);   /* -> */
    escreverLCD(17, 111);   /* o */
    escreverLCD(18, 117);   /* u */
    escreverLCD(19, 116);   /* t */
    escreverLCD(20, 112);   /* p */
    escreverLCD(21, 117);   /* u */   
    escreverLCD(22, 116);   /* t */
    escreverLCD(23, 32);
    escreverLCD(24, 49);    /* 1 */
    escreverLCD(25, 48);    /* 0 */
    
    escreverLCD(27, 32);
    escreverLCD(28, 32);
    escreverLCD(29, 32);

    output(var);    /* leitura */

    escreverLCD(16, 126);   /* -> */
    escreverLCD(17, 99);    /* c */
    escreverLCD(18, 111);   /* o */
    escreverLCD(19, 110);   /* n */
    escreverLCD(20, 102);   /* f */
    escreverLCD(21, 105);   /* i */   
    escreverLCD(22, 114);   /* r */
    escreverLCD(23, 109);   /* m */
    escreverLCD(24, 97);    /* a */
    escreverLCD(25, 63);    /* ? */
    
    escreverLCD(27, 40);    /* ( */
    escreverLCD(28, 49);    /* 1 */
    escreverLCD(29, 41);    /* ) */

    ack = input();
    if (ack != 1) {
        return errMEM;
    }

    escreverLCD(16, 126);   /* -> */
    escreverLCD(17, 111);   /* o */
    escreverLCD(18, 117);   /* u */
    escreverLCD(19, 116);   /* t */
    escreverLCD(20, 112);   /* p */
    escreverLCD(21, 117);   /* u */   
    escreverLCD(22, 116);   /* t */
    escreverLCD(23, 32);
    escreverLCD(24, 49);    /* 1 */
    escreverLCD(25, 48);    /* 0 */
    
    escreverLCD(27, 32);
    escreverLCD(28, 32);
    escreverLCD(29, 32);

    output(aux);    /* escrita */

    escreverLCD(16, 126);   /* -> */
    escreverLCD(17, 99);    /* c */
    escreverLCD(18, 111);   /* o */
    escreverLCD(19, 110);   /* n */
    escreverLCD(20, 102);   /* f */
    escreverLCD(21, 105);   /* i */   
    escreverLCD(22, 114);   /* r */
    escreverLCD(23, 109);   /* m */
    escreverLCD(24, 97);    /* a */
    escreverLCD(25, 63);    /* ? */
    
    escreverLCD(27, 40);    /* ( */
    escreverLCD(28, 49);    /* 1 */
    escreverLCD(29, 41);    /* ) */

    ack = input();
    if (ack != 1) {
        return errMEM;
    }

    return SUCESSO;
}

void imprimirErro(int erro) {
    limparLCD();

    escreverLCD(0, 101);    /* e */
    escreverLCD(1, 114);    /* r */
    escreverLCD(2, 114);    /* r */
    escreverLCD(3, 111);    /* o */
    escreverLCD(4, 58);     /* : */

    if (erro == errLCD) {
        escreverLCD(6, 76);     /* L */
        escreverLCD(7, 67);     /* C */
        escreverLCD(8, 68);     /* D */
    } else if (erro == errOUT) {
        escreverLCD(6, 79);     /* O */
        escreverLCD(7, 85);     /* U */
        escreverLCD(8, 84);     /* T */
    } else if (erro == errIN) {
        escreverLCD(6, 73);     /* I */
        escreverLCD(7, 78);     /* N */
    } else if (erro == errULA) {
        escreverLCD(6, 85);     /* U */
        escreverLCD(7, 76);     /* L */
        escreverLCD(8, 65);     /* A */
    } else if (erro = errMEM) {
        escreverLCD(6, 77);     /* M */
        escreverLCD(7, 69);     /* E */
        escreverLCD(8, 77);     /* M */
    }
}

int testePartida(void) {
    int codigoRetorno;
    int erro;

    escreverLCD(6, 116);    /* t */
    escreverLCD(7, 101);    /* e */
    escreverLCD(8, 115);    /* s */
    escreverLCD(9, 116);    /* t */
    escreverLCD(10, 101);   /* e */

    erro = 0;

    codigoRetorno = testeLCD();
    if (codigoRetorno != SUCESSO) {
        imprimirErro(codigoRetorno);
        erro = 1;
        return ERRO;
    }
    
    if (erro == 0) {
        codigoRetorno = testeOUT();
        if (codigoRetorno != SUCESSO) {
            imprimirErro(codigoRetorno);
            erro = 1;
            return ERRO;
        }
    }
    
    if (erro == 0) {
        codigoRetorno = testeIN();
        if (codigoRetorno != SUCESSO) {
            imprimirErro(codigoRetorno);
            erro = 1;
            return ERRO;
        }
    }

    if (erro == 0) {
        codigoRetorno = testeULA();
        if (codigoRetorno != SUCESSO) {
            imprimirErro(codigoRetorno);
            erro = 1;
            return ERRO;
        }
    }

    if (erro == 0) {
        codigoRetorno = testeMEM(10);
        if (codigoRetorno != SUCESSO) {
            imprimirErro(codigoRetorno);
            erro = 1;
            return ERRO;
        }
    }

    return SUCESSO;
}

void main(void) {
    int codigoRetorno;

    SUCESSO = 0;
    ERRO = 1;
    errLCD = 2;
    errIN = 3;
    errOUT = 4;
    errULA = 5;
    errMEM = 6;

    escreverLCD(0, 66);     /* B */
    escreverLCD(1, 73);     /* I */
    escreverLCD(2, 79);     /* O */
    escreverLCD(3, 83);     /* S */
    escreverLCD(4, 58);     /* : */

    codigoRetorno = testePartida();

    if (codigoRetorno == SUCESSO) {
        carregarSO(0);
        executarSO();        
    }
}