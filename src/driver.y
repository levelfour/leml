%{
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include "leml.hh"
#include "syntax.hh"

extern int yylex();
void yyerror(const char *s) { fprintf(stderr, "ERROR: %s\n", s); }

NExpression *program;
%}

%require "2.7"
%error-verbose

%union {
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
}

%token <token> TBOOL TINT TFLOAT TIDENT
%token <op> TNOT
%token <op> TAST
%token <op> TSLASH
%token <op> TMINUS
%token <op> TPLUS
%token <op> TMINUS_DOT
%token <op> TPLUS_DOT
%token <op> TAST_DOT
%token <op> TSLASH_DOT
%token <op> TEQUAL
%token <op> TLESS_GREATER
%token <op> TLESS_EQUAL
%token <op> TGREATER_EQUAL
%token <op> TLESS
%token <op> TGREATER
%token <op> TIF
%token <op> TTHEN
%token <op> TELSE
%token <op> TLET
%token <op> TIN
%token <op> TREC
%token <op> TCOMMA
%token <op> TARRAY_CREATE
%token <op> TDOT
%token <op> TLESS_MINUS
%token <op> TSEMICOLON
%token <op> TLPAREN
%token <op> TRPAREN
%token TENDOFFILE

%right prec_let
%right TSEMICOLON
%right prec_if
%right TLESS_MINUS
%left TCOMMA
%left TEQUAL TLESS_GREATER TLESS TGREATER TLESS_EQUAL TGREATER_EQUAL
%left TPLUS TMINUS TPLUS_DOT TMINUS_DOT
%left TAST TSLASH TAST_DOT TSLASH_DOT
%right prec_unary_minus
%left prec_app
%left TDOT

%type <syntax> exp simple_exp program
%type <fundef> fundef
%type <id> id_decl
%type <varlist> formal_args pat
%type <explist> actual_args elems
%start program

%%

program: exp { program = $1; };

simple_exp:
  TLPAREN exp TRPAREN
    { $$ = $2; }
| TLPAREN TRPAREN
    { $$ = new NUnit(); }
| TBOOL
    { $$ = new NBoolean(!std::strncmp($1->c_str(), "true", 4)); }
| TINT
    { $$ = new NInteger(atoi($1->c_str())); }
| TFLOAT
    { $$ = new NFloat(atof($1->c_str())); }
| TIDENT
    { $$ = new NIdentifier(*$1); }
| simple_exp TDOT TLPAREN exp TRPAREN
    { $$ = new NArrayGetExpression(*$<id>1, *$<intval>4); }

id_decl: TIDENT { $$ = new NIdentifier(*$1); }

exp:
  simple_exp
    { $$ = $1; }
| TNOT exp
    %prec prec_app
    { $$ = new NUnaryExpression(LNot, *$2); }
| TMINUS exp
    %prec prec_unary_minus
	{
		if(typeid(NFloat) == typeid(*$2)) {
			$$ = new NFloat(-(dynamic_cast<NFloat *>$2)->value);
		} else {
			$$ = new NUnaryExpression(LNeg, *$2);
		}
	}
| exp TPLUS exp
    { $$ = new NBinaryExpression(LAdd, *$1, *$3); }
| exp TMINUS exp
    { $$ = new NBinaryExpression(LSub, *$1, *$3); }
| exp TAST exp
    { $$ = new NBinaryExpression(LMul, *$1, *$3); }
| exp TSLASH exp
    { $$ = new NBinaryExpression(LDiv, *$1, *$3); }
| exp TEQUAL exp
    { $$ = new NCompExpression(LEq, *$1, *$3); }
| exp TLESS_GREATER exp
    { $$ = new NCompExpression(LNeq, *$1, *$3); }
| exp TLESS exp
    { $$ = new NCompExpression(LLT, *$1, *$3); }
| exp TGREATER exp
    { $$ = new NCompExpression(LGT, *$1, *$3); }
| exp TLESS_EQUAL exp
    { $$ = new NCompExpression(LLE, *$1, *$3); }
| exp TGREATER_EQUAL exp
    { $$ = new NCompExpression(LGE, *$1, *$3); }
| TIF exp TTHEN exp TELSE exp
	%prec prec_if
    { $$ = new NIfExpression(*$2, *$4, *$6); }
| TMINUS_DOT exp
    %prec prec_unary_minus
	{ $$ = new NUnaryExpression(LFNeg, *$2); }
| exp TPLUS_DOT exp
    { $$ = new NBinaryExpression(LFAdd, *$1, *$3); }
| exp TMINUS_DOT exp
    { $$ = new NBinaryExpression(LFSub, *$1, *$3); }
| exp TAST_DOT exp
    { $$ = new NBinaryExpression(LFMul, *$1, *$3); }
| exp TSLASH_DOT exp
    { $$ = new NBinaryExpression(LFDiv, *$1, *$3); }
| TLET id_decl TEQUAL exp TIN exp
    %prec prec_let
    { $$ = new NLetExpression(*$2, $4, $6); }
| TLET TREC fundef TEQUAL exp TIN exp
    %prec prec_let
    { $$ = new NLetRecExpression($3, *$5, *$7); }
| exp actual_args
    %prec prec_app
    { $$ = new NCallExpression(*$1, $2); }
| elems
    { $$ = new NTupleExpression(*$1); }
| TLET TLPAREN pat TRPAREN TEQUAL exp TIN exp
    { $$ = new NLetTupleExpression(*$3, *$6, *$8); }
| simple_exp TDOT TLPAREN exp TRPAREN TLESS_MINUS exp
    { $$ = new NArrayPutExpression(*$<id>1, *$<intval>4, *$7); }
| exp TSEMICOLON exp
    { $$ = new NLetExpression(*(new NIdentifier("_")), $1, $3); }
| exp TSEMICOLON
    { $$ = $1; }
| TARRAY_CREATE simple_exp simple_exp
    %prec prec_app
    { $$ = new NArrayExpression(*$2, *$3); }

fundef:
  id_decl formal_args
    { $$ = new NFundefExpression(*$1, *$2); }

formal_args:
  id_decl
	{ $$ = new std::vector<NLetExpression*>{new NLetExpression(*$1)}; }
| formal_args id_decl
	{ $1->push_back(new NLetExpression(*$2)); }

actual_args:
  simple_exp
    %prec prec_app
    { $$ = new std::vector<NExpression*>{$1}; }
| actual_args simple_exp
    %prec prec_app
	{ $1->push_back($2); }

elems:
  elems TCOMMA exp
    { $1->push_back($3); }
| exp TCOMMA exp
    { $$ = new std::vector<NExpression*>{$1, $3}; }

pat:
  pat TCOMMA id_decl
    { $$->push_back(new NLetExpression(*$3)); }
| id_decl TCOMMA id_decl
    {
	  $$ = new std::vector<NLetExpression*>{
	      new NLetExpression(*$1),
		  new NLetExpression(*$3)};
	}

%%
