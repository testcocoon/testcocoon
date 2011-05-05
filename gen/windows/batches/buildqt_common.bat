@echo on
set BUILDTOP=\qt\%BUILD_ARCH%
set QTBUILD=%BUILDTOP%\%QT%
set QTZIP=\qt\%QT%.zip
IF EXIST "%ProgramFiles%"\7-Zip\7z.exe set ZIP="%ProgramFiles%"\7-Zip\7z.exe
IF EXIST "%ProgramW6432%"\7-Zip\7z.exe set ZIP="%ProgramW6432%"\7-Zip\7z.exe
set LOG=%BUILDTOP%\%QT%.log
set NOMAKE=-nomake examples -nomake demos -nomake tutorials 

@echo on
echo "Building %QT%" > %LOG%
del /S /Q %QTBUILD%
cd \qt
md %BUILDTOP%
cd %BUILDTOP%
%ZIP% x %QTZIP%
IF ERRORLEVEL 1 goto enderr
cd %QTBUILD%
echo yes > cmd.txt
type cmd.txt | configure.exe  -opensource %NOMAKE% -script -scripttools -no-qt3support -no-opengl -no-openssl -no-dbus -no-phonon -no-webkit -fast >> %LOG% 2>&1 
IF ERRORLEVEL 1 goto enderr
nmake  >> %LOG% 2>&1
IF ERRORLEVEL 1 goto enderr
nmake clean  >> %LOG% 2>&1
IF ERRORLEVEL 1 goto enderr
:enderr
type %LOG%
:end
