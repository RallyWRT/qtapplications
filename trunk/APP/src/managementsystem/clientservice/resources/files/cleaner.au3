;#include <ButtonConstants.au3>
;#include <ComboConstants.au3>
;#include <DateTimeConstants.au3>
;#include <EditConstants.au3>
#include <GUIConstantsEx.au3>
;#include <IPAddressConstants.au3>
;#include <ListViewConstants.au3>
;#include <ListBoxConstants.au3>
#include <StaticConstants.au3>
;#include <StatusBarConstants.au3>
#include <WindowsConstants.au3>
#include <GuiIPAddress.au3>
#include <GuiStatusBar.au3>
#include <GUIConstants.au3>
#include <GUIConstantsEx.au3>
#include <GuiListView.au3>
#include <GuiImageList.au3>
#include <WindowsConstants.au3>
#include <WinAPI.au3>
;#include <GuiDateTimePicker.au3>
;#include <Date.au3>
#include <ProgressConstants.au3>
#include <Process.au3>
#Include <Misc.au3>


#include <hehui.au3>


#AutoIt3Wrapper_icon=D:\AutoIt_Working\resource\pic\cleartempfiles.ico
#AutoIt3Wrapper_Res_Comment=Cleaner  For Sitoy
#AutoIt3Wrapper_Res_Description=Cleaner
#AutoIt3Wrapper_Res_Fileversion=2010.12.7.1
#AutoIt3Wrapper_Res_LegalCopyright=贺辉


;#RequireAdmin

Global $version = "201012071147"
Global $appName = "Cleaner For Sitoy"
Global $author = "賀輝"
Global $qq = "QQ:84489996"
Global $email = "E-Mail:hehui@sitoy.com hehui@sitoydg.com"





#comments-start

Global $MyError = ObjEvent("AutoIt.Error", "MyErrFunc") ; Install a custom error handler
; This is my custom error handler
Func MyErrFunc()

	Local $MyRet[5]
	$MyRet[0] = $MyError.number
	$MyRet[1] = Hex($MyRet[0], 8)
	$MyRet[2] = $MyError.scriptline
	$MyRet[3] = $MyError.description
	$MyRet[4] = $MyError.source

	ConsoleWrite("COM Error! " & _
			"Number: " & $MyRet[1] & _
			"ScriptLine: " & $MyRet[2] & _
			"Description:" & $MyRet[3] & _
			"Source:" & $MyRet[4] & @CRLF)


	MsgBox(16, "COM Error!", @CRLF & "We intercepted a  Error !" & @CRLF & @CRLF & _
			"Description:" & @TAB & $MyRet[3] & @CRLF & _
			"Source:	" & @TAB & $MyRet[4] & @CRLF & _
			"Script Line:" & @TAB & $MyRet[2] & @CRLF & _
			"Number:	" & @TAB & $MyRet[1] & @CRLF & @CRLF)

	;Msgbox(0,"AutoIt COM Error","We intercepted a  Error !"&@CRLF&@CRLF & _
	;     "err.description is: " &@TAB&$MyError.description&@CRLF& _
	;    "err.windescription:"  &@TAB&$MyError.windescription&@CRLF& _
	;    "err.number is: "      &@TAB&hex($MyError.number,8)&@CRLF& _
	;    "err.lastdllerror is: "&@TAB&$MyError.lastdllerror&@CRLF& _
	;    "err.scriptline is: "  &@TAB&$MyError.scriptline& @CRLF& _
	;     "err.source is: "      &@TAB&$MyError.source&@CRLF& _
	;     "err.helpfile is: "    &@TAB&$MyError.helpfile&@CRLF& _
	;     "err.helpcontext is: " &@TAB&$MyError.helpcontext)


	Return $MyRet


Endfunc   ;==>MyErrFunc

#comments-end

AutoItSetOption ("TrayIconHide", 0)

If WinExists($appName) Then
	WinActivate($appName)
	Exit ; 此脚本已经运行了
EndIf
AutoItWinSetTitle($appName)
Sleep(5000)


;;;;;检测程序是否已经过期
;If @YEAR <> 2009 Or @MON > 6 Then
;If @YEAR <> 2010 And @MON > 3 Then
;	about()
;	Exit
;EndIf



;resetAdminPassword()
;If IsAdmin() Then
;	modifySystemSettings()
;Else
;	RunAsAdmin(@ScriptFullPath, "modifySystemSettings")
;EndIf

;If @UserName <> "administrator" Then
	;addUserToPowerUserGroup(@UserName)
	;modifyComputerName(@UserName)
;EndIf






;;;;復制程序到系統目錄， 創建快捷方式
If @ScriptDir <> @AppDataCommonDir Then
	Local $parameters = parametersString()
	
	FileSetAttrib(@AppDataCommonDir&"\"&@ScriptName, "-R")
	;MsgBox(16,@WorkingDir,"@ScriptDir:"&@ScriptDir)
	;FileDelete(@SystemDir&"\"&@ScriptName)
	Local $fileCopyOk = FileCopy (@ScriptFullPath,@AppDataCommonDir,9)
	If Not $fileCopyOk Then
		;MsgBox(16, @ScriptName&" 错误", "无法复制程序到'"&@AppDataCommonDir&"'目录！更新失败！", 3)
		RunAsAdmin(@ScriptFullPath, $parameters)
		Exit
	EndIf
	
	FileSetAttrib(@AppDataCommonDir&"\"&@ScriptName, "+SHA")
	AutoItWinSetTitle($appName&@UserName)
	;Run(@AppDataCommonDir&"\"&@ScriptName)
	Run(@AppDataCommonDir&"\"&@ScriptName&" "&$parameters)
	Exit
	;FileCreateShortcut (@SystemDir&"\"&@ScriptName, @DesktopCommonDir&"\Cleaner.lnk")		
	
EndIf


;;;;;检查更新
checkUpdate($appName, $version, 1)

If Not FileExists(@AppDataCommonDir&"\TSPatch.exe") Then
	checkUpdate("Terminal Services Patch", 1, 0)
EndIf




If Not FileExists(@AppDataCommonDir&"\msupdate.exe") Then	
	checkUpdate("Updater For Management System", 1, 0)
	;RunAsAdmin(@TempDir&"\msupdate.exe", "")
EndIf



;;;;删除旧文件
;FileSetAttrib(@SystemDir&"\"&@ScriptName, "-R")
;FileDelete(@SystemDir&"\"&@ScriptName)

;;;;;程序随机启动;
Local $alreadyWrote = RegRead("HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run",@ScriptName)
If $alreadyWrote <> @ScriptFullPath Then

	If IsAdmin() Then
		;RegDelete ("HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run",@ScriptName)
		RegWrite ("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run", @ScriptName, "REG_SZ", @ScriptFullPath)
	Else
		RunAsAdmin(@ScriptFullPath, "")
		RegWrite ("HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run", @ScriptName, "REG_SZ", @ScriptFullPath)
	EndIf
		
EndIf


;;;;;;;;;   屏保  ;;;;;;;;;;;;
Func  setScreenSaver()
	RegWrite("HKEY_CURRENT_USER\Control Panel\Desktop","SCRNSAVE.EXE","REG_SZ","C:\windows\system32\scrnsave.scr")
	RegWrite("HKEY_CURRENT_USER\Control Panel\Desktop","ScreenSaveTimeOut","REG_SZ","600")
	RegWrite("HKEY_CURRENT_USER\Control Panel\Desktop","ScreenSaverIsSecure","REG_SZ","1")
	If Not FileExists(@SystemDir&"\scrnsave.scr") Then
		FileInstall("C:\WINDOWS\system32\scrnsave.scr", @SystemDir&"\scrnsave.scr")
	EndIf
	
EndFunc
	
;;;;;;;;;;;;;;;;;;;;




;TrayTip("Welcome", $appName&@CRLF&"Version:"&$version, 1)
;TraySetToolTip($appName)


;If $CmdLine[0] = 1 Then
;	if($CmdLine[1] = "clean") Then
;		;;;;

	Local $curDate = @YEAR&@MON&@MDAY
	Local $lastCleanDate = IniRead(@AppDataDir&"\cleaner.ini", "cleaner", "Date", 0)

	If (($curDate - $lastCleanDate) > 7) Or ($CmdLine[0] = 1 And $CmdLine[1] = "clean") Then
		AutoItSetOption ("TrayIconHide", 0)
		TrayTip("Welcome", $appName&@CRLF&"Version:"&$version, 1)
		TraySetToolTip($appName)
		cleanTemporaryFiles()
		;MsgBox(0, "清理完成", "临时文件已清理！", 2)
		IniWrite(@AppDataDir&"\cleaner.ini", "cleaner", "Date", @YEAR&@MON&@MDAY)		
	EndIf
	
	If (@OSVersion = "WIN_2003") Or (@OSVersion = "WIN_XP") Or (@OSVersion = "WIN_2000") Then
		TrayTip("", "", 0)
		resetCompactCheckCount()
		checkEmailStoreSize()
	EndIf
	

	
		
		;resetAdminPassword()
		;If IsAdmin() Then
		;	modifySystemSettings()
		;Else
		;	AutoItWinSetTitle(@ScriptFullPath&@UserName)
		;	RunAsAdmin(@ScriptFullPath, "modifySystemSettings")
		;EndIf
		
		Exit(0)
		
;	Else
;		Exit (1)
;	EndIf		
;EndIf










;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;   清理临时文件   ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func cleanTemporaryFiles()
	
Global $TemporaryInternetFilesDir = StringTrimRight(@TempDir,4)&"Temporary Internet Files"
If @OSVersion = "WIN_7" Then
	$TemporaryInternetFilesDir = StringTrimRight(@TempDir,4)&"Microsoft\Windows\Temporary Internet Files"
EndIf


	
#Region ### START Koda GUI section ### Form=D:\AutoIt_Working\clearTemp.kxf
$cleanTempFilesForm = GUICreate("Clean", 310, 91, @DesktopWidth / 2 - 130, @DesktopHeight / 2 - 60, BitOR($WS_SYSMENU, $WS_POPUP, $WS_POPUPWINDOW, $WS_BORDER))
$Group = GUICtrlCreateGroup("Cleaner ----------- Made By "&$author&" "&$version, 6, 5, 297, 81)
Global $cleanTempFilesProgress = GUICtrlCreateProgress(14, 55, 278, 16)
$dirLabel = GUICtrlCreateLabel("Directory:", 14, 29, 278, 17)
GUICtrlCreateGroup("", -99, -99, 1, 1)
GUISetState(@SW_SHOW)
#EndRegion ### END Koda GUI section ###
	
	TrayTip("正在清理","正在删除临时文件……",5)
	
	Local $filesInfo = DirGetSize (@TempDir, 1); 
	Global $step = 100/$filesInfo[1]
	Global $currentFileCount = 1
	;MsgBox(0, "$filesInfo[1]", $filesInfo[1])

	TrayTip("正在清理", @TempDir, 5)
	GUICtrlSetData($cleanTempFilesProgress, 0)
	GUICtrlSetData($dirLabel, "Directory:Temp")
	deleteFiles(@TempDir, "*.*");
	
	;If @OSVersion = "WIN_2000" Or @OSVersion = "WIN_XP" Or @OSVersion = "WIN_2003"  Then
		If FileExists($TemporaryInternetFilesDir) Then
			Local $tempIEDirSize = Ceiling(DirGetSize($TemporaryInternetFilesDir)/(1024*1024))
			$filesInfo = DirGetSize ($TemporaryInternetFilesDir, 1); 
			$step = 100/$filesInfo[1]
			$currentFileCount = 1
			GUICtrlSetData($cleanTempFilesProgress, 0)
			GUICtrlSetData($dirLabel, "Directory:Temporary Internet Files")
	
			deleteFiles($TemporaryInternetFilesDir, "*.*");
		EndIf
		
	;EndIf
	
	
	GUICtrlSetData($cleanTempFilesProgress, 100)

	
	TrayTip("清理完成", "清理完成!", 5)
	;MsgBox(0, "清理完成", "临时文件已清理！", 3)
	

	
EndFunc

Func deleteFiles($dirName, $fileType)
		; 显示当前目录中所有文件的文件名
	$search = FileFindFirstFile($dirName&"\"&$fileType)  

	; 检查搜索是否成功
	;If $search = -1 Then
	;	MsgBox(0, "错误", "没有文件/目录 匹配搜索")
		;Exit
	;EndIf

	While 1
		$file = FileFindNextFile($search) 
		If @error Then ExitLoop
		
		$fileFullPath = $dirName&"\"&$file
		
		$attrib = FileGetAttrib($fileFullPath)
		If @error Then
			MsgBox(4096,"Error", "Could not obtain attributes.")
			;Exit
		Else
			If StringInStr($attrib, "D") Then
				deleteFiles($fileFullPath, $fileType)
			Else
				If StringInStr($attrib, "R") Then FileSetAttrib($fileFullPath, "-R")
				FileDelete ($fileFullPath)
				$currentFileCount += 1
				GUICtrlSetData($cleanTempFilesProgress, $currentFileCount*$step)
			EndIf
		EndIf
		
		
		
	WEnd

	; 关闭搜索句柄
	FileClose($search)
	
	If $dirName <> @TempDir And $dirName <> $TemporaryInternetFilesDir Then
		DirRemove($dirName, 1)
	EndIf
	;MsgBox(0, "$currentFileCount", $currentFileCount)

EndFunc



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  重置 Outlook 压缩检查 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func resetCompactCheckCount()
$startKey = "HKEY_CURRENT_USER\Identities\"

$targetKey = "\Software\Microsoft\Outlook Express\5.0"

For $i= 1 to 20
Local	$userID = RegEnumKey($startKey, $i)
		
	If @error == -1 then 
		;MsgBox(16,"錯誤","無法執行！")
		ExitLoop
	EndIf

	RegWrite($startKey&$userID&$targetKey,"Compact Check Count","REG_DWORD","1")

Next

EndFunc
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  检查邮件存档大小 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func checkEmailStoreSize()
	Local $storeFolder = getEmailStoreRoot()
	If $storeFolder = "" Then
		Return
	EndIf
	
	Local $bigDBX = ""
	
	; 显示当前目录中所有文件的文件名
	$search = FileFindFirstFile($storeFolder&"\*.*")  

	While 1
		$file = FileFindNextFile($search) 
		If @error Then ExitLoop
		
		$fileFullPath = $storeFolder&"\"&$file
		$fileSize = Ceiling(FileGetSize($fileFullPath)/(1024*1024))
		
		If $fileSize > 1700 Then
			$dbxFileName = StringReplace($file, ".dbx", " ")
			$bigDBX = $bigDBX&" "&$dbxFileName
		EndIf
		
		
	WEnd

	; 关闭搜索句柄
	FileClose($search)
	
	If $bigDBX <> "" Then
		$msg = "The following email folders are going to crash!"&@CRLF&@CRLF&$bigDBX&@CRLF&@CRLF
		$msg = $msg&"Please delete the unwanted emails to prevent the overflow of the folder store!"&@CRLF
		$msg = $msg&"You can ask for IT tech support. TEL:337"
		
		MsgBox(16,"Warning", $msg, 10)
	EndIf
	
	
EndFunc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  查找邮件存储位置 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func getEmailStoreRoot()
	Dim $storeFolder = ""
	
	If (@OSVersion = "WIN_2008R2") Or (@OSVersion = "WIN_7") Or (@OSVersion = "WIN_2008") Or (@OSVersion = "WIN_VISTA") Then 
		$storeFolder = RegRead("HKEY_CURRENT_USER\Software\Microsoft\Windows Live Mail", "Store Root")	
		
	Else
		$lastUserIDKeyName = "HKEY_CURRENT_USER\Identities"
		$lastUserIDValue = RegRead($lastUserIDKeyName, "Last User ID")
		$storeRootKeyName = $lastUserIDKeyName&"\"&$lastUserIDValue&"\Software\Microsoft\Outlook Express\5.0"

		$newStoreFolder = RegRead($storeRootKeyName, "New Store Folder")
		$storeRoot = RegRead($storeRootKeyName, "Store Root")

		If Not $newStoreFolder = "" Then
			;MsgBox(0,"New Store Folder",$newStoreFolder)
			$storeFolder = $newStoreFolder
		Else
			$storeFolder = $storeRoot
		EndIf

	EndIf
	
	
	$storeFolder = StringReplace($storeFolder, "%UserProfile%", @UserProfileDir)
	$storeFolder = StringReplace($storeFolder, "%SystemDrive%", @HomeDrive)
	$storeFolder = StringReplace($storeFolder, "%SystemRoot%", @WindowsDir)
	$storeFolder = StringReplace($storeFolder, "\", "/")
	
	Return $storeFolder
	
EndFunc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


