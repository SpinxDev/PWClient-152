# Microsoft Developer Studio Project File - Name="FileExtract" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FileExtract - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FileExtract.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FileExtract.mak" CFG="FileExtract - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FileExtract - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FileExtract - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "FileExtract - Win32 DbgRelease" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CElement/ClientTools/FileExtract", VTCBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FileExtract - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../../CCommon" /I "../../CElementData" /I "../../Include" /I "../../Include/LuaWrapper" /I "..\..\..\..\SDK\3rdSDK\include" /I "..\..\..\..\SDK\A3DSDK\Include" /I "..\..\..\..\SDK\acsdk\Include" /I "..\..\..\..\SDK\afsdk\include" /I "..\..\..\..\SDK\Amsdk\include" /I "..\..\..\..\SDK\Apsdk\Include" /I "..\..\..\..\GfxCommon\Output\include" /I "..\..\AUInterface2" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ElementSkill.lib FWEditorLib.lib zlibwapi.lib GfxCommon.lib AUInterface.lib FTDriver.lib CHBasedCD.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_u.lib AngelicaMedia_u.lib AngelicaFile_u.lib Angelica3D_u.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib glu32.lib ddraw.lib dsound.lib shlwapi.lib glu32.lib lua5.1.mt.lib LuaWrapper_u.lib dbghelp.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /libpath:"../../Lib" /libpath:"../../Lib/cd" /libpath:"../../Lib/AUI" /libpath:"../../Lib/stlport" /libpath:"..\..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\..\AUInterface2\Lib" /libpath:"..\..\..\..\GfxCommon\Output\Lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "FileExtract - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../../CCommon" /I "../../CElementData" /I "../../Include" /I "../../Include/LuaWrapper" /I "..\..\..\..\SDK\3rdSDK\include" /I "..\..\..\..\SDK\A3DSDK\Include" /I "..\..\..\..\SDK\acsdk\Include" /I "..\..\..\..\SDK\afsdk\include" /I "..\..\..\..\SDK\Amsdk\include" /I "..\..\..\..\SDK\Apsdk\Include" /I "..\..\..\..\GfxCommon\Output\include" /I "..\..\AUInterface2" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ElementSkill_d.lib FWEditorLib_d.lib zlibwapi.lib GfxCommon_d.lib AUInterface_d.lib FTDriver_D.lib CHBasedCD_d.lib mpg123lib_d.lib Immwrapper_d.lib AngelicaCommon_ud.lib AngelicaMedia_ud.lib AngelicaFile_ud.lib Angelica3D_ud.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib glu32.lib ddraw.lib dsound.lib shlwapi.lib glu32.lib lua5.1.mtd.lib LuaWrapper_du.lib dbghelp.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"Debug/FileExtract_d.exe" /pdbtype:sept /libpath:"../../Lib" /libpath:"../../Lib/cd" /libpath:"../../Lib/AUI" /libpath:"../../Lib/stlport" /libpath:"..\..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\..\AUInterface2\Lib" /libpath:"..\..\..\..\GfxCommon\Output\Lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "FileExtract - Win32 DbgRelease"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FileExtract___Win32_DbgRelease"
# PROP BASE Intermediate_Dir "FileExtract___Win32_DbgRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgRelease"
# PROP Intermediate_Dir "DbgRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "../../CCommon" /I "../../Include/LuaWrapper" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../../CCommon" /I "../../CElementData" /I "../../Include" /I "../../Include/LuaWrapper" /I "..\..\..\..\SDK\3rdSDK\include" /I "..\..\..\..\SDK\A3DSDK\Include" /I "..\..\..\..\SDK\acsdk\Include" /I "..\..\..\..\SDK\afsdk\include" /I "..\..\..\..\SDK\Amsdk\include" /I "..\..\..\..\SDK\Apsdk\Include" /I "..\..\..\..\GfxCommon\Output\include" /I "..\..\AUInterface2" /I "../../CFWEditorLib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 zlibwapi.lib GfxCommon.lib AUInterface.lib CHBasedCD.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_u.lib AngelicaMedia_u.lib AngelicaFile_u.lib Angelica3D_u.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib glu32.lib ddraw.lib dsound.lib shlwapi.lib glu32.lib lua5.1.mt.lib LuaWrapper_u.lib dbghelp.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /libpath:"../../Lib" /libpath:"../../Lib/cd"
# ADD LINK32 ElementSkill.lib FWEditorLib.lib zlibwapi.lib GfxCommon_su.lib AUInterface_su.lib FTDriver.lib CHBasedCD_su.lib mpg123lib.lib Immwrapper.lib AngelicaCommon_dr.lib AngelicaMedia_dr.lib AngelicaFile_dr.lib Angelica3D_dr.lib Imm32.lib zliblib.lib dxguid.lib d3d8.lib d3dx8.lib glu32.lib ddraw.lib dsound.lib shlwapi.lib glu32.lib lua5.1.mt.lib LuaWrapper_u.lib dbghelp.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /libpath:"../../Lib" /libpath:"../../Lib/cd" /libpath:"../../Lib/AUI" /libpath:"../../Lib/stlport" /libpath:"..\..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\..\SDK\Amsdk\lib" /libpath:"..\..\..\..\SDK\Apsdk\Lib" /libpath:"..\..\..\..\SDK\3rdSDK\lib\IMMLib" /libpath:"..\..\..\..\SDK\3rdSDK\lib\dshowbase" /libpath:"..\..\AUInterface2\Lib" /libpath:"..\..\..\..\GfxCommon\Output\Lib"

!ENDIF 

# Begin Target

# Name "FileExtract - Win32 Release"
# Name "FileExtract - Win32 Debug"
# Name "FileExtract - Win32 DbgRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\FileAnalyse.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\FileExtract.cpp

!IF  "$(CFG)" == "FileExtract - Win32 Release"

!ELSEIF  "$(CFG)" == "FileExtract - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "FileExtract - Win32 DbgRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FileExtractDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelOptionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\FileAnalyse.h
# End Source File
# Begin Source File

SOURCE=.\FileExtract.h
# End Source File
# Begin Source File

SOURCE=.\FileExtractDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModelOptionDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\FileExtract.ico
# End Source File
# Begin Source File

SOURCE=.\FileExtract.rc
# End Source File
# Begin Source File

SOURCE=.\res\FileExtract.rc2
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# End Group
# Begin Group "Utility"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EC_Utility.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\EC_Utility.h
# End Source File
# Begin Source File

SOURCE=..\..\CCommon\elementdataman.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\CCommon\elementdataman.h
# End Source File
# Begin Source File

SOURCE=..\..\CCommon\ExpTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\CElementData\Policy.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\CElementData\Policy.h
# End Source File
# Begin Source File

SOURCE=..\..\CElementData\PolicyType.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
