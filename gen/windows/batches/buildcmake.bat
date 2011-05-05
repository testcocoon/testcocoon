set NMAKE="nmake.exe"

pushd "%HERE%\..\.."
cd build_distrib

mkdir log_%BUILD_ARCH%
cd log_%BUILD_ARCH%
cmake -G"NMake Makefiles" -DCMAKE_BUILD_TYPE=log ../..
REM %NMAKE% clean
%NMAKE% coveragescanner
cd ..

mkdir %VS_MODE%_%BUILD_ARCH% 
cd %VS_MODE%_%BUILD_ARCH%
cmake -G"NMake Makefiles" -DCMAKE_BUILD_TYPE=%VS_MODE% ../..
REM %NMAKE% clean
%NMAKE%
%NMAKE% pdf
cd ..

popd
