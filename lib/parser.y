%{
    /* definitions */
    #define YYDEBUG 1
%}

%code requires {
    #include "../../include/Graph.h"
    #include<iostream>
    #include <ibex.h>

    extern FILE *yyin;
    extern int yydebug;

    int yylex(Graph *graph);
    int yyerror(Graph *graph, char *s);


    struct Number {
        enum { INT, FP } type;
        union {
            int ival;
            double fval;
        };
    };
    typedef struct Number Number;
}

%param { Graph *graph }

%union {
    Number num;
    Node::RoundingType rnd_type;
    char *str;
    Node *node;
}

%token EOL
%token<num> INT
%token<num> FP

%type<node> number

%token<rnd_type> FPTYPE
%token<rnd_type> INTTYPE
%token ADD SUB MUL DIV
%token EQ NEQ LEQ LT GEQ GT
%token AND OR NOT
%token INPUTS OUTPUTS CONSTRAINTS EXPRS
%token IF THEN ELSE ENDIF
%token SIN COS TAN
%token ASIN ACOS ATAN
%token SINH COSH TANH
%token SQRT EXP LOG
%token FMA
%token LBRACE RBRACE LPAREN RPAREN LBRACKET RBRACKET
%token COMMA COLON SEMICOLON ASSIGN
%token<str> ID

%type<node> arith_exp arith_term arith_fact interval assign_exp;
%type<num> intv_expr intv_term intv_factor;

/* rules */
%%

program: inputs EOL outputs EOL constraints EOL exprs
	    | inputs EOL outputs EOL constraints EOL exprs EOL
        | inputs EOL outputs EOL exprs
        | inputs EOL outputs EOL exprs EOL
        | inputs { std::cout << "Only LLVM programs can be parsed with INPUTS "
                                "and OUTPUTS sections" << std::endl; }
        | inputs EOL{ std::cout << "Only LLVM programs can be parsed with INPUTS "
                                   "and OUTPUTS sections" << std::endl; }
        ;

intv_factor:    SUB FP {
			$$ = $2;
			$$.fval = -$2.fval;
		}
		| SUB INT {
            $$ = $2;
            $$.ival = -$2.ival;
        }
		| FP { $$ = $1; }
		| INT { $$ = $1; }
        ;

intv_term:  intv_factor { $$ = $1; }
        | intv_term MUL intv_factor {
            $$ = $1;
            $$.fval = $1.fval * $3.fval; }
        | intv_term DIV intv_factor {
            $$ = $1;
            $$.fval = $1.fval / $3.fval; }
        ;

intv_expr:  intv_term { $$ = $1; }
        | intv_expr ADD intv_term {
            $$ = $1;
            $$.fval = $1.fval + $3.fval;
        }
        | intv_expr SUB intv_term {
            $$ = $1;
            $$.fval = $1.fval - $3.fval;
        }
        ;

interval:   ID FPTYPE COLON LPAREN intv_expr COMMA intv_expr RPAREN SEMICOLON {
                VariableNode *new_variable;
                if(Node *FreeVarNode = graph->findFreeVarNode($1)) {

                } else {
                    new_variable = new VariableNode($2);
                    graph->nodes.insert(new_variable);
                    // graph->depthTable[new_variable->depth].insert(new_variable);
                    graph->symbolTables[graph->currentScope]->insert($1, new_variable);
                }
                $$ = graph->inputs[$1] = new FreeVariable(*new ibex::Interval($5.fval, $7.fval), $2);
                graph->nodes.insert($$);

                // std::cout << *graph << std::endl;
            }
            | ID INTTYPE COLON LPAREN intv_expr COMMA intv_expr RPAREN SEMICOLON {
                VariableNode *new_variable;
                if(Node *FreeVarNode = graph->findFreeVarNode($1)) {

                } else {
                    new_variable = new VariableNode($2);
                    graph->nodes.insert(new_variable);
                    // graph->depthTable[new_variable->depth].insert(new_variable);
                    graph->symbolTables[graph->currentScope]->insert($1, new_variable);
                }
                $$ = graph->inputs[$1] = new FreeVariable(*new ibex::Interval($5.fval, $7.fval), $2);
                graph->nodes.insert($$);

                // std::cout << *graph << std::endl;
            }
            | EOL {  }
            ;

interval_list: interval_list interval { }
        | interval { }
        ;

inputs: INPUTS LBRACE interval_list RBRACE { }
        | EOL { }
        ;

output: ID SEMICOLON {
            graph->outputs.push_back($1);
        }
        | EOL
        ;

output_list:    output_list output
        | output
        ;

outputs: OUTPUTS LBRACE output_list RBRACE
        | EOL
        ;

cond_term:  LPAREN cond_expr RPAREN
        | arith_exp EQ arith_exp
        | arith_exp NEQ arith_exp
        | arith_exp LEQ arith_exp
        | arith_exp LT arith_exp
        | arith_exp GEQ arith_exp
        | arith_exp GT arith_exp
        ;

cond_expr:  cond_term
        | cond_expr AND cond_term
        | cond_expr OR cond_term
        ;

assign_constraint_expr: ID COLON cond_expr SEMICOLON
                        | EOL
                        ;

constraint_list:    constraint_list assign_constraint_expr
                | assign_constraint_expr
                ;

constraints:    CONSTRAINTS LBRACE constraint_list RBRACE
            | EOL
            ;

exprs:  EXPRS LBRACE stmts RBRACE
        ;

number: INT {
            $$ = new Integer($1.ival);
            graph->nodes.insert($$);
            // std::cout << *$$ << std::endl;
        }
        | FP {
            $$ = new Double($1.fval);
            graph->nodes.insert($$);
            // std::cout << *$$ << std::endl;
        }
        ;


arith_fact: number { $$ = $1; }
        | SIN LPAREN arith_exp RPAREN {
            $$ = &sin(*$3);
            graph->nodes.insert($$);
            graph->depthTable[$$->depth].insert($$);
            graph->numOperatorsOutput++;
            // std::cout << *$$ << std::endl;
        }
        | COS LPAREN arith_exp RPAREN {
            $$ = &cos(*$3);
            graph->nodes.insert($$);
            graph->depthTable[$$->depth].insert($$);
            graph->numOperatorsOutput++;
            // std::cout << *$$ << std::endl;
        }
        | TAN LPAREN arith_exp RPAREN {
            $$ = &tan(*$3);
            graph->nodes.insert($$);
            graph->depthTable[$$->depth].insert($$);
            graph->numOperatorsOutput++;
            // std::cout << *$$ << std::endl;
        }
        | SINH LPAREN arith_exp RPAREN {
            $$ = &sinh(*$3);
            graph->nodes.insert($$);
            graph->depthTable[$$->depth].insert($$);
            graph->numOperatorsOutput++;
            // std::cout << *$$ << std::endl;
        }
        | COSH LPAREN arith_exp RPAREN {
            $$ = &cosh(*$3);
            graph->nodes.insert($$);
            graph->depthTable[$$->depth].insert($$);
            graph->numOperatorsOutput++;
            // std::cout << *$$ << std::endl;
        }
        | TANH LPAREN arith_exp RPAREN {
            $$ = &tanh(*$3);
            graph->nodes.insert($$);
            graph->depthTable[$$->depth].insert($$);
            graph->numOperatorsOutput++;
            // std::cout << *$$ << std::endl;
        }
        | ASIN LPAREN arith_exp RPAREN {
            $$ = &asin(*$3);
            graph->nodes.insert($$);
            graph->depthTable[$$->depth].insert($$);
            graph->numOperatorsOutput++;
            // std::cout << *$$ << std::endl;
        }
        | ACOS LPAREN arith_exp RPAREN {
            $$ = &acos(*$3);
            graph->nodes.insert($$);
            graph->depthTable[$$->depth].insert($$);
            graph->numOperatorsOutput++;
            // std::cout << *$$ << std::endl;
        }
        | ATAN LPAREN arith_exp RPAREN {
            $$ = &atan(*$3);
            graph->nodes.insert($$);
            graph->depthTable[$$->depth].insert($$);
            graph->numOperatorsOutput++;
            // std::cout << *$$ << std::endl;
        }
        | LOG LPAREN arith_exp RPAREN {
            $$ = &log(*$3);
            graph->nodes.insert($$);
            graph->depthTable[$$->depth].insert($$);
            graph->numOperatorsOutput++;
            // std::cout << *$$ << std::endl;
        }
        | SQRT LPAREN arith_exp RPAREN {
            $$ = &sqrt(*$3);
            graph->nodes.insert($$);
            graph->depthTable[$$->depth].insert($$);
            graph->numOperatorsOutput++;
            // std::cout << *$$ << std::endl;
        }
        | EXP LPAREN arith_exp RPAREN {
            $$ = &exp(*$3);
            graph->nodes.insert($$);
            graph->depthTable[$$->depth].insert($$);
            graph->numOperatorsOutput++;
            // std::cout << *$$ << std::endl;
        }
        | FMA LPAREN arith_exp COMMA arith_exp COMMA arith_exp RPAREN {
            $$ = &fma(*$3, *$5, *$7);
            graph->nodes.insert($$);
            graph->depthTable[$$->depth].insert($$);
            graph->numOperatorsOutput++;
            // std::cout << *$$ << std::endl;
        }
	    | SUB number {
            $$ = &(-*$2);
		    graph->nodes.insert($$);
		    // graph->depthTable[$$->depth].insert($$);
		    graph->numOperatorsOutput++;
		    // std::cout << *$$ << std::endl;
        }
	    | LPAREN arith_exp RPAREN { $$ = $2; }
        | ID {
            if(Node *VarNode = graph->findVarNode($1)) {
                $$ = graph->symbolTables[graph->currentScope]->table[$1];
            } else {
                std::cout << "ERROR: Variable " << $1 << " not found" << std::endl;
            }
            // std::cout << *$$ << std::endl;
        }
        ;

arith_term: arith_fact {
                $$ = $1;
            }
            | arith_term MUL arith_fact {
                $$ = &(*$1*$3);
                graph->nodes.insert($$);
                graph->depthTable[$$->depth].insert($$);
                graph->numOperatorsOutput++;
                // std::cout << *$$ << std::endl;
            }
            | arith_term DIV arith_fact {
                $$ = &(*$1/$3);
                graph->nodes.insert($$);
                graph->depthTable[$$->depth].insert($$);
                graph->numOperatorsOutput++;
                // std::cout << *$$ << std::endl;
            }
            ;

arith_exp:  arith_term {
                $$ = $1;
            }
            | arith_exp ADD arith_term {
                $$ = &(*$1+$3);
                graph->nodes.insert($$);
                graph->depthTable[$$->depth].insert($$);
                graph->numOperatorsOutput++;
                // std::cout << *$$ << std::endl;
            }
            | arith_exp SUB arith_term {
                $$ = &(*$1-$3);
                graph->nodes.insert($$);
                graph->depthTable[$$->depth].insert($$);
                graph->numOperatorsOutput++;
                // std::cout << *$$ << std::endl;
            }
            ;

assign_exp: ID ASSIGN arith_exp SEMICOLON {
                $$ = $3;
                graph->symbolTables[graph->currentScope]->insert($1, $3);
                // std::cout << *$$ << std::endl;
            }
            | ID FPTYPE ASSIGN arith_exp SEMICOLON {
                $$ = $4;
                graph->symbolTables[graph->currentScope]->insert($1, $4);
                $$->setRoundingType($2);
                $$->setRoundingFromType($2);
                // std::cout << *$$ << std::endl;
            }
            | EOL {
                $$ = nullptr;
            }
            ;

if_block: IF cond_expr THEN stmts ELSE stmts ENDIF
        | IF cond_expr THEN stmts ENDIF
        ;

stmts:  stmts assign_exp
        | stmts if_block
        | assign_exp
        | if_block
        ;


%%


int yyerror(Graph *graph, char *s) {
    std::cout << "ERROR: " << s << std::endl;

    return 0;
}