PROJECT(testcocoondoc NONE)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
INCLUDE(UseLATEX.cmake)
SET(TESTCOCOON_DOC_PDF ${CMAKE_CURRENT_BINARY_DIR}/testcocoon.pdf)
SET(LATEX_SRCS 
              ../src/LICENSE.TXT
              customiofile.c
              customiosftp.c
              sort.c
              visualdsp.c
              testcoverageobject.h
              bench_athlon.tex
              bench_pentium_m.tex
              bench_via.tex
              codesize.tex
              coveragebrowsertutorial.tex
              testcocoondownload.tex
              testcocooneclipse.tex
              testcocoonlicense.tex
              testcocoonoverview.tex
              testcocoon.tex
              testcocoonvisualdsp.tex
              CppUnitListener.cpp
              googletest.cpp
              CxxTestListener.cpp
              download.tex
              eclipse.tex
              faq.tex
              license.tex
              overview.tex
              pdflayout.tex
              testcoverageobject.cpp
              tutorial.tex
              visualdsp.tex
              testcocoon.tex
              )

ADD_LATEX_DOCUMENT(testcocoon.tex
      INPUTS ${LATEX_SRCS}
      IMAGE_DIRS  pdffiles 
      DEFAULT_PDF USE_INDEX
      )


IF (DEFINED GENSRCIMG)
# SVG to PDF/EPS
  FIND_PROGRAM(INKSCAPE_COMMAND inkscape)
  SET(INKSCAPE_OPT --without-gui)
  SET(CONVERT_OPT )
  IF (DEFINED INKSCAPE_COMMAND)
    FILE(GLOB SVG_FILES_IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/images/*.svg")
    FILE(GLOB SVG_FILES_COVERAGEBROWSER "${TESTCOCOON_SRC_SOURCE_DIR}/coveragebrowser/images/*.svg")
    SET(SVG_FILES ${SVG_FILES_IMAGES} ${SVG_FILES_COVERAGEBROWSER})
  ENDIF()
  FILE(GLOB PNG_FILES_IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/images/*.png")
  SET(PNG_FILES ${PNG_FILES_IMAGES})

  FOREACH(file ${PNG_FILES})
    
    GET_FILENAME_COMPONENT(basename "${file}" NAME_WE)
    SET(PDF_FILE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/pdffiles/${basename}.pdf)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${PDF_FILE_SRC}
        COMMAND ${CMAKE_COMMAND} ARGS -E make_directory  ${CMAKE_CURRENT_SOURCE_DIR}/pdffiles
        COMMAND ${IMAGEMAGICK_CONVERT} ARGS ${CONVERT_OPT} "${file}"  pdf:${PDF_FILE_SRC}.tmp
        COMMAND pdftk ARGS ${PDF_FILE_SRC}.tmp update_info ${CMAKE_CURRENT_SOURCE_DIR}/pdfdata.txt output ${PDF_FILE_SRC}
        COMMAND ${CMAKE_COMMAND} ARGS -E remove ${PDF_FILE_SRC}.tmp 
        DEPENDS ${file} 
        )
    SET(ALL_PDF_FILES_SRC ${ALL_PDF_FILES_SRC} ${PDF_FILE_SRC})

  ENDFOREACH(file)

  FOREACH(file ${SVG_FILES})
    
    GET_FILENAME_COMPONENT(basename "${file}" NAME_WE)
    SET(PDF_FILE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/pdffiles/${basename}.pdf)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${PDF_FILE_SRC}
        COMMAND ${CMAKE_COMMAND} ARGS -E make_directory  ${CMAKE_CURRENT_SOURCE_DIR}/pdffiles
        COMMAND ${INKSCAPE_COMMAND} ARGS ${INKSCAPE_OPT} "${file}"  --export-pdf=${PDF_FILE_SRC}.tmp
        COMMAND pdftk ARGS ${PDF_FILE_SRC}.tmp update_info ${CMAKE_CURRENT_SOURCE_DIR}/pdfdata.txt output ${PDF_FILE_SRC}
        COMMAND ${CMAKE_COMMAND} ARGS -E remove ${PDF_FILE_SRC}.tmp 
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/images"
        DEPENDS ${file} 
        )
    SET(ALL_PDF_FILES_SRC ${ALL_PDF_FILES_SRC} ${PDF_FILE_SRC})

  ENDFOREACH(file)
  ADD_CUSTOM_TARGET(pdffiles DEPENDS ${ALL_PDF_FILES_SRC})
ENDIF()

ADD_CUSTOM_TARGET(kpdf
                  COMMAND kpdf ${TESTCOCOON_DOC_PDF}
                  DEPENDS  ${TESTCOCOON_DOC_PDF}
    )

INSTALL (FILES ${TESTCOCOON_DOC_PDF} DESTINATION share/doc/testcocoon)
