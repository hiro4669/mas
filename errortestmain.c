#include "mas.h"

int main(void) {    
    mas_compile_error(PARSE_ERR, 
            INT_MESSAGE_ARGUMENT, "token", 10, 
            STRING_MESSAGE_ARGUMENT, "test", "hoge", 
            MESSAGE_ARGUMENT_END);
    
    mas_compile_error(FUNCTION_MULTIPLE_DEFINE_ERR,
            STRING_MESSAGE_ARGUMENT, "name", "ftest", 
            MESSAGE_ARGUMENT_END);
    
    MEM_dump_memory(); // check memory
    return 0;
}
