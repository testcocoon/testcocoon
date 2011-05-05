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

#ifndef ___options_H
#define ___options_H
#include <qstring.h>
#include <QStringList>
#include <QByteArray>
#include <QMdiArea>
#include <QWidget>
#include <QMainWindow>
#include <QWorkspace>
#include <QList>
class WMain ;

class Options 
{
  public:
     static void            read_options ();
     static void            save_options ();
     static void            restore_window_position(const QString &domain,WMain *wmain_p);
     static void            save_window_position(const QString &domain,const QMdiArea *widget_p);
     static void            restore_window_position(const QString &domain,const QString &name,QMainWindow *widget_p);
     static void            save_window_position(const QString &domain,const QString &name,const QMainWindow *widget_p);
     static void            restore_window_position(const QString &domain,const QString &name,QWidget *widget_p);
     static void            save_window_position(const QString &domain,const QString &name,const QWidget *widget_p);




  public:
     static double          get_opt_double(const QString &domain,const QString&,double def=0.0);
     static long            get_opt_long(const QString &domain,const QString&,long def=0);
     static int             get_opt_int(const QString &domain,const QString&,int def=0);
     static bool            get_opt_bool(const QString &domain,const QString&,bool def=false);
     static QString         get_opt_str(const QString &domain,const QString&,const QString &def);
     static QStringList     get_opt_strlst(const QString &domain,const QString&,const QStringList &def=QStringList());
     static QList<int>      get_opt_intlst(const QString &domain,const QString&,const QList<int> &def=QList<int>());
     static QByteArray      get_opt_array(const QString &domain,const QString&,const QByteArray &def=QByteArray());
     static void            set_opt(const QString &domain,const QString&, double);
     static void            set_opt(const QString &domain,const QString&, long);
     static void            set_opt(const QString &domain,const QString&, int);
     static void            set_opt(const QString &domain,const QString&, bool);
     static void            set_opt(const QString &domain,const QString&,const  QString &);
     static void            set_opt(const QString &domain,const QString&,const  QStringList &);
     static void            set_opt(const QString &domain,const QString&,const  QList<int> &);
     static void            set_opt(const QString &domain,const QString&,const QByteArray &);

  private:
     static double          _get_opt_double (const QString&,double def=0.0);
     static long            _get_opt_long (const QString&,long def=0);
     static int             _get_opt_int (const QString&,int def=0);
     static bool            _get_opt_bool (const QString&,bool def=false);
     static QString         _get_opt_str (const QString&,const QString &def=QString::null);
     static QStringList     _get_opt_strlst (const QString&,const QStringList &def=QStringList());
     static QByteArray      _get_opt_array (const QString&,const QByteArray &def=QByteArray());
     static void            _set_opt (const QString&, double);
     static void            _set_opt (const QString&, long);
     static void            _set_opt (const QString&, int);
     static void            _set_opt (const QString&, bool);
     static void            _set_opt (const QString&,const  QString &);
     static void            _set_opt (const QString&,const  QStringList &);
     static void            _set_opt (const QString&,const QByteArray &);
};
#endif
