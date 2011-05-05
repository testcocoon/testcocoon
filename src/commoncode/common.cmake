GET_FILENAME_COMPONENT(COMMON_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if (NOT DEFINED TESTCOCOON_TOP_SOURCE_DIR)
  SET (TESTCOCOON_TOP_SOURCE_DIR ${PROJECT_SOURCE_DIR})
endif()
if (NOT DEFINED TESTCOCOON_TOP_BINARY_DIR)
  SET (TESTCOCOON_TOP_BINARY_DIR ${PROJECT_BINARY_DIR})
endif()
SET(TESTCOCOON_SRC_SOURCE_DIR "${TESTCOCOON_TOP_SOURCE_DIR}/src")
SET(TESTCOCOON_SRC_BINARY_DIR "${TESTCOCOON_TOP_BINARY_DIR}/src")
SET(TESTCOCOON_DOC_SOURCE_DIR "${TESTCOCOON_TOP_SOURCE_DIR}/doc")
SET(TESTCOCOON_DOC_BINARY_DIR "${TESTCOCOON_TOP_BINARY_DIR}/doc")
SET(TESTCOCOON_SOURCE_DIR "${TESTCOCOON_TOP_SOURCE_DIR}")
SET(TESTCOCOON_BINARY_DIR "${TESTCOCOON_TOP_BINARY_DIR}")

include("${TESTCOCOON_SRC_SOURCE_DIR}/config.cmake")
IF (BUILD_SHARED_LIBS)
   string(TOUPPER "${PROGNAME}" PROGNAME_UPPER)
   ADD_DEFINITIONS(-DTESTCOCOON_SHARED)
   ADD_DEFINITIONS(-DTESTCOCOON_SHARED_${PROGNAME_UPPER})
  SET(LIBRARY_TYPE SHARED)
ELSE()
  SET(LIBRARY_TYPE STATIC)
ENDIF()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
if (NOT DEFINED PNGTARGET)
  SET(PNGTARGET "Yes")
  ADD_CUSTOM_TARGET(png)
   IF (WIN32)
     set_property(TARGET png PROPERTY FOLDER "Tools")
   ENDIF()
ENDIF()
IF (WIN32)
  SET(EXE_SUFFIX ".exe")
  SET(BATCH_SUFFIX ".bat")
ELSE()
  SET(BATCH_SUFFIX ".sh")
ENDIF (WIN32)

FIND_PACKAGE(Qt4 ) # find and setup Qt4 for this project

IF (CMAKE_COMPILER_IS_GNUCXX)
  SET( CMAKE_C_FLAGS_RELEASE "-O2" CACHE STRING
      "Flags used by the C compiler during release builds."
      FORCE )
  SET( CMAKE_CXX_FLAGS_RELEASE "-O2" CACHE STRING
      "Flags used by the C++ compiler during release builds."
      FORCE )
ENDIF()
# Update the documentation string of CMAKE_BUILD_TYPE for GUIs
SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: Debug Release Log."
    FORCE )
string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)

SET(QTVER "4.6.3")

if (WIN32)
   SET(QTVER "4.6.3")
   ADD_DEFINITIONS(-D_UNICODE)
   ADD_DEFINITIONS(-DUNICODE)
   ADD_DEFINITIONS(-DYY_NO_UNISTD_H)
   ADD_DEFINITIONS(-DOS_WIN32)
   ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
   ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
   IF (${CMAKE_CL_64})
     SET(QT_QMAKE_EXECUTABLE \\QT\\x64\\qt-everywhere-opensource-src-${QTVER}\\bin\\qmake.exe )
   ELSE(${CMAKE_CL_64})
     SET(QT_QMAKE_EXECUTABLE \\QT\\x86\\qt-everywhere-opensource-src-${QTVER}\\bin\\qmake.exe )
   ENDIF(${CMAKE_CL_64})
  SET(PLATFORM_LIBS Ws2_32 Advapi32 iphlpapi)

   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-")
   SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Zc:wchar_t-")

ELSEIF(APPLE)
   ADD_DEFINITIONS(-DOS_MACX)
#SET(CMAKE_C_COMPILER "gcc-4.0")
#SET(CMAKE_CXX_COMPILER "g++-4.0")
  LINK_DIRECTORIES(/usr/local/Trolltech/Qt-${QTVER}/lib)
ELSEIF(UNIX)
#SET(CMAKE_CXX_COMPILER "g++")
#SET(CMAKE_C_COMPILER "gcc")
   ADD_DEFINITIONS(-DOS_LINUX)

  LINK_DIRECTORIES(/usr/local/Trolltech/Qt-${QTVER}/lib)
else()
	MESSAGE(FATAL_ERROR "Platform not supported")
endif()

# COVERAGE build mode
IF ("${CMAKE_BUILD_TYPE_UPPER}" STREQUAL "COVERAGE")
#SET(CMAKE_VERBOSE_MAKEFILE ON)
#SET(CMAKE_START_TEMP_FILE "")
#SET(CMAKE_END_TEMP_FILE "")
  SET(COVERAGE_FLAGS "--cs-on --cs-count --cs-include-path=${TESTCOCOON_SRC_SOURCE_DIR}/libcoveragebrowser --cs-include-path=${TESTCOCOON_SRC_SOURCE_DIR}/libcoveragescanner --cs-include-path=${TESTCOCOON_SRC_SOURCE_DIR}/libinstrumentation --cs-include-path=${TESTCOCOON_SRC_SOURCE_DIR}/libcmcsexeimport --cs-include-path=${TESTCOCOON_SRC_SOURCE_DIR}/libcmreport --cs-include-path=${TESTCOCOON_SRC_SOURCE_DIR}/libcmmerge --cs-include-path=${TESTCOCOON_SRC_SOURCE_DIR}/libcsmes --cs-qt4 --cs-exclude-file-wildcard=moc_* --cs-exclude-file-wildcard=ui_* --cs-exclude-file-wildcard=qrc_* ")
  if (WIN32)
    SET(COVERAGE_FLAGS "${COVERAGE_FLAGS} --cs-libgen=/MD")
  endif()
ENDIF()
SET( CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_C_FLAGS_RELEASE} ${COVERAGE_FLAGS}" CACHE STRING
    "Flags used by the C++ compiler during coverage builds."
    FORCE )
SET( CMAKE_C_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_RELEASE} ${COVERAGE_FLAGS}" CACHE STRING
    "Flags used by the C compiler during coverage builds."
    FORCE )
SET( CMAKE_EXE_LINKER_FLAGS_COVERAGE
    "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${COVERAGE_FLAGS}" CACHE STRING
    "Flags used for linking binaries during coverage builds."
    FORCE )
SET( CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} ${COVERAGE_FLAGS}" CACHE STRING
    "Flags used by the shared libraries linker during coverage builds."
    FORCE )
SET( CMAKE_STATIC_LINKER_FLAGS_COVERAGE
    "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} ${COVERAGE_FLAGS}" CACHE STRING
    "Flags used by the static libraries linker during coverage builds."
    FORCE )
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_C_FLAGS_COVERAGE
    CMAKE_EXE_LINKER_FLAGS_COVERAGE
    CMAKE_SHARED_LINKER_FLAGS_COVERAGE 
    CMAKE_STATIC_LINKER_FLAGS_COVERAGE 
    COMPILE_DEFINITIONS_COVERAGE
)


# LOG build mode
SET( CMAKE_CXX_FLAGS_LOG "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING
    "Flags used by the C++ compiler during log builds."
    FORCE )
SET( CMAKE_C_FLAGS_LOG "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING
    "Flags used by the C compiler during log builds."
    FORCE )
SET( CMAKE_EXE_LINKER_FLAGS_LOG
    "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING
    "Flags used for linking binaries during log builds."
    FORCE )
SET( CMAKE_SHARED_LINKER_FLAGS_LOG
    "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" CACHE STRING
    "Flags used by the shared libraries linker during log builds."
    FORCE )
SET( CMAKE_STATIC_LINKER_FLAGS_LOG
    "${CMAKE_STATIC_LINKER_FLAGS_RELEASE}" CACHE STRING
    "Flags used by the static libraries linker during log builds."
    FORCE )
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_LOG
    CMAKE_C_FLAGS_LOG
    CMAKE_EXE_LINKER_FLAGS_LOG
    CMAKE_SHARED_LINKER_FLAGS_LOG 
    CMAKE_STATIC_LINKER_FLAGS_LOG 
    COMPILE_DEFINITIONS_LOG
)


# PROFILER build mode
SET(PROFILE_FLAGS "-pg")

SET( CMAKE_CXX_FLAGS_PROFILER "${CMAKE_C_FLAGS_RELEASE} ${PROFILE_FLAGS}" CACHE STRING
    "Flags used by the C++ compiler during profiler builds."
    FORCE )
SET( CMAKE_C_FLAGS_PROFILER "${CMAKE_CXX_FLAGS_RELEASE} ${PROFILE_FLAGS}" CACHE STRING
    "Flags used by the C compiler during profiler builds."
    FORCE )
SET( CMAKE_EXE_LINKER_FLAGS_PROFILER
    "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${PROFILE_FLAGS}" CACHE STRING
    "Flags used for linking binaries during profiler builds."
    FORCE )
SET( CMAKE_SHARED_LINKER_FLAGS_PROFILER
    "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} ${PROFILE_FLAGS}" CACHE STRING
    "Flags used by the shared libraries linker during profiler builds."
    FORCE )
SET( CMAKE_STATIC_LINKER_FLAGS_PROFILER
    "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} ${PROFILE_FLAGS}" CACHE STRING
    "Flags used by the static libraries linker during profiler builds."
    FORCE )
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_PROFILER
    CMAKE_C_FLAGS_PROFILER
    CMAKE_EXE_LINKER_FLAGS_PROFILER
    CMAKE_SHARED_LINKER_FLAGS_PROFILER 
    CMAKE_STATIC_LINKER_FLAGS_PROFILER 
    COMPILE_DEFINITIONS_PROFILER
)



IF("${CMAKE_SYSTEM}" MATCHES "Linux")
    # use, i.e. don't skip the full RPATH for the build tree
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 

    # the RPATH to be used when installing
    SET(CMAKE_INSTALL_RPATH "/opt/TestCocoon/${TESTCOCOON_VERSION_MAJOR}.${TESTCOCOON_VERSION_MINOR}.${TESTCOCOON_VERSION_PATCH}/")

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
ENDIF()


SET( CMAKE_INCLUDE_CURRENT_DIR TRUE )
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/../commoncode)

SET_DIRECTORY_PROPERTIES(PROPERTIES
 COMPILE_DEFINITIONS_RELEASE "${COMPILE_DEFINITIONS_RELEASE};NO_DEBUG"
    )

SET_DIRECTORY_PROPERTIES(PROPERTIES
 COMPILE_DEFINITIONS_COVERAGE "${COMPILE_DEFINITIONS_RELEASE};NO_DEBUG"
    )

SET_DIRECTORY_PROPERTIES(PROPERTIES
 COMPILE_DEFINITIONS_LOG "${COMPILE_DEFINITIONS_RELEASE};NO_DEBUG;LOG=1"
    )


# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
#   set(QT_USE_OPENGL TRUE)
# -> this will cause cmake to include and link against the OpenGL module
include(${QT_USE_FILE})
# see http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4 for a complete list

SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTMAIN TRUE)
SET(QT_USE_QTSVG TRUE)
# Set some Win32 Specific Settings
IF(WIN32)
SET(GUI_TYPE WIN32)
ENDIF(WIN32)
# Set some Apple MacOS Specific settings
IF (APPLE)
SET(GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)

IF("${CMAKE_SYSTEM}" MATCHES "Linux")
# Automatic generation of PNG files
  IF (DEFINED PROGNAME)
    FIND_PROGRAM(INKSCAPE_COMMAND inkscape)
    SET(INKSCAPE_OPT --without-gui)
    IF (DEFINED INKSCAPE_COMMAND)
      FILE(GLOB SVG_FILES_IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/images/*.svg")
      SET(SVG_FILES ${SVG_FILES_IMAGES} )
    ENDIF()

    IF (DEFINED GENSRCIMG)
      FOREACH(file ${SVG_FILES})
        
        GET_FILENAME_COMPONENT(basename "${file}" NAME_WE)
        SET(PNG_FILE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/images/${basename}.png)
        ADD_CUSTOM_COMMAND(
            OUTPUT ${PNG_FILE_SRC}
            COMMAND ${INKSCAPE_COMMAND} ARGS ${INKSCAPE_OPT} "${file}"  --export-png=${PNG_FILE_SRC}
            DEPENDS ${file} 
            )
        SET(ALL_PNG_FILES_SRC ${ALL_PNG_FILES_SRC} ${PNG_FILE_SRC})

      ENDFOREACH(file)
    ENDIF()

    ADD_CUSTOM_TARGET(${PROGNAME}_png DEPENDS ${ALL_PNG_FILES_SRC})
    ADD_DEPENDENCIES(png ${PROGNAME}_png)
  ENDIF()
ENDIF()

