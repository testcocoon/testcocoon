@echo OFF
goto scriptend
if  "%TESTCOCOON%"=="" goto scriptend
set FOLDER=debug\
md %TESTCOCOON%\borland
md %TESTCOCOON%\visualstudio
copy *.cspro %TESTCOCOON%\
copy *.cspro %TESTCOCOON%\visualstudio
copy *.cspro %TESTCOCOON%\borland
copy %FOLDER%\coveragescanner.exe %TESTCOCOON%\csbcc32.exe
copy %FOLDER%\coveragescanner.exe %TESTCOCOON%\csilink32.exe
copy %FOLDER%\coveragescanner.exe %TESTCOCOON%\cscl.exe
copy %FOLDER%\coveragescanner.exe %TESTCOCOON%\cslink.exe
copy %FOLDER%\coveragescanner.exe %TESTCOCOON%\borland\bcc32.exe
copy %FOLDER%\coveragescanner.exe %TESTCOCOON%\borland\ilink32.exe
copy %FOLDER%\coveragescanner.exe %TESTCOCOON%\visualstudio\cl.exe
copy %FOLDER%\coveragescanner.exe %TESTCOCOON%\visualstudio\link.exe
:scriptend
