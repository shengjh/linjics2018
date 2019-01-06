#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  char s[32];
  int result;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */


} WP;

#endif
