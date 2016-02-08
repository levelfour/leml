%{
#include <iostream>
#include <string>
#include <cassert>
#include "syntax.hh"
#include "parser.hh"

#define TOKEN(t) (yylval.op = t)
#define SAVE_TOKEN yylval.token = new std::string(yytext, yyleng)
%}

%option noyywrap

%x ML_COMMENT

%%
[ \t]+
[\n\r]
"(*"    { BEGIN(ML_COMMENT); }
<ML_COMMENT>"*)" { BEGIN(INITIAL); }
<ML_COMMENT>"\n" {}
<ML_COMMENT>. {}
"("     { return TOKEN(TLPAREN); }
")"     { return TOKEN(TRPAREN); }
true    { SAVE_TOKEN; return TBOOL; }
false   { SAVE_TOKEN; return TBOOL; }
not     { return TOKEN(TNOT); }
[0-9]+  { SAVE_TOKEN; return TINT; }
[0-9]+(\.[0-9]*)?([eE][+-][0-9]+)? { SAVE_TOKEN; return TFLOAT; }
"-"     { return TOKEN(TMINUS); }
"+"     { return TOKEN(TPLUS); }
"*"     { return TOKEN(TAST); }
"/"     { return TOKEN(TSLASH); }
"-."    { return TOKEN(TMINUS_DOT); }
"+."    { return TOKEN(TPLUS_DOT); }
"*."    { return TOKEN(TAST_DOT); }
"/."    { return TOKEN(TSLASH_DOT); }
"="     { return TOKEN(TEQUAL); }
"<>"    { return TOKEN(TLESS_GREATER); }
"<="    { return TOKEN(TLESS_EQUAL); }
">="    { return TOKEN(TGREATER_EQUAL); }
"<"     { return TOKEN(TLESS); }
">"     { return TOKEN(TGREATER); }
if      { return TOKEN(TIF); }
then    { return TOKEN(TTHEN); }
else    { return TOKEN(TELSE); }
let     { return TOKEN(TLET); }
in      { return TOKEN(TIN); }
rec     { return TOKEN(TREC); }
","     { return TOKEN(TCOMMA); }
"_"     { SAVE_TOKEN; return TIDENT; }
Array\.create   { return TOKEN(TARRAY_CREATE); }
create_array    { return TOKEN(TARRAY_CREATE); }
"."     { return TOKEN(TDOT); }
"<-"    { return TOKEN(TLESS_MINUS); }
";"     { return TOKEN(TSEMICOLON); }
[a-z][0-9a-zA-Z_']* { SAVE_TOKEN; return TIDENT; }
<<EOF>> { yyterminate(); }
.       { std::cerr << "unknown token\n"; assert(false); }
%%
