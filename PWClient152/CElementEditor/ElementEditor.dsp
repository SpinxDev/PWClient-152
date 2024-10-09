# Microsoft Developer Studio Project File - Name="ElementEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ElementEditor - Win32 Debug2
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ElementEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ElementEditor.mak" CFG="ElementEditor - Win32 Debug2"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ElementEditor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ElementEditor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ElementEditor - Win32 Debug2" (based on "Win32 (x86) Application")
!MESSAGE "ElementEditor - Win32 Release2" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Element/ElementEditor", SIDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ElementEditor - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ElementEditor___Win32_Debug"
# PROP BASE Intermediate_Dir "ElementEditor___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Include" /I "..\Common" /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /I "..\..\Angelica2\Output\A3DSDK\Include" /I "..\..\Angelica2\Output\AFSDK\Include" /I "..\..\Angelica2\Output\ACSDK\Include" /I "..\..\Angelica2\Output\AMSDK\Include" /I "..\..\Angelica2\Output\3rdSDK\Include" /I "F:\DirectX 8.1 SDK\Include" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "UNICODE" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /Gm /GX /ZI /Od /I ".\\" /I "\Property" /I ".\PathMap" /I "..\Include\CD" /I "..\Include" /I "..\CCommon" /I "..\..\Angelica2\Output\AFSDK\Include" /I "..\..\Angelica2\Output\ACSDK\Include" /I "..\..\Angelica2\Output\AMSDK\Include" /I "..\..\Angelica2\Output\3rdSDK\Include" /I "F:\DirectX 8.1 SDK\Include" /I "..\include" /I "..\..\Angelica2\Output\A3DSDK\Include" /I "..\include\Gfx" /I "..\CCommon\autoca" /I "..\CCommon\Face" /I "..\Include\luawrapper" /I "..\CElementClient" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_MODEDITOREX" /D "_ELEMENT_EDITOR_" /D "GFX_EDITOR" /YX /FD /GZ /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Imm32.lib vmlib_d.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_ud.lib AngelicaMedia_ud.lib AngelicaFile_ud.lib Angelica3D_ud.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\Lib" /libpath:"..\..\Angelica2\Output\A3DSDK\Lib" /libpath:"..\..\Angelica2\Output\AFSDK\Lib" /libpath:"..\..\Angelica2\Output\ACSDK\Lib" /libpath:"..\..\Angelica2\Output\AMSDK\Lib" /libpath:"..\..\Angelica2\Output\3rdSDK\Lib" /libpath:"F:\DirectX 8.1 SDK\lib"
# ADD LINK32 DdsTrans.lib CHBasedCD_d.lib Imm32.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_d.lib AngelicaMedia_d.lib AngelicaFile_d.lib Angelica3D_d.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib shlwapi.lib dxerr8.lib SpeedTreeRT_d.lib GfxEditor_d.lib luawrapper_d.lib lua5.1.mt.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib APhysXEngineStatic_d.lib PhysXLoader.lib NxCooking.lib vss_d.lib dbghelp.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /out:"..\CBin\CElementEditor_d.exe" /pdbtype:sept /libpath:"..\Lib\CD" /libpath:"..\Lib\lua" /libpath:"..\Lib" /libpath:"..\..\Angelica2\Output\A3DSDK\Lib" /libpath:"..\..\Angelica2\Output\AFSDK\Lib" /libpath:"..\..\Angelica2\Output\ACSDK\Lib" /libpath:"..\..\Angelica2\Output\AMSDK\Lib" /libpath:"..\..\Angelica2\Output\3rdSDK\Lib" /libpath:"F:\DirectX 8.1 SDK\lib" /libpath:"..\Lib\stlport" /LARGEADDRESSAWARE
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ElementEditor - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\Include" /I "..\Common" /I "..\..\Output\A3DSDK\Include" /I "..\..\Output\AFSDK\Include" /I "..\..\Output\ACSDK\Include" /I "..\..\Output\AMSDK\Include" /I "..\..\Output\3rdSDK\Include" /I "..\..\Angelica2\Output\A3DSDK\Include" /I "..\..\Angelica2\Output\AFSDK\Include" /I "..\..\Angelica2\Output\ACSDK\Include" /I "..\..\Angelica2\Output\AMSDK\Include" /I "..\..\Angelica2\Output\3rdSDK\Include" /I "F:\DirectX 8.1 SDK\Include" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I ".\\" /I "\Property" /I "\PathMap" /I "..\Include\CD" /I "..\Include" /I "..\CCommon" /I "..\Include\Gfx" /I "..\CCommon\autoca" /I "..\CCommon\Face" /I "..\..\Angelica2\Output\A3DSDK\Include" /I "..\..\Angelica2\Output\AFSDK\Include" /I "..\..\Angelica2\Output\ACSDK\Include" /I "..\..\Angelica2\Output\AMSDK\Include" /I "..\..\Angelica2\Output\3rdSDK\Include" /I "F:\DirectX 8.1 SDK\Include" /I "..\include" /I "..\Include\luawrapper" /I "..\CElementClient" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MODEDITOREX" /D "_AFXDLL" /D "_ELEMENT_EDITOR_" /D "GFX_EDITOR" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Imm32.lib vmlib.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_u.lib AngelicaMedia_u.lib AngelicaFile_u.lib Angelica3D_u.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /libpath:"..\Lib" /libpath:"..\..\Angelica2\Output\A3DSDK\Lib" /libpath:"..\..\Angelica2\Output\AFSDK\Lib" /libpath:"..\..\Angelica2\Output\ACSDK\Lib" /libpath:"..\..\Angelica2\Output\AMSDK\Lib" /libpath:"..\..\Angelica2\Output\3rdSDK\Lib" /libpath:"F:\DirectX 8.1 SDK\lib"
# ADD LINK32 DdsTrans.lib CHBasedCD.lib Imm32.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib AngelicaMedia.lib AngelicaFile.lib Angelica3D.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib shlwapi.lib SpeedTreeRT.lib GfxEditor.lib luawrapper.lib lua5.1.mt.lib strmbase_u.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib APhysXEngineStatic.lib PhysXLoader.lib NxCooking.lib vss.lib /nologo /subsystem:windows /machine:I386 /out:"..\CBin\CElementEditor.exe" /libpath:"..\Lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\lua" /libpath:"..\..\Angelica2\Output\A3DSDK\Lib" /libpath:"..\..\Angelica2\Output\AFSDK\Lib" /libpath:"..\..\Angelica2\Output\ACSDK\Lib" /libpath:"..\..\Angelica2\Output\AMSDK\Lib" /libpath:"..\..\Angelica2\Output\3rdSDK\Lib" /libpath:"F:\DirectX 8.1 SDK\lib" /libpath:"..\Lib\stlport" /LARGEADDRESSAWARE
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ElementEditor - Win32 Debug2"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ElementEditor___Win32_Debug2"
# PROP BASE Intermediate_Dir "ElementEditor___Win32_Debug2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug2"
# PROP Intermediate_Dir "Debug2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /Gm /GX /ZI /Od /I ".\\" /I "\Property" /I ".\PathMap" /I "..\Include\CD" /I "..\Include" /I "..\CCommon" /I "..\..\Angelica2\Output\AFSDK\Include" /I "..\..\Angelica2\Output\ACSDK\Include" /I "..\..\Angelica2\Output\AMSDK\Include" /I "..\..\Angelica2\Output\3rdSDK\Include" /I "F:\DirectX 8.1 SDK\Include" /I "..\include" /I "..\..\Angelica2\Output\A3DSDK\Include" /I "..\include\Gfx" /I "..\CCommon\autoca" /I "..\CCommon\Face" /I "..\Include\luawrapper" /I "..\CElementClient" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_MODEDITOREX" /D "_ELEMENT_EDITOR_" /D "GFX_EDITOR" /YX /FD /GZ /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MDd /Gm /GX /ZI /Od /I ".\\" /I ".\Property" /I ".\PathMap" /I "..\Include\CD" /I "..\Include" /I "..\CCommon" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\CCommon\autoca" /I "..\CCommon\Face" /I "..\Include\luawrapper" /I "..\CElementClient" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_MODEDITOREX" /D "_ELEMENT_EDITOR_" /D "GFX_EDITOR" /FR /YX /FD /GZ /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# SUBTRACT BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 DdsTrans.lib CHBasedCD_d.lib Imm32.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_d.lib AngelicaMedia_d.lib AngelicaFile_d.lib Angelica3D_d.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib shlwapi.lib dxerr8.lib SpeedTreeRT_d.lib GfxEditor_d.lib luawrapper_d.lib lua5.1.mt.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib APhysXEngineStatic_d.lib PhysXLoader.lib NxCooking.lib vss_d.lib dbghelp.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /out:"..\CBin\CElementEditor_d.exe" /pdbtype:sept /libpath:"..\Lib\CD" /libpath:"..\Lib\lua" /libpath:"..\Lib" /libpath:"..\..\Angelica2\Output\A3DSDK\Lib" /libpath:"..\..\Angelica2\Output\AFSDK\Lib" /libpath:"..\..\Angelica2\Output\ACSDK\Lib" /libpath:"..\..\Angelica2\Output\AMSDK\Lib" /libpath:"..\..\Angelica2\Output\3rdSDK\Lib" /libpath:"F:\DirectX 8.1 SDK\lib" /libpath:"..\Lib\stlport" /LARGEADDRESSAWARE
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 DdsTrans.lib CHBasedCD_d.lib Imm32.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_d.lib AngelicaMedia_d.lib AngelicaFile_d.lib Angelica3D_d.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib shlwapi.lib dxerr8.lib SpeedTreeRT_d.lib GfxEditor_d.lib luawrapper_d.lib lua5.1.mt.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib APhysXEngineStatic_d.lib PhysXLoader.lib NxCooking.lib vss_d.lib dbghelp.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /out:"..\CBin\CElementEditor_d2.exe" /pdbtype:sept /libpath:"..\Lib\CD" /libpath:"..\Lib" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\..\..\GfxCommon\Output\Lib" /libpath:"..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\lib\Win32" /libpath:"..\lib\stlport" /LARGEADDRESSAWARE
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ElementEditor - Win32 Release2"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ElementEditor___Win32_Release2"
# PROP BASE Intermediate_Dir "ElementEditor___Win32_Release2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release2"
# PROP Intermediate_Dir "Release2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Ob2 /I ".\\" /I "\Property" /I "\PathMap" /I "..\Include\CD" /I "..\Include" /I "..\CCommon" /I "..\Include\Gfx" /I "..\CCommon\autoca" /I "..\CCommon\Face" /I "..\..\Angelica2\Output\A3DSDK\Include" /I "..\..\Angelica2\Output\AFSDK\Include" /I "..\..\Angelica2\Output\ACSDK\Include" /I "..\..\Angelica2\Output\AMSDK\Include" /I "..\..\Angelica2\Output\3rdSDK\Include" /I "F:\DirectX 8.1 SDK\Include" /I "..\include" /I "..\Include\luawrapper" /I "..\CElementClient" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MODEDITOREX" /D "_AFXDLL" /D "_ELEMENT_EDITOR_" /D "GFX_EDITOR" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I ".\\" /I ".\Property" /I ".\PathMap" /I "..\Include\CD" /I "..\Include" /I "..\CCommon" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\CCommon\autoca" /I "..\CCommon\Face" /I "..\Include\luawrapper" /I "..\CElementClient" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MODEDITOREX" /D "_AFXDLL" /D "_ELEMENT_EDITOR_" /D "GFX_EDITOR" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 DdsTrans.lib CHBasedCD.lib Imm32.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib AngelicaMedia.lib AngelicaFile.lib Angelica3D.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib shlwapi.lib SpeedTreeRT.lib GfxEditor.lib luawrapper.lib lua5.1.mt.lib strmbase_u.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib APhysXEngineStatic.lib PhysXLoader.lib NxCooking.lib vss.lib /nologo /subsystem:windows /machine:I386 /out:"..\CBin\CElementEditor.exe" /libpath:"..\Lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\lua" /libpath:"..\..\Angelica2\Output\A3DSDK\Lib" /libpath:"..\..\Angelica2\Output\AFSDK\Lib" /libpath:"..\..\Angelica2\Output\ACSDK\Lib" /libpath:"..\..\Angelica2\Output\AMSDK\Lib" /libpath:"..\..\Angelica2\Output\3rdSDK\Lib" /libpath:"F:\DirectX 8.1 SDK\lib" /libpath:"..\Lib\stlport" /LARGEADDRESSAWARE
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 DdsTrans.lib CHBasedCD.lib Imm32.lib mpg123lib.lib Immwrapper.lib AngelicaCommon.lib AngelicaMedia.lib AngelicaFile.lib Angelica3D.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib shlwapi.lib SpeedTreeRT.lib GfxEditor.lib luawrapper.lib lua5.1.mt.lib strmbase_u.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib APhysXEngineStatic.lib PhysXLoader.lib NxCooking.lib vss.lib /nologo /subsystem:windows /machine:I386 /out:"..\CBin\CElementEditor2.exe" /libpath:"..\Lib\CD" /libpath:"..\Lib" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\..\..\GfxCommon\Output\Lib" /libpath:"..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\lib\Win32" /libpath:"..\lib\stlport" /LARGEADDRESSAWARE
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ElementEditor - Win32 Debug"
# Name "ElementEditor - Win32 Release"
# Name "ElementEditor - Win32 Debug2"
# Name "ElementEditor - Win32 Release2"
# Begin Group "Main Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Status Bar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\StatusCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusCombo.h
# End Source File
# Begin Source File

SOURCE=.\StatusControl.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusControl.h
# End Source File
# Begin Source File

SOURCE=.\StatusDayNightSet.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusDayNightSet.h
# End Source File
# Begin Source File

SOURCE=.\StatusEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusEdit.h
# End Source File
# Begin Source File

SOURCE=.\StatusProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusProgress.h
# End Source File
# Begin Source File

SOURCE=.\StatusStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusStatic.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\BrushTreeExporter.cpp
# End Source File
# Begin Source File

SOURCE=.\BrushTreeExporter.h
# End Source File
# Begin Source File

SOURCE=.\DataChunkFile.cpp
# End Source File
# Begin Source File

SOURCE=.\DataChunkFile.h
# End Source File
# Begin Source File

SOURCE=.\DayNightSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DayNightSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\DlgExportBHT.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgExportPathInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMergePlayerPassMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSceneObjectFinder.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementEditor.h
# End Source File
# Begin Source File

SOURCE=.\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainFrmUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\MD5Collector.cpp
# End Source File
# Begin Source File

SOURCE=.\MD5Collector.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Render Files"

# PROP Default_Filter ""
# Begin Group "Viewports"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OrthoViewport.cpp
# End Source File
# Begin Source File

SOURCE=.\OrthoViewport.h
# End Source File
# Begin Source File

SOURCE=.\PersViewport.cpp
# End Source File
# Begin Source File

SOURCE=.\PersViewport.h
# End Source File
# Begin Source File

SOURCE=.\RenderWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderWnd.h
# End Source File
# Begin Source File

SOURCE=.\ViewFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewFrame.h
# End Source File
# Begin Source File

SOURCE=.\Viewport.cpp
# End Source File
# Begin Source File

SOURCE=.\Viewport.h
# End Source File
# End Group
# Begin Group "Utilities"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Bitmap.h
# End Source File
# Begin Source File

SOURCE=.\Box3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Box3D.h
# End Source File
# Begin Source File

SOURCE=.\Staff.cpp
# End Source File
# Begin Source File

SOURCE=.\Staff.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\CoordinateDirection.cpp
# End Source File
# Begin Source File

SOURCE=.\CoordinateDirection.h
# End Source File
# Begin Source File

SOURCE=.\Render.cpp
# End Source File
# Begin Source File

SOURCE=.\Render.h
# End Source File
# End Group
# Begin Group "Database Files"

# PROP Default_Filter ""
# Begin Group "Terrain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\A3DTerrain2ForEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DTerrain2ForEditor.h
# End Source File
# Begin Source File

SOURCE=.\A3DTerrain2LoaderForEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DTerrain2LoaderForEditor.h
# End Source File
# Begin Source File

SOURCE=.\A3DTerrain2MaskForEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\A3DTerrain2MaskForEditor.h
# End Source File
# Begin Source File

SOURCE=.\AutoMask.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoMask.h
# End Source File
# Begin Source File

SOURCE=.\ExpTerrain.cpp
# End Source File
# Begin Source File

SOURCE=.\ExpTerrain.h
# End Source File
# Begin Source File

SOURCE=.\MaskModifier.cpp
# End Source File
# Begin Source File

SOURCE=.\MaskModifier.h
# End Source File
# Begin Source File

SOURCE=.\Terrain.cpp
# End Source File
# Begin Source File

SOURCE=.\Terrain.h
# End Source File
# Begin Source File

SOURCE=.\TerrainExpImpA.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainExpImpA.h
# End Source File
# Begin Source File

SOURCE=.\TerrainExpImpB.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainExpImpB.h
# End Source File
# Begin Source File

SOURCE=.\TerrainExportor.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainExportor.h
# End Source File
# Begin Source File

SOURCE=.\TerrainLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainLayer.h
# End Source File
# Begin Source File

SOURCE=.\TerrainObstruct.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainObstruct.h
# End Source File
# Begin Source File

SOURCE=.\TerrainRender.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainRender.h
# End Source File
# Begin Source File

SOURCE=.\TerrainSquare.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainSquare.h
# End Source File
# Begin Source File

SOURCE=.\TerrainWater.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainWater.h
# End Source File
# End Group
# Begin Group "Scene Object"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BitChunk.cpp
# End Source File
# Begin Source File

SOURCE=.\BitChunk.h
# End Source File
# Begin Source File

SOURCE=.\CloudBox.cpp
# End Source File
# Begin Source File

SOURCE=.\CloudBox.h
# End Source File
# Begin Source File

SOURCE=.\CritterGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\CritterGroup.h
# End Source File
# Begin Source File

SOURCE=.\EditerBezier.cpp
# End Source File
# Begin Source File

SOURCE=.\EditerBezier.h
# End Source File
# Begin Source File

SOURCE=.\EditerBezierPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\EditerBezierPoint.h
# End Source File
# Begin Source File

SOURCE=.\EditerBezierSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\EditerBezierSegment.h
# End Source File
# Begin Source File

SOURCE=.\ExpSceneObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ExpSceneObject.h
# End Source File
# Begin Source File

SOURCE=.\ModelContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelContainer.h
# End Source File
# Begin Source File

SOURCE=.\SceneAIGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneAIGenerator.h
# End Source File
# Begin Source File

SOURCE=.\SceneAudioObject.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneAudioObject.h
# End Source File
# Begin Source File

SOURCE=.\SceneBoxArea.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneBoxArea.h
# End Source File
# Begin Source File

SOURCE=.\SceneDummyObject.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneDummyObject.h
# End Source File
# Begin Source File

SOURCE=.\SceneDynamicObject.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneDynamicObject.h
# End Source File
# Begin Source File

SOURCE=.\SceneExportor.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneExportor.h
# End Source File
# Begin Source File

SOURCE=.\SceneGatherObject.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneGatherObject.h
# End Source File
# Begin Source File

SOURCE=.\SceneInstanceBox.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneInstanceBox.h
# End Source File
# Begin Source File

SOURCE=.\SceneLightObject.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneLightObject.h
# End Source File
# Begin Source File

SOURCE=.\SceneObject.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneObject.h
# End Source File
# Begin Source File

SOURCE=.\SceneObjectManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneObjectManager.h
# End Source File
# Begin Source File

SOURCE=.\ScenePrecinctObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ScenePrecinctObject.h
# End Source File
# Begin Source File

SOURCE=.\SceneSkinModel.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneSkinModel.h
# End Source File
# Begin Source File

SOURCE=.\SceneSpeciallyObject.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneSpeciallyObject.h
# End Source File
# Begin Source File

SOURCE=.\SceneVernier.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneVernier.h
# End Source File
# Begin Source File

SOURCE=.\SceneWaterObject.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneWaterObject.h
# End Source File
# Begin Source File

SOURCE=.\StaticModelObject.cpp
# End Source File
# Begin Source File

SOURCE=.\StaticModelObject.h
# End Source File
# Begin Source File

SOURCE=.\TerrainPlants.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainPlants.h
# End Source File
# End Group
# Begin Group "PathMap"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PathMap\BitImage.h
# End Source File
# Begin Source File

SOURCE=.\PathMap\BlockImage.h
# End Source File
# Begin Source File

SOURCE=.\PathMap\EDTmpl.h
# End Source File
# Begin Source File

SOURCE=.\PathMap\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\PathMap\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\PathMap\NPCMoveMap.cpp
# End Source File
# Begin Source File

SOURCE=.\PathMap\NPCMoveMap.h
# End Source File
# Begin Source File

SOURCE=.\PathMap\PassMapGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\PathMap\PassMapGenerator.h
# End Source File
# Begin Source File

SOURCE=.\PathMap\PlayerPassMapGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\PathMap\PlayerPassMapGenerator.h
# End Source File
# Begin Source File

SOURCE=.\PathMap\TerrainObstructGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\PathMap\TerrainObstructGenerator.h
# End Source File
# Begin Source File

SOURCE=.\PathMap\WaterAreaMap.cpp
# End Source File
# Begin Source File

SOURCE=.\PathMap\WaterAreaMap.h
# End Source File
# End Group
# Begin Group "SPOctree"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SPOctree\CompactSpacePassableOctree.cpp
# End Source File
# Begin Source File

SOURCE=.\SPOctree\CompactSpacePassableOctree.h
# End Source File
# Begin Source File

SOURCE=.\SPOctree\SpacePassableOctree.cpp
# End Source File
# Begin Source File

SOURCE=.\SPOctree\SpacePassableOctree.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AIGenExportor.cpp
# End Source File
# Begin Source File

SOURCE=.\AIGenExportor.h
# End Source File
# Begin Source File

SOURCE=.\EditObject.cpp
# End Source File
# Begin Source File

SOURCE=.\EditObject.h
# End Source File
# Begin Source File

SOURCE=.\ElementMap.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementMap.h
# End Source File
# Begin Source File

SOURCE=.\ElementResMan.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementResMan.h
# End Source File
# Begin Source File

SOURCE=.\ExpElementMap.cpp
# End Source File
# Begin Source File

SOURCE=.\ExpElementMap.h
# End Source File
# Begin Source File

SOURCE=.\LightMapGen.cpp
# End Source File
# Begin Source File

SOURCE=.\LightMapGen.h
# End Source File
# Begin Source File

SOURCE=.\LightMapMerger.cpp
# End Source File
# Begin Source File

SOURCE=.\LightMapMerger.h
# End Source File
# Begin Source File

SOURCE=.\LightMapMergerA.cpp
# End Source File
# Begin Source File

SOURCE=.\LightMapMergerA.h
# End Source File
# Begin Source File

SOURCE=.\LightMapMergerB.cpp
# End Source File
# Begin Source File

SOURCE=.\LightMapMergerB.h
# End Source File
# Begin Source File

SOURCE=.\LightMapTransition.cpp
# End Source File
# Begin Source File

SOURCE=.\LightMapTransition.h
# End Source File
# Begin Source File

SOURCE=.\LightProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\LightProcessor.h
# End Source File
# Begin Source File

SOURCE=.\NPCGenData.cpp
# End Source File
# Begin Source File

SOURCE=.\NPCGenData.h
# End Source File
# Begin Source File

SOURCE=.\ProjList.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjList.h
# End Source File
# Begin Source File

SOURCE=.\SevBezier.cpp
# End Source File
# Begin Source File

SOURCE=.\SevBezier.h
# End Source File
# End Group
# Begin Group "Common Files"

# PROP Default_Filter ""
# Begin Group "Autoca"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutoButterfly.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutoButterfly.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutoCrow.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutoCrow.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutoEagle.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutoEagle.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutoFirefly.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutoFirefly.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutoFish.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutoFish.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutonomousAgent.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutonomousAgent.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutonomousBehaviour.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutonomousBehaviour.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutonomousGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\A3DAutonomousGroup.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\AUTOCA\Utilities.h
# End Source File
# End Group
# Begin Group "Face"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CCommon\Face\Expression.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\Face\Expression.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\Face\FaceAnimation.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\Face\FaceAnimation.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\Face\FaceBone.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\Face\FaceBone.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\Face\FaceBoneController.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\Face\FaceBoneController.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\Face\FaceCommon.h
# End Source File
# End Group
# Begin Group "Lua"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\base64.cpp
# End Source File
# Begin Source File

SOURCE=.\base64.h
# End Source File
# Begin Source File

SOURCE=.\LuaBind.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaBind.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\CCommon\AObject.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\AObject.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\AProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\AProperty.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\AVariant.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\AVariant.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EC_Bezier.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EC_Bezier.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EC_BrushMan.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EC_BrushMan.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EC_BrushManUtil.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EC_MD5Hash.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EC_MD5Hash.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EC_WorldFile.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Archive.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Archive.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_BackMusic.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_BackMusic.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Building.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Building.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Cloud.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Cloud.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_CloudManager.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_CloudManager.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Forest.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Forest.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Grassland.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Grassland.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_GrassType.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_GrassType.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Precinct.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Precinct.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Region.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Region.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_TerrainOutline.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_TerrainOutline.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Tree.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Tree.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\elementdataman.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\elementdataman.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\globaldataman.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\globaldataman.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\PolygonFunc.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\PolygonFunc.h
# End Source File
# End Group
# Begin Group "Operations"

# PROP Default_Filter ""
# Begin Group "Terrain Operations"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ObstructOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\ObstructOperation.h
# End Source File
# Begin Source File

SOURCE=.\ObstructOperation2.cpp
# End Source File
# Begin Source File

SOURCE=.\ObstructOperation2.h
# End Source File
# Begin Source File

SOURCE=.\TerrainMaskPaintOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainMaskPaintOperation.h
# End Source File
# Begin Source File

SOURCE=.\TerrainMaskSmoothOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainMaskSmoothOperation.h
# End Source File
# Begin Source File

SOURCE=.\TerrainNoiseOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainNoiseOperation.h
# End Source File
# Begin Source File

SOURCE=.\TerrainPlantPaintOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainPlantPaintOperation.h
# End Source File
# Begin Source File

SOURCE=.\TerrainSmoothOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainSmoothOperation.h
# End Source File
# Begin Source File

SOURCE=.\TerrainStretchBrush.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainStretchBrush.h
# End Source File
# Begin Source File

SOURCE=.\TerrainStretchOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainStretchOperation.h
# End Source File
# End Group
# Begin Group "Undo"

# PROP Default_Filter ""
# Begin Group "Undo Terrain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UndoMapAction.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoMapAction.h
# End Source File
# Begin Source File

SOURCE=.\UndoMaskAction.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoMaskAction.h
# End Source File
# End Group
# Begin Group "Undo Scene Object"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BezierUndoAction.cpp
# End Source File
# Begin Source File

SOURCE=.\BezierUndoAction.h
# End Source File
# Begin Source File

SOURCE=.\UndoLightAddAction.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoLightAddAction.h
# End Source File
# Begin Source File

SOURCE=.\UndoLightDeleteAction.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoLightDeleteAction.h
# End Source File
# Begin Source File

SOURCE=.\UndoLightPropertyAction.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoLightPropertyAction.h
# End Source File
# Begin Source File

SOURCE=.\UndoObjectMoveAction.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoObjectMoveAction.h
# End Source File
# Begin Source File

SOURCE=.\UndoRotateAction.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoRotateAction.h
# End Source File
# Begin Source File

SOURCE=.\UndoScaleAction.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoScaleAction.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\UndoAction.h
# End Source File
# Begin Source File

SOURCE=.\UndoMan.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoMan.h
# End Source File
# End Group
# Begin Group "Scene Object Operations"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AIGeneratorOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\AIGeneratorOperation.h
# End Source File
# Begin Source File

SOURCE=.\AudioAddOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioAddOperation.h
# End Source File
# Begin Source File

SOURCE=.\BezierAddOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\BezierAddOperation.h
# End Source File
# Begin Source File

SOURCE=.\BezierDeletePointOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\BezierDeletePointOperation.h
# End Source File
# Begin Source File

SOURCE=.\SceneAreaAddOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneAreaAddOperation.h
# End Source File
# Begin Source File

SOURCE=.\SceneDummyAddOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneDummyAddOperation.h
# End Source File
# Begin Source File

SOURCE=.\SceneLightAddOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneLightAddOperation.h
# End Source File
# Begin Source File

SOURCE=.\SceneModelAddOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneModelAddOperation.h
# End Source File
# Begin Source File

SOURCE=.\SceneObjectMoveOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneObjectMoveOperation.h
# End Source File
# Begin Source File

SOURCE=.\SceneObjectMultiSelectOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneObjectMultiSelectOperation.h
# End Source File
# Begin Source File

SOURCE=.\SceneObjectRotateOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneObjectRotateOperation.h
# End Source File
# Begin Source File

SOURCE=.\SceneObjectScaleOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneObjectScaleOperation.h
# End Source File
# Begin Source File

SOURCE=.\SceneObjectSelectOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneObjectSelectOperation.h
# End Source File
# Begin Source File

SOURCE=.\SceneWaterAddOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneWaterAddOperation.h
# End Source File
# Begin Source File

SOURCE=.\SetVernierOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SetVernierOperation.h
# End Source File
# Begin Source File

SOURCE=.\SpeciallyAddOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\SpeciallyAddOperation.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Brush.cpp
# End Source File
# Begin Source File

SOURCE=.\Brush.h
# End Source File
# Begin Source File

SOURCE=.\Operation.h
# End Source File
# Begin Source File

SOURCE=.\OperationManager.cpp
# End Source File
# Begin Source File

SOURCE=.\OperationManager.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\res\crosshr.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\delete.ico
# End Source File
# Begin Source File

SOURCE=.\res\ElementEditor.ico
# End Source File
# Begin Source File

SOURCE=.\ElementEditor.rc
# End Source File
# Begin Source File

SOURCE=.\res\ElementEditor.rc2
# End Source File
# Begin Source File

SOURCE=.\res\hmToolBar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon4.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon5.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon6.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon7.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon8.ico
# End Source File
# Begin Source File

SOURCE=.\res\listtag.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MAGNIFY.CUR
# End Source File
# Begin Source File

SOURCE=.\res\movedown.ico
# End Source File
# Begin Source File

SOURCE=.\res\moveup.ico
# End Source File
# Begin Source File

SOURCE=.\res\NewLayer.ico
# End Source File
# Begin Source File

SOURCE=.\res\object_add.cur
# End Source File
# Begin Source File

SOURCE=.\res\Open.ico
# End Source File
# Begin Source File

SOURCE=.\res\plant_ad.ico
# End Source File
# Begin Source File

SOURCE=.\res\readonly.ico
# End Source File
# Begin Source File

SOURCE=.\res\SARROWS.CUR
# End Source File
# Begin Source File

SOURCE=.\res\save.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\writeable.ico
# End Source File
# End Group
# Begin Group "Dialog Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Common Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BackBufWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\BackBufWnd.h
# End Source File
# Begin Source File

SOURCE=.\CommonFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CommonFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\DlgBars.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBars.h
# End Source File
# Begin Source File

SOURCE=.\DlgSelectFile.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSelectFile.h
# End Source File
# Begin Source File

SOURCE=.\IconButton.cpp
# End Source File
# Begin Source File

SOURCE=.\IconButton.h
# End Source File
# Begin Source File

SOURCE=.\ImageWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageWnd.h
# End Source File
# Begin Source File

SOURCE=.\ItemInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemInfo.h
# End Source File
# Begin Source File

SOURCE=.\NumEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\NumEdit.h
# End Source File
# Begin Source File

SOURCE=.\RollUpPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\RollUpPanel.h
# End Source File
# Begin Source File

SOURCE=.\RollUpWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\RollUpWnd.h
# End Source File
# Begin Source File

SOURCE=.\SelColorWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\SelColorWnd.h
# End Source File
# Begin Source File

SOURCE=.\WndInput.cpp
# End Source File
# Begin Source File

SOURCE=.\WndInput.h
# End Source File
# End Group
# Begin Group "Terrain Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgHeightMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgHeightMap.h
# End Source File
# Begin Source File

SOURCE=.\DlgNewHeiMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgNewHeiMap.h
# End Source File
# Begin Source File

SOURCE=.\DlgSurfaces.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSurfaces.h
# End Source File
# Begin Source File

SOURCE=.\HeightMapView.cpp
# End Source File
# Begin Source File

SOURCE=.\HeightMapView.h
# End Source File
# Begin Source File

SOURCE=.\ModifiedProfileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifiedProfileDlg.h
# End Source File
# Begin Source File

SOURCE=.\TerrainBlockViewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainBlockViewDlg.h
# End Source File
# Begin Source File

SOURCE=.\TreeTypesAddDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeTypesAddDlg.h
# End Source File
# End Group
# Begin Group "Page - Terrain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CurveFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveFilter.h
# End Source File
# Begin Source File

SOURCE=.\TerrainBrushSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainBrushSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\TerrainModifyLayMaskPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainModifyLayMaskPanel.h
# End Source File
# Begin Source File

SOURCE=.\TerrainModifyLayMaskSmoothPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainModifyLayMaskSmoothPanel.h
# End Source File
# Begin Source File

SOURCE=.\TerrainModifyNoisePanel.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainModifyNoisePanel.h
# End Source File
# Begin Source File

SOURCE=.\TerrainModifyPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainModifyPanel.h
# End Source File
# Begin Source File

SOURCE=.\TerrainModifyPlantsPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainModifyPlantsPanel.h
# End Source File
# Begin Source File

SOURCE=.\TerrainModifySmoothPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainModifySmoothPanel.h
# End Source File
# Begin Source File

SOURCE=.\TerrainModifyStretchPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainModifyStretchPanel.h
# End Source File
# Begin Source File

SOURCE=.\TerrainPage.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainPage.h
# End Source File
# End Group
# Begin Group "Page - Options"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BrushSetPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\BrushSetPanel.h
# End Source File
# Begin Source File

SOURCE=.\DlgOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOptions.h
# End Source File
# Begin Source File

SOURCE=.\DlgRenderOpt.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRenderOpt.h
# End Source File
# Begin Source File

SOURCE=.\OptionsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsPage.h
# End Source File
# End Group
# Begin Group "Page - Objects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CloudSetPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\CloudSetPanel.h
# End Source File
# Begin Source File

SOURCE=.\ObjectLightPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectLightPanel.h
# End Source File
# Begin Source File

SOURCE=.\ObjectPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPage.h
# End Source File
# Begin Source File

SOURCE=.\ObjectPropertyPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectPropertyPanel.h
# End Source File
# Begin Source File

SOURCE=.\OptVernierDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptVernierDlg.h
# End Source File
# Begin Source File

SOURCE=.\SceneModelListPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneModelListPanel.h
# End Source File
# Begin Source File

SOURCE=.\SceneObjectPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneObjectPanel.h
# End Source File
# Begin Source File

SOURCE=.\SceneObjectsList.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneObjectsList.h
# End Source File
# Begin Source File

SOURCE=.\SkySetDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SkySetDialog.h
# End Source File
# End Group
# Begin Group "Properties Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Property\CustomColorDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\CustomColorDialog.h
# End Source File
# Begin Source File

SOURCE=.\Property\CustomVectorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\CustomVectorDlg.h
# End Source File
# Begin Source File

SOURCE=.\Property\MemoEditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\MemoEditDlg.h
# End Source File
# Begin Source File

SOURCE=.\Property\PageDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PageDialog.h
# End Source File
# Begin Source File

SOURCE=.\Property\PathTree.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PathTree.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyBooleanBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyBooleanBox.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyBox.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyColorBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyColorBox.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyCustomBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyCustomBox.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyDialog.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyFileBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyFileBox.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyFloatBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyFloatBox.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyFontBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyFontBox.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyIDBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyIDBox.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyIntegerBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyIntegerBox.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyList.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyList.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyMemoBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyMemoBox.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyStringBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyStringBox.h
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyVectorBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\PropertyVectorBox.h
# End Source File
# Begin Source File

SOURCE=.\Property\SelectPath.cpp
# End Source File
# Begin Source File

SOURCE=.\Property\SelectPath.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\BezierMergeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BezierMergeDlg.h
# End Source File
# Begin Source File

SOURCE=.\BezierSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BezierSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\CreateWorkspaceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateWorkspaceDlg.h
# End Source File
# Begin Source File

SOURCE=.\DelObjectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DelObjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\DlgAutoPartHM.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAutoPartHM.h
# End Source File
# Begin Source File

SOURCE=.\DlgCopySel.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCopySel.h
# End Source File
# Begin Source File

SOURCE=.\DlgExpGameData.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgExpGameData.h
# End Source File
# Begin Source File

SOURCE=.\DlgExportBHT.h
# End Source File
# Begin Source File

SOURCE=.\DlgExportPathInfo.h
# End Source File
# Begin Source File

SOURCE=.\DlgLighting.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLighting.h
# End Source File
# Begin Source File

SOURCE=.\DlgMapGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMapGrid.h
# End Source File
# Begin Source File

SOURCE=.\DlgMergePlayerPassMap.h
# End Source File
# Begin Source File

SOURCE=.\DlgNewMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgNewMap.h
# End Source File
# Begin Source File

SOURCE=.\DlgRandomMapInfoSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRandomMapInfoSetting.h
# End Source File
# Begin Source File

SOURCE=.\DlgSaveAs.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSaveAs.h
# End Source File
# Begin Source File

SOURCE=.\DlgSceneObjectFinder.h
# End Source File
# Begin Source File

SOURCE=.\EL_RandomMapInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\EL_RandomMapInfo.h
# End Source File
# Begin Source File

SOURCE=.\ExpGameDataDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExpGameDataDlg.h
# End Source File
# Begin Source File

SOURCE=.\ExportLitDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportLitDlg.h
# End Source File
# Begin Source File

SOURCE=.\ExpPathSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExpPathSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\FindMonsterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FindMonsterDlg.h
# End Source File
# Begin Source File

SOURCE=.\FixedNpcSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FixedNpcSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\GatherDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GatherDlg.h
# End Source File
# Begin Source File

SOURCE=.\GatherPropertyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GatherPropertyDlg.h
# End Source File
# Begin Source File

SOURCE=.\HullPropertyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HullPropertyDlg.h
# End Source File
# Begin Source File

SOURCE=.\InputPlantsNameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InputPlantsNameDlg.h
# End Source File
# Begin Source File

SOURCE=.\LogDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LogDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModelChangedStatusDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelChangedStatusDlg.h
# End Source File
# Begin Source File

SOURCE=.\MonsterFaction.cpp
# End Source File
# Begin Source File

SOURCE=.\MonsterFaction.h
# End Source File
# Begin Source File

SOURCE=.\MousterStatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MousterStatDlg.h
# End Source File
# Begin Source File

SOURCE=.\MousterStatFilterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MousterStatFilterDlg.h
# End Source File
# Begin Source File

SOURCE=.\MoveAllObjHeightDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveAllObjHeightDlg.h
# End Source File
# Begin Source File

SOURCE=.\MusicSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MusicSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\NatureDataSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NatureDataSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\NpcPropertyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NpcPropertyDlg.h
# End Source File
# Begin Source File

SOURCE=.\NpcSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NpcSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\PrecinctImpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PrecinctImpDlg.h
# End Source File
# Begin Source File

SOURCE=.\PrecinctNpcSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PrecinctNpcSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\ResFinderDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ResFinderDlg.h
# End Source File
# Begin Source File

SOURCE=.\ResManDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ResManDlg.h
# End Source File
# Begin Source File

SOURCE=.\TerrainConnectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainConnectDlg.h
# End Source File
# Begin Source File

SOURCE=.\TerrainResExpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TerrainResExpDlg.h
# End Source File
# Begin Source File

SOURCE=.\ToolTabWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolTabWnd.h
# End Source File
# Begin Source File

SOURCE=.\VssOptionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VssOptionDlg.h
# End Source File
# Begin Source File

SOURCE=.\WorkSpaceSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkSpaceSetDlg.h
# End Source File
# End Group
# Begin Group "Vss"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\VssDlgBar.cpp
# End Source File
# Begin Source File

SOURCE=.\VssDlgBar.h
# End Source File
# End Group
# Begin Group "Cloud Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EL_CloudCreator.cpp
# End Source File
# Begin Source File

SOURCE=.\EL_CloudCreator.h
# End Source File
# Begin Source File

SOURCE=.\EL_CloudSceneRender.cpp
# End Source File
# Begin Source File

SOURCE=.\EL_CloudSceneRender.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_CloudSprite.cpp
# End Source File
# End Group
# Begin Group "Dynamic Object"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DynamicObjectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicObjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\DynamicObjectManDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicObjectManDlg.h
# End Source File
# Begin Source File

SOURCE=.\DynamicObjectSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicObjectSelDlg.h
# End Source File
# End Group
# Begin Group "NPC Controller"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\NpcControllerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NpcControllerDlg.h
# End Source File
# Begin Source File

SOURCE=.\NpcControllerManDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NpcControllerManDlg.h
# End Source File
# Begin Source File

SOURCE=.\NpcControllerSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NpcControllerSelDlg.h
# End Source File
# End Group
# Begin Source File

SOURCE="..\..\..\Angelica sdk2.0\SDK\A3DSDK\Include\A3DSceneVisible.h"
# End Source File
# End Target
# End Project
