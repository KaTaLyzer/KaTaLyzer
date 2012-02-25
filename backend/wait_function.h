#ifndef WAIT_FUNCTION_H
#define WAIT_FUNCTION_h


#include <time.h>
#include <stdio.h>
#include "variables.h"

void waiting();
int __nsleep ( const struct timespec *req, struct timespec *rem );
int msleep ( unsigned long milisec );

#endif
