cmake_minimum_required(VERSION 2.6.0)
SET(PROGNAME cmcsexeimportlib)
INCLUDE(../commoncode/common.cmake)

  set(SRCS
      cmcsexeimport_args.cpp 
  )


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(../commoncode ../libcsmes ../libinstrumentation )

ADD_LIBRARY(${PROGNAME} STATIC  ${SRCS} )

INCLUDE(../commoncode/install.cmake)
