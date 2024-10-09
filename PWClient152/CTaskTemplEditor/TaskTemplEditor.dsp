# Microsoft Developer Studio Project File - Name="TaskTemplEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TaskTemplEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TaskTemplEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TaskTemplEditor.mak" CFG="TaskTemplEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TaskTemplEditor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TaskTemplEditor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CTaskTemplEditor", NOBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TaskTemplEditor - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../CElementData" /I "../CElementClient/task" /I ".\\" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\include\stlport" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "TASK_TEMPL_EDITOR" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 zliblib.lib shlwapi.lib AngelicaFile.lib AngelicaCommon.lib AngelicaMedia.lib PropDlg.lib /nologo /subsystem:windows /machine:I386 /out:"..\CBin\CTaskTemplEditor.exe" /libpath:"..\Lib\STLPORT" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\..\..\AUInterface2\Lib" /libpath:"..\lib\stlport"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "TaskTemplEditor - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../CElementData" /I "../CElementClient/task" /I ".\\" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\include\stlport" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "TASK_TEMPL_EDITOR" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 zliblib.lib shlwapi.lib AngelicaFile_d.lib AngelicaCommon_d.lib AngelicaMedia_d.lib PropDlg_d.lib dbghelp.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\CBin\CTaskTemplEditor_d.exe" /pdbtype:sept /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\..\..\AUInterface2\Lib" /libpath:"..\lib\stlport"

!ENDIF 

# Begin Target

# Name "TaskTemplEditor - Win32 Release"
# Name "TaskTemplEditor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AbsTimeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AwardByItemsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AwardByRatioDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AwardChangeValue.cpp
# End Source File
# Begin Source File

SOURCE=.\AwardDisplayValue.cpp
# End Source File
# Begin Source File

SOURCE=.\AwardDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AwardRanking.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementData\BaseDataIDMan.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementData\BaseDataOrg.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementData\BaseDataTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\ComboItem.cpp
# End Source File
# Begin Source File

SOURCE=.\CopySubTaskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DateTypeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DisplayGlobalValue.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementData\EC_MD5Hash.cpp
# End Source File
# Begin Source File

SOURCE=.\EditItem.cpp
# End Source File
# Begin Source File

SOURCE=.\EditList.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementData\elementdataman.cpp
# End Source File
# Begin Source File

SOURCE=.\ExcelConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\FindTaskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FloatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemsCandDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemsWantedDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LogoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MemInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MonPropDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MonsterSummonedDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MonWantedDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NewTaskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NPCInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NumDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Occupation.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerProb.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerRequirement.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerWanted.cpp
# End Source File
# Begin Source File

SOURCE=.\PQGobalValue.cpp
# End Source File
# Begin Source File

SOURCE=.\PQSubTaskProp.cpp
# End Source File
# Begin Source File

SOURCE=.\PreTaskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProficiencyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RankingAward.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionPointDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetNPCInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\CElementData\TalkCreateDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementData\TalkModifyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskAwardPage.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskClassPage.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskClassSheet.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementData\TaskIDSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskLinkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskMethodPage.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskPremisePage.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskPropertyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskRegionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskTemplEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskTemplEditor.rc
# End Source File
# Begin Source File

SOURCE=.\TaskTemplEditorDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskTemplEditorView.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskTitle.cpp
# End Source File
# Begin Source File

SOURCE=.\TeamInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TemplIDSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementData\TestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TimespanDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TimetableDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VectorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VerifyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VssOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\VSSOptionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WeekSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementData\XTreeCtrl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AbsTimeDlg.h
# End Source File
# Begin Source File

SOURCE=.\AwardByItemsDlg.h
# End Source File
# Begin Source File

SOURCE=.\AwardByRatioDlg.h
# End Source File
# Begin Source File

SOURCE=.\AwardChangeValue.h
# End Source File
# Begin Source File

SOURCE=.\AwardDisplayValue.h
# End Source File
# Begin Source File

SOURCE=.\AwardDlg.h
# End Source File
# Begin Source File

SOURCE=.\AwardRanking.h
# End Source File
# Begin Source File

SOURCE=.\ComboItem.h
# End Source File
# Begin Source File

SOURCE=.\CopySubTaskDlg.h
# End Source File
# Begin Source File

SOURCE=.\DateTypeDlg.h
# End Source File
# Begin Source File

SOURCE=.\DisplayGlobalValue.h
# End Source File
# Begin Source File

SOURCE=.\EditItem.h
# End Source File
# Begin Source File

SOURCE=.\EditList.h
# End Source File
# Begin Source File

SOURCE=.\ExcelConverter.h
# End Source File
# Begin Source File

SOURCE=.\FindTaskDlg.h
# End Source File
# Begin Source File

SOURCE=.\FloatDlg.h
# End Source File
# Begin Source File

SOURCE=.\ItemsCandDlg.h
# End Source File
# Begin Source File

SOURCE=.\ItemsWantedDlg.h
# End Source File
# Begin Source File

SOURCE=.\LogoDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MemInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\MonPropDlg.h
# End Source File
# Begin Source File

SOURCE=.\MonsterSummonedDlg.h
# End Source File
# Begin Source File

SOURCE=.\MonWantedDlg.h
# End Source File
# Begin Source File

SOURCE=.\NewTaskDlg.h
# End Source File
# Begin Source File

SOURCE=.\NPCInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\NumDlg.h
# End Source File
# Begin Source File

SOURCE=.\Occupation.h
# End Source File
# Begin Source File

SOURCE=.\PlayerProb.h
# End Source File
# Begin Source File

SOURCE=.\PlayerRequirement.h
# End Source File
# Begin Source File

SOURCE=.\PlayerWanted.h
# End Source File
# Begin Source File

SOURCE=.\PQGobalValue.h
# End Source File
# Begin Source File

SOURCE=.\PQSubTaskProp.h
# End Source File
# Begin Source File

SOURCE=.\PreTaskDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProficiencyDlg.h
# End Source File
# Begin Source File

SOURCE=.\RankingAward.h
# End Source File
# Begin Source File

SOURCE=.\RegionPointDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SetNPCInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TaskAreaDlg.h
# End Source File
# Begin Source File

SOURCE=.\TaskAwardPage.h
# End Source File
# Begin Source File

SOURCE=.\TaskClassPage.h
# End Source File
# Begin Source File

SOURCE=.\TaskClassSheet.h
# End Source File
# Begin Source File

SOURCE=.\TaskLinkDlg.h
# End Source File
# Begin Source File

SOURCE=.\TaskMethodPage.h
# End Source File
# Begin Source File

SOURCE=.\TaskPremisePage.h
# End Source File
# Begin Source File

SOURCE=.\TaskPropertyPage.h
# End Source File
# Begin Source File

SOURCE=.\TaskPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\TaskRegionDlg.h
# End Source File
# Begin Source File

SOURCE=.\TaskTemplEditor.h
# End Source File
# Begin Source File

SOURCE=.\TaskTemplEditorDoc.h
# End Source File
# Begin Source File

SOURCE=.\TaskTemplEditorView.h
# End Source File
# Begin Source File

SOURCE=.\TaskTitle.h
# End Source File
# Begin Source File

SOURCE=.\TeamInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\TemplIDSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\TimespanDlg.h
# End Source File
# Begin Source File

SOURCE=.\TimetableDlg.h
# End Source File
# Begin Source File

SOURCE=.\VectorDlg.h
# End Source File
# Begin Source File

SOURCE=.\VerifyDlg.h
# End Source File
# Begin Source File

SOURCE=.\VssOperation.h
# End Source File
# Begin Source File

SOURCE=.\VSSOptionDlg.h
# End Source File
# Begin Source File

SOURCE=.\WeekSelDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\listtag.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TaskTemplEditor.ico
# End Source File
# Begin Source File

SOURCE=.\res\TaskTemplEditor.rc2
# End Source File
# Begin Source File

SOURCE=.\res\TaskTemplEditorDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "TaskTemplCommon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CElementClient\task\TaskExpAnalyser.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\task\TaskExpAnalyser.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Task\TaskInterface.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Task\TaskProcess.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Task\TaskProcess.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Task\TaskTempl.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Task\TaskTempl.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Task\TaskTempl.inl
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Task\TaskTemplMan.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Task\TaskTemplMan.h
# End Source File
# End Group
# Begin Group "ExcelCommon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\TaskExcelTool\ExcelObject.cpp
# End Source File
# Begin Source File

SOURCE=..\TaskExcelTool\ExcelObject.h
# End Source File
# Begin Source File

SOURCE=..\TaskExcelTool\TaskConverter.cpp
# End Source File
# Begin Source File

SOURCE=..\TaskExcelTool\TaskConverter.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\TaskTemplEditor.reg
# End Source File
# End Target
# End Project
# Section TaskTemplEditor : {242A477E-5929-4A98-A9FA-A6BD35870DA1}
# 	1:14:IDD_SEL_STRING:102
# 	2:16:Resource Include:resource.h
# 	2:13:STSelString.h:STSelString.h
# 	2:19:CLASS: CSTSelString:CSTSelString
# 	2:15:STSelString.cpp:STSelString.cpp
# 	2:10:ENUM: enum:enum
# 	2:19:Application Include:TaskTemplEditor.h
# 	2:14:IDD_SEL_STRING:IDD_SEL_STRING
# End Section
