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

#ifndef LIBINSTRUMENTATION_PDEF_H
#define LIBINSTRUMENTATION_PDEF_H
#include <qglobal.h>
#include "../commoncode/commonglobal.h"

#define PROGRAM_NAME    "Coverage Browser"
#define PDF_HELP "testcocoon.pdf"
#ifdef OS_WIN32
#define TRADUCTION_FILE ""                        /*!< format of the traduction file, here trd_fr.qm, trd_it.qm... */
#define ASSISTANT_PATH (qApp->applicationDirPath()+"\\assistant")            /*!< Assistant */
#define HELP_PATH (qApp->applicationDirPath()+"\\")            /*!< pdf dir */
#else
#define TRADUCTION_FILE "/usr/share/coveragebrowser"            /*!< format of the traduction file, here trd_fr.qm, trd_it.qm... */
#define ASSISTANT_PATH "/usr/bin/"            /*!< Assistant */
#define HELP_PATH ("/usr/share/coveragebrowser/")            /*!< help dir */
#endif
#define OPTION_DIRECTORY "/TestCocoon/CoverageBrowser" /*!< name of the directory used for saving the options */

#define DEF_LANGUAGE QLocale::languageToString(QLocale::system().language())

#define COL_GREEN QColor(0,170,0)
#define COL_YELLOW QColor(255,255,0)
#define COL_YELLOW_LIGHT QColor(255,255,128)
#define COL_DARK_YELLOW QColor(128,128,0)
#define COL_ORANGE QColor(255,128,0)
#define COL_BLUE QColor(0,0,255)
#define COL_RED QColor(255,0,0)
#define COL_GRAY QColor(128,128,128)
#define COL_GRAY_LIGHT QColor(170,170,170)
#define COL_DARK_GRAY QColor(90,90,90)
#define COL_WHITE QColor(255,255,255)
#define COL_BLACK QColor(0,0,0)
#define COL_MAGENTA QColor(255,0,255)
#define COL_STDERR QColor(255,0,0)
#define COL_STDOUT QColor(0,0,0)
#define COL_STDIN QColor(0,0,255)

#define MAX_UNCLEANED_EXITS 4 

#ifdef OS_WIN32
#define DEF_FONT_SOURCE_CODE QString("Courier,11,-1,5,50,0,0,0,0,0")
#define DEF_FONT_COMMENT QString("Courier,12,-1,5,50,0,0,0,0,0")
#define DEF_WINDOW_STYLE "WindowsXP"
#elif defined(OS_MACX)
#define DEF_FONT_SOURCE_CODE QString("Courier New,11,-1,5,50,0,0,0,0,0")
#define DEF_FONT_COMMENT QString("Courier New,12,-1,5,50,0,0,0,0,0")
#define DEF_WINDOW_STYLE "Macintosh (Aqua)"
#else
#define DEF_FONT_SOURCE_CODE QString("Courier New,11,-1,5,50,0,0,0,0,0")
#define DEF_FONT_COMMENT QString("Courier New,12,-1,5,50,0,0,0,0,0")
#define DEF_WINDOW_STYLE "Cleanlooks"
#endif
#define DEF_FONT_MAIN (QApplication::font().toString())
#define DEF_EXECUTED_BY_LIMIT 25
#define DEF_FUNCTION_LOW_COLOR_LEVEL 70.0
#define DEF_FUNCTION_MEDIUM_COLOR_LEVEL 99.0
#define DEF_CLASS_LOW_COLOR_LEVEL 60.0
#define DEF_CLASS_MEDIUM_COLOR_LEVEL 95.0
#define DEF_MODULE_LOW_COLOR_LEVEL 60.0
#define DEF_MODULE_MEDIUM_COLOR_LEVEL 95.0
#define DEF_EXECUTION_LOW_COLOR_LEVEL 10.0
#define DEF_EXECUTION_MEDIUM_COLOR_LEVEL 50.0
#define DEF_TOOLTIP_SOURCE_VIEW false
#define DEF_TOOLTIP_SOURCE_LIST true
#define DEF_TOOLTIP_EXECUTION_LIST true
#define DEF_TOOLTIP_METHOD_LIST true
#define DEF_MINIMUM_COMMENT_SIZE 12
#define DEF_CACHE_STATISTIC 1000
#define DEF_CACHE_SOURCE 100
#define DEF_CACHE_EXECUTION 5000
#define LRU_MAX_ITEM 512
#define LRU_MAX_CSMES_FILES_DISPLAYED 20
#define LRU_MAX_CSMES_FILES (LRU_MAX_CSMES_FILES_DISPLAYED*2)

#define QT_COMPLETOR ( QT_VERSION >= 0x040200 )
#define UPDATE_SELECTION_TIMER 50
#define INSTRUMENTATION_BACKGROUND_LIGHTING 150
#define INSTRUMENTATION_BACKGROUND_STYLE (Qt::Dense4Pattern)
#define LRU_TIMER_UPDATE 15000
#define SearchTextRole Qt::UserRole
#define CommentExplanationRole (Qt::UserRole+1)
#define CommentRole (Qt::UserRole+2)
#define ExecutionNameRole (Qt::UserRole+3)
#define DEF_STATISTICS_CALULATION_BACKGROUND true

#if defined(OS_WIN32)
#  if defined(TESTCOCOON_SHARED) // DLL
#    if defined(TESTCOCOON_SHARED_INSTRUMENTATION) // inside DLL
#      define LIBINSTRUMENTATION_API   __declspec(dllexport)
#    else // outside DLL
#      define LIBINSTRUMENTATION_API   __declspec(dllimport)
#    endif 
#  else
#    define LIBINSTRUMENTATION_API 
#  endif
#else
#  define LIBINSTRUMENTATION_API 
#endif

#endif
