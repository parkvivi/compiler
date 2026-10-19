
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_IF = 258,
     T_ELSE = 259,
     T_WHILE = 260,
     T_INT = 261,
     T_VOID = 262,
     T_RETURN = 263,
     T_MAIS = 264,
     T_MENOS = 265,
     T_MULT = 266,
     T_DIV = 267,
     T_ATRIBUICAO = 268,
     T_MAIOR = 269,
     T_MENOR = 270,
     T_IGUAL = 271,
     T_DIFERENTE = 272,
     T_MAIORIGUAL = 273,
     T_MENORIGUAL = 274,
     T_VIRGULA = 275,
     T_PONTOEVIRGULA = 276,
     T_APAR = 277,
     T_FPAR = 278,
     T_ACOLCHETE = 279,
     T_FCOLCHETE = 280,
     T_ACHAVE = 281,
     T_FCHAVE = 282,
     T_ID = 283,
     T_NUM = 284
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 13 "src/sintatico/sintatico.y"

    int                 ival;
    char*               id;
    struct AST*         noAST;



/* Line 1676 of yacc.c  */
#line 89 "os\\sintatico.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


