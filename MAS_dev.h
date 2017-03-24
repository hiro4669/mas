
#ifndef _MAS_DEV_H_
#define _MAS_DEV_H_

typedef struct MAS_Interpreter_tag MAS_Interpreter;
typedef struct MAS_String_tag MAS_String;

typedef enum {
    MAS_FALSE = 0,
    MAS_TRUE    
} MAS_Boolean;

struct MAS_String_tag {
    int         ref_count;
    char*       string;
    MAS_Boolean is_literal;
};

typedef struct {
    char* name;
} MAS_NativePointerInfo;

typedef struct {
    MAS_NativePointerInfo* info;
    void*                  pointer;    
} MAS_NativePointer;

typedef enum {
    MAS_BOOLEAN_VALUE = 1,
    MAS_INT_VALUE,
    MAS_DOUBLE_VALUE,
    MAS_STRING_VALUE,
    MAS_NATIVE_POINTER_VALUE,
    MAS_NULL_VALUE
} MAS_ValueType;


typedef struct {
    MAS_ValueType type;
    union {
        MAS_Boolean        boolean_value;
        int                int_value;
        double             double_value;
        MAS_String*        string_value;
        MAS_NativePointer* native_pointer;
    } u;    
} MAS_Value;

typedef MAS_Value (*MAS_NativeFunctionProc)(MAS_Interpreter* interp, int arg_count,
        MAS_Value* args);


#endif /* MAS_DEV_H */

