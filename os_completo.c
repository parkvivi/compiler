int programasHD[10];    /* guarda nome dos programas no HD */
int posicaoHD[10];      /* guarda posição inicial dos programas no HD */

int tamanhoKernelSO;    /* guarda tamanho do kernel do SO */
int tamanhoArquivo;     /* guarda tamanho de um programa de usuário (fixo) */

int tamanhoDados;
int tamanhoDadosSO;     /* guarda tamanho do espaço de endereçamento do SO */

int escolherPrograma(void) {
    int i;
    int programa;

    programa = 0;
    programa = input();
    
    i = 0;
    while (i < 10) {
        if (programasHD[i] == programa) {
            return i; /* retorna índice do programa */
        }

        i = i + 1;
    }

    return 11;
}

void executarSemPreempcao(void) {
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

    escreverLCD(16, 112);   /* p */
    escreverLCD(17, 114);   /* r */
    escreverLCD(18, 111);   /* o */
    escreverLCD(19, 103);   /* g */
    escreverLCD(20, 114);   /* r */
    escreverLCD(21, 97);    /* a */
    escreverLCD(22, 109);   /* m */
    escreverLCD(23, 97);    /* a */
    escreverLCD(24, 58);    /* : */

    indicePrograma = escolherPrograma();
    if (indicePrograma == 11) {
        return;
    }

    /* inserir nome do programa em execução aqui */

    carregarPrograma(posicaoHD[indicePrograma]);
    preservarContexto();
    configurarBase(tamanhoDadosSO);
    configurarPCInicial(1800);
    executarPrograma();
    configurarBase(0);
    restaurarContexto();
}

void executarComPreempcao(void) {
    int i;
    int indicesProgramas[3];
    int baseProgramas[3];
    int programasFinalizados[3];
    int flagFim;
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

    escreverLCD(16, 112);   /* p */
    escreverLCD(17, 114);   /* r */
    escreverLCD(18, 111);   /* o */
    escreverLCD(19, 103);   /* g */
    escreverLCD(20, 114);   /* r */
    escreverLCD(21, 97);    /* a */
    escreverLCD(22, 109);   /* m */
    escreverLCD(23, 97);    /* a */
    escreverLCD(24, 115);   /* s */
    escreverLCD(25, 58);    /* : */

    i = 0;
    while (i < 3) {
        indicesProgramas[i] = escolherPrograma();
        if (indicesProgramas[i] == 11) {
            return;
        }

        baseProgramas[i] = tamanhoDadosSO + i * tamanhoDados;
        programasFinalizados[i] = 0;
        i = i + 1;
    }

    /* inicializar PC aqui (alterar base -> escrever 1800 na posição 33 da memória)*/
    i = 0;
    while (i < 3) {
        configurarBase(baseProgramas[i]);
        configurarPCInicial(1800);
        configurarBase(0);
        i = i + 1;
    }

    escreverLCD(24, 32);   /* espaço em branco */

    i = 0;
    quantidadeProgramasFinalizados = 0;
    while (quantidadeProgramasFinalizados < 3) {
        i = i - i/3*3; /* resto, mecanismo round-robin */
        
        if (programasFinalizados[i] == 0) {
            if (i == 0) {
                escreverLCD(25, 49);    /* 1 */
            } else if (i == 1) {
                escreverLCD(25, 50);    /* 2 */
            } else {
                escreverLCD(25, 51);    /* 3 */
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

        i = i + 1;
    }
}

void shellprompt(void) {
    int opcao;

    while (1 == 1) {
        opcao = 0;

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

        opcao = input();

        if (opcao == 1) {
            executarSemPreempcao();
        } else if (opcao == 2) {
            executarComPreempcao();
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