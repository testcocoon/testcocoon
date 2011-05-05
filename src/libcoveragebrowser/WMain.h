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

#ifndef WMAIN_H
#define WMAIN_H
#include "coveragebrowserpdef.h"
#include "ui_WMainUI.h"
#include "WLoadExecution.h"
#include "CoverageSettings.h"
#include <QMainWindow>
#include <QTimer>
#include "csmes.h"
#include "instrumentation.h"
#include "signalmultiplexer.h"
#include "ui_WMainUI.h"

class IdleEvent;
class QtToolBarManager;
#include <QUndoView>
#include <QMdiArea>
#include <QDockWidget>
class HelpClient;
class WModule;
class WExplanation;
#include <QTreeView>
class WModuleList;
class WFunctionList;
#include <QComboBox>
class WLoadExecutionInfo;
class WExecutionList;
class FileSystemWatcher; 
class WSourceView;
class WExpressionHelp ;
class WStatistic;
#include <QMdiSubWindow>
#include <QSignalMapper>


class WMain : public QMainWindow, private Ui::WMainUI
{
    Q_OBJECT

public:
    WMain(QWidget* parent = 0, Qt::WindowFlags fl = Qt::Window);
    virtual ~WMain();

    virtual QMdiSubWindow * newSourceWindow( const QString & module, const QString & source );
    virtual CSMesUndoRedoFramework::source_type_t getViewType() const;
    virtual int getCoverageLevel() const;

public:
    Instrumentation::coverage_method_t getCoverageMethod() const;
    virtual QString exportCSVStatisticModule(const QString &filename,QChar separator,QChar floatSep) const;
    virtual QString exportCSVStatisticFunction(const QString &filename,QChar separator,QChar floatSep) const;
    QString exportEMMAStatistic(const QString &filename) const;
    bool getSourceCode(const QString &filename,bool preprocessed,QString &code) const ;
    bool getExecutionState(const QString &execution,Executions::execution_status_t &status) const;
    bool setExecutionState(const QString &execution,Executions::execution_status_t status) ;
    QStringList sourceList() const;
    QString openCSMesFile( const QString &filename );
    QString openCSMesRefenceFile( const QString &filename );
    QString closeCSMesRefenceFile( );
    QStringList executionsList() const;
    bool statisticExecutions(const QStringList &l,int &nb_tested,int &nb_untested) const;
    bool selectExecutions( const QStringList& selection_list, const QStringList &cmp_items,bool test_coverage_mode,Instrumentation::coverage_method_t method, bool execution_comparaison_mode, CSMes::comparaison_mode_t csmes_comparaison_mode,int coverage_level);
    QStringList selectedExecutions() const ;
    bool renameExecution(const QString &old_name,const QString & new_name);
    bool deleteExecution(const QString &name);
    bool saveCSMesFile(const QString &filename=QString());
    void closeCSMesFile();
    QString selectSource(const QString &name);
    bool isCSMesModified() const ;
    void loadCSExeFile(const QString &filename,const QString &prefix,CSMesUndoRedoFramework::csexe_import_policy_t,bool delete_after,Executions::execution_status_t default_execution_status,bool minimize_memory_usage);
    void loadCSExeCommand(const QString &command,const QStringList &arguments,const QString &prefix,CSMesUndoRedoFramework::csexe_import_policy_t policy,Executions::execution_status_t default_execution_status,bool minimize_memory_usage);
    QString relativeSourceName(const QString &abs_filename);
    QtToolBarManager *toolBarManager() { return toolbar_manager_p; }
    void selectBranchOnly(bool);
    void selectTestCountMode(bool);
    void selectCoverageLevel(int);
    QStringList selectedExecutionsComparaison() const ;
    void setReleaseComparaisonMode(CSMes::comparaison_mode_t m) ;

public slots:
  void on_actionSwitchCSMesFiles_triggered();
    void on_actionBranch_only_triggered(bool);
    void on_actionCondition_and_Branch_triggered(bool);
    void on_generateHtmlReportAction_triggered();
    void fileSave();
    void fileExit();
    void helpContents();
    void helpAbout();
    void aboutQt();
    void filter_help();
    void on_viewSourceAction_triggered();
    void currentWindowChanged( QMdiSubWindow * w );
    void LoadExecutionResult_triggered();
    void on_fileOpenCSMesAction_triggered();
    void on_fileMergeCSMesAction_triggered();
    void on_actionImportUnitTests_triggered();
    void on_Options_triggered();
    void showSource(const QString &module,const QString &source,CSMesUndoRedoFramework::source_type_t t,int start_line,int start_column,int end_line,int end_column);
    void on_nextInstrumentationAction_triggered();
    void on_previousInstrumentationAction_triggered();
    void on_nextInstrumentationTestedAction_triggered();
    void on_previousInstrumentationTestedAction_triggered();
    void on_nextInstrumentationUnTestedAction_triggered();
    void on_previousInstrumentationUnTestedAction_triggered();
    void on_nextInstrumentationManuallyValidatedAction_triggered();
    void on_previousInstrumentationManuallyValidatedAction_triggered();
    void on_nextInstrumentationCommentAction_triggered();
    void on_previousInstrumentationCommentAction_triggered();
    void on_nextModuleAction_triggered();
    void on_actionExecutionTextDump_triggered();
    void on_previousModuleAction_triggered();
    void clearComment_triggered();
    void clearManualValidation_triggered();
    void clearExecutionComment();
    void commentInstrumentation_triggered();
    void manualValidation_triggered();
    void editPossibilities( bool set_comment, bool clear_comment, bool set_manually_validated, bool clear_manually_validated , bool preprocessor_view);
    void preprocessorViewAction_toggled( bool b );
    void emitViewType();
    void setViewType( CSMesUndoRedoFramework::source_type_t t );
    void changeStyle(const QString &styleName);
    void on_actionCompareCSMes_triggered() ;
    void on_actionStopComparingCSMes_triggered();
    void on_actionBlackBoxTestingMode_toggled(bool);
protected slots:
    void on_actionOptimizedExecutionOrder_triggered();
    void updateLastRecentOpenedCSMes();
    void updateCompareWithLastRecentOpenedCSMes();
    void on_actionNewCodeAnalysis_toggled(bool);
    void on_actionRegressionAnalysis_toggled(bool);
    void on_actionTestCocoon_Website_triggered(bool);
    void on_actionTest_Coverage_Count_triggered(bool);
    void requestNextModule(Instrumentation::filter_t);
    void requestPreviousModule(Instrumentation::filter_t);
    void on_exportStatisticModuleAction_triggered();
    void on_exportStatisticFunctionAction_triggered();
    void on_exportStatisticEMMAAction_triggered();
    void on_findTextAction_triggered();
    void coverageLevelChanged();
    void CSExeFileModified();
    void contextMenu( const QPoint &pos);
    void configureToolBar();
    void on_executionBenefitAnalysisAction_triggered(bool);
    void lastRecentOpenedCSMes_triggered();
    void compareWithRecentlyOpen_triggered();
    void updateWindowMenu();
    void setActiveSubWindow(QWidget *window);
    void _setExecutionComment(const QString &name,const QString &comment);
    void _saveExecutionComment();
    void on_saveBlackBoxConfigurationAction_triggered();
    void importAutomaticallyEvent();
    void fullUpdate();
    void partialUpdate();
    void requestFindTextModule ( const QString & exp, QTextDocument::FindFlags options  );

signals:
    void firstInstrumentation(Instrumentation::filter_t t);
    void lastInstrumentation(Instrumentation::filter_t t);
    void previousInstrumentation(Instrumentation::filter_t);
    void nextInstrumentation(Instrumentation::filter_t);
    void searchFilterChanged(Instrumentation::filter_t);
    void clearComment();
    void clearManuallyValidated();
    void setComment();
    void showFindText();
    void setManuallyValidated();
    void loadCSExeFileFinished(bool error, const QString &info);
protected:
    virtual void closeEvent( QCloseEvent * e );
private:
    void mergeFiles(const QStringList &filenames,CSMESFile::merge_policy_t policy);
    void setExecutionAnalysisMode(bool b);
    bool getTestCoverageMode() const ;
    bool compareCSMes(const QString &filename);
    QString getWorkingModeStr() const;
    void setWorkingMode(CoverageSettings::working_mode_t w,bool force=false);
    CoverageSettings::working_mode_t getWorkingMode() const;
    bool isAutoSavedOnExit() const;
    void setCoverageMethod(Instrumentation::coverage_method_t method) ;
    QSignalMapper *window_mapper_p ;
    void createWindowMenu();
    void createViewMenu();
    WSourceView *activeWSourceView() const;
    QAction *actionSeparatorWindow;
    WExpressionHelp *filter_help_dialog_p;
    CSMesUndoRedoFramework *csmes_file_p;
    QString csexe_file;
    QString csexe_name;
    QString csexe_script;
    QStringList csexe_arguments;
    WLoadExecution::type_t csexe_type;
    WLoadExecution::watch_t csexe_watch;
    WLoadExecution *load_executions_dialog_p;
    CSMesUndoRedoFramework::csexe_import_policy_t csexe_importPolicy;
    Executions::execution_status_t csexe_default_status;
    bool csexe_deleteAfterLoading;
    bool csexe_minimize_memory_usage;
    QActionGroup *coverageMethodGroup_p;

    QtToolBarManager *toolbar_manager_p;
    FileSystemWatcher *file_system_watcher_p;
    QMdiArea *workspace_p;
    SignalMultiplexer multiplexer;
    bool delete_csexe_file;
    QString cur_source;

    virtual bool tryExit();
    void SetWindowTitle();
    void updateCoverageLevel();
    void LoadExecution(const WLoadExecution &exe,bool set_watch_only);
    void addLastRecentOpenedCSMes(const QString &file);
    void lastRecentOpenedCSMes(QStringList &active,QStringList &inactive) const;
    bool csmesFileSaved() ;
    void setWhiteBoxMode();
    void setBlackBoxMode();
    QAction *undoAction ;
    QAction *redoAction ;
    void quitApplication();
    QTimer importAutomatically;
    QUndoView *undoView;
    QDockWidget *dwUndoView;
    QString execution_comment_title;
    IdleEvent *fullUpdateEvent;
    IdleEvent *partialUpdateEvent;
    CSMes::comparaison_mode_t getReleaseComparaisonMode() const;
    bool getExecutionAnalysisMode() const;
private :
    static void _partialUpdate(QObject*);
    void _partialUpdate();
    static void _fullUpdate(QObject*);
    void _fullUpdate();
    void setCoverageLevel(int) ;
    WLoadExecutionInfo *_load_execution_info_p;
    static void enableToolbar(QToolBar *toolbar_p,bool enable);
    static void enableDock(QDockWidget *dock_p,bool enable);

private:
    friend class BenchOptimizedExecutionOrder;
};

#endif
