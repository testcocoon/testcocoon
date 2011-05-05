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

#include "coveragebrowserpdef.h"
#include "coveragebrowser_args.h"
#include "options.h"
#include "WMain.h"
#include "WSourceView.h"
#include <qsettings.h>
#include <qworkspace.h>
#include <qwidget.h>
#include <qtextstream.h>
#include "hexcoder.h"
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QMdiArea>

static long nb_uncleaned_exits=-1;

static QSettings *settings_p = NULL;
static QString option(const QString &opt)
{
  QString         opt_str;
  if (opt.startsWith ("#"))
    opt_str = OPTION_DIRECTORY + opt;
  else
    opt_str = OPTION_DIRECTORY + ("#" + opt);
  return opt_str;
}

static bool option_valid()
{
  if (nb_uncleaned_exits<0)
    return false;

  return (MAX_UNCLEANED_EXITS>=nb_uncleaned_exits) ;
}

void Options::read_options ()
{
  QString         opt_str=option("NB_UNCLEANED_EXITS");
  settings_p = new QSettings(QSettings::IniFormat,QSettings::UserScope,"testcocoon","coveragebrowser");
#ifdef NO_DEBUG
  nb_uncleaned_exits = 1 + settings_p->value (opt_str, 0).toInt();
#else
  nb_uncleaned_exits = 0;
#endif
  settings_p->setValue (opt_str, (int) nb_uncleaned_exits );
  delete settings_p;
  settings_p = new QSettings(QSettings::IniFormat,QSettings::UserScope,"testcocoon","coveragebrowser");
}


  void
Options::save_options ()
{
  _set_opt("NB_UNCLEANED_EXITS",(long)0);
  delete          settings_p;
  settings_p = NULL;
}


double Options::_get_opt_double (const QString &opt,double def)
{
  double            val;
  QString         opt_str=option(opt);
  if (!option_valid())
    return def;

  if (settings_p)
  {
    val = settings_p->value (opt_str, def ).toDouble();
    return val;
  }
  else
    return def;
}

int Options::_get_opt_int (const QString &opt,int def)
{
  return _get_opt_long(opt,def);
}

long Options::_get_opt_long (const QString &opt,long def)
{
  long            val;
  QString         opt_str=option(opt);
  if (!option_valid())
    return def;

  if (settings_p)
  {
    val = settings_p->value (opt_str, static_cast<int>(def) ).toInt();
    return val;
  }
  else
    return def;
}


bool Options::_get_opt_bool (const QString &opt,bool def)
{
  bool            result;
  QString         opt_str=option(opt);
  if (!option_valid())
    return def;

  if (settings_p)
  {
    result = settings_p->value (opt_str, def).toBool();
    return result;
  }
  else
    return def;
}


  QByteArray
Options::_get_opt_array (const QString &opt,const QByteArray &def)
{
  QString val=_get_opt_str(opt,def);
  QByteArray val_cstr=val.toAscii();
  char *hex=NULL;
  int hex_lg=0;
  hexdecode(val_cstr.constData(),&hex,&hex_lg);
  QByteArray ret(hex,hex_lg);
  free(hex);

  return ret;
}

  QStringList
Options::_get_opt_strlst (const QString &opt,const QStringList &def)
{
  QString         opt_str=option(opt);
  if (!option_valid())
    return def;

  if (settings_p)
  {
    QStringList ret= settings_p->value (opt_str,def).toStringList();
    return ret;
  }
  else
    return def;
}

  QString
Options::_get_opt_str (const QString &opt,const QString &def)
{
  QString         opt_str=option(opt);
  if (!option_valid())
    return def;

  if (settings_p)
  {
    QString ret= settings_p->value (opt_str,def).toString();
    return ret;
  }
  else
    return def;
}


void Options::_set_opt (const QString &opt, double val)
{
  QString         opt_str=option(opt);
  if (settings_p)
    settings_p->setValue (opt_str,  val);
}


void Options::_set_opt (const QString &opt, int val)
{
  _set_opt(opt,static_cast<long>(val));
}

void Options::_set_opt (const QString &opt, long val)
{
  QString         opt_str=option(opt);
  if (settings_p)
    settings_p->setValue (opt_str, (int) val);
}


void Options::_set_opt (const QString &opt, const QString &val)
{
  QString         opt_str=option(opt);
  if (settings_p)
    settings_p->setValue (opt_str, val);
}

void Options::_set_opt (const QString &opt, const QStringList &val)
{
  QString         opt_str=option(opt);
  if (settings_p)
    settings_p->setValue (opt_str, val);
}


void Options::_set_opt (const QString &opt, const QByteArray &val)
{
  char *hex=NULL;
  hexencode(val.data(),val.length(),&hex);
  _set_opt (opt, QString::fromLatin1(hex) );
  free(hex);
}


void Options::_set_opt (const QString &opt, bool val)
{
  QString         opt_str=option(opt);
  if (settings_p)
    settings_p->setValue (opt_str, val);
}

void Options::restore_window_position(const QString &domain,const QString &name,QMainWindow *widget_p)
{
  bool restore_pos = get_opt_bool(domain,"RESTORE_WINDOW_POSITION",false);
  if (!restore_pos)
    return ;

  restore_window_position(domain,name,(QWidget*)widget_p);
  widget_p->restoreState(get_opt_array(domain,"WIDGET_POS_DOCKPOS"));
}

void Options::save_window_position(const QString &domain,const QString &name,const QMainWindow *widget_p)
{
  save_window_position(domain,name,(QWidget*)widget_p);
  set_opt(domain,"WIDGET_POS_DOCKPOS",widget_p->saveState());
}

void Options::restore_window_position(const QString &domain,const QString &name,QWidget *widget_p)
{
  bool maximized ;
  bool restore_pos;
  long  posx,posy,height,width ;

  restore_pos = get_opt_bool(domain,"RESTORE_WINDOW_POSITION",false);
  if (!restore_pos)
    return ;

  maximized = get_opt_bool(domain,"WIDGET_POS_"+name+"_MAXIMIZED",false);
  posx = get_opt_long(domain,"WIDGET_POS_"+name+"_X",0);
  posy = get_opt_long(domain,"WIDGET_POS_"+name+"_Y",0);
  width = get_opt_long(domain,"WIDGET_POS_"+name+"_WIDTH",100);
  height = get_opt_long(domain,"WIDGET_POS_"+name+"_HEIGHT",100);

  if (maximized)
    widget_p->showMaximized();
  else
  {
    QPoint pos(posx,posy);
    QSize s(width,height);
    widget_p->showNormal();
    widget_p->move(pos);
    widget_p->resize(s);
  }
}

void Options::save_window_position(const QString &domain,const QString &name,const QWidget *widget_p)
{
  bool maximized ;
  long  posx,posy,height,width ;
  maximized = widget_p->isMaximized();
  posx = widget_p->x();
  posy= widget_p->y();

  height = widget_p->height();
  width =widget_p->width();

  set_opt(domain,"WIDGET_POS_"+name+"_MAXIMIZED",maximized);
  set_opt(domain,"WIDGET_POS_"+name+"_X",posx);
  set_opt(domain,"WIDGET_POS_"+name+"_Y",posy);
  set_opt(domain,"WIDGET_POS_"+name+"_WIDTH",width);
  set_opt(domain,"WIDGET_POS_"+name+"_HEIGHT",height);
}


void Options::save_window_position(const QString &domain,const QMdiArea *workspace_p)
{
  QList<QMdiSubWindow *>  windows=workspace_p->subWindowList(QMdiArea::StackingOrder);
  QMdiSubWindow *mdi_subwindow_p;
  long nb_widget=windows.count();
  set_opt(domain,"WIDGET_NB_WIDGET",nb_widget);
  int id=0;
  for (QList<QMdiSubWindow *>::const_iterator it= windows.begin(); it != windows.end(); ++it)
  {
    mdi_subwindow_p=*it;
    QString name="WORKSPACE"+QString::number(id);
    WSourceView *sourceview_p=dynamic_cast<WSourceView*>(mdi_subwindow_p->widget());
    if (sourceview_p)
    {
      set_opt(domain,"TYPE_"+name,QString("SOURCEVIEW"));
      QString module=sourceview_p->getModule();
      QString source=sourceview_p->getSource();
      set_opt(domain,"MODULE_"+name,module);
      set_opt(domain,"SOURCE_"+name,source);
      save_window_position(domain,name,mdi_subwindow_p);
      const WSourceView *wsource_p=dynamic_cast<const WSourceView*>(mdi_subwindow_p->widget());
      if (wsource_p)
      {
        set_opt(domain,"SOURCE_VIEW_COLUMN_STATE_"+name,wsource_p->getColumnState());
        set_opt(domain,"SOURCE_VIEW_SCROLL_POS_"+name,wsource_p->getScrollPos());
      }
    }
    id++;
  }
}

  void
Options::restore_window_position(const QString &domain,WMain *wmain_p)
{
  bool restore_pos = get_opt_bool(domain,"RESTORE_WINDOW_POSITION",false);
  if (!restore_pos)
    return ;
  long nb_widget = get_opt_long(domain,"WIDGET_NB_WIDGET",0);
  for (int i=0;i<nb_widget;i++)
  {
    QString name="WORKSPACE"+QString::number(i);
    QString type=get_opt_str(domain,"TYPE_"+name,QString());
    if (type=="SOURCEVIEW")
    {
      QString module=get_opt_str(domain,"MODULE_"+name,QString::null).toAscii();
      QString source=get_opt_str(domain,"SOURCE_"+name,QString::null).toAscii();
      QMdiSubWindow *widget_p=wmain_p->newSourceWindow(module,source);
      if (widget_p)
      {
        QByteArray column_state=get_opt_array(domain,"SOURCE_VIEW_COLUMN_STATE_"+name);
        int scroll_pos=get_opt_int(domain,"SOURCE_VIEW_SCROLL_POS_"+name);
        restore_window_position(domain,name,widget_p);
        WSourceView *wsource_p=dynamic_cast<WSourceView*>(widget_p->widget());
        if (wsource_p)
        {
          wsource_p->setColumnState(column_state);
          wsource_p->setScrollPos(scroll_pos);
        }
      }
    }
  }
}


double          Options::get_opt_double (const QString &domain,const QString&name,double def)
{
  if (!domain.isEmpty())
    return _get_opt_double(domain+"#"+name,_get_opt_double(name,def));
  else
    return _get_opt_double(name,def);
}

long            Options::get_opt_long (const QString &domain,const QString&name,long def)
{
  if (!domain.isEmpty())
    return _get_opt_long(domain+"#"+name,_get_opt_long(name,def));
  else
    return _get_opt_long(name,def);
}

int             Options::get_opt_int (const QString &domain,const QString&name,int def)
{
  if (!domain.isEmpty())
    return _get_opt_int(domain+"#"+name,_get_opt_int(name,def));
  else
    return _get_opt_int(name,def);
}

bool            Options::get_opt_bool (const QString &domain,const QString&name,bool def)
{
  if (!domain.isEmpty())
    return _get_opt_bool(domain+"#"+name,_get_opt_bool(name,def));
  else
    return _get_opt_bool(name,def);
}

QString         Options::get_opt_str (const QString &domain,const QString&name,const QString &def)
{
  if (!domain.isEmpty())
    return _get_opt_str(domain+"#"+name,_get_opt_str(name,def));
  else
    return _get_opt_str(name,def);
}

QList<int>     Options::get_opt_intlst (const QString &domain,const QString&name,const QList<int> &def)
{
  QStringList def_str;
  for (QList<int>::const_iterator it=def.begin();it!=def.end();++it)
    def_str.append(QString::number(*it));

  QStringList strlst;
  if (!domain.isEmpty())
    strlst = _get_opt_strlst(domain+"#"+name,_get_opt_strlst(name,def_str));
  else
    strlst = _get_opt_strlst(name,def_str);

  QList<int> ret;
  for (QStringList::const_iterator itStr=strlst.begin();itStr!=strlst.end();++itStr)
    ret.append((*itStr).toInt());
  return ret;
}

QStringList     Options::get_opt_strlst (const QString &domain,const QString&name,const QStringList &def)
{
  if (!domain.isEmpty())
    return _get_opt_strlst(domain+"#"+name,_get_opt_strlst(name,def));
  else
    return _get_opt_strlst(name,def);
}

QByteArray      Options::get_opt_array (const QString &domain,const QString&name, const QByteArray &def)
{
  if (!domain.isEmpty())
    return _get_opt_array(domain+"#"+name,_get_opt_array(name,def));
  else
    return _get_opt_array(name,def);
}

void            Options::set_opt (const QString &domain,const QString&name, double v)
{
  if (!domain.isEmpty())
    _set_opt(domain+"#"+name,v);
  _set_opt(name,v);
}

void            Options::set_opt (const QString &domain,const QString&name, long v)
{
  if (!domain.isEmpty())
    _set_opt(domain+"#"+name,v);
  _set_opt(name,v);
}

void            Options::set_opt (const QString &domain,const QString&name, int v)
{
  if (!domain.isEmpty())
    _set_opt(domain+"#"+name,v);
  _set_opt(name,v);
}

void            Options::set_opt (const QString &domain,const QString&name, bool v)
{
  if (!domain.isEmpty())
    _set_opt(domain+"#"+name,v);
  _set_opt(name,v);
}

void            Options::set_opt (const QString &domain,const QString&name,const  QString &v)
{
  if (!domain.isEmpty())
    _set_opt(domain+"#"+name,v);
  _set_opt(name,v);
}


void            Options::set_opt (const QString &domain,const QString&name,const  QList<int> &def)
{
  QStringList def_str;
  for (QList<int>::const_iterator it=def.begin();it!=def.end();++it)
    def_str.append(QString::number(*it));

  if (!domain.isEmpty())
    _set_opt(domain+"#"+name,def_str);
  _set_opt(name,def_str);
}

void            Options::set_opt (const QString &domain,const QString&name,const  QStringList &v)
{
  if (!domain.isEmpty())
    _set_opt(domain+"#"+name,v);
  _set_opt(name,v);
}

void            Options::set_opt (const QString &domain,const QString&name,const QByteArray &v)
{
  if (!domain.isEmpty())
    _set_opt(domain+"#"+name,v);
  _set_opt(name,v);
}

