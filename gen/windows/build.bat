@echo off
set HERE=%cd%
set DEVENV2005="%VS80COMNTOOLS%\..\IDE\devenv.exe"
set DEVENV2008="%VS90COMNTOOLS%\..\IDE\devenv.exe"
set DEVENV2010="%VS100COMNTOOLS%\..\IDE\devenv.exe"

pushd ..\..
del /Q /S /F build_distrib
md build_distrib
md binaries
popd 

set VS_MODE=Release
REM set VS_MODE=Debug
set BUILDOP=/rebuild

call "%HERE%\batches\vsaddin.bat"
cmd /C "%HERE%\batches\build_x86.bat"
REM cmd /C "%HERE%\batches\build_x64.bat"
exit /B 0
