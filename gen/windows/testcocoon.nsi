!include "MUI.nsh"
!define VS_MODE $%VS_MODE% 
!define BUILD_ARCH $%BUILD_ARCH% 
!define CSHARP 
;!define BORLAND 
!define VS2010 
!define VISUALDSP
!define IDE
!define ALL_USERS
!addplugindir "nsis"
!include "nsis\writeenvStr.nsh"
!include "nsis\EnvVarUpdate.nsh"
!include "sections.nsh"
!include "nsis\TextReplace.nsh"
!include "nsis\locate.nsh"
!include "..\..\src\commoncode\version.nsh"
!if ${BUILD_ARCH} = "x64"
!define PRGFILES $PROGRAMFILES64
!define VSRUNTIME "$%VS100COMNTOOLS%\..\..\VC\redist\x64\Microsoft.VC100.CRT\*.*"
!else
!define PRGFILES $PROGRAMFILES32
!define VSRUNTIME "$%VS80COMNTOOLS%\..\..\VC\redist\x86\Microsoft.VC80.CRT\*.*"
!endif

!define VISUALDSP50DIR "${PRGFILES}\Analog Devices\VisualDSP 5.0\"
!define VISUALDSP45DIR "${PRGFILES}\Analog Devices\VisualDSP 4.5\"
!define QTDIRBIN "$%QTDIR%\bin\"
!define OPENSSL "c:\openssl\out32dll\"
Var STARTMENU_FOLDER
!define VERSION_INFO ""
!define COVERAGESCANNER "..\..\build_distrib\${VS_MODE}_${BUILD_ARCH}\src\coveragescanner\coveragescanner.exe"

InstType "Full"
InstType "Visual Studio 2005"
InstType "Visual Studio 2008"
InstType "Black-Box Testing"

Name "TestCocoon"
BrandingText ""
!if ${VS_MODE} = "Release"
SetCompress force
SetDatablockOptimize on ; 
SetCompressor /SOLID /FINAL lzma
!define PREFIX "_${BUILD_ARCH}"
!else
SetDatablockOptimize on ; 
SetCompress off
!define PREFIX "_${VS_MODE}_${BUILD_ARCH}"
!endif
OutFile "..\..\..\binaries\TestCocoonSetup_${EXE_VERSION}${PREFIX}.exe"
Caption "TestCocoon Setup - Version ${MUI_VERSION}"

CRCCheck on ; 
ShowInstDetails hide ;
SetDateSave on ;
XPStyle on ;

InstallDir "${PRGFILES}\TestCocoon"
InstallDirRegKey HKEY_LOCAL_MACHINE "SOFTWARE\TestCocoon\TestCocoon" ""
DirText "Select the directory to install TestCocoon in:"
!define MUI_ICON "testcocoon.ico"
!define MUI_UNICON  "testcocoon_uninstall.ico"
!define MUI_COMPONENTSPAGE_NODESC 

; License page
!define MUI_PAGE_HEADER_TEXT "TestCocoon - Version ${MUI_VERSION} ${VERSION_INFO}"
!define MUI_PAGE_HEADER_SUBTEXT "Code Coverage Tool for C/C++ and C#"
!define MUI_LICENSEPAGE_RADIOBUTTONS
!define MUI_LICENSEPAGE_TEXT_TOP "TestCocoon - License Agreement"
!define MUI_LICENSEPAGE_TEXT_BOTTOM "You must agree to this license before installing."
!insertmacro MUI_PAGE_LICENSE "..\..\src\LICENSE.txt"

; Compenents
!insertmacro MUI_PAGE_COMPONENTS

; Directory page
!define MUI_PAGE_HEADER_TEXT "TestCocoon - Installation Directory"
!define MUI_PAGE_HEADER_SUBTEXT "Code Coverage Tool for C/C++"
!insertmacro MUI_PAGE_DIRECTORY

;Start Menu Folder Page Configuration
!define MUI_PAGE_HEADER_TEXT "TestCocoon - Start Menu"
!define MUI_PAGE_HEADER_SUBTEXT "Code Coverage Tool for C/C++"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "SOFTWARE\TestCocoon\TestCocoon"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
!define MUI_STARTMENUPAGE_NODISABLE 
!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER

; progress dialog
!define MUI_PAGE_HEADER_TEXT "TestCocoon - Installing..."
!define MUI_PAGE_HEADER_SUBTEXT "Code Coverage Tool for C/C++"
!define MUI_INSTFILESPAGE_FINISHHEADER_TEXT "TestCocoon - Installation Done"
!define MUI_INSTFILESPAGE_FINISHHEADER_SUBTEXT "Code Coverage Tool for C/C++"
!define MUI_INSTFILESPAGE_ABORTHEADER_TEXT "TestCocoon - Installation Aborted"
!define MUI_INSTFILESPAGE_ABORTHEADER_SUBTEXT "Code Coverage Tool for C/C++"
!insertmacro MUI_PAGE_INSTFILES

; confirm uninstall dialog
!define MUI_PAGE_HEADER_TEXT "TestCocoon - Version ${MUI_VERSION}"
!define MUI_PAGE_HEADER_SUBTEXT "Code Coverage Tool for C/C++"
!insertmacro MUI_UNPAGE_CONFIRM

; Uninstall progress
!define MUI_PAGE_HEADER_TEXT "TestCocoon - Uninstalling..."
!define MUI_PAGE_HEADER_SUBTEXT "Code Coverage Tool for C/C++"
!define MUI_UNINSTFILESPAGE_FINISHHEADER_TEXT "TestCocoon - Uninstallation Done"
!define MUI_UNINSTFILESPAGE_FINISHHEADER_SUBTEXT "Code Coverage Tool for C/C++"
!define MUI_UNINSTFILESPAGE_ABORTHEADER_TEXT "TestCocoon - Uninstallation Aborted"
!define MUI_UNINSTFILESPAGE_ABORTHEADER_SUBTEXT "Code Coverage Tool for C/C++"
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

VIProductVersion "${MUI_VERSION}.0.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "TestCocoon"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Code Coverage Tool for C/C++ and C#"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" ""
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" ""
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Installation program of TestCocoon"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${MUI_VERSION}"

Section "" ; (default section)
SectionIn RO
  DetailPrint "Setting System Restore point..."
  SysRestore::StartRestorePoint /NOUNLOAD "Installed $(^Name) ${MUI_VERSION}"
  Pop $0
  StrCmp $0 0 next_create_restore
  DetailPrint "Could not set System Restore start point. Error code: $0"
next_create_restore:
SetOutPath "$INSTDIR"
RMDir /r "$INSTDIR\logfiles"
; add files / whatever that need to be installed here.
RMDir /r "$INSTDIR\styles"
;CreateDirectory "$INSTDIR\styles"
;File "/oname=$INSTDIR\styles\qtdotnet2.dll" "$%QTDIR%\plugins\styles\qtdotnet2.dll"
File ..\..\build_distrib\${VS_MODE}_${BUILD_ARCH}\src\cmmerge\cmmerge.exe
File ..\..\build_distrib\${VS_MODE}_${BUILD_ARCH}\src\cmreport\cmreport.exe
File ..\..\build_distrib\${VS_MODE}_${BUILD_ARCH}\src\cmcsexeimport\cmcsexeimport.exe
File /r "${VSRUNTIME}"
CreateDirectory "$INSTDIR\imageformats"
CreateDirectory "$INSTDIR\iconengines"
!if ${VS_MODE} = "Release"
File "${QTDIRBIN}\QtGui4.dll"
File "${QTDIRBIN}\QtCore4.dll"
File "${QTDIRBIN}\QtSvg4.dll"
File "${QTDIRBIN}\QtXml4.dll"
File "${QTDIRBIN}\QtNetwork4.dll"
File "/oname=$INSTDIR\imageformats\qsvg4.dll" "${QTDIRBIN}\..\plugins\imageformats\qsvg4.dll"
File  "/oname=$INSTDIR\iconengines\qsvgicon4.dll" "${QTDIRBIN}\..\plugins\iconengines\qsvgicon4.dll"
!else
File "${QTDIRBIN}\QtGuid4.dll"
File "${QTDIRBIN}\QtSvgd4.dll"
File "${QTDIRBIN}\QtXmld4.dll"
File "${QTDIRBIN}\QtCored4.dll"
File "${QTDIRBIN}\QtNetworkd4.dll"
File "/oname=$INSTDIR\imageformats\qsvgd4.dll" "${QTDIRBIN}\..\plugins\imageformats\qsvgd4.dll"
File  "/oname=$INSTDIR\iconengines\qsvgicond4.dll" "${QTDIRBIN}\..\plugins\iconengines\qsvgicond4.dll"
!endif

File "console.ico"
File "testcocoon.ico"
File "testcocoon_uninstall.ico"
File "testcocoon_homepage.url"




; setting environment variable
DetailPrint "Setting environment variable %TESTCOCOON% to $INSTDIR..."
Push TESTCOCOON
Push $INSTDIR
Call WriteEnvStr

DetailPrint "Backup %PATH% variable to %PATH_TESTCOCOON_INSTALLER%"
ReadEnvStr $0 PATH
Push PATH_TESTCOCOON_INSTALLER
Push $0
Call WriteEnvStr

DetailPrint "Appending $INSTDIR to %PATH% variable..."
${EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR"
${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR"

; start menu
SetShellVarContext all
!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
DetailPrint "Creating shortcuts ($SMPROGRAMS\$STARTMENU_FOLDER)..."
CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\TestCocoon Homepage.lnk" "$INSTDIR\testcocoon_homepage.url"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\uninst.exe"

!insertmacro MUI_STARTMENU_WRITE_END


WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\TestCocoon\TestCocoon" "" "$INSTDIR"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestCocoon" "URLInfoAbout" "http://www.testcocoon.org"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestCocoon" "URLUpdateInfo" "http://www.testcocoon.org/download.html"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestCocoon" "DisplayVersion" "${MUI_VERSION}"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestCocoon" "DisplayName" "TestCocoon"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestCocoon" "UninstallString" '"$INSTDIR\uninst.exe"'
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestCocoon" "DisplayIcon" '"$INSTDIR\testcocoon_uninstall.ico"'
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestCocoon" "Publisher" "TestCocoon"

; write out uninstaller
WriteUninstaller "$INSTDIR\uninst.exe"
SectionEnd ; end of default section

Section "CoverageBrowser"
SectionIn 1 2 3 4

File ..\..\build_distrib\${VS_MODE}_${BUILD_ARCH}\src\coveragebrowser\coveragebrowser.exe
; coveragebrowser api

File ..\..\build_distrib\${VS_MODE}_${BUILD_ARCH}\src\libcoveragebrowser\French.qm

; File Association
DetailPrint "File association *.csmes->CoverageBrowser ..."
 ; back up old value of .csmes
!define Index "Line${__LINE__}"
  ReadRegStr $1 HKCR ".csmes" ""
  StrCmp $1 "" "${Index}-NoBackup"
    StrCmp $1 "TestCocoon.CSMes" "${Index}-NoBackup"
    WriteRegStr HKCR ".csmes" "backup_val" $1
"${Index}-NoBackup:"
  WriteRegStr HKCR ".csmes" "" "TestCocoon.CSMes"
  ReadRegStr $0 HKCR "TestCocoon.CSMes" ""
  StrCmp $0 "" 0 "${Index}-Skip"
        WriteRegStr HKCR "TestCocoon.CSMes" "" "TestCocoon Instrumentation"
        WriteRegStr HKCR "TestCocoon.CSMes\shell" "" "open"
        WriteRegStr HKCR "TestCocoon.CSMes\DefaultIcon" "" "$INSTDIR\execute.exe,0"
"${Index}-Skip:"
  WriteRegStr HKCR "TestCocoon.CSMes\shell\open\command" "" \
    '$INSTDIR\CoverageBrowser.exe --csmes="%1"'
 
  System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'
!undef Index
  ; Rest of script

CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\CoverageBrowser.lnk" "$INSTDIR\CoverageBrowser.exe"
SectionEnd


Section "CoverageScanner" scanner
SectionIn 1 2 3
File ${COVERAGESCANNER}
File "/oname=coveragescannerdbg.exe" ..\..\build_distrib\log_${BUILD_ARCH}\src\coveragescanner\coveragescanner.exe
File testcocoon_debug.bat

File ..\..\build_distrib\${VS_MODE}_${BUILD_ARCH}\src\toolselector\toolselector.exe
File testcocoonconsole.bat 
File testcocoonenv.bat 
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Build Environment Selection.lnk" "$INSTDIR\toolselector.exe"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Build Environment.lnk" "$INSTDIR\testcocoonconsole.bat" "" "$INSTDIR\console.ico"
SectionEnd


SectionGroup "Compiler Support"
Section "GNU GCC" gnugcc
SectionIn 1 2 3
File ..\..\src\coveragescanner\gcc.cspro
File ..\..\src\coveragescanner\g++.cspro
File ..\..\src\coveragescanner\ar.cspro
File "/oname=csgcc.exe" ${COVERAGESCANNER}
File "/oname=csg++.exe" ${COVERAGESCANNER}
File "/oname=csar.exe" ${COVERAGESCANNER}
SectionEnd 
!ifdef CSHARP
Section "Microsoft Visual Studio C# Compiler" visualstudiocsharp
SectionIn 1 2 3
SetOutPath "$INSTDIR"
File ..\..\src\coveragescanner\csc.cspro
File ..\..\src\coveragescanner\mcs.cspro
File "/oname=cscsc.exe" ${COVERAGESCANNER}
File "/oname=csmcs.exe" ${COVERAGESCANNER}
	#Locate files in Windows directory beginning with "csc",
	# with extensions "exe" 

	${locate::Open} "$WINDIR\Microsoft.NET\Framework" "/F=1 \
					/D=0 \
					/M=csc.exe \
					/X=exe \
					/B=1" $0
	StrCmp $0 -1 0 loop_csharp_install
	MessageBox MB_OK "Error" IDOK close_csharp_install

	loop_csharp_install:
	${locate::Find} $0 $1 $2 $3 $4 $5 $6
   StrCmp "$1" "" close_csharp_install

	;DetailPrint '"path\name"   =[$1]'
	;DetailPrint '"path" =[$2]'
	;DetailPrint '"name" =[$3]'
	;DetailPrint '"size" =[$4]'
	;DetailPrint '"time" =[$5]'
	;DetailPrint '"attributes =[$6]'
	DetailPrint 'Patching $2\Microsoft.CSharp.targets...'
  Rename "$2\Microsoft.CSharp.targets" "$2\_Microsoft.CSharp.targets" 
	${textreplace::ReplaceInFile} "$2\_Microsoft.CSharp.targets" "$2\Microsoft.CSharp.targets" "true</UseHostCompilerIfAvailable>" "false</UseHostCompilerIfAvailable>" "/S=1 /C=1 /AO=1" $9
	${textreplace::Unload}

   Rename "$1" "$2\_csc.exe"
   File "/oname=$1" ${COVERAGESCANNER}
   File "/oname=$2\csc.cspro" ..\..\src\coveragescanner\csc.cspro
   goto loop_csharp_install
	close_csharp_install:
	${locate::Close} $0
	${locate::Unload}
SectionEnd 
!endif
Section "Microsoft Visual Studio C/C++ Compiler" visualstudio
SectionIn 1 2 3
File ..\..\src\coveragescanner\link.cspro
File ..\..\src\coveragescanner\lib.cspro
File ..\..\src\coveragescanner\icl.cspro
File ..\..\src\coveragescanner\cl.cspro
CreateDirectory "$INSTDIR\visualstudio"
SetOutPath "$INSTDIR\visualstudio"
File /r "${VSRUNTIME}"
SetOutPath "$INSTDIR"
File "/oname=$INSTDIR\visualstudio\cl.exe" ${COVERAGESCANNER}
File "/oname=$INSTDIR\visualstudio\link.exe" ${COVERAGESCANNER}
File "/oname=$INSTDIR\visualstudio\lib.exe" ${COVERAGESCANNER}
File "/oname=$INSTDIR\visualstudio\link.cspro" ..\..\src\coveragescanner\link.cspro
File "/oname=$INSTDIR\visualstudio\lib.cspro" ..\..\src\coveragescanner\lib.cspro
File "/oname=$INSTDIR\visualstudio\cl.cspro" ..\..\src\coveragescanner\cl.cspro
File "/oname=cscl.exe" ${COVERAGESCANNER}
File "/oname=csicl.exe" ${COVERAGESCANNER}
File "/oname=cslink.exe" ${COVERAGESCANNER}
File "/oname=cslib.exe" ${COVERAGESCANNER}
SectionEnd 
Section "Microsoft eMbedded C++" msembedded
SectionIn 1 2 3
File ..\..\src\coveragescanner\cl.cspro
File ..\..\src\coveragescanner\clsh.cspro
File ..\..\src\coveragescanner\clarm.cspro
File ..\..\src\coveragescanner\clthumb.cspro
File ..\..\src\coveragescanner\clmips.cspro
File ..\..\src\coveragescanner\link.cspro
File ..\..\src\coveragescanner\lib.cspro
CreateDirectory "$INSTDIR\wince"
SetOutPath "$INSTDIR\wince"
File /r "${VSRUNTIME}"
SetOutPath "$INSTDIR"
File "/oname=$INSTDIR\wince\cl.exe" ${COVERAGESCANNER}
File "/oname=$INSTDIR\wince\clmips.exe" ${COVERAGESCANNER}
File "/oname=$INSTDIR\wince\clarm.exe" ${COVERAGESCANNER}
File "/oname=$INSTDIR\wince\clthumb.exe" ${COVERAGESCANNER}
File "/oname=$INSTDIR\wince\clsh.exe" ${COVERAGESCANNER}
File "/oname=$INSTDIR\wince\link.exe" ${COVERAGESCANNER}
File "/oname=$INSTDIR\wince\lib.exe" ${COVERAGESCANNER}
File "/oname=$INSTDIR\wince\link.cspro" ..\..\src\coveragescanner\link.cspro
File "/oname=$INSTDIR\wince\lib.cspro" ..\..\src\coveragescanner\lib.cspro
File "/oname=$INSTDIR\wince\cl.cspro" ..\..\src\coveragescanner\cl.embedded.cspro
File "/oname=$INSTDIR\wince\clmips.cspro" ..\..\src\coveragescanner\clmips.cspro
File "/oname=$INSTDIR\wince\clarm.cspro" ..\..\src\coveragescanner\clarm.cspro
File "/oname=$INSTDIR\wince\clthumb.cspro" ..\..\src\coveragescanner\clthumb.cspro
File "/oname=$INSTDIR\wince\clsh.cspro" ..\..\src\coveragescanner\clsh.cspro
File "/oname=cscl.exe" ${COVERAGESCANNER}
File "/oname=csclmips.exe" ${COVERAGESCANNER}
File "/oname=csclarm.exe" ${COVERAGESCANNER}
File "/oname=csclthumb.exe" ${COVERAGESCANNER}
File "/oname=csclsh.exe" ${COVERAGESCANNER}
File "/oname=cslink.exe" ${COVERAGESCANNER}
File "/oname=cslib.exe" ${COVERAGESCANNER}
SectionEnd 
!ifdef BORLAND
Section "Borland C++ 5.5 Compiler (experimental)"
SectionIn 1 2 3
File "/oname=csilink32.exe" ${COVERAGESCANNER}
File "/oname=csbcc32.exe" ${COVERAGESCANNER}
File ..\..\src\coveragescanner\bcc32.cspro
File ..\..\src\coveragescanner\ilink32.cspro
CreateDirectory "$INSTDIR\borland"
SetOutPath "$INSTDIR\borland"
File /r "${VSRUNTIME}"
SetOutPath "$INSTDIR"
File "/oname=$INSTDIR\borland\bcc32.exe" ${COVERAGESCANNER}
File "/oname=$INSTDIR\borland\ilink32.exe" ${COVERAGESCANNER}
File "/oname=$INSTDIR\borland\bcc32.cspro" ..\..\src\coveragescanner\bcc32.cspro
File "/oname=$INSTDIR\borland\ilink32.cspro" ..\..\src\coveragescanner\ilink32.cspro
SectionEnd
!endif
!ifdef VISUALDSP
Section "VisualDSP++ 5.0" visualdsp50sec
Rename "${VISUALDSP50DIR}/ccts.exe" "${VISUALDSP50DIR}/_ccts.exe"
File "/oname=${VISUALDSP50DIR}/ccts.exe" ${COVERAGESCANNER}
File "/oname=${VISUALDSP50DIR}/ccts.cspro" ..\..\src\coveragescanner\ccts.cspro
Rename "${VISUALDSP50DIR}/ccblkfn.exe" "${VISUALDSP50DIR}/_ccblkfn.exe"
File "/oname=${VISUALDSP50DIR}/ccblkfn.exe" ${COVERAGESCANNER}
File "/oname=${VISUALDSP50DIR}/ccblkfn.cspro" ..\..\src\coveragescanner\ccblkfn.cspro
Rename "${VISUALDSP50DIR}/cc21k.exe" "${VISUALDSP50DIR}/_cc21k.exe"
File "/oname=${VISUALDSP50DIR}/cc21k.exe" ${COVERAGESCANNER}
File "/oname=${VISUALDSP50DIR}/cc21k.cspro" ..\..\src\coveragescanner\cc21k.cspro
SectionEnd
Section "VisualDSP++ 4.5" visualdsp45sec
Rename "${VISUALDSP45DIR}/ccts.exe" "${VISUALDSP45DIR}/_ccts.exe"
File "/oname=${VISUALDSP45DIR}/ccts.exe" ${COVERAGESCANNER}
File "/oname=${VISUALDSP45DIR}/ccts.cspro" ..\..\src\coveragescanner\ccts.cspro
Rename "${VISUALDSP45DIR}/ccblkfn.exe" "${VISUALDSP45DIR}/_ccblkfn.exe"
File "/oname=${VISUALDSP45DIR}/ccblkfn.exe" ${COVERAGESCANNER}
File "/oname=${VISUALDSP45DIR}/ccblkfn.cspro" ..\..\src\coveragescanner\ccblkfn.cspro
Rename "${VISUALDSP45DIR}/cc21k.exe" "${VISUALDSP45DIR}/_cc21k.exe"
File "/oname=${VISUALDSP45DIR}/cc21k.exe" ${COVERAGESCANNER}
File "/oname=${VISUALDSP45DIR}/cc21k.cspro" ..\..\src\coveragescanner\cc21k.cspro
SectionEnd
!endif
SectionGroupEnd

!ifdef IDE
SectionGroup "IDE Extensions"
Section /o "Microsoft Visual Studio 2005 Add-In" vs2005addin
SectionIn 1 2
SetShellVarContext current
CreateDirectory "$DOCUMENTS\Visual Studio 2005\AddIns"
File "/oname=$DOCUMENTS\Visual Studio 2005\Addins\TestCocoonVs2005AddIn.addin" ..\..\src\TestCocoonVs2005AddIn\TestCocoonVs2005AddIn\TestCocoonVs2005AddIn.addin 
File "/oname=$DOCUMENTS\Visual Studio 2005\Addins\TestCocoonVs2005AddIn.dll" ..\..\src\TestCocoonVs2005AddIn\TestCocoonVs2005AddIn\bin_2005\TestCocoonVs2005AddIn.dll 
SetShellVarContext all
ReadEnvStr $R0 "VS80COMNTOOLS"
DetailPrint "Executing devenv.exe /setup..."
nsExec::ExecToLog /OEM '"$R0\..\IDE\devenv.exe" /setup'
SectionEnd 

Section "Microsoft Visual Studio 2008 Add-In" vs2008addin
SectionIn 1 3
SetShellVarContext current
CreateDirectory "$DOCUMENTS\Visual Studio 2008\AddIns"
File /nonfatal "/oname=$DOCUMENTS\Visual Studio 2008\Addins\TestCocoonVs2008AddIn.addin" ..\..\src\TestCocoonVs2005AddIn\TestCocoonVs2005AddIn\TestCocoonVs2008AddIn.addin 
File /nonfatal "/oname=$DOCUMENTS\Visual Studio 2008\Addins\TestCocoonVs2008AddIn.dll" ..\..\src\TestCocoonVs2005AddIn\TestCocoonVs2005AddIn\bin_2008\TestCocoonVs2008AddIn.dll 
SetShellVarContext all
ReadEnvStr $R0 "VS90COMNTOOLS"
DetailPrint "Executing devenv.exe /setup..."
nsExec::ExecToLog /OEM '"$R0\..\IDE\devenv.exe" /setup'
SectionEnd 

!ifdef VS2010
Section "Microsoft Visual Studio 2010 Add-In" vs2010addin
SectionIn 1 3
SetShellVarContext current
CreateDirectory "$DOCUMENTS\Visual Studio 2010\AddIns"
File /nonfatal "/oname=$DOCUMENTS\Visual Studio 2010\Addins\TestCocoonVs2010AddIn.addin" ..\..\src\TestCocoonVs2005AddIn\TestCocoonVs2005AddIn\TestCocoonVs2010AddIn.addin 
File /nonfatal "/oname=$DOCUMENTS\Visual Studio 2010\Addins\TestCocoonVs2010AddIn.dll" ..\..\src\TestCocoonVs2005AddIn\TestCocoonVs2005AddIn\bin_2010\TestCocoonVs2010AddIn.dll 
SetShellVarContext all
ReadEnvStr $R0 "VS100COMNTOOLS"
DetailPrint "Executing devenv.exe /setup..."
nsExec::ExecToLog /OEM '"$R0\..\IDE\devenv.exe" /setup'
SectionEnd 
!endif

SectionGroupEnd
!endif

Section "Documentation"
SectionIn 1 2 3 4
File /oname=testcocoon.pdf ..\..\build_distrib\${VS_MODE}_${BUILD_ARCH}\doc\testcocoon.pdf
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Documentation.lnk" "$INSTDIR\testcocoon.pdf"
SectionEnd 

SectionGroup "Samples"
Section "Tutorial" tutorial
SectionIn 1 2 3
; tutorial
CreateDirectory "$INSTDIR\tutorial"
File /oname=tutorial\build.bat ..\..\tutorial\build.bat 
File /oname=tutorial\tutorial.cpp ..\..\tutorial\tutorial.cpp 
File /oname=tutorial\tutorial.vcproj ..\..\tutorial\tutorial.vcproj 
File /oname=tutorial\execute_tutorial.bat ..\..\tutorial\execute_tutorial.bat
File /oname=tutorial\testsuite.bat ..\..\tutorial\testsuite.bat
File /oname=tutorial\visual_option_net.png ..\..\tutorial\visual_option_net.png
File /oname=tutorial\ReadMe_VS.htm ..\..\tutorial\ReadMe_VS.htm
File testcocoon_tutorial.url

CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER\Example"
CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER\Example\Tutorial"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Example\Online Demonstration.lnk" "$INSTDIR\testcocoon_tutorial.url"
SetOutPath "$INSTDIR\Tutorial"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Example\Tutorial\Step 1: Build With Command Line Compiler.lnk" "$INSTDIR\Tutorial\build.bat"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Example\Tutorial\Step 1: Build Width Visual Studio .NET.lnk" "$INSTDIR\Tutorial\tutorial.vcproj"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Example\Tutorial\Step 2: Execute Application.lnk" "$INSTDIR\Tutorial\execute_tutorial.bat"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Example\Tutorial\Step 2: Execute Test Suite.lnk" "$INSTDIR\Tutorial\testsuite.bat"
SetOutPath "$INSTDIR"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Example\Tutorial\Step 3: Code Coverage Analysis.lnk" "$INSTDIR\CoverageBrowser.exe" "--csexe=$\"$INSTDIR\Tutorial\hello.exe.csexe$\"  --csmes=$\"$INSTDIR\Tutorial\hello.exe.csmes$\""

SectionEnd 

Section "CppUnit" cppunit
SectionIn 1 2 3

File /r /x CVS /x *.obj /x *.dll /x *.exe /x *.lib /x *.csmes /x *.csexe /x dllplugintester /x lib /x msvc6 /x qttestrunner /x ClockerPLugIn /x contrib /x doc /x cppunittest /x DumperPlugIn /x hierarchy /x msvc6 /x qt /x simple "..\..\cppunit-1.12.0"
CreateDirectory "$INSTDIR\cppunit-1.12.0\lib"

File "readme_cppunit.html"

CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER\Example"
CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER\Example\CppUnit"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Example\CppUnit\ReadMe.lnk" "$INSTDIR\readme_cppunit.html"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Example\CppUnit\Step 1: Build Money (VS6.0).lnk" "$INSTDIR\cppunit-1.12.0\examples\examples.dsw"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Example\CppUnit\Step 1: Build Money (VS 2005).lnk" "$INSTDIR\cppunit-1.12.0\examples\examples.sln"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Example\CppUnit\Step 2: CoverageBrowser.lnk" "$INSTDIR\CoverageBrowser.exe" "--csexe=$\"$INSTDIR\cppunit-1.12.0\examples\money\CodeCoverage\moneyd.exe.csexe$\"  --csmes=$\"$INSTDIR\cppunit-1.12.0\examples\money\CodeCoverage\moneyd.exe.csmes$\""
SetOutPath "$INSTDIR"
SectionEnd 
SectionGroupEnd


 
!ifdef IDE
Function .onSelChange
  Push $0

!ifdef VS2010
  !insertmacro SectionFlagIsSet ${vs2010addin} ${SF_SELECTED} 0 nextsection0
  !insertmacro SelectSection ${visualstudio}
nextsection0:
!endif
 
  !insertmacro SectionFlagIsSet ${vs2008addin} ${SF_SELECTED} 0 nextsection1
  !insertmacro SelectSection ${visualstudio}
 
nextsection1:
  !insertmacro SectionFlagIsSet ${vs2005addin} ${SF_SELECTED} 0 nextsection2
  !insertmacro SelectSection ${visualstudio}

nextsection2:
  !insertmacro SectionFlagIsSet ${visualstudio} ${SF_SELECTED} 0 nextsection3 
  !insertmacro SelectSection ${scanner}
!ifdef CSHARP
  !insertmacro SelectSection ${visualstudiocsharp}
!endif
  goto nextsectiondone
nextsection3:
  !insertmacro SectionFlagIsSet ${msembedded} ${SF_SELECTED} 0 nextsection4 
  !insertmacro SelectSection ${scanner}
  goto nextsectiondone
nextsection4:
  !insertmacro SectionFlagIsSet ${gnugcc} ${SF_SELECTED} 0 nextsection5 
  !insertmacro SelectSection ${scanner}
  goto nextsectiondone
nextsection5:
  !insertmacro SectionFlagIsSet ${scanner} ${SF_SELECTED} 0 nextsection6 
  goto nextsectiondone

nextsection6:
  !insertmacro UnselectSection ${cppunit}
  !insertmacro UnselectSection ${tutorial}
 
nextsectiondone:
  Pop $0
FunctionEnd

Function un.onInit

FindProcDLL::FindProc "devenv.exe"
StrCmp "$R0" "1" 0 +3
MessageBox MB_OK "Please close Visual Studio before uninstalling TestCocoon" 0
Quit

FunctionEnd

Function .onInit
	ClearErrors
	UserInfo::GetName
	IfErrors Win9x
	Pop $0
	UserInfo::GetAccountType
	Pop $1
	# GetOriginalAccountType will check the tokens of the original user of the
	# current thread/process. If the user tokens were elevated or limited for
	# this process, GetOriginalAccountType will return the non-restricted
	# account type.
	# On Vista with UAC, for example, this is not the same value when running
	# with `RequestExecutionLevel user`. GetOriginalAccountType will return
	# "admin" while GetAccountType will return "user".
	UserInfo::GetOriginalAccountType
	Pop $2
#StrCmp $1 "Admin" 0 +3
#MessageBox MB_OK 'User "$0" is does not have administrator rights'
#Goto done
	StrCmp $1 "Power" 0 +3
		MessageBox MB_OK 'User "$0" is does not have administrator rights'
		Quit
	StrCmp $1 "User" 0 +3
		MessageBox MB_OK 'User "$0" is does not have administrator rights'
		Quit
	StrCmp $1 "Guest" 0 +3
		MessageBox MB_OK 'User "$0" is does not have administrator rights'
		Quit

	Win9x:
		# This one means you don't need to care about admin or
		# not admin because Windows 9x doesn't either

FindProcDLL::FindProc "devenv.exe"
StrCmp "$R0" "1" 0 +3
MessageBox MB_OK "Please close Visual Studio before installing TestCocoon" 0
Quit

; Visual Studio 2005 Detection
ReadEnvStr $R0 "VS80COMNTOOLS"
IfFileExists "$R0\..\IDE\devenv.exe" vs2005exist vs2005notexist
vs2005exist:
!insertmacro SelectSection ${vs2005addin}
goto next1
vs2005notexist:
!insertmacro UnselectSection ${vs2005addin}

next1:

; Visual Studio 2008 Detection
ReadEnvStr $R0 "VS90COMNTOOLS"
IfFileExists "$R0\..\IDE\devenv.exe" vs2008exist vs2008notexist
vs2008exist:
!insertmacro SelectSection ${vs2008addin}
goto next2
vs2008notexist:
!insertmacro UnselectSection ${vs2008addin}

next2:
!ifdef VISUALDSP
; VisualDSP++ Detection
IfFileExists "${VISUALDSP50DIR}\ccblkfn.exe" visualDSP50exist visualDSP50notexist
visualDSP50exist:
!insertmacro SelectSection ${visualdsp50sec}
goto next3
visualDSP50notexist:
!insertmacro UnselectSection ${visualdsp50sec}
!insertmacro SetSectionFlag ${visualdsp50sec} SF_RO
!endif

next3:

!ifdef VISUALDSP
; VisualDSP++ Detection
IfFileExists "${VISUALDSP45DIR}\ccblkfn.exe" visualDSP45exist visualDSP45notexist
visualDSP45exist:
!insertmacro SelectSection ${visualdsp45sec}
goto next4
visualDSP45notexist:
!insertmacro UnselectSection ${visualdsp45sec}
!insertmacro SetSectionFlag ${visualdsp45sec} SF_RO
!endif

next4:

!ifdef VS2010
; Visual Studio 2010 Detection
ReadEnvStr $R0 "VS100COMNTOOLS"
IfFileExists "$R0\..\IDE\devenv.exe" vs2010exist vs2010notexist
vs2010exist:
!insertmacro SelectSection ${vs2010addin}
goto next5
vs2010notexist:
!insertmacro UnselectSection ${vs2010addin}
!endif

next5:

FunctionEnd
!endif

Section Uninstall
  DetailPrint "Setting System Restore point..."
  SysRestore::StartUnRestorePoint /NOUNLOAD "Uninstalled $(^Name) ${MUI_VERSION}"
  Pop $0
  StrCmp $0 0 next_create_restore_un
  DetailPrint "Could not set System Restore start point. Error code: $0"
next_create_restore_un:
; add delete commands to delete whatever files/registry keys/etc you installed here.
SetShellVarContext all
Delete "$INSTDIR\uninst.exe"
RMDir /r "$INSTDIR"


!insertmacro MUI_STARTMENU_GETFOLDER Application $STARTMENU_FOLDER
DetailPrint "Removing shortcuts ($SMPROGRAMS\$STARTMENU_FOLDER)..."
RMDir /r "$SMPROGRAMS\$STARTMENU_FOLDER"

!ifdef VISUALDSP
IfFileExists "${VISUALDSP45DIR}\_cc21k.exe" 0 +4
  Delete "${VISUALDSP45DIR}\cc21k.exe"
  Delete "${VISUALDSP45DIR}\cc21k.cspro"
  Rename "${VISUALDSP45DIR}\_cc21k.exe" "${VISUALDSP45DIR}\cc21k.exe"
IfFileExists "${VISUALDSP50DIR}\_cc21k.exe" 0 +4
  Delete "${VISUALDSP50DIR}\cc21k.exe"
  Delete "${VISUALDSP50DIR}\cc21k.cspro"
  Rename "${VISUALDSP50DIR}\_cc21k.exe" "${VISUALDSP50DIR}\cc21k.exe"
IfFileExists "${VISUALDSP45DIR}\_ccts.exe" 0 +4
  Delete "${VISUALDSP45DIR}\ccts.exe"
  Delete "${VISUALDSP45DIR}\ccts.cspro"
  Rename "${VISUALDSP45DIR}\_ccts.exe" "${VISUALDSP45DIR}\ccts.exe"
IfFileExists "${VISUALDSP50DIR}\_ccts.exe" 0 +4
  Delete "${VISUALDSP50DIR}\ccts.exe"
  Delete "${VISUALDSP50DIR}\ccts.cspro"
  Rename "${VISUALDSP50DIR}\_ccts.exe" "${VISUALDSP50DIR}\ccts.exe"
IfFileExists "${VISUALDSP45DIR}\_ccblkfn.exe" 0 +4
  Delete "${VISUALDSP45DIR}\ccblkfn.exe"
  Delete "${VISUALDSP45DIR}\ccblkfn.cspro"
  Rename "${VISUALDSP45DIR}\_ccblkfn.exe" "${VISUALDSP45DIR}\ccblkfn.exe"
IfFileExists "${VISUALDSP50DIR}\_ccblkfn.exe" 0 +4
  Delete "${VISUALDSP50DIR}\ccblkfn.exe"
  Delete "${VISUALDSP50DIR}\ccblkfn.cspro"
  Rename "${VISUALDSP50DIR}\_ccblkfn.exe" "${VISUALDSP50DIR}\ccblkfn.exe"
!endif

!ifdef CSHARP
	#Locate files in Windows directory beginning with "csc",
	# with extensions "exe" 

	${locate::Open} "$WINDIR\Microsoft.NET\Framework" "/F=1 \
					/D=0 \
					/M=_csc.exe \
					/X=exe \
					/B=1" $0
	StrCmp $0 -1 0 loop_csharp_uninstall
	MessageBox MB_OK "Error" IDOK close_csharp_uninstall

	loop_csharp_uninstall:
	${locate::Find} $0 $1 $2 $3 $4 $5 $6
   StrCmp "$1" "" close_csharp_uninstall

	;DetailPrint '"path\name"   =[$1]'
	;DetailPrint '"path" =[$2]'
	;DetailPrint '"name" =[$3]'
	;DetailPrint '"size" =[$4]'
	;DetailPrint '"time" =[$5]'
	;DetailPrint '"attributes =[$6]'
   IfFileExists "$2\csc.exe" 0 +4
     Delete "$2\csc.exe"
     Delete "$2\csc.cspro"
     Rename "$1" "$2\csc.exe"
   IfFileExists "$2\_Microsoft.CSharp.targets" 0 +3
     Delete "$2\Microsoft.CSharp.targets"
     Rename "$2\_Microsoft.CSharp.targets" "$2\Microsoft.CSharp.targets"
   goto loop_csharp_uninstall
	close_csharp_uninstall:
	${locate::Close} $0
	${locate::Unload}
!endif

SetShellVarContext current
Delete "$DOCUMENTS\Visual Studio 2005\Addins\TestCocoonVs2005AddIn.addin"
Delete "$DOCUMENTS\Visual Studio 2005\Addins\TestCocoonVs2005AddIn.dll" 
RMDir "$DOCUMENTS\Visual Studio 2005\AddIns"
RMDir "$DOCUMENTS\Visual Studio 2005"
Delete "$DOCUMENTS\Visual Studio 2008\Addins\TestCocoonVs2008AddIn.addin"
Delete "$DOCUMENTS\Visual Studio 2008\Addins\TestCocoonVs2008AddIn.dll" 
RMDir "$DOCUMENTS\Visual Studio 2008\AddIns"
RMDir "$DOCUMENTS\Visual Studio 2008"
Delete "$DOCUMENTS\Visual Studio 2010\Addins\TestCocoonVs2010AddIn.addin"
Delete "$DOCUMENTS\Visual Studio 2010\Addins\TestCocoonVs2010AddIn.dll" 
RMDir "$DOCUMENTS\Visual Studio 2010\AddIns"
RMDir "$DOCUMENTS\Visual Studio 2010"
SetShellVarContext all

;start of restore script
; File Association
DetailPrint "File association *.csmes->CoverageBrowser ..."
!define Index "Line${__LINE__}"
  ReadRegStr $1 HKCR ".csmes" ""
  StrCmp $1 "TestCocoon.CSMes" 0 "${Index}-NoOwn" ; only do this if we own it
    ReadRegStr $1 HKCR ".csmes" "backup_val"
    StrCmp $1 "" 0 "${Index}-Restore" ; if backup="" then delete the whole key
      DeleteRegKey HKCR ".csmes"
    Goto "${Index}-NoOwn"
"${Index}-Restore:"
      WriteRegStr HKCR ".csmes" "" $1
      DeleteRegValue HKCR ".csmes" "backup_val"
   
    DeleteRegKey HKCR "TestCocoon.CSMes" ;Delete key with association settings
 
    System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'
"${Index}-NoOwn:"
!undef Index
  ;rest of script

; environment variables
DetailPrint "Removing environment variable %TESTCOCOON%..."
Push TESTCOCOON
Call un.DeleteEnvStr

DetailPrint "Removing TestCocoon from %PATH% variable..."
${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR"

DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\TestCocoon\TestCocoon"
DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\TestCocoon"

SectionEnd ; end of uninstall section
; eof

