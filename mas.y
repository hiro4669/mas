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
    Expression*     expression;
    ArgumentList*   argument_list;
    Statement*      statement;
    IdentifierList* identifier_list;
    Block*          block;
    StatementList*  statement_list;
    Elsif*          elsif;
    ParameterList*  parameter_list;
    FunctionDefinition* function_definition;
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
%token LB
%token RB
%token INCREMENT
%token DECREMENT
%token DOT

%token <identifier>   IDENTIFIER
%token <expression>   INT_LITERAL
%token <expression>   DOUBLE_LITERAL
%token <expression>   STRING_LITERAL

%type <expression> expression 
		logical_and_expression logical_or_expression
		equality_expression relational_expression
		additive_expression multiplicative_expression
		unary_expression primary_expression postfix_expression
                expression_opt

%type <argument_list> argument_list

%type <statement> statement global_statement break_statement 
                continue_statement return_statement
                while_statement for_statement
                if_statement

%type <identifier_list> identifier_list
%type <statement_list> statement_list
%type <block> block
%type <elsif> elsif elsif_list
%type <parameter_list> parameter_list
%type <function_definition> function_definition

%%
translation_unit 
                        : definision_or_statement { printf("accept\n"); }
			| translation_unit definision_or_statement { printf("double accept\n"); }
			;

definision_or_statement 
                        : statement {
                            printf("definision or statement\n"); 
                            MAS_Interpreter* interp;
                            StatementList* stmt_list = NULL;
                            interp = mas_get_interpreter();
                            stmt_list = interp->stmt_list;
                            interp->stmt_list = mas_chain_statement_list(stmt_list, $1);
                                                        
                        }
			| function_definition {
                            MAS_Interpreter* interp;
                            interp = mas_get_interpreter();
                            interp->func_list = mas_chain_function_definition(interp->func_list, $1);
                            printf("function definision\n");
                        }
                        ;

function_definition
			: FUNCTION IDENTIFIER LP parameter_list RP block {
                          $$ = mas_create_function_definition($2, $4, $6);
                        }
			| FUNCTION IDENTIFIER LP                RP block {
                          $$ = mas_create_function_definition($2, NULL, $5);
                        }
			;

parameter_list
			: IDENTIFIER { $$ = mas_create_parameter($1); }
			| parameter_list COMMA IDENTIFIER {
                            $$ = mas_chain_parameter($1, $3);
                        }
			;

statement_list          // this is matched only in block
			: statement { $$ = mas_create_statement_list($1); }
			| statement_list statement { $$ = mas_chain_statement_list($1, $2); }
			;

statement               : expression SEMICOLON { $$ = mas_create_expression_statement($1); } // OK exec OK
			| global_statement    // OK                                          // OK  
			| while_statement     // OK  exec OK
			| return_statement    // OK  exec OK
			| break_statement     // OK  exec OK
			| continue_statement  // OK  exec OK
			| for_statement       // OK  exec OK
			| if_statement        // OK  exec OK
			;
global_statement
			: GLOBAL_T identifier_list SEMICOLON {
                            $$ = mas_create_global_statement($2);
                        }
			;
identifier_list
			: IDENTIFIER { $$ = mas_create_identifier_list($1); }
			| identifier_list COMMA IDENTIFIER {
                            $$ = mas_chain_identifier_list($1, $3);
                        }
			;

expression              : logical_or_expression { 
//                          Expression* expr;
//                          MAS_Interpreter* interp;
//                          expr = $1;
//                          interp = mas_get_interpreter();
//                          interp->expression = expr;    
                        } // OK
                        | postfix_expression ASSIGN expression  { 
                           Expression* expr = mas_create_assignment_expression($1, $3);
  //                         MAS_Interpreter* interp;                            
//                           interp = mas_get_interpreter();
//                           interp->expression = expr;                            
                           $$ = expr; 
                        } // OK --> change
			;
logical_or_expression
			: logical_and_expression
			| logical_or_expression LOGICAL_OR logical_and_expression {
                            $$ = mas_create_binary_expression(LOGICAL_OR_EXPRESSION, $1, $3); // OK // exec OK
                        }
			;
logical_and_expression
			: equality_expression
			| logical_and_expression LOGICAL_AND equality_expression {
                            $$ = mas_create_binary_expression(LOGICAL_AND_EXPRESSION, $1, $3); // OK // exec OK
                        }
			;

equality_expression
			: relational_expression
			| equality_expression EQ relational_expression {
                            $$ = mas_create_binary_expression(EQ_EXPRESSION, $1, $3); // OK // exec OK
                        }
			| equality_expression NE relational_expression {
                            $$ = mas_create_binary_expression(NE_EXPRESSION, $1, $3); // OK // exec OK
                        }
			;
relational_expression
			: additive_expression
			| relational_expression GT additive_expression {
                            $$ = mas_create_binary_expression(GT_EXPRESSION, $1, $3); // OK // exec OK
                        }
			| relational_expression GE additive_expression {
                            $$ = mas_create_binary_expression(GE_EXPRESSION, $1, $3); // OK // exec OK
                        }
			| relational_expression LT additive_expression {
                            $$ = mas_create_binary_expression(LT_EXPRESSION, $1, $3); // OK // exec OK
                        }
			| relational_expression LE additive_expression {
                            $$ = mas_create_binary_expression(LE_EXPRESSION, $1, $3); // OK // exec OK
                        }
			;
additive_expression
			: multiplicative_expression 
			| additive_expression ADD multiplicative_expression { 
                            $$ = mas_create_binary_expression(ADD_EXPRESSION, $1, $3);  // OK  // exec OK
                        }
			| additive_expression SUB multiplicative_expression {
                            $$ = mas_create_binary_expression(SUB_EXPRESSION, $1, $3);  // OK  // exec OK
                        }
			;
multiplicative_expression
			: unary_expression                                              // OK
			| multiplicative_expression MUL unary_expression
                        {
                            $$ = mas_create_binary_expression(MUL_EXPRESSION, $1, $3);  // OK // exec OK
                        }
			| multiplicative_expression DIV unary_expression
                        {
                            $$ = mas_create_binary_expression(DIV_EXPRESSION, $1, $3);  // OK  // exec OK
                        }
			| multiplicative_expression MOD unary_expression
                        {
                            $$ = mas_create_binary_expression(MOD_EXPRESSION, $1, $3);  // OK  // exec OK
                        }
			;
unary_expression
			: postfix_expression                                                  // OK -> change
			| SUB unary_expression { $$ = mas_create_minus_expression($2); }      // OK
			;
argument_list
                        : expression  { $$ = mas_create_argument_list($1); }                  // OK
			| argument_list COMMA expression { $$ = mas_chain_argument($1, $3); } // OK
			;

postfix_expression      : primary_expression
                        | postfix_expression LB expression RB
                        | postfix_expression DOT IDENTIFIER LP argument_list RP
                        | postfix_expression DOT IDENTIFIER LP               RP
                        | postfix_expression INCREMENT
                        | postfix_expression DECREMENT
                        ;

array_literal           : LC expression_list RC
                        | LC expression_list COMMA RC
                        ;

expression_list         : 
                        | expression
                        | expression_list COMMA expression
                        ;


primary_expression
			: IDENTIFIER LP RP { $$ = mas_create_functioncall_expression($1, NULL); }  // OK  // exec half ok
			| IDENTIFIER LP argument_list RP { $$ = mas_create_functioncall_expression($1, $3); } //OK // exec half ok
			| LP expression RP { $$ = $2; }  //OK // exec ok
			| IDENTIFIER       { $$ = mas_create_identifier_expression($1); } //OK
			| INT_LITERAL     //OK // exec ok
			| DOUBLE_LITERAL  //OK // exec ok
			| STRING_LITERAL  //OK // exec ok
			| TRUE_T           { $$ = mas_create_boolean_expression(MAS_TRUE); }  //OK // exec ok
			| FALSE_T          { $$ = mas_create_boolean_expression(MAS_FALSE); } //OK // exec ok
			| NULL_T           { $$ = mas_create_null_expression(); }  //OK  // exec ok
			;

while_statement
			: WHILE LP expression RP block {
                            $$ = mas_create_while_statement($3, $5); // OK
                        }
                        ;
block     : LC statement_list RC {$$ = mas_create_block($2);}  // OK
	  | LC                RC {$$ = mas_create_block(NULL);}// OK
	  ;
expression_opt
			: {$$ = NULL;}  // OK
			| expression    // OK
			;
return_statement
			: RETURN_T expression_opt SEMICOLON {
                            $$ = mas_create_return_statement($2);
                        }
			;
break_statement
			: BREAK SEMICOLON {
                            $$ = mas_create_break_statement();
                        }
			;
continue_statement
			: CONTINUE SEMICOLON {
                            $$ = mas_create_continue_statement();
                        }
			;
                        
for_statement
			: FOR LP expression_opt SEMICOLON expression_opt SEMICOLON expression_opt RP block {
                            $$ = mas_create_for_statement($3, $5, $7, $9);
                        }
			;

if_statement
			: IF LP expression RP block { $$ = mas_create_if_statement($3, $5, NULL, NULL); }
			| IF LP expression RP block ELSE block { $$ = mas_create_if_statement($3, $5, NULL, $7); }
			| IF LP expression RP block elsif_list { $$ = mas_create_if_statement($3, $5, $6, NULL); }
			| IF LP expression RP block elsif_list ELSE block { $$ = mas_create_if_statement($3, $5, $6, $8); }
			;

elsif_list         : elsif
		   | elsif_list elsif { $$ = mas_chain_elsif($1, $2); }
		   ;

elsif	           : ELSIF LP expression RP block {
                     $$ = mas_create_elsif($3, $5);
                   }
		   ;

%%
int
yyerror(char const *str)
{
    extern char *yytext;
    fprintf(stderr, "parser error near %s\n", yytext);
    return 0;
}



