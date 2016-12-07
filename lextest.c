#include <stdio.h>
#include "y.tab.h"

int main(void) {
    extern FILE *yyin;
    extern int yylex(void);
    int i;
    int t_type;

    yyin = fopen("rtest.ma", "r");
    if (yyin == NULL) {
        fprintf(stderr, "cannot open file\n");
    }
    //	for (i = 0; i < 3; ++i) {
    while (1) {
        t_type = yylex();
        switch (t_type) {
            case FUNCTION: {
                printf("FUNCTION: function\n");
                break;
            }
            case IF: {
                printf("IF: if\n");
                break;
            }
            case ELSE: {
                printf("ELSE: else\n");
                break;
            }
            case ELSIF: { 
                printf("ELSIF: elsif\n");
                break;
            }
            case WHILE: {
                printf("WHILE: while\n");
                break;
            }
            case FOR: {
                printf("FOR: for\n");
                break;
            }
            case RETURN_T: {
                printf("RETURN_T: return\n");
                break;
            }
            case BREAK: {
                printf("BREAK: break\n");
                break;
            }
            case CONTINUE: {
                printf("CONTINUE: continue\n");
                break;
            }
            case NULL_T: {
                printf("NULL_T: null\n");
                break;
            }
            case TRUE_T: {
                printf("TRUE_T: true\n");
                break;
            }
            case FALSE_T: {
                printf("FALSE_T: false\n");
                break;
            }
            case GLOBAL_T: {
                printf("GLOBAL_T: global_t\n");
                break;
            }
            case LP: {
                printf("LP: ( \n");
                break;
            }
            case RP: {
                printf("RP: )\n");
                break;
            }
            case LC: {
                printf("LC: {\n");
                break;
            }
            case RC: {
                printf("RC: }\n");
                break;
            }
            case SEMICOLON: {
                printf("SEMICOLON: ;\n");
                break;
            }
            case COMMA: {
                printf("COMMA: comma\n");
                break;
            }
            case LOGICAL_AND: {
                printf("LOGICAL_AND: &&\n");
                break;
            }
            case LOGICAL_OR: {
                printf("LOGICAL_OR: ||\n");
                break;
            }
            case ASSIGN: {
                printf("ASSIGN: ==\n");
                break;
            }
            case EQ: {
                printf("EQ: ==\n");
                break;
            }
            case NE: {
                printf("EQ: !=\n");
                break;
            }
            case GT: {
                printf("GT: >\n");
                break;
            }
            case GE: {
                printf("GE: >=\n");
                break;
            }
            case LT: {
                printf("LT: <\n");
                break;
            }
            case LE: {
                printf("LE: <=\n");
                break;
            }
            case ADD: {
                printf("ADD: +\n");
                break;
            }
            case SUB: {
                printf("SUB: -\n");
                break;
            }
            case MUL: {
                printf("MUL: *\n");
                break;
            }
            case DIV: {
                printf("DIV: /\n");
                break;
            }
            case MOD: {
                printf("MOD: %% \n");
                break;
            }
            case IDENTIFIER: {
                printf("IDENTIFIER: %s \n", yylval.identifier);
                break;
            }
            case INT_LITERAL: {
                printf("INT_LITERAL: %d \n", yylval.int_value);
                break;
            }
            case DOUBLE_LITERAL: {
                printf("DOUBLE_LITERAL: %lf \n", yylval.double_value);
                break;
            }
            case STRING_LITERAL: {
                printf("STRING_LITERAL: \"%s \"\n", yylval.identifier);
                break;
            }
            case EOF: {
                printf("EOF break\n");
                goto end;
                break;
            }
            default:
                printf("cannot understand token kind\n");
                break;
        }
    }
end:
    return 0;

}
