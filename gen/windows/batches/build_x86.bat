@echo ON
call "%HERE%\setenv_x86.bat"

call "%HERE%\batches\buildcmake.bat"
call "%HERE%\batches\nsis.bat"

