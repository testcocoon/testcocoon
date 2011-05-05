cmake_minimum_required(VERSION 2.6.0)
SET(PROGNAME cmmerge)
INCLUDE(../commoncode/common.cmake)

  set(SRCS
      cmmerge.cpp 
  )


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(../commoncode ../libcsmes ../libcmmerge   )

if(WIN32)
  set(RESOURCES cmmerge.rc)
endif(WIN32)

ADD_EXECUTABLE(${PROGNAME} ${RESOURCES} ${SRCS}  )
TARGET_LINK_LIBRARIES(${PROGNAME} cmmergelib csmes ${PLATFORM_LIBS})
IF (WIN32)
   set_property(TARGET ${PROGNAME} PROPERTY FOLDER "executables")
ENDIF()

INCLUDE(../commoncode/install.cmake)
