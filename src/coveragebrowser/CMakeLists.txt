cmake_minimum_required(VERSION 2.6.0)
SET( PROGNAME coveragebrowser )

INCLUDE(../commoncode/common.cmake)


SET(SRCS
      coveragebrowser.cpp 
      )
set(RC coveragebrowser.qrc)

if(WIN32)
  set(RESOURCES coveragebrowser.rc)
endif(WIN32)

QT4_ADD_RESOURCES(RCS ${RC})

SOURCE_GROUP(Resources FILES ${RC} ${RESOURCES})
SOURCE_GROUP(Generated FILES ${RCS} )

INCLUDE_DIRECTORIES(../commoncode ../libcsmes ../libinstrumentation ../libcoveragebrowser ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../libcoveragebrowser)

ADD_EXECUTABLE(${PROGNAME} ${GUI_TYPE} ${RESOURCES} ${RCS}  ${SRCS} )

TARGET_LINK_LIBRARIES(${PROGNAME} csmes instrumentation coveragebrowserlib ${PLATFORM_LIBS} ${QT_LIBRARIES})
if (APPLE)
    SET( MACOSX_BUNDLE_ICON_FILE testcocoon_logo.icns )
    ADD_CUSTOM_COMMAND( TARGET ${PROGNAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} ARGS -E make_directory  ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources
      COMMAND cp ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${MACOSX_BUNDLE_ICON_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources
      COMMAND cp ARGS ${CMAKE_CURRENT_BINARY_DIR}/../libcoveragebrowser/*.qm ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources 
)
endif()
IF (WIN32)
   set_property(TARGET ${PROGNAME} PROPERTY FOLDER "executables")
ENDIF()

INCLUDE(../commoncode/install.cmake)
