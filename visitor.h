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

struct Visitor_tag {
    visit_expr* enter_list;
    visit_expr* leave_list;
};


#endif /* VISITOR_H */

