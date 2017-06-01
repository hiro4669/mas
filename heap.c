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

static void mas_run_mark(MAS_Interpreter* interp) { // this implementation is temporary
    MAS_Object* pos = NULL;
    int i;
    for (i = 0, pos = interp->heap.header; pos; pos = pos->next, ++i) {
        if (i == 1) {
            fprintf(stderr, "mark!\n");
            pos->marked = MAS_TRUE;
        }
    }
    
}

static void mas_delete_object(MAS_Object* obj) {
    fprintf(stderr, "delete object\n");
    if (obj->type == STRING_OBJECT) {
        if (!obj->u.string.is_literal) {
            MEM_free(obj->u.string.string);            
        } 
    }
    MEM_free(obj);
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
            mas_delete_object(rm_obj);            
        } else {
            pos = pos->next;
        }
    }
    if (interp->heap.header) {
        fprintf(stderr, "keep %s\n", interp->heap.header->u.string.string);
    }
}

void mas_run_gc(MAS_Interpreter* interp) {
    mas_run_mark(interp);
    mas_run_sweep(interp);
}


