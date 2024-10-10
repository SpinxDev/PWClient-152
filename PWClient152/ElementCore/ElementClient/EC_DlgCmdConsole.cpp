/*
 * FILE: EC_DlgCmdConsole.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/31
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Scene.h"
#include "EC_HostPlayer.h"
#include "EC_RTDebug.h"
#include "EC_GameSession.h"
#include "EC_GFXCaster.h"
#include "EC_Configs.h"
#include "EC_SunMoon.h"
#include "EC_Manager.h"
#include "EC_World.h"
#include "TaskTemplMan.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "EC_Instance.h"
#include "EC_ServerSimulate.h"
#include "EC_FactionPVP.h"
#include "EC_ProtocolDebug.h"

#ifdef _PROFILE_MEMORY
#include "Memory\EC_HookMemory.h"
#endif

#include "gnetdef.h"
#include "privilege.hxx"

#include "A3DConfig.h"
#include "A3DTerrainWater.h"
#include "A3DTerrain2CullHC.h"
#include "A3DTerrain2.h"
#include "A3DSkySphere.h"
#include "A3DDevice.h"
#include "AMemFile.h"
#include "AScriptFile.h"
#include "AWStringWithWildcard.h"
#include "AStringWithWildcard.h"
#include "elementdataman.h"
#include "ElementSkill.h"

#include "DlgGMConsole.h"
#include "DlgExplorer.h"
#include <map>
#include <utility>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Maximum number of tokens
#define MAXNUM_TOKEN	8

//	Console commands
enum
{
	CCMD_EXITGAME = 0,		//	Exit game

	//	Debug commands
	CCMD_CAMERAMODE,		//	Host's camera mode
	CCMD_BOUNDBOX,			//	Trigger bounding box
	CCMD_RTDEBUG,			//	Trigger runtime debug
	CCMD_NPCID,				//	Trigger NPC ID
	CCMD_RUNSPEED,			//	Host run speed
	CCMD_GOTO,				//	Goto specified position
	CCMD_FLY,				//	Go to fly mode
	CCMD_C2SCMD,			//	Send c2s command
	CCMD_VIEWRAIDUS,		//	Set view radius
	CCMD_RELOGIN,			//	Re-login
	CCMD_CASTSKILL,			//	Cast skill
	CCMD_RENDERWATER,		//	Water render quality
	CCMD_RENDERGRASS,		//	Grass render flag
	CCMD_RENDERFOREST,		//	Forest render flag
	CCMD_RENDERSHADOW,		//	Shadow render flag
	CCMD_RENDEROUTLINE,		//	Outline render flag
	CCMD_TURNAROUND,		//	Turnaround flag
	CCMD_TESTDIST,			//	Test distance
	CCMD_PLAYGFX,			//	Play specified GFX
	CCMD_SHOWPOS,			//	Show position
	CCMD_TRNLAYER,			//	Terrain layer
	CCMD_A3DSTAT,			//	A3D statistic
	CCMD_GAMESTAT,			//	Game statistic
	CCMD_TREELOD,			//	Tree lod
	CCMD_FPS,				//	FPS switch
	CCMD_PVRADIUS,			//	Player visible radius
	CCMD_SHOWID,			//	Show player ID
	CCMD_SKIPFRAME,			//	Skip frame flag
	CCMD_MODELUPDATE,		//	Model update optimization flag
	CCMD_MINIDUMP,			//	Test mini-dump
	CCMD_SETTIMEOFDAY,		//	Set time of the day
	CCMD_GETSERVERTIME,		//	Get time of the server
	CCMD_TASK,				//	task info
	CCMD_SETBIAS,			//	Set MipMap Bias value
	CCMD_TERRAINCULLER,		//	Enable / disable terrain culler
	CCMD_DEBUGGSCMD,		//	Debug GS command
	CCMD_DEBUGDELIVERCMD,	//	Debug deliver command
	CCMD_TITLE,				//  Change windows title to open 2 clients
	CCMD_NAMEPOS,			//  Set name adjust pos when player in riding state
	CCMD_CREATETIME,		//  Get role create time
	CCMD_LASTLOGINTIME,		//  Get role last login time
	CCMD_MONEY,				//  Show account money
	CCMD_ABSGOTO,			//  Absolute go to specified position
	CCMD_QUERY,				//	Query with item (part) name for whole name and ID
	CCMD_QUERY_NPC,			//	Query npc position within current map
	CCMD_THEME,				//  Apply the specific theme
	CCMD_QUERY_SERVICE,		//	Find npc id by service name
	CCMD_UIDEBUG,			//  Enable/disable the UI debug mode
	CCMD_RENDERNOFOCUS,		//	Enable/Disable Render without Window Focus
	CCMD_QUERYMODEL,		//	Query with model(part) name for whole name and ID
	CCMD_QUERYSKILL,		//	Query skill id with skill name(part)
	CCMD_IE,				//	Explore given http address
	CCMD_ENABLE_IE,			//	Enable using inner explorer
	CCMD_SHOW_FORTRESS,		//	Enable show fortress declare war dialog
	CCMD_IGNORE_URL,		//	Ignore all url navigate but only log it
	CCMD_SHOW_DIALOG,		//	Switch show with given dialog name
	CCMD_COME,				//	Simulate get protocol from server
	CCMD_FACTION_PVP_STATUS,//	Show faction pvp status description
	CCMD_IGNORE_PROTOCOL,	//	Ignore protocol send/receive
	CCMD_DEBUG_FOG,			//	Enable/Disable Fog and set debug parameter
	CCMD_BREAK_LINK,		//	Break link to test reconnect etc.
	CCMD_MEMORY_ADDI,		//	Generate memory additional file
	CCMD_MEMORY_DUMP,		//	Generate memory dump file

	//	GM commonds
	CCMD_GM_KICKOUTROLE,
	CCMD_GM_KICKOUTUSER,
	CCMD_GM_LISTONLINEUSER,
	CCMD_GM_ONLINENUMBER,
	CCMD_GM_RESTARTSERVER,
	CCMD_GM_SHUTUPROLE,
	CCMD_GM_SHUTUPUSER,
	CCMD_GM_MOVETOPLAYER,
	CCMD_GM_CALLINPLAYER,
	CCMD_GM_BROADCAST,
	CCMD_GM_SHOWID,
	CCMD_GM_FORBIDROLE,
	CCMD_GM_TRIGGERCHAT,
	CCMD_GM_GENERATE,
};

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

static AString	l_aCmdTokens[MAXNUM_TOKEN];		//	Command tokens
static int		l_iNumCmdToken = 0;				//	Number of command token
static ACString	l_strOriginCmd;					//	Original command string
static ACHAR	l_strMsg[256];
static bool		l_bInvalidParam;

static DWORD	l_colRed	= 0xffff0000;
static DWORD	l_colGreen	= 0xff00ff00;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

//	Parse command line
static void _ParseCommandLine(const char* szCmd)
{
	l_iNumCmdToken	= 0;
	l_bInvalidParam	= false;

	if (!szCmd)
		return;

	int iLen = strlen(szCmd);
	if (!iLen)
		return;

	AMemFile mf;
	mf.Attach((BYTE*)szCmd, iLen, 0);

	AScriptFile sf;
	sf.Open(&mf);

	while (l_iNumCmdToken < MAXNUM_TOKEN && sf.GetNextToken(false))
	{
		l_aCmdTokens[l_iNumCmdToken++] = sf.m_szToken;
	}

	sf.Close();
	mf.Detach();
}

//	Check command parameter number
static inline bool _CheckConsoleCmdParamNum(int iMinNum, int iMaxNum)
{
	int iNumParam = l_iNumCmdToken - 1;
	l_bInvalidParam = (iNumParam < iMinNum || iNumParam > iMaxNum) ? true : false;
	return !l_bInvalidParam;
}

static void OnConsoleQueryService(ACString str);
static void OnConsoleQuerySkill(AString str);
static void OnConsoleFationPVPStatus();

///////////////////////////////////////////////////////////////////////////
//	
//	Implement
//	
///////////////////////////////////////////////////////////////////////////

//	On console dialog command
bool CECGameUIMan::OnDlgCmd_Console(const ACHAR* szCommand)
{
	if (!g_pGame->GetConfigs()->HasConsole())
		return true;

	if (!szCommand || !szCommand[0])
		return true;
	
	// Just to avoid finger aches :P
	ACHAR szTemp[256];
	if(szCommand[0] == 'd' && szCommand[1] == ' ')
	{
		a_strcpy(szTemp, L"d_c2scmd ");
		a_strcat(szTemp, szCommand+2);
		szCommand = szTemp;
	}
	
	//	Convert UNICODE to ANSI string
	l_strOriginCmd = szCommand;
	AString strCmdStr = AC2AS(szCommand);
	_ParseCommandLine(strCmdStr);

	if (strCmdStr[0] == 'g' && strCmdStr[1] == 'm' && strCmdStr[2] == '_')
	{
		if (g_pGame->GetGameRun()->GetHostPlayer()->IsGM())
		{
			if (!m_pDlgGMConsole->IsShow())
				m_pDlgGMConsole->Show(true);
//			ParseGMConsoleCommand();
		}
	}
	else if (strCmdStr[0] == 'd' && strCmdStr[1] == '_')
		ParseDebugConsoleCommand();
	else
		ParseUserConsoleCommand();
	
	return true;
}

//	Compare command name
bool CECGameUIMan::CompConsoleCommand(int iCmd)
{
	const char* szCmd = m_ConsoleCmds.GetANSIString(iCmd);
	if (!szCmd)
		return false;

	return l_aCmdTokens[0].CompareNoCase(szCmd) == 0 ? true : false;
}

//	Parse user console command
void CECGameUIMan::ParseUserConsoleCommand()
{
	CECGameRun* pGameRun = g_pGame->GetGameRun();

	if (CompConsoleCommand(CCMD_EXITGAME))
	{
		//	Exit game
		pGameRun->PostMessage(MSG_EXITGAME, -1, 0);
	}
	else
	{
		//	Respond: unknown command
		AddConsoleLine(_AL("Unknown command."), l_colRed);
	}
}

//	Parse debug console command
void CECGameUIMan::ParseDebugConsoleCommand()
{
	CECGameSession* pSession = g_pGame->GetGameSession();
	CECGameRun* pGameRun = g_pGame->GetGameRun();
	CECConfigs* pGameCfg = g_pGame->GetConfigs();

	if (CompConsoleCommand(CCMD_CAMERAMODE))
	{
		//	Ghost mode switch
		pGameRun->PostMessage(MSG_HST_CAMERAMODE, MAN_PLAYER, 0);
	}
	else if (CompConsoleCommand(CCMD_BOUNDBOX))
	{
		g_pA3DConfig->RT_SetShowBoundBoxFlag(!g_pA3DConfig->RT_GetShowBoundBoxFlag());
	}
	else if (CompConsoleCommand(CCMD_RTDEBUG))
	{
		if (_CheckConsoleCmdParamNum(1, 2))
		{
			int iNumParam = l_iNumCmdToken - 1;
			if( iNumParam == 1 )
			{
				if( l_aCmdTokens[1] == "list" )
				{
					CECRTDebug::HideProtoList::iterator it;
					CECRTDebug::HideProtoList protos = g_pGame->GetRTDebug()->GetHideProtos();
					for( it=protos.begin();it!=protos.end();++it )
						AddConsoleLine(AS2AC(*it), 0xff00ff00);
				}
				else
					pGameCfg->SetRTDebugLevel(l_aCmdTokens[1].ToInt());
			}
			else
			{
				if( l_aCmdTokens[1] == "show" )
					g_pGame->GetRTDebug()->ShowProtocol(l_aCmdTokens[2]);
				else if( l_aCmdTokens[1] == "hide" )
					g_pGame->GetRTDebug()->HideProtocol(l_aCmdTokens[2]);
				else
					AddConsoleLine(_AL("Unknown parameters!"), 0xffff0000);
			}
		}
	}
	else if (CompConsoleCommand(CCMD_NPCID))
	{
		pGameCfg->ShowNPCID(!pGameCfg->IsNPCIDShown());
	}
	else if (CompConsoleCommand(CCMD_RUNSPEED))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
			pGameCfg->SetHostRunSpeed(l_aCmdTokens[1].ToFloat());
		else
		{
			a_sprintf(l_strMsg, _AL("runspeed = %.2f"), pGameCfg->GetHostRunSpeed());
			AddConsoleLine(l_strMsg, l_colGreen);
		}
	}
	else if (CompConsoleCommand(CCMD_GOTO))
	{
		if (_CheckConsoleCmdParamNum(2, 2))
		{
			A3DVECTOR3 v;
			v.x = l_aCmdTokens[1].ToFloat();
			v.z = l_aCmdTokens[2].ToFloat();
			v.y = 1.0f;
			pSession->c2s_CmdGoto(v.x, v.y, v.z);
		}else if (_CheckConsoleCmdParamNum(3, 3)){
			if (l_aCmdTokens[3].ToInt() == 1){
				//	跳到指定地图分块的中心位置
				int r = l_aCmdTokens[1].ToInt();
				int c = l_aCmdTokens[2].ToInt();
				CECWorld * pWorld = g_pGame->GetGameRun()->GetWorld();
				CECInstance *pInst = g_pGame->GetGameRun()->GetInstance(pWorld->GetInstanceID());
				if (pInst && r >= 0 && r < pInst->GetRowNum() &&
					c >= 0 && c < pInst->GetColNum()){
					float x = c*1024 - pInst->GetColNum()*1024*0.5f + 512.0f;
					float z = (pInst->GetRowNum() - r - 1) * 1024 - pInst->GetRowNum()*1024*0.5f + 512.0f;
					pSession->c2s_CmdGoto(x, 1.0f, z);
				}
			}
		}
	}
	else if (CompConsoleCommand(CCMD_ABSGOTO))
	{
		if (_CheckConsoleCmdParamNum(2, 2))
		{
			A3DVECTOR3 v;
			v.x = (l_aCmdTokens[1].ToFloat() - 400) * 10;
			v.z = (l_aCmdTokens[2].ToFloat() - 550) * 10;
			v.y = 1.0f;
			pSession->c2s_CmdGoto(v.x, v.y, v.z);
		}
	}
	else if (CompConsoleCommand(CCMD_FLY))
	{
	}
	else if (CompConsoleCommand(CCMD_C2SCMD))
	{
		if (_CheckConsoleCmdParamNum(1, 9))
		{
			int iNumParam = l_iNumCmdToken - 2;
			int iCmd = l_aCmdTokens[1].ToInt();

			if (iNumParam <= 0)
				c2s_SendDebugCmd(iCmd, 0);
			else if (iNumParam == 1)
				c2s_SendDebugCmd(iCmd, 1, l_aCmdTokens[2].ToInt());
			else if (iNumParam == 2)
				c2s_SendDebugCmd(iCmd, 2, l_aCmdTokens[2].ToInt(), l_aCmdTokens[3].ToInt());
			else if (iNumParam == 3)
				c2s_SendDebugCmd(iCmd, 3, l_aCmdTokens[2].ToInt(), l_aCmdTokens[3].ToInt(), l_aCmdTokens[4].ToInt());
			else if (iNumParam == 4)
				c2s_SendDebugCmd(iCmd, 4, l_aCmdTokens[2].ToInt(), l_aCmdTokens[3].ToInt(), l_aCmdTokens[4].ToInt(), l_aCmdTokens[5].ToInt());
			else if (iNumParam == 5)
				c2s_SendDebugCmd(iCmd, 5, l_aCmdTokens[2].ToInt(), l_aCmdTokens[3].ToInt(), l_aCmdTokens[4].ToInt(), l_aCmdTokens[5].ToInt(), l_aCmdTokens[6].ToInt());
			else if (iNumParam == 6)
				c2s_SendDebugCmd(iCmd, 6, l_aCmdTokens[2].ToInt(), l_aCmdTokens[3].ToInt(), l_aCmdTokens[4].ToInt(), l_aCmdTokens[5].ToInt(), l_aCmdTokens[6].ToInt(), l_aCmdTokens[7].ToInt());
			else if (iNumParam == 7)
				c2s_SendDebugCmd(iCmd, 7, l_aCmdTokens[2].ToInt(), l_aCmdTokens[3].ToInt(), l_aCmdTokens[4].ToInt(), l_aCmdTokens[5].ToInt(), l_aCmdTokens[6].ToInt(), l_aCmdTokens[7].ToInt(), l_aCmdTokens[8].ToInt());
			else if (iNumParam == 8)
				c2s_SendDebugCmd(iCmd, 8, l_aCmdTokens[2].ToInt(), l_aCmdTokens[3].ToInt(), l_aCmdTokens[4].ToInt(), l_aCmdTokens[5].ToInt(), l_aCmdTokens[6].ToInt(), l_aCmdTokens[7].ToInt(), l_aCmdTokens[8].ToInt(), l_aCmdTokens[9].ToInt());
		}
	}
	else if (CompConsoleCommand(CCMD_VIEWRAIDUS))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
			pGameCfg->SetSceneLoadRadius(l_aCmdTokens[1].ToFloat());
	}
	else if (CompConsoleCommand(CCMD_RELOGIN))
	{
		//	Re-login
		pSession->c2s_CmdLogout(_PLAYER_LOGOUT_HALF);
	}
	else if (CompConsoleCommand(CCMD_CASTSKILL))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
			pGameRun->GetHostPlayer()->ApplySkillShortcut(l_aCmdTokens[1].ToInt());
	}
	else if (CompConsoleCommand(CCMD_RENDERWATER))
	{
		EC_SYSTEM_SETTING ss = pGameCfg->GetSystemSettings();
		ss.nWaterEffect = l_aCmdTokens[1].ToInt();
		pGameCfg->SetSystemSettings(ss);
	}
	else if (CompConsoleCommand(CCMD_RENDERGRASS))
	{
		pGameCfg->m_bShowGrassLand = !pGameCfg->m_bShowGrassLand;
	}
	else if (CompConsoleCommand(CCMD_RENDERFOREST))
	{
		pGameCfg->m_bShowForest = !pGameCfg->m_bShowForest;
	}
	else if (CompConsoleCommand(CCMD_RENDERSHADOW))
	{
		EC_SYSTEM_SETTING ss = pGameCfg->GetSystemSettings();
		ss.bShadow = !ss.bShadow;
		pGameCfg->SetSystemSettings(ss);
	}
	else if (CompConsoleCommand(CCMD_RENDEROUTLINE))
	{
		//EC_SYSTEM_SETTING ss = pGameCfg->GetSystemSettings();
		//ss.bGroundOutline = !ss.bGroundOutline;
		//pGameCfg->SetSystemSettings(ss);
	}
	else if (CompConsoleCommand(CCMD_TURNAROUND))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			EC_GAME_SETTING gs = pGameCfg->GetGameSettings();
			gs.bTurnaround = l_aCmdTokens[1].ToInt() ? false : true;
			pGameCfg->SetGameSettings(gs);
		}
	}
	else if (CompConsoleCommand(CCMD_TESTDIST))
	{
		pGameCfg->SetTestDistFlag(!pGameCfg->GetTestDistFlag());
	}
	else if (CompConsoleCommand(CCMD_PLAYGFX))
	{
		if (_CheckConsoleCmdParamNum(1, 3))
		{
			//	Play specified gfx near host player
			A3DVECTOR3 vPos = pGameRun->GetHostPlayer()->GetPos();
			if (l_iNumCmdToken >= 3)
			{
				int n = l_aCmdTokens[2].ToInt();
				if (n == 1)
					vPos = pGameRun->GetHostPlayer()->GetPlayerAABB().Center;
				else if (n == 2)
					vPos += g_vAxisX * 2.0f + g_vAxisY;
			}

			float fScale = 0.0f;
			if (l_iNumCmdToken >= 4)
				fScale = l_aCmdTokens[3].ToFloat();

			g_pGame->GetGFXCaster()->PlayAutoGFXEx(l_aCmdTokens[1], 1000, vPos, g_vAxisZ, g_vAxisY, fScale);
		}
	}
	else if (CompConsoleCommand(CCMD_SHOWPOS))
	{
		pGameCfg->SetShowPosFlag(!pGameCfg->GetShowPosFlag());
	}
	else if (CompConsoleCommand(CCMD_TRNLAYER))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			//EC_SYSTEM_SETTING ss = pGameCfg->GetSystemSettings();
			//ss.nGroundDetail = l_aCmdTokens[1].ToInt();
			//a_ClampFloor(ss.nGroundDetail, (BYTE)0);
			//pGameCfg->SetSystemSettings(ss);
		}
	}
	else if (CompConsoleCommand(CCMD_A3DSTAT))
	{
		g_pA3DConfig->RT_SetShowPerformanceFlag(!g_pA3DConfig->RT_GetShowPerformanceFlag());
	}
	else if (CompConsoleCommand(CCMD_GAMESTAT))
	{
		pGameCfg->SetShowGameStatFlag(!pGameCfg->GetShowGameStatFlag());
	}
	else if (CompConsoleCommand(CCMD_TREELOD))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			float fLOD = l_aCmdTokens[1].ToFloat();
			pGameCfg->SetForestDetail(fLOD);
		}
	}
	else if (CompConsoleCommand(CCMD_FPS))
	{
		g_pA3DConfig->RT_SetShowFPSFlag(!g_pA3DConfig->RT_GetShowFPSFlag());
	}
	else if (CompConsoleCommand(CCMD_PVRADIUS))
	{
		float fRadius = l_aCmdTokens[1].ToFloat();
		pGameCfg->SetPlayerVisRadius(fRadius);
	}
	else if (CompConsoleCommand(CCMD_SHOWID))
	{
		pGameCfg->SetShowIDFlag(!pGameCfg->GetShowIDFlag());
	}
	else if (CompConsoleCommand(CCMD_SKIPFRAME))
	{
		pGameCfg->SetSkipFrameFlag(!pGameCfg->GetSkipFrameFlag());
	}
	else if (CompConsoleCommand(CCMD_MODELUPDATE))
	{
		pGameCfg->SetModelUpdateFlag(!pGameCfg->GetModelUpdateFlag());
	}
	else if (CompConsoleCommand(CCMD_MINIDUMP))
	{
		int* p = NULL;
		*p = 0;
	}
	else if (CompConsoleCommand(CCMD_SETTIMEOFDAY))
	{
		if( _CheckConsoleCmdParamNum(2, 2) )
		{
			int hour = l_aCmdTokens[1].ToInt();
			int minitute = l_aCmdTokens[2].ToInt();
			if( hour < 0 ) hour = 0;
			if( hour > 23 ) hour = 23;
			if( minitute < 0 ) minitute = 0;
			if( minitute > 59 ) minitute = 59;
			float t = (hour + minitute / 60.0f) / 24.0f; 
			g_pGame->GetGameRun()->GetWorld()->SetTimeOfDay(t);
		}
	}
	else if (CompConsoleCommand(CCMD_GETSERVERTIME))
	{
		struct tm servertime = g_pGame->GetServerLocalTime();
		char szTime[128];
		strcpy(szTime, asctime(&servertime));
		if( szTime[strlen(szTime) - 1] == '\n' )
			szTime[strlen(szTime) - 1] = '\0';
		AddConsoleLine(GetStringFromTable(808) + AS2AC(szTime), l_colGreen);
	}
	else if (CompConsoleCommand(CCMD_CREATETIME))
	{
		const long lRoleCreateTime = g_pGame->GetGameRun()->GetHostPlayer()->GetRoleCreateTime();
		struct tm createtime = *localtime(&lRoleCreateTime);
		char szTime[128];
		strcpy(szTime, asctime(&createtime));
		if( szTime[strlen(szTime) - 1] == '\n' )
			szTime[strlen(szTime) - 1] = '\0';
		AddConsoleLine(AS2AC(szTime), l_colGreen);	
	}
	else if (CompConsoleCommand(CCMD_LASTLOGINTIME))
	{
		const long lLastLoginTime = g_pGame->GetGameRun()->GetHostPlayer()->GetRoleLastLoginTime();
		struct tm lastlogintime = *localtime(&lLastLoginTime);
		char szTime[128];
		strcpy(szTime, asctime(&lastlogintime));
		if( szTime[strlen(szTime) - 1] == '\n' )
			szTime[strlen(szTime) - 1] = '\0';
		AddConsoleLine(AS2AC(szTime), l_colGreen);
	}
	else if (CompConsoleCommand(CCMD_MONEY))
	{
		const long lMoney = g_pGame->GetGameRun()->GetHostPlayer()->GetAccountTotalCash();
		char szMoney[128];
		sprintf(szMoney, "Account Money: %d",  lMoney);
		if( szMoney[strlen(szMoney) - 1] == '\n' )
			szMoney[strlen(szMoney) - 1] = '\0';
		AddConsoleLine(AS2AC(szMoney), l_colRed);	
	}
	else if (CompConsoleCommand(CCMD_TASK))
	{
#ifdef _DEBUG

		if (_CheckConsoleCmdParamNum(2, 2))
		{
			int func = l_aCmdTokens[1].ToInt();

			switch (func)
			{
			case 83284572:
				g_pGame->GetTaskTemplateMan()->ForceRemoveFinishTask(
					reinterpret_cast<TaskInterface*>(g_pGame->GetGameRun()->GetHostPlayer()->GetTaskInterface()),
					l_aCmdTokens[2].ToInt());
				break;
			}
		}

#endif
	}
	else if (CompConsoleCommand(CCMD_SETBIAS))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			float v = l_aCmdTokens[1].ToFloat(); 
			for (int i=0; i<m_pA3DDevice->GetMaxSimultaneousTextures(); i++)
			{
#ifdef ANGELICA_2_2
				m_pA3DDevice->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *(DWORD *)&v);
#else
				m_pA3DDevice->SetDeviceTextureStageState(i, D3DTSS_MIPMAPLODBIAS, *(DWORD *)&v);
#endif
			}
		}
	}
	else if (CompConsoleCommand(CCMD_TERRAINCULLER))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			int val = l_aCmdTokens[1].ToInt();
			A3DTerrain2Cull* pTrnCull = g_pGame->GetGameRun()->GetWorld()->GetTerrainCuller();
			if (pTrnCull)
				pTrnCull->Enable(val ? true : false);
		}
	}
	else if (CompConsoleCommand(CCMD_DEBUGGSCMD))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
			pSession->c2s_CmdDebugGSCmd(l_aCmdTokens[1]);
	}
	else if (CompConsoleCommand(CCMD_DEBUGDELIVERCMD))
	{
		if (_CheckConsoleCmdParamNum(2, 2))
		{
			int iType = l_aCmdTokens[1].ToInt();
			pSession->c2s_CmdDebugDeliverCmd((WORD)iType, l_aCmdTokens[2]);
		}
	}
	else if (CompConsoleCommand(CCMD_TITLE))
	{
		if (_CheckConsoleCmdParamNum(1, 100))
		{
			HWND hDevWnd = g_pGame->GetA3DDevice()->GetDeviceWnd();
			ACString strTitle = AS2AC(l_aCmdTokens[1]);
			::SetWindowTextW(hDevWnd, strTitle);
		}
	}
	else if (CompConsoleCommand(CCMD_NAMEPOS))
	{
		if (_CheckConsoleCmdParamNum(2, 2))
		{
			CECHostPlayer* pPlayer = g_pGame->GetGameRun()->GetHostPlayer();
			float x = l_aCmdTokens[1].ToFloat(); 
			float y = l_aCmdTokens[2].ToFloat(); 
			pPlayer->SetNamePos(A3DVECTOR3(x, y, 0.0f));
		}
	}
	else if (CompConsoleCommand(CCMD_QUERY))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			ACString strName = AS2AC(l_aCmdTokens[1]);
			OnConsoleQuery(strName);
		}
	}
	else if (CompConsoleCommand(CCMD_QUERY_NPC))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			int idTarget = l_aCmdTokens[1].ToInt();
			OnConsoleQueryNPC(idTarget);
		}
	}
	else if (CompConsoleCommand(CCMD_THEME))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			int layout = l_aCmdTokens[1].ToInt();
			
			CECUIManager* pUIMan = g_pGame->GetGameRun()->GetUIManager();
			CECBaseUIMan* pUI = dynamic_cast<CECBaseUIMan*>(pUIMan->GetCurrentUIManPtr());
			if(pUI)
			{
				pUI->ChangeLayout(pUIMan->GetUIDcf(pUIMan->GetCurrentUIMan(), layout));
			}
		}
	}
	else if (CompConsoleCommand(CCMD_QUERY_SERVICE))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			ACString strName = AS2AC(l_aCmdTokens[1]);
			OnConsoleQueryService(strName);
		}
	}
	else if (CompConsoleCommand(CCMD_UIDEBUG))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			bool val = l_aCmdTokens[1].ToInt() != 0;
			g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->SetDebugMode(val);
		}
	}
	else if (CompConsoleCommand(CCMD_RENDERNOFOCUS))
	{
		g_bRenderNoFocus = !g_bRenderNoFocus;
	}
	else if (CompConsoleCommand(CCMD_QUERYMODEL))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			AString strName = l_aCmdTokens[1];
			OnConsoleQueryModel(strName);
		}
	}
	else if (CompConsoleCommand(CCMD_QUERYSKILL))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			AString strName = l_aCmdTokens[1];
			OnConsoleQuerySkill(strName);
		}
	}
	else if (CompConsoleCommand(CCMD_IE))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			AString strAddress = l_aCmdTokens[1];			
			CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			CDlgExplorer *pDlgExplorer = dynamic_cast<CDlgExplorer *>(pGameUIMan->GetDialog("Win_Explorer"));
			if (pDlgExplorer)
			{
				if (!pDlgExplorer->IsShow())
					pDlgExplorer->Show(true);
				pDlgExplorer->NavigateUrl(strAddress);
			}
		}
	}
	else if (CompConsoleCommand(CCMD_ENABLE_IE))
	{
		bool bEnable = CECUIConfig::Instance().GetGameUI().bEnableIE;
		CECUIConfig::Instance().GetGameUI().bEnableIE = !bEnable;
	}
	else if (CompConsoleCommand(CCMD_SHOW_FORTRESS))
	{
		g_bEnableFortressDeclareWar = !g_bEnableFortressDeclareWar;
	}
	else if (CompConsoleCommand(CCMD_IGNORE_URL))
	{
		g_bIgnoreURLNavigate = !g_bIgnoreURLNavigate;
	}
	else if (CompConsoleCommand(CCMD_SHOW_DIALOG))
	{
		if (_CheckConsoleCmdParamNum(1, 1))
		{
			CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			PAUIDIALOG pDlg = pGameUIMan->GetDialog(l_aCmdTokens[1]);
			if (pDlg){
				pDlg->Show(!pDlg->IsShow());
			}
		}
	}
	else if (CompConsoleCommand(CCMD_COME))
	{
		CECServerSimulate::Instance().Come(l_aCmdTokens+1, l_iNumCmdToken-1);
	}
	else if (CompConsoleCommand(CCMD_FACTION_PVP_STATUS))
	{
		OnConsoleFationPVPStatus();
	}else if (CompConsoleCommand(CCMD_IGNORE_PROTOCOL)){
		if (_CheckConsoleCmdParamNum(2, 2)){
			char clientOrServer(0);
			char protocolOrCommand(0);
			int  id(0);
			bool ignore(false);
			if (sscanf(l_aCmdTokens[1], "%c%c%d", &clientOrServer, &protocolOrCommand, &id) == 3){
				ignore = l_aCmdTokens[2].ToInt() != 0;
				if (clientOrServer == 'c'){
					if (protocolOrCommand == 'p'){
						CECProtocolDebug::Instance().IgnoreClientProtocol(id, ignore);
					}else if (protocolOrCommand == 'c'){
						CECProtocolDebug::Instance().IgnoreClientCommand(id, ignore);
					}
				}else if (clientOrServer = 's'){
					if (protocolOrCommand == 'p'){
						CECProtocolDebug::Instance().IgnoreServerProtocol(id, ignore);
					}else if (protocolOrCommand == 'c'){
						CECProtocolDebug::Instance().IgnoreServerCommand(id, ignore);
					}
				}
			}
		}
	}else if (CompConsoleCommand(CCMD_DEBUG_FOG)){
		if (_CheckConsoleCmdParamNum(0, 0)){
			g_pGame->GetConfigs()->SetDebugFog(false);
		}else if (_CheckConsoleCmdParamNum(6, 6)){
			g_pGame->GetConfigs()->SetDebugFog(true);
			g_pGame->GetConfigs()->SetDebugFogParameter(l_aCmdTokens[1].ToFloat(), l_aCmdTokens[2].ToFloat(), l_aCmdTokens[3].ToFloat(), A3DCOLORRGB(l_aCmdTokens[4].ToInt(), l_aCmdTokens[5].ToInt(), l_aCmdTokens[6].ToInt()));
		}
	}else if (CompConsoleCommand(CCMD_BREAK_LINK)){
		if (g_pGame->GetGameSession()->IsConnected()){
			g_pGame->GetGameSession()->SetBreakLinkFlag(CECGameSession::LBR_DEBUG);
		}
	}else if (CompConsoleCommand(CCMD_MEMORY_ADDI)){
#ifdef _PROFILE_MEMORY
		g_GenerateAdditionFile();
#endif
	}else if (CompConsoleCommand(CCMD_MEMORY_DUMP)){
#ifdef _PROFILE_MEMORY
		g_MemoryDump();
#endif
	}else{
		//	Respond: unknown command
		AddConsoleLine(_AL("Unknown command."), l_colRed);
	}

	if (l_bInvalidParam)
		AddConsoleLine(_AL("Wrong parameter number."), l_colRed);
}

//	Parse GM console command
void CECGameUIMan::ParseGMConsoleCommand()
{
	CECGameSession* pSession = g_pGame->GetGameSession();
	GNET::Privilege* pPrivilege = g_pGame->GetPrivilege();
	CECConfigs* pGameCfg = g_pGame->GetConfigs();
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();

	if (CompConsoleCommand(CCMD_GM_KICKOUTROLE))
	{
		if (pPrivilege->Has_Force_Offline() && _CheckConsoleCmdParamNum(3, 3))
		{
			int idPlayer = l_aCmdTokens[1].ToInt();
			int iTime = l_aCmdTokens[2].ToInt();
			ACString strReason = AS2AC(l_aCmdTokens[3]);
			pSession->gm_KickOutRole(idPlayer, iTime, strReason);
		}
	}
	else if (CompConsoleCommand(CCMD_GM_KICKOUTUSER))
	{
		if (pPrivilege->Has_Force_Offline() && _CheckConsoleCmdParamNum(3, 3))
		{
			int idPlayer = l_aCmdTokens[1].ToInt();
			int iTime = l_aCmdTokens[2].ToInt();
			ACString strReason = AS2AC(l_aCmdTokens[3]);
			pSession->gm_KickOutUser(idPlayer, iTime, strReason);
		}
	}
	else if (CompConsoleCommand(CCMD_GM_LISTONLINEUSER))
	{
		if (pPrivilege->Has_ListUser())
		{
		}
	}
	else if (CompConsoleCommand(CCMD_GM_ONLINENUMBER))
	{
		if (pPrivilege->Has_ListUser())
			pSession->gm_OnlineNumber();
	}
	else if (CompConsoleCommand(CCMD_GM_RESTARTSERVER))
	{
		if (pPrivilege->Has_Shutdown_GameServer() && _CheckConsoleCmdParamNum(1, 1))
		{
			int iTime = l_aCmdTokens[1].ToInt();
			pSession->gm_RestartServer(-1, iTime);
		}
	}
	else if (CompConsoleCommand(CCMD_GM_SHUTUPROLE))
	{
		if (pPrivilege->Has_Forbid_Talk() && _CheckConsoleCmdParamNum(3, 3))
		{
			int idPlayer = l_aCmdTokens[1].ToInt();
			int iTime = l_aCmdTokens[2].ToInt();
			ACString strReason = AS2AC(l_aCmdTokens[3]);
			pSession->gm_ShutupRole(idPlayer, iTime, strReason);
		}
	}
	else if (CompConsoleCommand(CCMD_GM_SHUTUPUSER))
	{
		if (pPrivilege->Has_Forbid_Talk() && _CheckConsoleCmdParamNum(3, 3))
		{
			int idPlayer = l_aCmdTokens[1].ToInt();
			int iTime = l_aCmdTokens[2].ToInt();
			ACString strReason = AS2AC(l_aCmdTokens[3]);
			pSession->gm_ShutupUser(idPlayer, iTime, strReason);
		}
	}
	else if (CompConsoleCommand(CCMD_GM_MOVETOPLAYER))
	{
		if (pPrivilege->Has_MoveTo_Role() && _CheckConsoleCmdParamNum(1, 1))
		{
			int idPlayer = l_aCmdTokens[1].ToInt();
			pSession->gm_MoveToPlayer(idPlayer);
		}
	}
	else if (CompConsoleCommand(CCMD_GM_CALLINPLAYER))
	{
		if (pPrivilege->Has_Fetch_Role() && _CheckConsoleCmdParamNum(1, 1))
		{
			int idPlayer = l_aCmdTokens[1].ToInt();
			pSession->gm_CallInPlayer(idPlayer);
		}
	}
	else if (CompConsoleCommand(CCMD_GM_BROADCAST))
	{
		if (pPrivilege->Has_Broadcast() && _CheckConsoleCmdParamNum(1, 1))
		{
			ACString str = AS2AC(l_aCmdTokens[1]);
			pSession->SendChatData(GP_CHAT_BROADCAST, str);
		}
	}
	else if (CompConsoleCommand(CCMD_GM_SHOWID))
	{
		if (pPrivilege->Has_Toggle_NameID())
			pGameCfg->SetShowIDFlag(!pGameCfg->GetShowIDFlag());
	}
	else if (CompConsoleCommand(CCMD_GM_FORBIDROLE))
	{
		if (pHost->IsGM() && _CheckConsoleCmdParamNum(3, 3))
		{
			int iType = l_aCmdTokens[1].ToInt();
			int idPlayer = l_aCmdTokens[2].ToInt();
			int iTime = l_aCmdTokens[3].ToInt();
			ACString strReason = AS2AC(l_aCmdTokens[4]);
			pSession->gm_ForbidRole(iType, idPlayer, iTime, strReason);
		}
	}
	else if (CompConsoleCommand(CCMD_GM_TRIGGERCHAT))
	{
		if (pPrivilege->Has_Chat_OrNot() && _CheckConsoleCmdParamNum(1, 1))
		{
			int iFlag = l_aCmdTokens[1].ToInt();
			pSession->gm_TriggerChat(iFlag ? true : false);
		}
	}
	else if (CompConsoleCommand(CCMD_GM_GENERATE))
	{
		if (pHost->IsGM() && _CheckConsoleCmdParamNum(1, 1))
		{
			int tid = l_aCmdTokens[1].ToInt();
			pSession->gm_Generate(tid);
		}
	}
	else
	{
		//	Respond: unknown command
		AddConsoleLine(_AL("Unknown command."), l_colRed);
	}
	
	if (l_bInvalidParam)
		AddConsoleLine(_AL("Wrong parameter number."), l_colRed);
}

void CECGameUIMan::OnConsoleQueryNPC(int idTarget)
{
	//	根据 NPC ID 查找位置
	//

	if (!idTarget)
		return;
	
	bool bFind(false);
	abase::vector<CECGame::OBJECT_COORD> TargetTemp;
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	pHost->GetObjectCoordinates(idTarget, TargetTemp, bFind);
	if (bFind)
	{
		ACString strTemp;
		for (size_t u(0); u < TargetTemp.size(); ++ u)
		{
			const CECGame::OBJECT_COORD &coord = TargetTemp[u];
			strTemp.Format(_AL("%f %f %f"), coord.vPos.x, coord.vPos.y, coord.vPos.z);
			AddConsoleLine(strTemp, l_colRed);
		}
	}
}

void CECGameUIMan::OnConsoleQuery(ACString strPattern)
{
	// 根据名称模糊查询物品及其ID
#define CHECK_ESSENCE(e) if (dataType == DT_##e) \
	{ \
	e *pEssence = (e *)pElementDataMan->get_data_ptr(idTemp, ID_SPACE_ESSENCE, dataType); \
	if (matcher.IsMatch(ACString(pEssence->name), strPattern)) \
	{\
		strTemp.Format(_AL("%s: %d"), pEssence->name, pEssence->id); \
		AddConsoleLine(strTemp, l_colRed); \
	}\
	}
	
	strPattern = _AL("*") + strPattern + _AL("*");
	ACString strTemp;
	DATA_TYPE dataType;
	ACStringWithWildcard matcher;
	elementdataman *pElementDataMan = g_pGame->GetElementDataMan();
	unsigned int idTemp = pElementDataMan->get_first_data_id(ID_SPACE_ESSENCE, dataType);
	while (idTemp)
	{
		CHECK_ESSENCE(WEAPON_ESSENCE)
		else CHECK_ESSENCE(ARMOR_ESSENCE)
		else CHECK_ESSENCE(DECORATION_ESSENCE)
		else CHECK_ESSENCE(MEDICINE_ESSENCE)
		else CHECK_ESSENCE(MATERIAL_ESSENCE)
		else CHECK_ESSENCE(DAMAGERUNE_ESSENCE)
		else CHECK_ESSENCE(ARMORRUNE_ESSENCE)
		else CHECK_ESSENCE(SKILLTOME_ESSENCE)
		else CHECK_ESSENCE(FLYSWORD_ESSENCE)
		else CHECK_ESSENCE(WINGMANWING_ESSENCE)
		else CHECK_ESSENCE(TOWNSCROLL_ESSENCE)
		else CHECK_ESSENCE(UNIONSCROLL_ESSENCE)
		else CHECK_ESSENCE(REVIVESCROLL_ESSENCE)
		else CHECK_ESSENCE(ELEMENT_ESSENCE)
		else CHECK_ESSENCE(TASKMATTER_ESSENCE)
		else CHECK_ESSENCE(TOSSMATTER_ESSENCE)
		else CHECK_ESSENCE(PROJECTILE_ESSENCE)
		else CHECK_ESSENCE(QUIVER_ESSENCE)
		else CHECK_ESSENCE(STONE_ESSENCE)
		else CHECK_ESSENCE(MONSTER_ESSENCE)
		else CHECK_ESSENCE(NPC_ESSENCE)
		else CHECK_ESSENCE(FACE_TEXTURE_ESSENCE)
		else CHECK_ESSENCE(FACE_SHAPE_ESSENCE)
		else CHECK_ESSENCE(FACE_EXPRESSION_ESSENCE)
		else CHECK_ESSENCE(FACE_HAIR_ESSENCE)
		else CHECK_ESSENCE(FACE_MOUSTACHE_ESSENCE)
		else CHECK_ESSENCE(COLORPICKER_ESSENCE)
		else CHECK_ESSENCE(CUSTOMIZEDATA_ESSENCE)
		else CHECK_ESSENCE(RECIPE_ESSENCE)
		else CHECK_ESSENCE(TASKDICE_ESSENCE)
		else CHECK_ESSENCE(TASKNORMALMATTER_ESSENCE)
		else CHECK_ESSENCE(FACE_FALING_ESSENCE)
		else CHECK_ESSENCE(MINE_ESSENCE)
		else CHECK_ESSENCE(FASHION_ESSENCE)
		else CHECK_ESSENCE(FACETICKET_ESSENCE)
		else CHECK_ESSENCE(FACEPILL_ESSENCE)
		else CHECK_ESSENCE(SUITE_ESSENCE)
		else CHECK_ESSENCE(GM_GENERATOR_ESSENCE)
		else CHECK_ESSENCE(PET_ESSENCE)
		else CHECK_ESSENCE(PET_EGG_ESSENCE)
		else CHECK_ESSENCE(PET_FOOD_ESSENCE)
		else CHECK_ESSENCE(PET_FACETICKET_ESSENCE)
		else CHECK_ESSENCE(FIREWORKS_ESSENCE)
		else CHECK_ESSENCE(WAR_TANKCALLIN_ESSENCE)
		else CHECK_ESSENCE(SKILLMATTER_ESSENCE)
		else CHECK_ESSENCE(REFINE_TICKET_ESSENCE)
		else CHECK_ESSENCE(DESTROYING_ESSENCE)
		else CHECK_ESSENCE(BIBLE_ESSENCE)
		else CHECK_ESSENCE(SPEAKER_ESSENCE)
		else CHECK_ESSENCE(AUTOHP_ESSENCE)
		else CHECK_ESSENCE(AUTOMP_ESSENCE)
		else CHECK_ESSENCE(DOUBLE_EXP_ESSENCE)
		else CHECK_ESSENCE(TRANSMITSCROLL_ESSENCE)
		else CHECK_ESSENCE(DYE_TICKET_ESSENCE)
		else CHECK_ESSENCE(GOBLIN_ESSENCE)
		else CHECK_ESSENCE(GOBLIN_EQUIP_ESSENCE)
		else CHECK_ESSENCE(GOBLIN_EXPPILL_ESSENCE)
		else CHECK_ESSENCE(SELL_CERTIFICATE_ESSENCE)
		else CHECK_ESSENCE(TARGET_ITEM_ESSENCE)
		else CHECK_ESSENCE(LOOK_INFO_ESSENCE)
		else CHECK_ESSENCE(INC_SKILL_ABILITY_ESSENCE)
		else CHECK_ESSENCE(WEDDING_BOOKCARD_ESSENCE)
		else CHECK_ESSENCE(WEDDING_INVITECARD_ESSENCE)
		else CHECK_ESSENCE(SHARPENER_ESSENCE)
		else CHECK_ESSENCE(FACTION_MATERIAL_ESSENCE)
		else CHECK_ESSENCE(FACTION_BUILDING_ESSENCE)
		else CHECK_ESSENCE(CONGREGATE_ESSENCE)
		else CHECK_ESSENCE(FORCE_TOKEN_ESSENCE)
		else CHECK_ESSENCE(DYNSKILLEQUIP_ESSENCE)
		else CHECK_ESSENCE(MONEY_CONVERTIBLE_ESSENCE)
		else CHECK_ESSENCE(MONSTER_SPIRIT_ESSENCE)
		else CHECK_ESSENCE(POKER_DICE_ESSENCE)
		else CHECK_ESSENCE(POKER_ESSENCE)
		else CHECK_ESSENCE(SHOP_TOKEN_ESSENCE)
		else CHECK_ESSENCE(UNIVERSAL_TOKEN_ESSENCE)
		idTemp = pElementDataMan->get_next_data_id(ID_SPACE_ESSENCE, dataType);
	}

#undef CHECK_ESSENCE
}

void CECGameUIMan::OnConsoleQueryModel(AString strPattern)
{
	// 根据模型或ski路径名模糊查询物品及其ID
#define CHECK_ESSENCE(e) if (dataType == DT_##e) \
	{ \
	e *pEssence = (e *)pElementDataMan->get_data_ptr(idTemp, ID_SPACE_ESSENCE, dataType); \
	if (matcher.IsMatch(AString(pEssence->file_model), strPattern)) \
	{\
	strTemp.Format(_AL("%s: %d"), pEssence->name, pEssence->id); \
	AddConsoleLine(strTemp, l_colRed); \
	}\
	}

#define CHECK_WEAPON_ESSENCE(e) if (dataType == DT_##e) \
	{ \
	e *pEssence = (e *)pElementDataMan->get_data_ptr(idTemp, ID_SPACE_ESSENCE, dataType); \
	if (matcher.IsMatch(AString(pEssence->file_model_right), strPattern) || matcher.IsMatch(AString(pEssence->file_model_left), strPattern)) \
	{\
	strTemp.Format(_AL("%s: %d"), pEssence->name, pEssence->id); \
	AddConsoleLine(strTemp, l_colRed); \
	}\
	}

#define CHECK_ARMOR_ESSENCE(e) if (dataType == DT_##e) \
	{ \
	e *pEssence = (e *)pElementDataMan->get_data_ptr(idTemp, ID_SPACE_ESSENCE, dataType); \
	if (matcher.IsMatch(AString(pEssence->realname), strPattern)) \
	{\
	strTemp.Format(_AL("%s: %d"), pEssence->name, pEssence->id); \
	AddConsoleLine(strTemp, l_colRed); \
	}\
}
	
#define CHECK_FASHION_ESSENCE(e) if (dataType == DT_##e) \
	{ \
	e *pEssence = (e *)pElementDataMan->get_data_ptr(idTemp, ID_SPACE_ESSENCE, dataType); \
	if (matcher.IsMatch(AString(pEssence->realname), strPattern) || matcher.IsMatch(AString(pEssence->file_model_right), strPattern) || matcher.IsMatch(AString(pEssence->file_model_left), strPattern)) \
	{\
	strTemp.Format(_AL("%s: %d"), pEssence->name, pEssence->id); \
	AddConsoleLine(strTemp, l_colRed); \
	}\
	}

#define CHECK_CERTIFICATE_ESSENCE(e) if (dataType == DT_##e) \
	{ \
	e *pEssence = (e *)pElementDataMan->get_data_ptr(idTemp, ID_SPACE_ESSENCE, dataType); \
	if (matcher.IsMatch(AString(pEssence->show_model), strPattern)) \
	{\
	strTemp.Format(_AL("%s: %d"), pEssence->name, pEssence->id); \
	AddConsoleLine(strTemp, l_colRed); \
	}\
	}

#define CHECK_GOBLIN_ESSENCE(e) if (dataType == DT_##e) \
	{ \
	e *pEssence = (e *)pElementDataMan->get_data_ptr(idTemp, ID_SPACE_ESSENCE, dataType); \
	if (matcher.IsMatch(AString(pEssence->file_model1), strPattern) || \
		matcher.IsMatch(AString(pEssence->file_model2), strPattern) || \
		matcher.IsMatch(AString(pEssence->file_model3), strPattern) || \
		matcher.IsMatch(AString(pEssence->file_model4), strPattern)) \
	{\
	strTemp.Format(_AL("%s: %d"), pEssence->name, pEssence->id); \
	AddConsoleLine(strTemp, l_colRed); \
	}\
	}

	strPattern = AString("*") + strPattern + AString("*");
	ACString strTemp;
	DATA_TYPE dataType;
	AStringWithWildcard matcher;
	elementdataman *pElementDataMan = g_pGame->GetElementDataMan();
	unsigned int idTemp = pElementDataMan->get_first_data_id(ID_SPACE_ESSENCE, dataType);
	while (idTemp)
	{
		CHECK_WEAPON_ESSENCE(WEAPON_ESSENCE)
		else CHECK_ARMOR_ESSENCE(ARMOR_ESSENCE)
		else CHECK_ESSENCE(DECORATION_ESSENCE)
		else CHECK_ESSENCE(FLYSWORD_ESSENCE)
		else CHECK_ESSENCE(WINGMANWING_ESSENCE)
		else CHECK_ESSENCE(TOSSMATTER_ESSENCE)
		else CHECK_ESSENCE(PROJECTILE_ESSENCE)
		else CHECK_ESSENCE(MONSTER_ESSENCE)
		else CHECK_ESSENCE(NPC_ESSENCE)
		else CHECK_FASHION_ESSENCE(FASHION_ESSENCE)
		else CHECK_ESSENCE(PET_ESSENCE)
		else CHECK_GOBLIN_ESSENCE(GOBLIN_ESSENCE)
		else CHECK_ESSENCE(GOBLIN_EQUIP_ESSENCE)
		else CHECK_ESSENCE(MINE_ESSENCE)
		else CHECK_CERTIFICATE_ESSENCE(SELL_CERTIFICATE_ESSENCE)
		idTemp = pElementDataMan->get_next_data_id(ID_SPACE_ESSENCE, dataType);
	}

#undef CHECK_ESSENCE
#undef CHECK_WEAPON_ESSENCE
#undef CHECK_FASHION_ESSENCE
#undef CHECK_CERTIFICATE_ESSENCE
#undef CHECK_GOBLIN_ESSENCE
}

void OnConsoleQueryService(ACString strPattern)
{
	//	检查哪些 NPC 上挂了与给定字符串相关的固定名称服务
	//
	strPattern = _AL("*") + strPattern + _AL("*");
	ACStringWithWildcard matcher;

	//	构造所有 NPC 简单服务映射表，简单服务名称->简单服务ID
	typedef std::multimap<unsigned int, ACString> ServiceMap;
	typedef std::pair<ServiceMap::iterator, ServiceMap::iterator> II;

	const int NPC_COMBINED_SERVICE_COUNT = 2;	//	NPC 上挂的简单服务mask变量的个数
	ServiceMap serviceMaps[NPC_COMBINED_SERVICE_COUNT];

	CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	ACString strText;

#define ADD_SERVICE_MAP(index, idString, uMask) \
	strText = pGameUIMan->GetStringFromTable(idString); \
	if (!strText.IsEmpty()) \
	serviceMaps[index].insert(ServiceMap::value_type(uMask, strText));
	
	ADD_SERVICE_MAP(0, 558, 0x01);
	ADD_SERVICE_MAP(0, 885, 0x02);
	ADD_SERVICE_MAP(0, 592, 0x10);
	ADD_SERVICE_MAP(0, 593, 0x10);
	ADD_SERVICE_MAP(0, 594, 0x10);
	ADD_SERVICE_MAP(0, 595, 0x20);
	ADD_SERVICE_MAP(0, 634, 0x40);
	ADD_SERVICE_MAP(0, 644, 0x80);
	ADD_SERVICE_MAP(0, 691, 0x200);
	ADD_SERVICE_MAP(0, 692, 0x400);
	ADD_SERVICE_MAP(0, 734, 0x2000);
	ADD_SERVICE_MAP(0, 769, 0x4000);
	ADD_SERVICE_MAP(0, 874, 0x8000);
	ADD_SERVICE_MAP(0, 878, 0x10000);
	ADD_SERVICE_MAP(0, 7921, 0x800000);
	ADD_SERVICE_MAP(0, 8600, 0x1000000);
	ADD_SERVICE_MAP(0, 8601, 0x2000000);
	ADD_SERVICE_MAP(0, 8740, 0x4000000);
	ADD_SERVICE_MAP(0, 8741, 0x4000000);
	ADD_SERVICE_MAP(0, 9101, 0x8000000);
	ADD_SERVICE_MAP(0, 9102, 0x8000000);
	ADD_SERVICE_MAP(0, 9107, 0x8000000);
	ADD_SERVICE_MAP(0, 9108, 0x8000000);
	ADD_SERVICE_MAP(0, 9103, 0x10000000);
	ADD_SERVICE_MAP(0, 9104, 0x10000000);
	ADD_SERVICE_MAP(0, 9105, 0x10000000);
	ADD_SERVICE_MAP(0, 9109, 0x10000000);
	ADD_SERVICE_MAP(0, 9106, 0x20000000);
	ADD_SERVICE_MAP(0, 7106, 0x400000);
	ADD_SERVICE_MAP(0, 7100, 0x20000);
	ADD_SERVICE_MAP(0, 7101, 0x40000);
	ADD_SERVICE_MAP(0, 7102, 0x80000);
	ADD_SERVICE_MAP(0, 7105, 0x200000);
	ADD_SERVICE_MAP(0, 7103, 0x100000);
	ADD_SERVICE_MAP(0, 7104, 0x100000);
	ADD_SERVICE_MAP(0, 699, 0x800);
	ADD_SERVICE_MAP(0, 722, 0x800);
	ADD_SERVICE_MAP(0, 724, 0x1000);
	ADD_SERVICE_MAP(0, 9299, 0x40000000);
	ADD_SERVICE_MAP(0, 9340, 0x80000000);

	ADD_SERVICE_MAP(1, 9762, 0x00000001);
	ADD_SERVICE_MAP(1, 9763, 0x00000001);
	ADD_SERVICE_MAP(1, 9914, 0x00000002);
	ADD_SERVICE_MAP(1, 9994, 0x00000004);
	ADD_SERVICE_MAP(1, 10132, 0x00000008);
	ADD_SERVICE_MAP(1, 10133, 0x00000010);
	ADD_SERVICE_MAP(1, 10150, 0x00000020);
	ADD_SERVICE_MAP(1, 10172, 0x00000040);
	ADD_SERVICE_MAP(1, 10304, 0x00000080);
	ADD_SERVICE_MAP(1, 10508, 0x00000100);
	ADD_SERVICE_MAP(1, 10513, 0x00000100);
	ADD_SERVICE_MAP(1, 10430, 0x00000200);
	ADD_SERVICE_MAP(1, 10800, 0x00000400);
	ADD_SERVICE_MAP(1, 10820, 0x00000800);
	ADD_SERVICE_MAP(1, 10890, 0x00001000);
	ADD_SERVICE_MAP(1, 10912, 0x00001000);
	ADD_SERVICE_MAP(1, 11000, 0x00002000);
	ADD_SERVICE_MAP(1, 11193, 0x00004000);
	ADD_SERVICE_MAP(1, 11270, 0x00008000);

	int i(0), j(0);

	//	根据输入名称查询简单服务ID
	typedef std::map<unsigned int, ACString> ExistServiceMap;
	ExistServiceMap existServiceMaps[NPC_COMBINED_SERVICE_COUNT];
	for (i = 0; i < NPC_COMBINED_SERVICE_COUNT; ++ i){
		ServiceMap		&serviceMap		= serviceMaps[i];
		ExistServiceMap &existServiceMap= existServiceMaps[i];
		for (ServiceMap::iterator it = serviceMap.begin(); it != serviceMap.end(); ++ it){
			const ACString &strText = it->second;
			if (matcher.IsMatch(strText, strPattern)){
				//	插入匹配的服务
				unsigned int uMask = it->first;
				ExistServiceMap::iterator it2 = existServiceMap.find(uMask);
				if (it2 == existServiceMap.end()){
					it2 = existServiceMap.insert(ExistServiceMap::value_type(uMask, _AL(""))).first;
				}
				ACString &str = it2->second;
				if (!str.IsEmpty()){
					//	增加空格
					str += _AL(" ");
				}
				str += strText;
			}
		}
	}
	for (i = 0; i < NPC_COMBINED_SERVICE_COUNT; ++ i){
		ExistServiceMap &existServiceMap = existServiceMaps[i];
		if (!existServiceMap.empty()){
			break;
		}
	}
	if (i >= NPC_COMBINED_SERVICE_COUNT){
		//	没有匹配项
		return;
	}

	//	根据需要匹配的服务ID，查询所有NPC
	ACString strOutput, strTemp;
	DATA_TYPE dataType;
	elementdataman *pElementDataMan = g_pGame->GetElementDataMan();
	unsigned int idTemp = pElementDataMan->get_first_data_id(ID_SPACE_ESSENCE, dataType);
	while (idTemp){
		if (dataType == DT_NPC_ESSENCE){
			const NPC_ESSENCE *pEssence = (const NPC_ESSENCE *)pElementDataMan->get_data_ptr(idTemp, ID_SPACE_ESSENCE, dataType);
			if (pEssence){
				unsigned int combined_services_array[NPC_COMBINED_SERVICE_COUNT] = {
					pEssence->combined_services,
					pEssence->combined_services2,
				};
				strTemp.Empty();
				for (i = 0; i < NPC_COMBINED_SERVICE_COUNT; ++ i){
					if (unsigned int combined_services = combined_services_array[i]){
						//	查询所有可能服务，如果存在，以空格分开形成字符串
						ExistServiceMap &existServiceMap = existServiceMaps[i];
						for (ExistServiceMap::iterator it = existServiceMap.begin(); it != existServiceMap.end(); ++ it){
							unsigned int mask = it->first;
							if (mask & combined_services){
								strTemp += _AL(" ");
								strTemp += it->second;
							}
						}
					}
				}
				if (!strTemp.IsEmpty()){
					//	找到某服务
					strOutput.Format(_AL("%d%s"), pEssence->id, strTemp);
					pGameUIMan->AddConsoleLine(strOutput, l_colRed);
				}
			}
		}
		idTemp = pElementDataMan->get_next_data_id(ID_SPACE_ESSENCE, dataType);
	}

#undef ADD_SERVICE_MAP
}

void OnConsoleQuerySkill(AString str)
{
	//	根据技能名称查找技能id等信息

	AString strPattern = AString("*") + str + AString("*");
	AStringWithWildcard matcher;
	ACString strOutput;

	CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	unsigned int idSkill = 0;
	while(true)
	{
		idSkill = GNET::ElementSkill::NextSkill(idSkill);
		if( idSkill == 0 )
			break;

		AString strName = GNET::ElementSkill::GetNativeName(idSkill);
		if (matcher.IsMatch(strName, strPattern))
		{
			int requiredBook = GNET::ElementSkill::GetRequiredBook(idSkill, 1);
			if (requiredBook)
				strOutput.Format(_AL("%s: %d %d"), AS2AC(strName), idSkill, requiredBook);
			else
				strOutput.Format(_AL("%s: %d"), AS2AC(strName), idSkill);
			pGameUIMan->AddConsoleLine(strOutput, l_colRed);
		}
	}
}

void OnConsoleFationPVPStatus()
{	
	CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CECFactionPVPModel &factionPVPModel = CECFactionPVPModel::Instance();
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	
	ACString temp;

	temp.Format(_AL("m_factionPVPOpen=%s"), factionPVPModel.IsFactionPVPOpen()?_AL("true"):_AL("false"));
	pGameUIMan->AddConsoleLine(temp, l_colGreen);
	
	temp.Format(_AL("m_inFactionPVP=%s"), factionPVPModel.IsInFactionPVP()?_AL("true"):_AL("false"));
	pGameUIMan->AddConsoleLine(temp, l_colGreen);

	temp.Format(_AL("faction_id=%d"), pHost->GetFactionID());
	pGameUIMan->AddConsoleLine(temp, l_colGreen);
	
	temp.Format(_AL("CanAttackFactionPVPMineCar()=%s"), pHost->CanAttackFactionPVPMineCar()?_AL("true"):_AL("false"));
	pGameUIMan->AddConsoleLine(temp, l_colGreen);
	
	temp.Format(_AL("CanAttackFactionPVPMineBase()=%s"), pHost->CanAttackFactionPVPMineBase()?_AL("true"):_AL("false"));
	pGameUIMan->AddConsoleLine(temp, l_colGreen);
	
	temp.Format(_AL("m_inGuildBattle=%s"), !factionPVPModel.IsInGuildBattle()?_AL("true"):_AL("false"));	
	pGameUIMan->AddConsoleLine(temp, l_colGreen);
	
	temp.Format(_AL("m_inBattleStatusReady=%s"), factionPVPModel.InBattleStatusReady()?_AL("true"):_AL("false"));	
	pGameUIMan->AddConsoleLine(temp, l_colGreen);
	if (factionPVPModel.InBattleStatusReady()){
		temp.Format(_AL("m_idFaction=%d"), factionPVPModel.FactionID());
		pGameUIMan->AddConsoleLine(temp, l_colGreen);
		
		temp.Format(_AL("m_score=%d"), factionPVPModel.Score());		
		pGameUIMan->AddConsoleLine(temp, l_colGreen);
		
		temp.Format(_AL("m_robbedMineCarCount=%d"), factionPVPModel.RobbedMineCarCount());		
		pGameUIMan->AddConsoleLine(temp, l_colGreen);
		
		temp.Format(_AL("m_robbedMineBaseCount=%d"), factionPVPModel.RobbedMineBaseCount());		
		pGameUIMan->AddConsoleLine(temp, l_colGreen);
		
		temp.Format(_AL("m_canGetBonus=%s"), factionPVPModel.CanGetBonus()?_AL("true"):_AL("false"));		
		pGameUIMan->AddConsoleLine(temp, l_colGreen);
	}
	temp.Format(_AL("m_mapReady=%s"), factionPVPModel.FactionPVPMapReady()?_AL("true"):_AL("false"));	
	pGameUIMan->AddConsoleLine(temp, l_colGreen);
	if (factionPVPModel.FactionPVPMapReady()){
		for (CECFactionPVPModel::DomainWithResourceIterator cit = factionPVPModel.BeginDomainWithResource(); cit != factionPVPModel.EndDomainWithResource(); ++ cit){
			temp.Format(_AL("%d,"), *cit);
			pGameUIMan->AddConsoleLine(temp, l_colGreen);
		}
	}
	temp.Format(_AL("m_reportListReady=%s"), factionPVPModel.ReportListReady()?_AL("true"):_AL("false"));	
	pGameUIMan->AddConsoleLine(temp, l_colGreen);
	if (factionPVPModel.ReportListReady()){
		for (int i(0); i < factionPVPModel.ReportListCount(); ++ i){
			const CECFactionPVPModel::ReportItem *p = factionPVPModel.ReportListItem(i);
			temp.Format(_AL("[%d]roleid=%d,killCount=%u,deathCount=%u,useToolCount=%u,score=%d"),
				i, p->roleid, p->killCount, p->deathCount, p->useToolCount, p->score);			
			pGameUIMan->AddConsoleLine(temp, l_colGreen);
		}
	}
	temp.Format(_AL("m_sortIndex=%d"), factionPVPModel.ReportSortIndex());	
	pGameUIMan->AddConsoleLine(temp, l_colGreen);
	
	temp.Format(_AL("m_sortMethod=%d"), factionPVPModel.DefaultReportSortMethod());	
	pGameUIMan->AddConsoleLine(temp, l_colGreen);
}