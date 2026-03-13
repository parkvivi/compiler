%{
    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>

    extern int yylex();
    extern int yyparse();
    extern FILE* yyin;

    extern int linha_atual;

    void yyerror(const char* s);


    /* ========== ESTRUTURAS E PROTOTIPOS DA ARVORE ========== */

    // Tipos de nos da AST
    typedef enum {
        TipoPrograma,
        TipoListaDeclaracoes,
        TipoDeclaracao,
        TipoDeclaracaoVars,
        TipoEspecifico,
        TipoDeclaracaoFunc,
        TipoParametros,
        TipoListaParametros,
        TipoParametro,
        TipoEscopo,
        TipoDeclaracaoLocais,
        TipoListaEscopo,
        TipoCorpo,
        TipoDeclaracaoExpressao,
        TipoDeclaracaoSelecao,
        TipoDeclaracaoIteracao,
        TipoDeclaracaoRetorno,
        TipoExpressao,
        TipoVar,
        TipoExpressaoSimples,
        TipoRelacional,
        TipoExpressaoSoma,
        TipoSoma,
        TipoTermo,
        TipoMult,
        TipoFator,
        TipoChamadaFuncao,
        TipoArgumentos,
        TipoListaArgumentos,
        TipoID,
        TipoNum
    } TipoNo;

    #define MAXFILHOS 4

    // Estrutura de um no da AST
    typedef struct AST {
        TipoNo tipo;
        union {
            char* nome; // nome de funções ou variáveis
            int valor;
        } dado;
        struct AST* filhos[MAXFILHOS];
    } AST;

    AST *raizAST = NULL;
    char* origem = NULL;

    // Prototipos das funcoes para tratar da AST
    AST* criarNo(TipoNo tipo);
    void liberaAST(AST* no);

    // Prototipos das funcoes para gerar arquivo .dot
    void gerarDOT(AST* raiz);
    const char* nomeTipoNo(AST* no);
    void imprimirNoDOT(AST* no, FILE* file);


    /* ========== ESTRUTURAS E PROTOTIPOS DAS QUADRUPLAS ========== */

    // Estrutura de uma quadrupla
    typedef struct {
        char *operacao;
        char *argumento1;
        char *argumento2;
        char *resultado;
    } Quadrupla;

    Quadrupla *quads = NULL;
    int quads_cont = 0;
    char *funcao_atual = NULL;
    int temp_cont = 0;
    int label_cont = 0;

    // Prototipos das funcoes para tratar das quadruplas
    char* novoTemp();
    char* novoLabel();
    void emitir_quads(const char *op, const char *a1, const char *a2, const char *res);
    void limpa_quads();
    char* gerar_quads(AST* no);
    void print_quads(FILE *out);

    /* ========== ESTRUTURAS E PROTOTIPOS DA TABELA DE SIMBOLOS COM ESCOPO ========== */

    // Categoria do simbolo -> variavel ou funcao
    typedef enum { Variavel, Funcao } CategoriaSimbolo;
    // Tipo do simbolo -> void ou int
    typedef enum { TipoVoid, TipoInt } TipoSimbolo;
    // Tipo da variavel -> inteiro ou vetor
    typedef enum { TipoInteiro, TipoVetor } TipoVariavel;

    // No da tabela de simbolos
    typedef struct Simbolo {
        char *nome;
        CategoriaSimbolo categoria;
        struct Simbolo *prox;
        TipoSimbolo tipoSimbolo;

        // categoria eh Variavel
        TipoVariavel tipo;
        int tamanho;
        union {
            int inteiro;
            int *vetor;
        } valor;
    } Simbolo;

    typedef struct Escopo {
        Simbolo *simbolos;
        int linhaInicio;
        struct Escopo *prox;
    } Escopo;

    Escopo *topo_escopo = NULL;

    // Prototipos das funcoes para tratar da tabela de simbolos
    void entrarEscopo(int linhaInicio);
    void sairEscopo();
    Simbolo* buscarSimboloEscopoAtual(const char *nome, CategoriaSimbolo categoria, int verificarTipos);
    Simbolo* buscarSimboloTodosEscopos(const char *nome, CategoriaSimbolo categoria, int verificarTipos);
    void declararSimbolo(const char *nome, CategoriaSimbolo categoria, TipoSimbolo tipoSimbolo, TipoVariavel tipo, int tamanhoVetor);
    void atribuirValorAVariavel(const char *nome, int valorAtribuido, int indiceVetor);
    int buscarValorDeVariavel(const char *nome, int indiceVetor);
%}

%union {
    int                 ival;
    char*               id;
    AST*                noAST;
}

%error-verbose

/* TOKENS */
%token T_IF T_ELSE T_WHILE T_INT T_VOID T_RETURN
%token T_MAIS T_MENOS T_MULT T_DIV T_ATRIBUICAO
%token T_MAIOR T_MENOR T_IGUAL T_DIFERENTE T_MAIORIGUAL T_MENORIGUAL
%token T_VIRGULA T_PONTOEVIRGULA
%token T_APAR T_FPAR T_ACOLCHETE T_FCOLCHETE T_ACHAVE T_FCHAVE
%token <id>  T_ID
%token <ival> T_NUM

/* ORDEM DE PRECEDENCIA */
%left T_MAIS T_MENOS T_MULT T_DIV

/* TIPOS RETORNADOS */
%type <noAST> programa listaDeclaracoes declaracao declaracaoVariaveis declaracaoFuncao
%type <noAST> tipoEspecificador parametros listaParametros parametro 
%type <noAST> escopo declaracoesLocais listaEscopo corpo 
%type <noAST> declaracaoExpressao declaracaoSelecao declaracaoIteracao declaracaoRetorno 
%type <noAST> expressao variavel expressaoSimples relacional expressaoSoma 
%type <noAST> soma termo mult fator 
%type <noAST> chamadaFuncao argumentos listaArgumentos

/* VARIAVEL DE INICIO */
%start programa

%%
    programa: 
        listaDeclaracoes
            {
                $$ = $1;
            }
        ;

    listaDeclaracoes:
        listaDeclaracoes declaracao
            {
                $$ = criarNo(TipoListaDeclaracoes);
                $$->filhos[0] = $1;
                $$->filhos[1] = $2;
            }
        | declaracao
            {
                $$ = $1;
            }
        ;

    declaracao:
        declaracaoVariaveis
            {
                $$ = $1;
            }
        | declaracaoFuncao
            {
                $$ = $1;
            }
        ;

    declaracaoVariaveis:
        tipoEspecificador T_ID T_PONTOEVIRGULA
        | tipoEspecificador T_ID T_ACOLCHETE T_NUM T_FCOLCHETE T_PONTOEVIRGULA
        ;

    tipoEspecificador:
        T_INT
        | T_VOID
        ;

    declaracaoFuncao:
        tipoEspecificador T_ID T_APAR parametros T_FPAR escopo
        ;

    parametros: 
        listaParametros
        | T_VOID
        ;

    listaParametros:
        listaParametros T_VIRGULA parametro
        | parametro
        ;

    parametro:
        tipoEspecificador T_ID
        | tipoEspecificador T_ID T_ACOLCHETE T_FCOLCHETE
        ;

    escopo:
        T_ACHAVE declaracoesLocais listaEscopo T_FCHAVE
        ;

    declaracoesLocais:
        declaracoesLocais declaracaoVariaveis
        |
        ;

    listaEscopo:
        listaEscopo corpo
        |
        ;

    corpo:  
        declaracaoExpressao
        | escopo
        | declaracaoSelecao
        | declaracaoIteracao
        | declaracaoRetorno
        ;

    declaracaoExpressao:   
        expressao T_PONTOEVIRGULA
        | T_PONTOEVIRGULA
        ;

    declaracaoSelecao:
        T_IF T_APAR expressao T_FPAR corpo
        | T_IF T_APAR expressao T_FPAR corpo T_ELSE corpo
        ;

    declaracaoIteracao:
        T_WHILE T_APAR expressao T_FPAR corpo
        ;

    declaracaoRetorno:
        T_RETURN T_PONTOEVIRGULA
        | T_RETURN expressao T_PONTOEVIRGULA
        ;

    expressao:
        variavel T_ATRIBUICAO expressao
        | expressaoSimples
        ;

    variavel:
        T_ID
        | T_ID T_ACOLCHETE expressao T_FCOLCHETE
        ;

    expressaoSimples:
        expressaoSoma relacional expressaoSoma
        | expressaoSoma
        ;

    relacional: 
        T_MENORIGUAL 
        | T_MENOR
        | T_MAIOR
        | T_MAIORIGUAL
        | T_IGUAL
        | T_DIFERENTE
        ;

    expressaoSoma:
        expressaoSoma soma termo
        | termo
        ;

    soma:
        T_MAIS
        | T_MENOS
        ;

    termo:
        termo mult fator
        | fator
        ;

    mult:
        T_MULT
        | T_DIV
        ;

    fator:
        T_APAR expressao T_FPAR
        | variavel
        | chamadaFuncao
        | T_NUM
        ;

    chamadaFuncao:
        T_ID T_APAR argumentos T_FPAR
        ;

    argumentos:
        listaArgumentos
        |
        ;

    listaArgumentos:
        listaArgumentos T_VIRGULA expressao
        | expressao
        ;
%%

/* ========== FUNCOES PARA TRATAR DA AST ========== */
AST* criarNo(TipoNo tipo) {

    AST* no = malloc(sizeof(AST));
    no->tipo = tipo;

    for (int i=0; i<MAXFILHOS; i++) {
        no->filhos[i] = NULL;
    }
    
    return no;
}

void liberaAST(AST* no) {

    if (!no) return;
    
    for (int i=0; i<MAXFILHOS; i++) {
        liberaAST(no->filhos[i]);
    }

    if(no->tipo == TipoID || no->tipo == TipoEspecifico || no->tipo == TipoRelacional || no->tipo == TipoSoma || no->tipo == TipoMult) {
        free(no->dado.nome);
    }

    free(no);
}

/* ========== FUNCOES PARA TRATAR DA TABELA DE SIMBOLOS ========== */
void entrarEscopo(int linhaInicio) { // push
    Escopo *e = (Escopo*) malloc(sizeof(Escopo));
    e->simbolos = NULL;
    e->linhaInicio = linhaInicio;
    e->prox = topo_escopo;
    topo_escopo = e;
}

void sairEscopo() { // pop
    if (topo_escopo == NULL) return;

    Simbolo *s = topo_escopo->simbolos;

    if (topo_escopo->prox == NULL && strcmp(topo_escopo->simbolos->nome, "main") != 0) {
        // main() não foi declarada como último elemento do escopo global 
        fprintf(stderr, "ERRO SEMANTICO: funcao \"main\" nao declarada ou declarada incorretamente");
        return;
    }

    if (s) {
        printf("ESCOPO: LINHAS %d a %d\n", topo_escopo->linhaInicio, linha_atual);
    }

    while (s) {
        char *categoria = (s->categoria == Variavel) ? "variavel" : "funcao";
        char *tipoSimbolo = (s->tipoSimbolo == TipoInt) ? "int" : "void";
        char *tipoVariavel = (s->categoria == Variavel) ? (s->tipo == TipoInteiro ? "//inteiro" : "//vetor") : "";
        printf("\t-> %s\t\t[%s//%s%s]\n", s->nome, categoria, tipoSimbolo, tipoVariavel);
        
        Simbolo *tmp = s;
        s = s->prox;
        // Libera nome do símbolo
        free(tmp->nome);
        // Libera vetor caso seja variável e vetor
        if (tmp->categoria == Variavel && tmp->tipo == TipoVetor) {
            free(tmp->valor.vetor);
        }
        free(tmp);
    }

    Escopo *tmpE = topo_escopo;
    topo_escopo = topo_escopo->prox;
    free(tmpE);
}

Simbolo* buscarSimboloEscopoAtual(const char *nome, CategoriaSimbolo categoria, int verificarTipos) {
    if (!topo_escopo) return NULL;
    Simbolo *s = topo_escopo->simbolos;
    while (s) {
        if (strcmp(s->nome, nome) == 0) {
            if (verificarTipos == 1 && s->categoria != categoria) {
                s = s->prox;
                continue;
            }
            return s;
        }
        s = s->prox;
    }
    return NULL;
}

Simbolo* buscarSimboloTodosEscopos(const char *nome, CategoriaSimbolo categoria, int verificarTipos) {
    Escopo *e = topo_escopo;
    while (e) {
        Simbolo *s = e->simbolos;
        while (s) {
            if (strcmp(s->nome, nome) == 0) {
                if (verificarTipos == 1 && s->categoria != categoria) {
                    s = s->prox;
                    continue;
                }
                return s;
            }
            s = s->prox;
        }
        e = e->prox;
    }
    return NULL;
}

void declararSimbolo(const char *nome, CategoriaSimbolo categoria, TipoSimbolo tipoSimbolo, TipoVariavel tipo, int tamanhoVetor) {
    if (buscarSimboloEscopoAtual(nome, categoria, 1) != NULL) {
        // Erro de simbolo duplicada no escopo
        fprintf(stderr, "ERRO SEMANTICO: identificador \"%s\" em uso - LINHA: %d\n", nome, linha_atual);
        return;
    }
    if (strcmp(nome, "output") == 0 || strcmp(nome, "input") == 0) {
        // Erro de tentativa de declaração de função com nome reservado
        fprintf(stderr, "ERRO SEMANTICO: nome \"%s\" reservado para funcao - LINHA: %d\n", nome, linha_atual);
        return;
    }
    Simbolo *s = (Simbolo*) malloc(sizeof(Simbolo));
    s->nome = strdup(nome);
    s->categoria = categoria;
    s->tipoSimbolo = tipoSimbolo;
    s->prox = topo_escopo->simbolos;
    topo_escopo->simbolos = s;

    if (s->categoria != Variavel) {
        return;
    }
    
    // Continua somente se simbolo for variavel
    s->tipo = tipo;

    // Inicialização e Alocação
    if (tipo == TipoInteiro) {
        s->tamanho = 1;
        s->valor.inteiro = 0; 
    } else if (tipo == TipoVetor) {
        s->tamanho = tamanhoVetor;
        s->valor.vetor = (int*) calloc(tamanhoVetor, sizeof(int));
    }
}

void atribuirValorAVariavel(const char *nome, int valorAtribuido, int indiceVetor) {
    Simbolo *s = buscarSimboloTodosEscopos(nome, Variavel, 1);
    if (!s) {
        // Erro de variavel nao declarada
        fprintf(stderr, "ERRO SEMANTICO: variavel \"%s\" nao declarada - LINHA: %d\n", nome, linha_atual);
        return;
    }

    if (s->categoria != Variavel) {
        // Erro de tentativa de atribuição a algo que não é variável
        fprintf(stderr, "ERRO SEMANTICO: identificador \"%s\" nao eh variavel - LINHA: %d\n", nome, linha_atual);
        return;
    }

    // Se é vetor...
    if (s->tipo == TipoVetor) {
        if (indiceVetor >= s->tamanho || indiceVetor < 0) {
            // Erro de tentativa de acesso do vetor em campo não existente
            fprintf(stderr, "ERRO SEMANTICO: variavel \"%s\", índice de acesso fora do intervalo - LINHA: %d\n", nome, linha_atual);
            return;
        }
        s->valor.vetor[indiceVetor] = valorAtribuido;
        return;
    }

    // Se é inteiro simples...
    s->valor.inteiro = valorAtribuido;
}

int buscarValorDeVariavel(const char *nome, int indiceVetor) {
    Simbolo *s = buscarSimboloTodosEscopos(nome, Variavel, 1);
    if (!s) {
        // Erro de variavel nao declarada
        fprintf(stderr, "ERRO SEMANTICO: variavel \"%s\" nao declarada - LINHA: %d\n", nome, linha_atual);
        return 0;
    }

    if (s->categoria != Variavel) {
        // Erro de tentativa de acesso a algo que não é variável
        fprintf(stderr, "ERRO SEMANTICO: identificador \"%s\" nao eh variavel - LINHA: %d\n", nome, linha_atual);
        return 0;
    }

    // Se é vetor...
    if (s->tipo == TipoVetor) {
        if (indiceVetor >= s->tamanho) {
            // Erro de tentativa de acesso do vetor em campo não existente
            fprintf(stderr, "ERRO SEMANTICO: variavel \"%s\", índice de acesso fora do intervalo - LINHA: %d\n", nome, linha_atual);
            return 0;
        }
        return s->valor.vetor[indiceVetor];
    }

    // Se é inteiro simples...
    return s->valor.inteiro;
}

/* ========== FUNCOES PARA TRATAR DAS QUADRUPLAS ========== */
char* novoTemp() {
    char* t = malloc(sizeof(char)*20);
    sprintf(t, "t%d", temp_cont++);
    return t;
}

char* novoLabel() {
    char* l = malloc(sizeof(char)*20);
    sprintf(l, "L%d", label_cont++);
    return l;
}

void emitir_quads(const char *op, const char *a1, const char *a2, const char *res) {
    quads = realloc(quads, sizeof(Quadrupla)*(quads_cont + 1));

    quads[quads_cont].operacao = strdup(op);
    quads[quads_cont].argumento1 = a1  ? strdup(a1) : NULL;
    quads[quads_cont].argumento2 = a2  ? strdup(a2) : NULL;
    quads[quads_cont].resultado  = res ? strdup(res) : NULL;

    quads_cont++;
}

void limpa_quads() {
    for(int i=0;i<quads_cont;i++) {
        free(quads[i].operacao);
        if(quads[i].argumento1) free(quads[i].argumento1);
        if(quads[i].argumento2) free(quads[i].argumento2);
        if(quads[i].resultado) free(quads[i].resultado);
    }

    free(quads);
    quads = NULL;
    quads_cont = 0;
    quads = malloc(sizeof(Quadrupla)*quads_cont);
    temp_cont = 0;
    label_cont = 0;
}

char* gerar_quads(AST* no){
    if(!no) return NULL;

    if((no->tipo == TipoDeclaracaoVars)) { // declaracao de variaveis
        char* tipo = no->filhos[0]->dado.nome; 
        char *nome = no->filhos[1]->dado.nome;

        emitir_quads("VAR", tipo, nome, funcao_atual);

        if(no->filhos[2]) { // vetores
            char buffer[32];
            sprintf(buffer, "%d", no->filhos[2]->dado.valor);
            emitir_quads("VETOR", nome, buffer, funcao_atual);
        }

        return NULL;
    }

    else if((no->tipo == TipoDeclaracaoFunc)) { // funcoes
        funcao_atual = no->filhos[1]->dado.nome;
        emitir_quads("FUNCAO", no->filhos[0]->dado.nome, no->filhos[1]->dado.nome, "-");
        gerar_quads(no->filhos[2]); // parametros
        gerar_quads(no->filhos[3]); // escopo
        return NULL;
    }

    else if(no->tipo == TipoExpressaoSimples) { // expressao simples
        if(no->filhos[1] == NULL) return gerar_quads(no->filhos[0]);

        char* op = no->filhos[1]->dado.nome;
        char* arg1 = gerar_quads(no->filhos[0]);
        char* arg2 = gerar_quads(no->filhos[2]);
        char* temp = novoTemp();
        emitir_quads(op, arg1, arg2, temp);
        return temp;
    }

    else if(no->tipo == TipoExpressao) { // expressao 
        if(no->filhos[1] == NULL) return gerar_quads(no->filhos[0]);

        char* arg1 = gerar_quads(no->filhos[2]);
        char* var = no->filhos[0]->filhos[0]->dado.nome;
        emitir_quads("=", arg1, "-", var);
        return var;
    }

    else if(no->tipo == TipoExpressaoSoma) { // expressaoSoma
        if(no->filhos[1] == NULL) return gerar_quads(no->filhos[0]);

        char* op = no->filhos[1]->dado.nome;
        char* arg1 = gerar_quads(no->filhos[0]);
        char* arg2 = gerar_quads(no->filhos[2]);
        char* temp = novoTemp();
        emitir_quads(op, arg1, arg2, temp);
        return temp;
    }

    else if(no->tipo == TipoTermo) { // termo
        if(no->filhos[1] == NULL) return gerar_quads(no->filhos[0]);

        char* op = no->filhos[1]->dado.nome;
        char* arg1 = gerar_quads(no->filhos[0]);
        char* arg2 = gerar_quads(no->filhos[2]);
        char* temp = novoTemp();
        emitir_quads(op, arg1, arg2, temp);
        return temp;
    }

    else if(no->tipo == TipoFator) { // fator
        return gerar_quads(no->filhos[0]);
    }

    else if(no->tipo == TipoVar) { // variavel
        if(no->filhos[1] == NULL) // inteiro
            return no->filhos[0]->dado.nome;
        
        char* indice = gerar_quads(no->filhos[1]);
        char* nome = no->filhos[0]->dado.nome;

        char* var = malloc(strlen(nome) + strlen(indice) + 3);
        sprintf(var, "%s[%s]", nome, indice);

        return var;
    }

    else if(no->tipo == TipoNum){ // numero
        char* buffer = malloc(sizeof(char)*64);
        sprintf(buffer, "%d", no->dado.valor);
        return buffer;
    }

    else if(no->tipo == TipoID) { //id
        return no->dado.nome;
    }

    else if(no->tipo == TipoListaArgumentos) {
        return gerar_quads(no->filhos[0]);
    }

    else if(no->tipo == TipoChamadaFuncao) {
        int args_cont = 0;
        if(no->filhos[1]) { // se houver argumentos (pode ser NULL)
            AST *args = no->filhos[1]->filhos[0]; // pega a lista de argumentos
            while(args) {
                char *arg = gerar_quads(args->filhos[0]); // gera cada argumento
                emitir_quads("ARG", arg, "-", "-");
                args_cont++;
                args = args->filhos[1]; // próximo argumento da lista
            }
        }

        char *tmp = novoTemp();
        char* contador = malloc(sizeof(char)*64);
        sprintf(contador, "%d", args_cont);

        emitir_quads("CALL", no->filhos[0]->dado.nome, contador, tmp);
        return tmp;
    }

    else if((no->tipo == TipoDeclaracaoSelecao)) { // if, else
        char *cond = gerar_quads(no->filhos[0]);

        char *L_else = novoLabel();
        char *L_end  = novoLabel();
        
        emitir_quads("IF_FALSE", cond, "-", L_else);
        gerar_quads(no->filhos[1]); // then

        if(no->filhos[3]) { // else
            emitir_quads("GOTO", "-", "-", L_end);
            emitir_quads("LABEL", "-", "-", L_else);
            gerar_quads(no->filhos[3]); // else
            emitir_quads("LABEL", "-", "-", L_end);
        } else {
            emitir_quads("LABEL", "-", "-", L_else);
        }
        return NULL;
    }

    else if((no->tipo == TipoDeclaracaoIteracao)) { //while
        char *L_ini = novoLabel();
        char *L_end = novoLabel();

        emitir_quads("LABEL", "-", "-", L_ini);

        char *cond = gerar_quads(no->filhos[0]);
        emitir_quads("IF_FALSE", cond, "-", L_end);

        gerar_quads(no->filhos[1]);

        emitir_quads("GOTO", "-", "-", L_ini);
        emitir_quads("LABEL", "-", "-", L_end);

        return NULL;
    }

    else if(no->tipo == TipoParametros) {
        gerar_quads(no->filhos[0]);
        return NULL;
    }

    else if(no->tipo == TipoListaParametros) {
        gerar_quads(no->filhos[0]);
        gerar_quads(no->filhos[1]);
        return NULL;
    }

    else if(no->tipo == TipoParametro) {
        char *tipo = no->filhos[0]->dado.nome;
        char *nome = no->filhos[1]->dado.nome;

        emitir_quads("PARAM", tipo, nome, funcao_atual);
        if(no->filhos[2]) {
            emitir_quads("VETOR", nome, "-", funcao_atual);
        }

        return NULL;
    }

    else if(no->tipo == TipoDeclaracaoRetorno) {
        if(no->filhos[0]) {
            char *val = gerar_quads(no->filhos[0]);
            emitir_quads("RETURN", val, "-", "-");
        } else {
            emitir_quads("RETURN", "-", "-", "-");
        }
        return NULL;
    }

    else if(no->tipo == TipoEscopo || no->tipo == TipoListaEscopo || no->tipo == TipoDeclaracaoLocais) {
        gerar_quads(no->filhos[0]);
        gerar_quads(no->filhos[1]);
        return NULL;
    }

    else if (no->tipo == TipoCorpo) {
        gerar_quads(no->filhos[0]);
        return NULL;
    }

    for(int i=0;i<MAXFILHOS && no->filhos[i] != NULL;i++) 
        gerar_quads(no->filhos[i]);
    
    return NULL;
}

void print_quads(FILE *out){
    for(int i=0;i<quads_cont;i++){
        Quadrupla *q = &quads[i];
        fprintf(out, "%3d: (%s, %s, %s, %s)\n",
                i,
                q->operacao ? q->operacao : "-",
                q->argumento1 ? q->argumento1 : "-",
                q->argumento2 ? q->argumento2 : "-",
                q->resultado  ? q->resultado  : "-");
    }
}

/* ========== FUNCOES PARA GERAR ARQUIVO .dot ========== */
const char* nomeTipoNo(AST* no) {
    static char buffer[50];
    switch (no->tipo) {
        case TipoPrograma:              return "programa";
        case TipoListaDeclaracoes:      return "listaDeclaracoes";
        case TipoDeclaracao:            return "declaracao";
        case TipoDeclaracaoVars:        return "declaracaoVariaveis";
        case TipoEspecifico:            return no->dado.nome;
        case TipoDeclaracaoFunc:        return "declaracaoFuncao";
        case TipoID:                    return no->dado.nome;
        case TipoParametros:            return "parametros";
        case TipoListaParametros:       return "listaParametros";
        case TipoParametro:             return "parametro";
        case TipoEscopo:                return "escopo";
        case TipoDeclaracaoLocais:      return "declaracoesLocais";
        case TipoListaEscopo:           return "listaEscopo";
        case TipoDeclaracaoExpressao:   return "declaracaoExpressao";
        case TipoDeclaracaoSelecao:     return "if";
        case TipoDeclaracaoIteracao:    return "while";
        case TipoDeclaracaoRetorno:     return "return";
        case TipoExpressao:             return "expressao";
        case TipoVar:                   return "variavel";
        case TipoExpressaoSimples:      return "expressaoSimples";
        case TipoRelacional:            return no->dado.nome;
        case TipoExpressaoSoma:         return "expressaoSoma";
        case TipoSoma:                  return no->dado.nome;
        case TipoTermo:                 return "termo";
        case TipoMult:                  return no->dado.nome;
        case TipoFator:                 return "fator";
        case TipoChamadaFuncao:         return "chamadaFuncao";
        case TipoArgumentos:            return "argumentos";
        case TipoListaArgumentos:       return "listaArgumentos";
        case TipoCorpo:                 return "corpo";
        case TipoNum:                   sprintf(buffer, "%d", no->dado.valor); return buffer;
        default:                        return "???";
    }
}

void imprimirNoDOT(AST* no, FILE* file) {
    if (!no) return;

    char nomeDoNo[30];

    strcpy(nomeDoNo, nomeTipoNo(no));

    for(int i=0; i<MAXFILHOS; i++) {
        AST* filho = no->filhos[i];
        if(filho) {
            char nomeFilho[50];
            strcpy(nomeFilho, nomeTipoNo(filho));
            fprintf(file, "  node%p[label=\"%s\"];\n", (void*)no, nomeDoNo);
            fprintf(file, "  node%p[label=\"%s\"];\n", (void*)filho, nomeFilho);
            fprintf(file, "  node%p -> node%p;\n", (void*)no, (void*)filho);
            imprimirNoDOT(filho, file);
        }
    }
}

void gerarDOT(AST* raiz) {
    FILE *file;
    file = fopen("build/arvore.dot", "w");
    if(file==NULL) {
        printf("Erro ao abrir arquivo para gerar arquivo.dot\n");
        return;
    }
    
    fprintf(file, "digraph AST {\n");

    imprimirNoDOT(raiz, file);

    fprintf(file, "}\n");
    fclose(file);
}

/* ========== MAIN ========== */
int main(int argc, char **argv){
    if(argc > 1){
        yyin = fopen(argv[1], "r");
        if(yyin == NULL){
            fprintf(stderr, "Problema na leitura do arquivo!");
            exit(1);
        }
    }
    else{
        yyin = fopen("flex/entrada.txt", "r");        
        if(yyin == NULL){
            fprintf(stderr, "Problema na leitura do arquivo!");
            exit(1);
        }
    }

    /* cria escopo global */
    //entrarEscopo(linha_atual);

    // TODO: TIRAR DECLARACAO DE INPUT/OUTPUT? NAO PRECISA DISSO AQUI
    /* declararSimbolo("input", Funcao, TipoInt, TipoInteiro, 0);
    declararSimbolo("output", Funcao, TipoVoid, TipoInteiro, 0); */

    yyparse();

    /* sai do escopo global e libera memória */
    //sairEscopo();

    /* Geração de quádruplas a partir da AST */
    /*gerar_quads(raizAST);
    printf("\nQuadruplas geradas:\n");
    print_quads(stdout);
    printf("---------------------\n> ");
    limpa_quads();

    gerarDOT(raizAST);

    liberaAST(raizAST);*/

    fclose(yyin);

    return 0;
}

void yyerror(const char* msg) {
	fprintf(stderr, "ERRO SINTATICO: \"%s\" - LINHA: %d\n", msg, linha_atual);
	exit(1);
}