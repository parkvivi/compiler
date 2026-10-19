int programasHD[10];
int posicaoHD[10];

int tamanhoArquivo;

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

    indicePrograma = escolherPrograma();
    if (indicePrograma == 11) {
        return;
    }

    /*
        // carregar programa
        // carregarPrograma(posicaoHD[indicePrograma]);

        // preservar R0-R31 no PCB do SO
        // preservarContexto();

        // altera valor do registrador base pro programa de usuário
        // BUP
        // executarPrograma(valor da base + 34);

        // restaurar R0-R31
    */
}

void shellprompt(void) {
    int opcao;

    while (1 == 1) {
        opcao = 0;
        opcao = input();

        if (opcao == 1) {
            executarSemPreempcao();
        }
    }

}

void main(void) {
    int i;

    tamanhoArquivo = 200;

    i = 0;
    while (i < 10) {
        programasHD[i] = i + 1;
        posicaoHD[i] = i * tamanhoArquivo;
    }

    shellprompt();
}