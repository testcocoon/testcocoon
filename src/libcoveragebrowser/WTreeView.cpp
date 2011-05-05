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

#include "WTreeView.h"
#include <QMenu>
#include <QMessageBox>
#include <QTextDocument>
#include <Qt>
#include <QFileDialog>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include "options.h"

WTreeView::WTreeView(QWidget *p):QTreeView(p) 
{
   is_menu_enable=true;
   is_tree_view=true;
   setUniformRowHeights(false);
   setAnimated(false);
   header()->setStretchLastSection(true);
}

void WTreeView::saveState()
{
#if QT_VERSION >= 0x040300
   Options::set_opt(QString(),saveRestoreName(),header()->saveState());
#endif
}


void WTreeView::restoreState()
{
#if QT_VERSION >= 0x040300
   header()->restoreState(Options::get_opt_array(QString(),saveRestoreName()));
#endif
}

void WTreeView::contextMenuEvent ( QContextMenuEvent * e )
{
   if (!is_menu_enable)
      return ;

   QMenu *menu_p;
   menu_p = new QMenu(menuName());
   menu_p->addAction(tr("Refresh"),this,SLOT(refreshView()));
   menu_p->addSeparator();
   DecorateContextMenu(menu_p);
   menu_p->addSeparator();
   menu_p->addAction(tr("Adjust size of all columns"),this,SLOT(adjustSize()));
   QMenu *adjust_size_menu_p=menu_p->addMenu(tr("Adjust size"));
   for (int i=0;i<header()->count();i++)
   {
      adjust_size_menu_p->addAction(
            model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString(),
            this,
            SLOT(adjust1Column()));

   }
   QMenu *show_hide_menu_p=menu_p->addMenu(tr("Show/Hide Columns"));
   for (int i=1;i<header()->count();i++)
   {
      QAction *act_p=show_hide_menu_p->addAction(
            model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString());
      act_p->setCheckable(true);
      act_p->setChecked(!header()->isSectionHidden(i));
      connect(act_p,SIGNAL(triggered(bool)),
            this,SLOT(showHideColumn(bool)));
   }

   if (is_tree_view)
   {
      menu_p->addAction(tr("Collapse all"),this,SLOT(collapseAll()));
      menu_p->addAction(tr("Expand all"),this,SLOT(expandAll()));
   }
   menu_p->addAction(tr("Save as..."),this,SLOT(saveAs()));
   menu_p->exec(e->globalPos());
   delete menu_p;
}

void WTreeView::saveAs()
{
  QString csv_coma_filter=tr("CSV file, comma separator (*.csv)");
  QString csv_semi_coma_filter=tr("CSV file, semicolon separator, number with comma (*.csv)");
  QString csv_semi_dot_filter=tr("CSV file, semicolon separator, number with dot (*.csv)");
  QString html_filter=tr("HTML (*.htm *.html)");
  QString selectedFilter=Options::get_opt_str(QString(),"TREEVIEW_SAVE_FILTER","");
  QString filename=QFileDialog::getSaveFileName ( this , tr("Statistics Export"), Options::get_opt_str(QString(),"TREEVIEW_SAVE_FILE",""), 
      csv_coma_filter+";;"+csv_semi_coma_filter+";;"+csv_semi_dot_filter+";;"+html_filter ,
      &selectedFilter,
      FILE_DIALOG_OPTIONS);
  if (filename!=QString())
  {
    QString err;
    SaveAsType type;
    QChar separator,numberSep;
    Options::set_opt(QString(),"TREEVIEW_SAVE_FILE",filename);
    Options::set_opt(QString(),"TREEVIEW_SAVE_FILTER",selectedFilter);
    if (selectedFilter==csv_semi_dot_filter)
    {
      type=CSV;
      separator=';';
      numberSep='.';
    }
    else if (selectedFilter==csv_semi_coma_filter)
    {
      type=CSV;
      separator=';';
      numberSep=',';
    }
    else if (selectedFilter==html_filter)
    {
      type=HTML;
      separator=';';
      numberSep='.';
    }
    else
    {
      type=CSV;
      separator=',';
      numberSep='.';
    }

    QFile f(filename);
    if (!f.open(QFile::WriteOnly))
    {
      QMessageBox::warning(this,
          tr("Error"),
          tr("Could not write file"), 
          QMessageBox::Ok,QMessageBox::NoButton);
      return;
    }
    QTextStream stream(&f);

    if (type==HTML)
      stream << "<html><body><table><TR><TH></TH>";
    for (int i=0;i<header()->count();i++)
    {
      if (!header()->isSectionHidden(i))
      {
        if (type==HTML)
        {
          stream<<"<TH>";
          stream << Qt::escape(model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString());
          stream<<"</TH>";
        }
        else
        {
          stream<<separator;
          stream<<'"';
          stream << model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString().replace('"',"\"\"");
          stream<<'"';
        }
      }
    }
    if (type==HTML)
      stream << "</TR>";

    saveAsSection(stream,QModelIndex(),separator, numberSep, type ,0 );
    if (type==HTML)
      stream << "</table></body></html>";
    f.close();
  }
}

void WTreeView::saveAsSection(QTextStream &stream,QModelIndex parent,QChar separator,QChar numberSep, SaveAsType type, int level)
{
  QModelIndex index;
  int row_count=model()->rowCount(parent);
  for (int lin=0;lin<row_count;lin++)
  {
    QString space=" ";
    if (type==HTML)
    {
      stream << "<TR>";
      space="&nbsp;";
    }
    stream << '\n';
    if (type==HTML)
      stream << "<TD><TT>";
    for (int i=0;i<level-1;i++)
      stream<<space+space+space;
    if (level!=0)
    {
      if (lin==0)
        stream<<"+--";
      else
        stream<<space+space+space;
    }
    if (lin==0 || lin==row_count-1)
      stream<<"+--";
    else
      stream<<"|";
    if (type==HTML)
      stream << "</TT></TD>";
    
    for (int col=0;col<header()->count();col++)
    {
      index=model()->index(lin,col,parent);
      if (!header()->isSectionHidden(col))
      {
        if (type==HTML)
        {
          stream<<"<TD>";
          stream << Qt::escape(model()->data(index,Qt::DisplayRole).toString());
          stream<<"</TD>";
        }
        else
        {
			bool ok;
          stream<<separator;
		  QString str=model()->data(index,Qt::DisplayRole).toString();
		  double f=str.toDouble(&ok);
		  if (!ok && str.right(1)=="%")
		  {
			  f=str.left(str.length()-1).toDouble(&ok);
			  f=f/100.0;
		  }
		  if (ok)
		  {
			  if (numberSep!='.')
				  stream << QString::number(f).replace('.',numberSep);
			  else
				  stream << f ;
		  }
		  else
		  {
			  stream<<'"';
			  stream << str.replace('"',"\"\"");
			  stream<<'"';
		  }
        }
      }
    }
    if (type==HTML)
      stream << "</TR>";
    if (index.isValid() && isExpanded(index))
      saveAsSection(stream,index, separator, numberSep, type, level+1);
  }
}


void WTreeView::adjustSize()
{
   for (int i=0;i<header()->count();i++)
      resizeColumnToContents(i);
}


void WTreeView::adjust1Column()
{
   QAction *action_p=dynamic_cast<QAction*>(sender());
   if (action_p)
   {
      for (int i=0;i<header()->count();i++)
      {
         if (model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString()==action_p->text())
            resizeColumnToContents(i);
      }
   }
}


void WTreeView::showHideColumn(bool b)
{
  QAction *action_p=dynamic_cast<QAction*>(sender());
  if (action_p)
  {
    for (int i=0;i<header()->count();i++)
    {
      if (model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString()==action_p->text())
      {
        if (b)
        {
          header()->showSection(i);
          resizeColumnToContents(i);
        }
        else
        {
          if (header()->sortIndicatorSection()==i)
            header()->setSortIndicator(0,Qt::AscendingOrder);
          header()->hideSection(i);
        }
      }
    }
  }
}

void WTreeView::setMenuEnable(bool b) 
{
   is_menu_enable=b; 
}


void WTreeView::setTreeView(bool b) 
{
   is_tree_view=b; 
}
