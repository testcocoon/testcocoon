#!/bin/bash
IF NOT EXIST HELLO.exe goto buildfirst
rm  hello.csexe

echo Executing test "Test Suite/no params"...
echo §Test Suite/no params>> hello.csexe
./hello
echo The result of the test is set to CHECK_MANUALLY
echo !CHECK_MANUALLY>> hello.csexe

echo Executing test "Test Suite/1 param"...
echo §Test Suite/1 param>> hello.csexe
./hello Joe
echo The result of the test is set to FAILED
echo !FAILED>> hello.csexe

echo Executing test "Test Suite/2 param"...
echo §Test Suite/2 param>> hello.csexe
./hello Joe James
echo The result of the test is set to PASSED
echo !PASSED>> hello.csexe

echo Executing test "Test Suite/3 param"...
echo §Test Suite/3 param>> hello.csexe
./hello Joe James Jim
echo The result of the test is set to PASSED
echo !PASSED>> hello.csexe
