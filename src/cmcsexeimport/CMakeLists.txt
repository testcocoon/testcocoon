cmake_minimum_required(VERSION 2.6.0)
SET(PROGNAME cmcsexeimport)
INCLUDE(../commoncode/common.cmake)

  set(SRCS
      cmcsexeimport.cpp 
  )


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(../commoncode ../libcsmes ../libinstrumentation ../libcmcsexeimport)

if(WIN32)
  set(RESOURCES cmcsexeimport.rc)
endif(WIN32)

ADD_EXECUTABLE(${PROGNAME} ${RESOURCES}  ${SRCS}  )
TARGET_LINK_LIBRARIES(${PROGNAME} cmcsexeimportlib csmes instrumentation ${PLATFORM_LIBS} ${QT_LIBRARIES})
IF (WIN32)
   set_property(TARGET ${PROGNAME} PROPERTY FOLDER "executables")
ENDIF()

INCLUDE(../commoncode/install.cmake)
