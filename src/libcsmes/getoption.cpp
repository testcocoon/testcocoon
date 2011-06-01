/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 3 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.TXT included in the
 ** packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@testcocoon.org if any conditions of this licensing are
 ** not clear to you.
 **
 ****************************************************************************/

#include "libcsmespdef.h"
#include "getoption.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define GETOPT_TYPE_MASK 0xFF
typedef struct {
  const getoption_t *opt;
  int additional_argument;
  int skip_next_item;
} opt_item_t;

static opt_item_t corresponding_option(const getoption_t option_table[],const char *item)
{
  int i;
  int opt_has_arg;
  opt_item_t opt_item;
  opt_item.opt=NULL;
  opt_item.additional_argument=0 ;
  opt_item.skip_next_item=0;

  for (i=0;(option_table[i].type&GETOPT_TYPE_MASK)!=GETOPT_LAST;i++)
  {
    const getoption_t* opt;
    opt=&option_table[i];
    if ((opt->type&GETOPT_TYPE_MASK)==GETOPT_ARGUMENT)
    {
      opt_item.opt=opt;
      opt_item.additional_argument=0 ;
      continue ;
    }
    if ((opt->type&GETOPT_TYPE_MASK)==GETOPT_HELP)
      continue ;
    if (opt->short_option && strcmp(item,opt->short_option)==0)
    {
      opt_item.opt=opt;
      opt_item.additional_argument=opt->type&GETOPT_HASARG ;
      opt_item.skip_next_item=opt_item.additional_argument;
      return opt_item;
    }
    opt_has_arg=opt->type&GETOPT_HASARG ;
    if ( opt_has_arg ) 
    {
      char str[256];
      int j;
      for (j=0;item[j]!='=' && item[j]!='\0';j++)
        str[j]=item[j];
      str[j]='\0';
      if (opt->long_option && strcmp(str,opt->long_option)==0)
      {
        opt_item.opt=opt;
        opt_item.additional_argument=0 ;
        return opt_item;
      }
    }
    else
    {
      if ( opt->long_option && strcmp(item,opt->long_option)==0 )
      {
        opt_item.opt=opt;
        opt_item.additional_argument=0 ;
        return opt_item;
      }
    }
  }

  return opt_item;
}

bool getoption_process(const getoption_t option_table[],int argc,const char *const argv[],char *report,int report_lg)
{
  int i,j;
  const getoption_t *opt;
  opt_item_t *opt_item=NULL;

  int nb_option=argc;
  /* calculate the number of options */
  for (i=1;i<argc;i++)
  {
    if (strcmp("--",argv[i])==0)
    {
      nb_option=i;
      break;
    }
  }

  opt_item=(opt_item_t*)MALLOC(argc*sizeof(opt_item_t));

  for (i=1;i<nb_option;i++)
  {
    opt_item[i]=corresponding_option(option_table,argv[i]);
    if (opt_item[i].opt==NULL)
    {
      char tmp[1024];
      sprintf(tmp,"Error: argument '%s' not allowed.\n", argv[i]);
      strncpy(report,tmp,report_lg);
      FREE(opt_item);
      return false;
    }
    else if (opt_item[i].skip_next_item)
    {
      i++;
      if (argc==i)
      {
        char tmp[1024];
        sprintf(tmp,"Error: '%s' requires an argument.\n",
            opt_item[i-1].opt->short_option);
        strncpy(report,tmp,report_lg);
        FREE(opt_item);
        return false;
      }
      else
        memset(&opt_item[i],0,sizeof(opt_item_t));
    }
  }
  if (nb_option!=argc) 
  {
    const getoption_t *argopt=NULL;
    for (i=0;(option_table[i].type&GETOPT_TYPE_MASK)!=GETOPT_LAST;i++)
    {
      if ((option_table[i].type&GETOPT_TYPE_MASK)==GETOPT_ARGUMENT)
        argopt=&option_table[i];
    }

    if (argopt==NULL)
    {
      strncpy(report,"Error: arguments are not allowed in the command line.\n", report_lg);
      FREE(opt_item);
      return false;
    }

    opt_item[nb_option].opt=NULL;
    opt_item[nb_option].additional_argument=0;
    for (i=nb_option+1;i<argc;i++)
    {
      opt_item[i].opt=argopt;
      opt_item[i].additional_argument=0;
    }
  }

  /* finding if optional arguments are missing */
  for (i=1;i<nb_option-1;i++)
    if (opt_item[i].opt && opt_item[i].additional_argument 
        && opt_item[i+1].opt && (opt_item[i+1].opt->type&GETOPT_TYPE_MASK)!=GETOPT_ARGUMENT)
    {
      char tmp[1024];
      sprintf(tmp,"Error: '%s' requires an argument.\n",
          opt_item[i].opt->short_option);
      strncpy(report,tmp,report_lg);
      FREE(opt_item);
      return false;
    }
  if (nb_option>1 && opt_item[nb_option-1].opt && opt_item[nb_option-1].additional_argument)
  {
    char tmp[1024];
    sprintf(tmp,"Error: '%s' requires an argument.\n",
        opt_item[nb_option-1].opt->short_option);
    strncpy(report,tmp,report_lg);
    FREE(opt_item);
    return false;
  }

  /* finding if single arguments are not used mutiple times */
  for (i=1;i<nb_option;i++)
  {
    if (opt_item[i].opt && ! (opt_item[i].opt->type&GETOPT_MULTIPLE) )
    {
      for (j=i+1;j<nb_option;j++)
      {
        if (opt_item[i].opt == opt_item[j].opt)
        {
          char tmp[1024];
          if (opt_item[i].opt->short_option && strcmp(opt_item[i].opt->short_option,argv[i])==0)
            sprintf(tmp,"Error: '%s' can only be used once.\n",
                opt_item[i].opt->short_option);
          else
            sprintf(tmp,"Error: '%s' can only be used once.\n",
                opt_item[i].opt->long_option);
          strncpy(report,tmp,report_lg);
          FREE(opt_item);
          return false;
        }
        if (opt_item[j].additional_argument) j++;
      }
    }
    if (opt_item[i].additional_argument) i++;
  }

  /* finding if mandatory arguments are used */
  opt=option_table;
  while ((opt->type&GETOPT_TYPE_MASK)!=GETOPT_LAST)
  {
    if (opt->type&GETOPT_MANDATORY)
    {
      int found_id=-1;
      for (i=1;i<nb_option && found_id==-1;i++)
      {
        if (opt_item[i].opt == opt )
          found_id = i ;
      }
      if (found_id==-1)
      {
        char tmp[1024];
        if (opt->short_option && opt->long_option)
          sprintf(tmp,"Error: '%s' or '%s' are mandatory.\n",
              opt->short_option,opt->long_option);
        else if (opt->long_option)
          sprintf(tmp,"Error: '%s' is mandatory.\n", opt->long_option);
        else if (opt->short_option)
          sprintf(tmp,"Error: '%s' is mandatory.\n", opt->short_option);
        else
          strcpy(tmp,"Error: one argument is mandatory.\n");
        strncpy(report,tmp,report_lg);
        FREE(opt_item);
        return false;
      }
    }
    opt++;
  }

  /* calling the callbacks */
  opt=option_table;
  while ((opt->type&GETOPT_TYPE_MASK)!=GETOPT_LAST)
  {
    for (i=1;i<nb_option;i++)
    {
      if (opt_item[i].opt != opt)
        continue; /* process according the priority in the option table */
      if ( opt_item[i].opt && opt_item[i].opt->callback )
      {
        int ret ;
        if ( ((opt_item[i].opt->type&GETOPT_TYPE_MASK)==GETOPT_ARGUMENT) )
        {
          ret = opt_item[i].opt->callback (argv[i]);
          if (!ret)
          {
            char tmp[1024];
            sprintf(tmp,"Error: Invalid arguments '%s'.\n", argv[i]);
            strncpy(report,tmp,report_lg);
            FREE(opt_item);
            return false;
          }
        }
        else if (opt_item[i].opt->type&GETOPT_HASARG)
        {
          const char *arg;
          if (opt_item[i].opt->short_option && strcmp(argv[i],opt_item[i].opt->short_option)==0)
            arg=argv[i+1];
          else
          {
            arg=strchr(argv[i],'=');
            if (arg==NULL)
            {
              char tmp[1024];
              sprintf(tmp,"Error: '%s' or '%s' requires an argument.\n",
                  opt_item[i].opt->short_option,opt_item[i].opt->long_option);
              strncpy(report,tmp,report_lg);
              FREE(opt_item);
              return false;
            }
            arg++;
          }
          ret = opt_item[i].opt->callback (arg);
          if (!ret)
          {
            char tmp[1024];
            if (opt_item[i].opt->short_option && strcmp(opt_item[i].opt->short_option,argv[i])==0)
              sprintf(tmp,"Error: Invalid arguments ('%s') for option '%s' or usage of option '%s' not allowed.\n",
                  arg,opt_item[i].opt->short_option,opt_item[i].opt->short_option);
            else
              sprintf(tmp,"Error: Invalid arguments ('%s') for option '%s' or usage of option '%s' not allowed.\n",
                  arg,opt_item[i].opt->long_option,opt_item[i].opt->long_option);
            strncpy(report,tmp,report_lg);
            FREE(opt_item);
            return false;
          }
        }
        else
        {
          ret = opt_item[i].opt->callback (NULL);
          if (!ret)
          {
            char tmp[1024];
            if (opt_item[i].opt->short_option && strcmp(opt_item[i].opt->short_option,argv[i])==0)
              sprintf(tmp,"Error: option '%s' not allowed.\n",
                  opt_item[i].opt->short_option);
            else
              sprintf(tmp,"Error: option '%s' not allowed.\n",
                  opt_item[i].opt->long_option);
            strncpy(report,tmp,report_lg);
            FREE(opt_item);
            return false;
          }
        }
      }
    }
    opt++;
  }
  FREE(opt_item);
  return true;
}


const char* getoption_help(const char*application_name,const getoption_t option_table[])
{
  int i;
  static char *helpstr=NULL;
  FREE(helpstr);

  for (i=0;(option_table[i].type&GETOPT_TYPE_MASK)!=GETOPT_LAST;i++)
    if ((option_table[i].type&GETOPT_TYPE_MASK)==GETOPT_HELP)
      helpstr=STRDUP(option_table[i].help);

  helpstr=(char*)REALLOC(helpstr,strlen(helpstr)+1 + 2+strlen(application_name) + 1024);
  strcat(helpstr,"\n\n");

  strcat(helpstr,"Usage:");
  strcat(helpstr,application_name);
  strcat(helpstr," <arguments>\n");
  for (i=0;(option_table[i].type&GETOPT_TYPE_MASK)!=GETOPT_LAST;i++)
  {
    if (option_table[i].help)
    {
      char tmp[2048];

      if ((option_table[i].type&GETOPT_TYPE_MASK)==GETOPT_HELP)
        continue;

      strcpy(tmp,"");
      if (option_table[i].type&GETOPT_MANDATORY)
        ;
      else if (option_table[i].type&GETOPT_MULTIPLE)
        strcat(tmp,"( ");
      else
        strcat(tmp,"[ ");

      if (option_table[i].short_option)
      {
        strcat(tmp,option_table[i].short_option);
        if (option_table[i].type&GETOPT_HASARG)
          strcat(tmp," <argument>");
      }
      if (option_table[i].short_option && option_table[i].long_option)
        strcat(tmp," | ");
      if (option_table[i].long_option)
      {
        strcat(tmp,option_table[i].long_option);
        if (option_table[i].type&GETOPT_HASARG)
          strcat(tmp,"=<argument>");
      }
      if ( ! (option_table[i].short_option || option_table[i].long_option) )
        strcat(tmp,"<argument>");

      if (option_table[i].type&GETOPT_MANDATORY)
        ;
      else if (option_table[i].type&GETOPT_MULTIPLE)
        strcat(tmp," )");
      else
        strcat(tmp," ]");

      helpstr=(char*)REALLOC(helpstr,strlen(helpstr)+1 + strlen(tmp)+1);
      strcat(helpstr,tmp);
      strcat(helpstr,"\t");
      helpstr=(char*)REALLOC(helpstr,strlen(helpstr)+1 + strlen(option_table[i].help)+1);
      strcat(helpstr,option_table[i].help);
      strcat(helpstr,"\n");
    }
  }

  return helpstr;
}
