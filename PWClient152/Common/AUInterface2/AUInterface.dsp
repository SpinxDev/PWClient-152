# Microsoft Developer Studio Project File - Name="AUInterface" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=AUInterface - Win32 Mem_Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AUInterface.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AUInterface.mak" CFG="AUInterface - Win32 Mem_Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AUInterface - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AUInterface - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AUInterface - Win32 DbgRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "AUInterface - Win32 Mem_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AUInterface2", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AUInterface - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\ClientTools\LuaWrapper\Include" /I "..\include\luawrapper" /I "..\LuaWrapperExport\Include" /I "..\LuaWrapper\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_ANGELICA2" /D _WIN32_WINNT=0x0400 /FR /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Lib\AUInterface.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy *.h ..\include\*.h /R /Y /Q	xcopy lib\*.lib ..\lib\*.lib /R /Y /Q
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\ClientTools\LuaWrapper\Include" /I "..\include\luawrapper" /I "..\LuaWrapperExport\Include" /I "..\LuaWrapper\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_ANGELICA2" /D _WIN32_WINNT=0x0400 /Fr /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Lib\AUInterface_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy *.h ..\include\*.h /R /Y /Q	xcopy lib\*.lib ..\lib\*.lib /R /Y /Q
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AUInterface___Win32_DbgRelease"
# PROP BASE Intermediate_Dir "AUInterface___Win32_DbgRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DbgRelease"
# PROP Intermediate_Dir "DbgRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Output\A3DSDK\Include" /I "..\Output\AFSDK\Include" /I "..\Output\ACSDK\Include" /I "..\Output\AMSDK\Include" /I "..\Output\3rdSDK\Include" /I "F:\DirectX 8.1 SDK\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I "../include/gfx" /I "..\..\ClientTools\LuaWrapper\Include" /I "..\include\luawrapper" /I "..\LuaWrapperExport\Include" /I "..\LuaWrapper\Include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_ANGELICA2" /D _WIN32_WINNT=0x0400 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Lib\AUInterface.lib"
# ADD LIB32 /nologo /out:"Lib\AUInterface_su.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy *.h ..\include\*.h /R /Y /Q	xcopy lib\*.lib ..\lib\*.lib /R /Y /Q
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AUInterface___Win32_Mem_Debug"
# PROP BASE Intermediate_Dir "AUInterface___Win32_Mem_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Mem_Debug"
# PROP Intermediate_Dir "Mem_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /YX /FD /GZ /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\LuaWrapper\Include" /I "..\include\luawrapper" /I "..\LuaWrapperExport\Include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_A_FORBID_NEWDELETE" /D "_A_FORBID_MALLOC" /D "_ANGELICA2" /D _WIN32_WINNT=0x0400 /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Lib\AUInterface_d.lib"
# ADD LIB32 /nologo /out:"Lib\AUInterface_smd.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy *.h ..\include\*.h /R /Y /Q	xcopy lib\*.lib ..\lib\*.lib /R /Y /Q
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AUInterface - Win32 Release"
# Name "AUInterface - Win32 Debug"
# Name "AUInterface - Win32 DbgRelease"
# Name "AUInterface - Win32 Mem_Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\A2DDotLine.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\A2DDotLineMan.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\A3DAlterableSkinModel.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\A3DFTFont.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\A3DFTFontMan.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AStringWithWildcard.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUI.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yc"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yc"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yc"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUICheckBox.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIClockIcon.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIComboBox.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUICommon.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIConsole.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUICTranslate.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUICustomize.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIDialog.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIEditBox.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIFrame.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIImagePicture.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIImeHook.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUILabel.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIListBox.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUILuaDialog.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUILuaManager.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIManager.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIModelPicture.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIObject.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIObjectTemplateManager.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.H"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIPicture.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIProgress.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIRadioButton.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIScroll.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUISlider.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIStillImageButton.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUISubDialog.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUITag.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.H"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUITextArea.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUITreeView.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIVerticalText.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AUIWindowPicture.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AXMLFile.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CSplit.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ImeUi.cpp
# End Source File
# Begin Source File

SOURCE=.\IMEWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\LuaDlgApi.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UIRenderTarget.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WndThreadManager.cpp

!IF  "$(CFG)" == "AUInterface - Win32 Release"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Debug"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 DbgRelease"

# ADD CPP /Yu"AUI.h"

!ELSEIF  "$(CFG)" == "AUInterface - Win32 Mem_Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\A2DDotLine.h
# End Source File
# Begin Source File

SOURCE=.\A2DDotLineMan.h
# End Source File
# Begin Source File

SOURCE=.\A3DAlterableSkinModel.h
# End Source File
# Begin Source File

SOURCE=.\A3DFTFont.h
# End Source File
# Begin Source File

SOURCE=.\A3DFTFontMan.h
# End Source File
# Begin Source File

SOURCE=.\AStringWithWildcard.h
# End Source File
# Begin Source File

SOURCE=.\AUI.h
# End Source File
# Begin Source File

SOURCE=.\AUICheckBox.h
# End Source File
# Begin Source File

SOURCE=.\AUIClockIcon.h
# End Source File
# Begin Source File

SOURCE=.\AUIComboBox.h
# End Source File
# Begin Source File

SOURCE=.\AUICommon.h
# End Source File
# Begin Source File

SOURCE=.\AUIConsole.h
# End Source File
# Begin Source File

SOURCE=.\AUICTranslate.h
# End Source File
# Begin Source File

SOURCE=.\AUICustomize.h
# End Source File
# Begin Source File

SOURCE=.\AUIDef.h
# End Source File
# Begin Source File

SOURCE=.\AUIDialog.h
# End Source File
# Begin Source File

SOURCE=.\AUIEditBox.h
# End Source File
# Begin Source File

SOURCE=.\AUIFrame.h
# End Source File
# Begin Source File

SOURCE=.\AUIImagePicture.h
# End Source File
# Begin Source File

SOURCE=.\AUIImeHook.h
# End Source File
# Begin Source File

SOURCE=.\AUILabel.h
# End Source File
# Begin Source File

SOURCE=.\AUIListBox.h
# End Source File
# Begin Source File

SOURCE=.\AUILuaDialog.h
# End Source File
# Begin Source File

SOURCE=.\AUILuaManager.h
# End Source File
# Begin Source File

SOURCE=.\AUIManager.h
# End Source File
# Begin Source File

SOURCE=.\AUIModelPicture.h
# End Source File
# Begin Source File

SOURCE=.\AUIObject.h
# End Source File
# Begin Source File

SOURCE=.\AUIObjectTemplateManager.h
# End Source File
# Begin Source File

SOURCE=.\AUIPicture.h
# End Source File
# Begin Source File

SOURCE=.\AUIProgress.h
# End Source File
# Begin Source File

SOURCE=.\AUIRadioButton.h
# End Source File
# Begin Source File

SOURCE=.\AUIScroll.h
# End Source File
# Begin Source File

SOURCE=.\AUISlider.h
# End Source File
# Begin Source File

SOURCE=.\AUIStillImageButton.h
# End Source File
# Begin Source File

SOURCE=.\AUISubDialog.h
# End Source File
# Begin Source File

SOURCE=.\AUITag.h
# End Source File
# Begin Source File

SOURCE=.\AUITextArea.h
# End Source File
# Begin Source File

SOURCE=.\AUITreeView.h
# End Source File
# Begin Source File

SOURCE=.\AUIVerticalText.h
# End Source File
# Begin Source File

SOURCE=.\AUIWindowPicture.h
# End Source File
# Begin Source File

SOURCE=.\AXMLFile.h
# End Source File
# Begin Source File

SOURCE=.\CSplit.h
# End Source File
# Begin Source File

SOURCE=.\FTInterface.h
# End Source File
# Begin Source File

SOURCE=.\ImeUi.h
# End Source File
# Begin Source File

SOURCE=.\IMEWrapper.h
# End Source File
# Begin Source File

SOURCE=.\LuaDlgApi.h
# End Source File
# Begin Source File

SOURCE=.\UIRenderTarget.h
# End Source File
# Begin Source File

SOURCE=.\WndThreadManager.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\updatelog.txt
# End Source File
# End Target
# End Project
