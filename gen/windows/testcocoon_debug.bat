@echo OFF
del /S /Q "%TESTCOCOON%"\logfiles
if  "%TESTCOCOON%"=="" goto scriptend
IF EXIST "%TESTCOCOON%"\csicl.exe                copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\csicl.exe
IF EXIST "%TESTCOCOON%"\csccblkfn.exe            copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\csccblkfn.exe
IF EXIST "%TESTCOCOON%"\visualstudio\cl.exe      copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\visualstudio\cl.exe
IF EXIST "%TESTCOCOON%"\visualstudio\link.exe    copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\visualstudio\link.exe
IF EXIST "%TESTCOCOON%"\visualstudio\lib.exe    copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\visualstudio\lib.exe
IF EXIST "%TESTCOCOON%"\cscl.exe                 copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\cscl.exe
IF EXIST "%TESTCOCOON%"\cslib.exe                copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\cslib.exe
IF EXIST "%TESTCOCOON%"\borland\bcc32.exe        copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\borland\bcc32.exe
IF EXIST "%TESTCOCOON%"\borland\ilink32.exe    copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\borland\ilink32.exe
IF EXIST "%TESTCOCOON%"\csbcc32.exe              copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\csbcc32.exe
IF EXIST "%TESTCOCOON%"\csilink32.exe            copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\csilink32.exe
IF EXIST "%TESTCOCOON%"\csclarm.exe              copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\csclarm.exe
IF EXIST "%TESTCOCOON%"\csclmips.exe             copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\csclmips.exe
IF EXIST "%TESTCOCOON%"\csclsh.exe               copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\csclsh.exe
IF EXIST "%TESTCOCOON%"\cslink.exe               copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\cslink.exe
IF EXIST "%TESTCOCOON%"\csgcc.exe                copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\csgcc.exe
IF EXIST "%TESTCOCOON%"\"csg++.exe"                copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\"csg++.exe"
IF EXIST "%TESTCOCOON%"\wince\cl.exe             copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\wince\cl.exe
IF EXIST "%TESTCOCOON%"\wince\lib.exe           copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\wince\lib.exe
IF EXIST "%TESTCOCOON%"\wince\link.exe           copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\wince\link.exe
IF EXIST "%TESTCOCOON%"\wince\clsh.exe           copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\wince\clsh.exe
IF EXIST "%TESTCOCOON%"\wince\clarm.exe          copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\wince\clarm.exe
IF EXIST "%TESTCOCOON%"\wince\clmips.exe         copy /Y "%TESTCOCOON%"\coveragescannerdbg.exe "%TESTCOCOON%"\wince\clmips.exe

cls
ECHO Debug mode of TestCocoon activated.
ECHO You can now reproduce your issue.
color 0d
pause

IF EXIST "%TESTCOCOON%"\csicl.exe               copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\csicl.exe
IF EXIST "%TESTCOCOON%"\csccblkfn.exe           copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\csccblkfn.exe
IF EXIST "%TESTCOCOON%"\visualstudio\cl.exe     copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\visualstudio\cl.exe
IF EXIST "%TESTCOCOON%"\visualstudio\link.exe   copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\visualstudio\link.exe
IF EXIST "%TESTCOCOON%"\visualstudio\lib.exe   copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\visualstudio\lib.exe
IF EXIST "%TESTCOCOON%"\cscl.exe                copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\cscl.exe
IF EXIST "%TESTCOCOON%"\cslib.exe               copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\cslib.exe
IF EXIST "%TESTCOCOON%"\csclmips.exe            copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\csclmips.exe
IF EXIST "%TESTCOCOON%"\csclarm.exe             copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\csclarm.exe
IF EXIST "%TESTCOCOON%"\csclsh.exe              copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\csclsh.exe
IF EXIST "%TESTCOCOON%"\cslink.exe              copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\cslink.exe
IF EXIST "%TESTCOCOON%"\csgcc.exe               copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\csgcc.exe
IF EXIST "%TESTCOCOON%"\"csg++.exe"               copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\"csg++.exe"
IF EXIST "%TESTCOCOON%"\wince\cl.exe            copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\wince\cl.exe
IF EXIST "%TESTCOCOON%"\wince\clmips.exe        copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\wince\clmips.exe
IF EXIST "%TESTCOCOON%"\wince\clsh.exe          copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\wince\clsh.exe
IF EXIST "%TESTCOCOON%"\wince\clarm.exe         copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\wince\clarm.exe
IF EXIST "%TESTCOCOON%"\wince\link.exe          copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\wince\link.exe
IF EXIST "%TESTCOCOON%"\wince\lib.exe          copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\wince\lib.exe
IF EXIST "%TESTCOCOON%"\borland\bcc32.exe       copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\borland\bcc32.exe
IF EXIST "%TESTCOCOON%"\borland\ilink32.exe   copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\borland\ilink32.exe
IF EXIST "%TESTCOCOON%"\csbcc32.exe             copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\csbcc32.exe
IF EXIST "%TESTCOCOON%"\csilink32.exe           copy /Y "%TESTCOCOON%"\coveragescanner.exe "%TESTCOCOON%"\csilink32.exe

cls
ECHO Debug mode of TestCocoon disabled.
ECHO The log files are located in "%TESTCOCOON%"\logfiles.
color 07
pause
:scriptend
