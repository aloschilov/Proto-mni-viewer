;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

 ;Name and file
 Name "MNI object viewer"
 OutFile "mni_object_viewer_setup.exe"

 ;Default installation folder
 InstallDir $PROGRAMFILES\mni_object_viewer

 ;Registry key to check for directory (so if you install again, it will
 ;overwrite the old one automatically)
 InstallDirRegKey HKLM "Software\mni_object_viewer" "Install_Dir"

 ;Request application privileges for Windows Vista
 RequestExecutionLevel admin

 ; Compressor options
 SetCompressor /FINAL /SOLID lzma

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
; Pages

  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY

  !insertmacro MUI_PAGE_INSTFILES

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "MNI object viewer"

  SectionIn RO

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Put installation files there
  File /r "..\qtcreator-build\ProtoMniViewer.exe"

  File /r "C:\QtSDK\Desktop\Qt\4.8.1\mingw\bin\*.dll"

  File /r "C:\workspace\Gibby\VTK\bin\*.dll"

  File /r "C:\QtSDK\mingw\bin\*.dll"

  File /r "C:\workspace\ffmpeg\bin\*.dll"

  File /r "C:\QtSDK\Desktop\Qt\4.8.1\mingw\plugins\"

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\mni_object_viewer "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mni_object_viewer" "DisplayName" "MNI object viewer"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mni_object_viewer" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mni_object_viewer" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mni_object_viewer" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\mni_object_viewer"
  CreateShortCut "$SMPROGRAMS\mni_object_viewer\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0 "" "" "Uninstall coreport"
  CreateShortCut "$SMPROGRAMS\mni_object_viewer\mni_object_viewer.lnk" "$INSTDIR\ProtoMniViewer.exe" "" "$INSTDIR\ProtoMniViewer.exe" 0 "" "" "Run MNI object viewer"

  SetOutPath $INSTDIR

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mni_object_viewer"
  DeleteRegKey HKLM SOFTWARE\mni_object_viewer

  ; Remove files and uninstaller
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\mni_object_viewer\*.*"

  ; Remove directories used
  RMDir /r "$SMPROGRAMS\mni_object_viewer"
  RMDir /r "$INSTDIR"

SectionEnd

