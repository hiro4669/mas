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

struct Visitor_tag {
    visit_expr* enter_list;
    visit_expr* leave_list;
    
    visit_stmt* enter_stmt_list;
    visit_stmt* leave_stmt_list;
    
};


#endif /* VISITOR_H */

