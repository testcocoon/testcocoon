cmake_minimum_required(VERSION 2.6.0)
SET( PROGNAME coveragebrowserlib )

INCLUDE(../commoncode/common.cmake)

SET(SRCS
      options.cpp 
      csmesbackgroundcomputation.cpp 
      csmesbackgroundcomputations.cpp 
      coveragebrowser_main.cpp 
      IdleEvent.cpp 
      WTextSourceViewerHighlighter.cpp 
      CoverageSettings.cpp 
      WTreeView.cpp 
      TreeList.cpp 
      TreeListModel.cpp 
      csmesundoredoframework.cpp 
      undoexecutionbackup.cpp 
      undoexecutiondelete.cpp 
      undoexecutionrename.cpp 
      undoexecutionmerge.cpp 
      undoexecutioninsert.cpp 
      WExecutionRename.cpp 
      WHTMLReport.cpp 
      WExecutionMerge.cpp 
      WExecutionDelete.cpp 
      WExpressionHelp.cpp 
      ModuleListModel.cpp 
      ExecutionListDelegate.cpp 
      FunctionListModel.cpp 
      ExecutionListModel.cpp 
      MatchExpr.cpp 
      ModuleListView.cpp 
      FunctionListView.cpp 
      ExecutionListView.cpp 
      SortFilterProxy.cpp 
      helpclient.cpp 
      ProgresBar.cpp 
      coveragebrowser_args.cpp 
      WOption.cpp 
      signalmultiplexer.cpp 
      WComment.cpp 
      WCommentWidget.cpp 
      WOptimizedExecutionOrder.cpp 
      WLoadExecution.cpp 
      WExecutionTextDump.cpp 
      WTextInfo.cpp 
      WLoadExecutionInfo.cpp 
      WFunctionList.cpp 
      WExecutionList.cpp 
      WProcess.cpp 
      WMain.cpp 
      WStatistic.cpp 
      WModuleList.cpp 
      undomanuallyvalidated.cpp 
      undocomment.cpp 
      undoexecutioncomment.cpp 
      undoexecutionsettings.cpp 
      undoexecutionstatus.cpp 
      undoexecutionstatusstr.cpp 
      WTextSourceViewerLine.cpp 
      WTextSourceViewerCount.cpp 
      WTextSourceViewer.cpp 
      settingscheck.cpp 
      WTextSourceViewerContain.cpp 
      WExecutionManipulation.cpp 
      WExplanation.cpp 
      WTextSourceViewerSearch.cpp 
      WSourceView.cpp 
      )

SET(MOC_HDRS
      WHTMLReport.h 
      CoverageSettings.h 
      WTextSourceViewer.h 
      ExecutionListDelegate.h 
      csmesbackgroundcomputation.h 
      csmesbackgroundcomputations.h 
      WTextSourceViewerContain.h 
      TreeListModel.h 
      WOptimizedExecutionOrder.h 
      WComment.h 
      WCommentWidget.h 
      WTextSourceViewerCount.h 
      WTextSourceViewerLine.h 
      WTextSourceViewerHighlighter.h 
      csmesundoredoframework.h 
      WMain.h 
      WStatistic.h 
      signalmultiplexer.h 
      ModuleListModel.h 
      FunctionListModel.h 
      ExecutionListModel.h 
      WProcess.h 
      WTreeView.h 
      WOption.h 
      WLoadExecution.h 
      WExecutionTextDump.h 
      WExecutionManipulation.h 
      WExecutionRename.h 
      WExecutionMerge.h 
      WExecutionDelete.h 
      WTextInfo.h 
      WLoadExecutionInfo.h 
      WModuleList.h 
      WFunctionList.h 
      WExecutionList.h 
      WExpressionHelp.h 
      WTextSourceViewerSearch.h 
      WSourceView.h 
      WExplanation.h 
      ModuleListView.h 
      FunctionListView.h 
      IdleEvent.h 
      ExecutionListView.h 
  )


# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS})

# tell cmake to create .moc files for all files in the variable SRCS that require such a file.
# note: this assumes that you use #include "header.moc" in your files
#qt4_automoc(${SRCS})


# link the "${PROJECT}" target against the Qt libraries. which libraries exactly, is defined by the "include(${QT_USE_FILE})" line above, which sets up this variable.
#target_link_libraries(${PROJECT} ${QT_LIBRARIES})

SET(UIS
   WMainUI.ui 
   WStatisticUI.ui 
   WLoadExecutionUI.ui 
   WExecutionTextDumpUI.ui 
   WOptimizedExecutionOrderUI.ui 
   WExecutionManipulationUI.ui 
   WCommentUI.ui 
   WCommentWidgetUI.ui 
   WOptionUI.ui 
   WTextInfoUI.ui 
   WLoadExecutionInfoUI.ui 
   WHTMLReportUI.ui 
   WProcessUI.ui 
   WExpressionHelpUI.ui 
   WModuleListUI.ui 
   WExecutionListUI.ui 
   WFunctionListUI.ui 
   WTextSourceViewerSearchUI.ui 
   WExplanationUI.ui  
)

QT4_WRAP_UI(UIS_H ${UIS})
# add translations ...
SET(TRANS 
   French.ts
   German.ts
  )

QT4_ADD_TRANSLATION(QM ${TRANS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})


SOURCE_GROUP(Resources FILES ${TRANS} ${RESOURCES})
SOURCE_GROUP(UI FILES ${UIS})
SOURCE_GROUP(Generated FILES ${MOC_SRCS} ${UIS_H} ${QM})

INCLUDE_DIRECTORIES(../commoncode ../libcsmes ../libinstrumentation)

#Now add these generated files to the ADD_EXECUTABLE step
# If this is NOT done, then the ui_*.h files will not be generated

ADD_LIBRARY(${PROGNAME} STATIC  ${QM}  ${MOC_SRCS} ${SRCS} ${UIS_H})

TARGET_LINK_LIBRARIES(${PROGNAME} csmes instrumentation ${PLATFORM_LIBS} ${QT_LIBRARIES})

IF (WIN32)
   set_property(TARGET ${PROGNAME} PROPERTY FOLDER "libraries")
ENDIF()

INCLUDE(../commoncode/install.cmake)
