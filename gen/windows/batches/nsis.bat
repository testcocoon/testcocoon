pushd "%HERE%"
set NSIS="%PROGRAMFILES%\NSIS"
set path=%NSIS%;%path%

makensis testcocoon.nsi 
popd
