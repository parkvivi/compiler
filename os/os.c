int programasHD[10];
int posicaoHD[10];

int tamanhoSO;
int tamanhoArquivo;

int escolherPrograma(void) {
    int i;
    int programa;

    programa = 0;
    programa = input();
    
    i = 0;
    while (i < 10) {
        if (programasHD[i] == programa) {
            output(programa); /* printa nome do programa */
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

    carregarPrograma(posicaoHD[indicePrograma]);
    preservarContexto();
    configurarBase(200); /* offset corresponde ao espaço de endereçamento do SO */
    executarPrograma();
    configurarBase(0);
    restaurarContexto();
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

    tamanhoSO = 800;
    tamanhoArquivo = 200;

    i = 0;
    while (i < 10) {
        programasHD[i] = i + 1;
        output(programasHD[i]);
        posicaoHD[i] = tamanhoSO + i * tamanhoArquivo;
        output(posicaoHD[i]);
        i = i + 1;
    }

    shellprompt();
}