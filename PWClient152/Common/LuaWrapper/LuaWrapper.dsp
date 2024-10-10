# Microsoft Developer Studio Project File - Name="LuaWrapper" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LuaWrapper - Win32 S Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LuaWrapper.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LuaWrapper.mak" CFG="LuaWrapper - Win32 S Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LuaWrapper - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LuaWrapper - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LuaWrapper - Win32 DbgRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "LuaWrapper - Win32 Debug_Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "LuaWrapper - Win32 Release_Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "LuaWrapper - Win32 S Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LuaWrapper - Win32 S Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/LuaWrapper", NYNAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LuaWrapper - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy release\*.lib lib\ /R /Y /Q	xcopy debug\*.lib lib\ /R /Y /Q	xcopy LuaApi.h include\ /R /Y /Q	xcopy LuaMemTbl.h include\ /R /Y /Q	xcopy LuaScript.h include\ /R /Y /Q	xcopy LuaState.h include\ /R /Y /Q	xcopy LuaUtil.h include\ /R /Y /Q	xcopy ScriptValue.h include\ /R /Y /Q
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LuaWrapper - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\LuaWrapper_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy release\*.lib lib\ /R /Y /Q	xcopy debug\*.lib lib\ /R /Y /Q	xcopy LuaApi.h include\ /R /Y /Q	xcopy LuaMemTbl.h include\ /R /Y /Q	xcopy LuaScript.h include\ /R /Y /Q	xcopy LuaState.h include\ /R /Y /Q	xcopy LuaUtil.h include\ /R /Y /Q	xcopy ScriptValue.h include\ /R /Y /Q
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LuaWrapper - Win32 DbgRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LuaWrapper___Win32_DbgRelease"
# PROP BASE Intermediate_Dir "LuaWrapper___Win32_DbgRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgRelease"
# PROP Intermediate_Dir "DbgRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\LuaWrapper_dr.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy release\*.lib lib\ /R /Y /Q	xcopy debug\*.lib lib\ /R /Y /Q	xcopy LuaApi.h include\ /R /Y /Q	xcopy LuaMemTbl.h include\ /R /Y /Q	xcopy LuaScript.h include\ /R /Y /Q	xcopy LuaState.h include\ /R /Y /Q	xcopy LuaUtil.h include\ /R /Y /Q	xcopy ScriptValue.h include\ /R /Y /Q
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LuaWrapper - Win32 Debug_Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LuaWrapper___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "LuaWrapper___Win32_Debug_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode"
# PROP Intermediate_Dir "Debug_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_LIB" /D "WIN32" /D "_UNICODE" /D "UNICODE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\LuaWrapper_d.lib"
# ADD LIB32 /nologo /out:"Debug\LuaWrapper_du.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy release\*.lib lib\ /R /Y /Q	xcopy debug\*.lib lib\ /R /Y /Q	xcopy LuaApi.h include\ /R /Y /Q	xcopy LuaMemTbl.h include\ /R /Y /Q	xcopy LuaScript.h include\ /R /Y /Q	xcopy LuaState.h include\ /R /Y /Q	xcopy LuaUtil.h include\ /R /Y /Q	xcopy ScriptValue.h include\ /R /Y /Q
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LuaWrapper - Win32 Release_Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LuaWrapper___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "LuaWrapper___Win32_Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Unicode"
# PROP Intermediate_Dir "Release_Unicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\LuaWrapper_u.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy release\*.lib lib\ /R /Y /Q	xcopy debug\*.lib lib\ /R /Y /Q	xcopy LuaApi.h include\ /R /Y /Q	xcopy LuaMemTbl.h include\ /R /Y /Q	xcopy LuaScript.h include\ /R /Y /Q	xcopy LuaState.h include\ /R /Y /Q	xcopy LuaUtil.h include\ /R /Y /Q	xcopy ScriptValue.h include\ /R /Y /Q
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LuaWrapper - Win32 S Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LuaWrapper___Win32_S_Debug"
# PROP BASE Intermediate_Dir "LuaWrapper___Win32_S_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "LuaWrapper___Win32_S_Debug"
# PROP Intermediate_Dir "LuaWrapper___Win32_S_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\LuaWrapper_d.lib"
# ADD LIB32 /nologo /out:"Debug\LuaWrapper_sd.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy release\*.lib lib\ /R /Y /Q	xcopy debug\*.lib lib\ /R /Y /Q	xcopy LuaApi.h include\ /R /Y /Q	xcopy LuaMemTbl.h include\ /R /Y /Q	xcopy LuaScript.h include\ /R /Y /Q	xcopy LuaState.h include\ /R /Y /Q	xcopy LuaUtil.h include\ /R /Y /Q	xcopy ScriptValue.h include\ /R /Y /Q
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LuaWrapper - Win32 S Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LuaWrapper___Win32_S_Release"
# PROP BASE Intermediate_Dir "LuaWrapper___Win32_S_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LuaWrapper___Win32_S_Release"
# PROP Intermediate_Dir "LuaWrapper___Win32_S_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\LuaWrapper_s.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy release\*.lib lib\ /R /Y /Q	xcopy debug\*.lib lib\ /R /Y /Q	xcopy LuaApi.h include\ /R /Y /Q	xcopy LuaMemTbl.h include\ /R /Y /Q	xcopy LuaScript.h include\ /R /Y /Q	xcopy LuaState.h include\ /R /Y /Q	xcopy LuaUtil.h include\ /R /Y /Q	xcopy ScriptValue.h include\ /R /Y /Q
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "LuaWrapper - Win32 Release"
# Name "LuaWrapper - Win32 Debug"
# Name "LuaWrapper - Win32 DbgRelease"
# Name "LuaWrapper - Win32 Debug_Unicode"
# Name "LuaWrapper - Win32 Release_Unicode"
# Name "LuaWrapper - Win32 S Debug"
# Name "LuaWrapper - Win32 S Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Debugger.cpp
# End Source File
# Begin Source File

SOURCE=.\LD_NameValue.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaDebugHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaDebugIPC.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaDebugMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaMemTbl.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaScript.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaState.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptValue.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ATString.h
# End Source File
# Begin Source File

SOURCE=.\Debugger.h
# End Source File
# Begin Source File

SOURCE=.\LD_NameValue.h
# End Source File
# Begin Source File

SOURCE=.\LuaAPI.h
# End Source File
# Begin Source File

SOURCE=.\LuaDebug.h
# End Source File
# Begin Source File

SOURCE=.\LuaDebugHelper.h
# End Source File
# Begin Source File

SOURCE=.\LuaDebugIPC.h
# End Source File
# Begin Source File

SOURCE=.\LuaDebugMsg.h
# End Source File
# Begin Source File

SOURCE=.\LuaMemTbl.h
# End Source File
# Begin Source File

SOURCE=.\LuaScript.h
# End Source File
# Begin Source File

SOURCE=.\LuaState.h
# End Source File
# Begin Source File

SOURCE=.\LuaUtil.h
# End Source File
# Begin Source File

SOURCE=.\ScriptValue.h
# End Source File
# End Group
# End Target
# End Project
