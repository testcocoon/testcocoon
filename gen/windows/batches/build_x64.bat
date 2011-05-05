REM @echo OFF
call "%HERE%\setenv_x64.bat"

call "%HERE%\batches\buildcmake.bat"
call "%HERE%\batches\nsis.bat"

