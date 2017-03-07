%{
#include <stdio.h>
#include "mas.h"
#define YYDEBUG 1
%}
%union {
    /*
	int int_value;
	double double_value;
     */
    char *identifier;
    Expression* expression;
    ArgumentList* argument_list;
}

%token FUNCTION
%token IF
%token ELSE
%token ELSIF
%token WHILE
%token FOR
%token RETURN_T
%token BREAK
%token CONTINUE
%token NULL_T
%token TRUE_T
%token FALSE_T
%token GLOBAL_T
%token LP
%token RP
%token LC
%token RC
%token SEMICOLON
%token COMMA
%token LOGICAL_AND
%token LOGICAL_OR
%token ASSIGN
%token EQ
%token NE
%token GT
%token GE
%token LT
%token LE
%token ADD
%token SUB
%token MUL
%token DIV
%token MOD

%token <identifier>   IDENTIFIER
%token <expression>   INT_LITERAL
%token <expression>   DOUBLE_LITERAL
%token <expression>   STRING_LITERAL

%type <expression> expression 
		logical_and_expression logical_or_expression
		equality_expression relational_expression
		additive_expression multiplicative_expression
		unary_expression primary_expression

%type <argument_list> argument_list


%%
translation_unit 
                        : definision_or_statement { printf("accept\n"); }
			| translation_unit definision_or_statement { printf("double accept\n"); }
			;

definision_or_statement 
            : statement
			| function_definition
            ;

function_definition
			: FUNCTION IDENTIFIER LP parameter_list RP block
			| FUNCTION IDENTIFIER LP                RP block
			;

parameter_list
			: IDENTIFIER
			| parameter_list COMMA IDENTIFIER
			;

statement_list
			: statement
			| statement_list statement
			;

statement   : expression SEMICOLON { printf("expr statement\n"); }
			| global_statement
			| while_statement 
			| return_statement
			| break_statement
			| continue_statement
			| for_statement /*{ printf("accept for "); } */
			| if_statement
			;
global_statement
			: GLOBAL_T identifier_list SEMICOLON 
			;
identifier_list
			: IDENTIFIER
			| identifier_list COMMA IDENTIFIER
			;

expression              : logical_or_expression { 
    printf("expression\n");
    Expression* expr;
    MAS_Interpreter* interp;
    expr = $1;
    interp = mas_get_interpreter();
    interp->expression = expr;
    if (expr) {
        printf("type = %d\n", expr->type);
    } else {
        printf("no expr\n");
    }
}
                        | IDENTIFIER ASSIGN expression  { 
                            Expression* expr = mas_create_assignment_expression($1, $3);
                            MAS_Interpreter* interp;                            
                            interp = mas_get_interpreter();
                            interp->expression = expr;                            
                            $$ = expr; 
                        }
			;
logical_or_expression
			: logical_and_expression
			| logical_or_expression LOGICAL_OR logical_and_expression {
                            $$ = mas_create_binary_expression(LOGICAL_OR_EXPRESSION, $1, $3); // OK
                        }
			;
logical_and_expression
			: equality_expression
			| logical_and_expression LOGICAL_AND equality_expression {
                            $$ = mas_create_binary_expression(LOGICAL_AND_EXPRESSION, $1, $3); // OK
                        }
			;

equality_expression
			: relational_expression
			| equality_expression EQ relational_expression {
                            $$ = mas_create_binary_expression(EQ_EXPRESSION, $1, $3); // OK
                        }
			| equality_expression NE relational_expression {
                            $$ = mas_create_binary_expression(NE_EXPRESSION, $1, $3); // OK
                        }
			;
relational_expression
			: additive_expression
			| relational_expression GT additive_expression {
                            $$ = mas_create_binary_expression(GT_EXPRESSION, $1, $3); // OK
                        }
			| relational_expression GE additive_expression {
                            $$ = mas_create_binary_expression(GE_EXPRESSION, $1, $3); // OK
                        }
			| relational_expression LT additive_expression {
                            $$ = mas_create_binary_expression(LT_EXPRESSION, $1, $3); // OK
                        }
			| relational_expression LE additive_expression {
                            $$ = mas_create_binary_expression(LE_EXPRESSION, $1, $3); // OK
                        }
			;
additive_expression
			: multiplicative_expression 
			| additive_expression ADD multiplicative_expression { 
                            $$ = mas_create_binary_expression(ADD_EXPRESSION, $1, $3);  // OK
                        }
			| additive_expression SUB multiplicative_expression {
                            $$ = mas_create_binary_expression(SUB_EXPRESSION, $1, $3);  // OK
                        }
			;
multiplicative_expression
			: unary_expression                                              // OK
			| multiplicative_expression MUL unary_expression
                        {
                            $$ = mas_create_binary_expression(MUL_EXPRESSION, $1, $3);  // OK
                        }
			| multiplicative_expression DIV unary_expression
                        {
                            $$ = mas_create_binary_expression(DIV_EXPRESSION, $1, $3);  // OK
                        }
			| multiplicative_expression MOD unary_expression
                        {
                            $$ = mas_create_binary_expression(MOD_EXPRESSION, $1, $3);  // OK
                        }
			;
unary_expression
			: primary_expression                                                  // OK
			| SUB unary_expression { $$ = mas_create_minus_expression($2); }      // OK
			;
argument_list
                        : expression  { $$ = mas_create_argument_list($1); }                  // OK
			| argument_list COMMA expression { $$ = mas_chain_argument($1, $3); } // OK
			;
primary_expression
			: IDENTIFIER LP RP { $$ = mas_create_functioncall_expression($1, NULL); }  //OK
			| IDENTIFIER LP argument_list RP { $$ = mas_create_functioncall_expression($1, $3); } //OK
			| LP expression RP { $$ = $2; }  //OK
			| IDENTIFIER       { $$ = mas_create_identifier_expression($1); } //OK
			| INT_LITERAL     //OK
			| DOUBLE_LITERAL  //OK
			| STRING_LITERAL  //OK
			| TRUE_T           { $$ = mas_create_boolean_expression(MAS_TRUE); }  //OK
			| FALSE_T          { $$ = mas_create_boolean_expression(MAS_FALSE); } //OK
			| NULL_T           { $$ = mas_create_null_expression(); }  //OK
			;

while_statement
			: WHILE LP expression RP block

block : LC statement_list RC
	  | LC RC
	  ;
expression_opt
			: /* empty */ 
			| expression
			;
return_statement
			: RETURN_T expression_opt SEMICOLON
			;
break_statement
			: BREAK SEMICOLON
			;
continue_statement
			: CONTINUE SEMICOLON
			;

for_statement
			: FOR LP expression_opt SEMICOLON expression_opt SEMICOLON expression_opt RP block
			;

if_statement
			: IF LP expression RP block { printf("if match "); }
			| IF LP expression RP block ELSE block { printf("if match 2 "); }
			| IF LP expression RP block elsif_list { printf("if match 3 "); }
			| IF LP expression RP block elsif_list ELSE block { printf("if match 4 "); }
			;

elsif_list : elsif
		   | elsif_list elsif
		   ;

elsif	   : ELSIF LP expression RP block
		   ;

%%
int
yyerror(char const *str)
{
    extern char *yytext;
    fprintf(stderr, "parser error near %s\n", yytext);
    return 0;
}



