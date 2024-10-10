# Microsoft Developer Studio Project File - Name="ElementClient" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ElementClient - Win32 DebugLAAMemProfile
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ElementClient.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ElementClient.mak" CFG="ElementClient - Win32 DebugLAAMemProfile"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ElementClient - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ElementClient - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ElementClient - Win32 DbgRelease" (based on "Win32 (x86) Application")
!MESSAGE "ElementClient - Win32 DebugCRTAllocator" (based on "Win32 (x86) Application")
!MESSAGE "ElementClient - Win32 DbgReleaseLAA" (based on "Win32 (x86) Application")
!MESSAGE "ElementClient - Win32 DebugLAA" (based on "Win32 (x86) Application")
!MESSAGE "ElementClient - Win32 DbgReleaseLAALOG" (based on "Win32 (x86) Application")
!MESSAGE "ElementClient - Win32 DebugLAAMemProfile" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Element/ElementClient", AQDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ElementClient - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Include\stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /I ".\Home" /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\include\LuaWrapper" /I ".\CrossServer" /I "..\include\autopfimp" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /YX /FD /Zm150 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ArcAsiaSDK.lib psapi.lib zlibwapi.lib GfxCommon.lib CHBasedCD.lib ElementSkill.lib UnicoWS.lib kernel32.lib advapi32.lib user32.lib gdi32.lib shell32.lib comdlg32.lib version.lib mpr.lib rasapi32.lib winmm.lib winspool.lib vfw32.lib secur32.lib oleacc.lib oledlg.lib sensapi.lib AUInterface.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_u.lib AngelicaMedia_u.lib AngelicaFile_u.lib Angelica3D_u.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert.lib FTDriver.lib FWEditorLib.lib shlwapi.lib wininet.lib lua5.1.mt.lib LuaWrapper_u.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib asynbase_u.lib strmbase_u.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /nodefaultlib:"kernel32.lib" /nodefaultlib:"advapi32.lib" /nodefaultlib:"user32.lib" /nodefaultlib:"gdi32.lib" /nodefaultlib:"shell32.lib" /nodefaultlib:"comdlg32.lib" /nodefaultlib:"version.lib" /nodefaultlib:"mpr.lib" /nodefaultlib:"rasapi32.lib" /nodefaultlib:"winmm.lib" /nodefaultlib:"winspool.lib" /nodefaultlib:"vfw32.lib" /nodefaultlib:"secur32.lib" /nodefaultlib:"oleacc.lib" /nodefaultlib:"oledlg.lib" /nodefaultlib:"sensapi.lib" /out:"..\CBin\ElementClient.exe" /libpath:"..\lib\autopfimp" /libpath:"..\lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\AUI" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\AUInterface2\Lib" /libpath:"..\..\..\GfxCommon\Output\Lib" /libpath:"..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\lib\Win32" /libpath:"..\lib\stlport"
# SUBTRACT LINK32 /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=getcodeseg                                                                                                                                                                                                                                                                                                                                                                                             ..\CBin\ElementClient.exe                                                                                                                                                                                                                                                                                                                                                                                            ..\CBin\interfaces\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ElementClient - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "..\Include\stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /I ".\Home" /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\include\LuaWrapper" /I ".\CrossServer" /I "..\include\autopfimp" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /D "LOG_PROTOCOL" /FD /GZ /Zm150 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ArcAsiaSDK_d.lib version.lib psapi.lib zlibwapi.lib GfxCommon_d.lib CHBasedCD_d.lib ElementSkill_d.lib AUInterface_d.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_ud.lib AngelicaMedia_ud.lib AngelicaFile_ud.lib Angelica3D_ud.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib glu32.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert_d.lib FTDriver_D.lib FWEditorLib_d.lib shlwapi.lib wininet.lib glu32.lib lua5.1.mtd.lib LuaWrapper_du.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib asynbase_u.lib strmbase_u.lib dbghelp.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"..\CBin\ElementClient_d.exe" /pdbtype:sept /libpath:"..\lib\autopfimp" /libpath:"..\lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\AUI" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\AUInterface2\Lib" /libpath:"..\..\..\GfxCommon\Output\Lib" /libpath:"..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\lib\Win32" /libpath:"..\lib\stlport"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ElementClient - Win32 DbgRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ElementClient___Win32_DbgRelease"
# PROP BASE Intermediate_Dir "ElementClient___Win32_DbgRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgRelease"
# PROP Intermediate_Dir "DbgRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\..\Angelica2\Output\A3DSDK\Include" /I "..\..\Angelica2\Output\AFSDK\Include" /I "..\..\Angelica2\Output\ACSDK\Include" /I "..\..\Angelica2\Output\AMSDK\Include" /I "..\..\Angelica2\Output\3rdSDK\Include" /I "F:\DirectX 8.1 SDK\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I "..\Include\stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /I ".\Home" /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\include\LuaWrapper" /I ".\CrossServer" /I "..\include\autopfimp" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /FD /Zm150 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 zlibwapi.lib GfxCommon.lib CHBasedCD.lib ElementSkill.lib UnicoWS.lib kernel32.lib advapi32.lib user32.lib gdi32.lib shell32.lib comdlg32.lib version.lib mpr.lib rasapi32.lib winmm.lib winspool.lib vfw32.lib secur32.lib oleacc.lib oledlg.lib sensapi.lib AUInterface.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_u.lib AngelicaMedia_u.lib AngelicaFile_u.lib Angelica3D_u.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert.lib FTDriver.lib shlwapi.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /nodefaultlib:"kernel32.lib" /nodefaultlib:"advapi32.lib" /nodefaultlib:"user32.lib" /nodefaultlib:"gdi32.lib" /nodefaultlib:"shell32.lib" /nodefaultlib:"comdlg32.lib" /nodefaultlib:"version.lib" /nodefaultlib:"mpr.lib" /nodefaultlib:"rasapi32.lib" /nodefaultlib:"winmm.lib" /nodefaultlib:"winspool.lib" /nodefaultlib:"vfw32.lib" /nodefaultlib:"secur32.lib" /nodefaultlib:"oleacc.lib" /nodefaultlib:"oledlg.lib" /nodefaultlib:"sensapi.lib" /out:"..\Bin\ElementClient.exe" /libpath:"..\Lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport" /libpath:"..\Lib\AUI" /libpath:"..\..\Angelica2\Output\A3DSDK\Lib" /libpath:"..\..\Angelica2\Output\AFSDK\Lib" /libpath:"..\..\Angelica2\Output\ACSDK\Lib" /libpath:"..\..\Angelica2\Output\AMSDK\Lib" /libpath:"..\..\Angelica2\Output\3rdSDK\Lib" /libpath:"F:\DirectX 8.1 SDK\lib"
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 ArcAsiaSDK.lib psapi.lib zlibwapi.lib GfxCommon_su.lib CHBasedCD_su.lib ElementSkill.lib UnicoWS.lib kernel32.lib advapi32.lib user32.lib gdi32.lib shell32.lib comdlg32.lib version.lib mpr.lib rasapi32.lib winmm.lib winspool.lib vfw32.lib secur32.lib oleacc.lib oledlg.lib sensapi.lib AUInterface_su.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_dr.lib AngelicaMedia_dr.lib AngelicaFile_dr.lib Angelica3D_dr.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert.lib FTDriver.lib FWEditorLib.lib shlwapi.lib wininet.lib lua5.1.mt.lib LuaWrapper_u.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib asynbase_u.lib strmbase_u.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"kernel32.lib" /nodefaultlib:"advapi32.lib" /nodefaultlib:"user32.lib" /nodefaultlib:"gdi32.lib" /nodefaultlib:"shell32.lib" /nodefaultlib:"comdlg32.lib" /nodefaultlib:"version.lib" /nodefaultlib:"mpr.lib" /nodefaultlib:"rasapi32.lib" /nodefaultlib:"winmm.lib" /nodefaultlib:"winspool.lib" /nodefaultlib:"vfw32.lib" /nodefaultlib:"secur32.lib" /nodefaultlib:"oleacc.lib" /nodefaultlib:"oledlg.lib" /nodefaultlib:"sensapi.lib" /out:"..\CBin\ElementClient_dr.exe" /libpath:"..\lib\autopfimp" /libpath:"..\lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\AUI" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\AUInterface2\Lib" /libpath:"..\..\..\GfxCommon\Output\Lib" /libpath:"..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\lib\Win32" /libpath:"..\lib\stlport" /OPT:REF
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=getcodeseg                                                                                                                                                                                                                                                                                     ..\CBin\ElementClient_dr.exe                                                                                                                                                                                                                                                                                    ..\CBin\interfaces\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ElementClient - Win32 DebugCRTAllocator"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ElementClient___Win32_DebugCRTAllocator"
# PROP BASE Intermediate_Dir "ElementClient___Win32_DebugCRTAllocator"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugCRT"
# PROP Intermediate_Dir "DebugCRT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I ".\Home" /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\include\LuaWrapper" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /Fr /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "..\Include\stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /I ".\Home" /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\include\LuaWrapper" /I ".\CrossServer" /I "..\include\autopfimp" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "LOG_PROTOCOL" /Fr /FD /GZ /Zm150 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 zlibwapi.lib GfxCommon_d.lib CHBasedCD_d.lib ElementSkill_d.lib AUInterface_d.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_ud.lib AngelicaMedia_ud.lib AngelicaFile_ud.lib Angelica3D_ud.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib glu32.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert_d.lib FTDriver_D.lib FWEditorLib_d.lib shlwapi.lib wininet.lib glu32.lib lua5.1.mtd.lib LuaWrapper_du.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib asynbase_u.lib strmbase_u.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"..\CBin\ElementClient_d.exe" /pdbtype:sept /libpath:"..\lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport" /libpath:"..\Lib\AUI"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 LogicCheck.lib ArcAsiaSDK.lib version.lib psapi.lib zlibwapi.lib GfxCommon_smd.lib CHBasedCD_d.lib ElementSkill_d.lib AUInterface_smd.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_sud.lib AngelicaMedia_sud.lib AngelicaFile_sud.lib Angelica3D_sud.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib glu32.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert_d.lib FTDriver_D.lib FWEditorLib_d.lib shlwapi.lib wininet.lib glu32.lib lua5.1.mtd.lib LuaWrapper_du_CRT.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib asynbase_u.lib strmbase_u.lib dbghelp.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:no /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"..\CBin\ElementClient_d_CRT.exe" /pdbtype:sept /libpath:"..\lib\autopfimp" /libpath:"..\lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\AUI" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\AUInterface2\Lib" /libpath:"..\..\..\GfxCommon\Output\Lib" /libpath:"..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\lib\Win32" /libpath:"..\lib\stlport"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ElementClient - Win32 DbgReleaseLAA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ElementClient___Win32_DbgReleaseLAA"
# PROP BASE Intermediate_Dir "ElementClient___Win32_DbgReleaseLAA"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgReleaseLAA"
# PROP Intermediate_Dir "DbgReleaseLAA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I ".\Home" /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\include\LuaWrapper" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /FD /Zm150 /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I "..\Include\stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /I ".\Home" /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\include\LuaWrapper" /I ".\CrossServer" /I "..\include\autopfimp" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /FD /Zm150 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 zlibwapi.lib GfxCommon_su.lib CHBasedCD_su.lib ElementSkill.lib UnicoWS.lib kernel32.lib advapi32.lib user32.lib gdi32.lib shell32.lib comdlg32.lib version.lib mpr.lib rasapi32.lib winmm.lib winspool.lib vfw32.lib secur32.lib oleacc.lib oledlg.lib sensapi.lib AUInterface_su.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_dr.lib AngelicaMedia_dr.lib AngelicaFile_dr.lib Angelica3D_dr.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert.lib FTDriver.lib FWEditorLib.lib shlwapi.lib wininet.lib lua5.1.mt.lib LuaWrapper_u.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib asynbase_u.lib strmbase_u.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"kernel32.lib" /nodefaultlib:"advapi32.lib" /nodefaultlib:"user32.lib" /nodefaultlib:"gdi32.lib" /nodefaultlib:"shell32.lib" /nodefaultlib:"comdlg32.lib" /nodefaultlib:"version.lib" /nodefaultlib:"mpr.lib" /nodefaultlib:"rasapi32.lib" /nodefaultlib:"winmm.lib" /nodefaultlib:"winspool.lib" /nodefaultlib:"vfw32.lib" /nodefaultlib:"secur32.lib" /nodefaultlib:"oleacc.lib" /nodefaultlib:"oledlg.lib" /nodefaultlib:"sensapi.lib" /out:"..\CBin\ElementClient_law.exe" /libpath:"..\lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport" /libpath:"..\Lib\AUI" /OPT:REF /LARGEADDRESSAWARE
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 LogicCheck.lib ArcAsiaSDK.lib psapi.lib zlibwapi.lib GfxCommon_su.lib CHBasedCD_su.lib ElementSkill.lib UnicoWS.lib kernel32.lib advapi32.lib user32.lib gdi32.lib shell32.lib comdlg32.lib version.lib mpr.lib rasapi32.lib winmm.lib winspool.lib vfw32.lib secur32.lib oleacc.lib oledlg.lib sensapi.lib AUInterface_su.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_dr.lib AngelicaMedia_dr.lib AngelicaFile_dr.lib Angelica3D_dr.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert.lib FTDriver.lib FWEditorLib.lib shlwapi.lib wininet.lib lua5.1.mt.lib LuaWrapper_u.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib asynbase_u.lib strmbase_u.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"kernel32.lib" /nodefaultlib:"advapi32.lib" /nodefaultlib:"user32.lib" /nodefaultlib:"gdi32.lib" /nodefaultlib:"shell32.lib" /nodefaultlib:"comdlg32.lib" /nodefaultlib:"version.lib" /nodefaultlib:"mpr.lib" /nodefaultlib:"rasapi32.lib" /nodefaultlib:"winmm.lib" /nodefaultlib:"winspool.lib" /nodefaultlib:"vfw32.lib" /nodefaultlib:"secur32.lib" /nodefaultlib:"oleacc.lib" /nodefaultlib:"oledlg.lib" /nodefaultlib:"sensapi.lib" /out:"..\CBin\ElementClient_laa.exe" /libpath:"..\lib\autopfimp" /libpath:"..\lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\AUI" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\AUInterface2\Lib" /libpath:"..\..\..\GfxCommon\Output\Lib" /libpath:"..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\lib\Win32" /libpath:"..\lib\stlport" /OPT:REF /LARGEADDRESSAWARE
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=..\CBin\getcodeseg                                                                                                                                                                                                                                                                                     ..\CBin\ElementClient_laa.exe                                                                                                                                                                                                                                                                                    ..\CBin\interfaces\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ElementClient - Win32 DebugLAA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ElementClient___Win32_DebugLAA"
# PROP BASE Intermediate_Dir "ElementClient___Win32_DebugLAA"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugLAA"
# PROP Intermediate_Dir "DebugLAA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I ".\Home" /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\include\LuaWrapper" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /FD /GZ /Zm150 /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "..\Include\stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /I ".\Home" /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\include\LuaWrapper" /I ".\CrossServer" /I "..\include\autopfimp" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /D "LOG_PROTOCOL" /FD /GZ /Zm150 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 zlibwapi.lib GfxCommon_d.lib CHBasedCD_d.lib ElementSkill_d.lib AUInterface_d.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_ud.lib AngelicaMedia_ud.lib AngelicaFile_ud.lib Angelica3D_ud.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib glu32.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert_d.lib FTDriver_D.lib FWEditorLib_d.lib shlwapi.lib wininet.lib glu32.lib lua5.1.mtd.lib LuaWrapper_du.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib asynbase_u.lib strmbase_u.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"..\CBin\ElementClient_d.exe" /pdbtype:sept /libpath:"..\lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport" /libpath:"..\Lib\AUI"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 LogicCheck.lib ArcAsiaSDK_d.lib version.lib psapi.lib zlibwapi.lib GfxCommon_d.lib CHBasedCD_d.lib ElementSkill_d.lib AUInterface_d.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_ud.lib AngelicaMedia_ud.lib AngelicaFile_ud.lib Angelica3D_ud.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib glu32.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert_d.lib FTDriver_D.lib FWEditorLib_d.lib shlwapi.lib wininet.lib glu32.lib lua5.1.mtd.lib LuaWrapper_du.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib asynbase_u.lib strmbase_u.lib dbghelp.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:no /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"..\CBin\ElementClient_d_laa.exe" /pdbtype:sept /libpath:"..\lib\autopfimp" /libpath:"..\lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\AUI" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\AUInterface2\Lib" /libpath:"..\..\..\GfxCommon\Output\Lib" /libpath:"..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\lib\Win32" /libpath:"..\lib\stlport" /LARGEADDRESSAWARE
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ElementClient - Win32 DbgReleaseLAALOG"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ElementClient___Win32_DbgReleaseLAALOG"
# PROP BASE Intermediate_Dir "ElementClient___Win32_DbgReleaseLAALOG"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgReleaseLAALOG"
# PROP Intermediate_Dir "DbgReleaseLAALOG"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I ".\Home" /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\include\LuaWrapper" /I ".\CrossServer" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /FD /Zm150 /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I "..\Include\stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /I ".\Home" /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\include\LuaWrapper" /I ".\CrossServer" /I "..\include\autopfimp" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /D "LOG_PROTOCOL" /FD /Zm150 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 zlibwapi.lib GfxCommon_su.lib CHBasedCD_su.lib ElementSkill.lib UnicoWS.lib kernel32.lib advapi32.lib user32.lib gdi32.lib shell32.lib comdlg32.lib version.lib mpr.lib rasapi32.lib winmm.lib winspool.lib vfw32.lib secur32.lib oleacc.lib oledlg.lib sensapi.lib AUInterface_su.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_dr.lib AngelicaMedia_dr.lib AngelicaFile_dr.lib Angelica3D_dr.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert.lib FTDriver.lib FWEditorLib.lib shlwapi.lib wininet.lib lua5.1.mt.lib LuaWrapper_u.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib asynbase_u.lib strmbase_u.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"kernel32.lib" /nodefaultlib:"advapi32.lib" /nodefaultlib:"user32.lib" /nodefaultlib:"gdi32.lib" /nodefaultlib:"shell32.lib" /nodefaultlib:"comdlg32.lib" /nodefaultlib:"version.lib" /nodefaultlib:"mpr.lib" /nodefaultlib:"rasapi32.lib" /nodefaultlib:"winmm.lib" /nodefaultlib:"winspool.lib" /nodefaultlib:"vfw32.lib" /nodefaultlib:"secur32.lib" /nodefaultlib:"oleacc.lib" /nodefaultlib:"oledlg.lib" /nodefaultlib:"sensapi.lib" /out:"..\CBin\ElementClient_laa.exe" /libpath:"..\lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport" /libpath:"..\Lib\AUI" /OPT:REF /LARGEADDRESSAWARE
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 LogicCheck.lib ArcAsiaSDK.lib psapi.lib zlibwapi.lib GfxCommon_su.lib CHBasedCD_su.lib ElementSkill.lib UnicoWS.lib kernel32.lib advapi32.lib user32.lib gdi32.lib shell32.lib comdlg32.lib version.lib mpr.lib rasapi32.lib winmm.lib winspool.lib vfw32.lib secur32.lib oleacc.lib oledlg.lib sensapi.lib AUInterface_su.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_dr.lib AngelicaMedia_dr.lib AngelicaFile_dr.lib Angelica3D_dr.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert.lib FTDriver.lib FWEditorLib.lib shlwapi.lib wininet.lib lua5.1.mt.lib LuaWrapper_u.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib asynbase_u.lib strmbase_u.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"kernel32.lib" /nodefaultlib:"advapi32.lib" /nodefaultlib:"user32.lib" /nodefaultlib:"gdi32.lib" /nodefaultlib:"shell32.lib" /nodefaultlib:"comdlg32.lib" /nodefaultlib:"version.lib" /nodefaultlib:"mpr.lib" /nodefaultlib:"rasapi32.lib" /nodefaultlib:"winmm.lib" /nodefaultlib:"winspool.lib" /nodefaultlib:"vfw32.lib" /nodefaultlib:"secur32.lib" /nodefaultlib:"oleacc.lib" /nodefaultlib:"oledlg.lib" /nodefaultlib:"sensapi.lib" /out:"..\CBin\ElementClient_laa_log.exe" /libpath:"..\lib\autopfimp" /libpath:"..\lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\AUI" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\AUInterface2\Lib" /libpath:"..\..\..\GfxCommon\Output\Lib" /libpath:"..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\lib\Win32" /libpath:"..\lib\stlport" /OPT:REF /LARGEADDRESSAWARE
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ElementClient - Win32 DebugLAAMemProfile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ElementClient___Win32_DebugLAAMemProfile"
# PROP BASE Intermediate_Dir "ElementClient___Win32_DebugLAAMemProfile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugLAAMemProfile"
# PROP Intermediate_Dir "DebugLAAMemProfile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "..\include\autopfimp" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\..\..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /I ".\Home" /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\include\LuaWrapper" /I ".\CrossServer" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /D "LOG_PROTOCOL" /FD /GZ /Zm150 /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "..\Include\stlport" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /I "..\..\..\SDK\Apsdk\Include" /I "..\..\..\GfxCommon\Output\include" /I "..\AUInterface2" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Foundation\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Cooking\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\Physics\include" /I "..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\PhysXLoader\include" /I "..\include\stlport" /I ".\Home" /I "..\CCommon\Autoca" /I "..\CCommon\Face" /I ".\\" /I ".\Network\rpcdata" /I ".\Network\inl" /I ".\Network\IOLib" /I ".\Network" /I ".\Task" /I "..\include\CD" /I "..\Include" /I "..\CCommon" /I "..\CCommon\Gfx" /I "..\include\LuaWrapper" /I ".\CrossServer" /I "..\include\autopfimp" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_ELEMENTCLIENT" /D "LOG_PROTOCOL" /D "_PROFILE_MEMORY" /FD /GZ /Zm150 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 LogicCheck.lib ArcAsiaSDK_d.lib version.lib psapi.lib zlibwapi.lib GfxCommon_d.lib CHBasedCD_d.lib ElementSkill_d.lib AUInterface_d.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_ud.lib AngelicaMedia_ud.lib AngelicaFile_ud.lib Angelica3D_ud.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib glu32.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert_d.lib FTDriver_D.lib FWEditorLib_d.lib shlwapi.lib wininet.lib glu32.lib lua5.1.mtd.lib LuaWrapper_du.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib asynbase_u.lib strmbase_u.lib dbghelp.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:no /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"..\CBin\ElementClient_d_laa.exe" /pdbtype:sept /libpath:"..\lib\autopfimp" /libpath:"..\lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\AUI" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\..\..\AUInterface2\Lib" /libpath:"..\..\..\GfxCommon\Output\Lib" /libpath:"..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\lib\Win32" /libpath:"..\lib\stlport" /LARGEADDRESSAWARE
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 LogicCheck.lib ArcAsiaSDK_d.lib version.lib psapi.lib zlibwapi.lib GfxCommon_d.lib CHBasedCD_d.lib ElementSkill_d.lib AUInterface_d.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_ud.lib AngelicaMedia_ud.lib AngelicaFile_ud.lib Angelica3D_ud.lib ws2_32.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib glu32.lib ddraw.lib dsound.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib speedtreert_d.lib FTDriver_D.lib FWEditorLib_d.lib shlwapi.lib wininet.lib glu32.lib lua5.1.mtd.lib LuaWrapper_du.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib asynbase_u.lib strmbase_u.lib dbghelp.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:no /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"..\CBin\ElementClient_d_laa_mem.exe" /pdbtype:sept /libpath:"..\lib\autopfimp" /libpath:"..\lib\CD" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\AUI" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\AUInterface2\Lib" /libpath:"..\..\..\GfxCommon\Output\Lib" /libpath:"..\..\..\APhysXIntegration\PhysX_SDK\v2.8.1\SDKs\lib\Win32" /libpath:"..\lib\stlport" /LARGEADDRESSAWARE
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ElementClient - Win32 Release"
# Name "ElementClient - Win32 Debug"
# Name "ElementClient - Win32 DbgRelease"
# Name "ElementClient - Win32 DebugCRTAllocator"
# Name "ElementClient - Win32 DbgReleaseLAA"
# Name "ElementClient - Win32 DebugLAA"
# Name "ElementClient - Win32 DbgReleaseLAALOG"
# Name "ElementClient - Win32 DebugLAAMemProfile"
# Begin Group "Main Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Input Control"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_GhostInputFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_GhostInputFilter.h
# End Source File
# Begin Source File

SOURCE=.\EC_HostInputFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HostInputFilter.h
# End Source File
# Begin Source File

SOURCE=.\EC_InputCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_InputCtrl.h
# End Source File
# Begin Source File

SOURCE=.\EC_InputFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_InputFilter.h
# End Source File
# End Group
# Begin Group "Utilities"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AMiniDump.cpp
# End Source File
# Begin Source File

SOURCE=.\AMiniDump.h
# End Source File
# Begin Source File

SOURCE=.\auto_delete.h
# End Source File
# Begin Source File

SOURCE=.\base64.cpp
# End Source File
# Begin Source File

SOURCE=.\base64.h
# End Source File
# Begin Source File

SOURCE=.\DataPathMan.cpp
# End Source File
# Begin Source File

SOURCE=.\DataPathMan.h
# End Source File
# Begin Source File

SOURCE=.\EC_AssureMove.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_AssureMove.h
# End Source File
# Begin Source File

SOURCE=.\EC_CameraCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_CameraCtrl.h
# End Source File
# Begin Source File

SOURCE=.\EC_CDR.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_CDR.h
# End Source File
# Begin Source File

SOURCE=.\EC_CDS.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_CDS.h
# End Source File
# Begin Source File

SOURCE=.\EC_Clipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Clipboard.h
# End Source File
# Begin Source File

SOURCE=.\EC_ClockIcon.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ClockIcon.h
# End Source File
# Begin Source File

SOURCE=.\EC_CommandLine.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_CommandLine.h
# End Source File
# Begin Source File

SOURCE=.\EC_ComputerAid.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ComputerAid.h
# End Source File
# Begin Source File

SOURCE=.\EC_Configs.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Configs.h
# End Source File
# Begin Source File

SOURCE=.\EC_ContinueLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ContinueLoad.h
# End Source File
# Begin Source File

SOURCE=.\EC_Counter.h
# End Source File
# Begin Source File

SOURCE=.\EC_CountryConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_CountryConfig.h
# End Source File
# Begin Source File

SOURCE=.\EC_Ease.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Ease.h
# End Source File
# Begin Source File

SOURCE=.\EC_ElementDataHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ElementDataHelper.h
# End Source File
# Begin Source File

SOURCE=.\EC_FullGlowRender.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_FullGlowRender.h
# End Source File
# Begin Source File

SOURCE=.\EC_GetHostByName.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_GetHostByName.h
# End Source File
# Begin Source File

SOURCE=.\EC_GFXCaster.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_GFXCaster.h
# End Source File
# Begin Source File

SOURCE=.\EC_ImageDrawing.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ImageDrawing.h
# End Source File
# Begin Source File

SOURCE=.\EC_ImageRes.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ImageRes.h
# End Source File
# Begin Source File

SOURCE=.\EC_LoadProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_LoadProgress.h
# End Source File
# Begin Source File

SOURCE=.\EC_MCDownload.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_MCDownload.h
# End Source File
# Begin Source File

SOURCE=.\EC_MemSimplify.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_MemSimplify.h
# End Source File
# Begin Source File

SOURCE=.\EC_PateText.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_PateText.h
# End Source File
# Begin Source File

SOURCE=.\EC_PendingAction.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_PendingAction.h
# End Source File
# Begin Source File

SOURCE=.\EC_PortraitRender.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_PortraitRender.h
# End Source File
# Begin Source File

SOURCE=.\EC_Profile.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Profile.h
# End Source File
# Begin Source File

SOURCE=.\EC_Resource.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Resource.h
# End Source File
# Begin Source File

SOURCE=.\EC_RewuShader.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_RewuShader.h
# End Source File
# Begin Source File

SOURCE=.\EC_RTDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_RTDebug.h
# End Source File
# Begin Source File

SOURCE=.\EC_SceneCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_SceneCheck.h
# End Source File
# Begin Source File

SOURCE=.\EC_ScreenEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ScreenEffect.h
# End Source File
# Begin Source File

SOURCE=.\EC_ShadowRender.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ShadowRender.h
# End Source File
# Begin Source File

SOURCE=.\EC_ShakeCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ShakeCamera.h
# End Source File
# Begin Source File

SOURCE=.\EC_SoundCache.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_SoundCache.h
# End Source File
# Begin Source File

SOURCE=.\EC_Split.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Split.h
# End Source File
# Begin Source File

SOURCE=.\EC_StringTab.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_StringTab.h
# End Source File
# Begin Source File

SOURCE=.\EC_Time.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Time.h
# End Source File
# Begin Source File

SOURCE=.\EC_TimeSafeChecker.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_TimeSafeChecker.h
# End Source File
# Begin Source File

SOURCE=.\EC_Utility.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Utility.h
# End Source File
# Begin Source File

SOURCE=.\EC_WinAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_WinAPI.h
# End Source File
# End Group
# Begin Group "World"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_Instance.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Instance.h
# End Source File
# Begin Source File

SOURCE=.\EC_RandomMapPreProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_RandomMapPreProcessor.h
# End Source File
# Begin Source File

SOURCE=.\EC_Scene.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Scene.h
# End Source File
# Begin Source File

SOURCE=.\EC_SceneBlock.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_SceneBlock.h
# End Source File
# Begin Source File

SOURCE=.\EC_SceneLights.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_SceneLights.h
# End Source File
# Begin Source File

SOURCE=.\EC_SceneLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_SceneLoader.h
# End Source File
# Begin Source File

SOURCE=.\EC_World.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_World.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EC_WorldFile.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\EC_FixedMsg.h
# End Source File
# Begin Source File

SOURCE=.\EC_Game.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Game.h
# End Source File
# Begin Source File

SOURCE=.\EC_GameRun.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_GameRun.h
# End Source File
# Begin Source File

SOURCE=.\EC_Global.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Global.h
# End Source File
# Begin Source File

SOURCE=.\EC_MsgDataDef.h
# End Source File
# Begin Source File

SOURCE=.\EC_MsgDef.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_MsgDef.h
# End Source File
# Begin Source File

SOURCE=.\EC_Viewport.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Viewport.h
# End Source File
# Begin Source File

SOURCE=.\ElementClient.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementClient.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Object Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Players"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_ActionSwitcher.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ActionSwitcher.h
# End Source File
# Begin Source File

SOURCE=.\EC_AutoTeam.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_AutoTeam.h
# End Source File
# Begin Source File

SOURCE=.\EC_CastSkillWhenMove.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_CastSkillWhenMove.h
# End Source File
# Begin Source File

SOURCE=.\EC_ChangePill.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ChangePill.h
# End Source File
# Begin Source File

SOURCE=.\EC_ComboSkill.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ComboSkill.h
# End Source File
# Begin Source File

SOURCE=.\EC_ElsePlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ElsePlayer.h
# End Source File
# Begin Source File

SOURCE=.\EC_EPWork.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_EPWork.h
# End Source File
# Begin Source File

SOURCE=.\EC_Faction.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Faction.h
# End Source File
# Begin Source File

SOURCE=.\EC_FashionModel.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_FashionModel.h
# End Source File
# Begin Source File

SOURCE=.\EC_ForceMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ForceMgr.h
# End Source File
# Begin Source File

SOURCE=.\EC_Friend.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Friend.h
# End Source File
# Begin Source File

SOURCE=.\EC_HostCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HostMove.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HostMove.h
# End Source File
# Begin Source File

SOURCE=.\EC_HostMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HostPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HostPlayer.h
# End Source File
# Begin Source File

SOURCE=.\EC_HPWork.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HPWork.h
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkDead.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkDead.h
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkFall.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkFall.h
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkFly.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkFly.h
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkFollow.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkFollow.h
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkForceNavigate.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkForceNavigate.h
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkMelee.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkMelee.h
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkMove.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkMove.h
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkSit.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkSit.h
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkSpell.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkSpell.h
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkStand.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkStand.h
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkTrace.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkTrace.h
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkUse.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HPWorkUse.h
# End Source File
# Begin Source File

SOURCE=.\EC_InstanceReenter.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_InstanceReenter.h
# End Source File
# Begin Source File

SOURCE=.\EC_IntelligentRoute.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IntelligentRoute.h
# End Source File
# Begin Source File

SOURCE=.\EC_IntelligentRouteUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IntelligentRouteUtil.h
# End Source File
# Begin Source File

SOURCE=.\EC_LoginPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_LoginPlayer.h
# End Source File
# Begin Source File

SOURCE=.\EC_PetCorral.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_PetCorral.h
# End Source File
# Begin Source File

SOURCE=.\EC_Player.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Player.h
# End Source File
# Begin Source File

SOURCE=.\EC_PlayerActionController.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_PlayerActionController.h
# End Source File
# Begin Source File

SOURCE=.\EC_PlayerBodyController.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_PlayerBodyController.h
# End Source File
# Begin Source File

SOURCE=.\EC_PlayerClone.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_PlayerClone.h
# End Source File
# Begin Source File

SOURCE=.\EC_RandMallShopping.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_RandMallShopping.h
# End Source File
# Begin Source File

SOURCE=.\EC_RoleTypes.h
# End Source File
# Begin Source File

SOURCE=.\EC_Team.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Team.h
# End Source File
# Begin Source File

SOURCE=.\EC_TeamMan.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_TeamMan.h
# End Source File
# End Group
# Begin Group "NPCs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_Monster.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Monster.h
# End Source File
# Begin Source File

SOURCE=.\EC_NPC.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_NPC.h
# End Source File
# Begin Source File

SOURCE=.\EC_NPCModel.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_NPCModel.h
# End Source File
# Begin Source File

SOURCE=.\EC_NPCServer.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_NPCServer.h
# End Source File
# Begin Source File

SOURCE=.\EC_Pet.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Pet.h
# End Source File
# Begin Source File

SOURCE=.\EC_PetWords.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_PetWords.h
# End Source File
# End Group
# Begin Group "Scene Objects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CCommon\EC_Bezier.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EC_Bezier.h
# End Source File
# Begin Source File

SOURCE=.\EC_CritterGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_CritterGroup.h
# End Source File
# Begin Source File

SOURCE=.\EC_GrassArea.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_GrassArea.h
# End Source File
# Begin Source File

SOURCE=.\EC_HomeOrnament.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HomeOrnament.h
# End Source File
# Begin Source File

SOURCE=.\EC_Ornament.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Ornament.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EC_SceneObject.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EC_SceneThreadObj.h
# End Source File
# Begin Source File

SOURCE=.\EC_ScnBoxArea.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ScnBoxArea.h
# End Source File
# End Group
# Begin Group "Inventory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_DealInventory.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_DealInventory.h
# End Source File
# Begin Source File

SOURCE=.\EC_Inventory.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Inventory.h
# End Source File
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

SOURCE=.\EC_IvtrTypes.h
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrWeapon.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_IvtrWeapon.h
# End Source File
# Begin Source File

SOURCE=.\EC_ShoppingItemsMover.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ShoppingItemsMover.h
# End Source File
# End Group
# Begin Group "Shortcut"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_Shortcut.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Shortcut.h
# End Source File
# Begin Source File

SOURCE=.\EC_ShortcutSet.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ShortcutSet.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\EC_Decal.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Decal.h
# End Source File
# Begin Source File

SOURCE=.\EC_Face.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Face.h
# End Source File
# Begin Source File

SOURCE=.\EC_Goblin.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Goblin.h
# End Source File
# Begin Source File

SOURCE=.\EC_HostGoblin.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HostGoblin.h
# End Source File
# Begin Source File

SOURCE=.\EC_LogicHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_LogicHelper.h
# End Source File
# Begin Source File

SOURCE=.\EC_Matter.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Matter.h
# End Source File
# Begin Source File

SOURCE=.\EC_Object.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Object.h
# End Source File
# Begin Source File

SOURCE=.\EC_ObjectWork.h
# End Source File
# Begin Source File

SOURCE=.\EC_PolicyAction.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_PolicyAction.h
# End Source File
# Begin Source File

SOURCE=.\EC_Shop.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Shop.h
# End Source File
# Begin Source File

SOURCE=.\EC_ShopSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ShopSearch.h
# End Source File
# Begin Source File

SOURCE=.\EC_Skill.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Skill.h
# End Source File
# Begin Source File

SOURCE=.\EC_Sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Sprite.h
# End Source File
# Begin Source File

SOURCE=.\EC_SunMoon.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_SunMoon.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# End Group
# Begin Group "Interface Files"

# PROP Default_Filter ""
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgAction.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAction.h
# End Source File
# Begin Source File

SOURCE=.\DlgActivity.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgActivity.h
# End Source File
# Begin Source File

SOURCE=.\DlgArrangeTeam.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgArrangeTeam.h
# End Source File
# Begin Source File

SOURCE=.\DlgAskHelpToGM.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAskHelpToGM.h
# End Source File
# Begin Source File

SOURCE=.\DlgAskHelpToGM2.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAskHelpToGM2.h
# End Source File
# Begin Source File

SOURCE=.\DlgAuctionBase.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAuctionBase.h
# End Source File
# Begin Source File

SOURCE=.\DlgAuctionBuyList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAuctionBuyList.h
# End Source File
# Begin Source File

SOURCE=.\DlgAuctionList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAuctionList.h
# End Source File
# Begin Source File

SOURCE=.\DlgAuctionSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAuctionSearch.h
# End Source File
# Begin Source File

SOURCE=.\DlgAuctionSellList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAuctionSellList.h
# End Source File
# Begin Source File

SOURCE=.\DlgAutoHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAutoHelp.h
# End Source File
# Begin Source File

SOURCE=.\DlgAutoHPMP.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAutoHPMP.h
# End Source File
# Begin Source File

SOURCE=.\DlgAutoLock.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAutoLock.h
# End Source File
# Begin Source File

SOURCE=.\DlgAutoMove.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAutoMove.h
# End Source File
# Begin Source File

SOURCE=.\DlgAutoPolicy.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAutoPolicy.h
# End Source File
# Begin Source File

SOURCE=.\DlgAutoTask.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAutoTask.h
# End Source File
# Begin Source File

SOURCE=.\DlgAward.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAward.h
# End Source File
# Begin Source File

SOURCE=.\DlgBackHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBackHelp.h
# End Source File
# Begin Source File

SOURCE=.\DlgBackOthers.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBackOthers.h
# End Source File
# Begin Source File

SOURCE=.\DlgBackSelf.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBackSelf.h
# End Source File
# Begin Source File

SOURCE=.\DlgBag.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBag.h
# End Source File
# Begin Source File

SOURCE=.\DlgBase.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBase.h
# End Source File
# Begin Source File

SOURCE=.\DlgBBS.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBBS.h
# End Source File
# Begin Source File

SOURCE=.\DlgBlackList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBlackList.h
# End Source File
# Begin Source File

SOURCE=.\DlgBlackScreen.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBlackScreen.h
# End Source File
# Begin Source File

SOURCE=.\DlgBooth.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBooth.h
# End Source File
# Begin Source File

SOURCE=.\DlgBroadCast.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBroadCast.h
# End Source File
# Begin Source File

SOURCE=.\DlgBShop.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBShop.h
# End Source File
# Begin Source File

SOURCE=.\DlgBuddyState.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBuddyState.h
# End Source File
# Begin Source File

SOURCE=.\DlgBuyConfirm.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBuyConfirm.h
# End Source File
# Begin Source File

SOURCE=.\DlgCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCamera.h
# End Source File
# Begin Source File

SOURCE=.\DlgChangeSkirt.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgChangeSkirt.h
# End Source File
# Begin Source File

SOURCE=.\DlgChannelChat.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgChannelChat.h
# End Source File
# Begin Source File

SOURCE=.\DlgChannelCreate.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgChannelCreate.h
# End Source File
# Begin Source File

SOURCE=.\DlgChannelJoin.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgChannelJoin.h
# End Source File
# Begin Source File

SOURCE=.\DlgChannelOption.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgChannelOption.h
# End Source File
# Begin Source File

SOURCE=.\DlgChannelPW.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgChannelPW.h
# End Source File
# Begin Source File

SOURCE=.\DlgCharacter.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCharacter.h
# End Source File
# Begin Source File

SOURCE=.\DlgChariot.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgChariot.h
# End Source File
# Begin Source File

SOURCE=.\DlgChat.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgChat.h
# End Source File
# Begin Source File

SOURCE=.\DlgComboSkillShortCut.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgComboSkillShortCut.h
# End Source File
# Begin Source File

SOURCE=.\DlgComplexTitle.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgComplexTitle.h
# End Source File
# Begin Source File

SOURCE=.\DlgCongregate.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCongregate.h
# End Source File
# Begin Source File

SOURCE=.\DlgConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgConsole.h
# End Source File
# Begin Source File

SOURCE=.\DlgCountryMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCountryMap.h
# End Source File
# Begin Source File

SOURCE=.\DlgCountryMapSub.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCountryMapSub.h
# End Source File
# Begin Source File

SOURCE=.\DlgCountryReward.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCountryReward.h
# End Source File
# Begin Source File

SOURCE=.\DlgCountryScore.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCountryScore.h
# End Source File
# Begin Source File

SOURCE=.\DlgCountryWarMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCountryWarMap.h
# End Source File
# Begin Source File

SOURCE=.\DlgCoupon.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCoupon.h
# End Source File
# Begin Source File

SOURCE=.\DlgCWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCWindow.h
# End Source File
# Begin Source File

SOURCE=.\DlgDamageRep.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDamageRep.h
# End Source File
# Begin Source File

SOURCE=.\DlgDomainCondition.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDomainCondition.h
# End Source File
# Begin Source File

SOURCE=.\DlgDoubleExp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDoubleExp.h
# End Source File
# Begin Source File

SOURCE=.\DlgDoubleExpS.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDoubleExpS.h
# End Source File
# Begin Source File

SOURCE=.\DlgDownloadSpeed.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDownloadSpeed.h
# End Source File
# Begin Source File

SOURCE=.\DlgDragDrop.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDragDrop.h
# End Source File
# Begin Source File

SOURCE=.\DlgEarthBagRank.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEarthBagRank.h
# End Source File
# Begin Source File

SOURCE=.\DlgELFEquipRemove.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgELFEquipRemove.h
# End Source File
# Begin Source File

SOURCE=.\DlgELFGenius.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgELFGenius.h
# End Source File
# Begin Source File

SOURCE=.\DlgELFGeniusReset.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgELFGeniusReset.h
# End Source File
# Begin Source File

SOURCE=.\DlgELFInExp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgELFInExp.h
# End Source File
# Begin Source File

SOURCE=.\DlgELFLearn.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgELFLearn.h
# End Source File
# Begin Source File

SOURCE=.\DlgELFProp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgELFProp.h
# End Source File
# Begin Source File

SOURCE=.\DlgELFRefine.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgELFRefine.h
# End Source File
# Begin Source File

SOURCE=.\DlgELFRefineTrans.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgELFRefineTrans.h
# End Source File
# Begin Source File

SOURCE=.\DlgELFRetrain.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgELFRetrain.h
# End Source File
# Begin Source File

SOURCE=.\DlgELFSkillTree.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgELFSkillTree.h
# End Source File
# Begin Source File

SOURCE=.\DlgELFToPill.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgELFToPill.h
# End Source File
# Begin Source File

SOURCE=.\DlgELFTransaction.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgELFTransaction.h
# End Source File
# Begin Source File

SOURCE=.\DlgELFXiDian.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgELFXiDian.h
# End Source File
# Begin Source File

SOURCE=.\DlgEngrave.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEngrave.h
# End Source File
# Begin Source File

SOURCE=.\DlgEPEquip.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEPEquip.h
# End Source File
# Begin Source File

SOURCE=.\DlgEquipBind.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEquipBind.h
# End Source File
# Begin Source File

SOURCE=.\DlgEquipDestroy.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEquipDestroy.h
# End Source File
# Begin Source File

SOURCE=.\DlgEquipDye.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEquipDye.h
# End Source File
# Begin Source File

SOURCE=.\DlgEquipMark.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEquipMark.h
# End Source File
# Begin Source File

SOURCE=.\DlgEquipRefine.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEquipRefine.h
# End Source File
# Begin Source File

SOURCE=.\DlgEquipSlot.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEquipSlot.h
# End Source File
# Begin Source File

SOURCE=.\DlgEquipUndestroy.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEquipUndestroy.h
# End Source File
# Begin Source File

SOURCE=.\DlgEquipUp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEquipUp.h
# End Source File
# Begin Source File

SOURCE=.\DlgEquipUpEnsure.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEquipUpEnsure.h
# End Source File
# Begin Source File

SOURCE=.\DlgExclusiveAward.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgExclusiveAward.h
# End Source File
# Begin Source File

SOURCE=.\DlgExit.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgExit.h
# End Source File
# Begin Source File

SOURCE=.\DlgExp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgExp.h
# End Source File
# Begin Source File

SOURCE=.\DlgFaceLift.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFaceLift.h
# End Source File
# Begin Source File

SOURCE=.\DlgFactionPVPRank.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFactionPVPRank.h
# End Source File
# Begin Source File

SOURCE=.\DlgFactionPVPStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFactionPVPStatus.h
# End Source File
# Begin Source File

SOURCE=.\DlgFashionShop.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFashionShop.h
# End Source File
# Begin Source File

SOURCE=.\DlgFashionShopItem.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFashionShopItem.h
# End Source File
# Begin Source File

SOURCE=.\DlgFashionSplit.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFashionSplit.h
# End Source File
# Begin Source File

SOURCE=.\DlgFindPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFindPlayer.h
# End Source File
# Begin Source File

SOURCE=.\DlgFittingRoom.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFittingRoom.h
# End Source File
# Begin Source File

SOURCE=.\DlgFlySwordImprove.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFlySwordImprove.h
# End Source File
# Begin Source File

SOURCE=.\DlgForce.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgForce.h
# End Source File
# Begin Source File

SOURCE=.\DlgForceActivity.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgForceActivity.h
# End Source File
# Begin Source File

SOURCE=.\DlgForceActivityIcon.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgForceActivityIcon.h
# End Source File
# Begin Source File

SOURCE=.\DlgForceNPC.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgForceNPC.h
# End Source File
# Begin Source File

SOURCE=.\DlgFortressBuild.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFortressBuild.h
# End Source File
# Begin Source File

SOURCE=.\DlgFortressBuildHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFortressBuildHelp.h
# End Source File
# Begin Source File

SOURCE=.\DlgFortressBuildSub.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFortressBuildSub.h
# End Source File
# Begin Source File

SOURCE=.\DlgFortressBuildSubList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFortressBuildSubList.h
# End Source File
# Begin Source File

SOURCE=.\DlgFortressContrib.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFortressContrib.h
# End Source File
# Begin Source File

SOURCE=.\DlgFortressExchange.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFortressExchange.h
# End Source File
# Begin Source File

SOURCE=.\DlgFortressInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFortressInfo.h
# End Source File
# Begin Source File

SOURCE=.\DlgFortressMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFortressMaterial.h
# End Source File
# Begin Source File

SOURCE=.\DlgFortressMini.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFortressMini.h
# End Source File
# Begin Source File

SOURCE=.\DlgFortressStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFortressStatus.h
# End Source File
# Begin Source File

SOURCE=.\DlgFortressWar.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFortressWar.h
# End Source File
# Begin Source File

SOURCE=.\DlgFortressWarList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFortressWarList.h
# End Source File
# Begin Source File

SOURCE=.\DlgFriendChat.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFriendChat.h
# End Source File
# Begin Source File

SOURCE=.\DlgFriendColor.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFriendColor.h
# End Source File
# Begin Source File

SOURCE=.\DlgFriendGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFriendGroup.h
# End Source File
# Begin Source File

SOURCE=.\DlgFriendHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFriendHistory.h
# End Source File
# Begin Source File

SOURCE=.\DlgFriendList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFriendList.h
# End Source File
# Begin Source File

SOURCE=.\DlgFriendOptionGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFriendOptionGroup.h
# End Source File
# Begin Source File

SOURCE=.\DlgFriendOptionName.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFriendOptionName.h
# End Source File
# Begin Source File

SOURCE=.\DlgFriendOptionNormal.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFriendOptionNormal.h
# End Source File
# Begin Source File

SOURCE=.\DlgFriendRequest.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFriendRequest.h
# End Source File
# Begin Source File

SOURCE=.\DlgGeneralCard.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGeneralCard.h
# End Source File
# Begin Source File

SOURCE=.\DlgGeneralCardBuy.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGeneralCardBuy.h
# End Source File
# Begin Source File

SOURCE=.\DlgGeneralCardCollection.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGeneralCardCollection.h
# End Source File
# Begin Source File

SOURCE=.\DlgGeneralCardRespawn.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGeneralCardRespawn.h
# End Source File
# Begin Source File

SOURCE=.\DlgGiftCard.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGiftCard.h
# End Source File
# Begin Source File

SOURCE=.\DlgGivingFor.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGivingFor.h
# End Source File
# Begin Source File

SOURCE=.\DlgGodEvilConvert.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGodEvilConvert.h
# End Source File
# Begin Source File

SOURCE=.\DlgGoldAccount.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGoldAccount.h
# End Source File
# Begin Source File

SOURCE=.\DlgGoldInquire.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGoldInquire.h
# End Source File
# Begin Source File

SOURCE=.\DlgGoldPassword.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGoldPassword.h
# End Source File
# Begin Source File

SOURCE=.\DlgGoldTrade.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGoldTrade.h
# End Source File
# Begin Source File

SOURCE=.\DlgGuildCreate.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGuildCreate.h
# End Source File
# Begin Source File

SOURCE=.\DlgGuildDiplomacyApply.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGuildDiplomacyApply.h
# End Source File
# Begin Source File

SOURCE=.\DlgGuildDiplomacyMan.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGuildDiplomacyMan.h
# End Source File
# Begin Source File

SOURCE=.\DlgGuildMan.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGuildMan.h
# End Source File
# Begin Source File

SOURCE=.\DlgGuildMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGuildMap.h
# End Source File
# Begin Source File

SOURCE=.\DlgGuildMapStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGuildMapStatus.h
# End Source File
# Begin Source File

SOURCE=.\DlgGuildMapTravel.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGuildMapTravel.h
# End Source File
# Begin Source File

SOURCE=.\DlgHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgHelp.h
# End Source File
# Begin Source File

SOURCE=.\DlgHistoryStage.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgHistoryStage.h
# End Source File
# Begin Source File

SOURCE=.\DlgHost.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgHost.h
# End Source File
# Begin Source File

SOURCE=.\DlgHostELF.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgHostELF.h
# End Source File
# Begin Source File

SOURCE=.\DlgHostPet.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgHostPet.h
# End Source File
# Begin Source File

SOURCE=.\DlgIceThunderBall.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgIceThunderBall.h
# End Source File
# Begin Source File

SOURCE=.\DlgIdentify.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgIdentify.h
# End Source File
# Begin Source File

SOURCE=.\DlgInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgInfo.h
# End Source File
# Begin Source File

SOURCE=.\DlgInputName.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgInputName.h
# End Source File
# Begin Source File

SOURCE=.\DlgInputNO.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgInputNO.h
# End Source File
# Begin Source File

SOURCE=.\DlgInstall.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgInstall.h
# End Source File
# Begin Source File

SOURCE=.\DlgInventory.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgInventory.h
# End Source File
# Begin Source File

SOURCE=.\DlgInventoryDetail.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgInventoryDetail.h
# End Source File
# Begin Source File

SOURCE=.\DlgInvisibleList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgInvisibleList.h
# End Source File
# Begin Source File

SOURCE=.\DlgItemDesc.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgItemDesc.h
# End Source File
# Begin Source File

SOURCE=.\DlgKingAuction.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgKingAuction.h
# End Source File
# Begin Source File

SOURCE=.\DlgKingCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgKingCommand.h
# End Source File
# Begin Source File

SOURCE=.\DlgKingElection.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgKingElection.h
# End Source File
# Begin Source File

SOURCE=.\DlgLevel2UpgradeShow.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLevel2UpgradeShow.h
# End Source File
# Begin Source File

SOURCE=.\DlgLoginQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLoginQueue.h
# End Source File
# Begin Source File

SOURCE=.\DlgMailList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMailList.h
# End Source File
# Begin Source File

SOURCE=.\DlgMailOption.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMailOption.h
# End Source File
# Begin Source File

SOURCE=.\DlgMailRead.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMailRead.h
# End Source File
# Begin Source File

SOURCE=.\DlgMailToFriends.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMailToFriends.h
# End Source File
# Begin Source File

SOURCE=.\DlgMailToFriendsRemind.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMailToFriendsRemind.h
# End Source File
# Begin Source File

SOURCE=.\DlgMailWrite.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMailWrite.h
# End Source File
# Begin Source File

SOURCE=.\DlgMatchProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMatchProfile.h
# End Source File
# Begin Source File

SOURCE=.\DlgMatchResult.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMatchResult.h
# End Source File
# Begin Source File

SOURCE=.\DlgMeridians.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMeridians.h
# End Source File
# Begin Source File

SOURCE=.\DlgMeridiansImpact.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMeridiansImpact.h
# End Source File
# Begin Source File

SOURCE=.\DlgMessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMessageBox.h
# End Source File
# Begin Source File

SOURCE=.\DlgMiniMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMiniMap.h
# End Source File
# Begin Source File

SOURCE=.\DlgMinimizeBar.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMinimizeBar.h
# End Source File
# Begin Source File

SOURCE=.\DlgMonsterSpirit.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMonsterSpirit.h
# End Source File
# Begin Source File

SOURCE=.\DlgNameLink.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgNameLink.h
# End Source File
# Begin Source File

SOURCE=.\DlgNPC.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgNPC.h
# End Source File
# Begin Source File

SOURCE=.\DlgOfflineShopCreate.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOfflineShopCreate.h
# End Source File
# Begin Source File

SOURCE=.\DlgOfflineShopItemsList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOfflineShopItemsList.h
# End Source File
# Begin Source File

SOURCE=.\DlgOfflineShopList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOfflineShopList.h
# End Source File
# Begin Source File

SOURCE=.\DlgOfflineShopSelfSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOfflineShopSelfSetting.h
# End Source File
# Begin Source File

SOURCE=.\DlgOfflineShopStore.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOfflineShopStore.h
# End Source File
# Begin Source File

SOURCE=.\DlgOnlineAward.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOnlineAward.h
# End Source File
# Begin Source File

SOURCE=.\DlgOptimizeOS.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOptimizeOS.h
# End Source File
# Begin Source File

SOURCE=.\DlgPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPalette.h
# End Source File
# Begin Source File

SOURCE=.\DlgPalette2.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPalette2.h
# End Source File
# Begin Source File

SOURCE=.\DlgPaletteBase.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPaletteBase.h
# End Source File
# Begin Source File

SOURCE=.\DlgPetDetail.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPetDetail.h
# End Source File
# Begin Source File

SOURCE=.\DlgPetDye.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPetDye.h
# End Source File
# Begin Source File

SOURCE=.\DlgPetEvolution.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPetEvolution.h
# End Source File
# Begin Source File

SOURCE=.\DlgPetEvolutionAnim.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPetEvolutionAnim.h
# End Source File
# Begin Source File

SOURCE=.\DlgPetFittingRoom.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPetFittingRoom.h
# End Source File
# Begin Source File

SOURCE=.\DlgPetHatch.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPetHatch.h
# End Source File
# Begin Source File

SOURCE=.\DlgPetList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPetList.h
# End Source File
# Begin Source File

SOURCE=.\DlgPetNature.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPetNature.h
# End Source File
# Begin Source File

SOURCE=.\DlgPetRec.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPetRec.h
# End Source File
# Begin Source File

SOURCE=.\DlgPetRename.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPetRename.h
# End Source File
# Begin Source File

SOURCE=.\DlgPetRetrain.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPetRetrain.h
# End Source File
# Begin Source File

SOURCE=.\DlgPKSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPKSetting.h
# End Source File
# Begin Source File

SOURCE=.\DlgPlantPet.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPlantPet.h
# End Source File
# Begin Source File

SOURCE=.\DlgPlayerRename.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPlayerRename.h
# End Source File
# Begin Source File

SOURCE=.\DlgPopMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPopMsg.h
# End Source File
# Begin Source File

SOURCE=.\DlgPQ.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPQ.h
# End Source File
# Begin Source File

SOURCE=.\DlgPreviewPetRebuild.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPreviewPetRebuild.h
# End Source File
# Begin Source File

SOURCE=.\DlgPreviewProduce.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPreviewProduce.h
# End Source File
# Begin Source File

SOURCE=.\DlgProclaim.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProclaim.h
# End Source File
# Begin Source File

SOURCE=.\DlgProduce.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProduce.h
# End Source File
# Begin Source File

SOURCE=.\DlgProfView.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProfView.h
# End Source File
# Begin Source File

SOURCE=.\DlgPureColorFashionTip.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPureColorFashionTip.h
# End Source File
# Begin Source File

SOURCE=.\DlgQShop.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgQShop.h
# End Source File
# Begin Source File

SOURCE=.\DlgQShopBuy.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgQShopBuy.h
# End Source File
# Begin Source File

SOURCE=.\DlgQShopItem.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgQShopItem.h
# End Source File
# Begin Source File

SOURCE=.\DlgQuestion.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgQuestion.h
# End Source File
# Begin Source File

SOURCE=.\DlgQuestionTask.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgQuestionTask.h
# End Source File
# Begin Source File

SOURCE=.\DlgQuickAction.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgQuickAction.h
# End Source File
# Begin Source File

SOURCE=.\DlgQuickBar.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgQuickBar.h
# End Source File
# Begin Source File

SOURCE=.\DlgQuickBarPet.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgQuickBarPet.h
# End Source File
# Begin Source File

SOURCE=.\DlgQuickBuyBase.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgQuickBuyBase.h
# End Source File
# Begin Source File

SOURCE=.\DlgQuickBuyPop.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgQuickBuyPop.h
# End Source File
# Begin Source File

SOURCE=.\DlgQuickPay.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgQuickPay.h
# End Source File
# Begin Source File

SOURCE=.\DlgRandomMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRandomMap.h
# End Source File
# Begin Source File

SOURCE=.\DlgRandProp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRandProp.h
# End Source File
# Begin Source File

SOURCE=.\DlgRedSpark.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRedSpark.h
# End Source File
# Begin Source File

SOURCE=.\DlgRefineTrans.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRefineTrans.h
# End Source File
# Begin Source File

SOURCE=.\DlgReincarnation.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgReincarnation.h
# End Source File
# Begin Source File

SOURCE=.\DlgReincarnationBook.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgReincarnationBook.h
# End Source File
# Begin Source File

SOURCE=.\DlgReincarnationRewrite.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgReincarnationRewrite.h
# End Source File
# Begin Source File

SOURCE=.\DlgReincarnationShow.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgReincarnationShow.h
# End Source File
# Begin Source File

SOURCE=.\DlgReportToGM.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgReportToGM.h
# End Source File
# Begin Source File

SOURCE=.\DlgResetProp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgResetProp.h
# End Source File
# Begin Source File

SOURCE=.\DlgRevive.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRevive.h
# End Source File
# Begin Source File

SOURCE=.\DlgSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSetting.h
# End Source File
# Begin Source File

SOURCE=.\DlgSettingGame.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSettingGame.h
# End Source File
# Begin Source File

SOURCE=.\DlgSettingQuickKey.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSettingQuickKey.h
# End Source File
# Begin Source File

SOURCE=.\DlgSettingSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSettingSystem.h
# End Source File
# Begin Source File

SOURCE=.\DlgSettingVideo.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSettingVideo.h
# End Source File
# Begin Source File

SOURCE=.\DlgShop.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgShop.h
# End Source File
# Begin Source File

SOURCE=.\DlgShopCart.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgShopCart.h
# End Source File
# Begin Source File

SOURCE=.\DlgShopCartSub.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgShopCartSub.h
# End Source File
# Begin Source File

SOURCE=.\DlgShopCartSubList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgShopCartSubList.h
# End Source File
# Begin Source File

SOURCE=.\DlgSignIn.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSignIn.h
# End Source File
# Begin Source File

SOURCE=.\DlgSimpleCostItemService.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSimpleCostItemService.h
# End Source File
# Begin Source File

SOURCE=.\DlgSkill.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSkill.h
# End Source File
# Begin Source File

SOURCE=.\DlgSkillAction.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSkillAction.h
# End Source File
# Begin Source File

SOURCE=.\DlgSkillEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSkillEdit.h
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubAction.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubAction.h
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubList.h
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubListItem.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubListItem.h
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubListRankItem.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubListRankItem.h
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubOther.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubOther.h
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubPool.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubPool.h
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubTree.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSkillSubTree.h
# End Source File
# Begin Source File

SOURCE=.\DlgSkillTree.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSkillTree.h
# End Source File
# Begin Source File

SOURCE=.\DlgSplit.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSplit.h
# End Source File
# Begin Source File

SOURCE=.\DlgStoneChange.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgStoneChange.h
# End Source File
# Begin Source File

SOURCE=.\DlgStoneReplace.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgStoneReplace.h
# End Source File
# Begin Source File

SOURCE=.\DlgStoneTransfer.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgStoneTransfer.h
# End Source File
# Begin Source File

SOURCE=.\DlgStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgStorage.h
# End Source File
# Begin Source File

SOURCE=.\DlgStorageChangePW.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgStorageChangePW.h
# End Source File
# Begin Source File

SOURCE=.\DlgStoragePW.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgStoragePW.h
# End Source File
# Begin Source File

SOURCE=.\DlgSysModuleQuickBar.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSysModuleQuickBar.h
# End Source File
# Begin Source File

SOURCE=.\DlgSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSystem.h
# End Source File
# Begin Source File

SOURCE=.\DlgSystem2.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSystem2.h
# End Source File
# Begin Source File

SOURCE=.\DlgSystem3.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSystem3.h
# End Source File
# Begin Source File

SOURCE=.\DlgTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTarget.h
# End Source File
# Begin Source File

SOURCE=.\DlgTargetItem.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTargetItem.h
# End Source File
# Begin Source File

SOURCE=.\DlgTask.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTask.h
# End Source File
# Begin Source File

SOURCE=.\DlgTaskConfirm.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTaskConfirm.h
# End Source File
# Begin Source File

SOURCE=.\DlgTaskList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTaskList.h
# End Source File
# Begin Source File

SOURCE=.\DlgTaskTrace.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTaskTrace.h
# End Source File
# Begin Source File

SOURCE=.\DlgTeach.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTeach.h
# End Source File
# Begin Source File

SOURCE=.\DlgTeamContext.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTeamContext.h
# End Source File
# Begin Source File

SOURCE=.\DlgTeamMain.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTeamMain.h
# End Source File
# Begin Source File

SOURCE=.\DlgTeamMate.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTeamMate.h
# End Source File
# Begin Source File

SOURCE=.\DlgTextHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTextHelp.h
# End Source File
# Begin Source File

SOURCE=.\DlgTheme.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTheme.h
# End Source File
# Begin Source File

SOURCE=.\DlgTitleBubble.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTitleBubble.h
# End Source File
# Begin Source File

SOURCE=.\DlgTitleChallenge.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTitleChallenge.h
# End Source File
# Begin Source File

SOURCE=.\DlgTitleList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTitleList.h
# End Source File
# Begin Source File

SOURCE=.\DlgTokenShop.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTokenShop.h
# End Source File
# Begin Source File

SOURCE=.\DlgTouchShop.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTouchShop.h
# End Source File
# Begin Source File

SOURCE=.\DlgTrade.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTrade.h
# End Source File
# Begin Source File

SOURCE=.\DlgTreasureMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTreasureMap.h
# End Source File
# Begin Source File

SOURCE=.\DlgViewModel.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgViewModel.h
# End Source File
# Begin Source File

SOURCE=.\DlgWarTower.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWarTower.h
# End Source File
# Begin Source File

SOURCE=.\DlgWebList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWebList.h
# End Source File
# Begin Source File

SOURCE=.\DlgWebMyShop.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWebMyShop.h
# End Source File
# Begin Source File

SOURCE=.\DlgWebTradeBase.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWebTradeBase.h
# End Source File
# Begin Source File

SOURCE=.\DlgWebTradeOption.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWebTradeOption.h
# End Source File
# Begin Source File

SOURCE=.\DlgWebViewProduct.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWebViewProduct.h
# End Source File
# Begin Source File

SOURCE=.\DlgWedding.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWedding.h
# End Source File
# Begin Source File

SOURCE=.\DlgWeddingInvite.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWeddingInvite.h
# End Source File
# Begin Source File

SOURCE=.\DlgWorldMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWorldMap.h
# End Source File
# End Group
# Begin Group "CustomizeDlg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgCustomize.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCustomize.h
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeBase.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeBase.h
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeEye.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeEye.h
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeFace.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeFace.h
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeHair.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeHair.h
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeNose.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeNose.h
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizePaint.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizePaint.h
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizePre.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizePre.h
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeStature.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeStature.h
# End Source File
# Begin Source File

SOURCE=.\DlgFaceName.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFaceName.h
# End Source File
# Begin Source File

SOURCE=.\EC_CustomizePolicy.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_CustomizePolicy.h
# End Source File
# End Group
# Begin Group "GMTool Files"

# PROP Default_Filter ""
# Begin Group "SimpleDB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SDBArchive.h
# End Source File
# Begin Source File

SOURCE=.\SDBDatabase.h
# End Source File
# Begin Source File

SOURCE=.\SDBDateTime.h
# End Source File
# Begin Source File

SOURCE=.\SDBField.h
# End Source File
# Begin Source File

SOURCE=.\SDBMisc.cpp
# End Source File
# Begin Source File

SOURCE=.\SDBMisc.h
# End Source File
# Begin Source File

SOURCE=.\SDBRecord.h
# End Source File
# Begin Source File

SOURCE=.\SDBTable.h
# End Source File
# Begin Source File

SOURCE=.\TPBase.h
# End Source File
# Begin Source File

SOURCE=.\TypeList.h
# End Source File
# Begin Source File

SOURCE=.\ValueList.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\DlgDelMsgReason.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGMConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGMConsole.h
# End Source File
# Begin Source File

SOURCE=.\DlgGMConsoleForbid.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGMConsoleForbid.h
# End Source File
# Begin Source File

SOURCE=.\DlgGMDelMsgReason.h
# End Source File
# Begin Source File

SOURCE=.\DlgGMFinishMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGMFinishMsg.h
# End Source File
# Begin Source File

SOURCE=.\DlgGMParam.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGMParam.h
# End Source File
# Begin Source File

SOURCE=.\DlgGMQueryItem.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGMQueryItem.h
# End Source File
# Begin Source File

SOURCE=.\GMCDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\GMCDatabase.h
# End Source File
# Begin Source File

SOURCE=.\GMCDBMan.cpp
# End Source File
# Begin Source File

SOURCE=.\GMCDBMan.h
# End Source File
# Begin Source File

SOURCE=.\GMCommand.h
# End Source File
# Begin Source File

SOURCE=.\GMCommandFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\GMCommandFactory.h
# End Source File
# Begin Source File

SOURCE=.\GMCommandInGame.cpp
# End Source File
# Begin Source File

SOURCE=.\GMCommandInGame.h
# End Source File
# Begin Source File

SOURCE=.\GMCommandListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\GMCommandListBox.h
# End Source File
# Begin Source File

SOURCE=.\Param.cpp
# End Source File
# Begin Source File

SOURCE=.\Param.h
# End Source File
# Begin Source File

SOURCE=.\PredefinedGMHelpMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\PredefinedGMHelpMsg.h
# End Source File
# End Group
# Begin Group "LoginDialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgAgreement.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAgreement.h
# End Source File
# Begin Source File

SOURCE=.\DlgCreate.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCreate.h
# End Source File
# Begin Source File

SOURCE=.\DlgFindServer.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFindServer.h
# End Source File
# Begin Source File

SOURCE=.\DlgLogin.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLogin.h
# End Source File
# Begin Source File

SOURCE=.\DlgLoginButton.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLoginButton.h
# End Source File
# Begin Source File

SOURCE=.\DlgLoginOther.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLoginOther.h
# End Source File
# Begin Source File

SOURCE=.\DlgLoginPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLoginPage.h
# End Source File
# Begin Source File

SOURCE=.\DlgLoginServerList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLoginServerList.h
# End Source File
# Begin Source File

SOURCE=.\DlgLoginVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLoginVersion.h
# End Source File
# Begin Source File

SOURCE=.\DlgPwdHint.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPwdHint.h
# End Source File
# Begin Source File

SOURCE=.\DlgPwdProtect.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPwdProtect.h
# End Source File
# Begin Source File

SOURCE=.\DlgSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSelect.h
# End Source File
# Begin Source File

SOURCE=.\DlgWebRoleTrade.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWebRoleTrade.h
# End Source File
# Begin Source File

SOURCE=.\DlgWebTradeInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWebTradeInfo.h
# End Source File
# End Group
# Begin Group "HomeDlgs"

# PROP Default_Filter ""
# End Group
# Begin Group "Wiki"

# PROP Default_Filter ""
# Begin Group "BaseView"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgWikiBase.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiBase.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiByNameBase.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiByNameBase.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiItemListBase.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiItemListBase.h
# End Source File
# End Group
# Begin Group "ItemListView"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgWikiAreaList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiAreaList.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiEquipmentDrop.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiEquipmentDrop.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiItemConfirm.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiItemConfirm.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiMonsterDrop.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiMonsterDrop.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiMonsterList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiMonsterList.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiMonsterTask.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiMonsterTask.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiNPCList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiNPCList.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiRandomProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiRandomProperty.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiTaskList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiTaskList.h
# End Source File
# End Group
# Begin Group "SearchData"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WikiEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\WikiEntity.h
# End Source File
# Begin Source File

SOURCE=.\WikiEquipmentProp.cpp
# End Source File
# Begin Source File

SOURCE=.\WikiEquipmentProp.h
# End Source File
# Begin Source File

SOURCE=.\WikiGuideData.cpp
# End Source File
# Begin Source File

SOURCE=.\WikiGuideData.h
# End Source File
# Begin Source File

SOURCE=.\WikiItemProp.cpp
# End Source File
# Begin Source File

SOURCE=.\WikiItemProp.h
# End Source File
# End Group
# Begin Group "SearchLogic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WikiDataReader.cpp
# End Source File
# Begin Source File

SOURCE=.\WikiDataReader.h
# End Source File
# Begin Source File

SOURCE=.\WikiPage.h
# End Source File
# Begin Source File

SOURCE=.\WikiSearchCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\WikiSearchCommand.h
# End Source File
# Begin Source File

SOURCE=.\WikiSearcher.cpp
# End Source File
# Begin Source File

SOURCE=.\WikiSearcher.h
# End Source File
# Begin Source File

SOURCE=.\WikiSearchNPC.cpp
# End Source File
# Begin Source File

SOURCE=.\WikiSearchNPC.h
# End Source File
# Begin Source File

SOURCE=.\WikiSearchTask.cpp
# End Source File
# Begin Source File

SOURCE=.\WikiSearchTask.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\DlgWiki.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWiki.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiEquipment.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiEquipment.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiFeature.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiFeature.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiGuide.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiGuide.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiItem.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiItem.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiMonster.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiMonster.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiRecipe.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiRecipe.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiRecipeDetail.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiRecipeDetail.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiSkill.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiSkill.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiTask.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiTask.h
# End Source File
# Begin Source File

SOURCE=.\DlgWikiTaskDetail.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWikiTaskDetail.h
# End Source File
# End Group
# Begin Group "Explorer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgExplorer.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgExplorer.h
# End Source File
# Begin Source File

SOURCE=.\MsgTransFun.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgTransFun.h
# End Source File
# Begin Source File

SOURCE=.\MY_ATLCOM.H
# End Source File
# Begin Source File

SOURCE=.\PWAtl.h
# End Source File
# Begin Source File

SOURCE=.\PWBrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\PWBrowser.h
# End Source File
# Begin Source File

SOURCE=.\SafeWinImpl.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AUI_ImageGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\AUI_ImageGrid.h
# End Source File
# Begin Source File

SOURCE=.\AWStringWithWildcard.cpp
# End Source File
# Begin Source File

SOURCE=.\AWStringWithWildcard.h
# End Source File
# Begin Source File

SOURCE=.\DlgCurrentServer.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCurrentServer.h
# End Source File
# Begin Source File

SOURCE=.\EC_AutoTeamConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_AutoTeamConfig.h
# End Source File
# Begin Source File

SOURCE=.\EC_BaseUIMan.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_BaseUIMan.h
# End Source File
# Begin Source File

SOURCE=.\EC_BBSFileDownloader.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_BBSFileDownloader.h
# End Source File
# Begin Source File

SOURCE=.\EC_BBSPage.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_BBSPage.h
# End Source File
# Begin Source File

SOURCE=.\EC_BBSPageFile.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_BBSPageFile.h
# End Source File
# Begin Source File

SOURCE=.\EC_ControlLoopAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ControlLoopAnimation.h
# End Source File
# Begin Source File

SOURCE=.\EC_CustomizeBound.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_CustomizeBound.h
# End Source File
# Begin Source File

SOURCE=.\EC_CustomizeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_CustomizeMgr.h
# End Source File
# Begin Source File

SOURCE=.\EC_DlgCmdConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_DomainBase.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_DomainBase.h
# End Source File
# Begin Source File

SOURCE=.\EC_DomainCountry.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_DomainCountry.h
# End Source File
# Begin Source File

SOURCE=.\EC_DomainGuild.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_DomainGuild.h
# End Source File
# Begin Source File

SOURCE=.\EC_DragDropHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_DragDropHelper.h
# End Source File
# Begin Source File

SOURCE=.\EC_GameTalk.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_GameTalk.h
# End Source File
# Begin Source File

SOURCE=.\EC_GameUIMan.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_GameUIMan.h
# End Source File
# Begin Source File

SOURCE=.\EC_GameUIMan1.inl
# End Source File
# Begin Source File

SOURCE=.\EC_GameUIMan2.inl
# End Source File
# Begin Source File

SOURCE=.\EC_GameUIManDlgs.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_GuildDiplomacyReminder.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_GuildDiplomacyReminder.h
# End Source File
# Begin Source File

SOURCE=.\EC_LoginSwitch.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_LoginSwitch.h
# End Source File
# Begin Source File

SOURCE=.\EC_LoginUIMan.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_LoginUIMan.h
# End Source File
# Begin Source File

SOURCE=.\EC_Meridians.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Meridians.h
# End Source File
# Begin Source File

SOURCE=.\EC_OfflineShopCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_OfflineShopCtrl.h
# End Source File
# Begin Source File

SOURCE=.\EC_ProfConfigs.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ProfConfigs.h
# End Source File
# Begin Source File

SOURCE=.\EC_ProfDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ProfDisplay.h
# End Source File
# Begin Source File

SOURCE=.\EC_QuickBuyActivity.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_QuickBuyActivity.h
# End Source File
# Begin Source File

SOURCE=.\EC_QuickBuyPopMan.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_QuickBuyPopMan.h
# End Source File
# Begin Source File

SOURCE=.\EC_QuickBuyPopPolicy.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_QuickBuyPopPolicy.h
# End Source File
# Begin Source File

SOURCE=.\EC_QuickBuySaveLife.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_QuickBuySaveLife.h
# End Source File
# Begin Source File

SOURCE=.\EC_RecordAccount.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_RecordAccount.h
# End Source File
# Begin Source File

SOURCE=.\EC_ServerList.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ServerList.h
# End Source File
# Begin Source File

SOURCE=.\EC_ShortcutMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ShortcutMgr.h
# End Source File
# Begin Source File

SOURCE=.\EC_UIAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_UIAnimation.h
# End Source File
# Begin Source File

SOURCE=.\EC_UIConfigs.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_UIConfigs.h
# End Source File
# Begin Source File

SOURCE=.\EC_UIHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_UIHelper.h
# End Source File
# Begin Source File

SOURCE=.\EC_UIManager.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_UIManager.h
# End Source File
# Begin Source File

SOURCE=.\EC_URLOSNavigator.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_URLOSNavigator.h
# End Source File
# Begin Source File

SOURCE=..\CElementData\vector_string.cpp
# End Source File
# Begin Source File

SOURCE=..\CElementData\vector_string.h
# End Source File
# End Group
# Begin Group "Manager Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_Manager.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Manager.h
# End Source File
# Begin Source File

SOURCE=.\EC_ManAttacks.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ManAttacks.h
# End Source File
# Begin Source File

SOURCE=.\EC_ManDecal.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ManDecal.h
# End Source File
# Begin Source File

SOURCE=.\EC_ManMatter.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ManMatter.h
# End Source File
# Begin Source File

SOURCE=.\EC_ManMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ManMessage.h
# End Source File
# Begin Source File

SOURCE=.\EC_ManNPC.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ManNPC.h
# End Source File
# Begin Source File

SOURCE=.\EC_ManOrnament.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ManOrnament.h
# End Source File
# Begin Source File

SOURCE=.\EC_ManPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ManPlayer.h
# End Source File
# Begin Source File

SOURCE=.\EC_ManSkillGfx.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ManSkillGfx.h
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Group "Protocol Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\accountloginrecord.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\acremotecode.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\acreport.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\addfriend.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\addfriend_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\addfriendremarks.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\addfriendremarks_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\addfriendrqst.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\announceforbidinfo.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\announcenewmail.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\announcesellresult.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionattendlist.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionattendlist_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionbid.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionbid_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionclose.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionclose_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionexitbid.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionexitbid_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionget.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionget_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctiongetitem.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctiongetitem_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionlist.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionlist_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionlistupdate.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionlistupdate_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionopen.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\auctionopen_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\autolockset.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\autolockset_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\battlechallenge.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\battlechallenge_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\battlechallengemap.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\battlechallengemap_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\battleenter.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\battleenter_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\battlegetmap.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\battlegetmap_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\battlestatus.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\battlestatus_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\buypoint.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\buypoint_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\cancelwaitqueue.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\cancelwaitqueue_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\cashlock.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\cashlock_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\cashpasswordset.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\cashpasswordset_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\challenge.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\changeds_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\chatmessage.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\chatroomcreate.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\chatroomcreate_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\chatroomexpel.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\chatroominvite.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\chatroominvite_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\chatroomjoin.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\chatroomjoin_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\chatroomleave.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\chatroomlist.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\chatroomlist_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\chatroomspeak.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\checknewmail.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\collectclientmachineinfo.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\complain2gm.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\complain2gm_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlegetbattlelimit.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlegetbattlelimit_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlegetconfig.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlegetconfig_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlegetkingcommandpoint.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlegetkingcommandpoint_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlegetmap.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlegetmap_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlegetplayerlocation.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlegetscore.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlegetscore_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlekingassignassault.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlekingassignassault_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlekingresetbattlelimit.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlemove.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlemove_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlepreenter.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlepreenternotify.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattleresult.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlereturncapital.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlesinglebattleresult.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\countrybattlesyncplayerlocation.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\createfactionfortress.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\createfactionfortress_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\createrole.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\createrole_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\deletemail.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\deletemail_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\deleterole.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\deleterole_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\delfriend.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\delfriend_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\deliverrolestatus.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\enterworld.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\errorinfo.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\exchangeconsumepoints.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\exchangeconsumepoints_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionacceptjoin.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionacceptjoin_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionallianceapply_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionalliancereply_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionapplyjoin_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionappoint_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionbroadcastnotice_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionchangproclaim_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionchat.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factioncreate_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factiondegrade_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factiondelayexpelannounce.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factiondismiss_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionexpel_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionfortressbattlelist.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionfortressbattlelist_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionfortresschallenge.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionfortresschallenge_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionfortressenter.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionfortressget.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionfortressget_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionfortresslist.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionfortresslist_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionhostileapply_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionhostilereply_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factioninvitejoin.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\factionleave_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionlistmember_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionlistonline.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionlistonline_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionlistrelation_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionmasterresign_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionoprequest.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionoprequest_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionrelationrecvapply.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionrelationrecvreply.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionremoverelationapply_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionremoverelationreply_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionrename_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionrenameannounce.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionresign_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionresourcebattlegetmap.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionresourcebattlegetmap_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionresourcebattlegetrecord.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionresourcebattlegetrecord_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionresourcebattlenotifyplayerevent.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionresourcebattleplayerqueryresult.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\factionupgrade_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\findrole.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\findrolebyname.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\findsellpointinfo.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\findsellpointinfo_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\friendalllist.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\frienddel.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\friendextlist.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\friendmove.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\friendstatus.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gameclient.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\gameclient.h
# End Source File
# Begin Source File

SOURCE=.\Network\gamedatasend.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gamezonelist.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\getcnetserverconfig.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getcnetserverconfig_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getcustomdata.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getcustomdata_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getfactionbaseinfo.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getfactionbaseinfo_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getfriends.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getfriends_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gethelpstates.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gethelpstates_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getmail.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getmail_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getmailattachobj.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getmailattachobj_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getmaillist.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getmaillist_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getplayerbriefinfo.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getplayerbriefinfo_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getplayerfactioninfo.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getplayerfactioninfo_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getplayeridbyname.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getplayeridbyname_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getrewardlist.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getrewardlist_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getsavedmsg.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getsavedmsg_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getselllist.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getselllist_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getuiconfig.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\getuiconfig_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmforbidrole.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmforbidrole_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmgetplayerconsumeinfo.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmgetplayerconsumeinfo_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmkickoutrole.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmkickoutrole_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmkickoutuser.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmkickoutuser_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmlistonlineuser.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmlistonlineuser_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmonlinenum.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmonlinenum_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmrestartserver.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmrestartserver_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmshutup.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmshutup_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmshutuprole.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmshutuprole_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmtogglechat.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\gmtogglechat_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\groupgetinfo.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\groupsetinfo.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\kecandidateapply.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\kecandidateapply_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\keepalive.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\kegetstatus.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\kegetstatus_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\kevoting.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\kevoting_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\keyexchange.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\keyreestablish.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\matrixchallenge.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\matrixresponse.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\msgretrieveoffline.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\onlineannounce.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playeraccuse.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playeraccuse_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playeraskforpresent_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playerbaseinfo.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playerbaseinfo_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playerbaseinfocrc.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playerbaseinfocrc_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playerchangeds_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playergivepresent_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playerlogout.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playernameupdate.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playerpositionresetrqst.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\playerrename_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playersendmail.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playersendmail_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\playersendmassmail.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\preservemail.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\preservemail_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\privatechat.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\publicchat.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\qqaddfriend.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\qqaddfriend_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\qqaddfriendrqst.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\qqclient.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\qqclient.h
# End Source File
# Begin Source File

SOURCE=.\Network\queryuserprivilege_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\refgetreferencecode.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\refgetreferencecode_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\reflistreferrals.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\reflistreferrals_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\refwithdrawbonus.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\refwithdrawbonus_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\report2gm.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\report2gm_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\reportchat.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\response.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\rewardmaturenotice.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\rolealllist.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\rolelist.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\rolelist_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\rolestatusannounce.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\selectrole.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\selectrole_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sellcancel.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sellcancel_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sellpoint.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sellpoint_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sendmsg.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\setcustomdata.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\setcustomdata_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\setfriendgroup.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\setfriendgroup_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\setgroupname.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\setgroupname_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sethelpstates.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sethelpstates_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\setuiconfig.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\setuiconfig_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\ssogetticket.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\ssogetticket_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\statusannounce.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sysauctionaccount.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sysauctionaccount_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sysauctionbid.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sysauctionbid_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sysauctioncashtransfer.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sysauctioncashtransfer_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sysauctiongetitem.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sysauctiongetitem_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sysauctionlist.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\sysauctionlist_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\tradeaddgoods.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\tradeaddgoods_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\tradeconfirm.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\tradeconfirm_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\tradediscard.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\tradediscard_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\tradeend.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\trademoveobj.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\trademoveobj_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\traderemovegoods.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\traderemovegoods_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\tradestart.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\tradestart_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\tradestartrqst.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\tradesubmit.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\tradesubmit_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\undodeleterole.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\undodeleterole_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\updateremaintime.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\useraddcash.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\useraddcash_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\usergetinfo.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\usersetinfo.hrp
# End Source File
# Begin Source File

SOURCE=.\Network\waitqueuestatenotify.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradeattendlist.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradeattendlist_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradegetdetail.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradegetdetail_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradegetitem.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradegetitem_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradelist.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradelist_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradeprecancelpost.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradeprecancelpost_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradeprepost.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradeprepost_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradeupdate.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\webtradeupdate_re.hpp
# End Source File
# Begin Source File

SOURCE=.\Network\worldchat.hpp
# End Source File
# End Group
# Begin Group "inl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\inl\accountloginrecord
# End Source File
# Begin Source File

SOURCE=.\Network\inl\acremotecode
# End Source File
# Begin Source File

SOURCE=.\Network\inl\acreport
# End Source File
# Begin Source File

SOURCE=.\Network\inl\addfriend
# End Source File
# Begin Source File

SOURCE=.\Network\inl\addfriend_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\addfriendremarks
# End Source File
# Begin Source File

SOURCE=.\Network\inl\addfriendremarks_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\addfriendrqst
# End Source File
# Begin Source File

SOURCE=.\Network\inl\announceforbidinfo
# End Source File
# Begin Source File

SOURCE=.\Network\inl\announcenewmail
# End Source File
# Begin Source File

SOURCE=.\Network\inl\announcesellresult
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionattendlist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionattendlist_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionbid
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionbid_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionclose
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionclose_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionexitbid
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionexitbid_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionget
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionget_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctiongetitem
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctiongetitem_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionlist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionlist_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionlistupdate
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionlistupdate_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionopen
# End Source File
# Begin Source File

SOURCE=.\Network\inl\auctionopen_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\autolockset
# End Source File
# Begin Source File

SOURCE=.\Network\inl\autolockset_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\battlechallenge
# End Source File
# Begin Source File

SOURCE=.\Network\inl\battlechallenge_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\battlechallengemap
# End Source File
# Begin Source File

SOURCE=.\Network\inl\battlechallengemap_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\battleenter
# End Source File
# Begin Source File

SOURCE=.\Network\inl\battleenter_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\battlegetmap
# End Source File
# Begin Source File

SOURCE=.\Network\inl\battlegetmap_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\battlestatus
# End Source File
# Begin Source File

SOURCE=.\Network\inl\battlestatus_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\buypoint
# End Source File
# Begin Source File

SOURCE=.\Network\inl\buypoint_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\cancelwaitqueue
# End Source File
# Begin Source File

SOURCE=.\Network\inl\cancelwaitqueue_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\cashlock
# End Source File
# Begin Source File

SOURCE=.\Network\inl\cashlock_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\cashpasswordset
# End Source File
# Begin Source File

SOURCE=.\Network\inl\cashpasswordset_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\challenge
# End Source File
# Begin Source File

SOURCE=.\Network\inl\changeds_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\chatmessage
# End Source File
# Begin Source File

SOURCE=.\Network\inl\chatroomcreate
# End Source File
# Begin Source File

SOURCE=.\Network\inl\chatroomcreate_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\chatroomexpel
# End Source File
# Begin Source File

SOURCE=.\Network\inl\chatroominvite
# End Source File
# Begin Source File

SOURCE=.\Network\inl\chatroominvite_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\chatroomjoin
# End Source File
# Begin Source File

SOURCE=.\Network\inl\chatroomjoin_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\chatroomleave
# End Source File
# Begin Source File

SOURCE=.\Network\inl\chatroomlist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\chatroomlist_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\chatroomspeak
# End Source File
# Begin Source File

SOURCE=.\Network\inl\checknewmail
# End Source File
# Begin Source File

SOURCE=.\Network\inl\collectclientmachineinfo
# End Source File
# Begin Source File

SOURCE=.\Network\inl\complain2gm
# End Source File
# Begin Source File

SOURCE=.\Network\inl\complain2gm_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlegetbattlelimit
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlegetbattlelimit_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlegetconfig
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlegetconfig_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlegetkingcommandpoint
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlegetkingcommandpoint_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlegetmap
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlegetmap_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlegetplayerlocation
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlegetscore
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlegetscore_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlekingassignassault
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlekingassignassault_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlekingresetbattlelimit
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlemove
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlemove_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlepreenter
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlepreenternotify
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattleresult
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlereturncapital
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlesinglebattleresult
# End Source File
# Begin Source File

SOURCE=.\Network\inl\countrybattlesyncplayerlocation
# End Source File
# Begin Source File

SOURCE=.\Network\inl\createfactionfortress
# End Source File
# Begin Source File

SOURCE=.\Network\inl\createfactionfortress_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\createrole
# End Source File
# Begin Source File

SOURCE=.\Network\inl\createrole_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\deletemail
# End Source File
# Begin Source File

SOURCE=.\Network\inl\deletemail_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\deleterole
# End Source File
# Begin Source File

SOURCE=.\Network\inl\deleterole_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\delfriend
# End Source File
# Begin Source File

SOURCE=.\Network\inl\delfriend_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\deliverrolestatus
# End Source File
# Begin Source File

SOURCE=.\Network\inl\enterworld
# End Source File
# Begin Source File

SOURCE=.\Network\inl\errorinfo
# End Source File
# Begin Source File

SOURCE=.\Network\inl\exchangeconsumepoints
# End Source File
# Begin Source File

SOURCE=.\Network\inl\exchangeconsumepoints_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionacceptjoin
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionacceptjoin_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionallianceapply_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionalliancereply_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionapplyjoin_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionappoint_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionbroadcastnotice_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionchangproclaim_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionchat
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factioncreate_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factiondegrade_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factiondelayexpelannounce
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factiondismiss_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionexpel_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionfortressbattlelist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionfortressbattlelist_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionfortresschallenge
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionfortresschallenge_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionfortressenter
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionfortressget
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionfortressget_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionfortresslist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionfortresslist_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionhostileapply_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionhostilereply_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factioninvitejoin
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionleave_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionlistmember_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionlistonline
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionlistonline_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionlistrelation_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionmasterresign_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionoprequest
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionoprequest_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionrelationrecvapply
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionrelationrecvreply
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionremoverelationapply_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionremoverelationreply_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionrename_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionrenameannounce
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionresign_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionresourcebattlegetmap
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionresourcebattlegetmap_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionresourcebattlegetrecord
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionresourcebattlegetrecord_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionresourcebattlenotifyplayerevent
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionresourcebattleplayerqueryresult
# End Source File
# Begin Source File

SOURCE=.\Network\inl\factionupgrade_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\findrole
# End Source File
# Begin Source File

SOURCE=.\Network\inl\findrolebyname
# End Source File
# Begin Source File

SOURCE=.\Network\inl\findsellpointinfo
# End Source File
# Begin Source File

SOURCE=.\Network\inl\findsellpointinfo_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\friendalllist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\frienddel
# End Source File
# Begin Source File

SOURCE=.\Network\inl\friendextlist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\friendmove
# End Source File
# Begin Source File

SOURCE=.\Network\inl\friendstatus
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gamedatasend
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gamezonelist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getcnetserverconfig
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getcnetserverconfig_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getcustomdata
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getcustomdata_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getfactionbaseinfo
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getfactionbaseinfo_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getfriends
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getfriends_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gethelpstates
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gethelpstates_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getmail
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getmail_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getmailattachobj
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getmailattachobj_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getmaillist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getmaillist_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getplayerbriefinfo
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getplayerbriefinfo_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getplayerfactioninfo
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getplayerfactioninfo_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getplayeridbyname
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getplayeridbyname_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getrewardlist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getrewardlist_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getsavedmsg
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getsavedmsg_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getselllist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getselllist_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getuiconfig
# End Source File
# Begin Source File

SOURCE=.\Network\inl\getuiconfig_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmforbidrole
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmforbidrole_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmgetplayerconsumeinfo
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmgetplayerconsumeinfo_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmkickoutrole
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmkickoutrole_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmkickoutuser
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmkickoutuser_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmlistonlineuser
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmlistonlineuser_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmonlinenum
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmonlinenum_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmrestartserver
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmrestartserver_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmshutup
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmshutup_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmshutuprole
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmshutuprole_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmtogglechat
# End Source File
# Begin Source File

SOURCE=.\Network\inl\gmtogglechat_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\groupgetinfo
# End Source File
# Begin Source File

SOURCE=.\Network\inl\groupsetinfo
# End Source File
# Begin Source File

SOURCE=.\Network\inl\kecandidateapply
# End Source File
# Begin Source File

SOURCE=.\Network\inl\kecandidateapply_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\keepalive
# End Source File
# Begin Source File

SOURCE=.\Network\inl\kegetstatus
# End Source File
# Begin Source File

SOURCE=.\Network\inl\kegetstatus_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\kevoting
# End Source File
# Begin Source File

SOURCE=.\Network\inl\kevoting_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\keyexchange
# End Source File
# Begin Source File

SOURCE=.\Network\inl\keyreestablish
# End Source File
# Begin Source File

SOURCE=.\Network\inl\matrixchallenge
# End Source File
# Begin Source File

SOURCE=.\Network\inl\matrixresponse
# End Source File
# Begin Source File

SOURCE=.\Network\inl\msgretrieveoffline
# End Source File
# Begin Source File

SOURCE=.\Network\inl\onlineannounce
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playeraccuse
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playeraccuse_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playeraskforpresent_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playerbaseinfo
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playerbaseinfo_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playerbaseinfocrc
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playerbaseinfocrc_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playerchangeds_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playergivepresent_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playerlogout
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playernameupdate
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playerpositionresetrqst
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playerrename_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playersendmail
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playersendmail_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\playersendmassmail
# End Source File
# Begin Source File

SOURCE=.\Network\inl\preservemail
# End Source File
# Begin Source File

SOURCE=.\Network\inl\preservemail_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\privatechat
# End Source File
# Begin Source File

SOURCE=.\Network\inl\publicchat
# End Source File
# Begin Source File

SOURCE=.\Network\inl\qqaddfriend
# End Source File
# Begin Source File

SOURCE=.\Network\inl\qqaddfriend_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\qqaddfriendrqst
# End Source File
# Begin Source File

SOURCE=.\Network\inl\queryuserprivilege_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\refgetreferencecode
# End Source File
# Begin Source File

SOURCE=.\Network\inl\refgetreferencecode_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\reflistreferrals
# End Source File
# Begin Source File

SOURCE=.\Network\inl\reflistreferrals_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\refwithdrawbonus
# End Source File
# Begin Source File

SOURCE=.\Network\inl\refwithdrawbonus_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\report2gm
# End Source File
# Begin Source File

SOURCE=.\Network\inl\report2gm_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\reportchat
# End Source File
# Begin Source File

SOURCE=.\Network\inl\response
# End Source File
# Begin Source File

SOURCE=.\Network\inl\rewardmaturenotice
# End Source File
# Begin Source File

SOURCE=.\Network\inl\rolealllist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\rolelist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\rolelist_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\rolestatusannounce
# End Source File
# Begin Source File

SOURCE=.\Network\inl\selectrole
# End Source File
# Begin Source File

SOURCE=.\Network\inl\selectrole_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sellcancel
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sellcancel_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sellpoint
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sellpoint_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sendmsg
# End Source File
# Begin Source File

SOURCE=.\Network\inl\setcustomdata
# End Source File
# Begin Source File

SOURCE=.\Network\inl\setcustomdata_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\setfriendgroup
# End Source File
# Begin Source File

SOURCE=.\Network\inl\setfriendgroup_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\setgroupname
# End Source File
# Begin Source File

SOURCE=.\Network\inl\setgroupname_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sethelpstates
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sethelpstates_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\setuiconfig
# End Source File
# Begin Source File

SOURCE=.\Network\inl\setuiconfig_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\ssogetticket
# End Source File
# Begin Source File

SOURCE=.\Network\inl\ssogetticket_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\statusannounce
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sysauctionaccount
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sysauctionaccount_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sysauctionbid
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sysauctionbid_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sysauctioncashtransfer
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sysauctioncashtransfer_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sysauctiongetitem
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sysauctiongetitem_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sysauctionlist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\sysauctionlist_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradeaddgoods
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradeaddgoods_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradeconfirm
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradeconfirm_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradediscard
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradediscard_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradeend
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradeinventory
# End Source File
# Begin Source File

SOURCE=.\Network\inl\trademoveobj
# End Source File
# Begin Source File

SOURCE=.\Network\inl\trademoveobj_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\traderemovegoods
# End Source File
# Begin Source File

SOURCE=.\Network\inl\traderemovegoods_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradesave
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradestart
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradestart_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradestartrqst
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradesubmit
# End Source File
# Begin Source File

SOURCE=.\Network\inl\tradesubmit_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\undodeleterole
# End Source File
# Begin Source File

SOURCE=.\Network\inl\undodeleterole_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\updateremaintime
# End Source File
# Begin Source File

SOURCE=.\Network\inl\useraddcash
# End Source File
# Begin Source File

SOURCE=.\Network\inl\useraddcash_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\usergetinfo
# End Source File
# Begin Source File

SOURCE=.\Network\inl\usersetinfo
# End Source File
# Begin Source File

SOURCE=.\Network\inl\waitqueuestatenotify
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradeattendlist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradeattendlist_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradegetdetail
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradegetdetail_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradegetitem
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradegetitem_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradelist
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradelist_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradeprecancelpost
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradeprecancelpost_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradeprepost
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradeprepost_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradeupdate
# End Source File
# Begin Source File

SOURCE=.\Network\inl\webtradeupdate_re
# End Source File
# Begin Source File

SOURCE=.\Network\inl\worldchat
# End Source File
# End Group
# Begin Group "rpcdata"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\rpcdata\addfriendrqstarg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\addfriendrqstres
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\auctionid
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\factioninvitearg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\factioninviteres
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\findrolearg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\findrolebynamearg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\findrolebynameres
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\findroleres
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\fmemberinfo
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\friendalllistarg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\friendalllistres
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\friendbrief
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\frienddelarg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\frienddelres
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\friendmovearg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\friendmoveres
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gamezonelistarg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gamezonelistres
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gauctionbrief
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gauctiondetail
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gauctionitem
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gauctionlist
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gbattlechallenge
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gchatmember
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gchatroom
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gcountrybattledomain
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gcountrybattlelimit
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gfactionalliance
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gfactionbaseinfo
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gfactionfortressbattleinfo
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gfactionfortressbriefinfo
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gfactionfortressdetail
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gfactionfortressinfo
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gfactionfortressinfo2
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gfactionhostile
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gfactionrelation
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gfactionrelationapply
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gfactionresourcebattlerecord
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gfriendextinfo
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gfriendinfo
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\ggroupinfo
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gmail
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gmailheader
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gmailid
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gmailsyncdata
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gmplayerinfo
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\greincarnationdata
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\greincarnationrecord
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\grolebase
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\groleforbid
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\groleinventory
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\grolestorehouse
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\groomdetail
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\groupgetinfoarg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\groupgetinfores
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\groupsetinfoarg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\groupsetinfores
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gsysauctiondetail
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gsysauctionitem
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gterritory
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\guserfaction
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gwebtradedetail
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gwebtradeitem
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\gwebtraderolebrief
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\kecandidate
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\keking
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\message
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\msg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\msgretrieveofflinearg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\msgretrieveofflineres
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\playerbriefinfo
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\playerconsumeinfo
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\playerpositionresetrqstarg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\playerpositionresetrqstres
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\qqaddfriendrqstarg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\qqaddfriendrqstres
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\referralbrief
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\rewarditem
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\role
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\rolealllistarg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\rolealllistres
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\roleinfo
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\sellid
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\sellpointinfo
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\ssouser
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\tradestartrqstarg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\tradestartrqstres
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\transid
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\usergetinfoarg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\usergetinfores
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\userinfodetail
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\usersetinfoarg
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\usersetinfores
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdata\zone
# End Source File
# End Group
# Begin Group "IOLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\IOLib\gnactiveio.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gnbyteorder.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gnconf.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gnerrno.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gnmarshal.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gnnetio.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gnoctets.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gnpassiveio.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gnpollio.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gnproto.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gnrpc.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gnsecure.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gnstatic.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gnthread.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\gntimer.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\iolib.h
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\sha256.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\IOLib\sha256.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Network\callid.hxx
# End Source File
# Begin Source File

SOURCE=.\Network\chatdata.h
# End Source File
# Begin Source File

SOURCE=.\EC_C2SCmdCache.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_C2SCmdCache.h
# End Source File
# Begin Source File

SOURCE=.\Network\EC_GameDataPrtc.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\EC_GameSession.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\EC_GameSession.h
# End Source File
# Begin Source File

SOURCE=.\Network\EC_GPDataType.h
# End Source File
# Begin Source File

SOURCE=.\EC_ProtocolDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ProtocolDebug.h
# End Source File
# Begin Source File

SOURCE=.\Network\EC_PrtcProc.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\EC_Reconnect.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\EC_Reconnect.h
# End Source File
# Begin Source File

SOURCE=.\EC_SendC2SCmds.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_SendC2SCmds.h
# End Source File
# Begin Source File

SOURCE=.\Network\EC_ServerSimulate.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\EC_ServerSimulate.h
# End Source File
# Begin Source File

SOURCE=.\Network\factiondata.hxx
# End Source File
# Begin Source File

SOURCE=.\Network\forbid.hxx
# End Source File
# Begin Source File

SOURCE=.\Network\gnet.h
# End Source File
# Begin Source File

SOURCE=.\Network\gnetdef.h
# End Source File
# Begin Source File

SOURCE=.\Network\ids.hxx
# End Source File
# Begin Source File

SOURCE=.\Network\netclient.h
# End Source File
# Begin Source File

SOURCE=.\Network\privilege.hxx
# End Source File
# Begin Source File

SOURCE=.\Network\rpcdefs.h
# End Source File
# Begin Source File

SOURCE=.\Network\state.cxx
# End Source File
# Begin Source File

SOURCE=.\Network\state.hxx
# End Source File
# Begin Source File

SOURCE=.\Network\stubs.cxx
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Group "GFX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CCommon\Gfx\A3DSkillGfxComposer2.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\Gfx\A3DSkillGfxComposer2.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\Gfx\A3DSkillGfxEvent2.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\Gfx\A3DSkillGfxEvent2.h
# End Source File
# End Group
# Begin Group "AutoCa"

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
# Begin Group "Cloud"

# PROP Default_Filter ""
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

SOURCE=..\CCommon\EL_CloudSprite.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_CloudSprite.h
# End Source File
# End Group
# Begin Group "Memory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CCommon\Memory\ClientMemoryLog.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\Memory\ClientMemoryLog.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\Memory\EC_HookMemory.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\Memory\EC_HookMemory.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\EC_Algorithm.h
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

SOURCE=..\CCommon\EC_TriangleMan.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EC_TriangleMan.h
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

SOURCE=..\CCommon\EL_BrushBuilding.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_BrushBuilding.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Building.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Building.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_BuildingWithBrush.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_BuildingWithBrush.h
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

SOURCE=.\EL_RandomMapInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\EL_RandomMapInfo.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Region.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\EL_Region.h
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

SOURCE=..\CCommon\elementpckdir.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\ELOutlineQuadTree.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\ELOutlineQuadTree.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\ELOutlineTexture.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\ELOutlineTexture.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\ELTerrainOutline2.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\ELTerrainOutline2.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\ExpTypes.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\generate_item_temp.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\globaldataman.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\globaldataman.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\itemdataman.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\itemdataman.h
# End Source File
# End Group
# Begin Group "Task"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Task\EC_TaskInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\Task\EC_TaskInterface.h
# End Source File
# Begin Source File

SOURCE=.\Task\TaskClient.cpp
# End Source File
# Begin Source File

SOURCE=.\Task\TaskClient.h
# End Source File
# Begin Source File

SOURCE=.\task\TaskExpAnalyser.cpp
# End Source File
# Begin Source File

SOURCE=.\task\TaskExpAnalyser.h
# End Source File
# Begin Source File

SOURCE=.\Task\TaskInterface.h
# End Source File
# Begin Source File

SOURCE=.\Task\TaskProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\Task\TaskProcess.h
# End Source File
# Begin Source File

SOURCE=.\Task\TaskTempl.cpp
# End Source File
# Begin Source File

SOURCE=.\Task\TaskTempl.h
# End Source File
# Begin Source File

SOURCE=.\Task\TaskTempl.inl
# End Source File
# Begin Source File

SOURCE=.\Task\TaskTemplMan.cpp
# End Source File
# Begin Source File

SOURCE=.\Task\TaskTemplMan.h
# End Source File
# End Group
# Begin Group "HelpSystem Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CodeTemplate.h
# End Source File
# Begin Source File

SOURCE=.\DlgScriptHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgScriptHelp.h
# End Source File
# Begin Source File

SOURCE=.\DlgScriptHelpHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgScriptHelpHistory.h
# End Source File
# Begin Source File

SOURCE=.\DlgScriptTip.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgScriptTip.h
# End Source File
# Begin Source File

SOURCE=.\DlgSettingScriptHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSettingScriptHelp.h
# End Source File
# Begin Source File

SOURCE=.\ECParamList.cpp
# End Source File
# Begin Source File

SOURCE=.\ECParamList.h
# End Source File
# Begin Source File

SOURCE=.\ECScript.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScript.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptBoolExp.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptBoolExp.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptCheckBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptCheckBase.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptCheckImp.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptCheckImp.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptCheckState.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptCheckState.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptCheckStateInGame.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptCheckStateInGame.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptContext.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptContext.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptController.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptController.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptFunctionBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptFunctionBase.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptFunctionInGame.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptFunctionInGame.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptGlobalResource.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptGlobalResourceInGame.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptGlobalResourceInGame.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptMan.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptMan.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptManInGame.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptManInGame.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptOption.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptOption.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptOptionInGame.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptOptionInGame.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptTipWork.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptTipWork.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptTipWorkMan.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptTipWorkMan.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptUI.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptUIInGame.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptUIInGame.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptUnit.h
# End Source File
# Begin Source File

SOURCE=.\ECScriptUnitExecutor.cpp
# End Source File
# Begin Source File

SOURCE=.\ECScriptUnitExecutor.h
# End Source File
# Begin Source File

SOURCE=.\Factory.h
# End Source File
# Begin Source File

SOURCE=.\SingletonHolder.h
# End Source File
# End Group
# Begin Group "Defence"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\defence\APILoader.cpp
# End Source File
# Begin Source File

SOURCE=.\defence\APILoader.h
# End Source File
# Begin Source File

SOURCE=.\defence\cheaterkiller.cpp
# End Source File
# Begin Source File

SOURCE=.\defence\cheaterkiller.h
# End Source File
# Begin Source File

SOURCE=.\defence\defence.cpp
# End Source File
# Begin Source File

SOURCE=.\defence\defence.h
# End Source File
# Begin Source File

SOURCE=.\defence\EnumWin32Pro.cpp
# End Source File
# Begin Source File

SOURCE=.\defence\EnumWin32Pro.h
# End Source File
# Begin Source File

SOURCE=.\defence\hardinfo.h
# End Source File
# Begin Source File

SOURCE=.\defence\imagechecker.cpp
# End Source File
# Begin Source File

SOURCE=.\defence\imagechecker.h
# End Source File
# Begin Source File

SOURCE=.\defence\infocollection.h
# End Source File
# Begin Source File

SOURCE=.\defence\Mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\defence\Mouse.h
# End Source File
# Begin Source File

SOURCE=.\defence\Process.cpp
# End Source File
# Begin Source File

SOURCE=.\defence\Process.h
# End Source File
# Begin Source File

SOURCE=.\defence\pwacp.cpp
# End Source File
# Begin Source File

SOURCE=.\defence\pwacp.h
# End Source File
# Begin Source File

SOURCE=.\defence\replaceapi.cpp
# End Source File
# Begin Source File

SOURCE=.\defence\replaceapi.h
# End Source File
# Begin Source File

SOURCE=.\defence\socket_helper.cpp
# End Source File
# Begin Source File

SOURCE=.\defence\stackchecker.cpp
# End Source File
# Begin Source File

SOURCE=.\defence\stackchecker.h
# End Source File
# Begin Source File

SOURCE=.\defence\stackinfocollection.cpp
# End Source File
# Begin Source File

SOURCE=.\defence\stackinfocollection.h
# End Source File
# Begin Source File

SOURCE=.\defence\tracer.cpp
# End Source File
# End Group
# Begin Group "Home"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Home\AutoSceneFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\Home\AutoSceneFunc.h
# End Source File
# Begin Source File

SOURCE=.\Home\ClosedArea.cpp
# End Source File
# Begin Source File

SOURCE=.\Home\ClosedArea.h
# End Source File
# End Group
# Begin Group "Lua"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LuaBind.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaBind.h
# End Source File
# End Group
# Begin Group "GT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GT\gt_overlay.cpp
# End Source File
# Begin Source File

SOURCE=.\GT\gt_overlay.h
# End Source File
# Begin Source File

SOURCE=.\GT\gt_overlay_implement.cpp
# End Source File
# Begin Source File

SOURCE=.\GT\hook_game.h
# End Source File
# Begin Source File

SOURCE=.\GT\overlay.h
# End Source File
# Begin Source File

SOURCE=.\GT\overlay_export.h
# End Source File
# End Group
# Begin Group "CrossServer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CrossServer\EC_CrossServer.cpp
# End Source File
# Begin Source File

SOURCE=.\CrossServer\EC_CrossServer.h
# End Source File
# Begin Source File

SOURCE=.\CrossServer\EC_CrossServerList.cpp
# End Source File
# Begin Source File

SOURCE=.\CrossServer\EC_CrossServerList.h
# End Source File
# End Group
# Begin Group "Optimize"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgOptimizeBase.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOptimizeBase.h
# End Source File
# Begin Source File

SOURCE=.\DlgOptimizeFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOptimizeFunc.h
# End Source File
# Begin Source File

SOURCE=.\DlgOptimizeGfx.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOptimizeGfx.h
# End Source File
# Begin Source File

SOURCE=.\DlgOptimizeMem.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOptimizeMem.h
# End Source File
# Begin Source File

SOURCE=.\EC_NativeAPI.h
# End Source File
# Begin Source File

SOURCE=.\EC_Optimize.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Optimize.h
# End Source File
# End Group
# Begin Group "Arc"

# PROP Default_Filter ""
# Begin Group "Asia"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Arc\Asia\ArcAsia_API.h
# End Source File
# Begin Source File

SOURCE=.\Arc\Asia\ArcAsia_Datatype.h
# End Source File
# Begin Source File

SOURCE=.\Arc\Asia\ArcAsia_Error.h
# End Source File
# Begin Source File

SOURCE=.\Arc\Asia\EC_ArcAsia.cpp
# End Source File
# Begin Source File

SOURCE=.\Arc\Asia\EC_ArcAsia.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Arc\arc_overlay.cpp
# End Source File
# Begin Source File

SOURCE=.\Arc\arc_overlay.h
# End Source File
# Begin Source File

SOURCE=.\Arc\CC_API.h
# End Source File
# Begin Source File

SOURCE=.\Arc\CC_Datatype.h
# End Source File
# Begin Source File

SOURCE=.\Arc\CC_Error.h
# End Source File
# End Group
# Begin Group "AutoPolicy"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_AutoPolicy.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_AutoPolicy.h
# End Source File
# Begin Source File

SOURCE=.\EC_PlayerAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_PlayerWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_PlayerWrapper.h
# End Source File
# End Group
# Begin Group "Model Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_BaseColor.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_BaseColor.h
# End Source File
# Begin Source File

SOURCE=.\EC_ComboSkillState.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ComboSkillState.h
# End Source File
# Begin Source File

SOURCE=.\EC_FactionPVP.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_FactionPVP.h
# End Source File
# Begin Source File

SOURCE=.\EC_FashionShop.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_FashionShop.h
# End Source File
# Begin Source File

SOURCE=.\EC_HostSkillModel.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_HostSkillModel.h
# End Source File
# Begin Source File

SOURCE=.\EC_LoginQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_LoginQueue.h
# End Source File
# Begin Source File

SOURCE=.\EC_PlayerLevelRankRealmChangeCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_PlayerLevelRankRealmChangeCheck.h
# End Source File
# Begin Source File

SOURCE=.\EC_ShoppingCart.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ShoppingCart.h
# End Source File
# Begin Source File

SOURCE=.\EC_SkillConvert.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_SkillConvert.h
# End Source File
# Begin Source File

SOURCE=.\EC_TaoistRank.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_TaoistRank.h
# End Source File
# End Group
# Begin Group "Patterns"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_Handler.h
# End Source File
# Begin Source File

SOURCE=.\EC_Observer.h
# End Source File
# Begin Source File

SOURCE=.\EC_StubbornNetSender.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_StubbornNetSender.h
# End Source File
# Begin Source File

SOURCE=.\EC_UniversalTokenVisitHTTPCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_UniversalTokenVisitHTTPCommand.h
# End Source File
# Begin Source File

SOURCE=.\EC_UseUniversalToken.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_UseUniversalToken.h
# End Source File
# End Group
# Begin Group "Controller Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_ShoppingCartChecker.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ShoppingCartChecker.h
# End Source File
# Begin Source File

SOURCE=.\EC_ShoppingController.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ShoppingController.h
# End Source File
# Begin Source File

SOURCE=.\EC_ShoppingManager.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_ShoppingManager.h
# End Source File
# End Group
# End Target
# End Project
