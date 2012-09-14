;=========================================================
;
; Atheros Powerline Toolkit NSIS Installer
;
; Contributor(s):
;	Nathaniel Houghton <nathaniel.houghton@atheros.com>
;
;---------------------------------------------------------

!include "EnvVarUpdate.nsh"

;=========================================================
; Include MUI2 and set options
;---------------------------------------------------------

!include "MUI2.nsh"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\CHANGELOG.txt"
!define MUI_FINISHPAGE_SHOWREADME_TEXT "View ChangeLog"
!define MUI_FINISHPAGE_LINK "Visit Atheros Website"
!define MUI_FINISHPAGE_LINK_LOCATION "http://www.atheros.com/"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "atheros.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "atheros_side.bmp"
!define MUI_ICON "amp.ico"
!define MUI_UNICON "amp.ico"
!define MUI_ABORTWARNING

;=========================================================
; Misc. Variables
;---------------------------------------------------------
Var StartMenuFolder

;=========================================================
; Definitions
;---------------------------------------------------------
!define PackageName "Atheros Powerline Toolkit"
!define PackageVersion "0.1.6"
!define PackageType "-current"

;=========================================================
; General Configuration
;---------------------------------------------------------

; Name and output file
Name "${PackageName}"
OutFile "amp-utils.exe"
SetCompressor /SOLID lzma

; Default installation folder
InstallDir "$PROGRAMFILES\Atheros\${PackageName}"

; Get installation folder from registry if available
InstallDirRegKey HKLM "Software\${PackageName}" ""

; GUI options
ShowInstDetails show
ShowUninstDetails show

; Request administrator privileges
RequestExecutionLevel admin

;=========================================================
; Uninstall other version before continuing
;---------------------------------------------------------
Function .onInit
	ClearErrors
	ReadRegStr $R0 HKLM "Software\${PackageName}" ""
	IfErrors 0 +2 
		goto continue

	MessageBox MB_YESNO|MB_ICONQUESTION "Another version of the toolkit is installed. You must uninstall to continue. Continue?" IDNO justexit
	ExecWait '"$INSTDIR\Uninstall.exe" _?=$INSTDIR' $0
	IntCmp $0 0 continue justexit justexit

justexit:
	Abort
continue:
FunctionEnd

;=========================================================
; Installer Pages
;---------------------------------------------------------

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY

; Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${PackageName}" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

;=========================================================
; Uninstaller Pages
;---------------------------------------------------------
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;=========================================================
; Language Settings
;---------------------------------------------------------
!insertmacro MUI_LANGUAGE "English"

;=========================================================
; Installation Types
;---------------------------------------------------------
!define SEC_FULL 1
!define SEC_TYPICAL 2

InstType "Full"
InstType "Typical"

;=========================================================
; Installer File definitions
;---------------------------------------------------------
!define GenericIOTools_Files "..\grab\Release\grab.exe ..\odc\Release\odc.exe ..\odd\Release\odd.exe ..\odx\Release\odx.exe ..\mac2pw\Release\mac2pw.exe ..\mac2pwd\Release\mac2pwd.exe"

!define AtherosIOTools_NVM_Files "..\chknvm\Release\chknvm.exe ..\chknvm2\Release\chknvm2.exe ..\nvmmerge\Release\nvmmerge.exe ..\nvmsplit\Release\nvmsplit.exe ..\setnvm\Release\setnvm.exe"
!define AtherosIOTools_PIB_Files "..\chkpib\Release\chkpib.exe ..\chkpib2\Release\chkpib2.exe ..\getpib\Release\getpib.exe ..\modpib\Release\modpib.exe ..\mrgpib\Release\mrgpib.exe ..\pibruin\Release\pibruin.exe ..\pibrump\Release\pibrump.exe ..\psin\Release\psin.exe ..\pskey\Release\pskey.exe ..\psnotch\Release\psnotch.exe ..\psout\Release\psout.exe ..\pxe\Release\pxe.exe ..\setpib\Release\setpib.exe"
!define AtherosIOTools_KEY_Files "..\hpavkey\Release\hpavkey.exe ..\hpavkeys\Release\hpavkeys.exe ..\rkey\Release\rkey.exe"
!define AtherosIOTools_RAM_Files "..\sdram\Release\sdram.exe ..\config2cfg\Release\config2cfg.exe"
!define AtherosIOTools_MISC_Files "..\mme\Release\mme.exe ..\watchdog\Release\watchdog.exe"

!define GenericSerialTools_Files "..\weeder\Release\weeder.exe"
!define AtherosSerialTools_Files "..\int6kdetect\Release\int6kdetect.exe ..\int6kuart\Release\int6kuart.exe ..\int6kbaud\Release\int6kbaud.exe"

!define GenericEthernetTools_Files "..\edru\Release\edru.exe ..\edsu\Release\edsu.exe ..\efbu\Release\efbu.exe ..\efsu\Release\efsu.exe ..\netifs\Release\netifs.exe ..\pcapdevs\Release\pcapdevs.exe"

!define AtherosEthernetTools_CORE_Files "..\CMEncrypt\Release\CMEncrypt.exe ..\amplist\Release\amplist.exe ..\amprate\Release\amprate.exe ..\ampsnr\Release\ampsnr.exe ..\ampstat\Release\ampstat.exe ..\amptest\Release\amptest.exe ..\amptone\Release\amptone.exe ..\amptool\Release\amptool.exe ..\ar8236mdio\Release\ar8236mdio.exe ..\int6k\Release\int6k.exe ..\int6kboot\Release\int6kboot.exe ..\int6kecho\Release\int6kecho.exe ..\int6keth\Release\int6keth.exe ..\int6kid\Release\int6kid.exe ..\int6klist\Release\int6klist.exe ..\int6klog\Release\int6klog.exe ..\int6km\Release\int6km.exe ..\int6kmdio\Release\int6kmdio.exe ..\int6kmod\Release\int6kmod.exe ..\int6kp\Release\int6kp.exe ..\int6kpair\Release\int6kpair.exe ..\int6kprobe\Release\int6kprobe.exe ..\int6krate\Release\int6krate.exe ..\int6krule\Release\int6krule.exe ..\int6kstat\Release\int6kstat.exe ..\int6ktest\Release\int6ktest.exe ..\int6ktone\Release\int6ktone.exe ..\int6kug\Release\int6kug.exe ..\int6kwait\Release\int6kwait.exe ..\plcnets\Release\plcnets.exe ..\sada\Release\sada.exe"

!define AtherosEthernetTools_BOOT_Files "..\ampboot\Release\ampboot.exe ..\amphost\Release\amphost.exe ..\ampinit\Release\ampinit.exe ..\int64host\Release\int64host.exe ..\int6khost\Release\int6khost.exe ..\int6kf\Release\int6kf.exe"

!define AtherosEthernetTools_SNIFFER_Files "..\ampsnif\Release\ampsnif.exe ..\ampsnot\Release\ampsnot.exe ..\hpav\Release\hpav.exe ..\arpc\Release\arpc.exe ..\arpc1\Release\arpc1.exe"

!define AtherosEthernetTools_DEMO_Files "..\int6k2\Release\int6k2.exe"

!define AtherosEthernetTools_COQOS_Files "..\coqos_add\Release\coqos_add.exe ..\coqos_info\Release\coqos_info.exe ..\coqos_man\Release\coqos_man.exe ..\coqos_mod\Release\coqos_mod.exe ..\coqos_rel\Release\coqos_rel.exe"

;=========================================================
; Installer Sections
;---------------------------------------------------------
SubSection "File I/O Tools" SecIOTools
	Section "Generic" SecGenericIOTools
		SectionIn ${SEC_FULL}
		SetOutPath "$INSTDIR"
		File ${GenericIOTools_Files}
	SectionEnd
	SubSection "Atheros" SecAtherosIOTools
		Section "NVM" SecAtherosIOTools_NVM
			SectionIn ${SEC_FULL}
			SetOutPath "$INSTDIR"
			File ${AtherosIOTools_NVM_Files}
		SectionEnd
		Section "PIB" SecAtherosIOTools_PIB
			SectionIn ${SEC_FULL} ${SEC_TYPICAL}
			SetOutPath "$INSTDIR"
			File ${AtherosIOTools_PIB_Files}
		SectionEnd
		Section "Key" SecAtherosIOTools_KEY
			SectionIn ${SEC_FULL} ${SEC_TYPICAL}
			SetOutPath "$INSTDIR"
			File ${AtherosIOTools_KEY_Files}
		SectionEnd
		Section "RAM" SecAtherosIOTools_RAM
			SectionIn ${SEC_FULL}
			SetOutPath "$INSTDIR"
			File ${AtherosIOTools_RAM_Files}
		SectionEnd
		Section "Misc" SecAtherosIOTools_MISC
			SectionIn ${SEC_FULL}
			SetOutPath "$INSTDIR"
			File ${AtherosIOTools_MISC_Files}
		SectionEnd
	SubSectionEnd
SubSectionEnd

SubSection "Serial Tools" SecSerialTools
	Section "Generic" SecGenericSerialTools
		SectionIn ${SEC_FULL}
		SetOutPath "$INSTDIR"
		File ${GenericSerialTools_Files}
	SectionEnd
	Section "Atheros" SecAtherosSerialTools
		SectionIn ${SEC_FULL}
		SetOutPath "$INSTDIR"
		File ${AtherosSerialTools_Files}
	SectionEnd
SubSectionEnd

SubSection "Ethernet Tools" SecEthernetTools
	Section "Generic" SecGenericEthernetTools
		SectionIn ${SEC_FULL} ${SEC_TYPICAL}
		SetOutPath "$INSTDIR"
		File ${GenericEthernetTools_Files}
	SectionEnd
	SubSection "Atheros" SecAtherosEthernetTools
		Section "Core" SecAtherosEthernetTools_CORE
			SectionIn ${SEC_FULL} ${SEC_TYPICAL}
			SetOutPath "$INSTDIR"
			File ${AtherosEthernetTools_CORE_Files}
		SectionEnd
		Section "Boot" SecAtherosEthernetTools_BOOT
			SectionIn ${SEC_FULL} ${SEC_TYPICAL}
			SetOutPath "$INSTDIR"
			File ${AtherosEthernetTools_BOOT_Files}
		SectionEnd
		Section "Sniffer" SecAtherosEthernetTools_SNIFFER
			SectionIn ${SEC_FULL}
			SetOutPath "$INSTDIR"
			File ${AtherosEthernetTools_SNIFFER_Files}
		SectionEnd
		Section "API Demo" SecAtherosEthernetTools_DEMO
			SectionIn ${SEC_FULL}
			SetOutPath "$INSTDIR"
			File ${AtherosEthernetTools_DEMO_Files}
		SectionEnd
		Section "CoQoS" SecAtherosEthernetTools_COQOS
			SectionIn ${SEC_FULL}
			SetOutPath "$INSTDIR"
			File ${AtherosEthernetTools_COQOS_Files}
		SectionEnd
	SubSectionEnd
SubSectionEnd

Section "Documentation" SecDocs
	SectionIn ${SEC_FULL}
	SetOutPath "$INSTDIR\docs"

	; Files
	File "..\..\docbook\*.html"
	File "..\..\docbook\*.css"
	File "..\..\docbook\*.png"

	; Create shortcuts
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Users's Guide.lnk" "$INSTDIR\docs\index.html", "$INSTDIR\docs.ico"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Programmer's Guide.lnk" "$INSTDIR\docs\toolkit.html", "$INSTDIR\docs.ico"
	!insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "Required" SecRequired
	SectionIn RO
	SetOutPath "$INSTDIR"

	; Files
	File /oname=CHANGELOG.txt "..\..\README"
	File "launchcmd.bat"
	File "docs.ico"
	File "WinPcap_4_1_2.exe"
	ExecWait '"$INSTDIR\WinPcap_4_1_2.exe"' $0
	DetailPrint "WinPcap installer returned $0"
	Delete "$INSTDIR\WinPcap_4_1_2.exe"

	; Store installation folder
	WriteRegStr HKLM "Software\${PackageName}" "" $INSTDIR

	; Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"

	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application

	; Create shortcuts
	CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
	CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
	CreateShortCut "$SMPROGRAMS\$StartMenuFolder\CHANGELOG.lnk" "$INSTDIR\CHANGELOG.txt"
	CreateShortCut "$SMPROGRAMS\$StartMenuFolder\launchcmd.lnk" "$INSTDIR\launchcmd.bat"

	!insertmacro MUI_STARTMENU_WRITE_END

	; Windows Add/Remove programs
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PackageName}" "DisplayName" "${PackageName} ${PackageVersion}${PackageType}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PackageName}" "UninstallString" "$INSTDIR\Uninstall.exe"

	; Add to PATH for all users
	${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR"
SectionEnd

;=========================================================
; Descriptions for sections
;---------------------------------------------------------
; Language strings

LangString DESC_SecIOTools ${LANG_ENGLISH} "File I/O tools. These operate on files or Standard input/output."
LangString DESC_SecGenericIOTools ${LANG_ENGLISH} "Generic File I/O tools."
LangString DESC_SecAtherosIOTools ${LANG_ENGLISH} "Atheros File I/O tools."
LangString DESC_SecAtherosIOTools_NVM ${LANG_ENGLISH} "NVM tools."
LangString DESC_SecAtherosIOTools_PIB ${LANG_ENGLISH} "PIB tools."
LangString DESC_SecAtherosIOTools_KEY ${LANG_ENGLISH} "Key tools."
LangString DESC_SecAtherosIOTools_RAM ${LANG_ENGLISH} "RAM tools."
LangString DESC_SecAtherosIOTools_MISC ${LANG_ENGLISH} "Misc. tools."

LangString DESC_SecSerialTools ${LANG_ENGLISH} "Serial tools. These tools make use of the serial port."
LangString DESC_SecGenericSerialTools ${LANG_ENGLISH} "Generic Serial tools."
LangString DESC_SecAtherosSerialTools ${LANG_ENGLISH} "Atheros Serial tools."

LangString DESC_SecEthernetTools ${LANG_ENGLISH} "Ethernet tools. These tools make use of Ethernet port."
LangString DESC_SecGenericEthernetTools ${LANG_ENGLISH} "Generic Ethernet tools."
LangString DESC_SecAtherosEthernetTools ${LANG_ENGLISH} "Atheros Ethernet tools."
LangString DESC_SecAtherosEthernetTools_BOOT ${LANG_ENGLISH} "Boot tools."
LangString DESC_SecAtherosEthernetTools_SNIFFER ${LANG_ENGLISH} "Sniffer tools."
LangString DESC_SecAtherosEthernetTools_DEMO ${LANG_ENGLISH} "API Demo tools."
LangString DESC_SecAtherosEthernetTools_COQOS ${LANG_ENGLISH} "CoQoS tools."
LangString DESC_SecAtherosEthernetTools_CORE ${LANG_ENGLISH} "Core tools."

LangString DESC_SecDocs ${LANG_ENGLISH} "Documentation for the ${PackageName}. This includes a user's guide as well as a programmer's guide."
LangString DESC_SecRequired ${LANG_ENGLISH} "Required: WinPcap, update PATH, CHANGELOG, Uninstaller, and Shortcuts."

; Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN

!insertmacro MUI_DESCRIPTION_TEXT ${SecIOTools} $(DESC_SecIOTools)
!insertmacro MUI_DESCRIPTION_TEXT ${SecGenericIOTools} $(DESC_SecGenericIOTools)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAtherosIOTools} $(DESC_SecAtherosIOTools)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAtherosIOTools_NVM} $(DESC_SecAtherosIOTools_NVM)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAtherosIOTools_PIB} $(DESC_SecAtherosIOTools_PIB)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAtherosIOTools_KEY} $(DESC_SecAtherosIOTools_KEY)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAtherosIOTools_RAM} $(DESC_SecAtherosIOTools_RAM)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAtherosIOTools_MISC} $(DESC_SecAtherosIOTools_MISC)

!insertmacro MUI_DESCRIPTION_TEXT ${SecSerialTools} $(DESC_SecSerialTools)
!insertmacro MUI_DESCRIPTION_TEXT ${SecGenericSerialTools} $(DESC_SecGenericSerialTools)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAtherosSerialTools} $(DESC_SecAtherosSerialTools)

!insertmacro MUI_DESCRIPTION_TEXT ${SecEthernetTools} $(DESC_SecEthernetTools)
!insertmacro MUI_DESCRIPTION_TEXT ${SecGenericEthernetTools} $(DESC_SecGenericEthernetTools)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAtherosEthernetTools} $(DESC_SecAtherosEthernetTools)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAtherosEthernetTools_BOOT} $(DESC_SecAtherosEthernetTools_BOOT)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAtherosEthernetTools_SNIFFER} $(DESC_SecAtherosEthernetTools_SNIFFER)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAtherosEthernetTools_DEMO} $(DESC_SecAtherosEthernetTools_DEMO)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAtherosEthernetTools_COQOS} $(DESC_SecAtherosEthernetTools_COQOS)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAtherosEthernetTools_CORE} $(DESC_SecAtherosEthernetTools_CORE)

!insertmacro MUI_DESCRIPTION_TEXT ${SecDocs} $(DESC_SecDocs)
!insertmacro MUI_DESCRIPTION_TEXT ${SecRequired} $(DESC_SecRequired)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;=========================================================
; Uninstaller Section
;---------------------------------------------------------
Section "Uninstall"
	; Files
	Delete "$INSTDIR\docs\*.*"
	Delete "$INSTDIR\Uninstall.exe"
	Delete "$INSTDIR\*.exe"

	; Remove directories
	RMDir "$INSTDIR\docs"
	RMDir "$INSTDIR"

	; Remove start menu items
	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
	Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\Users's Guide.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\Programmer's Guide.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\CHANGELOG.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\launchcmd.lnk"
	RMDir "$SMPROGRAMS\$StartMenuFolder"

	; Remove from Add/Remove programs
	DeleteRegKey /ifempty HKLM "Software\${PackageName}"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PackageName}"

	; Remove from PATH for all users
	${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR"
SectionEnd
