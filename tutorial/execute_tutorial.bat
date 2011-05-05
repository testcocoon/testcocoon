@echo OFF
cls
IF NOT EXIST HELLO.exe goto buildfirst
ECHO To execute the tutorial type:
ECHO     hello "person 1" ... "person n"
cmd /K

exit
:buildfirst
color d0
echo Error: build first the tutorial
pause
