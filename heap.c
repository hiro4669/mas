#include <stdio.h>

#include "mas.h"

static MAS_Object* alloc_mas_object(MAS_Interpreter* interp, ObjectType type) {
    MAS_Object* obj;
    obj = (MAS_Object*)MEM_malloc(sizeof(MAS_Object));
    interp->heap.current_heap_size += sizeof(MAS_Object);
    obj->marked = MAS_FALSE;
    obj->type = type;
    obj->next = interp->heap.header;
    if (obj->next) {
        obj->next->prev = obj;
    }
    interp->heap.header = obj;
    obj->prev = NULL;    
    return obj;
}

MAS_Object* mas_literal_to_mas_ostring(MAS_Interpreter* interp, char* str) {
    MAS_Object* s_obj = alloc_mas_object(interp, STRING_OBJECT);
    s_obj->u.string.is_literal = MAS_TRUE;
    s_obj->u.string.ref_count = 1; // initialize;
    s_obj->u.string.string = str;
    return s_obj;
}

MAS_Object* mas_create_array_literal(MAS_Interpreter* interp, int size) {
    MAS_Object* obj;
    obj = alloc_mas_object(interp, ARRAY_OBJECT);
    obj->u.array.alloc_size = size;
    obj->u.array.size = size;
    obj->u.array.array = (MAS_Value*)MEM_malloc(sizeof(MAS_Value) * size);
    return obj;
}

static void mas_run_mark(MAS_Interpreter* interp) { // this implementation is temporary
    
    Variable* pos;
    for (pos = interp->variable; pos; pos = pos->next) {
        switch (pos->value.type) {
            case MAS_STRING_VALUE:
            case MAS_ARRAY_VALUE: {
                fprintf(stderr, "mark\n");
                pos->value.u.object_value->marked = MAS_TRUE;
                break;
            }                
            default: {
                break;
            }
        }
    }
    
    /*
    MAS_Object* pos = NULL;
    int i;
    for (i = 0, pos = interp->heap.header; pos; pos = pos->next, ++i) {
        if (i == 1) {
            fprintf(stderr, "mark!\n");
            pos->marked = MAS_TRUE;
        }
    }
    */
    
}

static void mas_delete_object(MAS_Interpreter* interp, MAS_Object* obj) {
    if (obj->type == STRING_OBJECT) {
        if (!obj->u.string.is_literal) {
            MEM_free(obj->u.string.string);            
        } 
    } else if (obj->type == ARRAY_OBJECT) {
        MEM_free(obj->u.array.array);
    }
    
    MEM_free(obj);
    interp->heap.current_heap_size -= sizeof(MAS_Object);
}

static MAS_Object* mas_unchain(MAS_Interpreter* interp, MAS_Object* target) {
    if (target->prev) { // prev is not head        
        target->prev->next = target->next;
        if (target->next) {
            target->next->prev = target->prev;
        }
    } else { // prev is head
        if (target->next) {
            target->next->prev = NULL;
            interp->heap.header = target->next;
        } else {
            interp->heap.header = NULL;
        }
    } 
    return target;
}
static void mas_run_sweep(MAS_Interpreter* interp) {
    MAS_Object* pos = interp->heap.header;
    MAS_Object* rm_obj;
    while(pos) {
        if (!pos->marked) {
            rm_obj = pos;
            mas_unchain(interp, rm_obj);
            pos = pos->next;            
            mas_delete_object(interp, rm_obj);            
        } else {
            pos->marked = MAS_FALSE;
            pos = pos->next;
        }
    }
    if (interp->heap.header) {
        fprintf(stderr, "keep %s\n", interp->heap.header->u.string.string);
    }
}

void mas_run_gc(MAS_Interpreter* interp) {
    fprintf(stderr, "start gc\n");
    mas_run_mark(interp);
    mas_run_sweep(interp);
    
    fprintf(stderr, "after heap size = %d\n", interp->heap.current_heap_size);
//    fprintf(stderr, "sizefo masvalue = %d\n", (int)sizeof(MAS_Object));
}


