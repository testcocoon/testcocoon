cmake_minimum_required(VERSION 2.6.0)

project(TestCocoon) # the name of your project
include("src/commoncode/common.cmake")
include("src/commoncode/version.cmake")

add_subdirectory("src")
add_subdirectory("doc" EXCLUDE_FROM_ALL)
MESSAGE( STATUS "Build mode: " ${CMAKE_BUILD_TYPE_UPPER} )

SET(SRCTBZ2 ${CMAKE_CURRENT_SOURCE_DIR}/binaries/testcocoon-src-${TESTCOCOON_VERSION_DOT}.tar.bz2)
SET(SRC_CONTAINS HEAD src/ gen/ doc/ cppunit-1.12.0/ tutorial/ CMakeLists.txt)
ADD_CUSTOM_TARGET(src 
      COMMAND git archive --format=tar --prefix=testcocoon-src-${TESTCOCOON_VERSION_DOT}/ ${SRC_CONTAINS}| bzip2 > ${SRCTBZ2}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
IF (WIN32)
  set_property(TARGET src PROPERTY FOLDER "Tools")
ENDIF()
