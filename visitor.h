/* 
 * File:   visitor.h
 * Author: hiroaki
 *
 * Created on February 25, 2017, 6:41 AM
 */

#ifndef _VISITOR_H_
#define _VISITOR_H_

#include "mas.h"

typedef void (*visit_expr)(Expression* expr);
typedef void (*visit_stmt)(Statement*  stmt);
typedef void (*visit_func)(FunctionDefinition* func);

struct Visitor_tag {
    visit_expr* enter_list;
    visit_expr* leave_list;
    
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list;
    
    visit_func* enter_func_list;
    visit_func* leave_func_list;
    
};


#endif /* VISITOR_H */

