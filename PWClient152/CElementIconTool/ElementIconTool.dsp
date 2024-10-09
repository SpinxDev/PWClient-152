# Microsoft Developer Studio Project File - Name="ElementIconTool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ElementIconTool - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ElementIconTool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ElementIconTool.mak" CFG="ElementIconTool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ElementIconTool - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ElementIconTool - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CElement/CElementIconTool", ICXAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ElementIconTool - Win32 Release"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CElementClient\Network" /I "..\CElementClient\Network\IOLib" /I "..\CElementClient\Network\rpcdata" /I ".\include" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 libpng.lib Dbghelp.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib d3d8.lib d3dx8.lib dsound.lib dxguid.lib AngelicaCommon_u.lib Angelica3D.lib Immwrapper.lib mpg123lib.lib AngelicaFile_u.lib zliblib.lib ElementSkill.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"..\CBin\ElementIconTool.exe" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport" /libpath:".\lib" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib"

!ELSEIF  "$(CFG)" == "ElementIconTool - Win32 Debug"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CElementClient\Network" /I "..\CElementClient\Network\IOLib" /I "..\CElementClient\Network\rpcdata" /I ".\include" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /I "..\..\..\SDK\Amsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "_AFXDLL" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libpng.lib Dbghelp.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib d3d8.lib d3dx8.lib dsound.lib dxguid.lib AngelicaCommon_ud.lib Angelica3D_d.lib Immwrapper_d.lib mpg123lib_d.lib AngelicaFile_ud.lib zliblib.lib ElementSkill_d.lib dbghelp.lib shlwapi.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"..\CBin\ElementIconTool_d.exe" /pdbtype:sept /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport" /libpath:".\lib" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaMedia\DBGRelease" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\afsdk\lib" /libpath:"..\..\..\SDK\Amsdk\lib"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "ElementIconTool - Win32 Release"
# Name "ElementIconTool - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\EC_Global.cpp
# End Source File
# Begin Source File

SOURCE=.\EC_Global.h
# End Source File
# Begin Source File

SOURCE=.\ElementIconTool.cpp
# End Source File
# Begin Source File

SOURCE=.\ElementIconTool.rc
# End Source File
# Begin Source File

SOURCE=.\ElementIconToolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\CCommon\elementdataman.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\elementdataman.h
# End Source File
# Begin Source File

SOURCE=.\ElementIconTool.h
# End Source File
# Begin Source File

SOURCE=.\ElementIconToolDlg.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\itemdataman.cpp
# End Source File
# Begin Source File

SOURCE=..\CCommon\itemdataman.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ElementIconTool.ico
# End Source File
# Begin Source File

SOURCE=.\res\ElementIconTool.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
