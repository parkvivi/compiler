%option noyywrap

%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include "sintatico.tab.h"

    #define YY_DECL int yylex()

    void erroLexico(int linha, const char* mensagem);

    int linha_atual = 1;
    int inicio_comentario = 0;
%}

%x COMENTARIO

DIGITO [0-9]
LETRA [a-zA-Z]
ID {LETRA}+
NUM {DIGITO}+

%%

[ \t\r]+            {}

"if"                {
    return T_IF;
}

"else"              {
    return T_ELSE;
}

"while"             {
    return T_WHILE;
}

"int"               {
    return T_INT;
}

"void"              {
    return T_VOID;
}

"return"            {
    return T_RETURN;
}

{ID}                {
    yylval.id = strdup(yytext);
    return T_ID;
}

{NUM}               {
    yylval.ival = atoi(yytext);
    return T_NUM;
}

"+"                 {
    return T_MAIS;
}

"-"                 {
    return T_MENOS;
}



"/*"                {
    inicio_comentario = linha_atual;
    BEGIN(COMENTARIO);
}

<COMENTARIO>"*/"    {
    BEGIN(INITIAL);
}

<COMENTARIO>\n      {
    linha_atual++;
}

<COMENTARIO>.       {}

<COMENTARIO><<EOF>>      {
    erroLexico(inicio_comentario, "Comentario nao fechado");
}

"*/"                {
    erroLexico(linha_atual, yytext);
}


"*"                 {
    return T_MULT;
}

"/"                 {
    return T_DIV;
}

">="                {
    return T_MAIORIGUAL;
}

"<="                {
    return T_MENORIGUAL;
}

"=="                {
    return T_IGUAL;
}

"!="                {
    return T_DIFERENTE;
}

">"                 {
    return T_MAIOR;
}

"<"                 {
    return T_MENOR;
}

"="                 {
    return T_ATRIBUICAO;
}

";"                 {
    return T_PONTOEVIRGULA;
}

","                 {
    return T_VIRGULA;
}

"("                 {
    return T_APAR;
}

")"                 {
    return T_FPAR;
}

"["                 {
    return T_ACOLCHETE;
}

"]"                 {
    return T_FCOLCHETE;
}

"{"                 {
    return T_ACHAVE;
}

"}"                 {
    return T_FCHAVE;
}

"\n"                {
    linha_atual++;
}

.                   {
    erroLexico(linha_atual, yytext);
}

%%

void erroLexico(int linha, const char* mensagem) {
	fprintf(stderr, "LINHA %d :: ERRO LEXICO :: \"%s\"\n", linha, mensagem);
	exit(1);
}