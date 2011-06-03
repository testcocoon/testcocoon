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

#include <QMdiArea>
#include <QSignalMapper>
#include <QProgressDialog>
#include "WMain.h"
#include "version.h"
#include "IdleEvent.h"
#include "FileSystemWatcher.h"
#include "WOptimizedExecutionOrder.h"
#include "WProcess.h"
#include "WStatistic.h"
#include "CoverageSettings.h"
#include <QFileDialog>
#include <QDateTime>
#include <QApplication>
#include <QStyleFactory>
#include <QUndoStack>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <QCloseEvent>
#include <QFileInfo>
#include <QList>
#include "coveragebrowser_args.h"
#include <QDockWidget>
#include <QUndoView>
#include <QTreeView>
#include <QVector>
#include <QList>
#ifdef QT_TOOLBAR_DIALOG
#include <qttoolbardialog.h>
#endif
#include <qinputdialog.h>
#include "options.h"
#include "helpclient.h"
#include "Service.h"
#include "WLoadExecution.h"
#include "WExpressionHelp.h"
#include "WLoadExecutionInfo.h"
#include "WOption.h"
#include "WModuleList.h"
#include "WExecutionTextDump.h"
#include "WFunctionList.h"
#include "WExecutionList.h"
#include "instrumentation.h"
#include "csmesbackgroundcomputations.h"
#include <QMdiSubWindow>
#include "WHTMLReport.h"

#define CSEXE_LOAD_STATE_NONE 1
#define CSEXE_LOAD_STATE_GET 2
#define CSEXE_LOAD_STATE_REMOVE 3

WMain::WMain(QWidget* parent, Qt::WindowFlags fl): QMainWindow ( parent, fl )
{
  csmes_file_p=NULL;
  _load_execution_info_p=new WLoadExecutionInfo();

  connect(&CoverageSettings::object(),SIGNAL(modified()), this,SLOT(partialUpdate()));
  CSMesBackgroundComputations::Pauser statistic_pauser;
  setupUi(this);
  fullUpdateEvent = new IdleEvent(this,&WMain::_fullUpdate);
  partialUpdateEvent = new IdleEvent(this,&WMain::_partialUpdate);

  actionCompareCSMes->setEnabled(false);
  menuCompareWithRecentlyOpen->setEnabled(false);
  actionStopComparingCSMes->setEnabled(false);
  actionSwitchCSMesFiles->setEnabled(false);
  actionNewCodeAnalysis->setEnabled(false);
  actionRegressionAnalysis->setEnabled(false);
  actionBlackBoxTestingMode->setEnabled(false);
  setExecutionAnalysisMode(CoverageSettings::object().getExecutionAnalysisMode());

  dwUndoView = new QDockWidget(tr("Undo/Redo"),this);
  dwUndoView->setObjectName("UndoView");
  addDockWidget(Qt::RightDockWidgetArea, dwUndoView);
  execution_comment_title=dwExecutionComment_p->windowTitle();



  changeStyle(Options::get_opt_str(QString(),"STYLE",DEF_WINDOW_STYLE));
  SetWindowTitle();
  importAutomatically.setSingleShot(true);
  connect(&importAutomatically,SIGNAL(timeout()),this,SLOT(importAutomaticallyEvent()));

  actionSeparatorWindow = new QAction(this);
  actionSeparatorWindow->setSeparator(true);
  window_mapper_p = new QSignalMapper(this);
  connect(window_mapper_p, SIGNAL(mapped(QWidget *)),
          this, SLOT(setActiveSubWindow(QWidget *)));

  load_executions_dialog_p=NULL;
  file_system_watcher_p = NULL;
  filter_help_dialog_p=new WExpressionHelp();
  connect(menuRecently_Opened,SIGNAL(aboutToShow()),
          this,SLOT(updateLastRecentOpenedCSMes()));
  connect(menuCompareWithRecentlyOpen,SIGNAL(aboutToShow()),
          this,SLOT(updateCompareWithLastRecentOpenedCSMes()));

  updateCoverageLevel();
  coverageMethodGroup_p=new QActionGroup(this);
  coverageMethodGroup_p->addAction(actionBranch_only);
  coverageMethodGroup_p->addAction(actionCondition_and_Branch);

  csmes_file_p=new CSMesUndoRedoFramework(this);
  csmes_file_p->setStatisticCacheMaxCost(Options::get_opt_long(QString(),"CACHE_STATISTIC",DEF_CACHE_STATISTIC));
  csmes_file_p->setSourceCacheMaxCost(Options::get_opt_long(QString(),"CACHE_SOURCE",DEF_CACHE_SOURCE));
  csmes_file_p->setExecutionCacheMaxCost(Options::get_opt_long(QString(),"CACHE_EXECUTION",DEF_CACHE_EXECUTION));
  WLoadExecution load_exec_tmp(csmes_file_p);
  LoadExecution(load_exec_tmp,true);

  // context menu
  connect(this,SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu(const QPoint&)));
  setContextMenuPolicy(Qt::CustomContextMenu);

  execution_comment_p->setCommentLengthCheck(false);
  execution_comment_p->setEnabled(false);
  actionOptimizedExecutionOrder->setEnabled(false);
  actionTest_Coverage_Count->setEnabled(false);
  coverageLevelAction->setEnabled(false);
  menuCoverage_Method->setEnabled(false);
  generateHtmlReportAction->setEnabled(false);
  actionExecutionTextDump->setEnabled(false);
  exportStatisticModuleAction->setEnabled(false);
  exportStatisticFunctionAction->setEnabled(false);
  exportStatisticEMMAAction->setEnabled(false);
  nextInstrumentationAction->setEnabled(false);
  nextInstrumentationTestedAction->setEnabled(false);
  nextInstrumentationUnTestedAction->setEnabled(false);
  nextInstrumentationManuallyValidatedAction->setEnabled(false);
  nextInstrumentationCommentAction->setEnabled(false);
  nextModuleAction->setEnabled(false);
  previousInstrumentationAction->setEnabled(false);
  previousInstrumentationTestedAction->setEnabled(false);
  previousInstrumentationUnTestedAction->setEnabled(false);
  previousInstrumentationManuallyValidatedAction->setEnabled(false);
  previousInstrumentationCommentAction->setEnabled(false);
  previousModuleAction->setEnabled(false);
  preprocessorViewAction->setEnabled(false);
  findTextAction->setEnabled(false);
  executionBenefitAnalysisAction->setEnabled(false);

  // signals and slots connections
  connect(coverageLevelAction, SIGNAL(triggered()), this, SLOT(coverageLevelChanged()));
  connect(fileSaveCSMesAction, SIGNAL(triggered()), this, SLOT(fileSave()));
  connect(fileExitAction, SIGNAL(triggered()), this, SLOT(fileExit()));
  connect(helpContentsAction, SIGNAL(triggered()), this, SLOT(helpContents()));
  connect(filter_help_p, SIGNAL(triggered()), this, SLOT(filter_help()));
  connect(helpAboutAction, SIGNAL(triggered()), this, SLOT(helpAbout()));
  connect(actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
  connect(LoadExecutionResultAction, SIGNAL(triggered()), this, SLOT(LoadExecutionResult_triggered()));
  connect(clearCommentAction, SIGNAL(triggered()), this, SLOT(clearComment_triggered()));
  connect(clearManuallyValidatedAction, SIGNAL(triggered()), this, SLOT(clearManualValidation_triggered()));
  connect(setCommentAction, SIGNAL(triggered()), this, SLOT(commentInstrumentation_triggered()));
  connect(setManuallyValidatedAction, SIGNAL(triggered()), this, SLOT(manualValidation_triggered()));
  connect(preprocessorViewAction, SIGNAL(toggled(bool)), this, SLOT(preprocessorViewAction_toggled(bool)));
  QFont f;
  f.fromString(Options::get_opt_str(QString(),"FONT_MAIN",DEF_FONT_MAIN ));
  QApplication::setFont(f);
  setFont(f);
  set_MainWindow(this);
  //  setWFlags(Qt::WDestructiveClose);

  // Workspace
  workspace_p = new QMdiArea(this) ;
  setCentralWidget(workspace_p);
  setCoverageMethod(CoverageSettings::object().getCoverageMethod());

  // undo management
  undoAction = csmes_file_p->undoStack()->createUndoAction(this);
  redoAction = csmes_file_p->undoStack()->createRedoAction(this);
  undoAction->setShortcut(QKeySequence("Ctrl+Z"));
  redoAction->setShortcut(QKeySequence("Shift+Ctrl+Z"));
  undoAction->setIcon(QIcon(QPixmap(":/images/commentundo.svg")));
  redoAction->setIcon(QIcon(QPixmap(":/images/commentredo.svg")));
  undoView = new QUndoView(csmes_file_p->undoStack(),dwUndoView);
  undoView->setWindowTitle(tr("Undo/Redo"));
  dwUndoView->setWidget(undoView);
  viewMenu->addAction(dwUndoView->toggleViewAction());

  instrumentationMenu->insertAction(setCommentAction,redoAction);
  instrumentationMenu->insertAction(redoAction,undoAction);
  instrumentationMenu->insertSeparator(setCommentAction);


  toolbar_coverage_settings_p->setEnabled(true);
  toolbar_comparing_p->setEnabled(true);
  toolbar_statistics_p->setEnabled(true);
  toolbar_blackbox_p->setEnabled(true);
  toolbar_source_p->setEnabled(true);
  toolbar_navigation_all_p->setEnabled(true);
  toolbar_navigation_comment_p->setEnabled(true);
  toolbar_navigation_untested_p->setEnabled(true);
  toolbar_navigation_tested_p->setEnabled(true);
  toolbar_navigation_manually_validated_p->setEnabled(true);
  toolbar_navigation_source_p->setEnabled(true);
  dwExecutions_p->toggleViewAction()->setEnabled(false);
  dwExecutionComment_p->toggleViewAction()->setEnabled(false);
  dwStatistics_p->toggleViewAction()->setEnabled(false);
  dwModules_p->toggleViewAction()->setEnabled(false);
  dwExplanation_p->toggleViewAction()->setEnabled(false);
  dwFunctions_p->toggleViewAction()->setEnabled(false);
  viewSourceAction->setEnabled(false);

  // Executions
  connect( executions_p,SIGNAL( setExplanation(const QString &)),
           explanation_p ,SLOT( setExplanation( const QString &) ) ) ;
  connect( executions_p,SIGNAL( setExecutionComment(const QString &,const QString &)),
           this ,SLOT( _setExecutionComment( const QString &,const QString &) ) ) ;
  connect( execution_comment_p,SIGNAL( accept()),
           this ,SLOT( _saveExecutionComment( ) ) ) ;
  connect( executions_p,SIGNAL( setExecutionCommentAvailable(bool)),
           execution_comment_p ,SLOT( setEnabled( bool ) ) ) ;
  executions_p->setEnabled(false);

  // Statistics
  statistics_p->setCSMes(csmes_file_p);
  statistics_p->setEnabled(false);


  // Explanation
  multiplexer.connect (SIGNAL(setExplanationOriginal(const QString &,const QString &,const QList<int>&,int ,Instrumentation::coverage_method_t,int)),
                       explanation_p,SLOT(setExplanationOriginal(const QString &,const QString &,const QList<int>&,int ,Instrumentation::coverage_method_t,int)));
  multiplexer.connect (SIGNAL(setExplanationPreprocessed(const QString &,const QString &,const QList<int>&,int ,Instrumentation::coverage_method_t,int)),
                       explanation_p,SLOT(setExplanationPreprocessed(const QString &,const QString &,const QList<int>&,int ,Instrumentation::coverage_method_t,int)));
  explanation_p->setCSMes(csmes_file_p);

  // Functions Browser
  functions_browser_p->setCSMes(csmes_file_p);
  functions_browser_p->setStatusColorLevelFunction(Options::get_opt_double(QString(),"FUNCTION_LOW_COLOR_LEVEL",DEF_FUNCTION_LOW_COLOR_LEVEL),Options::get_opt_double(QString(),"FUNCTION_MEDIUM_COLOR_LEVEL",DEF_FUNCTION_MEDIUM_COLOR_LEVEL));
  functions_browser_p->setStatusColorLevelClass(Options::get_opt_double(QString(),"CLASS_LOW_COLOR_LEVEL",DEF_CLASS_LOW_COLOR_LEVEL),Options::get_opt_double(QString(),"CLASS_MEDIUM_COLOR_LEVEL",DEF_CLASS_MEDIUM_COLOR_LEVEL));
  connect( functions_browser_p,SIGNAL( selectedSource(const ModuleFile &,const SourceFile &,CSMesUndoRedoFramework::source_type_t,int,int,int,int)),
           this ,SLOT( showSource(const ModuleFile &,const SourceFile &,CSMesUndoRedoFramework::source_type_t,int,int,int,int) ) ) ;
  functions_browser_p->setEnabled(false);

  // Source browser
  modules_browser_p->setObjectName("context list view" );
  modules_browser_p->setStatusColorLevel(Options::get_opt_double(QString(),"MODULE_LOW_COLOR_LEVEL",DEF_MODULE_LOW_COLOR_LEVEL),Options::get_opt_double(QString(),"MODULE_MEDIUM_COLOR_LEVEL",DEF_MODULE_MEDIUM_COLOR_LEVEL));
  modules_browser_p->setCSMes(csmes_file_p);

  connect( modules_browser_p,SIGNAL( selectedSource(const ModuleFile &,const SourceFile &,CSMesUndoRedoFramework::source_type_t,int,int,int,int)),
           this ,SLOT( showSource(const ModuleFile &,const SourceFile &,CSMesUndoRedoFramework::source_type_t,int,int,int,int) ) ) ;
  modules_browser_p->setEnabled(false);

  multiplexer.connect (SIGNAL(updateViewType(CSMesUndoRedoFramework::source_type_t)),this,SLOT(setViewType(CSMesUndoRedoFramework::source_type_t)));
  multiplexer.connect (this,SIGNAL(firstInstrumentation(Instrumentation::filter_t)),SLOT(firstInstrumentation(Instrumentation::filter_t)));
  multiplexer.connect (this,SIGNAL(lastInstrumentation(Instrumentation::filter_t)),SLOT(lastInstrumentation(Instrumentation::filter_t)));
  multiplexer.connect (this,SIGNAL(nextInstrumentation(Instrumentation::filter_t)),SLOT(next(Instrumentation::filter_t)));
  multiplexer.connect (this,SIGNAL(previousInstrumentation(Instrumentation::filter_t)),SLOT(previous(Instrumentation::filter_t)));
  multiplexer.connect (this,SIGNAL(searchFilterChanged(Instrumentation::filter_t)),SLOT(setSearchFilter(Instrumentation::filter_t)));

  multiplexer.connect (this,SIGNAL(clearComment()),SLOT(clearComment()));
  multiplexer.connect (this,SIGNAL(clearManuallyValidated()),SLOT(clearManuallyValidated()));
  multiplexer.connect (this,SIGNAL(showFindText()),SLOT(showFindText()));
  multiplexer.connect (this,SIGNAL(setComment()),SLOT(setComment()));
  multiplexer.connect (this,SIGNAL(setManuallyValidated()),SLOT(setManuallyValidated()));
  multiplexer.connect (SIGNAL(editPossibilities(bool ,bool , bool , bool , bool)),this,SLOT(editPossibilities(bool ,bool , bool , bool ,bool )));

  connect( workspace_p, SIGNAL(subWindowActivated(QMdiSubWindow *)),
           this, SLOT( currentWindowChanged(QMdiSubWindow *) ) );

  fileSaveCSMesAction->setEnabled(false);
  saveBlackBoxConfigurationAction->setEnabled(false);
  fileMergeCSMesAction->setEnabled(false);
  actionImportUnitTests->setEnabled(false);
  LoadExecutionResultAction->setEnabled(false);
#ifdef QT_TOOLBAR_DIALOG
  toolbar_manager_p=new QtToolBarManager(this);
  toolbar_manager_p->setMainWindow(this);
  toolbar_manager_p->addToolBar(toolbar_file_p,tr("File"));
  toolbar_manager_p->addToolBar(toolbar_navigation_p,tr("Navigation"));
  toolbar_manager_p->addToolBar(toolbar_instrumentation_p,tr("Instrumentation"));
  toolbar_manager_p->addToolBar(toolbar_tools_p,tr("Tools"));
#endif
  workspace_p->closeAllSubWindows();
#ifdef QT_TOOLBAR_DIALOG
  toolbar_manager_p->restoreState(Options::get_opt_array(getWorkingModeStr(),"TOOLBAR_SETTING"));
#endif

  setWorkingMode(getWorkingMode(),true);

  if (Options::get_opt_str(QString(),"CSMES_FILENAME",QString()).length()>0)
  {
    compareCSMes(Options::get_opt_str(QString(),"CSMES_BROWSER_COMPARE_CSMES_FILENAME",QString()));
    QString err;
    QString filename=Options::get_opt_str(QString(),"CSMES_FILENAME",QString());
    if ((err=openCSMesFile(filename))!=QString())
    {
      QMessageBox::warning(this,
          tr("Could not load the file '%1'").arg(filename),
          csmes_file_p->error(),
          QMessageBox::Close);
      Options::set_opt(QString(),"CSMES_FILENAME",filename);
    }
    setReleaseComparaisonMode(CoverageSettings::object().getReleaseComparaisonMode());
  }
  if (arg_csexe_filename()!=QString())
  {
    Options::set_opt(QString(),"CSEXE_FILE",arg_csexe_filename());
    LoadExecutionResult_triggered();
  }

  editPossibilities(false,false,false,false,false);
  actionTest_Coverage_Count->setChecked(CoverageSettings::object().getTestCoverageMode());
  setViewType(CoverageSettings::object().getSourceType());

  createWindowMenu();
  createViewMenu();
  fullUpdate();
  QStringList execs=selectedExecutions();
  QStringList cmps=selectedExecutionsComparaison();
  selectExecutions(QStringList(),QStringList(),getTestCoverageMode(),getCoverageMethod(),getExecutionAnalysisMode(),getReleaseComparaisonMode(),getCoverageLevel());
  selectExecutions(execs,cmps,getTestCoverageMode(),getCoverageMethod(),getExecutionAnalysisMode(),getReleaseComparaisonMode(),getCoverageLevel());
  csmes_file_p->clearUndoStack();
}

WMain::~WMain()
{
  CSMesBackgroundComputations::GetObject().setCSMes(NULL);
  delete dwUndoView;
  if (file_system_watcher_p)
    delete file_system_watcher_p;
  delete filter_help_dialog_p;
  delete coverageMethodGroup_p;
  delete csmes_file_p;
  delete actionSeparatorWindow;
  delete fullUpdateEvent;
  delete partialUpdateEvent;
  delete _load_execution_info_p;
}

static bool load_csexe_progress(float f,bool finished)
{
  static QProgressDialog *load_csexe_progress_p=NULL;
  if (finished)
  {
    if (load_csexe_progress_p)
      delete load_csexe_progress_p;
    load_csexe_progress_p=NULL;
    return true;
  }
  else
  {
    if (load_csexe_progress_p==NULL)
    {
      load_csexe_progress_p=new QProgressDialog(QObject::tr("Loading execution report in progress..."),QObject::tr("Interrupt"),0,1000);
      load_csexe_progress_p->setWindowModality(Qt::WindowModal);
    }
    int v=1000*f;
    if (v<0) v=0;
    if (v>1000) v=1000;
    load_csexe_progress_p->setValue(v);
    return (load_csexe_progress_p->wasCanceled());
  }
}


void WMain::createViewMenu()
{
  viewMenu->addAction(dwExecutions_p->toggleViewAction());
  viewMenu->addAction(dwExecutionComment_p->toggleViewAction());
  viewMenu->addAction(dwStatistics_p->toggleViewAction());
  viewMenu->addAction(dwModules_p->toggleViewAction());
  viewMenu->addAction(dwExplanation_p->toggleViewAction());
  viewMenu->addAction(dwFunctions_p->toggleViewAction());
  viewMenu->addSeparator();
  viewMenu->addAction(toolbar_file_p->toggleViewAction());
  viewMenu->addAction(toolbar_navigation_all_p->toggleViewAction());
  viewMenu->addAction(toolbar_navigation_comment_p->toggleViewAction());
  viewMenu->addAction(toolbar_navigation_manually_validated_p->toggleViewAction());
  viewMenu->addAction(toolbar_navigation_tested_p->toggleViewAction());
  viewMenu->addAction(toolbar_navigation_untested_p->toggleViewAction());
  viewMenu->addAction(toolbar_navigation_source_p->toggleViewAction());
  viewMenu->addAction(toolbar_instrumentation_p->toggleViewAction());
  viewMenu->addAction(toolbar_coverage_settings_p->toggleViewAction());
  viewMenu->addAction(toolbar_comparing_p->toggleViewAction());
  viewMenu->addAction(toolbar_statistics_p->toggleViewAction());
  viewMenu->addAction(toolbar_blackbox_p->toggleViewAction());
  viewMenu->addAction(toolbar_source_p->toggleViewAction());
}


void WMain::fileSave()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (!saveCSMesFile())
  {
    QMessageBox::warning(this,
                         tr("Could not save the file '%1'").arg(csmes_file_p->getFilename()),
                         csmes_file_p->error(),
                         QMessageBox::Close);
  }
}


void WMain::fileExit()
{
  if (tryExit())
    qApp->quit();
}


void WMain::helpContents()
{
  HelpClient::OpenPDFHelp();
}


void WMain::filter_help()
{
  filter_help_dialog_p->show();
}

void WMain::aboutQt()
{
  QApplication::aboutQt();
}

void WMain::helpAbout()
{
  QString version=QString::number((TESTCOCOON_VERSION>>16)&0xFF)+QString(".")+
                  QString::number((TESTCOCOON_VERSION>>8)&0xFF)+QString(".")+
                  QString::number((TESTCOCOON_VERSION)&0xFF);

  QMessageBox::about(this,
                     tr("About CoverageBrowser"),
                     "<html><body>"
                     "<center>"
                     "<B>CoverageBrowser</B><br>"
                     "Part of <a href=\"http://www.testcocoon.org\">TestCocoon</a> software suite."
                     "</center>"
                     "<br><font size=\"-1\">"
                     +tr("Version:")+version+"<br>"
                     +tr("Date:")+__DATE__" "__TIME__
                     "</font>"
                     "<BR>"
                     "<p>Icon derivared from <a href=\"http://art.gnome.org/download/themes/icon/1261/ICON-TangoMateria.tar.gz\">Tango-Materia</a> by Marco Sordi and <a href=\"http://art.gnome.org/download/themes/icon/1150/ICON-UnofficialTango.tar.bz2\">Unofficial Tango</A> by Jones Lee </P>"
                     "<p>Graphical interface generated using Qt toolkit from Nokia</P>"
                     "</body></html>"
      );
}


void WMain::on_viewSourceAction_triggered()
{
  newSourceWindow (modules_browser_p->Module(),modules_browser_p->Source());
}

QMdiSubWindow *WMain::newSourceWindow (const QString &module,const QString &source)
{
  WSourceView *t = new WSourceView( workspace_p );
  QMdiSubWindow *subwindow_p=workspace_p->addSubWindow(t);
  connect( t,SIGNAL( requestFindTextModule ( const QString & , QTextDocument::FindFlags ) ), this ,SLOT( requestFindTextModule ( const QString & , QTextDocument::FindFlags ) ) ) ;
  connect( t,SIGNAL( requestNextModule(Instrumentation::filter_t)), this ,SLOT(  requestNextModule(Instrumentation::filter_t)) ) ;
  connect( t,SIGNAL( requestPreviousModule(Instrumentation::filter_t)), this ,SLOT(  requestPreviousModule(Instrumentation::filter_t)) ) ;
  t->setCSMes(csmes_file_p);
  t->show();

  multiplexer.setCurrentObject( t );
  emitViewType();
  emit searchFilterChanged(Instrumentation::FILTER_NONE);
  showSource(module,source,getViewType(), -1,-1,-1,-1);
  return subwindow_p;
}

void WMain::currentWindowChanged( QMdiSubWindow* w )
{
  if (w)
  {
    multiplexer.setCurrentObject( w->widget() );
    WSourceView *source_p=dynamic_cast<WSourceView*>(w->widget());
    if (source_p)
    {
      setViewType(source_p->getViewType());
    }
  }
}

void WMain::LoadExecutionResult_triggered()
{
  if (load_executions_dialog_p)
    return ;
  load_executions_dialog_p = new WLoadExecution(csmes_file_p);
  bool load_csexe = load_executions_dialog_p->exec();
  if (load_csexe)
  {
    bool set_watch_only=load_executions_dialog_p->applyOnly();
    LoadExecution(*load_executions_dialog_p,set_watch_only);
  }
  delete load_executions_dialog_p;
  load_executions_dialog_p=NULL;
}

void WMain::LoadExecution(const WLoadExecution &exe,bool set_watch_only)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (file_system_watcher_p)
    delete file_system_watcher_p;
  file_system_watcher_p=NULL;
  csexe_type=exe.type();
  csexe_name=exe.name();
  csexe_importPolicy=exe.importPolicy();
  csexe_default_status=exe.defaultStatus();
  csexe_minimize_memory_usage=exe.minimizeMemoryUsage();
  switch (csexe_type)
  {
    case WLoadExecution::CSEXE_FILE:
      csexe_file=exe.file();
      csexe_deleteAfterLoading=exe.deleteAfterLoading();
      csexe_watch=exe.watch();

      if (!set_watch_only)
        loadCSExeFile(csexe_file,csexe_name,csexe_importPolicy,csexe_deleteAfterLoading,csexe_default_status,csexe_minimize_memory_usage);
      file_system_watcher_p = new FileSystemWatcher(csexe_file);
      connect(file_system_watcher_p,SIGNAL(fileChanged()),this,SLOT(CSExeFileModified()));
      break;
    case WLoadExecution::CSEXE_SCRIPT:
      csexe_script=exe.script();
      csexe_arguments=exe.arguments();
      if (!set_watch_only)
        loadCSExeCommand(csexe_script,csexe_arguments,csexe_name,csexe_importPolicy,csexe_default_status,csexe_minimize_memory_usage);
      break;
  }
}

void WMain::loadCSExeCommand(const QString &command,const QStringList &arguments,const QString &prefix,CSMesUndoRedoFramework::csexe_import_policy_t policy,Executions::execution_status_t default_execution_status,bool minimize_memory_usage)
{
  QString info,short_status;
  QStringList new_measurements;
  WProcess *command_p=new WProcess;
  QByteArray csexe_data;
  command_p->setPrintStdOutput(false);
  command_p->setStdOutput(&csexe_data);
  int retval=command_p->execute(command,arguments,tr("Reading Execution Report..."),true);
  delete command_p;
  switch (retval)
  {
    case 0:
      if (!csmes_file_p->loadCSExe( csexe_data , prefix ,policy ,default_execution_status,new_measurements,info , short_status,!minimize_memory_usage,load_csexe_progress))
      {
        QMessageBox::warning(this,
            tr("Could not load the file"),
            csmes_file_p->error(),
            QMessageBox::Close);
      }
      else
      {
        printStatus(short_status);
        if (info!=QString())
        {
          _load_execution_info_p->setInfo(  info);
          _load_execution_info_p->show();
        }
    else
        _load_execution_info_p->show();

        selectExecutions(selectedExecutions()+new_measurements,selectedExecutionsComparaison(),getTestCoverageMode(),getCoverageMethod(),getExecutionAnalysisMode(),getReleaseComparaisonMode(),getCoverageLevel());
      }
      break;
    case -2:
      break;
    case -1:
      QMessageBox::warning(this,
          tr("Execution Error"),
          tr("Could not execute:")+command,
          QMessageBox::Close);
      break;
    default:
      QMessageBox::warning(this,
          tr("Execution Error"),
          tr("Exit value of %1 is %2").arg(command).arg(QString::number(retval)),
          QMessageBox::Close);
      break;
  }
}

void WMain::loadCSExeFile(const QString &filename,const QString &prefix,CSMesUndoRedoFramework::csexe_import_policy_t policy,bool delete_after,Executions::execution_status_t default_execution_status,bool minimize_memory_usage)
{
  QString info,short_status;
  QStringList new_measurements;
  if (!csmes_file_p->loadCSExe( filename , prefix ,policy , default_execution_status,new_measurements,info,short_status ,!minimize_memory_usage,load_csexe_progress))
  {
    QMessageBox::warning(this,
        tr("Could not load the file"),
        csmes_file_p->error(),
        QMessageBox::Close);
    return;
  }
  else
  {
    if (delete_after)
    {
      QFile f(filename);
      if (!f.remove())
      {
        QMessageBox::warning(this,
            tr("Delete file"),
            tr("Error: file '%1' is not deleted.").arg(filename),
            QMessageBox::Close);
      }

    }
    if (info!=QString() && Options::get_opt_bool(QString(),"DISPLAY_IMPORT_SUMMARY",true))
    {
      _load_execution_info_p->setInfo(info);
      _load_execution_info_p->show();
    }
    else
      _load_execution_info_p->hide();
    printStatus(short_status);
    selectExecutions(selectedExecutions()+new_measurements,selectedExecutionsComparaison(),getTestCoverageMode(),getCoverageMethod(),getExecutionAnalysisMode(),getReleaseComparaisonMode(),getCoverageLevel());
  }
}


QString WMain::openCSMesFile(const QString &filename)
{
  CSMesBackgroundComputations::GetObject().setCSMes(NULL);
  if (csmes_file_p->loadCSMes( filename ))
  {
    CSMesBackgroundComputations::GetObject().setCSMes(csmes_file_p);
    if (csmes_file_p->isBlackBoxConfiguration())
    {
      setWorkingMode(CoverageSettings::WORKING_MODE_BLACK_BOX);
      actionBlackBoxTestingMode->setEnabled(false);
    }
    else
    {
      setWorkingMode(CoverageSettings::WORKING_MODE_WHITE_BOX);
      actionBlackBoxTestingMode->setEnabled(true);
    }
    addLastRecentOpenedCSMes(filename);
    Options::set_opt(QString(),"CSMES_FILENAME",filename);
    modules_browser_p->setCSMes(csmes_file_p);
    functions_browser_p->setCSMes(csmes_file_p);

    actionCompareCSMes->setEnabled(true);
    menuCompareWithRecentlyOpen->setEnabled(true);
    executions_p->setEnabled(true);
    statistics_p->setEnabled(true);
    functions_browser_p->setEnabled(true);
    modules_browser_p->setEnabled(true);
    actionOptimizedExecutionOrder->setEnabled(true);
    actionTest_Coverage_Count->setEnabled(true);
    coverageLevelAction->setEnabled(true);
    menuCoverage_Method->setEnabled(true);
    exportStatisticFunctionAction->setEnabled(true);
    exportStatisticEMMAAction->setEnabled(true);
    generateHtmlReportAction->setEnabled(true);
    actionExecutionTextDump->setEnabled(true);
    exportStatisticModuleAction->setEnabled(true);
    fileSaveCSMesAction->setEnabled(true);
    saveBlackBoxConfigurationAction->setEnabled(true);
    fileMergeCSMesAction->setEnabled(true);
    actionImportUnitTests->setEnabled(true);
    LoadExecutionResultAction->setEnabled(true);
    nextInstrumentationTestedAction->setEnabled(true);
    nextInstrumentationUnTestedAction->setEnabled(true);
    nextInstrumentationManuallyValidatedAction->setEnabled(true);
    nextInstrumentationCommentAction->setEnabled(true);
    nextInstrumentationAction->setEnabled(true);
    nextModuleAction->setEnabled(true);
    previousInstrumentationAction->setEnabled(true);
    previousInstrumentationTestedAction->setEnabled(true);
    previousInstrumentationUnTestedAction->setEnabled(true);
    previousInstrumentationCommentAction->setEnabled(true);
    previousInstrumentationManuallyValidatedAction->setEnabled(true);
    previousModuleAction->setEnabled(true);
    preprocessorViewAction->setEnabled(true);
    findTextAction->setEnabled(true);
    executionBenefitAnalysisAction->setEnabled(true);
    dwStatistics_p->toggleViewAction()->setEnabled(true);
    dwExecutions_p->toggleViewAction()->setEnabled(true);
    dwExecutionComment_p->toggleViewAction()->setEnabled(true);
    dwExplanation_p->toggleViewAction()->setEnabled(true);
    dwFunctions_p->toggleViewAction()->setEnabled(true);
    dwModules_p->toggleViewAction()->setEnabled(true);
    viewSourceAction->setEnabled(true);
    menuCoverage_Method->setEnabled(csmes_file_p->hasConditionInstrumentation());

    executions_p->setCSMes(csmes_file_p);
    statistics_p->setCSMes(csmes_file_p);
    fullUpdate();
    csmes_file_p->clearUndoStack();
    return QString();
  }
  else
  {
    CSMesBackgroundComputations::GetObject().setCSMes(csmes_file_p);
    QString err = csmes_file_p->error();
    if (err==QString())
      return "Undefined error";
    return err;
  }
}

bool WMain::csmesFileSaved()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (csmes_file_p->modificationFlag())
  {
    QString title=tr("Loading Measurements");
    QString text=tr("Measurement file %1 not saved.\nDo you really want to load an other file?").arg(csmes_file_p->getFilename());
    switch (QMessageBox::warning(this,title, text,
          QMessageBox::Yes,QMessageBox::No|QMessageBox::Default| QMessageBox::Escape))
    {
      case QMessageBox::Yes:
        return true;
      default:
        return false;
    }
  }
  return true;
}

bool WMain::compareCSMes(const QString &filename)
{
  bool ret=false;
   if (filename.isEmpty() || getWorkingMode()==CoverageSettings::WORKING_MODE_BLACK_BOX)
   {
      csmes_file_p->compareCSMes(QString());
      actionStopComparingCSMes->setEnabled(false);
      actionSwitchCSMesFiles->setChecked(false);
      actionNewCodeAnalysis->setChecked(false);
      actionNewCodeAnalysis->setEnabled(false);
      actionSwitchCSMesFiles->setEnabled(false);
      actionRegressionAnalysis->setChecked(false);
      actionRegressionAnalysis->setEnabled(false);
      actionCompareCSMes->setEnabled(true);
      menuCompareWithRecentlyOpen->setEnabled(true);
      ret=true;
   }
   else
   {
      ret = csmes_file_p->compareCSMes(filename);
      actionStopComparingCSMes->setEnabled(true);
      actionSwitchCSMesFiles->setEnabled(true);
      actionNewCodeAnalysis->setEnabled(true);
      actionRegressionAnalysis->setEnabled(true);
      actionCompareCSMes->setEnabled(true);
      menuCompareWithRecentlyOpen->setEnabled(true);
   }
   menuCoverage_Method->setEnabled(csmes_file_p->hasConditionInstrumentation());
   Options::set_opt(getWorkingModeStr(),"CSMES_BROWSER_COMPARE_CSMES_FILENAME",filename);
   return ret;
}

void WMain::on_actionNewCodeAnalysis_toggled(bool t)
{
  if (t)
    actionRegressionAnalysis->setChecked(false);
  selectExecutions(selectedExecutions(),selectedExecutionsComparaison(),getTestCoverageMode(),getCoverageMethod(),getExecutionAnalysisMode(),getReleaseComparaisonMode(),getCoverageLevel());
}

void WMain::on_actionRegressionAnalysis_toggled(bool t)
{
  if (t)
    actionNewCodeAnalysis->setChecked(false);
  selectExecutions(selectedExecutions(),selectedExecutionsComparaison(),getTestCoverageMode(),getCoverageMethod(),getExecutionAnalysisMode(),getReleaseComparaisonMode(),getCoverageLevel());
}

void WMain::on_actionSwitchCSMesFiles_triggered()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (csmes_file_p->modificationFlag())
  {
    QString title=tr("Merging Measurements");
    QString text=tr("Measurement file %1 not saved.").arg(csmes_file_p->getFilename());
    QMessageBox::warning(this,title, text, QMessageBox::Ok);
    return ;
  }
  QString csmes_filename=csmes_file_p->getFilename();
  QString csmes_filename_reference=csmes_file_p->getFilenameReference();
  if (csmes_filename.isEmpty())
  {
    QMessageBox::warning(this,tr("Warning"),tr("No CSMes file opened."));
    return;
  }
  if (csmes_filename.isEmpty())
  {
    QMessageBox::warning(this,tr("Warning"),tr("No CSMes reference file opened."));
    return;
  }
  csmes_file_p->closeCSMes( ) ;
  csmes_file_p->loadCSMes(csmes_filename_reference);
  compareCSMes(csmes_filename);
  fullUpdate();
}

QString WMain::closeCSMesRefenceFile()
{
  compareCSMes(QString());
  fullUpdate();
  return QString();
}


QString WMain::openCSMesRefenceFile(const QString &file)
{
  bool ret = compareCSMes(file);
  fullUpdate();
  if (ret)
    return QString();
  else
    return tr("Error comparing with file '%1'").arg(file);
}

void WMain::on_actionStopComparingCSMes_triggered()
{
  compareCSMes(QString());
  fullUpdate();
}

void WMain::on_actionCompareCSMes_triggered()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  QString filename =Options::get_opt_str(QString(),"CSMES_BROWSER_COMPARE_CSMES_FILENAME",QString());
  QString selectedFilters = Options::get_opt_str(QString(),"CSMES_BROWSER_COMPARE_CSMES_FILENAME_FILTERS",QString());
  if ( (filename = QFileDialog::getOpenFileName(this,
          tr("Compare with..."),
          filename,
#ifdef OS_WIN32
          tr("Measurements (*.exe.csmes *.lib.csmes *.dll.csmes);;Measurements (*.csmes)"),
#else
          tr("Measurements (*.csmes)"),
#endif
          &selectedFilters,
          FILE_DIALOG_OPTIONS
          )) != QString())
  {
    Options::set_opt(QString(),"CSMES_BROWSER_COMPARE_CSMES_FILENAME_FILTERS",selectedFilters);
    addLastRecentOpenedCSMes(filename);
    compareCSMes(filename);
    fullUpdate();
  }
}

void WMain::on_fileOpenCSMesAction_triggered()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (!csmesFileSaved())
    return;
  QString filename =Options::get_opt_str(QString(),"CSMES_BROWSER_FILENAME",QString());
  QString selectedFilters = Options::get_opt_str(QString(),"CSMES_BROWSER_FILENAME_FILTERS",QString());
  if ( (filename = QFileDialog::getOpenFileName(this,
          tr("Open Measurements"),
          filename,
#ifdef OS_WIN32
      tr("Measurements (*.exe.csmes *.lib.csmes *.dll.csmes);;Measurements (*.csmes)"),
#else
      tr("Measurements (*.csmes)"),
#endif
      &selectedFilters,
      FILE_DIALOG_OPTIONS
      )) != QString())
  {
    Options::set_opt(QString(),"CSMES_BROWSER_FILENAME_FILTERS",selectedFilters);
    Options::set_opt(QString(),"CSMES_BROWSER_FILENAME",filename);
    QString err;
    if ((err=openCSMesFile(filename))!=QString())
    {
        QMessageBox::warning(NULL,
                tr("Could not load the file '%1'").arg(filename),
                csmes_file_p->error(),
                QMessageBox::Close);
        Options::set_opt(QString(),"CSMES_FILENAME",QString());
    }
  }
}

void WMain::mergeFiles(const QStringList &filenames,CSMESFile::merge_policy_t policy)
{
  int nb_file=filenames.size();
  int i_file=0;
  for (QStringList::const_iterator filename=filenames.begin();filename!=filenames.end();++filename,++i_file)
  {
    printStatus(tr("Processing file '%1'...").arg(*filename),static_cast<float>(i_file)/static_cast<float>(nb_file));
    QString err;
    if (!csmes_file_p->mergeCSMes(*filename,policy))
    {
      QFileInfo file(*filename);
      QMessageBox::warning(NULL,
          tr("Could not merge contain with the file '%1'").arg(file.fileName()),
          tr("Merge with file:'%1'\n\n").arg(file.fileName())+"\n\n"+
          csmes_file_p->error(),
          QMessageBox::Close);
    }
  }
  fullUpdate();
}

void WMain::on_actionImportUnitTests_triggered()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (csmes_file_p->modificationFlag())
  {
    QString title=tr("Import Unit Test");
    QString text=tr("Measurement file %1 not saved.").arg(csmes_file_p->getFilename());
    QMessageBox::warning(this,title, text, QMessageBox::Ok);
    return ;
  }

  QStringList filenames ;
  QString merge_default_directory=Options::get_opt_str(QString(),"MERGE_UNITTEST_DEFAULT_DIRECTORY",QString());
  QString selectedFilters = Options::get_opt_str(QString(),"MERGE_UNITTEST_DEFAULT_DIRECTORY_FILTER",QString());

  filenames = QFileDialog::getOpenFileNames(this,
      tr("Import Unit Tests"),
      merge_default_directory,
#ifdef OS_WIN32
      tr("Measurements (*.exe.csmes *.lib.csmes *.dll.csmes);;Measurements (*.csmes)"),
#else
      tr("Measurements (*.csmes)"),
#endif
      &selectedFilters,
      FILE_DIALOG_OPTIONS
      );

  if (!filenames.isEmpty())
  {
    QFileInfo filename_info(filenames.first());
    merge_default_directory=filename_info.absolutePath();
    Options::set_opt(QString(),"MERGE_UNITTEST_DEFAULT_DIRECTORY",merge_default_directory);
    Options::set_opt(QString(),"MERGE_UNITTEST_DEFAULT_DIRECTORY_FILTER",selectedFilters);
    mergeFiles(filenames,CSMESFile::ONLY_INSTRUMENTED_SOURCES);
  }
}

void WMain::on_fileMergeCSMesAction_triggered()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (csmes_file_p->modificationFlag())
  {
    QString title=tr("Merging Measurements");
    QString text=tr("Measurement file %1 not saved.").arg(csmes_file_p->getFilename());
    QMessageBox::warning(this,title, text, QMessageBox::Ok);
    return ;
  }

  QStringList filenames ;
  QString merge_default_directory=Options::get_opt_str(QString(),"MERGE_DEFAULT_DIRECTORY",QString());
  QString selectedFilters = Options::get_opt_str(QString(),"MERGE_DEFAULT_DIRECTORY_FILTER",QString());

  filenames = QFileDialog::getOpenFileNames(this,
      tr("Merge Measurements"),
      merge_default_directory,
#ifdef OS_WIN32
      tr("Measurements (*.exe.csmes *.lib.csmes *.dll.csmes);;Measurements (*.csmes)"),
#else
      tr("Measurements (*.csmes)"),
#endif
      &selectedFilters,
      FILE_DIALOG_OPTIONS
      );
  if (!filenames.isEmpty())
  {
    QFileInfo filename_info(filenames.first());
    merge_default_directory=filename_info.absolutePath();
    Options::set_opt(QString(),"MERGE_DEFAULT_DIRECTORY",merge_default_directory);
    Options::set_opt(QString(),"MERGE_DEFAULT_DIRECTORY_FILTER",selectedFilters);
    mergeFiles(filenames,CSMESFile::FULL);
  }
}

void WMain::on_Options_triggered()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  WOption *option_p;
  option_p = new WOption(this);
  int cache_hit,cache_miss,max_cost,cost;
  csmes_file_p->getSourceCacheStatistics(cache_hit,cache_miss,max_cost,cost);
  option_p->setSourceCacheStatistics(cache_hit,cache_miss,max_cost,cost);
  csmes_file_p->getStatisticCacheStatistics(cache_hit,cache_miss,max_cost,cost);
  option_p->setStatisticCacheStatistics(cache_hit,cache_miss,max_cost,cost);
  csmes_file_p->getExecutionCacheStatistics(cache_hit,cache_miss,max_cost,cost);
  option_p->setExecutionCacheStatistics(cache_hit,cache_miss,max_cost,cost);
  if (option_p->exec())
  {
    if (modules_browser_p!=NULL)
    {
      modules_browser_p->setStatusColorLevel(option_p->module_low_level(),option_p->module_medium_level());
    }
    if (functions_browser_p!=NULL)
    {
        functions_browser_p->setStatusColorLevelFunction(option_p->function_low_level(),option_p->function_medium_level());
        functions_browser_p->setStatusColorLevelClass(option_p->class_low_level(),option_p->class_medium_level());
    }

    csmes_file_p->setStatisticCacheMaxCost(option_p->getStatisticCacheMaxCost());
    Options::set_opt(QString(),"CACHE_STATISTIC",option_p->getStatisticCacheMaxCost());
    csmes_file_p->setSourceCacheMaxCost(option_p->getSourceCacheMaxCost());
    Options::set_opt(QString(),"CACHE_SOURCE",option_p->getSourceCacheMaxCost());
    csmes_file_p->setExecutionCacheMaxCost(option_p->getExecutionCacheMaxCost());
    Options::set_opt(QString(),"CACHE_EXECUTION",option_p->getExecutionCacheMaxCost());
    Options::set_opt(QString(),"LANGUAGE",option_p->language());
    changeStyle(option_p->window_style());
    partialUpdate();
    QFont f;
    f.fromString(Options::get_opt_str(QString(),"FONT_MAIN",DEF_FONT_MAIN));
    QApplication::setFont(f);
  }
}

void WMain::closeEvent( QCloseEvent *e )
{
  if (!tryExit())
    e->ignore();
  else
  {
    e->accept();
  }
}

void WMain::showSource(const ModuleFile &module,const SourceFile &source,CSMesUndoRedoFramework::source_type_t t,int start_line,int start_column,int end_line,int end_column)
{
  setViewType(t);
  if (workspace_p->isHidden())
    return ;

  if (source.isEmpty() && module.isEmpty())
    return ;

  if (source.isEmpty())
    cur_source=module;
  else
    cur_source=source;
  WSourceView *source_view_p= NULL;
  if (workspace_p->activeSubWindow())
    source_view_p=dynamic_cast<WSourceView*> (workspace_p->activeSubWindow()->widget());
  if ( (source_view_p==NULL) && (workspace_p->currentSubWindow()) )
    source_view_p=dynamic_cast<WSourceView*> (workspace_p->currentSubWindow()->widget());

  if (source_view_p==NULL)
  {
    on_viewSourceAction_triggered();
    source_view_p= dynamic_cast<WSourceView*> (workspace_p->currentSubWindow()->widget());
  }
  if (source_view_p)
    source_view_p->setSource(module,source,
        t==CSMesUndoRedoFramework::SOURCE_UNDEF?getViewType():t,
        start_line,start_column,end_line,end_column);
  if (modules_browser_p)
    modules_browser_p->selectSource(module,source);
}

void WMain::fullUpdate()
{
  fullUpdateEvent->send();
}

void WMain::partialUpdate()
{
  partialUpdateEvent->send();
}

void WMain::_partialUpdate(QObject *me)
{
  WMain *wmain=dynamic_cast<WMain*>(me);
  if (wmain)
    wmain->_partialUpdate();
}

void WMain::_fullUpdate(QObject *me)
{
  WMain *wmain=dynamic_cast<WMain*>(me);
  if (wmain)
    wmain->_fullUpdate();
}

void WMain::_partialUpdate()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (statistics_p) statistics_p->refreshView();
  if (executions_p) executions_p->refreshView();
  if (modules_browser_p) modules_browser_p->refreshView();
  if (functions_browser_p) functions_browser_p->refreshView();

  QList<QMdiSubWindow *>  windows = workspace_p->subWindowList();
  for ( int i = 0; i < int(windows.count()); ++i )
  {
    WSourceView *src_p=dynamic_cast<WSourceView*>(windows.at(i)->widget());
    if (src_p) src_p->refreshView();
  }
}

void WMain::_fullUpdate()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (statistics_p) statistics_p->setCSMes(csmes_file_p);
  if (executions_p) executions_p->setCSMes(csmes_file_p);
  if (modules_browser_p) modules_browser_p->setCSMes(csmes_file_p);
  if (functions_browser_p) functions_browser_p->setCSMes(csmes_file_p);

  QList<QMdiSubWindow *>  windows = workspace_p->subWindowList();
  for ( int i = 0; i < int(windows.count()); ++i )
  {
    WSourceView *src_p=dynamic_cast<WSourceView*>(windows.at(i)->widget());
    if (src_p) src_p->setCSMes(csmes_file_p);
  }
  _partialUpdate();
}

void WMain::on_nextInstrumentationAction_triggered()
{
  emit nextInstrumentation(Instrumentation::FILTER_NONE);
}


void WMain::on_previousInstrumentationAction_triggered()
{
  emit previousInstrumentation(Instrumentation::FILTER_NONE);
}


void WMain::on_nextInstrumentationTestedAction_triggered()
{
  emit nextInstrumentation(Instrumentation::FILTER_VALIDATED);
}


void WMain::on_previousInstrumentationTestedAction_triggered()
{
  emit previousInstrumentation(Instrumentation::FILTER_VALIDATED);
}


void WMain::on_nextInstrumentationUnTestedAction_triggered()
{
  emit nextInstrumentation(Instrumentation::FILTER_UNVALIDATED);
}


void WMain::on_previousInstrumentationUnTestedAction_triggered()
{
  emit previousInstrumentation(Instrumentation::FILTER_UNVALIDATED);
}


void WMain::on_nextInstrumentationCommentAction_triggered()
{
  emit nextInstrumentation(Instrumentation::FILTER_COMMENT);
}


void WMain::on_previousInstrumentationCommentAction_triggered()
{
  emit previousInstrumentation(Instrumentation::FILTER_COMMENT);
}



void WMain::on_nextInstrumentationManuallyValidatedAction_triggered()
{
  emit nextInstrumentation(Instrumentation::FILTER_MANUALLY_VALIDATED);
}


void WMain::on_previousInstrumentationManuallyValidatedAction_triggered()
{
  emit previousInstrumentation(Instrumentation::FILTER_MANUALLY_VALIDATED);
}


void WMain::on_nextModuleAction_triggered()
{
  const SourceFiles &sources=csmes_file_p->Sources(CSMes::NON_EMPTY);

  SourceFiles::const_iterator cur=qBinaryFind(sources.begin(), sources.end(),cur_source);

  if (cur!=sources.end())
    ++cur;
  if(cur==sources.end())
    cur_source=sources.first();
  else
    cur_source=*cur;
  showSource(QString(),cur_source,getViewType(),-1,-1,-1,-1);
}


void WMain::on_previousModuleAction_triggered()
{
  const SourceFiles &sources=csmes_file_p->Sources(CSMes::NON_EMPTY);

  SourceFiles::const_iterator cur=qBinaryFind(sources.begin(), sources.end(),cur_source);

  if (cur!=sources.end() && cur!=sources.begin())
    --cur;
  if(cur==sources.end())
    cur_source=sources.last();
  else
    cur_source=*cur;
  showSource(QString(),cur_source,getViewType(), -1,-1,-1,-1);
}



void WMain::requestNextModule(Instrumentation::filter_t t)
{
  const SourceFiles &sources=csmes_file_p->Sources(CSMes::NON_EMPTY);

  SourceFiles::const_iterator cur=qBinaryFind(sources.begin(), sources.end(),cur_source);

  bool found=false;
  while (!found)
  {
    if (cur!=sources.end())
      ++cur;
    if(cur==sources.end())
      return;
    QList<int> instrumented_lines;
    if (getViewType()==CSMesUndoRedoFramework::ORIGINAL)
      instrumented_lines=csmes_file_p->instrumentedLinesOrg(getCoverageLevel(),getCoverageMethod(), t ,QString(),*cur);
    else
      instrumented_lines=csmes_file_p->instrumentedLinesPre(getCoverageLevel(),getCoverageMethod(), t ,QString(),*cur);
    found = ! instrumented_lines.isEmpty();
  }
  cur_source=*cur;
  showSource(QString(),cur_source,getViewType(), -1,-1,-1,-1);
  emit firstInstrumentation(t);
}

void WMain::requestPreviousModule(Instrumentation::filter_t t)
{
  const SourceFiles &sources=csmes_file_p->Sources(CSMes::NON_EMPTY);

  SourceFiles::const_iterator cur=qBinaryFind(sources.begin(), sources.end(),cur_source);

  bool found=false;
  while (!found)
  {
    if (cur!=sources.end() && cur!=sources.begin())
      --cur;
    if(cur==sources.end())
      return;

    QList<int> instrumented_lines;
    if (getViewType()==CSMesUndoRedoFramework::ORIGINAL)
      instrumented_lines=csmes_file_p->instrumentedLinesOrg(getCoverageLevel(),getCoverageMethod(), t ,QString(),*cur);
    else
      instrumented_lines=csmes_file_p->instrumentedLinesPre(getCoverageLevel(),getCoverageMethod(), t ,QString(),*cur);
    found = ! instrumented_lines.isEmpty();

    if(cur==sources.begin())
      return;
  }

  cur_source=*cur;
  showSource(QString(),cur_source,getViewType(),-1,-1,-1,-1);
  emit lastInstrumentation(t);
}

void WMain::clearComment_triggered()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  emit clearComment();
}


void WMain::clearManualValidation_triggered()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  emit clearManuallyValidated();
}


void WMain::commentInstrumentation_triggered()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  emit setComment();
}


void WMain::manualValidation_triggered()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  emit setManuallyValidated();
}

void WMain::editPossibilities(bool set_comment,bool clear_comment, bool set_manually_validated, bool clear_manually_validated, bool preprocessor_view)
{
  clearManuallyValidatedAction->setEnabled(clear_manually_validated);
  setManuallyValidatedAction->setEnabled(set_manually_validated);
  setCommentAction->setEnabled(set_comment);
  clearCommentAction->setEnabled(clear_comment);
  preprocessorViewAction->setEnabled(preprocessor_view);
  findTextAction->setEnabled( set_comment || clear_comment || set_manually_validated || clear_manually_validated || preprocessor_view);
}

bool WMain::isAutoSavedOnExit() const
{
  return Options::get_opt_bool(QString(),"SAVE_ON_EXIT",false);
}

bool WMain::tryExit()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (csmes_file_p->modificationFlag())
  {
    if (isAutoSavedOnExit())
    {
      fileSave();
    }
    else
    {
      switch (QMessageBox::warning(this,tr("Application Exit"),
            tr("Measurement file not saved.\nDo you really want to quit?"),
            QMessageBox::Ok,QMessageBox::No|QMessageBox::Default| QMessageBox::Escape))
      {
        case QMessageBox::Ok:
          break;
        default:
          return false;
      }
    }
  }
  Options::save_window_position(getWorkingModeStr(),workspace_p);
  Options::save_window_position(getWorkingModeStr(),"WMAIN",this);
#ifdef QT_TOOLBAR_DIALOG
  Options::set_opt(getWorkingModeStr(),"TOOLBAR_SETTING",toolbar_manager_p->saveState());
#endif
  CSMesBackgroundComputations::GetObject().setCSMes(NULL);
  csmes_file_p->clear();
  return true;
}


void WMain::preprocessorViewAction_toggled( bool b)
{
  if (b)
    showSource(QString(),cur_source,CSMesUndoRedoFramework::CSMES, -1,-1,-1,-1);
  else
    showSource(QString(),cur_source,CSMesUndoRedoFramework::ORIGINAL, -1,-1,-1,-1);
}

void WMain::emitViewType()
{
  showSource(QString(),cur_source,getViewType(), -1,-1,-1,-1);
}

CSMesUndoRedoFramework::source_type_t WMain::getViewType() const
{
  CSMesUndoRedoFramework::source_type_t t = CoverageSettings::object().getSourceType();
  if (t==CSMesUndoRedoFramework::SOURCE_UNDEF)
  {
    if (preprocessorViewAction->isChecked())
      return CSMesUndoRedoFramework::CSMES;
    else
      return CSMesUndoRedoFramework::ORIGINAL;
  }
  return t;
}

void WMain::setViewType(CSMesUndoRedoFramework::source_type_t t)
{
   switch(t)
   {
   default:
   case CSMesUndoRedoFramework::ORIGINAL:
      CoverageSettings::object().setSourceType(CSMesUndoRedoFramework::ORIGINAL);
      preprocessorViewAction->setChecked(false);
      break;
   case CSMesUndoRedoFramework::CSMES:
      CoverageSettings::object().setSourceType(t);
      preprocessorViewAction->setChecked(true);
      break;
   }
}

SourceFiles WMain::sourceList() const
{
  return csmes_file_p->Sources(CSMes::NON_EMPTY);
}

QStringList WMain::executionsList() const
{
  return csmes_file_p->executionList();
}

bool WMain::statisticExecutions(const QStringList &l,int &nb_tested,int &nb_untested) const
{
  return csmes_file_p->statisticExecution(l,QStringList(),false,getCoverageLevel(),getCoverageMethod(),getReleaseComparaisonMode(),nb_tested,nb_untested,false);
}

bool WMain::selectExecutions( const QStringList& selection_list, const QStringList &cmp_items,bool test_coverage_mode,Instrumentation::coverage_method_t method, bool execution_comparaison_mode, CSMes::comparaison_mode_t csmes_comparaison_mode, int coverage_level)
{
  if (executions_p)
  {
    executions_p->selectComparaison(selection_list, cmp_items,test_coverage_mode,method, execution_comparaison_mode, csmes_comparaison_mode, coverage_level);
    return true;
  }
  return false;
}

QStringList WMain::selectedExecutions() const
{
  return CoverageSettings::object().getSelectedExecutions();
}

QStringList WMain::selectedExecutionsComparaison() const
{
  return CoverageSettings::object().getSelectedExecutionsComparaison();
}


bool WMain::renameExecution(const QString &old_name,const QString & new_name)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (csmes_file_p->renameExecution(old_name,new_name))
  {
    executions_p->setCSMes(csmes_file_p);
    return true;
  }
  return false;
}

bool WMain::deleteExecution(const QString &name)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (csmes_file_p->deleteExecution(name))
  {
    executions_p->setCSMes(csmes_file_p);
    return true;
  }
  return false;
}

bool WMain::saveCSMesFile(const QString &/*filename*/)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  return  csmes_file_p->saveCSMes( ) ;
}

void WMain::closeCSMesFile()
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  csmes_file_p->closeCSMes();
}

QString WMain::selectSource(const SourceFile &name)
{
  const SourceFiles &sources=csmes_file_p->Sources(CSMes::NON_EMPTY);
  SourceFiles::const_iterator cur=qBinaryFind(sources.begin(), sources.end(),name);
  if (cur!=sources.end())
  {
    cur_source=name;
    showSource(QString(),cur_source,getViewType(), -1,-1,-1,-1);
    return QString();
  }
  else
    return tr("Error: no source file:'%1'").arg( name );
}

bool WMain::isCSMesModified() const
{
  return (csmes_file_p->modificationFlag());
}

void WMain::quitApplication()
{
    qApp->quit();
}

bool WMain::getExecutionState(const QString &execution,Executions::execution_status_t &status) const
{
  status=csmes_file_p->getExecutionStatus(execution);
  return true;
}

bool WMain::setExecutionState(const QString &execution,Executions::execution_status_t status)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  csmes_file_p->setExecutionStatus(execution,status);
  executions_p->setCSMes(csmes_file_p);
  return true;
}

void WMain::on_findTextAction_triggered()
{
   emit showFindText();
}

void WMain::on_exportStatisticEMMAAction_triggered()
{
  const QStringList executions=selectedExecutions();
  if (executions.count()==0)
  {
    QMessageBox::warning(this,tr("Error"),tr("No executions selected"), QMessageBox::Ok,QMessageBox::NoButton);
    return ;
  }


  QString emma_filter=tr("EMMA-XML file, XML (*.xml)");
  QString selectedFilter=Options::get_opt_str(QString(),"EMMA_STATISTICS_FUNCTION_FILTER","");
  QString filename=QFileDialog::getSaveFileName ( this , tr("Statistics Export"), Options::get_opt_str(QString(),"EMMA_STATISTICS_FUNCTION_FILE",""),
     emma_filter+";;",
     &selectedFilter,
     FILE_DIALOG_OPTIONS);
  if (filename!=QString())
  {
    QString err;
    Options::set_opt(QString(),"EMMA_STATISTICS_FUNCTION_FILE",filename);
    Options::set_opt(QString(),"EMMA_STATISTICS_FUNCTION_FILTER",selectedFilter);

    if ( (err=exportEMMAStatistic(filename))!=QString())
    {
      QMessageBox::warning(this,tr("Error"),err, QMessageBox::Ok,QMessageBox::NoButton);
    }
  }
}

void WMain::on_exportStatisticFunctionAction_triggered()
{
  const QStringList executions=selectedExecutions();
  if (executions.count()==0)
  {
    QMessageBox::warning(this,tr("Error"),tr("No executions selected"), QMessageBox::Ok,QMessageBox::NoButton);
    return ;
  }


  QString csv_coma_filter=tr("CSV file, comma separator (*.csv)");
  QString csv_semi_coma_filter=tr("CSV file, semicolon separator, number with comma (*.csv)");
  QString csv_semi_dot_filter=tr("CSV file, semicolon separator, number with dot (*.csv)");
  QString selectedFilter=Options::get_opt_str(QString(),"CSV_STATISTICS_FUNCTION_FILTER","");
  QString filename=QFileDialog::getSaveFileName ( this , tr("Statistics Export"), Options::get_opt_str(QString(),"CSV_STATISTICS_FUNCTION_FILE",""),
     csv_coma_filter+";;"+csv_semi_coma_filter+";;"+csv_semi_dot_filter+";;",
     &selectedFilter,
     FILE_DIALOG_OPTIONS);
  if (filename!=QString())
  {
    QString err;
    QChar separator,numberSep;
    Options::set_opt(QString(),"CSV_STATISTICS_FUNCTION_FILE",filename);
    Options::set_opt(QString(),"CSV_STATISTICS_FUNCTION_FILTER",selectedFilter);

    if (selectedFilter==csv_semi_dot_filter)
    {
      separator=';';
      numberSep='.';
    }
    else if (selectedFilter==csv_semi_coma_filter)
    {
      separator=';';
      numberSep=',';
    }
    else
    {
      separator=',';
      numberSep='.';
    }
    if ( (err=exportCSVStatisticFunction(filename,separator,numberSep))!=QString())
    {
      QMessageBox::warning(this,tr("Error"),err, QMessageBox::Ok,QMessageBox::NoButton);
    }
  }
}

void WMain::on_exportStatisticModuleAction_triggered()
{
  const QStringList executions=selectedExecutions();
  if (executions.count()==0)
  {
    QMessageBox::warning(this,tr("Error"),tr("No executions selected"), QMessageBox::Ok,QMessageBox::NoButton);
    return ;
  }


  QString csv_coma_filter=tr("CSV file, comma separator (*.csv)");
  QString csv_semi_coma_filter=tr("CSV file, semicolon separator, number with comma (*.csv)");
  QString csv_semi_dot_filter=tr("CSV file, semicolon separator, number with dot (*.csv)");
  QString selectedFilter=Options::get_opt_str(QString(),"CSV_STATISTICS_MODULE_FILTER","");
  QString filename=QFileDialog::getSaveFileName ( this , tr("Statistics Export"), Options::get_opt_str(QString(),"CSV_STATISTICS_MODULE_FILE",""),
     csv_coma_filter+";;"+csv_semi_coma_filter+";;"+csv_semi_dot_filter +";;",
     &selectedFilter,
     FILE_DIALOG_OPTIONS);
  if (filename!=QString())
  {
    QString err;
    QChar separator,numberSep;
    Options::set_opt(QString(),"CSV_STATISTICS_MODULE_FILE",filename);
    Options::set_opt(QString(),"CSV_STATISTICS_MODULE_FILTER",selectedFilter);

    if (selectedFilter==csv_semi_dot_filter)
    {
      separator=';';
      numberSep='.';
    }
    else if (selectedFilter==csv_semi_coma_filter)
    {
      separator=';';
      numberSep=',';
    }
    else
    {
      separator=',';
      numberSep='.';
    }

    CSMesBackgroundComputations::Pauser statistic_pauser;
    if ( (err=exportCSVStatisticModule(filename,separator,numberSep))!=QString())
    {
      QMessageBox::warning(this,tr("Error"),err, QMessageBox::Ok,QMessageBox::NoButton);
    }
  }
}

QString WMain::exportEMMAStatistic(const QString &filename) const
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (csmes_file_p)
    return csmes_file_p->exportEMMAStatistic(filename,getCoverageLevel(),getCoverageMethod());
  else
    return tr("No CSMES file open");
}

QString WMain::exportCSVStatisticFunction(const QString &filename,QChar separator,QChar floatSep) const
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (csmes_file_p)
    return csmes_file_p->exportCSVStatisticFunction(filename,separator,floatSep,getCoverageLevel(),getCoverageMethod());
  else
    return tr("No CSMES file open");
}

QString WMain::exportCSVStatisticModule(const QString &filename,QChar separator,QChar floatSep) const
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (csmes_file_p)
    return csmes_file_p->exportCSVStatisticModule(filename,separator,floatSep,getCoverageLevel(),getCoverageMethod());
  else
    return tr("No CSMES file open");
}

QString WMain::relativeSourceName(const QString &abs_filename)
{
  if (csmes_file_p)
    return csmes_file_p->relativeSourceName(abs_filename);
  else
    return abs_filename;
}

void WMain::setCoverageLevel(int l)
{
  selectExecutions(selectedExecutions(),selectedExecutionsComparaison(),getTestCoverageMode(),getCoverageMethod(),getExecutionAnalysisMode(),getReleaseComparaisonMode(),l);
  updateCoverageLevel();
}

int WMain::getCoverageLevel() const
{
  return CoverageSettings::object().getCoverageLevel();
}

void WMain::setReleaseComparaisonMode(CSMes::comparaison_mode_t m)
{
  if (getWorkingMode()==CoverageSettings::WORKING_MODE_BLACK_BOX)
    m=CSMes::COMPARAISON_MODE_NONE;
  if (!actionStopComparingCSMes->isChecked())
    m=CSMes::COMPARAISON_MODE_NONE;

  selectExecutions(selectedExecutions(),selectedExecutionsComparaison(),getTestCoverageMode(),getCoverageMethod(),getExecutionAnalysisMode(),m,getCoverageLevel());
  switch (m)
  {
    case CSMes::COMPARAISON_MODE_NOT_MODIFIED_FUNCTIONS:
      if (actionNewCodeAnalysis->isChecked())
        actionNewCodeAnalysis->setChecked(false);
      if (!actionRegressionAnalysis->isChecked())
        actionRegressionAnalysis->setChecked(true);
      break;
    case CSMes::COMPARAISON_MODE_MODIFIED_FUNCTIONS:
      if (!actionNewCodeAnalysis->isChecked())
        actionNewCodeAnalysis->setChecked(true);
      if (actionRegressionAnalysis->isChecked())
        actionRegressionAnalysis->setChecked(false);
      break;
    default:
      if (actionNewCodeAnalysis->isChecked())
        actionRegressionAnalysis->setChecked(false);
      if (actionRegressionAnalysis->isChecked())
        actionNewCodeAnalysis->setChecked(false);
      break;
  }
}

CSMes::comparaison_mode_t WMain::getReleaseComparaisonMode() const
{
  CSMes::comparaison_mode_t mode;
  ASSERT( ! (actionRegressionAnalysis->isChecked() && actionNewCodeAnalysis->isChecked() ) );
  if (actionRegressionAnalysis->isChecked())
    mode = CSMes::COMPARAISON_MODE_NOT_MODIFIED_FUNCTIONS;
  else if (actionNewCodeAnalysis->isChecked())
    mode = CSMes::COMPARAISON_MODE_MODIFIED_FUNCTIONS;
  else
    mode = CSMes::COMPARAISON_MODE_NONE;
  return mode;
}

Instrumentation::coverage_method_t WMain::getCoverageMethod() const
{
  Instrumentation::coverage_method_t  method;
  if (!actionBranch_only->isChecked()
      &&
      !actionCondition_and_Branch->isChecked())
  {
    ASSERT(false);
    method = Instrumentation::METHOD_UNKNOWN;
  }
  else if (actionBranch_only->isChecked())
    method = Instrumentation::COVERAGE_BRANCH;
  else
    method = Instrumentation::COVERAGE_CONDITION;

  ASSERT(CoverageSettings::object().getCoverageMethod()==method);
  return method;
}

void WMain::setCoverageMethod(Instrumentation::coverage_method_t method)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  switch (method)
  {
    case Instrumentation::COVERAGE_BRANCH:
      if (!actionBranch_only->isChecked())
        actionBranch_only->setChecked(true);
      break;
    default:
      ASSERT(false);
    case Instrumentation::COVERAGE_CONDITION:
      if (!actionCondition_and_Branch->isChecked())
        actionCondition_and_Branch->setChecked(true);
      break;
  }
  showSource(QString(),cur_source,getViewType(), -1,-1,-1,-1);
  selectExecutions(selectedExecutions(),selectedExecutionsComparaison(),getTestCoverageMode(),method,getExecutionAnalysisMode(),getReleaseComparaisonMode(),getCoverageLevel());
}

void WMain::coverageLevelChanged()
{
  bool ok;
  int v=QInputDialog::getInteger(this,tr("Coverage Level"),tr("Coverage Level:"),getCoverageLevel(),1,0x7FFFFFFF,1,&ok);
  if (ok)
  {
    CSMesBackgroundComputations::Pauser statistic_pauser;
    setCoverageLevel(v);
    showSource(QString(),cur_source,getViewType(), -1,-1,-1,-1);
  }
}

void WMain::updateCoverageLevel()
{
  coverageLevelAction->setIconText(tr("Level:")+QString::number(getCoverageLevel()));
  coverageLevelAction->setText(tr("Level:")+QString::number(getCoverageLevel()));
}

void WMain::SetWindowTitle()
{
  QString title="CoverageBrowser";
  setWindowTitle(title);
}

void WMain::CSExeFileModified()
{
  switch (csexe_watch)
  {
    case WLoadExecution::WATCH_DO_NOTHING:
      printStatus(QTime::currentTime().toString()+"> "+tr("Execution report is modified.")+" ["+csexe_file+"]");
      break;
    case WLoadExecution::WATCH_OPEN_DIALOG:
      LoadExecutionResult_triggered();
      break;
    case WLoadExecution::WATCH_IMPORT_AUTOMATICALLY:
      importAutomatically.start(500);
      break;
    default:
      ASSERT(0);
      break;
  }
}

void WMain::importAutomaticallyEvent()
{
  loadCSExeFile(csexe_file,csexe_name,csexe_importPolicy,csexe_deleteAfterLoading,csexe_default_status,csexe_minimize_memory_usage);
}


void WMain::contextMenu( const QPoint &pos)
{
  if (getWorkingMode()==CoverageSettings::WORKING_MODE_BLACK_BOX)
    return ;

  QMainWindow *w=dynamic_cast<QMainWindow*>(sender());
  QMenu * menu;
  if (w)
  {
    menu=w->createPopupMenu();
#ifdef QT_TOOLBAR_DIALOG
    QAction *a=new QAction(tr("Configure toolbar"),this);
    connect(a,SIGNAL(triggered()),this,SLOT(configureToolBar()));
    menu->addAction(a);
#endif

    menu->popup(mapToGlobal(pos));
  }
}

void WMain::configureToolBar()
{
  WOption opt(this);
  opt.on_toolbar_p_clicked();
}


void WMain::on_actionTestCocoon_Website_triggered(bool)
{
  HelpClient::OpenTestCocoonWebsite();
}

void WMain::on_executionBenefitAnalysisAction_triggered(bool b)
{
  if (b != getExecutionAnalysisMode() )
    selectExecutions(QStringList(),QStringList(),getTestCoverageMode(),getCoverageMethod(),b,getReleaseComparaisonMode(),getCoverageLevel());
}

bool WMain::getExecutionAnalysisMode() const
{
  return CoverageSettings::object().getExecutionAnalysisMode();
}

void WMain::setExecutionAnalysisMode(bool b)
{
  executionBenefitAnalysisAction->setChecked(b);
}

void WMain::on_actionTest_Coverage_Count_triggered(bool b)
{
  selectExecutions(selectedExecutions(),selectedExecutionsComparaison(),b,getCoverageMethod(),getExecutionAnalysisMode(),getReleaseComparaisonMode(),getCoverageLevel());
}


void WMain::addLastRecentOpenedCSMes(const QString &file)
{
  QStringList last_recent_opened_csmes=Options::get_opt_strlst(QString(),"LAST_RECENT_OPENED_CSMES",QStringList());
  if (last_recent_opened_csmes.contains(file))
    last_recent_opened_csmes.removeAll(file);

  while (last_recent_opened_csmes.count()>LRU_MAX_CSMES_FILES)
    last_recent_opened_csmes.removeLast();

  last_recent_opened_csmes.prepend(file);
  Options::set_opt(QString(),"LAST_RECENT_OPENED_CSMES",last_recent_opened_csmes);
}

void WMain::lastRecentOpenedCSMes(QStringList &active,QStringList &inactive) const
{
  int count=0;
  QStringList last_recent_opened_csmes=Options::get_opt_strlst(QString(),"LAST_RECENT_OPENED_CSMES",QStringList());
  for(QStringList::const_iterator it=last_recent_opened_csmes.begin();it!=last_recent_opened_csmes.end();++it)
  {
    QFileInfo info(*it);
    if (info.exists())
    {
      active+= *it;
      count++;
    }
    if (count>=LRU_MAX_CSMES_FILES_DISPLAYED)
      break;
  }
  for(QStringList::const_iterator it=last_recent_opened_csmes.begin();it!=last_recent_opened_csmes.end();++it)
  {
    if (!active.contains(*it))
    {
      inactive+= *it;
      count++;
    }
    if (count>=LRU_MAX_CSMES_FILES_DISPLAYED)
      break;
  }
}

void WMain::updateCompareWithLastRecentOpenedCSMes()
{
  QStringList last_recent_opened_csmes_active;
  QStringList last_recent_opened_csmes_inactive;
  lastRecentOpenedCSMes(last_recent_opened_csmes_active,last_recent_opened_csmes_inactive);

  menuCompareWithRecentlyOpen->clear();
  QStringList::const_iterator it;
  for(it=last_recent_opened_csmes_active.begin();it!=last_recent_opened_csmes_active.end();++it)
  {
    QAction *act=menuCompareWithRecentlyOpen->addAction(*it);
    if (csmes_file_p)
    {
      if ( (*it) == csmes_file_p->getFilename() )
        act->setEnabled(false);
      if ( (*it) == csmes_file_p->getFilenameReference() )
      {
        QFont f=act->font();
        f.setBold(true);
        act->setFont(f);
      }
    }
    connect(act,SIGNAL(triggered()),this,SLOT(compareWithRecentlyOpen_triggered()));
  }
  for(it=last_recent_opened_csmes_inactive.begin();it!=last_recent_opened_csmes_inactive.end();++it)
  {
    QAction *act=menuCompareWithRecentlyOpen->addAction(*it);
    act->setEnabled(false);
    connect(act,SIGNAL(triggered()),this,SLOT(compareWithRecentlyOpen_triggered()));
  }
}

void WMain::updateLastRecentOpenedCSMes()
{
  QStringList last_recent_opened_csmes_active;
  QStringList last_recent_opened_csmes_inactive;
  lastRecentOpenedCSMes(last_recent_opened_csmes_active,last_recent_opened_csmes_inactive);

  menuRecently_Opened->clear();
  QStringList::const_iterator it;
  for(it=last_recent_opened_csmes_active.begin();it!=last_recent_opened_csmes_active.end();++it)
  {
    QAction *act=menuRecently_Opened->addAction(*it);
    if (csmes_file_p)
    {
      if ( (*it) == csmes_file_p->getFilenameReference() )
        act->setEnabled(false);
      if ( (*it) == csmes_file_p->getFilename() )
      {
        QFont f=act->font();
        f.setBold(true);
        act->setFont(f);
      }
    }
    connect(act,SIGNAL(triggered()),this,SLOT(lastRecentOpenedCSMes_triggered()));
  }
  for(it=last_recent_opened_csmes_inactive.begin();it!=last_recent_opened_csmes_inactive.end();++it)
  {
    QAction *act=menuRecently_Opened->addAction(*it);
    act->setEnabled(false);
    connect(act,SIGNAL(triggered()),this,SLOT(lastRecentOpenedCSMes_triggered()));
  }
}


void WMain::compareWithRecentlyOpen_triggered()
{
  QAction *act=dynamic_cast<QAction*>(sender());
  if (act)
  {
    compareCSMes(act->text());
    fullUpdate();
  }
}

void WMain::lastRecentOpenedCSMes_triggered()
{
  if (!csmesFileSaved())
    return;
  QAction *act=dynamic_cast<QAction*>(sender());
  if (act)
    openCSMesFile(act->text());
}

void WMain::changeStyle(const QString &styleName)
{
  QStyle *style_p=QStyleFactory::create(styleName);
  if (style_p)
  {
    QApplication::setStyle(style_p);
    if (styleName!="WindowsXP")
       QApplication::setPalette(QApplication::style()->standardPalette());
  }
}

void WMain::updateWindowMenu()
{
  windowMenu->clear();
  windowMenu->addAction(actionCloseWindow);
  windowMenu->addAction(actionCloseAllWindow);
  windowMenu->addSeparator();
  windowMenu->addAction(actionTileWindow);
  windowMenu->addAction(actionCascadeWindow);
  windowMenu->addSeparator();
  windowMenu->addAction(actionNextWindow);
  windowMenu->addAction(actionPreviousWindow);
  windowMenu->addAction(actionSeparatorWindow);

  QList<QMdiSubWindow *> windows = workspace_p->subWindowList();
  actionSeparatorWindow->setVisible(!windows.isEmpty());

  for (int i = 0; i < windows.size(); ++i)
  {
     QMdiSubWindow *sub_window_p=windows.at(i);
     if (sub_window_p)
     {
        const WSourceView *child = dynamic_cast<WSourceView *>(sub_window_p->widget());

        if (child)
        {
           QString text;
           QString name=child->windowTitle();
           QFileInfo fileName(name);
           name=fileName.fileName();

           if (i < 9)
              text = "&";
           text+=QString::number(i+1)+" ";
           if (sub_window_p->isMinimized())
              text += "["+name+"]";
           else
              text+=name;

           QAction *action  = windowMenu->addAction(text);
           action->setCheckable(true);
           action ->setChecked(child == activeWSourceView());
           connect(action, SIGNAL(triggered()), window_mapper_p, SLOT(map()));
           window_mapper_p->setMapping(action, sub_window_p);
        }
     }
  }
}

WSourceView *WMain::activeWSourceView() const
{
  if (QMdiSubWindow *currentSubWindow = workspace_p->currentSubWindow())
    return dynamic_cast<WSourceView *>(currentSubWindow->widget());
  return NULL;
}

void WMain::createWindowMenu()
{
  connect(actionCloseWindow, SIGNAL(triggered()),
      workspace_p, SLOT(closeActiveSubWindow()));

  connect(actionCloseAllWindow, SIGNAL(triggered()),
      workspace_p, SLOT(closeAllSubWindows()));

  connect(actionTileWindow, SIGNAL(triggered()), workspace_p, SLOT(tileSubWindows()));

  connect(actionCascadeWindow, SIGNAL(triggered()), workspace_p, SLOT(cascadeSubWindows()));

  connect(actionNextWindow, SIGNAL(triggered()),
      workspace_p, SLOT(activateNextSubWindow()));

  connect(actionPreviousWindow, SIGNAL(triggered()),
      workspace_p, SLOT(activatePreviousSubWindow()));

  updateWindowMenu();
  connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
}

void WMain::setActiveSubWindow(QWidget *window)
{
  if (!window)
    return;

  QMdiSubWindow *mdi_window_p=dynamic_cast<QMdiSubWindow *>(window);
  if (!mdi_window_p)
     return;

  workspace_p->setActiveSubWindow(mdi_window_p);
}

void WMain::on_actionBranch_only_triggered(bool b)
{
  if (b)
    setCoverageMethod(Instrumentation::COVERAGE_BRANCH);
}

void WMain::on_actionCondition_and_Branch_triggered(bool b)
{
  if (b)
    setCoverageMethod(Instrumentation::COVERAGE_CONDITION);
}

void WMain::on_generateHtmlReportAction_triggered()
{
  const QStringList executions=selectedExecutions();
  if (executions.count()==0)
  {
    QMessageBox::warning(this,tr("Error"),tr("No executions selected"), QMessageBox::Ok,QMessageBox::NoButton);
    return ;
  }

  WHTMLReport *html_report_p= new WHTMLReport(csmes_file_p,this);
  if (html_report_p->exec())
  {
  }
  delete html_report_p;
}

void WMain::selectBranchOnly(bool b)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  if (b)
    setCoverageMethod(Instrumentation::COVERAGE_BRANCH);
  else
    setCoverageMethod(Instrumentation::COVERAGE_CONDITION);
}

void WMain::selectTestCountMode(bool b)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  actionTest_Coverage_Count->setChecked(b);
}

bool WMain::getTestCoverageMode() const
{
  //ASSERT( CoverageSettings::object().getTestCoverageMode()==actionTest_Coverage_Count->isChecked());
  return CoverageSettings::object().getTestCoverageMode();
}

void WMain::selectCoverageLevel(int v)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  setCoverageLevel(v);
}

void WMain::setWhiteBoxMode()
{
  clearCommentAction->setVisible(true);
  clearManuallyValidatedAction->setVisible(true);
  exportStatisticFunctionAction->setVisible(true);
  exportStatisticEMMAAction->setVisible(true);
  exportStatisticModuleAction->setVisible(true);
  fileMergeCSMesAction->setVisible(true);
  actionImportUnitTests->setVisible(true);
  generateHtmlReportAction->setVisible(true);
  actionExecutionTextDump->setVisible(true);
  nextInstrumentationAction->setVisible(true);
  nextInstrumentationTestedAction->setVisible(true);
  nextInstrumentationUnTestedAction->setVisible(true);
  nextInstrumentationManuallyValidatedAction->setVisible(true);
  nextInstrumentationCommentAction->setVisible(true);
  nextModuleAction->setVisible(true);
  preprocessorViewAction->setVisible(true);
  findTextAction->setVisible(true);
  previousInstrumentationAction->setVisible(true);
  previousInstrumentationTestedAction->setVisible(true);
  previousInstrumentationUnTestedAction->setVisible(true);
  previousInstrumentationCommentAction->setVisible(true);
  previousInstrumentationManuallyValidatedAction->setVisible(true);
  previousModuleAction->setVisible(true);
  redoAction->setVisible(true);
  saveBlackBoxConfigurationAction->setVisible(true);
  setCommentAction->setVisible(true);
  setManuallyValidatedAction->setVisible(true);
  undoAction->setVisible(true);

  enableToolbar(toolbar_coverage_settings_p,true);
  enableToolbar(toolbar_comparing_p,true);
  enableToolbar(toolbar_statistics_p,true);
  enableToolbar(toolbar_blackbox_p,true);
  enableToolbar(toolbar_source_p,true);
  enableToolbar(toolbar_instrumentation_p,true);
  enableToolbar(toolbar_navigation_all_p,true);
  enableToolbar(toolbar_navigation_tested_p,true);
  enableToolbar(toolbar_navigation_untested_p,true);
  enableToolbar(toolbar_navigation_comment_p,true);
  enableToolbar(toolbar_navigation_manually_validated_p,true);
  enableToolbar(toolbar_navigation_source_p,true);

  enableDock(dwExplanation_p,true);
  enableDock(dwFunctions_p,true);
  enableDock(dwModules_p,true);

  viewSourceAction->setVisible(true);
  windowMenu->menuAction()->setVisible(true);
  navigationMenu->menuAction()->setVisible(true);
  executionBenefitAnalysisAction->setVisible(true);
  actionStopComparingCSMes->setVisible(true);
  actionSwitchCSMesFiles->setVisible(true);
  actionNewCodeAnalysis->setVisible(true);
  actionRegressionAnalysis->setVisible(true);
  actionCompareCSMes->setVisible(true);
  menuCompareWithRecentlyOpen->menuAction()->setVisible(true);

  workspace_p->show();
}

void WMain::setBlackBoxMode()
{
  clearCommentAction->setVisible(false);
  clearManuallyValidatedAction->setVisible(false);
  exportStatisticFunctionAction->setVisible(false);
  exportStatisticEMMAAction->setVisible(false);
  exportStatisticModuleAction->setVisible(false);
  fileMergeCSMesAction->setVisible(false);
  actionImportUnitTests->setVisible(false);
  generateHtmlReportAction->setVisible(false);
  actionExecutionTextDump->setVisible(false);
  nextModuleAction->setVisible(false);
  preprocessorViewAction->setVisible(false);
  findTextAction->setVisible(false);
  nextInstrumentationAction->setVisible(false);
  nextInstrumentationTestedAction->setVisible(false);
  nextInstrumentationUnTestedAction->setVisible(false);
  nextInstrumentationManuallyValidatedAction->setVisible(false);
  nextInstrumentationCommentAction->setVisible(false);
  previousInstrumentationAction->setVisible(false);
  previousInstrumentationTestedAction->setVisible(false);
  previousInstrumentationUnTestedAction->setVisible(false);
  previousInstrumentationManuallyValidatedAction->setVisible(false);
  previousInstrumentationCommentAction->setVisible(false);
  previousModuleAction->setVisible(false);
  redoAction->setVisible(false);
  saveBlackBoxConfigurationAction->setVisible(false);
  setCommentAction->setVisible(false);
  setManuallyValidatedAction->setVisible(false);

  enableToolbar(toolbar_coverage_settings_p,true);
  enableToolbar(toolbar_comparing_p,false);
  enableToolbar(toolbar_statistics_p,false);
  enableToolbar(toolbar_blackbox_p,false);
  enableToolbar(toolbar_source_p,false);
  enableToolbar(toolbar_instrumentation_p,false);
  enableToolbar(toolbar_navigation_all_p,false);
  enableToolbar(toolbar_navigation_tested_p,false);
  enableToolbar(toolbar_navigation_untested_p,false);
  enableToolbar(toolbar_navigation_comment_p,false);
  enableToolbar(toolbar_navigation_manually_validated_p,false);
  enableToolbar(toolbar_navigation_source_p,false);

  enableDock(dwExplanation_p,false);
  enableDock(dwFunctions_p,false);
  enableDock(dwModules_p,false);

  undoAction->setVisible(false);

  viewSourceAction->setVisible(false);
  windowMenu->menuAction()->setVisible(false);
  navigationMenu->menuAction()->setVisible(false);
  executionBenefitAnalysisAction->setVisible(false);
  actionStopComparingCSMes->setVisible(false);
  actionSwitchCSMesFiles->setVisible(false);
  actionNewCodeAnalysis->setVisible(false);
  actionRegressionAnalysis->setVisible(false);
  actionCompareCSMes->setVisible(false);
  menuCompareWithRecentlyOpen->menuAction()->setVisible(false);
  setExecutionAnalysisMode(false);
  compareCSMes(QString());

  workspace_p->hide();
}

void WMain::enableDock(QDockWidget *dock_p,bool enable)
{
  dock_p->setVisible(enable);
  dock_p->toggleViewAction()->setVisible(enable);
}

void WMain::enableToolbar(QToolBar *toolbar_p,bool enable)
{
  toolbar_p->setVisible(enable);
  toolbar_p->toggleViewAction()->setVisible(enable);
}

void WMain::_saveExecutionComment()
{
  if (csmes_file_p)
  {
    if (csmes_file_p->executionList().contains(execution_comment_p->name()))
      csmes_file_p->setExecutionComment(execution_comment_p->name(),execution_comment_p->text());
  }
}

void WMain::clearExecutionComment()
{
  dwExecutionComment_p->setWindowTitle(execution_comment_title);
  execution_comment_p->setName(QString());
  execution_comment_p->setText(QString());
}

void WMain::_setExecutionComment(const QString &name,const QString &comment)
{
  CSMesBackgroundComputations::Pauser statistic_pauser;
  QString oldcomment;
  if (execution_comment_p->isModified())
  {
    switch (QMessageBox::warning(this,tr("Warning"),tr("Comment is modified. Do you want to save it?"),
          QMessageBox::Yes,QMessageBox::No|QMessageBox::Default| QMessageBox::Escape))
    {
      case QMessageBox::Yes:
        _saveExecutionComment();
        break;
      default:
        break;
    }
  }
  dwExecutionComment_p->setWindowTitle(execution_comment_title+" ("+name+")");
  execution_comment_p->setName(name);
  execution_comment_p->setText(comment);
}

QString WMain::getWorkingModeStr() const
{
  switch (getWorkingMode())
  {
    case CoverageSettings::WORKING_MODE_WHITE_BOX: return "WHITEBOX";
    case CoverageSettings::WORKING_MODE_BLACK_BOX: return "BLACKBOX";
    default:                     return QString();
  }
}

void WMain::setWorkingMode(CoverageSettings::working_mode_t w, bool force)
{
  if (w!=getWorkingMode() || force)
  {
     CSMesBackgroundComputations::Pauser statistic_pauser;
    if (!force)
    {
      Options::save_window_position(getWorkingModeStr(),workspace_p);
      Options::save_window_position(getWorkingModeStr(),"WMAIN",this);
#ifdef QT_TOOLBAR_DIALOG
      Options::set_opt(getWorkingModeStr(),"TOOLBAR_SETTING",toolbar_manager_p->saveState());
#endif
    }
    CoverageSettings::object().setWorkingMode(w);
    switch (w)
    {
      case CoverageSettings::WORKING_MODE_WHITE_BOX:
        actionBlackBoxTestingMode->setChecked(false);
        setWhiteBoxMode();
        break;
      case CoverageSettings::WORKING_MODE_BLACK_BOX:
        actionBlackBoxTestingMode->setChecked(true);
        setBlackBoxMode();
        break;
    }
    workspace_p->closeAllSubWindows();
    Options::restore_window_position(getWorkingModeStr(),"WMAIN",this);
    Options::restore_window_position(getWorkingModeStr(),this);
#ifdef QT_TOOLBAR_DIALOG
    toolbar_manager_p->restoreState(Options::get_opt_array(getWorkingModeStr(),"TOOLBAR_SETTING"));
#endif
  }
}

CoverageSettings::working_mode_t WMain::getWorkingMode() const
{
  return CoverageSettings::object().getWorkingMode();
}

void WMain::on_saveBlackBoxConfigurationAction_triggered()
{
  if (csmes_file_p)
  {
    QString selectedFilter;
    QString filename=csmes_file_p->getFilename();
    filename="blackbox_"+filename;
    filename=QFileDialog::getSaveFileName ( this ,
        tr("Save Black-Box Configuration"),
        filename,
        "*.csmes",
        &selectedFilter,
        FILE_DIALOG_OPTIONS);
    if (filename!=QString())
    {
      if (!csmes_file_p->saveBlackBoxConfiguration(filename))
      {
        QMessageBox::warning(this,tr("Error"),tr("No black-box configuration generated"), QMessageBox::Ok,QMessageBox::NoButton);
      }
    }
  }
}

void WMain::on_actionOptimizedExecutionOrder_triggered()
{
  if (!csmes_file_p)
    return;
  if (selectedExecutions().isEmpty())
  {
    QMessageBox::warning(this,tr("Error"),tr("No executions selected"), QMessageBox::Ok,QMessageBox::NoButton);
    return;
  }

  WOptimizedExecutionOrder optimized_test_order(selectedExecutions(),getCoverageMethod(),this);
  optimized_test_order.calculateStatistics();
  if (optimized_test_order.exec())
  {
  }
}

void WMain::on_actionBlackBoxTestingMode_toggled(bool b)
{
   CoverageSettings::working_mode_t w;
  if (b)
    w=(CoverageSettings::WORKING_MODE_BLACK_BOX);
  else
    w=(CoverageSettings::WORKING_MODE_WHITE_BOX);

  setWorkingMode(w);
}

void WMain::requestFindTextModule( const QString & exp, QTextDocument::FindFlags options  )
{
   const SourceFiles &src=csmes_file_p->Sources(CSMes::NON_EMPTY);
   bool forward= (QTextDocument::FindBackward&options) == 0 ;
   SourceFiles sources;
   if (forward)
      sources=src;
   else
   {
      for (SourceFiles::const_iterator it=src.begin();it!=src.end();++it)
         sources.prepend(*it);
   }
   SourceFiles::const_iterator cur_start=qFind(sources.begin(), sources.end(),cur_source);
   if (cur_start==sources.end())
      return ;

   SourceFiles _sources;
   SourceFiles::const_iterator cur;
   for (cur=cur_start;cur!=sources.end();++cur)
   {
      if (cur!=cur_start)
         _sources+=*cur;
   }

   for (cur=sources.begin();cur!=cur_start;++cur)
      _sources+=*cur;
   _sources+= *cur_start;

   QProgressDialog progress(tr("Searching text '%1' in files...").arg(exp), tr("Abort searching"), 0, sources.count(), this);
   progress.setWindowModality(Qt::WindowModal);
   bool found=false;
   int file_count=0;
   QTextDocument text_doc;
   for (cur=_sources.begin();cur!=_sources.end();++cur)
   {
      if (progress.wasCanceled())
         return;
      progress.setValue( file_count++ );

      csmes_file_p->toRTFSyntax(QString(),*cur,getCoverageLevel(),getCoverageMethod(),getViewType(),false,-1,text_doc) ;
      printStatus(tr("Searching in '%1'").arg(*cur));
      QTextCursor cursor(&text_doc);
      cursor.setPosition(QTextCursor::Start);
      found = (text_doc.find(exp,cursor,options& (~QTextDocument::FindBackward) )!=QTextCursor());
      if (found)
         break;
   }
   if(!found)
   {
      progress.close();
      QMessageBox::warning(this,
         tr("Next not found"),
         tr("Next not found '%1' in any files").arg(exp),
         QMessageBox::Close);
      return;
   }
   cur_source=*cur;
   showSource(QString(),cur_source,getViewType(), -1,-1,-1,-1);
   WSourceView *source_view_p= NULL;
   if (workspace_p->activeSubWindow())
      source_view_p=dynamic_cast<WSourceView*> (workspace_p->activeSubWindow()->widget());
   if ( (source_view_p==NULL) && (workspace_p->currentSubWindow()) )
      source_view_p=dynamic_cast<WSourceView*> (workspace_p->currentSubWindow()->widget());
   if (source_view_p)
   {
      if (forward)
         source_view_p->setCursorPosition(0,0);
      else
      {
         source_view_p->setCursorPosition(0x7FFFFFFF,0x7FFFFFFF);
      }
      source_view_p->find(exp,options);
   }
}

void WMain::on_actionExecutionTextDump_triggered()
{
  WExecutionTextDump executionTextDump(csmes_file_p,this);
  if (executionTextDump.exec())
  {
    QString err;
    if ( (err=csmes_file_p->exportTextReport
          (
           executionTextDump.fileName(),
           executionTextDump.formatExecuted(),
           executionTextDump.formatUnexecuted(),
           getCoverageLevel(),getCoverageMethod()))!=QString())
    {
      QMessageBox::warning(this,tr("Error"),err, QMessageBox::Ok,QMessageBox::NoButton);
    }
  }
}
