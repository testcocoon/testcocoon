cmake_minimum_required(VERSION 2.6.0)
SET(PROGNAME csmes)
INCLUDE(../commoncode/common.cmake)

SET(SRCS
    csmesfile.cpp
    getoption.cpp 
    debug.cpp 
    crc.cpp 
    tmpfile.cpp
    stringtool.cpp
    fileinfo.cpp 
    hexcoder.cpp 
    system.cpp
  )
if (WIN32)
  SET(SRCS ${SRCS} system_win32.cpp
		  ms_dirent.cpp)
else (WIN32)
  IF("${CMAKE_SYSTEM}" MATCHES "Linux")
    SET(SRCS ${SRCS} system_linux.cpp)
  ELSE ("${CMAKE_SYSTEM}" MATCHES "Linux")
    SET(SRCS ${SRCS} system_macos.cpp)
  ENDIF ("${CMAKE_SYSTEM}" MATCHES "Linux")
  SET(SRCS ${SRCS} system_unix.cpp)
endif ()

INCLUDE_DIRECTORIES(../commoncode)


ADD_LIBRARY(${PROGNAME} ${LIBRARY_TYPE}  ${SRCS} )
IF (WIN32)
   set_property(TARGET ${PROGNAME} PROPERTY FOLDER "libraries")
ENDIF()

INCLUDE(../commoncode/install.cmake)
