# Microsoft Developer Studio Project File - Name="ClientSimulator" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ClientSimulator - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ClientSimulator.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ClientSimulator.mak" CFG="ClientSimulator - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ClientSimulator - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ClientSimulator - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CElement/ClientSimulator", FQABAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ClientSimulator - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\CCommon" /I "..\CElementClient" /I "..\CElementClient\Network" /I "..\CElementClient\Network\inl" /I ".\IOLib" /I "..\CElementClient\Network\rpcdata" /I "..\CElementSkill" /I "..\Include" /I "..\Include\stlport" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\afsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /Zm150 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 dbghelp.lib shlwapi.lib lua5.1.mt.lib LuaWrapper_u.lib ElementSkill.lib zlibwapi.lib ws2_32.lib stlport_vc6_static.lib AngelicaCommon.lib AngelicaFile.lib zliblib.lib /nologo /subsystem:windows /machine:I386 /libpath:"..\Lib" /libpath:"..\Lib\stlport" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib"

!ELSEIF  "$(CFG)" == "ClientSimulator - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\CCommon" /I "..\CElementClient" /I "..\CElementClient\Network" /I "..\CElementClient\Network\inl" /I ".\IOLib" /I "..\CElementClient\Network\rpcdata" /I "..\CElementSkill" /I "..\Include" /I "..\Include\stlport" /I "..\..\..\SDK\acsdk\Include" /I "..\..\..\SDK\A3DSDK\Include" /I "..\..\..\SDK\3rdSDK\include" /I "..\..\..\SDK\afsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /Zm150 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dbghelp.lib shlwapi.lib lua5.1.mtd.lib LuaWrapper_du.lib ElementSkill_d.lib zlibwapi.lib ws2_32.lib stlport_vc6_static.lib AngelicaCommon_d.lib AngelicaFile_d.lib zliblib.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"MSVCRT" /out:"Debug/ClientSimulator_d.exe" /pdbtype:sept /libpath:"..\Lib" /libpath:"..\Lib\stlport" /libpath:"..\..\..\SDK\3rdSDK\lib" /libpath:"..\..\..\SDK\A3DSDK\Lib" /libpath:"..\..\..\SDK\acsdk\lib" /libpath:"..\..\..\SDK\afsdk\lib"

!ENDIF 

# Begin Target

# Name "ClientSimulator - Win32 Release"
# Name "ClientSimulator - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ClientSimulator.ico
# End Source File
# Begin Source File

SOURCE=.\ClientSimulator.rc
# End Source File
# Begin Source File

SOURCE=.\res\ClientSimulator.rc2
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# End Group
# Begin Group "MFC Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ClientSimulator.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientSimulator.h
# End Source File
# Begin Source File

SOURCE=.\ClientSimulatorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientSimulatorDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Main Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CmdManager.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\CmdManager.h
# End Source File
# Begin Source File

SOURCE=.\CmdProc.cpp

!IF  "$(CFG)" == "ClientSimulator - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ClientSimulator - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GameObject.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\GameObject.h
# End Source File
# Begin Source File

SOURCE=.\GameSession.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\GameSession.h
# End Source File
# Begin Source File

SOURCE=.\Global.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\MsgManager.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MsgManager.h
# End Source File
# Begin Source File

SOURCE=.\SessionManager.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SessionManager.h
# End Source File
# End Group
# Begin Group "Object Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AIObject.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\AIObject.h
# End Source File
# Begin Source File

SOURCE=.\EC_Skill.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\EC_Skill.h
# End Source File
# Begin Source File

SOURCE=.\MoveAgent.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MoveAgent.h
# End Source File
# Begin Source File

SOURCE=.\PlayerObject.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\PlayerObject.h
# End Source File
# End Group
# Begin Group "Utility"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CElementClient\AMiniDump.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\CElementClient\AMiniDump.h
# End Source File
# Begin Source File

SOURCE=.\Common.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Common.h
# End Source File
# Begin Source File

SOURCE=.\cslock.h
# End Source File
# Begin Source File

SOURCE=.\CustomizeData.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\CustomizeData.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\EC_StringTab.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\CElementClient\EC_StringTab.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\elementdataman.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\CCommon\elementdataman.h
# End Source File
# Begin Source File

SOURCE=..\CCommon\ExpTypes.h
# End Source File
# Begin Source File

SOURCE=.\luaFunc.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\luaFunc.h
# End Source File
# Begin Source File

SOURCE=.\zgm_luadebug.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zgm_luadebug.h
# End Source File
# End Group
# Begin Group "API Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameApi.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\PlayerApi.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\UtilApi.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Group "Protocol Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CElementClient\Network\acanswer.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\accountloginrecord.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\acquestion.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\acremotecode.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\acreport.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\addfriend.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\addfriend_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\addfriendrqst.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\announceforbidinfo.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\announcenewmail.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\announcesellresult.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionattendlist.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionattendlist_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionbid.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionbid_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionclose.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionclose_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionexitbid.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionexitbid_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionget.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionget_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctiongetitem.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctiongetitem_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionlist.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionlist_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionlistupdate.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionlistupdate_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionopen.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\auctionopen_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\autolockset.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\autolockset_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\battlechallenge.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\battlechallenge_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\battlechallengemap.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\battlechallengemap_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\battleenter.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\battleenter_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\battlegetmap.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\battlegetmap_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\battlestatus.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\battlestatus_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\buypoint.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\buypoint_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\cashlock.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\cashlock_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\cashpasswordset.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\cashpasswordset_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\challenge.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\changeds_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\chatmessage.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\chatroomcreate.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\chatroomcreate_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\chatroomexpel.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\chatroominvite.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\chatroominvite_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\chatroomjoin.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\chatroomjoin_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\chatroomleave.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\chatroomlist.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\chatroomlist_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\chatroomspeak.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\checknewmail.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\complain2gm.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\complain2gm_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlegetconfig.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlegetconfig_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlegetmap.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlegetmap_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlegetplayerlocation.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlegetscore.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlegetscore_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlegettotalbonus.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlegettotalbonus_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlemove.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlemove_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlepreenter.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlepreenternotify.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattleresult.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlereturncapital.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlesinglebattleresult.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\countrybattlesyncplayerlocation.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\createfactionfortress.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\createfactionfortress_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\createrole.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\createrole_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\deletemail.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\deletemail_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\deleterole.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\deleterole_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\delfriend.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\delfriend_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\deliverrolestatus.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\enterworld.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\errorinfo.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\exchangeconsumepoints.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\exchangeconsumepoints_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionacceptjoin.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionacceptjoin_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionallianceapply_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionalliancereply_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionapplyjoin_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionappoint_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionbroadcastnotice_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionchangproclaim_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionchat.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factioncreate_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factiondegrade_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factiondismiss_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionexpel_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionfortressbattlelist.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionfortressbattlelist_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionfortresschallenge.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionfortresschallenge_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionfortressenter.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionfortressget.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionfortressget_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionfortresslist.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionfortresslist_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionhostileapply_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionhostilereply_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factioninvitejoin.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionleave_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionlistmember_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionlistonline.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionlistonline_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionlistrelation_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionmasterresign_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionoprequest.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionoprequest_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionrelationrecvapply.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionrelationrecvreply.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionremoverelationapply_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionremoverelationreply_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionrename_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionresign_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factionupgrade_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\findrole.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\findrolebyname.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\findsellpointinfo.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\findsellpointinfo_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\friendalllist.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\frienddel.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\friendextlist.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\friendmove.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\friendstatus.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gamedatasend.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gamezonelist.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getcustomdata.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getcustomdata_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getfactionbaseinfo.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getfactionbaseinfo_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getfriends.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getfriends_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gethelpstates.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gethelpstates_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getmail.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getmail_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getmailattachobj.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getmailattachobj_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getmaillist.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getmaillist_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getplayerbriefinfo.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getplayerbriefinfo_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getplayerfactioninfo.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getplayerfactioninfo_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getplayeridbyname.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getplayeridbyname_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getrewardlist.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getrewardlist_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getsavedmsg.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getsavedmsg_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getselllist.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getselllist_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getuiconfig.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\getuiconfig_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmforbidrole.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmforbidrole_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmgetplayerconsumeinfo.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmgetplayerconsumeinfo_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmkickoutrole.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmkickoutrole_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmkickoutuser.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmkickoutuser_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmlistonlineuser.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmlistonlineuser_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmonlinenum.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmonlinenum_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmrestartserver.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmrestartserver_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmsettimelessautolock.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmsettimelessautolock_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmshutup.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmshutup_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmshutuprole.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmshutuprole_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmtogglechat.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gmtogglechat_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\groupgetinfo.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\groupsetinfo.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\keepalive.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\keyexchange.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\keyreestablish.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\matrixchallenge.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\matrixresponse.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\msgretrieveoffline.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\onlineannounce.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\playerbaseinfo.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\playerbaseinfo_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\playerbaseinfocrc.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\playerbaseinfocrc_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\playerchangeds_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\playerlogout.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\playerpositionresetrqst.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\playersendmail.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\playersendmail_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\preservemail.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\preservemail_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\privatechat.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\publicchat.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\qpaddcash.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\qpaddcash_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\qpannouncediscount.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\qpgetactivatedservices.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\qpgetactivatedservices_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\qqaddfriend.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\qqaddfriend_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\qqaddfriendrqst.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\qqclient.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\qqclient.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\queryuserprivilege_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\refgetreferencecode.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\refgetreferencecode_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\reflistreferrals.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\reflistreferrals_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\refwithdrawbonus.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\refwithdrawbonus_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\report2gm.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\report2gm_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\reportchat.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\response.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rewardmaturenotice.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rolealllist.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rolelist.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rolelist_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rolestatusannounce.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\selectrole.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\selectrole_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sellcancel.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sellcancel_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sellpoint.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sellpoint_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sendaumail.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sendaumail_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sendmsg.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\setcustomdata.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\setcustomdata_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\setfriendgroup.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\setfriendgroup_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\setgroupname.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\setgroupname_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sethelpstates.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sethelpstates_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\setuiconfig.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\setuiconfig_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\ssogetticket.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\ssogetticket_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\statusannounce.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\stockaccount.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\stockaccount_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\stockbill.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\stockbill_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\stockcancel.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\stockcancel_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\stockcommission.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\stockcommission_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\stocktransaction_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sysauctionaccount.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sysauctionaccount_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sysauctionbid.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sysauctionbid_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sysauctioncashtransfer.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sysauctioncashtransfer_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sysauctiongetitem.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sysauctiongetitem_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sysauctionlist.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\sysauctionlist_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\tradeaddgoods.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\tradeaddgoods_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\tradeconfirm.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\tradeconfirm_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\tradediscard.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\tradediscard_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\tradeend.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\trademoveobj.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\trademoveobj_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\traderemovegoods.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\traderemovegoods_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\tradestart.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\tradestart_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\tradestartrqst.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\tradesubmit.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\tradesubmit_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\undodeleterole.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\undodeleterole_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\updateremaintime.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\useraddcash.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\useraddcash_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\usercoupon.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\usercoupon_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\usercouponexchange.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\usercouponexchange_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\usergetinfo.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\usersetinfo.hrp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradeattendlist.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradeattendlist_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradegetdetail.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradegetdetail_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradegetitem.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradegetitem_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradelist.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradelist_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradeprecancelpost.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradeprecancelpost_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradeprepost.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradeprepost_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtraderolegetdetail.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtraderoleprecancelpost.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtraderoleprepost.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradeupdate.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\webtradeupdate_re.hpp
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\worldchat.hpp
# End Source File
# End Group
# Begin Group "IOLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IOLib\gnactiveio.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnbyteorder.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gncompress.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnconf.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gncoordinate.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\IOLib\gncoordinate.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnerrno.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnmarshal.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnmppc.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnnetio.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnoctets.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnpassiveio.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnpollio.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnproto.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnrpc.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnsecure.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnstatic.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\IOLib\gnthread.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\gntimer.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\iolib.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\sha256.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\IOLib\sha256.h
# End Source File
# Begin Source File

SOURCE=.\IOLib\streamcompress.h
# End Source File
# End Group
# Begin Group "inl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CElementClient\Network\inl\acanswer
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\accountloginrecord
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\acquestion
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\acremotecode
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\acreport
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\addfriend
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\addfriend_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\addfriendrqst
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\announceforbidinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\announcenewmail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\announcesellresult
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionattendlist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionattendlist_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionbid
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionbid_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionclose
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionclose_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionexitbid
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionexitbid_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionget
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionget_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctiongetitem
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctiongetitem_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionlist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionlist_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionlistupdate
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionlistupdate_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionopen
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\auctionopen_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\autolockset
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\autolockset_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\battlechallenge
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\battlechallenge_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\battlechallengemap
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\battlechallengemap_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\battleenter
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\battleenter_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\battlegetmap
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\battlegetmap_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\battlestatus
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\battlestatus_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\buypoint
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\buypoint_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\cashlock
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\cashlock_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\cashpasswordset
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\cashpasswordset_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\challenge
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\changeds_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\chatmessage
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\chatroomcreate
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\chatroomcreate_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\chatroomexpel
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\chatroominvite
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\chatroominvite_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\chatroomjoin
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\chatroomjoin_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\chatroomleave
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\chatroomlist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\chatroomlist_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\chatroomspeak
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\checknewmail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\complain2gm
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\complain2gm_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlegetconfig
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlegetconfig_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlegetmap
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlegetmap_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlegetplayerlocation
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlegetscore
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlegetscore_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlegettotalbonus
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlegettotalbonus_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlemove
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlemove_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlepreenter
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlepreenternotify
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattleresult
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlereturncapital
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlesinglebattleresult
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\countrybattlesyncplayerlocation
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\createfactionfortress
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\createfactionfortress_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\createrole
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\createrole_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\deletemail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\deletemail_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\deleterole
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\deleterole_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\delfriend
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\delfriend_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\deliverrolestatus
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\enterworld
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\errorinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\exchangeconsumepoints
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\exchangeconsumepoints_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionacceptjoin
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionacceptjoin_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionallianceapply_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionalliancereply_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionapplyjoin_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionappoint_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionbroadcastnotice_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionchangproclaim_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionchat
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factioncreate_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factiondegrade_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factiondismiss_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionexpel_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionfortressbattlelist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionfortressbattlelist_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionfortresschallenge
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionfortresschallenge_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionfortressenter
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionfortressget
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionfortressget_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionfortresslist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionfortresslist_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionhostileapply_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionhostilereply_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factioninvitejoin
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionleave_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionlistmember_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionlistonline
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionlistonline_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionlistrelation_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionmasterresign_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionoprequest
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionoprequest_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionrelationrecvapply
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionrelationrecvreply
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionremoverelationapply_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionremoverelationreply_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionrename_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionresign_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\factionupgrade_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\findrole
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\findrolebyname
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\findsellpointinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\findsellpointinfo_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\friendalllist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\frienddel
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\friendextlist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\friendmove
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\friendstatus
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gamedatasend
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gamezonelist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getcustomdata
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getcustomdata_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getfactionbaseinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getfactionbaseinfo_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getfriends
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getfriends_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gethelpstates
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gethelpstates_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getmail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getmail_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getmailattachobj
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getmailattachobj_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getmaillist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getmaillist_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getplayerbriefinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getplayerbriefinfo_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getplayerfactioninfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getplayerfactioninfo_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getplayeridbyname
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getplayeridbyname_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getrewardlist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getrewardlist_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getsavedmsg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getsavedmsg_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getselllist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getselllist_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getuiconfig
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\getuiconfig_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmforbidrole
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmforbidrole_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmgetplayerconsumeinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmgetplayerconsumeinfo_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmkickoutrole
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmkickoutrole_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmkickoutuser
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmkickoutuser_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmlistonlineuser
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmlistonlineuser_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmonlinenum
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmonlinenum_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmrestartserver
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmrestartserver_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmsettimelessautolock
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmsettimelessautolock_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmshutup
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmshutup_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmshutuprole
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmshutuprole_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmtogglechat
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\gmtogglechat_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\groupgetinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\groupsetinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\keepalive
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\keyexchange
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\keyreestablish
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\matrixchallenge
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\matrixresponse
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\msgretrieveoffline
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\onlineannounce
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\playerbaseinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\playerbaseinfo_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\playerbaseinfocrc
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\playerbaseinfocrc_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\playerchangeds_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\playerlogout
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\playerpositionresetrqst
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\playersendmail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\playersendmail_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\preservemail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\preservemail_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\privatechat
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\publicchat
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\qpaddcash
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\qpaddcash_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\qpannouncediscount
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\qpgetactivatedservices
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\qpgetactivatedservices_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\qqaddfriend
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\qqaddfriend_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\qqaddfriendrqst
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\queryuserprivilege_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\refgetreferencecode
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\refgetreferencecode_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\reflistreferrals
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\reflistreferrals_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\refwithdrawbonus
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\refwithdrawbonus_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\report2gm
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\report2gm_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\reportchat
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\response
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\rewardmaturenotice
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\rolealllist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\rolelist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\rolelist_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\rolestatusannounce
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\selectrole
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\selectrole_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sellcancel
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sellcancel_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sellpoint
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sellpoint_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sendaumail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sendaumail_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sendmsg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\setcustomdata
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\setcustomdata_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\setfriendgroup
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\setfriendgroup_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\setgroupname
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\setgroupname_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sethelpstates
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sethelpstates_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\setuiconfig
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\setuiconfig_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\ssogetticket
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\ssogetticket_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\statusannounce
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\stockaccount
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\stockaccount_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\stockbill
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\stockbill_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\stockcancel
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\stockcancel_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\stockcommission
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\stockcommission_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\stocktransaction_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sysauctionaccount
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sysauctionaccount_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sysauctionbid
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sysauctionbid_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sysauctioncashtransfer
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sysauctioncashtransfer_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sysauctiongetitem
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sysauctiongetitem_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sysauctionlist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\sysauctionlist_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradeaddgoods
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradeaddgoods_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradeconfirm
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradeconfirm_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradediscard
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradediscard_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradeend
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradeinventory
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\trademoveobj
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\trademoveobj_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\traderemovegoods
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\traderemovegoods_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradesave
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradestart
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradestart_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradestartrqst
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradesubmit
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\tradesubmit_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\undodeleterole
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\undodeleterole_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\updateremaintime
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\useraddcash
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\useraddcash_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\usercoupon
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\usercoupon_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\usercouponexchange
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\usercouponexchange_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\usergetinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\usersetinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradeattendlist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradeattendlist_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradegetdetail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradegetdetail_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradegetitem
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradegetitem_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradelist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradelist_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradeprecancelpost
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradeprecancelpost_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradeprepost
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradeprepost_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtraderolegetdetail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtraderoleprecancelpost
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtraderoleprepost
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradeupdate
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\webtradeupdate_re
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\inl\worldchat
# End Source File
# End Group
# Begin Group "rpcdata"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\addfriendrqstarg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\addfriendrqstres
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\auctionid
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\factioninvitearg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\factioninviteres
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\findrolearg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\findrolebynamearg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\findrolebynameres
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\findroleres
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\fmemberinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\friendalllistarg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\friendalllistres
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\friendbrief
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\frienddelarg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\frienddelres
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\friendmovearg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\friendmoveres
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gamezonelistarg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gamezonelistres
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gauctionbrief
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gauctiondetail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gauctionitem
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gauctionlist
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gbattlechallenge
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gchatmember
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gchatroom
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gcountrybattledomain
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gfactionalliance
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gfactionbaseinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gfactionfortressbattleinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gfactionfortressbriefinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gfactionfortressdetail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gfactionfortressinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gfactionfortressinfo2
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gfactionhostile
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gfactionrelation
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gfactionrelationapply
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gfriendextinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gfriendinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\ggroupinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gmail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gmailheader
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gmailid
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gmailsyncdata
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gmplayerinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\grolebase
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\groleequipment
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\groleforbid
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\groleinventory
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\grolepocket
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\grolestorehouse
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\groomdetail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\groupgetinfoarg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\groupgetinfores
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\groupsetinfoarg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\groupsetinfores
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gsendaumailrecord
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gsysauctiondetail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gsysauctionitem
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gterritory
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\guserfaction
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gwebtradedetail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gwebtradeitem
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\gwebtraderolebrief
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\message
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\msg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\msgretrieveofflinearg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\msgretrieveofflineres
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\playerbriefinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\playerconsumeinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\playerpositionresetrqstarg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\playerpositionresetrqstres
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\qpdiscountinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\qpdiscountlevel
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\qqaddfriendrqstarg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\qqaddfriendrqstres
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\referralbrief
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\rewarditem
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\role
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\rolealllistarg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\rolealllistres
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\roleinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\sellid
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\sellpointinfo
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\ssouser
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\stocklog
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\stockorder
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\stockprice
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\tradestartrqstarg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\tradestartrqstres
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\transid
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\usergetinfoarg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\usergetinfores
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\userinfodetail
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\usersetinfoarg
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\usersetinfores
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdata\zone
# End Source File
# End Group
# Begin Source File

SOURCE=..\CElementClient\Network\callid.hxx
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\EC_GPDataType.h
# End Source File
# Begin Source File

SOURCE=.\EC_SendC2SCmds.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\EC_SendC2SCmds.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\factiondata.hxx
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\forbid.hxx
# End Source File
# Begin Source File

SOURCE=.\gameclient.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\gameclient.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gnet.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\gnetdef.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\ids.hxx
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\netclient.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\privilege.hxx
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\rpcdefs.h
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\state.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\state.hxx
# End Source File
# Begin Source File

SOURCE=..\CElementClient\Network\stubs.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
