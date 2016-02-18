/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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

#ifndef YY_YY_PARSER_HH_INCLUDED
# define YY_YY_PARSER_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TBOOL = 258,
    TINT = 259,
    TFLOAT = 260,
    TIDENT = 261,
    TNOT = 262,
    TAST = 263,
    TSLASH = 264,
    TMINUS = 265,
    TPLUS = 266,
    TMINUS_DOT = 267,
    TPLUS_DOT = 268,
    TAST_DOT = 269,
    TSLASH_DOT = 270,
    TEQUAL = 271,
    TLESS_GREATER = 272,
    TLESS_EQUAL = 273,
    TGREATER_EQUAL = 274,
    TLESS = 275,
    TGREATER = 276,
    TIF = 277,
    TTHEN = 278,
    TELSE = 279,
    TLET = 280,
    TIN = 281,
    TREC = 282,
    TCOMMA = 283,
    TARRAY_CREATE = 284,
    TDOT = 285,
    TLESS_MINUS = 286,
    TSEMICOLON = 287,
    TLPAREN = 288,
    TRPAREN = 289,
    TENDOFFILE = 290,
    prec_let = 291,
    prec_if = 292,
    prec_unary_minus = 293,
    prec_app = 294
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 18 "driver.y" /* yacc.c:1909  */

	NExpression* syntax;
	NIdentifier* id;
	NInteger* intval;
	NCompExpression* comp_expr;
	NIfExpression* if_expr;
	NFundefExpression* fundef;
	std::vector<NLetExpression*>* varlist;
	std::vector<NExpression*>* explist;
	std::string* token;
	int op;

#line 107 "parser.hh" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_HH_INCLUDED  */
