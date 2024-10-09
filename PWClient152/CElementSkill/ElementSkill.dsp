# Microsoft Developer Studio Project File - Name="ElementSkill" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ElementSkill - Win32 DebugLAAMemProfile
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ElementSkill.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ElementSkill.mak" CFG="ElementSkill - Win32 DebugLAAMemProfile"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ElementSkill - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ElementSkill - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ElementSkill - Win32 DbgRelease" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ElementSkill - Win32 DebugCRTAllocator" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ElementSkill - Win32 DbgReleaseLAA" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ElementSkill - Win32 DebugLAA" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ElementSkill - Win32 DbgReleaseLAALOG" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ElementSkill - Win32 DebugLAAMemProfile" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Element/ElementSkill", CBHAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ElementSkill - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "..\Include\stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /D "_SKILL_CLIENT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /libpath:"..\Lib\stlport"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\ElementSkill.lib ..\lib	copy Release\ElementSkill.dll ..\cbin	copy ElementSkill.h ..\include	copy skillstr.txt ..\cbin\configs
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ElementSkill - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Include\stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /D "_SKILL_CLIENT" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Debug/ElementSkill_d.dll" /pdbtype:sept /libpath:"..\Lib\stlport"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy debug\elementskill_d.lib ..\lib	copy debug\elementskill_d.dll ..\cbin	copy ElementSkill.h ..\include	copy skillstr.txt ..\cbin\configs
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ElementSkill - Win32 DbgRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ElementSkill___Win32_DbgRelease"
# PROP BASE Intermediate_Dir "ElementSkill___Win32_DbgRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgRelease"
# PROP Intermediate_Dir "DbgRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../include/iolib" /I "../include/stlport" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../include/iolib" /I "../include/stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /D "_SKILL_CLIENT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /libpath:"../lib/stlport"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /libpath:"../lib/stlport"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy DbgRelease\ElementSkill.lib ..\lib	copy DbgRelease\ElementSkill.dll ..\cbin	copy ElementSkill.h ..\include	copy skillstr.txt ..\cbin\configs
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ElementSkill - Win32 DebugCRTAllocator"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ElementSkill___Win32_DebugCRTAllocator"
# PROP BASE Intermediate_Dir "ElementSkill___Win32_DebugCRTAllocator"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugCRT"
# PROP Intermediate_Dir "DebugCRT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /D "_SKILL_CLIENT" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\Include\stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /D "_SKILL_CLIENT" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Debug/ElementSkill_d.dll" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"DebugCRT/ElementSkill_d.dll" /pdbtype:sept /libpath:"..\Lib\stlport"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy debugCRT\elementskill_d.lib ..\libCRT	copy debugCRT\elementskill_d.dll ..\cbinCRT	copy ElementSkill.h ..\include	copy skillstr.txt ..\cbin\configs
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ElementSkill - Win32 DbgReleaseLAA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ElementSkill___Win32_DbgReleaseLAA"
# PROP BASE Intermediate_Dir "ElementSkill___Win32_DbgReleaseLAA"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgReleaseLAA"
# PROP Intermediate_Dir "DbgReleaseLAA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../include/iolib" /I "../include/stlport" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /D "_SKILL_CLIENT" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../include/iolib" /I "../include/stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /D "_SKILL_CLIENT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /libpath:"../lib/stlport"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /libpath:"../lib/stlport"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy DbgReleaseLAA\ElementSkill.lib ..\lib	copy DbgReleaseLAA\ElementSkill.dll ..\cbin	copy ElementSkill.h ..\include	copy skillstr.txt ..\cbin\configs
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ElementSkill - Win32 DebugLAA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ElementSkill___Win32_DebugLAA"
# PROP BASE Intermediate_Dir "ElementSkill___Win32_DebugLAA"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugLAA"
# PROP Intermediate_Dir "DebugLAA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /D "_SKILL_CLIENT" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /D "_SKILL_CLIENT" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Debug/ElementSkill_d.dll" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"DebugLAA/ElementSkill_d.dll" /pdbtype:sept /libpath:"..\Lib\stlport"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy debuglaa\elementskill_d.lib ..\lib	copy debuglaa\elementskill_d.dll ..\cbin	copy ElementSkill.h ..\include	copy skillstr.txt ..\cbin\configs
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ElementSkill - Win32 DbgReleaseLAALOG"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ElementSkill___Win32_DbgReleaseLAALOG"
# PROP BASE Intermediate_Dir "ElementSkill___Win32_DbgReleaseLAALOG"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgReleaseLAALOG"
# PROP Intermediate_Dir "DbgReleaseLAALOG"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../include/iolib" /I "../include/stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /D "_SKILL_CLIENT" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../include/iolib" /I "../include/stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /D "_SKILL_CLIENT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /libpath:"../lib/stlport"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /libpath:"../lib/stlport"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy DbgReleaseLAA\ElementSkill.lib ..\lib	copy DbgReleaseLAA\ElementSkill.dll ..\cbin	copy ElementSkill.h ..\include	copy skillstr.txt ..\cbin\configs
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ElementSkill - Win32 DebugLAAMemProfile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ElementSkill___Win32_DebugLAAMemProfile"
# PROP BASE Intermediate_Dir "ElementSkill___Win32_DebugLAAMemProfile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugLAAMemProfile"
# PROP Intermediate_Dir "DebugLAAMemProfile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /D "_SKILL_CLIENT" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ELEMENTSKILL_EXPORTS" /D "_SKILL_CLIENT" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"DebugLAA/ElementSkill_d.dll" /pdbtype:sept /libpath:"..\Lib\stlport"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"DebugLAA/ElementSkill_d.dll" /pdbtype:sept /libpath:"..\Lib\stlport"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy debuglaa\elementskill_d.lib ..\lib	copy debuglaa\elementskill_d.dll ..\cbin	copy ElementSkill.h ..\include	copy skillstr.txt ..\cbin\configs
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ElementSkill - Win32 Release"
# Name "ElementSkill - Win32 Debug"
# Name "ElementSkill - Win32 DbgRelease"
# Name "ElementSkill - Win32 DebugCRTAllocator"
# Name "ElementSkill - Win32 DbgReleaseLAA"
# Name "ElementSkill - Win32 DebugLAA"
# Name "ElementSkill - Win32 DbgReleaseLAALOG"
# Name "ElementSkill - Win32 DebugLAAMemProfile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ElementSkill.cpp
# End Source File
# Begin Source File

SOURCE=.\gnstring.cpp
# End Source File
# Begin Source File

SOURCE=.\skill.cpp
# End Source File
# Begin Source File

SOURCE=.\skillwrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\stubs1.cpp
# End Source File
# Begin Source File

SOURCE=.\stubs10.cpp
# End Source File
# Begin Source File

SOURCE=.\stubs11.cpp
# End Source File
# Begin Source File

SOURCE=.\stubs12.cpp
# End Source File
# Begin Source File

SOURCE=.\stubs2.cpp
# End Source File
# Begin Source File

SOURCE=.\stubs3.cpp
# End Source File
# Begin Source File

SOURCE=.\stubs4.cpp
# End Source File
# Begin Source File

SOURCE=.\stubs5.cpp
# End Source File
# Begin Source File

SOURCE=.\stubs6.cpp
# End Source File
# Begin Source File

SOURCE=.\stubs7.cpp
# End Source File
# Begin Source File

SOURCE=.\stubs8.cpp
# End Source File
# Begin Source File

SOURCE=.\stubs9.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ElementSkill.h
# End Source File
# Begin Source File

SOURCE=.\gnstring.h
# End Source File
# Begin Source File

SOURCE=.\playerwrapper.h
# End Source File
# Begin Source File

SOURCE=.\skill.h
# End Source File
# Begin Source File

SOURCE=.\skillwrapper.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
