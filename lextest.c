#include <stdio.h>
#include "y.tab.h"

int main(void) {
    extern FILE *yyin;
    extern int yylex(void);
    int i;
    int t_type;

    yyin = fopen("test.ma", "r");
    if (yyin == NULL) {
        fprintf(stderr, "cannot open file\n");
    }
    //	for (i = 0; i < 3; ++i) {
    while (1) {
        t_type = yylex();
        //		printf("t_type = %d\n", t_type);
        switch (t_type) {
            case LP: {
                printf("LP: ( \n");
                break;
            }
            case RP: {
                printf("RP: )\n");
                break;
            }
            case IDENTIFIER: {
                printf("IDENTIFIER: %s \n", yylval.identifier);
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
                break;
        }
    }
end:
    return 0;

}
