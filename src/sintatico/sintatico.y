%{
    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>

    extern int yylex();
    extern int yyparse();
    extern FILE* yyin;

    extern int linha_atual;

    void yyerror(const char* s);
    int contErros = 0;
    
    /* ========== ESTRUTURAS E PROTOTIPOS DA ARVORE ========== */

    // Tipos de nos da AST
    typedef enum {
        TipoDeclaracaoFuncao,
        TipoDeclaracaoVariavel,
        TipoTipoEspecificador,
        TipoNum,
        TipoVar,
        TipoDeclaracaoSelecao,
        TipoDeclaracaoIteracao,
        TipoDeclaracaoRetorno,
        TipoChamadaFuncao,
        TipoParametro,
        TipoAtribuicao,
        TipoMenorIgual,
        TipoMenor,
        TipoMaiorIgual,
        TipoMaior,
        TipoIgual,
        TipoDiferente,
        TipoMultiplicacao,
        TipoDivisao,
        TipoSoma,
        TipoSubtracao
    } TipoNo;

    #define MAXFILHOS 3

    // Estrutura de um no da AST
    typedef struct AST {
        TipoNo tipo;
        union {
            char* nome;
            int valor;
        } dado;
        struct AST* irmao;
        struct AST* filhos[MAXFILHOS];
    } AST;

    AST *raizAST = NULL;
    char* origem = NULL;

    // Prototipos das funcoes para tratar da AST
    AST* criarNo(TipoNo tipo);
    AST* anexarIrmao(AST* lista, AST* novoNo);
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

    typedef enum { Variavel, Funcao } CategoriaSimbolo;     // Categoria do simbolo -> variavel ou funcao
    typedef enum { TipoVoid, TipoInt } TipoSimbolo;         // Tipo do simbolo -> void ou int
    typedef enum { TipoInteiro, TipoVetor } TipoVariavel;   // Tipo da variavel -> inteiro ou vetor

    typedef struct Simbolo {
        char *nome;
        CategoriaSimbolo categoria;
        struct Simbolo *prox;
        TipoSimbolo tipoSimbolo;
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
    Simbolo* buscarSimboloEscopoAtual(const char *nome, CategoriaSimbolo categoria, TipoVariavel tipo, int verificarTipos);
    Simbolo* buscarSimboloTodosEscopos(const char *nome, CategoriaSimbolo categoria, TipoVariavel tipo, int verificarTipos);
    void declararSimbolo(const char *nome, CategoriaSimbolo categoria, TipoSimbolo tipoSimbolo, TipoVariavel tipo, int tamanhoVetor);
    void atribuirValorAVariavel(const char *nome, int valorAtribuido, int indiceVetor);
    int buscarValorDeVariavel(const char *nome, int indiceVetor);
%}

%union {
    int                 ival;
    char*               id;
    struct AST*         noAST;
}

%error-verbose

/* TOKENS */
%token T_IF T_ELSE T_WHILE T_INT T_VOID T_RETURN
%token T_MAIS T_MENOS T_MULT T_DIV T_ATRIBUICAO
%token T_MAIOR T_MENOR T_IGUAL T_DIFERENTE T_MAIORIGUAL T_MENORIGUAL
%token T_VIRGULA T_PONTOEVIRGULA
%token T_APAR T_FPAR T_ACOLCHETE T_FCOLCHETE T_ACHAVE T_FCHAVE
%token <id> T_ID
%token <ival> T_NUM

/* ORDEM DE PRECEDENCIA */
%left T_MAIS T_MENOS T_MULT T_DIV

/* TIPOS RETORNADOS */
%type <noAST> programa listaDeclaracoes declaracao declaracaoVariaveis declaracaoFuncao
%type <noAST> tipoEspecificador parametros listaParametros parametro 
%type <noAST> escopo declaracoesLocais listaEscopo corpo 
%type <noAST> declaracaoExpressao declaracaoSelecao declaracaoIteracao declaracaoRetorno 
%type <noAST> expressao variavel expressaoSimples expressaoSoma 
%type <noAST> termo fator 
%type <noAST> chamadaFuncao argumentos listaArgumentos

%type <id> soma mult relacional

/* VARIAVEL DE INICIO */
%start programa

%%
    programa: 
        listaDeclaracoes
            {
                $$ = $1;
                raizAST = $$;
            }
        ;

    listaDeclaracoes:
        listaDeclaracoes declaracao
            {
                $$ = anexarIrmao($1, $2);
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
            {
                if (strcmp($1->dado.nome, "void") == 0) {
                    fprintf(stderr, "LINHA %d :: ERRO SEMANTICO :: variavel \"%s\" declarada com tipo \"void\"\n", linha_atual, $2);
                    contErros++;
                }

                declararSimbolo($2, Variavel, TipoInt, TipoInteiro, 1);

                $$ = criarNo(TipoDeclaracaoVariavel);
                $$->dado.nome = strdup($2);

                $$->filhos[0] = $1;

                free($2);
            }
        | tipoEspecificador T_ID T_ACOLCHETE T_NUM T_FCOLCHETE T_PONTOEVIRGULA
            {
                if (strcmp($1->dado.nome, "void") == 0) {
                    fprintf(stderr, "LINHA %d :: ERRO SEMANTICO :: variavel \"%s\" declarada com tipo \"void\"\n", linha_atual, $2);
                    contErros++;
                }

                declararSimbolo($2, Variavel, TipoInt, TipoVetor, 1);
                
                $$ = criarNo(TipoDeclaracaoVariavel);
                $$->dado.nome = strdup($2);
                
                AST* tamanho = criarNo(TipoNum);
                tamanho->dado.valor = $4;

                $$->filhos[0] = $1;
                $$->filhos[1] = tamanho;

                free($2);
            }
        ;

    tipoEspecificador:
        T_INT
            {
                $$ = criarNo(TipoTipoEspecificador);
                $$->dado.nome = malloc(sizeof("int") + 1);
                strcpy($$->dado.nome, "int");
            }
        | T_VOID
            {
                $$ = criarNo(TipoTipoEspecificador);
                $$->dado.nome = malloc(sizeof("void") + 1);
                strcpy($$->dado.nome, "void"); 
            }
        ;

    declaracaoFuncao:
        tipoEspecificador T_ID T_APAR { entrarEscopo(linha_atual); } parametros T_FPAR escopo
            {
                TipoSimbolo tipoFuncao = (strcmp($1->dado.nome, "int") == 0) ? TipoInt : TipoVoid;
                declararSimbolo($2, Funcao, tipoFuncao, TipoInteiro, 0);

                $$ = criarNo(TipoDeclaracaoFuncao);
                $$->dado.nome = strdup($2);

                $$->filhos[0] = $1;
                $$->filhos[1] = $5;
                $$->filhos[2] = $7;

                free($2);
            }
        ;

    parametros: 
        listaParametros
            {
                $$ = $1;
            }
        | T_VOID
            {
                $$ = NULL;
            }
        ;

    listaParametros:
        listaParametros T_VIRGULA parametro
            {
                $$ = anexarIrmao($1, $3);
            }
        | parametro
            {
                $$ = $1;
            }
        ;

    parametro:
        tipoEspecificador T_ID
            {   
                if (strcmp($1->dado.nome, "void") == 0) {
                    fprintf(stderr, "LINHA %d :: ERRO SEMANTICO :: parametro \"%s\" declarado com tipo \"void\"\n", linha_atual, $2);
                    contErros++;
                }

                declararSimbolo($2, Variavel, TipoInt, TipoInteiro, 1);

                $$ = criarNo(TipoParametro);
                $$->dado.nome = strdup($2);

                $$->filhos[0] = $1;

                free($2);
            }
        | tipoEspecificador T_ID T_ACOLCHETE T_FCOLCHETE
            {   
                if (strcmp($1->dado.nome, "void") == 0) {
                    fprintf(stderr, "LINHA %d :: ERRO SEMANTICO :: parametro \"%s\" declarado com tipo \"void\"\n", linha_atual, $2);
                    contErros++;
                }

                declararSimbolo($2, Variavel, TipoInt, TipoVetor, 100); // tamanho maximo de vetor definido como 100

                $$ = criarNo(TipoParametro);
                $$->dado.nome = strdup($2);

                $$->filhos[0] = $1; // mais alguma coisa para vetor?

                free($2);
            }
        ;

    escopo:
        T_ACHAVE declaracoesLocais listaEscopo T_FCHAVE
            {
                sairEscopo();
                $$ = anexarIrmao($2, $3);
            }
        ;

    declaracoesLocais:
        declaracoesLocais declaracaoVariaveis
            {
                $$ = anexarIrmao($1, $2);
            }
        |
            {
                $$ = NULL;
            }
        ;

    listaEscopo:
        listaEscopo corpo
            {
                $$ = anexarIrmao($1, $2);
            }
        |
            {
                $$ = NULL;
            }
        ;

    corpo:  
        declaracaoExpressao
            {
                $$ = $1;
            }
        | { entrarEscopo(linha_atual); } escopo
            {
                $$ = $2;
            }
        | declaracaoSelecao
            {
                $$ = $1;
            }
        | declaracaoIteracao
            {
                $$ = $1;
            }
        | declaracaoRetorno
            {
                $$ = $1;
            }
        ;

    declaracaoExpressao:   
        expressao T_PONTOEVIRGULA
            {
                $$ = $1;
            }
        | T_PONTOEVIRGULA
            {
                $$ = NULL;
            }
        ;

    declaracaoSelecao:
        T_IF T_APAR expressao T_FPAR corpo
            {
                $$ = criarNo(TipoDeclaracaoSelecao);
                $$->filhos[0] = $3;
                $$->filhos[1] = $5;
            }
        | T_IF T_APAR expressao T_FPAR corpo T_ELSE corpo
            {
                $$ = criarNo(TipoDeclaracaoSelecao);
                $$->filhos[0] = $3;
                $$->filhos[1] = $5;
                $$->filhos[2] = $7;
            }
        ;

    declaracaoIteracao:
        T_WHILE T_APAR expressao T_FPAR corpo
            {
                $$ = criarNo(TipoDeclaracaoIteracao);
                $$->filhos[0] = $3;
                $$->filhos[1] = $5;
            }
        ;

    declaracaoRetorno:
        T_RETURN T_PONTOEVIRGULA
            {
                $$ = criarNo(TipoDeclaracaoRetorno);
            }
        | T_RETURN expressao T_PONTOEVIRGULA
            {
                $$ = criarNo(TipoDeclaracaoRetorno);
                $$->filhos[0] = $2;
            }
        ;

    expressao:
        variavel T_ATRIBUICAO expressao
            {
                $$ = criarNo(TipoAtribuicao);
                $$->filhos[0] = $1;
                $$->filhos[1] = $3;
            }
        | expressaoSimples
            {
                $$ = $1;
            }
        ;

    variavel:
        T_ID
            {   
                if (buscarSimboloTodosEscopos($1, Variavel, TipoInteiro, 1) == NULL) {
                    fprintf(stderr, "LINHA %d :: ERRO SEMANTICO :: variavel \"%s\" nao declarada\n", linha_atual, $1);
                    contErros++;
                }

                $$ = criarNo(TipoVar);
                $$->dado.nome = strdup($1);

                free($1);
            }
        | T_ID T_ACOLCHETE expressao T_FCOLCHETE
            {
                if (buscarSimboloTodosEscopos($1, Variavel, TipoVetor, 1) == NULL) {
                    fprintf(stderr, "LINHA %d :: ERRO SEMANTICO :: variavel \"%s\" nao declarada\n", linha_atual, $1);
                    contErros++;
                }

                $$ = criarNo(TipoVar);
                $$->dado.nome = strdup($1);

                $$->filhos[0] = $3;

                free($1);
            }
        ;

    expressaoSimples:
        expressaoSoma relacional expressaoSoma
            {
                if (strcmp($2, "<=") == 0) {
                    $$ = criarNo(TipoMenorIgual);
                } else if (strcmp($2, "<") == 0) {
                    $$ = criarNo(TipoMenor);
                } else if (strcmp($2, ">=") == 0) {
                    $$ = criarNo(TipoMaiorIgual);
                } else if (strcmp($2, ">") == 0) {
                    $$ = criarNo(TipoMaior);
                } else if (strcmp($2, "==") == 0) {
                    $$ = criarNo(TipoIgual);
                } else if (strcmp($2, "!=") == 0) {
                    $$ = criarNo(TipoDiferente);
                }

                free($2);

                $$->filhos[0] = $1;
                $$->filhos[1] = $3;
            }
        | expressaoSoma
            {
                $$ = $1;
            }
        ;

    relacional: 
        T_MENORIGUAL
            {
                $$ = malloc(strlen("<=") + 1);
                strcpy($$, "<=");
            }
        | T_MENOR
            {
                $$ = malloc(strlen("<") + 1);
                strcpy($$, "<");
            }
        | T_MAIOR
            {
                $$ = malloc(strlen(">") + 1);
                strcpy($$, ">");
            }
        | T_MAIORIGUAL
            {
                $$ = malloc(strlen(">=") + 1);
                strcpy($$, ">=");
            }
        | T_IGUAL
            {
                $$ = malloc(strlen("==") + 1);
                strcpy($$, "==");
            }
        | T_DIFERENTE
            {
                $$ = malloc(strlen("!=") + 1);
                strcpy($$, "!=");
            }
        ;

    expressaoSoma:
        expressaoSoma soma termo
            {
                if (strcmp($2, "+") == 0) {
                    $$ = criarNo(TipoSoma);
                } else if (strcmp($2, "-") == 0) {
                    $$ = criarNo(TipoSubtracao);
                }

                $$->filhos[0] = $1;
                $$->filhos[1] = $3;

                free($2);
            }
        | termo
            {
                $$ = $1;
            }
        ;

    soma:
        T_MAIS
            {
                $$ = malloc(strlen("+") + 1);
                strcpy($$, "+");
            }
        | T_MENOS
            {
                $$ = malloc(strlen("-") + 1);
                strcpy($$, "-");
            }
        ;

    termo:
        termo mult fator
            {
                if (strcmp($2, "*") == 0) {
                    $$ = criarNo(TipoMultiplicacao);
                } else if (strcmp($2, "/") == 0) {
                    $$ = criarNo(TipoDivisao);
                }

                $$->filhos[0] = $1;
                $$->filhos[1] = $3;

                free($2);
            }
        | fator
            {
                $$ = $1;
            }
        ;

    mult:
        T_MULT
            {
                $$ = malloc(strlen("*") + 1);
                strcpy($$, "*");
            }
        | T_DIV
            {
                $$ = malloc(strlen("/") + 1);
                strcpy($$, "/");
            }
        ;

    fator:
        T_APAR expressao T_FPAR
            {
                $$ = $2;
            }
        | variavel
            {
                $$ = $1;
            }
        | chamadaFuncao
            {
                $$ = $1;
            }
        | T_NUM
            {
                $$ = criarNo(TipoNum);
                $$->dado.valor = $1;
            }
        ;

    chamadaFuncao:
        T_ID T_APAR argumentos T_FPAR
            {
                $$ = criarNo(TipoChamadaFuncao);
                $$->dado.nome = strdup($1);

                $$->filhos[0] = $3;

                free($1);
            }
        ;

    argumentos:
        listaArgumentos
            {
                $$ = $1;
            }
        |
            {
                $$ = NULL;
            }
        ;

    listaArgumentos:
        listaArgumentos T_VIRGULA expressao
            {
                $$ = anexarIrmao($1, $3);
            }
        | expressao
            {
                $$ = $1;
            }
        ;
%%

/* ========== FUNCOES PARA TRATAR DA AST ========== */
AST* criarNo(TipoNo tipo) {

    AST* novoNo = malloc(sizeof(AST));
    novoNo->tipo = tipo;

    novoNo->irmao = NULL;

    for (int i=0; i<MAXFILHOS; i++) {
        novoNo->filhos[i] = NULL;
    }
    
    return novoNo;
}

AST* anexarIrmao(AST* listaIrmaos, AST* novoIrmao) {

    if (listaIrmaos == NULL) return novoIrmao;

    AST* atual;
    for (atual=listaIrmaos; atual->irmao!=NULL; atual=atual->irmao);
    atual->irmao = novoIrmao;

    return listaIrmaos;
}

void liberaAST(AST* no) {

    if (!no) return;
    
    for (int i=0; i<MAXFILHOS; i++) {
        liberaAST(no->filhos[i]);
    }

    if(no->tipo == TipoVar || no->tipo == TipoDeclaracaoFuncao || no->tipo == TipoParametro || no->tipo == TipoChamadaFuncao || no->tipo == TipoTipoEspecificador) {
        free(no->dado.nome);
    }

    liberaAST(no->irmao);
    free(no);
}

/* ========== FUNCOES PARA TRATAR DA TABELA DE SIMBOLOS ========== */
void entrarEscopo(int linhaInicio) {

    Escopo *novoEscopo = (Escopo*) malloc(sizeof(Escopo));

    novoEscopo->simbolos = NULL;
    novoEscopo->linhaInicio = linhaInicio;
    novoEscopo->prox = topo_escopo;

    topo_escopo = novoEscopo;
}

void sairEscopo() { // pop
    if (topo_escopo == NULL) return;

    Simbolo *s = topo_escopo->simbolos;
    if (topo_escopo->prox == NULL && strcmp(topo_escopo->simbolos->nome, "main") != 0) {
        // main() não foi declarada como último elemento do escopo global 
        fprintf(stderr, "ERRO SEMANTICO :: funcao \"main\" nao declarada ou declarada incorretamente");
        contErros++;
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
        
        free(tmp->nome);
        
        if (tmp->categoria == Variavel && tmp->tipo == TipoVetor) {
            free(tmp->valor.vetor);
        }
        free(tmp);
    }

    Escopo *tmpE = topo_escopo;
    topo_escopo = topo_escopo->prox;
    free(tmpE);
}

Simbolo* buscarSimboloEscopoAtual(const char *nome, CategoriaSimbolo categoria, TipoVariavel tipoVar, int verificarTipos) {

    if (!topo_escopo) return NULL;

    Simbolo *s = topo_escopo->simbolos;
    while (s) {
        if (strcmp(s->nome, nome) == 0) {
            if (verificarTipos == 1 && s->categoria != categoria && (categoria == Variavel && s->tipo != tipoVar)) {
                s = s->prox;
                continue;
            }
            return s;
        }
        s = s->prox;
    }
    return NULL;
}

Simbolo* buscarSimboloTodosEscopos(const char *nome, CategoriaSimbolo categoria, TipoVariavel tipoVar, int verificarTipos) {

    Escopo *e = topo_escopo;
    while (e) {
        Simbolo *s = e->simbolos;
        while (s) {
            if (strcmp(s->nome, nome) == 0) {
                if (verificarTipos == 1 && s->categoria != categoria && (categoria == Variavel && s->tipo != tipoVar)) {
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

    if (buscarSimboloEscopoAtual(nome, categoria, tipo, 1) != NULL) {
        // Erro de simbolo duplicada no escopo
        fprintf(stderr, "LINHA %d :: ERRO SEMANTICO :: identificador \"%s\" em uso\n", linha_atual, nome);
        contErros++;
        return;
    }

    if (strcmp(nome, "output") == 0 || strcmp(nome, "input") == 0) {
        // Erro de tentativa de declaração de função com nome reservado
        fprintf(stderr, "LINHA %d :: ERRO SEMANTICO :: nome \"%s\" reservado para funcao\n", linha_atual, nome);
        contErros++;
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
    
    s->tipo = tipo;

    if (tipo == TipoInteiro) {
        s->tamanho = 1;
        s->valor.inteiro = 0; 
    } else if (tipo == TipoVetor) {
        s->tamanho = tamanhoVetor;
        s->valor.vetor = (int*) calloc(tamanhoVetor, sizeof(int));
    }
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

    if((no->tipo == TipoDeclaracaoVariavel)) { // declaracao de variaveis
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

    else if((no->tipo == TipoDeclaracaoFuncao)) { // funcoes
        funcao_atual = no->filhos[1]->dado.nome;
        emitir_quads("FUNCAO", no->filhos[0]->dado.nome, no->filhos[1]->dado.nome, "-");
        gerar_quads(no->filhos[2]); // parametros
        gerar_quads(no->filhos[3]); // escopo
        return NULL;
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
        case TipoDeclaracaoVariavel:    return no->dado.nome;
        case TipoDeclaracaoFuncao:      return no->dado.nome;
        case TipoTipoEspecificador:     return no->dado.nome;
        case TipoParametro:             return no->dado.nome;
        case TipoVar:                   return no->dado.nome;
        case TipoChamadaFuncao:         return no->dado.nome;
        case TipoNum:                   sprintf(buffer, "%d", no->dado.valor); return buffer;
        case TipoDeclaracaoSelecao:     return "IF";
        case TipoDeclaracaoIteracao:    return "WHILE";
        case TipoDeclaracaoRetorno:     return "RETURN";
        case TipoAtribuicao:            return "=";
        case TipoMenorIgual:            return  "<=";
        case TipoMenor:                 return  "<";
        case TipoMaiorIgual:            return  ">=";
        case TipoMaior:                 return  ">";
        case TipoIgual:                 return  "==";
        case TipoDiferente:             return  "!=";
        case TipoMultiplicacao:         return  "*";
        case TipoDivisao:               return  "/";
        case TipoSoma:                  return  "+";
        case TipoSubtracao:             return  "-";
        default:                        return "???";
    }
}

void imprimirNoDOT(AST* no, FILE* file) {
    if (!no) return;

    AST* atual = no;
    while (atual) {
        fprintf(file, "  node%p[label=\"%s\"];\n", (void*)atual, nomeTipoNo(atual));
        atual = atual->irmao;
    }

    if (no->irmao) {
        fprintf(file, "  { rank=same; ");

        atual = no;
        while (atual) {
            fprintf(file, "node%p; ", (void*)atual);
            atual = atual->irmao;
        }

        fprintf(file, "}\n");

        atual = no;
        while (atual->irmao) {
            fprintf(file, "  node%p -> node%p [color=\"#189d37\", weight=100, dir=none];\n", (void*)atual, (void*)atual->irmao);
            atual = atual->irmao;
        }
    }

    atual = no;
    while (atual) {
        AST** ordemFilhos = NULL;
        int qtdFilhos = 0;

        for (int i = 0; i < MAXFILHOS; i++) {
            AST* filho = atual->filhos[i];
            if (filho) {
                AST* it = filho;
                while (it) {
                    ordemFilhos = realloc(ordemFilhos, sizeof(AST*) * (qtdFilhos + 1));
                    ordemFilhos[qtdFilhos++] = it;
                    it = it->irmao;
                }

                fprintf(file, "  node%p -> node%p;\n", (void*)atual, (void*)filho);
                imprimirNoDOT(filho, file);
            }
        }

        if (qtdFilhos > 1) {
            fprintf(file, "  { rank=same; ");
            for (int i = 0; i < qtdFilhos; i++) {
                fprintf(file, "node%p; ", (void*)ordemFilhos[i]);
            }
            fprintf(file, "}\n");

            // Arestas invisiveis para fixar ordem esquerda->direita dos filhos.
            for (int i = 0; i < qtdFilhos - 1; i++) {
                fprintf(file, "  node%p -> node%p [style=invis, weight=100];\n",
                        (void*)ordemFilhos[i], (void*)ordemFilhos[i + 1]);
            }
        }

        free(ordemFilhos);

        atual = atual->irmao;
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
    fprintf(file, "  ordering=out;\n");

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

    entrarEscopo(linha_atual);
    yyparse();
    sairEscopo();

    if (contErros != 0) {
        liberaAST(raizAST);
        fclose(yyin);
        exit(1);
    }

    /* Geração de quádruplas a partir da AST */
    /*gerar_quads(raizAST);
    printf("\nQuadruplas geradas:\n");
    print_quads(stdout);
    printf("---------------------\n> ");
    limpa_quads();*/

    gerarDOT(raizAST);

    liberaAST(raizAST);
    fclose(yyin);

    return 0;
}

void yyerror(const char* mensagem) {
	fprintf(stderr, "LINHA %d :: ERRO SINTATICO :: \"%s\"\n", linha_atual, mensagem);
	exit(1);
}