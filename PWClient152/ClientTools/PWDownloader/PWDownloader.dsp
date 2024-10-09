# Microsoft Developer Studio Project File - Name="PWDownloader" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PWDownloader - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PWDownloader.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PWDownloader.mak" CFG="PWDownloader - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PWDownloader - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PWDownloader - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CElement/ClientTools/PWDownloader", PVCBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./7z/cpp" /I "../../../CNewSkin/p2sp_4th_lib" /I "../../../CNewSkin/include" /I "../../../../SDK/3rdSDK/include" /I "../../../../SDK/acsdk/Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 gdiplus.lib /nologo /subsystem:windows /machine:I386 /libpath:"../../../CNewSkin/lib"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./7z/cpp" /I "../../../CNewSkin/p2sp_4th_lib" /I "../../../CNewSkin/include" /I "../../../../SDK/3rdSDK/include" /I "../../../../SDK/acsdk/Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 gdiplus.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/PWDownloader_d.exe" /pdbtype:sept /libpath:"../../../CNewSkin/lib"

!ENDIF 

# Begin Target

# Name "PWDownloader - Win32 Release"
# Name "PWDownloader - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DlgPreInstall.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSetSpeed.cpp
# End Source File
# Begin Source File

SOURCE=.\PWDownloader.cpp
# End Source File
# Begin Source File

SOURCE=.\PWDownloader.rc
# End Source File
# Begin Source File

SOURCE=.\PWDownloaderDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PWInstallDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DlgPreInstall.h
# End Source File
# Begin Source File

SOURCE=.\DlgSetSpeed.h
# End Source File
# Begin Source File

SOURCE=.\PWDownloader.h
# End Source File
# Begin Source File

SOURCE=.\PWDownloaderDlg.h
# End Source File
# Begin Source File

SOURCE=.\PWInstallDlg.h
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

SOURCE=.\res\pw_install.png
# End Source File
# Begin Source File

SOURCE=.\res\PWDownloader.ico
# End Source File
# Begin Source File

SOURCE=.\res\PWDownloader.rc2
# End Source File
# End Group
# Begin Group "7Z Lib"

# PROP Default_Filter ""
# Begin Group "Windows"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\7z\CPP\Windows\DLL.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\DLL.h
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\Error.cpp
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\Error.h
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\FileDir.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\FileDir.h
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\FileFind.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\FileFind.h
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\FileIO.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\FileIO.h
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\FileName.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\FileName.h
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\PropVariant.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\PropVariant.h
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\PropVariantConversions.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Windows\PropVariantConversions.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\7z\CPP\7zip\Common\FileStreams.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\7zip\Common\FileStreams.h
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Common\IntToString.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Common\IntToString.h
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Common\MyString.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Common\MyString.h
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Common\MyVector.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Common\MyVector.h
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Common\NewHandler.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Common\NewHandler.h
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Common\StringConvert.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Common\StringConvert.h
# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Common\Wildcard.cpp

!IF  "$(CFG)" == "PWDownloader - Win32 Release"

!ELSEIF  "$(CFG)" == "PWDownloader - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\7z\CPP\Common\Wildcard.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\EC_Archive7Z.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\EC_Archive7Z.h
# End Source File
# End Group
# Begin Group "Download"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\CNewSkin\patchlib\EC_HttpGet.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\..\CNewSkin\patchlib\EC_HttpGet.h
# End Source File
# Begin Source File

SOURCE=.\EC_Pipe.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\EC_Pipe.h
# End Source File
# Begin Source File

SOURCE=.\EC_PWDownload.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\EC_PWDownload.h
# End Source File
# Begin Source File

SOURCE=.\LogFile.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\LogFile.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
