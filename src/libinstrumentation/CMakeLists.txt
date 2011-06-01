cmake_minimum_required(VERSION 2.6.0)
SET(PROGNAME instrumentation)
INCLUDE(../commoncode/common.cmake)

set(SRCS
    csmesinstrumentationdatabase.cpp
    instrumentation.cpp
    Service.cpp
    csmesanalysis.cpp
    csmescomment.cpp
    csmes.cpp
    csmesexecution.cpp
    sourcefile.cpp
    sourcefiles.cpp
    modulefile.cpp
    modulefiles.cpp
    csmesfilethread.cpp
    csmesfunctioninfo.cpp
    csmesinstrumentation.cpp
    csmesio.cpp
    csmesreport.cpp
    csmestextreport.cpp
    csmesrtf.cpp
    csmesstatistic.cpp
    executions.cpp
    FunctionInfo.cpp
    rtf.cpp
    TextSourceInstrumentationData.cpp
    FileSystemWatcher.cpp
    diffalgo.cpp
    accesslocker.cpp
    writer.cpp
    htmlwriter.cpp
    xmlwriter.cpp
    csmesemma.cpp
    csmescsv.cpp
)

set(MOC_HDRS
    FileSystemWatcher.h
)

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS})

# tell cmake to create .moc files for all files in the variable SRCS that require such a file.
# note: this assumes that you use #include "header.moc" in your files
#qt4_automoc(${SRCS})


# link the "${PROJECT}" target against the Qt libraries. which libraries exactly, is defined by the "include(${QT_USE_FILE})" line above, which sets up this variable.
#target_link_libraries(${PROJECT} ${QT_LIBRARIES})


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})


INCLUDE_DIRECTORIES(../commoncode ../libcsmes)
#Now add these generated files to the ADD_EXECUTABLE step
# If this is NOT done, then the ui_*.h files will not be generated
ADD_LIBRARY(${PROGNAME} ${LIBRARY_TYPE} ${MOC_SRCS} ${SRCS} )

TARGET_LINK_LIBRARIES(${PROGNAME} csmes ${QT_LIBRARIES})
IF (WIN32)
   set_property(TARGET ${PROGNAME} PROPERTY FOLDER "libraries")
ENDIF()

INCLUDE(../commoncode/install.cmake)
