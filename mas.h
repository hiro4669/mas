/* 
 * File:   mas.h
 * Author: hiroaki
 *
 * Created on February 16, 2017, 8:20 AM
 */

#ifndef _MAS_H_
#define _MAS_H_

typedef struct LocalInfo_tag LocalInfo;


/*  util.c */
void mas_set_localinfo(LocalInfo *l_info);
LocalInfo *mas_get_localinfo();

#endif /* MAS_H */

