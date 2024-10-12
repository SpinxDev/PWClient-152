# Microsoft Developer Studio Project File - Name="ElementHintTool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ElementHintTool - Win32 Debug2
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ElementHintTool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ElementHintTool.mak" CFG="ElementHintTool - Win32 Debug2"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ElementHintTool - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ElementHintTool - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "ElementHintTool - Win32 DbgRelease" (based on "Win32 (x86) Console Application")
!MESSAGE "ElementHintTool - Win32 Debug2" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CElement/CElementHintTool", DGTAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ElementHintTool - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CElementClient\Network" /I "..\CElementClient\Network\IOLib" /I "..\CElementClient\Network\rpcdata" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\AUInterface2" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "UNICODE" /FR /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 AUInterface.lib AngelicaCommon_u.lib AngelicaFile_u.lib zliblib.lib ElementSkill.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib zlibwapi.lib /nologo /subsystem:console /machine:I386 /out:"..\CBin\wmgj_HintTool.exe" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\AUInterface2\Lib"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "ElementHintTool - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CElementClient\Network" /I "..\CElementClient\Network\IOLib" /I "..\CElementClient\Network\rpcdata" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\AUInterface2" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "UNICODE" /D "HINT_TOOL_DEBUG" /Fr /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 AUInterface_d.lib AngelicaCommon_ud.lib AngelicaFile_ud.lib ElementSkill_d.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib zliblib.lib zlibwapi.lib dbghelp.lib shlwapi.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\CBin\wmgj_HintTool_d.exe" /pdbtype:sept /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\AUInterface2\Lib"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "ElementHintTool - Win32 DbgRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ElementHintTool___Win32_DbgRelease"
# PROP BASE Intermediate_Dir "ElementHintTool___Win32_DbgRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgRelease"
# PROP Intermediate_Dir "DbgRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CElementClient\Network" /I "..\CElementClient\Network\IOLib" /I "..\CElementClient\Network\rpcdata" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "UNICODE" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CElementClient\Network" /I "..\CElementClient\Network\IOLib" /I "..\CElementClient\Network\rpcdata" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\AUInterface2" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "UNICODE" /FR /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AUInterface.lib AngelicaCommon_u.lib AngelicaFile_u.lib zliblib.lib ElementSkill.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib zlibwapi.lib /nologo /subsystem:console /machine:I386 /out:"..\CBin\wmgj_HintTool.exe" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport"
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 AUInterface_su.lib AngelicaCommon_dr.lib AngelicaFile_dr.lib zliblib.lib ElementSkill.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib zlibwapi.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\CBin\wmgj_HintTool_dr.exe" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\AUInterface2\Lib"

!ELSEIF  "$(CFG)" == "ElementHintTool - Win32 Debug2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ElementHintTool___Win32_Debug2"
# PROP BASE Intermediate_Dir "ElementHintTool___Win32_Debug2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug2"
# PROP Intermediate_Dir "Debug2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CElementClient\Network" /I "..\CElementClient\Network\IOLib" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "UNICODE" /D "HINT_TOOL_DEBUG" /Fr /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CElementClient\Network" /I "..\CElementClient\Network\IOLib" /I "..\CElementClient\Network\rpcdata" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\AUInterface2" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "UNICODE" /Fr /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 AUInterface_d.lib AngelicaCommon_ud.lib AngelicaFile_ud.lib ElementSkill_d.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib zliblib.lib zlibwapi.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\CBin\wmgj_HintTool_d.exe" /pdbtype:sept /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 AUInterface_d.lib AngelicaCommon_ud.lib AngelicaFile_ud.lib ElementSkill_d.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib zliblib.lib zlibwapi.lib shlwapi.lib dbghelp.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\CBin\wmgj_HintTool_d.exe" /pdbtype:sept /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\AUInterface2\Lib"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "ElementHintTool - Win32 Release"
# Name "ElementHintTool - Win32 Debug"
# Name "ElementHintTool - Win32 DbgRelease"
# Name "ElementHintTool - Win32 Debug2"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Inventory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_IvtrArmor.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrArmor.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrArrow.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrArrow.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrConsume.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrConsume.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrDecoration.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrDecoration.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrDestroyingEssence.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrDestroyingEssence.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrEquip.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrEquip.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrEquipMatter.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrEquipMatter.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrFashion.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrFashion.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrFlySword.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrFlySword.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrGoblin.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrGoblin.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrItem.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrItem.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrMaterial.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrMoneyConvertible.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrMoneyConvertible.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrPetItem.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrPetItem.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrScroll.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrScroll.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrStone.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrStone.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrTaskItem.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrTaskItem.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\EC_IvtrTypes.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrWeapon.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrWeapon.h
# End Source File
# End Group
# Begin Group "Main Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CElementClient\AMiniDump.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\AMiniDump.h
# End Source File
# Begin Source File

SOURCE=.\auto_delete.h
# End Source File
# Begin Source File

SOURCE=.\EC_BaseColor.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_BaseColor.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\EC_FixedMsg.h
# End Source File
# Begin Source File

SOURCE=.\EC_Game.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Game.h
# End Source File
# Begin Source File

SOURCE=.\EC_Global.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Global.h
# End Source File
# Begin Source File

SOURCE=.\EC_GPDataType.h
# End Source File
# Begin Source File

SOURCE=.\EC_Instance.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Instance.h
# End Source File
# Begin Source File

SOURCE=.\EC_PetCorral.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_PetCorral.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\EC_RoleTypes.h
# End Source File
# Begin Source File

SOURCE=.\EC_RTDebug.h
# End Source File
# Begin Source File

SOURCE=.\EC_Skill.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Skill.h
# End Source File
# Begin Source File

SOURCE=.\EC_StringTab.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_StringTab.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\elementdataman.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\elementdataman.h
# End Source File
# Begin Source File

SOURCE=.\ElementHintProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementHintProcess.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\itemdataman.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\itemdataman.h
# End Source File
# Begin Source File

SOURCE=.\MyLog.cpp
# End Source File
# Begin Source File

SOURCE=.\MyLog.h
# End Source File
# Begin Source File

SOURCE=.\xmlcoder.cpp
# End Source File
# Begin Source File

SOURCE=.\xmlcoder.h
# End Source File
# End Group
# Begin Group "GTransform"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GTransForm\callid.hxx
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getencrypteditemdesc
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getencrypteditemdesc.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getencrypteditemdesc_re
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getencrypteditemdesc_re.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getitemdesc
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getitemdesc.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getitemdesc_re
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getitemdesc_re.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getroledesc
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getroledesc.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getroledesc_re
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getroledesc_re.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getroleequipmentdesc
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getroleequipmentdesc.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getroleequipmentdesc_re
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getroleequipmentdesc_re.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getrolepetcorraldesc
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getrolepetcorraldesc.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getrolepetcorraldesc_re
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getrolepetcorraldesc_re.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getroleskilldesc
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getroleskilldesc.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getroleskilldesc_re
# End Source File
# Begin Source File

SOURCE=.\GTransForm\getroleskilldesc_re.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\gforcedata
# End Source File
# Begin Source File

SOURCE=.\GTransForm\gforcedatalist
# End Source File
# Begin Source File

SOURCE=.\GTransForm\gmeridiandata
# End Source File
# Begin Source File

SOURCE=.\GTransForm\gpet
# End Source File
# Begin Source File

SOURCE=.\GTransForm\gpetcorral
# End Source File
# Begin Source File

SOURCE=.\GTransForm\greincarnationdata
# End Source File
# Begin Source File

SOURCE=.\GTransForm\greincarnationrecord
# End Source File
# Begin Source File

SOURCE=.\GTransForm\groleequipment
# End Source File
# Begin Source File

SOURCE=.\GTransForm\groleinventory
# End Source File
# Begin Source File

SOURCE=.\GTransForm\grolepocket
# End Source File
# Begin Source File

SOURCE=.\GTransForm\grolestorehouse
# End Source File
# Begin Source File

SOURCE=.\GTransForm\gtransformclient.cpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\gtransformclient.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\gwebtraderolebrief
# End Source File
# Begin Source File

SOURCE=.\GTransForm\gwebtraderolebriefextend
# End Source File
# Begin Source File

SOURCE=.\GTransForm\keepalive
# End Source File
# Begin Source File

SOURCE=.\GTransForm\keepalive.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransForm\snsroleequipment
# End Source File
# Begin Source File

SOURCE=.\GTransForm\snsrolepetcorral
# End Source File
# Begin Source File

SOURCE=.\GTransForm\snsroleskills
# End Source File
# Begin Source File

SOURCE=.\GTransForm\state.cxx
# End Source File
# Begin Source File

SOURCE=.\GTransForm\state.hxx
# End Source File
# Begin Source File

SOURCE=.\GTransForm\stubs.cxx
# End Source File
# Begin Source File

SOURCE=.\GTransForm\toolannounceaid
# End Source File
# Begin Source File

SOURCE=.\GTransForm\toolannounceaid.hpp
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Group "IOLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\IOLib\gnstatic.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\IOLib\sha256.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\IOLib\sha256.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\ElementHintTool.cpp
# End Source File
# End Target
# End Project
