# Microsoft Developer Studio Project File - Name="ElementHintToolServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ElementHintToolServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ElementHintToolServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ElementHintToolServer.mak" CFG="ElementHintToolServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ElementHintToolServer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ElementHintToolServer - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ElementHintToolServer - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CElementClient\Network" /I "..\CElementClient\Network\IOLib" /I "..\CElementClient\Network\rpcdata" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "UNICODE" /FR /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 AngelicaCommon_u.lib zliblib.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib zlibwapi.lib /nologo /subsystem:console /machine:I386 /out:"..\CBin\ElementHintToolServer.exe" /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\afsdk\lib"

!ELSEIF  "$(CFG)" == "ElementHintToolServer - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\Include" /I "..\Include\stlport" /I "..\CCommon" /I "..\CElementClient\Network" /I "..\CElementClient\Network\IOLib" /I "..\CElementClient\Network\rpcdata" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\afsdk\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "UNICODE" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 AngelicaCommon_ud.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib zliblib.lib zlibwapi.lib dbghelp.lib shlwapi.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\CBin\ElementHintToolServer_d.exe" /pdbtype:sept /libpath:"..\Lib" /libpath:"..\Lib\IOLib" /libpath:"..\Lib\stlport" /libpath:"..\..\..\SDK\DbgRelease\Angelica3D\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaCommon\DBGRelease" /libpath:"..\..\..\SDK\DbgRelease\AngelicaFile\DBGRelease" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\afsdk\lib"

!ENDIF 

# Begin Target

# Name "ElementHintToolServer - Win32 Release"
# Name "ElementHintToolServer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ElementHintToolServer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
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
# Begin Group "GTransform"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GTransform\callid.hxx
# End Source File
# Begin Source File

SOURCE=.\GTransform\getitemdesc
# End Source File
# Begin Source File

SOURCE=.\GTransform\getitemdesc.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransform\getitemdesc_re
# End Source File
# Begin Source File

SOURCE=.\GTransform\getitemdesc_re.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransform\gtransformserver.cpp
# End Source File
# Begin Source File

SOURCE=.\GTransform\gtransformserver.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransform\keepalive
# End Source File
# Begin Source File

SOURCE=.\GTransform\keepalive.hpp
# End Source File
# Begin Source File

SOURCE=.\GTransform\state.cxx
# End Source File
# Begin Source File

SOURCE=.\GTransform\state.hxx
# End Source File
# Begin Source File

SOURCE=.\GTransform\stubs.cxx
# End Source File
# Begin Source File

SOURCE=.\GTransform\toolannounceaid
# End Source File
# Begin Source File

SOURCE=.\GTransform\toolannounceaid.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
