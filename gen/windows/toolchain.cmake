#INCLUDE(CMakeForceCompiler)

# this one is important
SET(CMAKE_SYSTEM_NAME Windows)

# specify the cross compiler
FILE(TO_CMAKE_PATH "$ENV{TESTCOCOON}/visualstudio" TESTCOCOON)
FILE(TO_CMAKE_PATH "${CMAKE_BINARY_DIR}\\..\\release\\src\\coveragescanner\\visualstudio" TESTCOCOON)
SET(CMAKE_C_COMPILER ${TESTCOCOON}/cl.exe CACHE FILEPATH "CoverageScanner wrapper" FORCE)
SET(CMAKE_CXX_COMPILER ${TESTCOCOON}/cl.exe CACHE FILEPATH "CoverageScanner wrapper" FORCE)
SET(CMAKE_LINKER ${TESTCOCOON}/link.exe CACHE FILEPATH "CoverageScanner wrapper" FORCE)
