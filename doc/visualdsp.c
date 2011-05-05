#include <stdio.h>
#include <string.h>

#ifdef __COVERAGESCANNER__
#pragma CoverageScanner(cov-off)
static int csfputs(const char *s, void *stream) 
{
  return fputs(s, (FILE *)stream); 
}

static void *csfopenappend(const char *path)
{
  return (void*)fopen(path,"a+");
}

static int csfclose(void *fp)        
{
  return fclose((FILE*)fp);
}
#pragma CoverageScanner(cov-on)
#endif

int main()
{
  char location[256];
  int lg_location;

#ifdef __COVERAGESCANNER__
  printf(".csexe file name (without extension}:"); fflush(stdout);
  fgets(location,sizeof(location),stdin);
  lg_location=strlen(location);
  if (lg_location)
    location[lg_location-1]='\0'; // strip \n
  __coveragescanner_set_custom_io( NULL,
      csfputs,
      csfopenappend,
      NULL,
      NULL,
      csfclose,
      NULL);
  __coveragescanner_filename(location);
#endif


  /* some sone to instrument here */

#ifdef __COVERAGESCANNER__
  __coveragescanner_save();
#endif
}
