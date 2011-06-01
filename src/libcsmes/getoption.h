/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ** This file and the library libcsmes can be integrated in any kind
 ** of products without any licensing restrictions.
 **
 ** Contact info@testcocoon.org if any conditions of this licensing are
 ** not clear to you.
 **
 ****************************************************************************/

#ifndef GETOPT_H_CSMES
#define GETOPT_H_CSMES

#include "libcsmespdef.h"

#define  GETOPT_OPTION 0          /*!< then entry is an option */
#define  GETOPT_HELP 1            /*!< default help for the --help, -h of /? command */
#define  GETOPT_MULTIPLE (1<<8)   /*!< this option can only occurs several times */
#define  GETOPT_HASARG (1<<9)     /*!< this option needs one mandatory argument */
#define  GETOPT_MANDATORY (1<<10) /*!< this option is mandatory */
#define  GETOPT_LAST 2            /*!< End of the list */
#define  GETOPT_ARGUMENT 3        /*!< Not an option, an argument */

typedef struct {
  int type;                               /*!< combination of GETOPT_ flags */
  const char *short_option;               /*!< short option. ex:"-d" or "-d arg" (can be NULL) */
  const char *long_option;                /*!< long option. ex:"--degug" or "--debug=arg" */
  const char *help;                       /*!< help string (can be NULL, in this case the option is not apearing into the help */
  bool (*callback)(const char *argument);  /*!< callback */
} getoption_t;

 LIBCSMES_API bool  getoption_process(const getoption_t option_table[],int argc,const char *const argv[],char *report,int report_lg); 
 LIBCSMES_API const char*  getoption_help(const char*application_name,const getoption_t option_table[]); 


#endif
