#include <stdio.h>
#include "mas.h"
#include "./memory/MEM.h"

int main(void) {
    MAS_Interpreter *interp;
    
    interp = mas_create_interpreter();
    interp->execution_storage = MEM_open_storage(0);
    MAS_Value mv;
    printf("variabletest\n");
    mv.type = MAS_INT_VALUE;
    mv.u.int_value = 10;
    MAS_add_global_variable(interp, "test", &mv);
    
    
    Variable* gv = MAS_search_global_variable(interp, "test");
    if (gv != NULL) {
        printf("not null int value = %d\n", gv->value.u.int_value); 
    } else {
        printf("null!\n");
    }
    
    LocalEnvironment *env = (LocalEnvironment*)MEM_malloc(sizeof(LocalEnvironment));
    env->variable = NULL;
    env->global_variable = NULL;

    
    MAS_Value lmv;
    lmv.type = MAS_DOUBLE_VALUE;
    lmv.u.double_value = 20.0;
    MAS_add_local_variable(env, "localv", &lmv);
    
    Variable* lv = MAS_search_local_variable(env, "localv");
    
    if (lv != NULL) {
        printf("local variable not null : %f\n", lv->value.u.double_value);
    } else {
        printf("local variable null\n");
    }

    
    return 0;
}