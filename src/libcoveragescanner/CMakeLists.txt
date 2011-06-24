cmake_minimum_required(VERSION 2.6.0)
SET(PROGNAME coveragescannerlib)
INCLUDE(../commoncode/common.cmake)
IF("${CMAKE_SYSTEM}" MATCHES "Linux")
  FIND_PROGRAM(FLEX_EXECUTABLE flex)
  FIND_PROGRAM(BISON_EXECUTABLE bison)
endif()

SET(SRCS
  argv.cpp
  c_cpp_libgen.cpp
  coveragescanner_main.cpp
  compiler.cpp
  compilerfactory.cpp
  compilerinterface.cpp
  compilerwrapper.cpp
  compilerwrapperenumoption.cpp
  compilerwrapperintoption.cpp
  compilerwrapperlanguageoption.cpp
  compilerwrapperlistoption.cpp
  compilerwrapperoption.cpp
  compilerwrapperoptionpool.cpp
  compilerwrapperregexpoption.cpp
  expression_computation.cpp
  expression_condition.cpp
  expression_subcondition.cpp
  expression_nop.cpp
  expression_label.cpp
  expression_case.cpp
  expression_main_function.cpp
  expression_assignment.cpp
  expression_if_then.cpp
  expression_while.cpp
  expression_for.cpp
  expression_parenthesis_expr.cpp
  expression_not_expr.cpp
  expression_some_binary_operator_expr.cpp
  expression_or_expr.cpp
  expression_and_expr.cpp
  expression_if_then_else.cpp
  expression_condition_false_only.cpp
  patch_cs_label_start.cpp
  patch_cs_label_end.cpp
  patch_cpp_label_start.cpp
  patch_cpp_label_end.cpp
  patch_cpp_computation_start.cpp
  patch_cpp_full_condition_start.cpp
  patch_cpp.cpp
  ignorearea.cpp
  ignoreareas.cpp
  patch_cpp_start.cpp
  patch_cpp_end.cpp
  patch_cpp_full_for_end1.cpp
  patch_cpp_full_for_end2.cpp
  patch_cpp_full_for_start1.cpp
  patch_cpp_full_for_start2.cpp
  patch_cpp_semi_for_end1.cpp
  patch_cpp_semi_for_end2.cpp
  patch_cpp_semi_for_start1.cpp
  patch_cpp_semi_for_start2.cpp
  patch_cpp_false_only_condition_start.cpp
  patch_cpp_computation_end.cpp
  patch_cpp_full_condition_end.cpp
  patch_cpp_false_only_condition_end.cpp
  patch_cpp_full_ifthen_end1.cpp
  patch_cpp_full_ifthen_end2.cpp
  patch_cpp_full_ifthen_start1.cpp
  patch_cpp_full_ifthen_start2.cpp
  patch_cpp_semi_ifthen_end1.cpp
  patch_cpp_semi_ifthen_end2.cpp
  patch_cpp_semi_ifthen_start1.cpp
  patch_cpp_semi_ifthen_start2.cpp
  patch_cpp_full_ifthenelse_end1.cpp
  patch_cpp_full_ifthenelse_end2.cpp
  patch_cpp_full_ifthenelse_start1.cpp
  patch_cpp_full_ifthenelse_start2.cpp
  patch_cpp_full_while_end1.cpp
  patch_cpp_full_while_end2.cpp
  patch_cpp_full_while_start1.cpp
  patch_cpp_full_while_start2.cpp
  patch_cpp_semi_while_end1.cpp
  patch_cpp_semi_while_end2.cpp
  patch_cpp_semi_while_start1.cpp
  patch_cpp_semi_while_start2.cpp
  patch.cpp
  patch_cs_computation_start.cpp
  patch_cs_full_condition_start.cpp
  patch_cs.cpp
  patch_cs_start.cpp
  patch_cs_end.cpp
  patch_cs_false_only_condition_start.cpp
  patch_cs_main_body_start.cpp
  patch_cs_main_body_end.cpp
  patch_cs_main_start.cpp
  patch_cs_main_end.cpp
  patch_cs_computation_end.cpp
  patch_cs_full_condition_end.cpp
  patch_cs_false_only_condition_end.cpp
  patch_cs_full_for_end1.cpp
  patch_cs_full_for_end2.cpp
  patch_cs_full_for_start1.cpp
  patch_cs_full_for_start2.cpp
  patch_cs_semi_for_end1.cpp
  patch_cs_semi_for_end2.cpp
  patch_cs_semi_for_start1.cpp
  patch_cs_semi_for_start2.cpp
  patch_cs_full_ifthen_end1.cpp
  patch_cs_full_ifthen_end2.cpp
  patch_cs_full_ifthen_start1.cpp
  patch_cs_full_ifthen_start2.cpp
  patch_cs_semi_ifthen_end1.cpp
  patch_cs_semi_ifthen_end2.cpp
  patch_cs_semi_ifthen_start1.cpp
  patch_cs_semi_ifthen_start2.cpp
  patch_cs_full_ifthenelse_end1.cpp
  patch_cs_full_ifthenelse_end2.cpp
  patch_cs_full_ifthenelse_start1.cpp
  patch_cs_full_ifthenelse_start2.cpp
  patch_cs_full_while_end1.cpp
  patch_cs_full_while_end2.cpp
  patch_cs_full_while_start1.cpp
  patch_cs_full_while_start2.cpp
  patch_cs_semi_while_end1.cpp
  patch_cs_semi_while_end2.cpp
  patch_cs_semi_while_start1.cpp
  patch_cs_semi_while_start2.cpp
  compilerwrappertextoption.cpp
  cs_libgen.cpp
  defines.cpp
  expression.cpp
  expressions.cpp
  expressions_optimization.cpp
  filename.cpp
  filter.cpp
  filterfunctions.cpp
  filtersource.cpp
  function.cpp
  functions.cpp
  libgen.cpp
  linepos.cpp
  memory.cpp
  option.cpp
  parsemisc.cpp
  parser.cpp
  parseroption.cpp
  regerror.cpp
  regexpr.cpp
  regsub.cpp
  scope.cpp
  source.cpp
  stringpool.cpp
  wildcard.cpp
  utf.cpp
  widechar.cpp
  )


#SET(LEX_YACC_WDIR ${CMAKE_CURRENT_SOURCE_DIR}/)

IF(DEFINED FLEX_EXECUTABLE AND DEFINED BISON_EXECUTABLE)
  MACRO (GENPARSER FLEX_ARGS BISON_ARGS PARSER_NAME PARSER_POSTFIX)
#SET(BISON_GLOBAL_ARGS --no-lines)
#SET(FLEX_GLOBAL_ARGS --noline)
# Create custom command for flex/lex (note the outputs)
     ADD_CUSTOM_COMMAND(
       OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_NAME}_lex_${PARSER_POSTFIX}.cxx
       COMMAND ${FLEX_EXECUTABLE} ${FLEX_ARGS} ${FLEX_GLOBAL_ARGS} -P ${PARSER_NAME} -o ${PARSER_NAME}_lex_${PARSER_POSTFIX}.cxx ${LEX_YACC_WDIR}${PARSER_NAME}.l
       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_NAME}.l
       )

# Create custom command for bison/yacc (note the DEPENDS)
     ADD_CUSTOM_COMMAND(
       OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER_NAME}_yacc_${PARSER_POSTFIX}.cxx
       COMMAND ${BISON_EXECUTABLE} ${BISON_ARGS} -d ${BISON_GLOBAL_ARGS} ${LEX_YACC_WDIR}${PARSER_NAME}.y -o ${PARSER_NAME}_yacc_${PARSER_POSTFIX}.cxx
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

  GENPARSER("" ""     c_cs_cpp_parser       release)
  GENPARSER("" "-t"   c_cs_cpp_parser       debug)
  GENPARSER("" "-t"   preprocessor_parser   debug)
  GENPARSER("" ""     preprocessor_parser   release)
endif()

if ("${CMAKE_BUILD_TYPE_UPPER}" STREQUAL "LOG")
  SET(PARSER_BUILD_MODE debug)
else()
  SET(PARSER_BUILD_MODE release)
endif()
  
SET(SRCS ${SRCS}
      preprocessor_parser_yacc_${PARSER_BUILD_MODE}.cxx
      preprocessor_parser_lex_${PARSER_BUILD_MODE}.cxx
      c_cs_cpp_parser_yacc_${PARSER_BUILD_MODE}.cxx
      c_cs_cpp_parser_lex_${PARSER_BUILD_MODE}.cxx
)

INCLUDE_DIRECTORIES(../commoncode ../libcsmes)


#Now add these generated files to the ADD_EXECUTABLE step
# If this is NOT done, then the ui_*.h files will not be generated
ADD_LIBRARY(${PROGNAME} STATIC ${SRCS} )

IF("${CMAKE_SYSTEM}" MATCHES "Linux")
  ADD_DEPENDENCIES(${PROGNAME} c_cs_cpp_parser_release)
  ADD_DEPENDENCIES(${PROGNAME} c_cs_cpp_parser_debug)
  ADD_DEPENDENCIES(${PROGNAME} preprocessor_parser_release)
  ADD_DEPENDENCIES(${PROGNAME} preprocessor_parser_debug)
ENDIF()
IF (WIN32)
   set_property(TARGET ${PROGNAME} PROPERTY FOLDER "libraries")
ENDIF()

INCLUDE(../commoncode/install.cmake)
