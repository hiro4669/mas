
#include "mas.h"
#include "./memory/MEM.h"

static MAS_String* alloc_mas_string(MAS_Interpreter* interp, char* str, MAS_Boolean is_literal) {
    MAS_String* ret;
    ret = (MAS_String*)MEM_malloc(sizeof(MAS_String));
    ret->string = str;
    ret->ref_count = 0;
    ret->is_literal = is_literal;
    return ret;
}

MAS_String* mas_literal_to_mas_string(MAS_Interpreter* interp, char* str) {
    MAS_String* ret;
    ret = alloc_mas_string(interp, str, MAS_TRUE);
    ret->ref_count++;
    return ret;
}

MAS_String* mas_create_mas_string(MAS_Interpreter* interp, char* str) {
    MAS_String* ret;
    ret = alloc_mas_string(interp, str, MAS_FALSE);
    ret->ref_count++;
    return ret;
}

void mas_refer_string(MAS_String* str) {
    ++str->ref_count;
//    printf("refer string--> %d\n", str->ref_count);
}

void mas_release_string(MAS_String* str) {
    
    --str->ref_count;
//    printf("release string--> %d\n", str->ref_count);
    if (str->ref_count == 0) {
        if (!str->is_literal) {
            MEM_free(str->string);
        }
        MEM_free(str);
    }
}