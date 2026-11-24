int programasHD[10];    /* guarda nome dos programas no HD */
int posicaoHD[10];      /* guarda posição inicial dos programas no HD */
int ocupadoHD[10];      /* há um programa ocupando aquela posição do HD (1) ou não (0)*/

int tamanhoKernelSO;    /* guarda tamanho do kernel do SO */
int tamanhoArquivo;     /* guarda tamanho de um programa de usuário (fixo) */

int tamanhoDados;
int tamanhoDadosSO;     /* guarda tamanho do espaço de endereçamento do SO */

int escolherPrograma(void) {
    int i;
    int programa;

    programa = 0;
    programa = input();

    if (programa != 0) {    /* 0 não é um nome permitido */
        i = 0;
        while (i < 10) {
            if (programasHD[i] == programa) {
                return i;   /* retorna índice do programa */
            }
    
            i = i + 1;
        }
    }

    return 11;
}

int executarSemPreempcao(void) {
    int indicePrograma;

    limparLCD();
    escreverLCD(1, 115);    /* s */
    escreverLCD(2, 101);    /* e */
    escreverLCD(3, 109);    /* m */

    escreverLCD(5, 112);    /* p */ 
    escreverLCD(6, 114);    /* r */
    escreverLCD(7, 101);    /* e */
    escreverLCD(8, 101);    /* e */
    escreverLCD(9, 109);    /* m */
    escreverLCD(10, 112);   /* p */
    escreverLCD(11, 99);    /* c */
    escreverLCD(12, 97);    /* a */
    escreverLCD(13, 111);   /* o */

    escreverLCD(16, 91);    /* [ */
    escreverLCD(17, 105);   /* i */
    escreverLCD(18, 110);   /* n */
    escreverLCD(19, 93);    /* ] */

    escreverLCD(21, 112);   /* p */
    escreverLCD(22, 114);   /* r */
    escreverLCD(23, 111);   /* o */
    escreverLCD(24, 103);   /* g */
    escreverLCD(25, 114);   /* r */
    escreverLCD(26, 97);    /* a */
    escreverLCD(27, 109);   /* m */
    escreverLCD(28, 97);    /* a */

    indicePrograma = escolherPrograma();
    if (indicePrograma == 11) {
        return 1;
    }

    escreverLCD(16, 91);    /* [ */
    escreverLCD(17, 101);   /* e */
    escreverLCD(18, 120);   /* x */
    escreverLCD(19, 93);    /* ] */

    carregarPrograma(posicaoHD[indicePrograma]);
    preservarContexto();
    configurarBase(tamanhoDadosSO);
    configurarPCInicial(1800);
    executarPrograma();
    configurarBase(0);
    restaurarContexto();

    return 0;
}

int roundRobin(int programaAtual, int maximoProgramas) {
    int programaEscalonado;

    programaAtual = programaAtual + 1;
    programaEscalonado = programaAtual - programaAtual/maximoProgramas*maximoProgramas;

    return programaEscalonado;
}

int executarComPreempcao(void) {
    int i;
    int flagFim;
    int baseProgramas[3];
    int indicesProgramas[3];
    int programasFinalizados[3];
    int quantidadeProgramasFinalizados;

    limparLCD();
    escreverLCD(1, 112);    /* p */ 
    escreverLCD(2, 114);    /* r */
    escreverLCD(3, 101);    /* e */
    escreverLCD(4, 101);    /* e */
    escreverLCD(5, 109);    /* m */
    escreverLCD(6, 112);    /* p */
    escreverLCD(7, 99);     /* c */
    escreverLCD(8, 97);     /* a */
    escreverLCD(9, 111);    /* o */

    escreverLCD(16, 91);    /* [ */
    escreverLCD(17, 105);   /* i */
    escreverLCD(18, 110);   /* n */
    escreverLCD(19, 93);    /* ] */

    escreverLCD(21, 112);   /* p */
    escreverLCD(22, 114);   /* r */
    escreverLCD(23, 111);   /* o */
    escreverLCD(24, 103);   /* g */
    escreverLCD(25, 114);   /* r */
    escreverLCD(26, 97);    /* a */
    escreverLCD(27, 109);   /* m */
    escreverLCD(28, 97);    /* a */

    i = 0;
    while (i < 3) {
        if (i == 0) {
            escreverLCD(30, 49);    /* 1 */
        } else if (i == 1) {
            escreverLCD(30, 50);    /* 2 */
        } else {
            escreverLCD(30, 51);    /* 3 */
        }
        
        indicesProgramas[i] = escolherPrograma();
        if (indicesProgramas[i] == 11) {
            return 1;
        }

        baseProgramas[i] = tamanhoDadosSO + i * tamanhoDados;
        programasFinalizados[i] = 0;
        i = i + 1;
    }

    i = 0;
    while (i < 3) {
        configurarBase(baseProgramas[i]);
        configurarPCInicial(1800);
        configurarBase(0);
        i = i + 1;
    }

    escreverLCD(16, 91);    /* [ */
    escreverLCD(17, 101);   /* e */
    escreverLCD(18, 120);   /* x */
    escreverLCD(19, 93);    /* ] */

    escreverLCD(29, 32);    /* espaço em branco */

    i = 0;
    quantidadeProgramasFinalizados = 0;
    while (quantidadeProgramasFinalizados < 3) {
        if (programasFinalizados[i] == 0) {
            if (i == 0) {
                escreverLCD(30, 49);    /* 1 */
            } else if (i == 1) {
                escreverLCD(30, 50);    /* 2 */
            } else {
                escreverLCD(30, 51);    /* 3 */
            }

            flagFim = 0;
            
            carregarPrograma(posicaoHD[indicesProgramas[i]]);
            preservarContexto();                                /* preserva contexto do SO */
            configurarBase(baseProgramas[i]);                   /* base -> PROGRAMA DE USUÁRIO */
            restaurarContexto();                                /* restaura contexto do programa de usuário */
            configurarQuantum(50);                              /* configura quantum e habilita interrupções */
            executarPrograma();
            preservarContexto();                                /* preserva contexto do programa de usuário */
            configurarBase(0);                                  /* base -> SISTEMA OPERACIONAL */
            guardarStatusFinalizacao();                         /* guarda R28 (reg de status) na posição 28 do BCP do SO */
            restaurarContexto();                                /* restaura contexto do SO */

            flagFim = lerStatusFinalizacao();                   /* lê R28 e decide se programa de usuário finalizou sua execução */
            if (flagFim == 1) {
                quantidadeProgramasFinalizados = quantidadeProgramasFinalizados + 1;
                programasFinalizados[i] = 1;
            }
        }

        i = roundRobin(i, 3);   /* escalonamento */
    }

    return 0;
}

int renomearPrograma(void) {
    int i;
    int novoNome;
    int indicePrograma;

    limparLCD();
    escreverLCD(1, 114);    /* r */
    escreverLCD(2, 101);    /* e */
    escreverLCD(3, 110);    /* n */
    escreverLCD(4, 111);    /* o */
    escreverLCD(5, 109);    /* m */
    escreverLCD(6, 101);    /* e */
    escreverLCD(7, 97);     /* a */
    escreverLCD(8, 114);    /* r */

    escreverLCD(16, 91);    /* [ */
    escreverLCD(17, 105);   /* i */
    escreverLCD(18, 110);   /* n */
    escreverLCD(19, 93);    /* ] */

    escreverLCD(21, 112);   /* p */
    escreverLCD(22, 114);   /* r */
    escreverLCD(23, 111);   /* o */
    escreverLCD(24, 103);   /* g */
    escreverLCD(25, 114);   /* r */
    escreverLCD(26, 97);    /* a */
    escreverLCD(27, 109);   /* m */
    escreverLCD(28, 97);    /* a */

    indicePrograma = escolherPrograma();
    if (indicePrograma == 11) {
        return 1;
    }

    escreverLCD(21, 110);   /* n */
    escreverLCD(22, 111);   /* o */
    escreverLCD(23, 118);   /* v */    
    escreverLCD(24, 111);   /* o */    

    escreverLCD(25, 32);    /* espaço em branco */

    escreverLCD(26, 110);   /* n */
    escreverLCD(27, 111);   /* o */
    escreverLCD(28, 109);   /* m */    
    escreverLCD(29, 101);   /* e */
    
    novoNome = input();

    if (novoNome == 0) {
        return 2;   /* 0 não é um nome permitido */
    }

    i = 0;
    while (i < 10) {
        if (programasHD[i] == novoNome) {
            return 2;
        }

        i = i + 1;
    }

    programasHD[indicePrograma] = novoNome;

    return 0;
}

int deletarPrograma(void) {
    int indicePrograma;

    limparLCD();
    escreverLCD(1, 100);    /* d */
    escreverLCD(2, 101);    /* e */
    escreverLCD(3, 108);    /* l */
    escreverLCD(4, 101);    /* e */
    escreverLCD(5, 116);    /* t */
    escreverLCD(6, 97);     /* a */
    escreverLCD(7, 114);    /* r */

    escreverLCD(16, 91);    /* [ */
    escreverLCD(17, 105);   /* i */
    escreverLCD(18, 110);   /* n */
    escreverLCD(19, 93);    /* ] */

    escreverLCD(21, 112);   /* p */
    escreverLCD(22, 114);   /* r */
    escreverLCD(23, 111);   /* o */
    escreverLCD(24, 103);   /* g */
    escreverLCD(25, 114);   /* r */
    escreverLCD(26, 97);    /* a */
    escreverLCD(27, 109);   /* m */
    escreverLCD(28, 97);    /* a */

    indicePrograma = escolherPrograma();
    if (indicePrograma == 11) {
        return 1;
    }

    programasHD[indicePrograma] = 0;
    ocupadoHD[indicePrograma] = 0;

    return 0;
}

int criarPrograma(void) {
    int i;
    int nome;
    int indice;
    int endereco;
    int tamanhoPrograma;

    limparLCD();
    escreverLCD(1, 99);     /* c */
    escreverLCD(2, 114);    /* r */
    escreverLCD(3, 105);    /* i */
    escreverLCD(4, 97);     /* a */
    escreverLCD(5, 114);    /* r */

    i = 0;
    indice = 11;
    while (i < 10) {
        if (ocupadoHD[i] == 0) {
            indice = i;
        }

        i = i + 1;
    }

    if (indice == 11) {
        return 3;
    }

    escreverLCD(16, 110);   /* n */
    escreverLCD(17, 111);   /* o */
    escreverLCD(18, 109);   /* m */    
    escreverLCD(19, 101);   /* e */
    escreverLCD(20, 58);    /* : */

    nome = input();

    i = 0;
    while (i < 10) {
        if (programasHD[i] == nome) {
            return 2;
        }

        i = i + 1;
    }

    programasHD[indice] = nome;
    ocupadoHD[indice] = 1;

    escreverLCD(16, 116);   /* t */
    escreverLCD(17, 97);    /* a */
    escreverLCD(18, 109);   /* m */
    escreverLCD(19, 97);    /* a */
    escreverLCD(20, 110);   /* n */
    escreverLCD(21, 104);   /* h */
    escreverLCD(22, 111);   /* o */
    escreverLCD(23, 58);    /* : */
    
    tamanhoPrograma = input();

    if (tamanhoPrograma > tamanhoArquivo) {
        return 4;
    }

    escreverLCD(16, 112);   /* p */
    escreverLCD(17, 114);   /* r */
    escreverLCD(18, 111);   /* o */
    escreverLCD(19, 103);   /* g */
    escreverLCD(20, 114);   /* r */
    escreverLCD(21, 97);    /* a */
    escreverLCD(22, 109);   /* m */
    escreverLCD(23, 97);    /* a */
    escreverLCD(24, 58);    /* : */

    i = 0;
    endereco = posicaoHD[indice];
    while (i < tamanhoPrograma) {
        inputPrograma(endereco);
        endereco = endereco + 1;
        i = i + 1;
    }

    return 0;
}

void shellprompt(void) {
    int opcao;
    int codigoRetorno;

    while (1 == 1) {
        opcao = 0;
        codigoRetorno = 0;

        limparLCD();
        escreverLCD(1, 109);
        escreverLCD(2, 101);
        escreverLCD(3, 110);
        escreverLCD(4, 117);

        escreverLCD(6, 49);     /* 1 */ 
        escreverLCD(7, 126);    /* -> */ 
        escreverLCD(8, 83);     /* S */ 
        escreverLCD(9, 80);     /* P */ 

        escreverLCD(11, 50);    /* 2 */
        escreverLCD(12, 126);   /* -> */  
        escreverLCD(13, 80);    /* P */ 

        escreverLCD(16, 51);    /* 3 */
        escreverLCD(17, 126);   /* -> */
        escreverLCD(18, 82);    /* R */

        escreverLCD(20, 52);    /* 4 */
        escreverLCD(21, 126);   /* -> */
        escreverLCD(22, 68);    /* D */

        escreverLCD(24, 53);    /* 5 */
        escreverLCD(25, 126);   /* -> */
        escreverLCD(26, 67);    /* C */

        opcao = input();

        if (opcao == 1) {
            codigoRetorno = executarSemPreempcao();
        } else if (opcao == 2) {
            codigoRetorno = executarComPreempcao();
        } else if (opcao == 3) {
            codigoRetorno = renomearPrograma();
        } else if (opcao == 4) {
            codigoRetorno = deletarPrograma();
        } else if (opcao == 5) {
            codigoRetorno = criarPrograma();
        } else {
            codigoRetorno = 10;
        }

        if (codigoRetorno != 0) {
            limparLCD();
            escreverLCD(8, 91);     /* [ */
            escreverLCD(9, 101);    /* e */
            escreverLCD(10, 114);   /* r */
            escreverLCD(11, 114);   /* r */
            escreverLCD(12, 58);    /* : */
            escreverLCD(15, 93);    /* ] */
        
            if (codigoRetorno == 1) {               /* programa não encontrado */
                escreverLCD(13, 112);   /* p */
                escreverLCD(14, 103);   /* g */
            } else if (codigoRetorno == 2) {        /* programa com esse nome já existe ou nome não é permitido */
                escreverLCD(13, 110);   /* n */
                escreverLCD(14, 109);   /* m */ 
            } else if (codigoRetorno == 3) {        /* HD cheio */
                escreverLCD(13, 102);   /* f */
                escreverLCD(14, 108);   /* l */
            } else if (codigoRetorno == 4) {        /* tamanho do programa inválido */
                escreverLCD(13, 116);   /* t */
                escreverLCD(14, 109);   /* m */
            } else if (codigoRetorno == 10) {       /* opção escolhida não é válida */
                escreverLCD(13, 111);   /* o */
                escreverLCD(14, 112);   /* p */
            }
            
            output(0);
        }
    }

}

void main(void) {
    int i;

    tamanhoKernelSO = 1800;
    tamanhoArquivo = 200;

    tamanhoDadosSO = 500;
    tamanhoDados = 500;

    i = 0;
    while (i < 10) {
        programasHD[i] = i + 1;
        posicaoHD[i] = tamanhoKernelSO + i * tamanhoArquivo;
        ocupadoHD[i] = 1;
        i = i + 1;
    }

    limparLCD();
    escreverLCD(6, 109);    /* m */
    escreverLCD(7, 97);     /* a */
    escreverLCD(8, 114);    /* r */
    escreverLCD(9, 105);    /* i */
    escreverLCD(10, 79);    /* O */
    escreverLCD(11, 83);    /* S */
    escreverLCD(16, 235);
    escreverLCD(17, 43);
    escreverLCD(18, 58);
    escreverLCD(19, 161);
    escreverLCD(20, 110);
    escreverLCD(21, 40);
    escreverLCD(22, 165);
    escreverLCD(23, 119);
    escreverLCD(24, 165);
    escreverLCD(25, 41);
    escreverLCD(26, 110);
    escreverLCD(27, 235);
    escreverLCD(28, 43);
    escreverLCD(29, 58);
    escreverLCD(30, 161);
    output(0);

    shellprompt();
}