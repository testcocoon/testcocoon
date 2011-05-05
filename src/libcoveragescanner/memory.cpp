#include "regexp_int.h"
#include "regexp_custom.h"
#include <stdlib.h>
#include <stdio.h>
#include "debug.h"

void re_report(const char* error)
{
  FUNCTION_TRACE;
  // trace errors
  DEBUG2("REGEXP ERROR: %s\n", error);
}


