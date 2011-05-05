/****************************************************************************
 **
 ** Copyright (C) see AUTHORS included in the packaging of this file.
 **                ---   All rights reserved ---
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
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

#include "libinstrumentationpdef.h"
#include "csmestextreport.h"
#include "xmlwriter.h"
#include "htmlwriter.h"
#include "csmesinstrumentation.h"
#include "csmes.h"
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextDocumentWriter>
#include <QTime>
#include <QBuffer>
#include <QXmlStreamWriter>
#include <QSet>
#include "Service.h"

CSMesTextReport::CSMesTextReport() : CSMesReport()
{
}


QString CSMesTextReport::exportTextReport(const QString &filename,const QString & format_executed,const QString & format_unexecuted ,int level,Instrumentation::coverage_method_t method) const
{
  QFile output;
  QTextStream stream(stdout);
  if (!filename.isEmpty())
  {
    output.setFileName(filename);
    if (!output.open(QFile::WriteOnly))
      return QObject::tr("Error opening file '%1'").arg(filename);
    stream.setDevice(&output);
  }
  QStringList sources=Sources(NON_EMPTY);
  bool global_print_executed= !format_executed.isEmpty();
  bool global_print_unexecuted= !format_unexecuted.isEmpty();
  {
    for (QStringList::const_iterator itsrc=sources.begin();itsrc!=sources.end();++itsrc)
    {
      QString source=*itsrc;
      QString module="";
      if (findSourceModule(module,source))
      {
        const QVector<Instrumentation> inst=instrumentationList(module,source);
        for (int index=0;index<inst.size();index++)
        {
          bool print_executed=false;
          bool print_unexecuted=false;
          switch ( inst.at(index).status(level,method) )
          {
            case Instrumentation::STATUS_PARTIALLY_VALIDATED:
            case Instrumentation::STATUS_UNVALIDATED:
              print_unexecuted=global_print_unexecuted;
              break;
            case Instrumentation::STATUS_MANUALLY_VALIDATED:
            case Instrumentation::STATUS_VALIDATED:
              print_executed=global_print_executed;
              break;
            default:
              break;
          }
          QString message;
          if (print_executed)
            message=explanationFragmentTextFromIndex(format_executed,module,source,index,CSMes::ORIGINAL,level,method) ;
          if (print_unexecuted)
            message=explanationFragmentTextFromIndex(format_unexecuted,module,source,index,CSMes::ORIGINAL,level,method) ;

          if (!message.isEmpty())
            stream << message << '\n';
        }
      }
    }
  }
  return QString();
}



