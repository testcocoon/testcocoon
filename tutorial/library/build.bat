@echo OFF
set PATH=%PATH%;%TESTCOCOON%
call "%VS80COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
cls
@ECHO ON
cscl --cs-count /MT /c main.c  /nologo /Fomain.obj
cscl --cs-count /MT /c staticlib.c  /nologo /Fostaticlib.obj
cscl --cs-count /MT /c dynamiclib.c /DDLL /nologo /Fodynamiclib.obj
cslib staticlib.obj /OUT:staticlib.lib
cslink /DLL dynamiclib.obj /OUT:dynamiclib.dll /IMPLIB:dynamiclib.lib
cslink --cs-libgen=/MT main.obj dynamiclib.lib staticlib.lib /OUT:main.exe
@ECHO OFF
pause
exit
