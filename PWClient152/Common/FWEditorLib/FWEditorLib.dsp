# Microsoft Developer Studio Project File - Name="FWEditorLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=FWEditorLib - Win32 DebugLAAMemProfile
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FWEditorLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FWEditorLib.mak" CFG="FWEditorLib - Win32 DebugLAAMemProfile"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FWEditorLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FWEditorLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "FWEditorLib - Win32 DbgRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "FWEditorLib - Win32 DebugCRTAllocator" (based on "Win32 (x86) Static Library")
!MESSAGE "FWEditorLib - Win32 DbgReleaseLAA" (based on "Win32 (x86) Static Library")
!MESSAGE "FWEditorLib - Win32 DebugLAA" (based on "Win32 (x86) Static Library")
!MESSAGE "FWEditorLib - Win32 DbgReleaseLAALOG" (based on "Win32 (x86) Static Library")
!MESSAGE "FWEditorLib - Win32 DebugLAAMemProfile" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Tools/FWEditor/FWEditorLib", PHGAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FWEditorLib - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\FWEditorLib.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=attrib -r lib\FWAssemblySet.h	attrib -r lib\FWTemplate.h	copy FWAssemblySet.h lib\FWAssemblySet.h /Y	copy FWTemplate.h lib\FWTemplate.h /Y	copy FWAssemblySet.h ..\include\FWAssemblySet.h /Y	copy FWTemplate.h ..\include\FWTemplate.h /Y	copy lib\FWEditorLib.lib ..\lib /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FWEditorLib - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FWEditorLib___Win32_Debug"
# PROP BASE Intermediate_Dir "FWEditorLib___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /D "_LIB" /D "WIN32" /D "_WINDOWS_DEBUG" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\FWEditorLib_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=attrib -r lib\FWAssemblySet.h	attrib -r lib\FWTemplate.h	copy FWAssemblySet.h lib\FWAssemblySet.h /Y	copy FWTemplate.h lib\FWTemplate.h /Y	copy FWAssemblySet.h ..\include\FWAssemblySet.h /Y	copy FWTemplate.h ..\include\FWTemplate.h /Y	copy lib\FWEditorLib_d.lib ..\lib /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FWEditorLib - Win32 DbgRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FWEditorLib___Win32_DbgRelease"
# PROP BASE Intermediate_Dir "FWEditorLib___Win32_DbgRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgRelease"
# PROP Intermediate_Dir "DbgRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Op /Ob2 /I "./" /I "../AUInterface" /I "../common/gfx" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Op /Ob2 /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\FWEditorLib.lib"
# ADD LIB32 /nologo /out:"lib\FWEditorLib.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=attrib -r lib\FWAssemblySet.h	attrib -r lib\FWTemplate.h	copy FWAssemblySet.h lib\FWAssemblySet.h /Y	copy FWTemplate.h lib\FWTemplate.h /Y	copy FWAssemblySet.h ..\include\FWAssemblySet.h /Y	copy FWTemplate.h ..\include\FWTemplate.h /Y	copy lib\FWEditorLib.lib ..\lib /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FWEditorLib - Win32 DebugCRTAllocator"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FWEditorLib___Win32_DebugCRTAllocator"
# PROP BASE Intermediate_Dir "FWEditorLib___Win32_DebugCRTAllocator"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugCRT"
# PROP Intermediate_Dir "DebugCRT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "./" /I "../Include/AUI" /D "_LIB" /D "WIN32" /D "_WINDOWS_DEBUG" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /D "_LIB" /D "WIN32" /D "_WINDOWS_DEBUG" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\FWEditorLib_d.lib"
# ADD LIB32 /nologo /out:"libCRT\FWEditorLib_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=attrib -r libCRT\FWAssemblySet.h	attrib -r libCRT\FWTemplate.h	copy FWAssemblySet.h libCRT\FWAssemblySet.h /Y	copy FWTemplate.h libCRT\FWTemplate.h /Y	copy FWAssemblySet.h ..\include\FWAssemblySet.h /Y	copy FWTemplate.h ..\include\FWTemplate.h /Y	copy libCRT\FWEditorLib_d.lib ..\libCRT /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FWEditorLib - Win32 DbgReleaseLAA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FWEditorLib___Win32_DbgReleaseLAA"
# PROP BASE Intermediate_Dir "FWEditorLib___Win32_DbgReleaseLAA"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgReleaseLAA"
# PROP Intermediate_Dir "DbgReleaseLAA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /Op /Ob2 /I "./" /I "../Include/AUI" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Op /Ob2 /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\FWEditorLib.lib"
# ADD LIB32 /nologo /out:"lib\FWEditorLib.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=attrib -r lib\FWAssemblySet.h	attrib -r lib\FWTemplate.h	copy FWAssemblySet.h lib\FWAssemblySet.h /Y	copy FWTemplate.h lib\FWTemplate.h /Y	copy FWAssemblySet.h ..\include\FWAssemblySet.h /Y	copy FWTemplate.h ..\include\FWTemplate.h /Y	copy lib\FWEditorLib.lib ..\lib /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FWEditorLib - Win32 DebugLAA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FWEditorLib___Win32_DebugLAA"
# PROP BASE Intermediate_Dir "FWEditorLib___Win32_DebugLAA"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugLAA"
# PROP Intermediate_Dir "DebugLAA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "./" /I "../Include/AUI" /D "_LIB" /D "WIN32" /D "_WINDOWS_DEBUG" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /D "_LIB" /D "WIN32" /D "_WINDOWS_DEBUG" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\FWEditorLib_d.lib"
# ADD LIB32 /nologo /out:"lib\FWEditorLib_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=attrib -r lib\FWAssemblySet.h	attrib -r lib\FWTemplate.h	copy FWAssemblySet.h lib\FWAssemblySet.h /Y	copy FWTemplate.h lib\FWTemplate.h /Y	copy FWAssemblySet.h ..\include\FWAssemblySet.h /Y	copy FWTemplate.h ..\include\FWTemplate.h /Y	copy lib\FWEditorLib_d.lib ..\lib /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FWEditorLib - Win32 DbgReleaseLAALOG"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FWEditorLib___Win32_DbgReleaseLAALOG"
# PROP BASE Intermediate_Dir "FWEditorLib___Win32_DbgReleaseLAALOG"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgReleaseLAALOG"
# PROP Intermediate_Dir "DbgReleaseLAALOG"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /Op /Ob2 /I "./" /I "../Include/AUI" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Op /Ob2 /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\FWEditorLib.lib"
# ADD LIB32 /nologo /out:"lib\FWEditorLib.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=attrib -r lib\FWAssemblySet.h	attrib -r lib\FWTemplate.h	copy FWAssemblySet.h lib\FWAssemblySet.h /Y	copy FWTemplate.h lib\FWTemplate.h /Y	copy FWAssemblySet.h ..\include\FWAssemblySet.h /Y	copy FWTemplate.h ..\include\FWTemplate.h /Y	copy lib\FWEditorLib.lib ..\lib /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FWEditorLib - Win32 DebugLAAMemProfile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FWEditorLib___Win32_DebugLAAMemProfile"
# PROP BASE Intermediate_Dir "FWEditorLib___Win32_DebugLAAMemProfile"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugLAAMemProfile"
# PROP Intermediate_Dir "DebugLAAMemProfile"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /D "_LIB" /D "WIN32" /D "_WINDOWS_DEBUG" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /D "_LIB" /D "WIN32" /D "_WINDOWS_DEBUG" /D "_UNICODE" /D "_FW_EDITOR" /D "UNICODE" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\FWEditorLib_d.lib"
# ADD LIB32 /nologo /out:"lib\FWEditorLib_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=attrib -r lib\FWAssemblySet.h	attrib -r lib\FWTemplate.h	copy FWAssemblySet.h lib\FWAssemblySet.h /Y	copy FWTemplate.h lib\FWTemplate.h /Y	copy FWAssemblySet.h ..\include\FWAssemblySet.h /Y	copy FWTemplate.h ..\include\FWTemplate.h /Y	copy lib\FWEditorLib_d.lib ..\lib /Y
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "FWEditorLib - Win32 Release"
# Name "FWEditorLib - Win32 Debug"
# Name "FWEditorLib - Win32 DbgRelease"
# Name "FWEditorLib - Win32 DebugCRTAllocator"
# Name "FWEditorLib - Win32 DbgReleaseLAA"
# Name "FWEditorLib - Win32 DebugLAA"
# Name "FWEditorLib - Win32 DbgReleaseLAALOG"
# Name "FWEditorLib - Win32 DebugLAAMemProfile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CharContour.cpp
# End Source File
# Begin Source File

SOURCE=.\CharCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\CharOutline.cpp
# End Source File
# Begin Source File

SOURCE=.\CharVerticesBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\CodeTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\FWAlgorithm.cpp
# End Source File
# Begin Source File

SOURCE=.\FWArchive.cpp
# End Source File
# Begin Source File

SOURCE=.\FWArt.cpp
# End Source File
# Begin Source File

SOURCE=.\FWArtArc.cpp
# End Source File
# Begin Source File

SOURCE=.\FWArtCreator.cpp
# End Source File
# Begin Source File

SOURCE=.\FWArtDoubleArc.cpp
# End Source File
# Begin Source File

SOURCE=.\FWArtNoChange.cpp
# End Source File
# Begin Source File

SOURCE=.\FWArtStepScale.cpp
# End Source File
# Begin Source File

SOURCE=.\FWArtTilt.cpp
# End Source File
# Begin Source File

SOURCE=.\FWArtWave.cpp
# End Source File
# Begin Source File

SOURCE=.\FWAssemblyCache.cpp
# End Source File
# Begin Source File

SOURCE=.\FWAssemblyProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\FWAssemblySet.cpp
# End Source File
# Begin Source File

SOURCE=.\FWAUIManager.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommandCombine.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommandCreate.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommandDelete.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommandDuplicateHandle.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommandList.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommandMove.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommandMoveHandle.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommandRemoveHandle.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommandResize.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommandRotate.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommandSetBezierMode.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommandSetGlyphProp.cpp
# End Source File
# Begin Source File

SOURCE=.\FWCommandUnCombine.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogBezierMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogCustom.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogGraphMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogLanch.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogMain.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogParam.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogPlay.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogPolygonMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogPropBase.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogScale.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogSimple.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogText.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDialogTool.cpp
# End Source File
# Begin Source File

SOURCE=.\FWDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\FWFirework.cpp
# End Source File
# Begin Source File

SOURCE=.\FWFireworkCustom.cpp
# End Source File
# Begin Source File

SOURCE=.\FWFireworkElement.cpp
# End Source File
# Begin Source File

SOURCE=.\FWFireworkGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\FWFireworkLanch.cpp
# End Source File
# Begin Source File

SOURCE=.\FWFireworkLeaf.cpp
# End Source File
# Begin Source File

SOURCE=.\FWFireworkSimple.cpp
# End Source File
# Begin Source File

SOURCE=.\FWFlatCollector.cpp
# End Source File
# Begin Source File

SOURCE=.\FWFontEnum.cpp
# End Source File
# Begin Source File

SOURCE=.\FWFormula.cpp
# End Source File
# Begin Source File

SOURCE=.\FWFormulaElement.cpp
# End Source File
# Begin Source File

SOURCE=.\FWFormulaList.cpp
# End Source File
# Begin Source File

SOURCE=.\FWGfxWithModifier.cpp
# End Source File
# Begin Source File

SOURCE=.\FWGlyph.cpp
# End Source File
# Begin Source File

SOURCE=.\FWGlyphBezier.cpp
# End Source File
# Begin Source File

SOURCE=.\FWGlyphComposite.cpp
# End Source File
# Begin Source File

SOURCE=.\FWGlyphEllipse.cpp
# End Source File
# Begin Source File

SOURCE=.\FWGlyphFirework.cpp
# End Source File
# Begin Source File

SOURCE=.\FWGlyphGraphBase.cpp
# End Source File
# Begin Source File

SOURCE=.\FWGlyphList.cpp
# End Source File
# Begin Source File

SOURCE=.\FWGlyphPolygon.cpp
# End Source File
# Begin Source File

SOURCE=.\FWGlyphRectangle.cpp
# End Source File
# Begin Source File

SOURCE=.\FWGlyphRectBase.cpp
# End Source File
# Begin Source File

SOURCE=.\FWGlyphText.cpp
# End Source File
# Begin Source File

SOURCE=.\FWInnerProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\FWLog.cpp
# End Source File
# Begin Source File

SOURCE=.\FWManager.cpp
# End Source File
# Begin Source File

SOURCE=.\FWModifier.cpp
# End Source File
# Begin Source File

SOURCE=.\FWModifierCreator.cpp
# End Source File
# Begin Source File

SOURCE=.\FWModifierParabola.cpp
# End Source File
# Begin Source File

SOURCE=.\FWModifierRing.cpp
# End Source File
# Begin Source File

SOURCE=.\FWObject.cpp
# End Source File
# Begin Source File

SOURCE=.\FWOperand.cpp
# End Source File
# Begin Source File

SOURCE=.\FWOperator.cpp
# End Source File
# Begin Source File

SOURCE=.\FWOutterProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\FWParam.cpp
# End Source File
# Begin Source File

SOURCE=.\FWParticalQuota.cpp
# End Source File
# Begin Source File

SOURCE=.\FWParticalQuotaList.cpp
# End Source File
# Begin Source File

SOURCE=.\FWPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\FWRuntimeClass.cpp
# End Source File
# Begin Source File

SOURCE=.\FWSharedFile.cpp
# End Source File
# Begin Source File

SOURCE=.\FWState.cpp
# End Source File
# Begin Source File

SOURCE=.\FWStateCreateBezier.cpp
# End Source File
# Begin Source File

SOURCE=.\FWStateCreateFirework.cpp
# End Source File
# Begin Source File

SOURCE=.\FWStateCreatePolygon.cpp
# End Source File
# Begin Source File

SOURCE=.\FWStateCreateRect.cpp
# End Source File
# Begin Source File

SOURCE=.\FWStateCreateText.cpp
# End Source File
# Begin Source File

SOURCE=.\FWStateMove.cpp
# End Source File
# Begin Source File

SOURCE=.\FWStateMoveHandle.cpp
# End Source File
# Begin Source File

SOURCE=.\FWStateMoveWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\FWStateNetSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\FWStateResize.cpp
# End Source File
# Begin Source File

SOURCE=.\FWStateRotate.cpp
# End Source File
# Begin Source File

SOURCE=.\FWStateSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\FWStream.cpp
# End Source File
# Begin Source File

SOURCE=.\FWTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\FWTextMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\FWView.cpp
# End Source File
# Begin Source File

SOURCE=.\GLUContour.cpp
# End Source File
# Begin Source File

SOURCE=.\GLUPolygon.cpp
# End Source File
# Begin Source File

SOURCE=.\GLUTess.cpp
# End Source File
# Begin Source File

SOURCE=.\Mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Triangle.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CharContour.h
# End Source File
# Begin Source File

SOURCE=.\CharCurve.h
# End Source File
# Begin Source File

SOURCE=.\CharOutline.h
# End Source File
# Begin Source File

SOURCE=.\CharVerticesBuffer.h
# End Source File
# Begin Source File

SOURCE=.\CodeTemplate.h
# End Source File
# Begin Source File

SOURCE=.\FWAlgorithm.h
# End Source File
# Begin Source File

SOURCE=.\FWArchive.h
# End Source File
# Begin Source File

SOURCE=.\FWArray.h
# End Source File
# Begin Source File

SOURCE=.\FWArt.h
# End Source File
# Begin Source File

SOURCE=.\FWArtArc.h
# End Source File
# Begin Source File

SOURCE=.\FWArtCreator.h
# End Source File
# Begin Source File

SOURCE=.\FWArtDoubleArc.h
# End Source File
# Begin Source File

SOURCE=.\FWArtNoChange.h
# End Source File
# Begin Source File

SOURCE=.\FWArtStepScale.h
# End Source File
# Begin Source File

SOURCE=.\FWArtTilt.h
# End Source File
# Begin Source File

SOURCE=.\FWArtWave.h
# End Source File
# Begin Source File

SOURCE=.\FWAssemblyCache.h
# End Source File
# Begin Source File

SOURCE=.\FWAssemblyProfile.h
# End Source File
# Begin Source File

SOURCE=.\FWAssemblySet.h
# End Source File
# Begin Source File

SOURCE=.\FWAUIManager.h
# End Source File
# Begin Source File

SOURCE=.\FWCommand.h
# End Source File
# Begin Source File

SOURCE=.\FWCommandCombine.h
# End Source File
# Begin Source File

SOURCE=.\FWCommandCreate.h
# End Source File
# Begin Source File

SOURCE=.\FWCommandDelete.h
# End Source File
# Begin Source File

SOURCE=.\FWCommandDuplicateHandle.h
# End Source File
# Begin Source File

SOURCE=.\FWCommandList.h
# End Source File
# Begin Source File

SOURCE=.\FWCommandMove.h
# End Source File
# Begin Source File

SOURCE=.\FWCommandMoveHandle.h
# End Source File
# Begin Source File

SOURCE=.\FWCommandRemoveHandle.h
# End Source File
# Begin Source File

SOURCE=.\FWCommandResize.h
# End Source File
# Begin Source File

SOURCE=.\FWCommandRotate.h
# End Source File
# Begin Source File

SOURCE=.\FWCommandSetBezierMode.h
# End Source File
# Begin Source File

SOURCE=.\FWCommandSetGlyphProp.h
# End Source File
# Begin Source File

SOURCE=.\FWCommandUnCombine.h
# End Source File
# Begin Source File

SOURCE=.\FWConfig.h
# End Source File
# Begin Source File

SOURCE=.\FWDefinedMsg.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogBase.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogBezierMenu.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogBoard.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogCustom.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogEdit.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogGraph.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogGraphMenu.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogLanch.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogMain.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogParam.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogPlay.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogPolygonMenu.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogPropBase.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogScale.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogSimple.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogText.h
# End Source File
# Begin Source File

SOURCE=.\FWDialogTool.h
# End Source File
# Begin Source File

SOURCE=.\FWDoc.h
# End Source File
# Begin Source File

SOURCE=.\FWFirework.h
# End Source File
# Begin Source File

SOURCE=.\FWFireworkCustom.h
# End Source File
# Begin Source File

SOURCE=.\FWFireworkElement.h
# End Source File
# Begin Source File

SOURCE=.\FWFireworkGraph.h
# End Source File
# Begin Source File

SOURCE=.\FWFireworkLanch.h
# End Source File
# Begin Source File

SOURCE=.\FWFireworkLeaf.h
# End Source File
# Begin Source File

SOURCE=.\FWFireworkSimple.h
# End Source File
# Begin Source File

SOURCE=.\FWFlatCollector.h
# End Source File
# Begin Source File

SOURCE=.\FWFontEnum.h
# End Source File
# Begin Source File

SOURCE=.\FWFormula.h
# End Source File
# Begin Source File

SOURCE=.\FWFormulaElement.h
# End Source File
# Begin Source File

SOURCE=.\FWFormulaList.h
# End Source File
# Begin Source File

SOURCE=.\FWGfxWithModifier.h
# End Source File
# Begin Source File

SOURCE=.\FWGlyph.h
# End Source File
# Begin Source File

SOURCE=.\FWGlyphBezier.h
# End Source File
# Begin Source File

SOURCE=.\FWGlyphComposite.h
# End Source File
# Begin Source File

SOURCE=.\FWGlyphEllipse.h
# End Source File
# Begin Source File

SOURCE=.\FWGlyphFirework.h
# End Source File
# Begin Source File

SOURCE=.\FWGlyphGraphBase.h
# End Source File
# Begin Source File

SOURCE=.\FWGlyphList.h
# End Source File
# Begin Source File

SOURCE=.\FWGlyphPolygon.h
# End Source File
# Begin Source File

SOURCE=.\FWGlyphRectangle.h
# End Source File
# Begin Source File

SOURCE=.\FWGlyphRectBase.h
# End Source File
# Begin Source File

SOURCE=.\FWGlyphText.h
# End Source File
# Begin Source File

SOURCE=.\FWInfoSet.h
# End Source File
# Begin Source File

SOURCE=.\FWInnerProperty.h
# End Source File
# Begin Source File

SOURCE=.\FWLog.h
# End Source File
# Begin Source File

SOURCE=.\FWManager.h
# End Source File
# Begin Source File

SOURCE=.\FWModifier.h
# End Source File
# Begin Source File

SOURCE=.\FWModifierCreator.h
# End Source File
# Begin Source File

SOURCE=.\FWModifierParabola.h
# End Source File
# Begin Source File

SOURCE=.\FWModifierRing.h
# End Source File
# Begin Source File

SOURCE=.\FWObject.h
# End Source File
# Begin Source File

SOURCE=.\FWObPtrList.h
# End Source File
# Begin Source File

SOURCE=.\FWOperand.h
# End Source File
# Begin Source File

SOURCE=.\FWOperator.h
# End Source File
# Begin Source File

SOURCE=.\FWOutterProperty.h
# End Source File
# Begin Source File

SOURCE=.\FWParam.h
# End Source File
# Begin Source File

SOURCE=.\FWParticalQuota.h
# End Source File
# Begin Source File

SOURCE=.\FWParticalQuotaList.h
# End Source File
# Begin Source File

SOURCE=.\FWPlayer.h
# End Source File
# Begin Source File

SOURCE=.\FWRuntimeClass.h
# End Source File
# Begin Source File

SOURCE=.\FWSharedFile.h
# End Source File
# Begin Source File

SOURCE=.\FWState.h
# End Source File
# Begin Source File

SOURCE=.\FWStateCreateBezier.h
# End Source File
# Begin Source File

SOURCE=.\FWStateCreateFirework.h
# End Source File
# Begin Source File

SOURCE=.\FWStateCreatePolygon.h
# End Source File
# Begin Source File

SOURCE=.\FWStateCreateRect.h
# End Source File
# Begin Source File

SOURCE=.\FWStateCreateText.h
# End Source File
# Begin Source File

SOURCE=.\FWStateMove.h
# End Source File
# Begin Source File

SOURCE=.\FWStateMoveHandle.h
# End Source File
# Begin Source File

SOURCE=.\FWStateMoveWindow.h
# End Source File
# Begin Source File

SOURCE=.\FWStateNetSelect.h
# End Source File
# Begin Source File

SOURCE=.\FWStateResize.h
# End Source File
# Begin Source File

SOURCE=.\FWStateRotate.h
# End Source File
# Begin Source File

SOURCE=.\FWStateSelect.h
# End Source File
# Begin Source File

SOURCE=.\FWStream.h
# End Source File
# Begin Source File

SOURCE=.\FWTemplate.h
# End Source File
# Begin Source File

SOURCE=.\FWTextMesh.h
# End Source File
# Begin Source File

SOURCE=.\FWView.h
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\GLUContour.h
# End Source File
# Begin Source File

SOURCE=.\GLUPolygon.h
# End Source File
# Begin Source File

SOURCE=.\GLUTess.h
# End Source File
# Begin Source File

SOURCE=.\Mesh.h
# End Source File
# Begin Source File

SOURCE=.\Triangle.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
