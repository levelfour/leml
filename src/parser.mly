%{
let addtyp x = (x, Type.gentyp ())
%}

%token <bool> BOOL
%token <int> INT
%token <float> FLOAT
%token NOT
%token AST
%token SLASH
%token MINUS
%token PLUS
%token MINUS_DOT
%token PLUS_DOT
%token AST_DOT
%token SLASH_DOT
%token EQUAL
%token LESS_GREATER
%token LESS_EQUAL
%token GREATER_EQUAL
%token LESS
%token GREATER
%token IF
%token THEN
%token ELSE
%token <Id.t> IDENT
%token LET
%token IN
%token REC
%token COMMA
%token ARRAY_CREATE
%token DOT
%token LESS_MINUS
%token SEMICOLON
%token LPAREN
%token RPAREN
%token EOF

%right prec_let
%right SEMICOLON
%right prec_if
%right LESS_MINUS
%left COMMA
%left EQUAL LESS_GREATER LESS GREATER LESS_EQUAL GREATER_EQUAL
%left PLUS MINUS PLUS_DOT MINUS_DOT
%left AST SLASH AST_DOT SLASH_DOT
%right prec_unary_minus
%left prec_app
%left DOT

%type <Syntax.t> exp
%start exp

%%

simple_exp:
| LPAREN exp RPAREN
    { ($2) }
| LPAREN RPAREN
    { Syntax.Unit }
| BOOL
    { Syntax.Bool($1) }
| INT
    { Syntax.Int($1) }
| FLOAT
    { Syntax.Float($1) }
| IDENT
    { Syntax.Var($1) }
| simple_exp DOT LPAREN exp RPAREN
    { Syntax.Get($1, $4) }

exp:
| simple_exp
    { $1 }
| NOT exp
    %prec prec_app
    { Syntax.Not($2) }
| MINUS exp
    %prec prec_unary_minus
    { match $2 with
    | Syntax.Float(f) -> Syntax.Float(-.f)
    | e -> Syntax.Neg($2) }
| exp PLUS exp
    { Syntax.Add($1, $3) }
| exp MINUS exp
    { Syntax.Sub($1, $3) }
| exp EQUAL exp
    { Syntax.Eq($1, $3) }
| exp LESS_GREATER exp
    { Syntax.Not(Syntax.Eq($1, $3)) }
| exp LESS exp
    { Syntax.Not(Syntax.LE($3, $1)) }
| exp GREATER exp
    { Syntax.Not(Syntax.LE($1, $3)) }
| exp LESS_EQUAL exp
    { Syntax.LE($1, $3) }
| exp GREATER_EQUAL exp
    { Syntax.LE($3, $1) }
| IF exp THEN exp ELSE exp
    %prec prec_if
    { Syntax.If($2, $4, $6) }
| MINUS_DOT exp
    %prec prec_unary_minus
    { Syntax.FNeg($2) }
| exp PLUS_DOT exp
    { Syntax.FAdd($1, $3) }
| exp MINUS_DOT exp
    { Syntax.FSub($1, $3) }
| exp AST_DOT exp
    { Syntax.FMul($1, $3) }
| exp SLASH_DOT exp
    { Syntax.FDiv($1, $3) }
| LET IDENT EQUAL exp IN exp
    %prec prec_let
    { Syntax.Let(addtyp $2, $4, $6) }
| LET REC fundef IN exp
    %prec prec_let
    { Syntax.LetRec($3, $5) }
| exp actual_args
    %prec prec_app
    { Syntax.App($1, $2) }
| elems
    { Syntax.Tuple($1) }
| LET LPAREN pat RPAREN EQUAL exp IN exp
    { Syntax.LetTuple($3, $6, $8) }
| simple_exp DOT LPAREN exp RPAREN LESS_MINUS exp
    { Syntax.Put($1, $4, $7) }
| exp SEMICOLON exp
    { Syntax.Let((Id.gentmp Type.Unit, Type.Unit), $1, $3) }
| exp SEMICOLON
    { $1 }
| ARRAY_CREATE simple_exp simple_exp
    %prec prec_app
    { Syntax.Array($2, $3) }
| error
    { failwith "parser error" }

fundef:
| IDENT formal_args EQUAL exp
    { { Syntax.name = addtyp $1; Syntax.args = $2; Syntax.body = $4 } }

formal_args:
| IDENT formal_args
    { (addtyp $1 :: $2) }
| IDENT
    { [addtyp $1] }

actual_args:
| actual_args simple_exp
    %prec prec_app
    { $1 @ [$2] }
| simple_exp
    %prec prec_app
    { [$1] }

elems:
| elems COMMA exp
    { ($1 @ [$3]) }
| exp COMMA exp
    { [$1; $3] }

pat:
| pat COMMA IDENT
    { ($1 @ [addtyp $3]) }
| IDENT COMMA IDENT
    { [addtyp $1; addtyp $3] }
