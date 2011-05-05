cmake_minimum_required(VERSION 2.6.0)
SET(PROGNAME toolselector)
INCLUDE(../commoncode/common.cmake)

set(SRCS
    toolselector.cpp 
    intelcompiler.cpp 
    notools.cpp 
    WMainUI.cpp
    msys.cpp 
    mingw.cpp 
    cygwin.cpp 
    visualstudio.cpp 
    tools.cpp
)

set(MOC_HDRS
    WMainUI.h 
   )

set(RC
   toolselector.qrc
)

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(MOC_SRCS ${MOC_HDRS})

# tell cmake to create .moc files for all files in the variable SRCS that require such a file.
# note: this assumes that you use #include "header.moc" in your files
#qt4_automoc(${SRCS})


# link the "${PROJECT}" target against the Qt libraries. which libraries exactly, is defined by the "include(${QT_USE_FILE})" line above, which sets up this variable.
#target_link_libraries(${PROJECT} ${QT_LIBRARIES})

SET(UIS
    WMain.ui
)

QT4_WRAP_UI(UIS_H ${UIS})
QT4_ADD_RESOURCES(RCS ${RC})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})


if(WIN32)
  set(RESOURCES toolselector.rc)
endif(WIN32)

SOURCE_GROUP(Resources FILES ${TRANS} ${RC} ${RESOURCES})
SOURCE_GROUP(UI FILES ${UIS})
SOURCE_GROUP(Generated FILES ${RCS} ${MOC_SRCS} ${UIS_H} ${QM})


#Now add these generated files to the ADD_EXECUTABLE step
# If this is NOT done, then the ui_*.h files will not be generated
ADD_EXECUTABLE(${PROGNAME} ${GUI_TYPE} ${RESOURCES} ${RCS} ${MOC_SRCS} ${SRCS} ${UIS_H} )
TARGET_LINK_LIBRARIES(${PROGNAME} ${QT_LIBRARIES})
IF (WIN32)
   set_property(TARGET ${PROGNAME} PROPERTY FOLDER "executables")
ENDIF()

INCLUDE(../commoncode/install.cmake)
