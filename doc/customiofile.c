#include <stdio.h>
#include <string.h>
#define VERBOSE 1

static int csfputs(const char *s, void *stream) 
{
#if VERBOSE
  fprintf(stderr,"csfputs:%s\n",s);
#endif
  return fputs(s, (FILE *)stream); 
}

static void *csfopenappend(const char *path)
{
#if VERBOSE
  fprintf(stderr,"csfopenappend:%s\n",path);
#endif
  return (void*)fopen(path,"a+");
}

static void *csfopenread(const char *path)            
{
#if VERBOSE
  fprintf(stderr,"csfopenread:%s\n",path);
#endif
  return (void*)fopen(path,"r"); 
}

static void *csfopenwrite(const char *path) 
{
#if VERBOSE
  fprintf(stderr,"csfopenwrite:%s\n",path);
#endif
  return (void*)fopen(path,"w"); 
}

static char *csfgets(char *s, int size, void *stream) 
{
  char * ret;
  ret = fgets(s, size, (FILE *)stream); 
#if VERBOSE
  fprintf(stderr,"csfgets:%s\n",s);
#endif
  return ret;
}

static int csremove(const char *filename)  
{
#if VERBOSE
  fprintf(stderr,"csremove:%s\n",filename);
#endif
  return remove(filename);
}

static int csfclose(void *fp)        
{
#if VERBOSE
  fprintf(stderr,"csfclose\n");
#endif
  return fclose((FILE*)fp);
}

int main()
{
  char location[256];
  int lg_location;

  printf(".csexe file name (without extension}:"); fflush(stdout);
  fgets(location,sizeof(location),stdin);
  lg_location=strlen(location);
  if (lg_location)
    location[lg_location-1]='\0'; // strip \n
#ifdef __COVERAGESCANNER__
  __coveragescanner_set_custom_io( csfgets,
      csfputs,
      csfopenappend,
      csfopenread,
      csfopenwrite,
      csfclose,
      csremove);
  __coveragescanner_install(location);
#endif
}
