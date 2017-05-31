#include <stdio.h>


#include "mas.h"

StatementResult mas_execute_statementlist(MAS_Interpreter *interp,
        LocalEnvironment *env, StatementList *stmt_lise) {
    StatementResult result;
    result.type = NORMAL_STATEMENT_RESULT;
    result.u.return_value.type = MAS_NULL_VALUE;
    
        
    printf("execute statement list\n");
    
    MAS_Value v1, v2, v3, v4;
    v1.type = MAS_BOOLEAN_VALUE;
    v2.type = MAS_INT_VALUE;
    v3.type = MAS_STRING_VALUE;
    v4.type = MAS_DOUBLE_VALUE;
    
    push_value(interp, &v1);
    push_value(interp, &v2);
    push_value(interp, &v3);
    push_value(interp, &v4);
    
    
    MAS_Value v;
    for (int i = 0; i < 0; ++i) {
        v = pop_value(interp);
        fprintf(stderr, "type = %d\n", v.type);
    }
    
    fprintf(stderr, "type = %d\n", peek_stack(interp, 1)->type);
    
    return result;    
}
