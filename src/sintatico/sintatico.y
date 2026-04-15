%{
    #include<string.h>

    #include "ast.h"
    #include "tabsimbolos.h"

    extern int linha_atual;
    extern int yylex();

    void yyerror(const char* mensagem);
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
                $$ = criarNo(TipoDeclaracaoVariavel);
                $$->dado.nome = strdup($2);

                $$->filhos[0] = $1;

                free($2);
            }
        | tipoEspecificador T_ID T_ACOLCHETE T_NUM T_FCOLCHETE T_PONTOEVIRGULA
            {
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
        tipoEspecificador T_ID T_APAR parametros T_FPAR escopo
            {
                $$ = criarNo(TipoDeclaracaoFuncao);
                $$->dado.nome = strdup($2);

                $$->filhos[0] = $1;
                $$->filhos[1] = $4;
                $$->filhos[2] = $6;

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
                $$ = criarNo(TipoParametro);
                $$->dado.nome = strdup($2);

                $$->filhos[0] = $1;

                free($2);
            }
        | tipoEspecificador T_ID T_ACOLCHETE T_FCOLCHETE
            {
                $$ = criarNo(TipoParametro);
                $$->dado.nome = strdup($2);

                $$->filhos[0] = $1;
                $$->filhos[1] = criarNo(TipoVetor);

                free($2);
            }
        ;

    escopo:
        T_ACHAVE declaracoesLocais listaEscopo T_FCHAVE
            {
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
        | escopo
            {
                $$ = criarNo(TipoEscopo);
                $$->filhos[0] = $1;
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
                $$ = criarNo(TipoVar);
                $$->dado.nome = strdup($1);

                free($1);
            }
        | T_ID T_ACOLCHETE expressao T_FCOLCHETE
            {
                $$ = criarNo(TipoVar);
                $$->dado.nome = strdup($1);

                $$->filhos[0] = $3;
                $$->filhos[1] = criarNo(TipoVetor);

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

void yyerror(const char* mensagem) {
	fprintf(stderr, "LINHA %d :: ERRO SINTATICO :: \"%s\"\n", linha_atual, mensagem);
	exit(1);
}