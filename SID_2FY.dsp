# Microsoft Developer Studio Project File - Name="SID_2FY" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SID_2FY - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SID_2FY.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SID_2FY.mak" CFG="SID_2FY - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SID_2FY - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SID_2FY - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SID_2FY - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "_NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ws2_32.lib midSerial.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"Release/SID-2FY_V1.14.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "SID_2FY - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib midSerial.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"Debug/SID-2FY.exe" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "SID_2FY - Win32 Release"
# Name "SID_2FY - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Language"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Languge\LanguageManger.cpp
# End Source File
# Begin Source File

SOURCE=.\Languge\LanguageManger.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AdoDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\AdoRecordset.cpp
# End Source File
# Begin Source File

SOURCE=.\AnalogData.cpp
# End Source File
# Begin Source File

SOURCE=.\ButtonTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ComService.cpp
# End Source File
# Begin Source File

SOURCE=.\comSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateProjectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CurvePrintSet.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveView.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomizeWareSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DefineScale.cpp
# End Source File
# Begin Source File

SOURCE=.\DigitalData.cpp
# End Source File
# Begin Source File

SOURCE=.\dMessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\DownloadSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EventProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EventView.cpp
# End Source File
# Begin Source File

SOURCE=.\excel.cpp
# End Source File
# Begin Source File

SOURCE=.\FileInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame.cpp
# End Source File
# Begin Source File

SOURCE=.\FrequencyAndAngle.cpp
# End Source File
# Begin Source File

SOURCE=.\hangePswDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Md5Class.cpp
# End Source File
# Begin Source File

SOURCE=.\MltiTree.cpp
# End Source File
# Begin Source File

SOURCE=.\MyView.cpp
# End Source File
# Begin Source File

SOURCE=.\NetSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\ParaView.cpp
# End Source File
# Begin Source File

SOURCE=.\PreView.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectData.cpp
# End Source File
# Begin Source File

SOURCE=.\ProtectData.cpp
# End Source File
# Begin Source File

SOURCE=.\PWDDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RealDataView.cpp
# End Source File
# Begin Source File

SOURCE=.\RealMonitorView.cpp
# End Source File
# Begin Source File

SOURCE=.\scbarg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetIP.cpp
# End Source File
# Begin Source File

SOURCE=.\SetProjectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SID_2FY.cpp
# End Source File
# Begin Source File

SOURCE=.\SID_2FY.rc
# End Source File
# Begin Source File

SOURCE=.\SID_2FYDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\sizecbar.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusBarEx.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TestToolTip.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolBarEx.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolTipListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\UDPNetWork.cpp
# End Source File
# Begin Source File

SOURCE=.\UniteCurvDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\USBMidWare.cpp
# End Source File
# Begin Source File

SOURCE=.\USBSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\WareDataSubItem.cpp
# End Source File
# Begin Source File

SOURCE=.\WareEventData.cpp
# End Source File
# Begin Source File

SOURCE=.\WareEventList.cpp
# End Source File
# Begin Source File

SOURCE=.\WareFileData.cpp
# End Source File
# Begin Source File

SOURCE=.\WareView.cpp
# End Source File
# Begin Source File

SOURCE=.\XComboList.cpp
# End Source File
# Begin Source File

SOURCE=.\XHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\XListCtrl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AdoDatabase.h
# End Source File
# Begin Source File

SOURCE=.\AdoRecordset.h
# End Source File
# Begin Source File

SOURCE=.\AnalogData.h
# End Source File
# Begin Source File

SOURCE=.\ButtonTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ComService.h
# End Source File
# Begin Source File

SOURCE=.\comSocket.h
# End Source File
# Begin Source File

SOURCE=.\CoolTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\CreateProjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\CurvePrintSet.h
# End Source File
# Begin Source File

SOURCE=.\CurveView.h
# End Source File
# Begin Source File

SOURCE=.\CustomFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\CustomizeWareSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\DefineScale.h
# End Source File
# Begin Source File

SOURCE=.\DigitalData.h
# End Source File
# Begin Source File

SOURCE=.\dMessageBox.h
# End Source File
# Begin Source File

SOURCE=.\DownloadSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\EventProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\EventView.h
# End Source File
# Begin Source File

SOURCE=.\excel.h
# End Source File
# Begin Source File

SOURCE=.\FileInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\Frame.h
# End Source File
# Begin Source File

SOURCE=.\FrequencyAndAngle.h
# End Source File
# Begin Source File

SOURCE=.\hangePswDlg.h
# End Source File
# Begin Source File

SOURCE=.\InfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\InPlaceEdit.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Md5Class.h
# End Source File
# Begin Source File

SOURCE=.\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\midSerialLib.h
# End Source File
# Begin Source File

SOURCE=.\MltiTree.h
# End Source File
# Begin Source File

SOURCE=.\NetSocket.h
# End Source File
# Begin Source File

SOURCE=.\ParaView.h
# End Source File
# Begin Source File

SOURCE=.\PreView.h
# End Source File
# Begin Source File

SOURCE=.\ProjectData.h
# End Source File
# Begin Source File

SOURCE=.\ProtectData.h
# End Source File
# Begin Source File

SOURCE=.\PWDDlg.h
# End Source File
# Begin Source File

SOURCE=.\RealDataView.h
# End Source File
# Begin Source File

SOURCE=.\RealMonitorView.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\scbarg.h
# End Source File
# Begin Source File

SOURCE=.\SetIP.h
# End Source File
# Begin Source File

SOURCE=.\SetProjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\SID_2FY.h
# End Source File
# Begin Source File

SOURCE=.\SID_2FYDoc.h
# End Source File
# Begin Source File

SOURCE=.\sizecbar.h
# End Source File
# Begin Source File

SOURCE=.\StatusBarEx.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TestToolTip.h
# End Source File
# Begin Source File

SOURCE=.\ToolBarEx.h
# End Source File
# Begin Source File

SOURCE=.\ToolTipListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\UDPNetWork.h
# End Source File
# Begin Source File

SOURCE=.\UniteCurvDlg.h
# End Source File
# Begin Source File

SOURCE=.\USBMidWare.h
# End Source File
# Begin Source File

SOURCE=.\USBSocket.h
# End Source File
# Begin Source File

SOURCE=.\WareDataSubItem.h
# End Source File
# Begin Source File

SOURCE=.\WareEventData.h
# End Source File
# Begin Source File

SOURCE=.\WareEventList.h
# End Source File
# Begin Source File

SOURCE=.\WareFileData.h
# End Source File
# Begin Source File

SOURCE=.\WareView.h
# End Source File
# Begin Source File

SOURCE=.\XComboList.h
# End Source File
# Begin Source File

SOURCE=.\XHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\XListCtrl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\all.bmp
# End Source File
# Begin Source File

SOURCE=.\res\all_gray.bmp
# End Source File
# Begin Source File

SOURCE=.\res\arrow_down.ico
# End Source File
# Begin Source File

SOURCE=.\res\arrow_UP.ico
# End Source File
# Begin Source File

SOURCE=.\res\big_tool.bmp
# End Source File
# Begin Source File

SOURCE=.\image\bmp_back.bmp
# End Source File
# Begin Source File

SOURCE=.\res\change_all.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checkboxes.bmp
# End Source File
# Begin Source File

SOURCE=.\res\device.ico
# End Source File
# Begin Source File

SOURCE=.\res\event.ico
# End Source File
# Begin Source File

SOURCE=.\res\folder.ico
# End Source File
# Begin Source File

SOURCE=.\res\gray_change_all.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gray_mainTool.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon4.ico
# End Source File
# Begin Source File

SOURCE=.\res\lang_ch.bin
# End Source File
# Begin Source File

SOURCE=.\res\lang_eng.bin
# End Source File
# Begin Source File

SOURCE=.\res\mainTool.bmp
# End Source File
# Begin Source File

SOURCE=.\res\para.ico
# End Source File
# Begin Source File

SOURCE=.\image\preview.ico
# End Source File
# Begin Source File

SOURCE=.\res\realClock.ico
# End Source File
# Begin Source File

SOURCE=.\res\realData.ico
# End Source File
# Begin Source File

SOURCE=.\res\setting.bin
# End Source File
# Begin Source File

SOURCE=.\res\SID_2FY.ico
# End Source File
# Begin Source File

SOURCE=.\res\SID_2FY.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SID_2FYDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\image\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ware.ico
# End Source File
# Begin Source File

SOURCE=.\res\wareFiles.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\corona.smf
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
