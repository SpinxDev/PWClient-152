# Microsoft Developer Studio Project File - Name="ElementData" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ElementData - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ElementData.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ElementData.mak" CFG="ElementData - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ElementData - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ElementData - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Element/ElementData", OWEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ElementData - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\include\stlport" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_MD5_CHECK" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Imm32.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib AngelicaMedia.lib AngelicaFile.lib Angelica3D.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib shlwapi.lib dxerr8.lib speedtreert.lib CommonFile.lib PropDlg.lib AFileDialog2.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib AUInterface.lib /nologo /subsystem:windows /machine:I386 /out:"..\CBin\CElementData.exe" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\AUInterface2\Lib" /libpath:"..\lib\stlport"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "ElementData - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\include\stlport" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_MD5_CHECK" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 AngelicaCommon_d.lib Imm32.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaMedia_d.lib AngelicaFile_d.lib Angelica3D_d.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib shlwapi.lib dxerr8.lib speedtreert.lib CommonFile_d.lib PropDlg_d.lib AFileDialog2_d.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib AUInterface_d.lib dbghelp.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"..\CBin\CElementData_d.exe" /pdbtype:sept /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\AUInterface2\Lib" /libpath:"..\lib\stlport"

!ENDIF 

# Begin Target

# Name "ElementData - Win32 Release"
# Name "ElementData - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Md5"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_MD5Hash.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_MD5Hash.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\CElementClient\AMiniDump.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\AMiniDump.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\AWStringWithWildcard.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgExpireTime.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgExpireTime.h
# End Source File
# Begin Source File

SOURCE=.\DlgFindTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFindTemplResult.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProgressNotify.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSetTime.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSetTime.h
# End Source File
# Begin Source File

SOURCE=.\DlgTimeLength.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTimeLength.h
# End Source File
# Begin Source File

SOURCE=.\ElementData.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementData.rc
# End Source File
# Begin Source File

SOURCE=.\ElementDataDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementDataView.cpp
# End Source File
# Begin Source File

SOURCE=.\EquipMaskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExtTemplateIDSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FactionsSel.cpp
# End Source File
# Begin Source File

SOURCE=.\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\ListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ListDlgBar.cpp
# End Source File
# Begin Source File

SOURCE=.\LogoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MonsterFaction.cpp
# End Source File
# Begin Source File

SOURCE=.\PasswdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcTypeMaskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Render.cpp
# End Source File
# Begin Source File

SOURCE=.\SingleTextDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TalkCreateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TalkModifyDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Task\TaskExpAnalyser.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskIDSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TemplateCreateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TemplateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TemplateExtDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TemplIDSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TemplIDSelDlgEx.cpp
# End Source File
# Begin Source File

SOURCE=.\TestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TextInputDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeFlagMan.cpp
# End Source File
# Begin Source File

SOURCE=.\WipeOffDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XTreeCtrl.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ElementData.ico
# End Source File
# Begin Source File

SOURCE=.\res\ElementData.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ElementDataDoc.ico
# End Source File
# Begin Source File

SOURCE=".\res\folder .ico"
# End Source File
# Begin Source File

SOURCE=.\res\listtag.bmp
# End Source File
# Begin Source File

SOURCE=.\res\no_il.cur
# End Source File
# Begin Source File

SOURCE=.\res\object.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Writeable.ico
# End Source File
# End Group
# Begin Group "Data"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BaseDataExp.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseDataExp.h
# End Source File
# Begin Source File

SOURCE=.\BaseDataIDMan.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseDataIDMan.h
# End Source File
# Begin Source File

SOURCE=.\BaseDataOrg.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseDataOrg.h
# End Source File
# Begin Source File

SOURCE=.\BaseDataTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseDataTemplate.h
# End Source File
# Begin Source File

SOURCE=.\elementdataman.cpp
# End Source File
# Begin Source File

SOURCE=.\elementdataman.h
# End Source File
# Begin Source File

SOURCE=.\EnumTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\EnumTypes.h
# End Source File
# Begin Source File

SOURCE=.\ExpTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\ExpTypes.h
# End Source File
# Begin Source File

SOURCE=.\ExtendDataTempl.cpp
# End Source File
# Begin Source File

SOURCE=.\ExtendDataTempl.h
# End Source File
# Begin Source File

SOURCE=.\hardinfo.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Task\TaskTempl.cpp
# End Source File
# End Group
# Begin Group "VSS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OptionVssDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionVssDlg.h
# End Source File
# Begin Source File

SOURCE=.\VssOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\VssOperation.h
# End Source File
# End Group
# Begin Group "Policy"

# PROP Default_Filter ""
# Begin Group "Dialog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ConditionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConditionDlg.h
# End Source File
# Begin Source File

SOURCE=.\PolicyCreateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PolicyCreateDlg.h
# End Source File
# Begin Source File

SOURCE=.\PolicyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PolicyDlg.h
# End Source File
# Begin Source File

SOURCE=.\PolicyOperationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PolicyOperationDlg.h
# End Source File
# Begin Source File

SOURCE=.\PolicyTreeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PolicyTreeDlg.h
# End Source File
# Begin Source File

SOURCE=.\PolicyTriggerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PolicyTriggerDlg.h
# End Source File
# Begin Source File

SOURCE=.\RightTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\RightTreeCtrl.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ConditionParam.h
# End Source File
# Begin Source File

SOURCE=.\OperationParam.cpp
# End Source File
# Begin Source File

SOURCE=.\OperationParam.h
# End Source File
# Begin Source File

SOURCE=.\Policy.cpp
# End Source File
# Begin Source File

SOURCE=.\Policy.h
# End Source File
# Begin Source File

SOURCE=.\PolicyType.h
# End Source File
# Begin Source File

SOURCE=.\TargetParam.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\CElementClient\AWStringWithWildcard.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\DlgFindTemplate.h
# End Source File
# Begin Source File

SOURCE=.\DlgFindTemplResult.h
# End Source File
# Begin Source File

SOURCE=.\DlgProgressNotify.h
# End Source File
# Begin Source File

SOURCE=.\ElementData.h
# End Source File
# Begin Source File

SOURCE=.\ElementDataDoc.h
# End Source File
# Begin Source File

SOURCE=.\ElementDataView.h
# End Source File
# Begin Source File

SOURCE=.\EquipMaskDlg.h
# End Source File
# Begin Source File

SOURCE=.\ExtTemplateIDSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\FactionsSel.h
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\ListDlg.h
# End Source File
# Begin Source File

SOURCE=.\ListDlgBar.h
# End Source File
# Begin Source File

SOURCE=.\LogoDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MonsterFaction.h
# End Source File
# Begin Source File

SOURCE=.\PasswdDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcTypeMaskDlg.h
# End Source File
# Begin Source File

SOURCE=.\Render.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SingleTextDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TalkCreateDlg.h
# End Source File
# Begin Source File

SOURCE=.\TalkModifyDlg.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Task\TaskExpAnalyser.h
# End Source File
# Begin Source File

SOURCE=.\TaskIDSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\TemplateCreateDlg.h
# End Source File
# Begin Source File

SOURCE=.\TemplateDlg.h
# End Source File
# Begin Source File

SOURCE=.\TemplateExtDlg.h
# End Source File
# Begin Source File

SOURCE=.\TemplIDSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\TemplIDSelDlgEx.h
# End Source File
# Begin Source File

SOURCE=.\TestDlg.h
# End Source File
# Begin Source File

SOURCE=.\TextInputDlg.h
# End Source File
# Begin Source File

SOURCE=.\TreeFlagMan.h
# End Source File
# Begin Source File

SOURCE=.\WipeOffDlg.h
# End Source File
# Begin Source File

SOURCE=.\XTreeCtrl.h
# End Source File
# End Group
# Begin Group "Mounser Fall"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MounsterFallEleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MounsterFallEleDlg.h
# End Source File
# Begin Source File

SOURCE=.\MounsterFallItemDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MounsterFallItemDlg.h
# End Source File
# Begin Source File

SOURCE=.\MounsterFallListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MounsterFallListDlg.h
# End Source File
# Begin Source File

SOURCE=.\MounsterResetFallList.h
# End Source File
# End Group
# Begin Group "MultiTree"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MltiTree.cpp
# End Source File
# Begin Source File

SOURCE=.\MltiTree.h
# End Source File
# End Group
# Begin Group "Treasure House"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EditTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\EditTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\InputSubTypeNameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InputSubTypeNameDlg.h
# End Source File
# Begin Source File

SOURCE=.\TimeSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\Treasure.cpp
# End Source File
# Begin Source File

SOURCE=.\Treasure.h
# End Source File
# Begin Source File

SOURCE=.\TreasureDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TreasureDlg.h
# End Source File
# Begin Source File

SOURCE=.\TreasureItemDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TreasureItemDlg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
