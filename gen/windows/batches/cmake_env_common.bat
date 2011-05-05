@echo ON
cd %HERE%\..\..
set BUILDIR=build_%BUILD_ARCH%
del /Q /F CMakeCache.txt
del /Q /S /F %BUILDIR%
REM del /Q /S /F "%TESTCOCOON%\logfiles"

md  %BUILDIR%
cd %BUILDIR%


mkdir log
pushd log 
cmake.exe -DCMAKE_BUILD_TYPE=log -G "NMake Makefiles" ../.. 
nmake coveragescanner
popd

md debug
pushd debug
cmake.exe -DCMAKE_BUILD_TYPE=debug -G "NMake Makefiles" ../.. 
nmake coveragescanner
popd

mkdir log
pushd log 
cmake.exe -DCMAKE_BUILD_TYPE=log -G "NMake Makefiles" ../.. 
nmake coveragescanner
popd

md release
pushd release
cmake.exe -DCMAKE_BUILD_TYPE=release -G "NMake Makefiles" ../.. 
nmake coveragescanner
popd


mkdir coverage
pushd coverage 
cmake.exe -DCMAKE_BUILD_TYPE=coverage -DCMAKE_TOOLCHAIN_FILE=%HERE%\toolchain.cmake -G "NMake Makefiles" ../.. 
popd


mkdir vs
pushd vs 
cmake.exe -DCMAKE_CONFIGURATION_TYPES="debug;release;log" -G "Visual Studio 10 Win64" ../.. 
popd

