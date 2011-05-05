#include <string>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
#ifdef __COVERAGESCANNER__
  __coveragescanner_install(argv[0]);
#endif

  string message="Hello ";

  if (argc==1)
    message += "World" ;

  for (int i=1;i<argc;i++)
  {
    if (i==argc-1 && argc>2)
      message += " and ";
    else if (i>1)
      message += ", ";
    message += argv[i];
  }

  cout << message ;
  cout << endl;
  return 0;
}
