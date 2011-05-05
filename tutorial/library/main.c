// main application code which uses the code of a static and dynamic library.
#include "staticlib.h"
#include "dynamiclib.h"

int main(int argc, char* argv[])
{
#ifdef __COVERAGESCANNER__
  __coveragescanner_install(argv[0]);
#endif

  return static_function() + dynamic_function() ;
}
