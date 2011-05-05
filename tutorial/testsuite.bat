@echo OFF
cls
IF NOT EXIST HELLO.exe goto buildfirst
del  /Q /F hello.exe.csexe

echo Executing test "Test Suite/no params"...
echo §Test Suite/no params>> hello.exe.csexe
@echo ON
hello.exe
@echo OFF
echo The result of the test is set to CHECK_MANUALLY
echo !CHECK_MANUALLY>> hello.exe.csexe

echo Executing test "Test Suite/1 param"...
echo §Test Suite/1 param>> hello.exe.csexe
@echo ON
hello.exe Joe
@echo OFF
echo The result of the test is set to FAILED
echo !FAILED>> hello.exe.csexe

echo Executing test "Test Suite/2 param"...
echo §Test Suite/2 param>> hello.exe.csexe
@echo ON
hello.exe Joe James
@echo OFF
echo The result of the test is set to PASSED
echo !PASSED>> hello.exe.csexe

echo Executing test "Test Suite/3 param"...
echo §Test Suite/3 param>> hello.exe.csexe
@echo ON
hello.exe Joe James Jim
@echo OFF
echo The result of the test is set to PASSED
echo !PASSED>> hello.exe.csexe
pause
exit
:buildfirst
color d0
echo Error: build first the tutorial
pause
