pushd %HERE%\..\..\src\TestCocoonVs2005Addin
ECHO VS Addin
del /Q /F *.obj
del /Q /F *.exe
del /Q /F release\*.exe
del /Q /F release\*.obj
del /S /Q /F release
del /S /Q /F debug
md release
md debug
%DEVENV2005% testcocoonvs2005addin.sln %BUILDOP% %VS_MODE%
%DEVENV2008% testcocoonvs2008addin.sln %BUILDOP% %VS_MODE%
%DEVENV2010% testcocoonvs2010addin.sln %BUILDOP% %VS_MODE%
popd

