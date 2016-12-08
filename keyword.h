/* 
 * File:   keyword.h
 * Author: hiroaki
 *
 * Created on December 9, 2016, 8:09 AM
 */

#ifndef _KEYWORD_H_
#define _KEYWORD_H_

typedef enum lstate {
    BEGIN,
    MIDDLE,
    END,
} LSTATE;

struct OPE {
  char *name; 
  int type; 
  LSTATE state;
};

#endif /* KEYWORD_H */

