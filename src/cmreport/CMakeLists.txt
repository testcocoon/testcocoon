cmake_minimum_required(VERSION 2.6.0)
SET(PROGNAME cmreport)
INCLUDE(../commoncode/common.cmake)

  set(SRCS
      cmreport.cpp 
  )




INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(../commoncode ../libcmreport ../libcsmes ../libinstrumentation)

if(WIN32)
  set(RESOURCES cmreport.rc)
endif(WIN32)

ADD_EXECUTABLE(${PROGNAME} ${RESOURCES}  ${SRCS}  )
TARGET_LINK_LIBRARIES(${PROGNAME} cmreportlib csmes instrumentation ${PLATFORM_LIBS} ${QT_LIBRARIES})
IF (WIN32)
   set_property(TARGET ${PROGNAME} PROPERTY FOLDER "executables")
ENDIF()

INCLUDE(../commoncode/install.cmake)
