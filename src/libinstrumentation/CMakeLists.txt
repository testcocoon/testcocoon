cmake_minimum_required(VERSION 2.6.0)
SET(PROGNAME instrumentation)
INCLUDE(../commoncode/common.cmake)
IF("${CMAKE_SYSTEM}" MATCHES "Linux")
  FIND_PROGRAM(FLEX_EXECUTABLE flex)
  FIND_PROGRAM(BISON_EXECUTABLE bison)
endif()

set(SRCS
    executionname.cpp
    executionnames.cpp
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


IF(DEFINED FLEX_EXECUTABLE AND DEFINED BISON_EXECUTABLE)
  MACRO (GENPARSER FLEX_ARGS BISON_ARGS PARSER_NAME PARSER_POSTFIX)
#SET(BISON_GLOBAL_ARGS --no-lines)
#SET(FLEX_GLOBAL_ARGS --noline)
# Create custom command for flex/lex (note the outputs)
     ADD_CUSTOM_COMMAND(
       OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_NAME}_lex_${PARSER_POSTFIX}.cxx
       COMMAND ${FLEX_EXECUTABLE} ${FLEX_ARGS} ${FLEX_GLOBAL_ARGS} -P ${PARSER_NAME} -o ${PARSER_NAME}_lex_${PARSER_POSTFIX}.cxx ${PARSER_NAME}.l
       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_NAME}.l
       )

# Create custom command for bison/yacc (note the DEPENDS)
     ADD_CUSTOM_COMMAND(
       OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_NAME}_yacc_${PARSER_POSTFIX}.cxx
       COMMAND ${BISON_EXECUTABLE} ${BISON_ARGS} -d ${BISON_GLOBAL_ARGS} ${PARSER_NAME}.y -o ${PARSER_NAME}_yacc_${PARSER_POSTFIX}.cxx
       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_NAME}_lex_${PARSER_POSTFIX}.cxx ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_NAME}.y
       )

# Since parser.c does not exists yet when cmake is run, mark
# it as generated
     SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_NAME}_yacc_${PARSER_POSTFIX}.cxx GENERATED)
     SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_NAME}_lex_${PARSER_POSTFIX}.cxx GENERATED)

     ADD_CUSTOM_TARGET (${PARSER_NAME}_${PARSER_POSTFIX}
         DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_NAME}_yacc_${PARSER_POSTFIX}.cxx
                 ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_NAME}_lex_${PARSER_POSTFIX}.cxx )
  ENDMACRO()

  GENPARSER("" ""     csexe_parser       release)
  GENPARSER("" "-t"   csexe_parser       debug)
endif()

if ("${CMAKE_BUILD_TYPE_UPPER}" STREQUAL "LOG")
  SET(PARSER_BUILD_MODE debug)
else()
  SET(PARSER_BUILD_MODE release)
endif()
  
SET(SRCS ${SRCS}
      csexe_parser_yacc_${PARSER_BUILD_MODE}.cxx
      csexe_parser_lex_${PARSER_BUILD_MODE}.cxx
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
