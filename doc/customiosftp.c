#define VERBOSE 1

#ifdef WIN32
# include <winsock2.h>
#define LIBSSH2_WIN32
#define LIBSSH2_API 
#else
#include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <libssh2.h>
#include <libssh2_sftp.h>

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

static LIBSSH2_SESSION *session=NULL;
static LIBSSH2_SFTP *sftp_session=NULL;
static int sock=0;

static void extract_location(const char *location, char *server, char *user, char*passwd, char *file)
{
  int i,j;
  int lg_location;
  lg_location=strlen(location);
  for (i=0;i<lg_location;i++)
  {
    if (location[i]=='\n')
      break;
    server[i]=location[i];
  }
  server[i]='\0';
  i++;

  for (j=0;i<lg_location;i++,j++)
  {
    if (location[i]=='\n')
      break;
    user[j]=location[i];
  }
  user[j]='\0';
  i++;

  for (j=0;i<lg_location;i++,j++)
  {
    if (location[i]=='\n')
      break;
    passwd[j]=location[i];
  }
  passwd[j]='\0';
  i++;

  for (j=0;i<lg_location;i++,j++)
  {
    if (location[i]=='\0')
      break;
    file[j]=location[i];
  }
  file[j]='\0';
}

static void close_sftp_session()
{
  if (sftp_session)
    libssh2_sftp_shutdown(sftp_session);
  sftp_session=NULL;

  if (session)
  {
    libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);
  }
  session=NULL;

  if (sock)
  {
#ifdef WIN32
    Sleep(1000);
    closesocket(sock);
#else
    sleep(1);
    close(sock);
#endif
  }
  sock=0;
}

static  int open_sftp_session(const char *server,const char *user,const char *passwd)
{
  struct sockaddr_in sin;
  int rc;

  sock = socket(AF_INET, SOCK_STREAM, 0);

  sin.sin_family = AF_INET;
  sin.sin_port = htons(22);
  sin.sin_addr.s_addr = inet_addr(server);
  if (connect(sock, (struct sockaddr*)(&sin),
        sizeof(struct sockaddr_in)) != 0) 
  {
    close_sftp_session();
    return 0;
  }

  /* Create a session instance
  */
  session = libssh2_session_init();
  if(!session)
  {
    close_sftp_session();
    return 0;
  }

  /* ... start it up. This will trade welcome banners, exchange keys,
   * and setup crypto, compression, and MAC layers
   */
  rc = libssh2_session_startup(session, sock);
  if(rc)
  {
    close_sftp_session();
    return 0;
  }

  libssh2_session_set_blocking(session, 1);

  if (libssh2_userauth_password(session, user, passwd))
  {
    close_sftp_session();
    return 0;
  }

  sftp_session = libssh2_sftp_init(session);

  if (!sftp_session)
  {
    close_sftp_session();
    return 0;
  }
  return 1;
}

static int csfputs(const char *s, void *stream) 
{
#if VERBOSE
  fprintf(stderr,"csfputs:%s\n",s);
#endif
  return libssh2_sftp_write((LIBSSH2_SFTP_HANDLE*)stream,s,strlen(s));
}

static void *csfopenappend(const char *location)
{
  LIBSSH2_SFTP_HANDLE *handle;
  char server[1024];
  char user[1024];
  char passwd[1024];
  char file[1024];
  LIBSSH2_SFTP_ATTRIBUTES attrs;

  extract_location(location,server,user,passwd,file);
#if VERBOSE
  fprintf(stderr,"csfopenappend %s:%s:%s\n",server,user,file);
#endif
  if (open_sftp_session(server,user,passwd))
  {
    handle = libssh2_sftp_open(sftp_session, file, LIBSSH2_FXF_CREAT|LIBSSH2_FXF_WRITE,
        LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
        LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH );
  }
  else
    return NULL;

  if (handle)
  {
    if (libssh2_sftp_fstat(handle,&attrs)==0)
    { /* Go to the end of the file */
      libssh2_sftp_seek(handle,attrs.filesize);
    }
  }
  return handle;
}

static void *csfopenread(const char *location)            
{
  char server[1024];
  char user[1024];
  char passwd[1024];
  char file[1024];

  extract_location(location,server,user,passwd,file);
#if VERBOSE
  fprintf(stderr,"csfopenread %s:%s:%s\n",server,user,file);
#endif
  if (open_sftp_session(server,user,passwd))
    return (void*)  libssh2_sftp_open(sftp_session, file, LIBSSH2_FXF_READ, 0);
  else
    return NULL;
}

static void *csfopenwrite(const char *location) 
{
  char server[1024];
  char user[1024];
  char passwd[1024];
  char file[1024];

  extract_location(location,server,user,passwd,file);
#if VERBOSE
  fprintf(stderr,"csfopenwrite %s:%s:%s\n",server,user,file);
#endif
  if (open_sftp_session(server,user,passwd))
    return (void*)  libssh2_sftp_open(sftp_session, file, LIBSSH2_FXF_CREAT|LIBSSH2_FXF_WRITE,
        LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
        LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH );
  else
    return NULL;
}

static char *csfgets(char *s, int size, void *stream) 
{
  size_t ss;
  ss = libssh2_sftp_read((LIBSSH2_SFTP_HANDLE*)stream,s,size-1);
  if (ss)
  {
    s[ss]='\0';
#if VERBOSE
    fprintf(stderr,"csfgets:%s\n",s);
#endif
    return s;
  }
  else
    return NULL;
}

static int csremove(const char *location)  
{
  int ret;
  char server[1024];
  char user[1024];
  char passwd[1024];
  char file[1024];

  extract_location(location,server,user,passwd,file);
#if VERBOSE
  fprintf(stderr,"csremove %s:%s:%s\n",server,user,file);
#endif
  if (open_sftp_session(server,user,passwd))
  {
    ret = libssh2_sftp_unlink(sftp_session,file);
    close_sftp_session();
    return ret;
  }
  else
    return -1;
}

static int csfclose(void *fp)        
{
#if VERBOSE
  fprintf(stderr,"csfclose\n");
#endif
  return libssh2_sftp_close((LIBSSH2_SFTP_HANDLE*)fp);
}

int main()
{
  char location[1024];
  char tmp[1024];
#ifdef WIN32
  WSADATA wsadata;

  WSAStartup(WINSOCK_VERSION, &wsadata);
#endif

  location[0]='\0';

  printf("server IP:"); fflush(stdout);
  fgets(tmp,sizeof(tmp),stdin);
  strcat(location,tmp);

  printf("user:"); fflush(stdout);
  fgets(tmp,sizeof(tmp),stdin);
  strcat(location,tmp);

  printf("passwd:"); fflush(stdout);
  fgets(tmp,sizeof(tmp),stdin);
  strcat(location,tmp);

  printf(".csexe file name (without extension}:"); fflush(stdout);
  fgets(tmp,sizeof(tmp),stdin);
  strcat(location,tmp);

  location[strlen(location)-1]='\0';

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
