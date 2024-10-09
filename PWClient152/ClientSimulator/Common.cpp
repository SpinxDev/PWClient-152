/*
 * FILE: Common.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "StdAfx.h"
#include "ClientSimulator.h"
#include "ClientSimulatorDlg.h"

#include "Common.h"
#include "AMiniDump.h"
#include "SessionManager.h"
#include "elementdataman.h"
#include "EC_Skill.h"
#include "EC_RoleTypes.h"

#include <AF.h>
#include <algorithm>
#include <set>
#include <vector>


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define EXTRACT_ITEM_ATTRIBUTE(TYPE, CONTENT) \
	case DT_ ## TYPE: \
		value = ((TYPE*)pDataMan->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType))->CONTENT; \
		break;


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////

ALog	g_Log;
char	g_szWorkDir[MAX_PATH];
char	g_szIniFile[MAX_PATH];

A3DVECTOR3	g_vOrigin(0.0f);
A3DVECTOR3	g_vAxisX(1.0f, 0.0f, 0.0f);
A3DVECTOR3	g_vAxisY(0.0f, 1.0f, 0.0f);
A3DVECTOR3	g_vAxisZ(0.0f, 0.0f, 1.0f);

CsMutex*	g_csLog;
CsMutex*  	g_csException;

//	Error messages
static char* l_aErrorMsgs[] = 
{
	"Unknown error.",			//	ECERR_UNKNOWN
	"Invalid parameter.",		//	ECERR_INVALIDPARAMETER	
	"Not enough memory.",		//	ECERR_NOTENOUGHMEMORY
	"File operation error.",	//	ECERR_FILEOPERATION	
	"Failed to call function.",	//	ECERR_FAILEDTOCALL	
	"Wrong version.",			//	ECERR_WRONGVERSION
	"Can't find the config file!",
	"Load the server_error.txt failed!",
	"Load the elements.data failed!",
	"Initialize Windows Sockets failed!"
};


///////////////////////////////////////////////////////////////////////////
//  
//  Implement Global functions
//  
///////////////////////////////////////////////////////////////////////////

// 输出信息到控制台
void AddConsoleText(const char* szMessage, int color)
{
	CClientSimulatorDlg* pMainWnd =
		dynamic_cast<CClientSimulatorDlg*>(AfxGetMainWnd());
	if( pMainWnd ) pMainWnd->PrintMessage(szMessage, color);
}

// 清除控制台信息
void ClearConsoleText()
{
	CClientSimulatorDlg* pMainWnd =
		dynamic_cast<CClientSimulatorDlg*>(AfxGetMainWnd());
	if( pMainWnd ) pMainWnd->ClearMessage();
}

static void _LogOutput(const char* szMsg)
{
	if( CsLockScoped lock(g_csLog) )
	{
		g_Log.Log(szMsg);
		AddConsoleText(szMsg, LOGCOLOR_WHITE);
	}
}

//	Initialize log system
bool glb_InitLogSystem(const char* szFile)
{
	if (!g_Log.Init(szFile, "Element client simulator log file"))
		return false;

	a_RedirectDefLogOutput(_LogOutput);
	g_csLog = new CsMutex;
	return true;
}

//	Close log system
void glb_CloseLogSystem()
{
	g_Log.Release();
	a_RedirectDefLogOutput(NULL);
	delete g_csLog;
	g_csLog = NULL;
}

//	Output predefined error
void glb_ErrorOutput(int iErrCode, const char* szFunc, int iLine)
{
	char szMsg[1024];
	sprintf(szMsg, "%s: %s (line: %d)", szFunc, l_aErrorMsgs[iErrCode], iLine);
	a_LogOutput(1, szMsg);
}

//  Show error message box
void glb_ErrorMessage(int iErrCode)
{
	if( iErrCode >= 0 && iErrCode < sizeof(l_aErrorMsgs) )
	{
		AfxMessageBox(l_aErrorMsgs[iErrCode], MB_OK|MB_ICONERROR);
	}
}

unsigned long glb_HandleException( LPEXCEPTION_POINTERS pExceptionPointers )
{
	if( IsDebuggerPresent() )
		return EXCEPTION_CONTINUE_SEARCH;
	
	TCHAR szFile[MAX_PATH];
	wsprintf(szFile, _AL("%S\\client_simulator.dmp"), g_szWorkDir);
	
	if( GetVersion() < 0x80000000 )
	{
		// WinNT or Win2000
		AMiniDump::Create(NULL, pExceptionPointers, szFile, g_csException->GetMtx());
	}
	
	return EXCEPTION_EXECUTE_HANDLER;
}

// 输出脚本错误
void ScriptErrHandler( const char* szMsg )
{
	a_LogOutput(1, szMsg);
}

// 取应用程序参数表
void ParseCommandLine( const AString& line,StringVector& params )
{
	int i = 0;
	int len = line.GetLength();
	params.clear();

#define IS_SEPCHAR(ch) ((ch) == ' ' || (ch) == '\t')

	while( i < len )
	{
		while( i < len && IS_SEPCHAR(line[i]) ) i++;
		if( i >= len ) return;

		if( line[i] == '"' )
		{
			i++;
			int j = i;
			while( i < len && line[i] != '"' ) i++;
			params.push_back( line.Mid(j, i-j) );
			i++;
		}
		else
		{
			int j = i;
			while( i < len && !IS_SEPCHAR(line[i]) ) i++;
			params.push_back( line.Mid(j, i-j) );
		}
	}
}

// 遍历指定目录，获取指定文件
void EnumFolder( const char* pattern,StringVector& file_list )
{
	WIN32_FIND_DATA data;
	file_list.clear();

	HANDLE hFind = FindFirstFile(pattern, &data);
	while( hFind != INVALID_HANDLE_VALUE )
	{
		file_list.push_back( data.cFileName );

		if( !FindNextFile(hFind, &data) )
			break;
	}

	FindClose( hFind );
}

// 获取物体的堆叠信息
int GetItemPileLimit( int tid )
{
	elementdataman* pDataMan = SessionManager::GetSingleton().GetDataMan();

	int value = 0;
	DATA_TYPE DataType = pDataMan->get_data_type(tid, ID_SPACE_ESSENCE);
	switch( DataType )
	{
	EXTRACT_ITEM_ATTRIBUTE(WEAPON_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(PROJECTILE_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(ARMOR_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(DECORATION_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(FASHION_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(MEDICINE_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(MATERIAL_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(DAMAGERUNE_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(ARMORRUNE_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(SKILLTOME_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(FLYSWORD_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(TOWNSCROLL_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(UNIONSCROLL_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(REVIVESCROLL_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(ELEMENT_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(TOSSMATTER_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(TASKMATTER_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(STONE_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(WINGMANWING_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(TASKDICE_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(TASKNORMALMATTER_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(FACETICKET_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(FACEPILL_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(GM_GENERATOR_ESSENCE, pile_num_max)
	//EXTRACT_ITEM_ATTRIBUTE(RECIPE_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(PET_EGG_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(PET_FOOD_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(PET_FACETICKET_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(FIREWORKS_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(WAR_TANKCALLIN_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(SKILLMATTER_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(INC_SKILL_ABILITY_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(REFINE_TICKET_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(DESTROYING_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(BIBLE_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(SPEAKER_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(AUTOHP_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(AUTOMP_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(DOUBLE_EXP_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(DYE_TICKET_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(TRANSMITSCROLL_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(GOBLIN_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(GOBLIN_EQUIP_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(GOBLIN_EXPPILL_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(SELL_CERTIFICATE_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(TARGET_ITEM_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(LOOK_INFO_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(WEDDING_BOOKCARD_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(WEDDING_INVITECARD_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(SHARPENER_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(FACTION_MATERIAL_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(CONGREGATE_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(FORCE_TOKEN_ESSENCE, pile_num_max)
	EXTRACT_ITEM_ATTRIBUTE(DYNSKILLEQUIP_ESSENCE, pile_num_max)
	default: ASSERT(0);
	}

	return value;
}

// 对技能进行排序
struct SkillSortHelper
{
	// 检测技能B，是否是技能A的前置技能
	bool IsPreSkill(int A, int B)
	{
		CECSkill skillA(A, 1);
		SkillArrayWrapper wapper = skillA.GetRequiredSkill();
		int iPreA = wapper.Count() > 0 ? wapper[0] : 0;
		if( !iPreA )
			return false;
		else if( iPreA == B )
			return true;
		else
			return IsPreSkill(iPreA, B);
	}

	bool operator () (int skill1, int skill2)
	{
		if( IsPreSkill(skill1, skill2) )
			return false;
		else if( IsPreSkill(skill2, skill1) )
			return true;
		else
			return skill1 < skill2;
	}
};

// 枚举指定职业的技能列表
void EnumSkillList(int iProfession, abase::vector<int>& skills)
{
	static const int LEARN_SKILL_MAP[NUM_PROFESSION] =
	{
		2252,					// 武士
		2253,					// 法师
		26752,					// 巫师
		9097,					// 妖精
		2254,					// 妖兽
		26712,					// 刺客
		6845,					// 羽芒
		2255,					// 羽灵
		28676,					// 剑灵
		28867,					// 魅灵
	};

	DATA_TYPE dt;
	elementdataman* pDataMan = SessionManager::GetSingleton().GetDataMan();

	if( iProfession < 0 || iProfession >= NUM_PROFESSION )
		return;

	NPC_SKILL_SERVICE* pTeach = (NPC_SKILL_SERVICE*)pDataMan->get_data_ptr(
		LEARN_SKILL_MAP[iProfession], ID_SPACE_ESSENCE, dt);
	if( dt != DT_NPC_SKILL_SERVICE ) return;

	std::set<int> skillSet;
	for( int j=0;j<128;j++ )
	{
		if( !pTeach->id_skills[j] || skillSet.find(pTeach->id_skills[j]) != skillSet.end() )
			continue;

		// 只学主动技能
		CECSkill skillData(pTeach->id_skills[j], 1);
		if( !skillData.IsPositiveSkill() ) continue;

		skills.push_back(pTeach->id_skills[j]);
		skillSet.insert(pTeach->id_skills[j]);
	}

	// 按照学习的先后顺序进行排序
	std::sort(skills.begin(), skills.end(), SkillSortHelper());
}