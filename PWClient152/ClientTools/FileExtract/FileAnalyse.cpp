/*
 * FILE: FileAnalyse.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/6/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "StdAfx.h"
#include "FileAnalyse.h"
#include "elementdataman.h"
#include "EC_Utility.h"
#include "EC_Model.h"
#include "EC_ModelMan.h"
#include "LuaState.h"
#include "C:\CPW_DevLatest_ElementClientVSS\CElement\CElementSkill\ElementSkill.h"
#include "Policy.h"
#include "FWTemplate.h"
#include "FWAssemblySet.h"

#include <A3DMacros.h>
#include <A3DEngine.h>
#include <A3DDevice.h>
#include <A3DCamera.h>
#include <A3DGFXEx.h>
#include <A3DGFXExMan.h>
#include <A3DGFXModel.h>
#include <A3DGFXECModel.h>
#include <A3DGFXContainer.h>
#include <A3DGFXSound.h>
#include <A3DCombinedAction.h>
#include <A3DGFXFuncs.h>
#include <RandStringContainer.h>
#include <A3DSkillGfxComposer.h>
#include <A3DSkin.h>
#include <A3DSkinMan.h>
#include <A3DSkinModel.h>
#include <A3DSkinModelAct.h>
#include <A3DSkeleton.h>
#include <AXMLFile.h>
#include <AUICommon.h>
#include <CSplit.h>

#include <ALog.h>
#include <AF.h>
#include <AFilePackMan.h>
#include <io.h>


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define DEFCAMERA_FOV 56.0f
#define ALL_CONTENT "全部"

#define SHOW_ERRMSG(str) ::MessageBoxA(NULL, str, "错误", MB_OK|MB_ICONERROR)

const char* ERR_OPENPCKFAIL = "打开pck文件失败，请将程序放于客户端目录下！";
const char* ERR_OPENDATAFAIL = "打开elements.data失败！";
const char* ERR_INVALIDDESTDIR = "客户端目录不允许导出文件，请选择其他目录！";
const char* ERR_INVALIDNPCID = "这不是一个合法的NPC模板ID！";
const char* ERR_INITA3DFAIL = "初始化A3D引擎失败！";
const char* ERR_INVALIDUIFILE = "无效的界面文件，加载失败！";
const char* ERR_INVALIDSKILL = "无效的技能ID，分析技能失败！";
const char* ERR_OPENAIFAIL = "加载策略数据失败！";
const char* ERR_INVALIDEQUIP = "无效的装备ID，分析装备失败！";

#define EXTRACT_UI_ITEM(item, key, dir) \
	strPath.Empty(); \
	XMLGET_STRING_VALUE(item, key, strPath); \
	if( !strPath.IsEmpty() ) { \
		AString temp = (dir != "") ? (dir + AString("\\") + strPath) : strPath; \
		AddFileRecord(temp); \
	}


///////////////////////////////////////////////////////////////////////////
//  
//  Local types and Global variables
//  
///////////////////////////////////////////////////////////////////////////

ALog g_Log;
char g_szWorkDir[MAX_PATH];

static const char* l_strProf[] =
{
	"武侠", "法师", "巫师", "妖精", "妖兽", "刺客", "羽芒", "羽灵", "剑灵", "魅灵", "夜影", "月仙"
};

static const char* l_strPlayerModel[] =
{
	"Models\\Players\\形象\\武侠男\\躯干\\武侠男.ecm", "Models\\Players\\形象\\武侠女\\躯干\\武侠女.ecm",
	"Models\\Players\\形象\\法师男\\躯干\\法师男.ecm", "Models\\Players\\形象\\法师女\\躯干\\法师女.ecm",
	"Models\\Players\\形象\\巫师男\\躯干\\巫师男.ecm", "Models\\Players\\形象\\巫师女\\躯干\\巫师女.ecm",
	"", "Models\\Players\\形象\\妖精\\躯干\\妖精.ecm",
	"Models\\Players\\形象\\妖兽男\\躯干\\妖兽男.ecm", "",
	"Models\\Players\\形象\\刺客男\\躯干\\刺客男.ecm", "Models\\Players\\形象\\刺客女\\躯干\\刺客女.ecm",
	"Models\\Players\\形象\\羽芒男\\躯干\\羽芒男.ecm", "Models\\Players\\形象\\羽芒女\\躯干\\羽芒女.ecm",
	"Models\\Players\\形象\\羽灵男\\躯干\\羽灵男.ecm", "Models\\Players\\形象\\羽灵女\\躯干\\羽灵女.ecm",
	"Models\\Players\\形象\\剑灵男\\躯干\\剑灵男.ecm", "Models\\Players\\形象\\剑灵女\\躯干\\剑灵女.ecm",
	"Models\\Players\\形象\\魅灵男\\躯干\\魅灵男.ecm", "Models\\Players\\形象\\魅灵女\\躯干\\魅灵女.ecm",
	"Models\\Players\\形象\\夜影男\\躯干\\夜影男.ecm", "Models\\Players\\形象\\夜影女\\躯干\\夜影女.ecm",
	"Models\\Players\\形象\\月仙男\\躯干\\月仙男.ecm", "Models\\Players\\形象\\月仙女\\躯干\\月仙女.ecm",
};


///////////////////////////////////////////////////////////////////////////
//  
//  Local Functions
//  
///////////////////////////////////////////////////////////////////////////

extern const char* AfxGetECMHullPath();
extern char* ECMPathFindExtensionA(const char* szPath);
extern char* ECMPathFindFileNameA(const char* szPath);

static void _LogOutput(const char* szMsg)
{
	g_Log.Log(szMsg);
}


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class FileAnalyse
//  
///////////////////////////////////////////////////////////////////////////

// 获取实例
FileAnalyse& FileAnalyse::GetInstance()
{
	static FileAnalyse obj;
	return obj;
}

// 构造函数
FileAnalyse::FileAnalyse()
{
	m_pA3DEngine = NULL;
	m_pA3DDevice = NULL;
	m_pDataMan = NULL;
	m_pGFXExMan = NULL;
	m_pModelMan = NULL;
	m_pA3DCamera = NULL;
	m_pCBOperation = NULL;
	m_pPolicies = NULL;
}

// 析构函数
FileAnalyse::~FileAnalyse()
{
	AfxReleaseFWEditor();
	A3DRELEASE(m_pA3DCamera);
	A3DRELEASE(m_pGFXExMan);
	A3DRELEASE(m_pModelMan);
	A3DRELEASE(m_pA3DEngine);

	if( m_pPolicies )
	{
		m_pPolicies->Release();
		delete m_pPolicies;
		m_pPolicies = NULL;
	}

	if( m_pDataMan )
	{
		delete m_pDataMan;
		m_pDataMan = NULL;
	}

	g_LuaStateMan.Release();
	g_Log.Release();
	af_Finalize();
}

// 初始化文件分析器
bool FileAnalyse::Init()
{
	static const char* szPckFiles[] =
	{
		"configs.pck", "configs",
		"facedata.pck", "facedata",
		"models.pck", "models",
		"gfx.pck", "gfx",
		"interfaces.pck", "interfaces",
		"surfaces.pck", "surfaces",
		"grasses.pck", "grasses",
		"sfx.pck", "sfx",
		"shaders.pck", "shaders",
		"textures.pck", "textures",
	};

	int i;
	af_Initialize();
	GetCurrentDirectoryA(MAX_PATH, g_szWorkDir);
	af_SetBaseDir(g_szWorkDir);

	// 打开日志
	g_Log.Init("FileExtract.log", "File extractor log file");
	a_RedirectDefLogOutput(_LogOutput);

	for( i=0;i<sizeof(szPckFiles)/sizeof(const char*)/2;i++ )
	{
		if( !g_AFilePackMan.OpenFilePackageInGame(szPckFiles[i*2]) && _access(szPckFiles[i*2+1], 0) == -1 )
		{
			SHOW_ERRMSG(ERR_OPENPCKFAIL);
			a_LogOutput(1, "FileAnalyse::Init, Open the package (%s) failed, and can't find the separate directory!", szPckFiles[i]);
			return false;
		}
	}

	// 加载elements.data
	m_pDataMan = new elementdataman();
	if( !m_pDataMan || m_pDataMan->load_data("data\\elements.data") == -1 )
	{
		SHOW_ERRMSG(ERR_OPENDATAFAIL);
		a_LogOutput(1, "FileAnalyse::Init, Open the elements.data failed!");
		return false;
	}

	// 初始化A3DEngine
	if( !InitA3DEngine() )
	{
		SHOW_ERRMSG(ERR_INITA3DFAIL);
		a_LogOutput(1, "FileAnalyse::Init, Init the A3DEngine failed!");
		return false;
	}

	AfxInitFWEditor(m_pA3DDevice);

	// 初始化装备特效表
	if( !LoadEquipGfx() )
	{
		a_LogOutput(1, "FileAnalyse::Init, Load the equipment gfx failed!");
		return false;
	}

	// 加载策略数据
	m_pPolicies = new CPolicyDataManager();
	if( !m_pPolicies || !m_pPolicies->Load("data\\aipolicy.data") )
	{
		SHOW_ERRMSG(ERR_OPENAIFAIL);
		a_LogOutput(1, "FileAnalyse::Init, Load the ai policy data failed!");
		return false;
	}

	// 初始化策略哈希表
	for( i=0;i<m_pPolicies->GetPolicyNum();i++ )
	{
		CPolicyData* pPolicy = m_pPolicies->GetPolicy(i);
		m_PolicyMap[pPolicy->GetID()] = pPolicy;
	}

	return true;
}

// 初始化A3DEngine
bool FileAnalyse::InitA3DEngine()
{
	a_InitRandom();

	//	Init Engine
	if (!(m_pA3DEngine = new A3DEngine))
	{
		a_LogOutput(1, "FileAnalyse::InitA3DEngine, create the A3DEngine object failed!");
		return false;
	}

	RECT rcWnd;
	AfxGetMainWnd()->GetClientRect(&rcWnd);

	A3DDEVFMT devFmt;
	devFmt.bWindowed = TRUE;
	devFmt.nWidth = rcWnd.right;
	devFmt.nHeight = rcWnd.bottom;
	devFmt.fmtTarget = A3DFMT_UNKNOWN;
	devFmt.fmtDepth = A3DFMT_UNKNOWN;
	devFmt.bVSync = false;
	
	// A3DDEV_ALPHATARGETFIRST: if we use alpha target first and the desktop display mode is just what we use 
	// as the full-screen mode for the game, the game may not minimized when switched off. To solve this, we should
	// add a manual call to ChangeDisplaySetting when we receive WM_ACTIVATEAPP
	DWORD devFlags = A3DDEV_ALLOWMULTITHREAD;
	if (!m_pA3DEngine->Init(AfxGetInstanceHandle(), AfxGetMainWnd()->GetSafeHwnd(), &devFmt, devFlags))
	{
		a_LogOutput(1, "FileAnalyse::InitA3DEngine, Init the A3DEngine failed!");
		return false;
	}

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();

	// In element client we use multithread to load terrain meshes etc.
	// so the reset operation can not be done by the device automatically, or we may not release all unmanaged
	// device objects before we reset, so reset will fail.
	m_pA3DDevice->SetAutoResetFlag(false);

	// show a black screen without loading text
	m_pA3DDevice->BeginRender();
	m_pA3DDevice->Clear(D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	m_pA3DDevice->EndRender();
	m_pA3DDevice->Present();

	//	Create perspective camera
	if (!(m_pA3DCamera = new A3DCamera))
	{
		a_LogOutput(1, "FileAnalyse::InitA3DEngine, Create the camera object failed!");
		return false;
	}

	if (!m_pA3DCamera->Init(m_pA3DDevice, DEFCAMERA_FOV, 0.2f, 2000.0f, rcWnd.right * 1.0f / rcWnd.bottom))
	{
		a_LogOutput(1, "FileAnalyse::InitA3DEngine, Init the A3DCamera failed!");
		return false;
	}
	
	m_pA3DCamera->SetPos(g_vOrigin + g_vAxisY * 50.0f);
	m_pA3DCamera->SetDirAndUp(g_vAxisZ, g_vAxisY);

	//	Create gfx manager
	m_pGFXExMan = new A3DGFXExMan();
	if (!m_pGFXExMan->Init(m_pA3DDevice))
	{
		a_LogOutput(1, "FileAnalyse::InitA3DEngine, Init the Gfx manager failed!");
		return false;
	}

	// 初始化Lua模块
	g_LuaStateMan.Init();
	InitECMApi(g_LuaStateMan.GetConfigState());

	//  Create the model manager
	m_pModelMan = new CECModelMan();
	m_pModelMan->Init();

	return true;
}

// 初始化玩家装备特效表
bool FileAnalyse::LoadEquipGfx()
{
	for( int i=0;i<sizeof(l_strPlayerModel)/sizeof(const char*);i++ )
	{
		if( !strcmp(l_strPlayerModel[i], "") )
			continue;

		CECModel* pModel = new CECModel();
		if( !pModel->Load(l_strPlayerModel[i]) )
		{
			A3DRELEASE(pModel);
			a_LogOutput(1, "FileAnalyse::LoadEquipGfx, Load the player model failed! (%s)", l_strPlayerModel[i]);
			continue;
		}

		int iLength = pModel->GetScript(enumECMScriptChangeEquipTableInit).GetLength();
		const char* szLuaText = (const char*)pModel->GetScript(enumECMScriptChangeEquipTableInit);

		AMemFile file;
		file.Attach((BYTE*)szLuaText, iLength, 0);
		AScriptFile sf;
		sf.Open(&file);

		while(!sf.IsEnd())
		{
			sf.GetNextToken(true);
			if( !strstr(sf.m_szToken, "self.equip_id_table[") || !strstr(sf.m_szToken, "][") )
				continue;

			// 提取equip_id
			int equip_id = 0, gfx_idx;
			sscanf(sf.m_szToken, "self.equip_id_table[%d][%d]", &equip_id, &gfx_idx);

			// 找gfx_path
			while(sf.GetNextToken(true))
			{
				if( !strcmp(sf.m_szToken, "gfx_path") )
				{
					sf.GetNextToken(true);	// 跳过=号
					sf.GetNextToken(true);	// 特效文件路径

					if( m_EquipGfx.find(equip_id) == m_EquipGfx.end() )
						m_EquipGfx[equip_id] = std::set<AString>();
					m_EquipGfx[equip_id].insert(sf.m_szToken);
					break;
				}
			}
		}

		sf.Close();
		file.Detach();
		A3DRELEASE(pModel);
	}

	return true;
}

// 分析指定NPC或UI界面
void FileAnalyse::Analyse(const char* szText, FileType Type)
{
	m_FileDup.clear();
	m_Filelist.clear();

	CSplit split(szText);
	CSplit::VectorAString strs = split.Split(",");

	for( size_t i=0;i<strs.size();i++ )
	{
		switch( Type )
		{
		case TYPE_NPCMODEL:
			{
				int tid = atoi(strs[i]);
				AnalyseNPCModel(tid);
			}
			break;
		case TYPE_UI:
			if( af_CheckFileExt(strs[i], ".dcf") )
				AnalyseDCFFile(strs[i]);
			else
				AnalyseUI(strs[i]);
			break;
		case TYPE_SKILL:
			{
				int skill_id = atoi(strs[i]);
				if( !AnalyseSkill(skill_id) )
					SHOW_ERRMSG(ERR_INVALIDSKILL);
			}
			break;
		case TYPE_PLAYERMODEL:
			AnalysePlayerModel(strs[i]);
			break;
		case TYPE_BUFF:
			if( strs[i] == ALL_CONTENT )
				AnalyseBuff(-1);
			else
			{
				int buff_id = atoi(strs[i]);
				AnalyseBuff(buff_id);
			}
			break;
		case TYPE_ECMODEL:
			AnalyseECModelByName(strs[i]);
			break;
		case TYPE_EQUIP:
			{
				int tid = atoi(strs[i]);
				AnalyseEquip(tid);
			}
			break;
		case TYPE_SCALEDNPC:
			AnalyseScaledNPC();
			break;
		case TYPE_EQUIPGFXNOHANGER:
			AnalyseEquipGfxNoHanger();
			break;
		case TYPE_FIREWORK:
			{
				int tid = atoi(strs[i]);
				AnalyseFirework(tid);
			}
			break;
		default:
			ASSERT(0);
		}
	}
}

// 分析NPC模型相关的文件
bool FileAnalyse::AnalyseNPCModel(int tid)
{
	bool bRet = false;
	DATA_TYPE dt;
	AString strModelFile;
	const void* pData = m_pDataMan->get_data_ptr(tid, ID_SPACE_ESSENCE, dt);
	switch( dt )
	{
	case DT_NPC_ESSENCE:
		strModelFile = ((NPC_ESSENCE*)pData)->file_model;
		break;
	case DT_MONSTER_ESSENCE:
		strModelFile = ((MONSTER_ESSENCE*)pData)->file_model;
		break;
	case DT_PET_ESSENCE:
		strModelFile = ((PET_ESSENCE*)pData)->file_model;
		break;
	default:
		SHOW_ERRMSG(ERR_INVALIDNPCID);
		a_LogOutput(1, "FileAnalyse::AnalyseNPCModel, Invalid npc id(%d), analyse failed!", tid);
		return false;
	}

	// ecm文件
	bRet = AnalyseECModelByName(strModelFile);

	// ski文件
	char szSkin[MAX_PATH];
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();

	// Skin lod1
	strncpy(szSkin, strModelFile, MAX_PATH);
	glb_ChangeExtension(szSkin, "ski");
	A3DSkin* pSkin1 = pSkinMan->LoadSkinFile(szSkin);
	bRet = AnalyseSkin(pSkin1);

	// Skin lod2
	glb_ClearExtension(szSkin);
	strcat(szSkin, "二级.ski");
	A3DSkin* pSkin2 = pSkinMan->LoadSkinFile(szSkin);
	if( pSkin2 ) bRet = AnalyseSkin(pSkin2);

	pSkinMan->ReleaseSkin(&pSkin1);
	pSkinMan->ReleaseSkin(&pSkin2);

	// 导出技能相关的资源
	if( dt == DT_MONSTER_ESSENCE )
	{
		int i;
		MONSTER_ESSENCE* pMonster = (MONSTER_ESSENCE*)pData;

		// 攻击效果
		bRet = AnalyseGfxByName(pMonster->file_gfx_short);
		bRet = AnalyseGfxByName(pMonster->file_gfx_short_hit);

		if( pMonster->id_skill ) bRet = AnalyseSkill(pMonster->id_skill);

		// 条件技能
		for( i=0;i<STRUCT_SIZE(pMonster->skill_hp75);i++ )
		{
			if( pMonster->skill_hp75[i].id_skill )
				bRet = AnalyseSkill(pMonster->skill_hp75[i].id_skill);
		}
		for( i=0;i<STRUCT_SIZE(pMonster->skill_hp50);i++ )
		{
			if( pMonster->skill_hp50[i].id_skill )
				bRet = AnalyseSkill(pMonster->skill_hp50[i].id_skill);
		}
		for( i=0;i<STRUCT_SIZE(pMonster->skill_hp25);i++ )
		{
			if( pMonster->skill_hp25[i].id_skill )
				bRet = AnalyseSkill(pMonster->skill_hp25[i].id_skill);
		}
		// 普通被动技能
		for( i=0;i<STRUCT_SIZE(pMonster->skills);i++ )
		{
			if( pMonster->skills[i].id_skill )
				bRet = AnalyseSkill(pMonster->skills[i].id_skill);
		}

		// 策略相关技能
		if( pMonster->common_strategy )
		{
			CPolicyData* pPolicy = m_PolicyMap[pMonster->common_strategy];
			if( !pPolicy ) return bRet;

			for( i=0;i<pPolicy->GetTriggerPtrNum();i++ )
			{
				CTriggerData* pTrigger = pPolicy->GetTriggerPtr(i);
				bRet = AnalysePolicyTrigger(pPolicy, pTrigger);
			}
		}
	}
	else
	{
		// 宠物技能可以学，所以不确定宠物都有哪些技能，这里就不导出了
		PET_ESSENCE* pPet = (PET_ESSENCE*)pData;

		// 宠物攻击效果
		bRet = AnalyseGfxByName(pPet->file_gfx_short);
	}

	return bRet;
}

// 分析玩家职业相关的模型数据
bool FileAnalyse::AnalysePlayerModel(const char* szProf)
{
	if( !strcmp(szProf, ALL_CONTENT) )
	{
		for( int i=0;i<sizeof(l_strPlayerModel)/sizeof(const char*)/2;i++ )
		{
			if( strcmp(l_strPlayerModel[i*2+0], "") )
				AnalyseECModelByName(l_strPlayerModel[i*2+0], true);
			if( strcmp(l_strPlayerModel[i*2+1], "") )
				AnalyseECModelByName(l_strPlayerModel[i*2+1], true);
		}
	}
	else
	{
		bool bFound = false;
		for( int i=0;i<sizeof(l_strProf)/sizeof(const char*);i++ )
		{
			if( !stricmp(l_strProf[i], szProf) )
			{
				bFound = true;
				break;
			}
		}

		if( bFound )
		{
			if( strcmp(l_strPlayerModel[i*2+0], "") )
				AnalyseECModelByName(l_strPlayerModel[i*2+0], true);
			if( strcmp(l_strPlayerModel[i*2+1], "") )
				AnalyseECModelByName(l_strPlayerModel[i*2+1], true);
		}
		else
		{
			a_LogOutput(1, "FileAnalyse::AnalysePlayerModel, Invalid profession name! (%s)", szProf);
			return false;
		}
	}

	return true;
}

// 分析玩家时装特效脚本
bool FileAnalyse::AnalyseModelScript(const char* szLuaText)
{
	bool bRet = false;
	if( !szLuaText || !strcmp(szLuaText, "") )
		return false;

	AMemFile file;
	file.Attach((BYTE*)szLuaText, strlen(szLuaText)+1, 0);

	AScriptFile sf;
	if( !sf.Open(&file) )
	{
		a_LogOutput(1, "FileAnalyse::AnalyseModelScript, Open the script text failed!");
		return false;
	}

	while( !sf.IsEnd() )
	{
		if( !sf.GetNextToken(true) )
			break;

		if( !stricmp(sf.m_szToken, "gfx_path") )
		{
			sf.GetNextToken(true);		// 跳过=号
			sf.GetNextToken(true);		// 特效的文件路径

			bRet = AnalyseGfxByName(sf.m_szToken);
		}
	}

	sf.Close();
	file.Detach();
	return true;
}

// 分析技能相关的资源
bool FileAnalyse::AnalyseSkill(int skill_id)
{
	using namespace GNET;
	ElementSkill* pSkill = ElementSkill::Create(skill_id, 1);
	if( !pSkill )
	{
		a_LogOutput(1, "FileAnalyse::AnalyseSkill, Invalid skill id, analyse failed!");
		return false;
	}

	AnalyseSGCFile(pSkill->GetEffect());
	AnalyseSGCFile(pSkill->GetElseEffect());

	pSkill->Destroy();
	return true;
}

// 分析BUFF相关的资源
bool FileAnalyse::AnalyseBuff(int buff_id)
{
	using namespace GNET;

	if( buff_id == -1 )
	{
		bool bRet = false;

		// 目前Buff ID最大值离1000还很遥远，暂时设这个数
		for( int i=0;i<1000;i++ )
		{
			const VisibleState* pState = VisibleState::Query(0, i);
			if( pState && pState->GetEffect() && strcmp(pState->GetEffect(), "") )
			{
				AString strPath = AString("策划联入\\状态效果\\") + pState->GetEffect();
				bRet = AnalyseGfxByName(strPath);
			}
		}

		return bRet;
	}
	else
	{
		const VisibleState* pState = VisibleState::Query(0, buff_id);
		if( !pState || !pState->GetEffect() || !strcmp(pState->GetEffect(), "") )
		{
			a_LogOutput(1, "FileAnalyse::AnalyseBuff, Invalid buff id, analyse failed!");
			return false;
		}

		return AnalyseGfxByName(AString("策划联入\\状态效果\\") + pState->GetEffect());
	}
}

// 分析装备相关的资源
bool FileAnalyse::AnalyseEquip(int equip_id)
{
	bool bRet = false;
	DATA_TYPE dt;
	const void* pData = m_pDataMan->get_data_ptr(equip_id, ID_SPACE_ESSENCE, dt);
	switch( dt )
	{
	case DT_WEAPON_ESSENCE:
		{
			WEAPON_ESSENCE* pEquip = (WEAPON_ESSENCE*)pData;
			if( strcmp(pEquip->file_model_left, "") )
				bRet = AnalyseECModelByName(pEquip->file_model_left);
			if( strcmp(pEquip->file_model_right, "") )
				bRet = AnalyseECModelByName(pEquip->file_model_right);
		}
		break;
	case DT_ARMOR_ESSENCE:
		{
			FilePathList fileList;
			ARMOR_ESSENCE* pEquip = (ARMOR_ESSENCE*)pData;
			if( pEquip->equip_location == 9 )
			{
				DATA_TYPE dt2;
				FACE_HAIR_ESSENCE* pHair = (FACE_HAIR_ESSENCE*)m_pDataMan->get_data_ptr(pEquip->id_hair, ID_SPACE_FACE, dt2);
				if( pHair && dt2 == DT_FACE_HAIR_ESSENCE )
					bRet = AnalyseSkinByName(pHair->file_hair_skin);
				FACE_TEXTURE_ESSENCE* pFaceTex = (FACE_TEXTURE_ESSENCE*)m_pDataMan->get_data_ptr(pEquip->id_hair_texture, ID_SPACE_FACE, dt2);
				if( pFaceTex && dt2 == DT_FACE_TEXTURE_ESSENCE )
				{
					AddFileRecord(pFaceTex->file_base_tex);
					AddFileRecord(pFaceTex->file_high_tex);
				}
			}
			else
			{
				GenSkinPath(pEquip->realname, fileList);
				for( size_t i=0;i<fileList.size();i++ )
					bRet = AnalyseSkinByName(fileList[i]);
			}
		}
		break;
	case DT_WINGMANWING_ESSENCE:
		{
			WINGMANWING_ESSENCE* pEquip = (WINGMANWING_ESSENCE*)pData;
			if( strcmp(pEquip->file_model, "") )
				bRet = AnalyseECModelByName(pEquip->file_model);
		}
		break;
	case DT_FLYSWORD_ESSENCE:
		{
			FLYSWORD_ESSENCE* pEquip = (FLYSWORD_ESSENCE*)pData;
			if( strcmp(pEquip->file_model, "") )
				bRet = AnalyseECModelByName(pEquip->file_model);
		}
		break;
	case DT_FASHION_ESSENCE:
		{
			FASHION_ESSENCE* pEquip = (FASHION_ESSENCE*)pData;
			if( pEquip->equip_location == 10 )
			{
				if( strcmp(pEquip->file_model_left, "") )
					bRet = AnalyseECModelByName(pEquip->file_model_left);
				if( strcmp(pEquip->file_model_right, "") )
					bRet = AnalyseECModelByName(pEquip->file_model_right);
			}
			else if( pEquip->equip_location == 9 )
			{
				DATA_TYPE dt2;
				FACE_HAIR_ESSENCE* pHair = (FACE_HAIR_ESSENCE*)m_pDataMan->get_data_ptr(pEquip->id_hair, ID_SPACE_FACE, dt2);
				if( pHair && dt2 == DT_FACE_HAIR_ESSENCE )
					bRet = AnalyseSkinByName(pHair->file_hair_skin);
				FACE_TEXTURE_ESSENCE* pFaceTex = (FACE_TEXTURE_ESSENCE*)m_pDataMan->get_data_ptr(pEquip->id_hair_texture, ID_SPACE_FACE, dt2);
				if( pFaceTex && dt2 == DT_FACE_TEXTURE_ESSENCE )
				{
					AddFileRecord(pFaceTex->file_base_tex);
					AddFileRecord(pFaceTex->file_high_tex);
				}
			}
			else
			{
				FilePathList fileList;
				GenSkinPath(pEquip->realname, fileList, pEquip->gender ? 2 : 1);
				for( size_t i=0;i<fileList.size();i++ )
					bRet = AnalyseSkinByName(fileList[i]);
			}
		}
		break;
	default:
		SHOW_ERRMSG(ERR_INVALIDEQUIP);
		a_LogOutput(1, "FileAnalyse::AnalyseEquip, Invalid equip id(%d), analyse failed!", equip_id);
		return false;
	}

	// 提取装备对应的特效
	EquipGfxMap::iterator it = m_EquipGfx.find(equip_id);
	if( it != m_EquipGfx.end() )
	{
		std::set<AString>& pathList = it->second;
		std::set<AString>::const_iterator pit;
		for( pit=pathList.begin();pit!=pathList.end();++pit )
			bRet = AnalyseGfxByName(*pit);
	}

	return bRet;
}

// 分析DCF文件
bool FileAnalyse::AnalyseDCFFile(const char* szDCFFile)
{
	bool bRet;
	AScriptFile file;
	AString strFilePath = szDCFFile;
	if( !file.Open(strFilePath) )
	{
		SHOW_ERRMSG(ERR_INVALIDUIFILE);
		a_LogOutput(1, "FileAnalyse::AnalyseDCFFile, Load the dcf file failed!");
		return false;
	}

	AddFileRecord(strFilePath);

	while( !file.IsEnd() )
	{
		if( !file.GetNextToken(true) )
			break;

		if( 0 == strcmpi(file.m_szToken, "#") ||
			0 == strcmpi(file.m_szToken, "//") )
		{
			file.SkipLine();
		}
		else
		{
			bRet = AnalyseUI(AString("Interfaces\\") + file.m_szToken);

			// Skip the parameters
			file.GetNextToken(false);
			file.GetNextToken(false);
			file.GetNextToken(false);
		}
	}

	file.Close();
	return true;
}

// 分析UI相关的文件
bool FileAnalyse::AnalyseUI(const char* szFilename)
{
	AString strPath;
	AXMLFile xmlFile;
	AString strFilePath = szFilename;
	if( !xmlFile.LoadFromFile(strFilePath) )
	{
		SHOW_ERRMSG(ERR_INVALIDUIFILE);
		a_LogOutput(1, "FileAnalyse::AnalyseUI, Load the interface file failed!");
		return false;
	}

	AddFileRecord(strFilePath);

	AXMLItem* pItem = xmlFile.GetRootItem()->GetFirstChildItem();
	EXTRACT_UI_ITEM(pItem, _AL("SoundEffect"), "");
	EXTRACT_UI_ITEM(pItem, _AL("SoundEffectClose"), "");

	// Resource For Dialog
	AXMLItem* pChildItem = pItem->GetFirstChildItem();
	while( pChildItem )
	{
		if( a_stricmp(pChildItem->GetName(), _AL("Resource")) == 0)
		{
			AXMLItem* pResouceChildItem = pChildItem->GetFirstChildItem();
			while( pResouceChildItem )
			{
				if( a_stricmp(pResouceChildItem->GetName(), _AL("FrameImage")) == 0 )
				{
					EXTRACT_UI_ITEM(pResouceChildItem, _AL("FileName"), "surfaces");
				}

				pResouceChildItem = pResouceChildItem->GetNextItem();
			}
		}
		else
		{
			// Resource For Controls
			if( !a_stricmp(pChildItem->GetName(), _AL("CHECK")) ||
				!a_stricmp(pChildItem->GetName(), _AL("RADIO")) ||
				!a_stricmp(pChildItem->GetName(), _AL("STILLIMAGEBUTTON")) )
			{
				EXTRACT_UI_ITEM(pChildItem, _AL("SoundEffect"), "");
				EXTRACT_UI_ITEM(pChildItem, _AL("SoundEffectClose"), "");
			}
			else if( !a_stricmp(pChildItem->GetName(), _AL("COMBO")) ||
				!a_stricmp(pChildItem->GetName(), _AL("LIST")) )
			{
				EXTRACT_UI_ITEM(pChildItem, _AL("SoundEffect"), "");
			}
			else if( !a_stricmp(pChildItem->GetName(), _AL("SUBDIALOG")) )
			{
				strPath.Empty();
				XMLGET_STRING_VALUE(pChildItem, _AL("DialogFile"), strPath);
				AnalyseUI("Interfaces\\" + strPath);
			}

			AXMLItem* pObjSubItem = pChildItem->GetFirstChildItem();
			while( pObjSubItem )
			{
				if( !a_stricmp(pObjSubItem->GetName(), _AL("Resource")) )
				{
					AXMLItem* pObjResourceItem = pObjSubItem->GetFirstChildItem();
					while( pObjResourceItem )
					{
						strPath.Empty();
						XMLGET_STRING_VALUE(pObjResourceItem, _AL("FileName"), strPath);
						if( !strPath.IsEmpty() )
						{
							if( af_CheckFileExt(strPath, ".gfx") )
								AnalyseGfxByName(strPath);
							else
							{
								AString temp = AString("surfaces\\") + strPath;
								AddFileRecord(temp);
							}
						}

						pObjResourceItem = pObjResourceItem->GetNextItem();
					}
				}

				pObjSubItem = pObjSubItem->GetNextItem();
			}
		}

		pChildItem = pChildItem->GetNextItem();
	}

	xmlFile.Release();
	return true;
}

// 分析策略触发器
bool FileAnalyse::AnalysePolicyTrigger(CPolicyData* pPolicy, CTriggerData* pTrigger)
{
	if( !pTrigger )
		return false;

	for( int i=0;i<pTrigger->GetOperaionNum();i++ )
	{
		CTriggerData::_s_operation* pOP = pTrigger->GetOperaion(i);
		if( pOP->iType == CTriggerData::o_use_skill )
		{
			int skill_id = ((O_USE_SKILL*)pOP->pParam)->uSkill;
			if( skill_id ) AnalyseSkill(skill_id);
		}
	}

	return true;
}

// 分析GFX相关的文件
bool FileAnalyse::AnalyseGfxByName(const char* filename)
{
	if( !filename || !strcmp(filename, "") )
		return false;

	AString strPath = filename;
	strPath.MakeLower();
	if( strPath.Find("gfx\\") == 0 )
		strPath.CutLeft(4);
	A3DGFXEx* pGFX = AfxGetGFXExMan()->LoadGfx(m_pA3DDevice, strPath);
	if( !pGFX )
	{
		A3DRELEASE(pGFX);
		a_LogOutput(1, "FileAnalyse::AnalyseGfxByName, Load the gfx failed! (%s)", filename);
		return false;
	}

	bool bRet = AnalyseGfx(pGFX);
	A3DRELEASE(pGFX);
	return bRet;
}

// 分析GFX相关的文件
bool FileAnalyse::AnalyseGfx(A3DGFXEx* pGFX)
{
	bool bRet = true;
	if( !pGFX ) return false;

	// gfx文件本身
	AddFileRecord(pGFX->GetPath());

	for( int i=0;i<pGFX->GetElementCount();i++ )
	{
		A3DGFXElement* pElement = pGFX->GetElement(i);
		if( !pElement->GetTexPath().IsEmpty() )
			AddFileRecord("Gfx\\textures\\" + pElement->GetTexPath());
		if( !pElement->GetShaderTexture().IsEmpty() )
			AddFileRecord("Gfx\\textures\\" + pElement->GetShaderTexture());

		switch( pElement->GetEleTypeId() )
		{
		case ID_ELE_TYPE_MODEL:
			{
				A3DGFXModel* pModel = (A3DGFXModel*)pElement;

				// 提取A3DSkinModel相关资源
				bRet = AnalyseSkinModel(pModel->GetSkinModel());
			}
			break;
		case ID_ELE_TYPE_GFX_CONTAINER:
			{
				A3DGFXContainer* pContainer = (A3DGFXContainer*)pElement;

				// 提取Gfx的资源
				bRet = AnalyseGfxByName(pContainer->GetGfxPath());
			}
			break;
		case ID_ELE_TYPE_ECMODEL:
			{
				A3DGFXECModel* pModel = (A3DGFXECModel*)pElement;

				// 提取CECModel相关资源
				bRet = AnalyseECModelByName(pModel->GetECMFileName());
			}
			break;
		case ID_ELE_TYPE_SOUND:
			{
				A3DGFXSound* pSound = (A3DGFXSound*)pElement;
				if( pSound )
				{
					if( GFX_IsUseAudioEvent() )
					{
						AString strEvent = (AString)pElement->GetProperty(ID_GFXOP_SOUND_FILE);
						if( !strEvent.IsEmpty() )
							AddFileRecord("sfx\\" + strEvent);
					}
					else
					{
						GFX_PROPERTY prop = pElement->GetProperty(ID_GFXOP_SOUND_FILE);
						RandStringContainer* pRSC = (RandStringContainer*)prop;
						if( pRSC )
						{
							for( int i=0;i<pRSC->GetSize();i++ )
								AddFileRecord(AString("sfx\\") + pRSC->GetString(i));
						}
					}
				}
			}
			break;
		case ID_ELE_TYPE_DECAL_3D:
		case ID_ELE_TYPE_DECAL_2D:
		case ID_ELE_TYPE_DECAL_BILLBOARD:
		case ID_ELE_TYPE_TRAIL:
		case ID_ELE_TYPE_PARTICLE_POINT:
		case ID_ELE_TYPE_PARTICLE_BOX:
		case ID_ELE_TYPE_PARTICLE_MULTIPLANE:
		case ID_ELE_TYPE_PARTICLE_ELLIPSOID:
		case ID_ELE_TYPE_PARTICLE_CYLINDER:
		case ID_ELE_TYPE_PARTICLE_CURVE:
		case ID_ELE_TYPE_LIGHT:
		case ID_ELE_TYPE_RING:
		case ID_ELE_TYPE_LIGHTNING:
		case ID_ELE_TYPE_LTNBOLT:
		case ID_ELE_TYPE_LIGHTNINGEX:
		case ID_ELE_TYPE_LTNTRAIL:
		case ID_ELE_TYPE_PARABOLOID:
		case ID_ELE_TYPE_GRID_DECAL_3D:
		case ID_ELE_TYPE_GRID_DECAL_2D:
		case ID_ELE_TYPE_PHYS_EMITTER:
		case ID_ELE_TYPE_PHYS_POINTEMITTER:
		case ID_ELE_TYPE_RIBBON:
		default:
			break;
		}
	}

	return bRet;
}

// 分析A3DSkin相关资源
bool FileAnalyse::AnalyseSkinByName(const char* filename)
{
	if( !filename || !strcmp(filename, "") )
		return false;

	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	A3DSkin* pSkin = pSkinMan->LoadSkinFile(filename);
	if( !pSkin )
	{
		a_LogOutput(1, "FileAnalyse::AnalyseSkinByName, Load the skin file failed! (%s)", filename);
		return false;
	}

	bool bRet = AnalyseSkin(pSkin);
	pSkinMan->ReleaseSkin(&pSkin);
	return bRet;
}

// 分析A3DSkin相关资源
bool FileAnalyse::AnalyseSkin(A3DSkin* pSkin)
{
	if( !pSkin )
		return false;

	AddFileRecord(pSkin->GetFileName());

	for( int t=0;t<pSkin->GetTextureArray().GetSize();t++ )
	{
		A3DTexture* pTexture = pSkin->GetTexture(t);
		if( pTexture )
			AddFileRecord(pTexture->GetMapFile());
	}

	for( int n=0;n<pSkin->GetNormalMapArray().GetSize();n++ )
	{
		A3DTexture* pTexture = pSkin->GetNormalMap(n);
		if( pTexture )
			AddFileRecord(pTexture->GetMapFile());
	}

	return true;
}

// 分析A3DSkinModel相关资源
bool FileAnalyse::AnalyseSkinModel(A3DSkinModel* pModel)
{
	int i;
	bool bRet = true;
	if( !pModel ) return false;

	AddFileRecord(pModel->GetFileName());
	if( pModel->GetSkeleton() )
		AddFileRecord(pModel->GetSkeleton()->GetFileName());

	// Track dir
	if( pModel->GetTrackDataDir() )
	{
		A3DSkinModelActionCore* pAction = pModel->GetFirstAction();
		while( pAction )
		{
			AddFileRecord(pAction->GetTrackSetFileName());
			pAction = pModel->GetNextAction();
		}
	}

	// Skin List
	for( i=0;i<pModel->GetSkinNum();i++ )
	{
		A3DSkin* pSkin = pModel->GetA3DSkin(i);
		bRet = AnalyseSkin(pSkin);
	}

	// Child Model
	for( i=0;i<pModel->GetChildModelNum();i++ )
	{
		A3DSkinModel* pChildModel = pModel->GetChildModel(i);
		bRet = AnalyseSkinModel(pChildModel);
	}

	return bRet;
}

// 分析CECModel相关资源
bool FileAnalyse::AnalyseECModel(CECModel* pModel)
{
	int i;
	bool bRet = true;
	if( !pModel ) return false;

	// ecm文件
	AddFileRecord(pModel->GetFilePath());

	// SkinModel
	if( pModel->GetA3DSkinModel() )
		bRet = AnalyseSkinModel(pModel->GetA3DSkinModel());

	// Shader texture
	AddFileRecord(pModel->GetStaticData()->GetShaderTexture());

	// CoGfx
	CoGfxMap& coGfx = pModel->GetCoGfxMap();
	for( CoGfxMap::iterator it=coGfx.begin();it!=coGfx.end();++it )
	{
		bRet = AnalyseGfx(it->second->GetGfx());
	}

	// Combine actions
	for( i=0;i<pModel->GetComActCount();i++ )
	{
		if( m_ECMOption.actionExport.size() > (size_t)i &&
			!m_ECMOption.actionExport[i] )
			continue;

		A3DCombinedAction* pAction = pModel->GetComActByIndex(i);
		for( int e=0;e<pAction->GetEventCount();e++ )
		{
			EVENT_INFO* pEvent = pAction->GetEventInfo(e);
			switch(pEvent->GetType())
			{
			case EVENT_TYPE_GFX:
				{
					GFX_INFO* pGfxInfo = (GFX_INFO*)pEvent;
					if( !pGfxInfo->GetGfx() ) pGfxInfo->LoadGfx();
					bRet = AnalyseGfx(pGfxInfo->GetGfx());
				}
				break;
			case EVENT_TYPE_SFX:
				{
					SFX_INFO* pSfxInfo = (SFX_INFO*)pEvent;
					for( int i=0;i<pSfxInfo->GetFilePathCount();i++ )
					{
						if( pSfxInfo->GetFilePathByIndex(i) && strcmp(pSfxInfo->GetFilePathByIndex(i), "") )
							AddFileRecord(AString("sfx\\") + pSfxInfo->GetFilePathByIndex(i));
					}
				}
				break;
			case EVENT_TYPE_AUDIOEVENT:
				{
					AUDIOEVENT_INFO* pAudioInfo = (AUDIOEVENT_INFO*)pEvent;
					if( pAudioInfo->GetFilePath() && strcmp(pAudioInfo->GetFilePath(), "") )
						AddFileRecord(AString("sfx\\") + pAudioInfo->GetFilePath());
				}
				break;
			case EVENT_TYPE_ATT_PT:
				{
					SGCAttackPointMark* pAttackInfo = (SGCAttackPointMark*)pEvent;
					if( pAttackInfo->GetAtkFile() && strcmp(pAttackInfo->GetAtkFile(), "") )
						AddFileRecord(AString("Gfx\\SkillAttack\\") + pAttackInfo->GetAtkFile());
				}
				break;
			case EVENT_TYPE_CHLDACT:
			case EVENT_TYPE_MATCHG:
			case EVENT_TYPE_SCRIPT:
			case EVENT_TYPE_CAM_PT:
			case EVENT_TYPE_MODELSCLCHG:
			case EVENT_TYPE_MATTRANS:
				break;
			default: ASSERT(0);
			}
		}
	}

	// Child Model
	for( i=0;i<pModel->GetChildCount();i++ )
	{
		CECModel* pChildModel = pModel->GetChildModel(i);
		bRet = AnalyseECModel(pChildModel);
	}

	// Phys file name
	if( !pModel->GetStaticData()->GetPhysFileName().IsEmpty() )
	{
		char sz[MAX_PATH];
		strcpy(sz, pModel->GetFilePath());
		strcpy(ECMPathFindFileNameA(sz), pModel->GetStaticData()->GetPhysFileName());

		if( af_IsFileExist(sz) )
			AddFileRecord(sz);
	}

	// Convex hull data
	const char* szPath = strstr(pModel->GetFilePath(), "\\");
	if (szPath)
	{
		char szProjFile[MAX_PATH];
		strcpy(szProjFile, AfxGetECMHullPath());
		strcat(szProjFile, szPath);
		
		char* ext = ECMPathFindExtensionA(szProjFile);
		if (*ext!=0)
		{
			strcpy(ext, ".ecp");
			
			AFileImage file;
			if (af_IsFileExist(szProjFile) && file.Open(szProjFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
				AddFileRecord(szProjFile);
			file.Close();
		}
	}

	if( m_ECMOption.bEquipGfx )
	{
		bRet = AnalyseModelScript(pModel->GetScript(enumECMScriptChangeEquipTableInit));
	}

	AString strStck = pModel->GetFilePath();
	af_ChangeFileExt(strStck, ".stck");
	if( af_IsFileExist(strStck) ) AddFileRecord(strStck);

	return bRet;
}

// 分析CECModel相关资源
bool FileAnalyse::AnalyseECModelByName(const char* filename, bool bShowOption)
{
	if( bShowOption )
		ShowModelOption(filename);
	else
	{
		m_ECMOption.actionExport.clear();
		m_ECMOption.bEquipGfx = false;
	}

	CECModel* pModel = new CECModel();
	if( !pModel ) return false;
	if( !pModel->Load(filename) )
	{
		A3DRELEASE(pModel);
		return false;
	}

	bool bRet = AnalyseECModel(pModel);
	A3DRELEASE(pModel);
	return bRet;
}

// 分析SGC文件
bool FileAnalyse::AnalyseSGCFile(const char* szSGCFile)
{
	if( !szSGCFile || !strcmp(szSGCFile, "") )
		return false;

	AString strPath = AString("Gfx\\sgc\\") + szSGCFile;

	using namespace _SGC;
	A3DSkillGfxComposer* pComposer = new A3DSkillGfxComposer();
	if( !pComposer->Load(strPath) )
	{
		delete pComposer;
		a_LogOutput(1, "FileAnalyse::AnalyseSGC, Load the SGC file failed!");
		return false;
	}

	AddFileRecord(strPath);

	AnalyseGfxByName(pComposer->m_szFlyGfx);
	AnalyseGfxByName(pComposer->m_szHitGfx);
	AnalyseGfxByName(pComposer->m_szHitGrndGfx);

	delete pComposer;
	return true;
}

// 分析所有被缩放过的NPC模型
bool FileAnalyse::AnalyseScaledNPC()
{
	DATA_TYPE dt;
	const char* pszModelPath = NULL;
	
	unsigned int id = m_pDataMan->get_first_data_id(ID_SPACE_ESSENCE, dt);
	while(id)
	{
		switch(dt)
		{
		case DT_NPC_ESSENCE:
			{
				NPC_ESSENCE* pEssence = (NPC_ESSENCE*)m_pDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, dt);
				pszModelPath = pEssence->file_model;
			}
			break;
		case DT_MONSTER_ESSENCE:
			{
				MONSTER_ESSENCE* pEssence = (MONSTER_ESSENCE*)m_pDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, dt);
				pszModelPath = pEssence->file_model;
			}
			break;
		case DT_PET_ESSENCE:
			{
				PET_ESSENCE* pEssence = (PET_ESSENCE*)m_pDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, dt);
				pszModelPath = pEssence->file_model;
			}
			break;
		default:
			id = m_pDataMan->get_next_data_id(ID_SPACE_ESSENCE, dt);
			continue;
		}
		
		CECModel* pModel = new CECModel();
		if( !pModel->Load(pszModelPath) )
		{
			A3DRELEASE(pModel);
			id = m_pDataMan->get_next_data_id(ID_SPACE_ESSENCE, dt);
			continue;
		}

		if( m_pCBOperation && !m_pCBOperation(pszModelPath) )
		{
			A3DRELEASE(pModel);
			break;
		}

		CoGfxMap& coGfx = pModel->GetCoGfxMap();
		if ( !coGfx.empty() )
		{
			A3DSkeleton* pSkeleton = pModel->GetA3DSkinModel()->GetSkeleton();
			if (pSkeleton)
			{
				int num_bone = pSkeleton->GetRootBoneNum();
				for(int j = 0; j < num_bone; ++j)
				{
					int root_index = pSkeleton->GetRootBone(j);
					A3DBone* pBone = pSkeleton->GetBone(j);
					if (pBone)
					{
						A3DVECTOR3 vScale = pBone->GetScaleFactor();
						if( !FloatEqual(vScale.x, 1.0f) || !FloatEqual(vScale.y, 1.0f) || !FloatEqual(vScale.z, 1.0f) )
							AddFileRecord(pModel->GetFilePath());
						break;
					}	
				}
			}
		}
		
		A3DRELEASE(pModel);
		id = m_pDataMan->get_next_data_id(ID_SPACE_ESSENCE, dt);
	}

	return true;
}

// 分析有特效但没有挂到挂点上的装备
bool FileAnalyse::AnalyseEquipGfxNoHanger()
{
	DATA_TYPE dt;
	const char* pszModelPath[2];
	bool bExit = false;
	
	unsigned int id = m_pDataMan->get_first_data_id(ID_SPACE_ESSENCE, dt);
	while(id && !bExit)
	{
		switch(dt)
		{
		case DT_WEAPON_ESSENCE:
			{
				WEAPON_ESSENCE* pEssence = (WEAPON_ESSENCE*)m_pDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, dt);
				pszModelPath[0] = pEssence->file_model_left;
				pszModelPath[1] = pEssence->file_model_right;
			}
			break;
		case DT_FLYSWORD_ESSENCE:
			{
				FLYSWORD_ESSENCE* pEssence = (FLYSWORD_ESSENCE*)m_pDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, dt);
				pszModelPath[0] = pEssence->file_model;
				pszModelPath[1] = NULL;
			}
			break;
		case DT_WINGMANWING_ESSENCE:
			{
				WINGMANWING_ESSENCE* pEssence = (WINGMANWING_ESSENCE*)m_pDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, dt);
				pszModelPath[0] = pEssence->file_model;
				pszModelPath[1] = NULL;
			}
			break;
		default:
			id = m_pDataMan->get_next_data_id(ID_SPACE_ESSENCE, dt);
			continue;
		}
		
		for( int i=0;i<2;i++ )
		{
			if( !pszModelPath[i] ) continue;
			
			CECModel* pModel = new CECModel();
			if( !pModel->Load(pszModelPath[i]) )
			{
				A3DRELEASE(pModel);
				continue;
			}
			
			if( m_pCBOperation && !m_pCBOperation(pszModelPath[i]) )
			{
				A3DRELEASE(pModel);
				bExit = true;
				break;
			}
			
			CoGfxMap& coGfx = pModel->GetCoGfxMap();
			for( CoGfxMap::iterator it=coGfx.begin();it!=coGfx.end();++it )
			{
				GFX_INFO* pInfo = it->second;
				if( pInfo->GetHookName().IsEmpty() )
				{
					AddFileRecord(pModel->GetFilePath());
					break;
				}
			}
			
			A3DRELEASE(pModel);
		}
		
		id = m_pDataMan->get_next_data_id(ID_SPACE_ESSENCE, dt);
	}

	return true;
}

// 分析烟花相关资源
bool FileAnalyse::AnalyseFirework(int tid)
{
	DATA_TYPE dt;
	FIREWORKS_ESSENCE* pData = (FIREWORKS_ESSENCE*)m_pDataMan->get_data_ptr(tid, ID_SPACE_ESSENCE, dt);
	if( !pData || dt != DT_FIREWORKS_ESSENCE )
		return false;

	// 加载烟花文件
	AFileImage fileTemplate;
	if( !fileTemplate.Open(pData->file_fw, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY) )
		return false;

	FWTemplate* pTemplate = new FWTemplate;
	if( !pTemplate->LoadFrom(&fileTemplate) )
	{
		delete pTemplate;
		fileTemplate.Close();
		return false;
	}

	fileTemplate.Close();

	// 创建特效集
	FWAssemblySet* pFW = new FWAssemblySet;
	if( !pFW->CreateFromTemplate(pTemplate) )
	{
		delete pFW;
		delete pTemplate;
		return false;
	}

	// 导出各个特效
	ALISTPOSITION pos = pFW->GetHeadPosition();
	while(pos)
	{
		FWAssembly* pGFX = pFW->GetNext(pos);
		if( pGFX ) AnalyseGfx(pGFX);
	}

	// 导出FW文件
	AddFileRecord(pData->file_fw);

	delete pFW;
	delete pTemplate;
	return true;
}

// 导出文件列表
void FileAnalyse::Export(const char* szDestDir)
{
	if( !szDestDir || !_stricmp(szDestDir, g_szWorkDir) )
	{
		SHOW_ERRMSG(ERR_INVALIDDESTDIR);
		return;
	}

	for( size_t i=0;i<m_Filelist.size();i++ )
	{
		if( m_pCBOperation && !m_pCBOperation(m_Filelist[i]) )
			break;

		ExportFile(m_Filelist[i], szDestDir);
	}
}

// 导出指定文件
void FileAnalyse::ExportFile(const char* szFile, const char* szDestDir)
{
	char szNewFile[MAX_PATH];
	_snprintf(szNewFile, MAX_PATH, "%s\\%s", szDestDir, szFile);

	// 创建目标目录
	AString newDestDir;
	af_GetFilePath(szNewFile, newDestDir);
	glb_CreateDirectory(newDestDir);

	if( _access(szFile, 0) == -1 )
	{
		AFileImage file;
		if( file.Open("", szFile, AFILE_OPENEXIST|AFILE_BINARY|AFILE_TEMPMEMORY) )
		{
			DWORD dwReadLen;
			DWORD dwLen = file.GetFileLength();
			BYTE* pBuffer = (BYTE*)a_malloctemp(dwLen ? dwLen : 1);
			file.ResetPointer();
			file.Read(pBuffer, dwLen, &dwReadLen);
			file.Close();

			if( dwReadLen == dwLen )
			{
				FILE* pFile = fopen(szNewFile, "wb");
				if( pFile )
				{
					fwrite(pBuffer, dwLen, 1, pFile);
					fclose(pFile);
				}
			}
			else
			{
				ASSERT(0);
				a_LogOutput(1, "FileAnalyse::ExportFile, Read file data from package error! (%s)", szFile);
			}

			a_freetemp(pBuffer);
		}
	}
	else
	{
		CopyFileA(szFile, szNewFile, FALSE);
	}
}

// 添加文件记录
bool FileAnalyse::AddFileRecord(const char* filename)
{
	if( !filename || !stricmp(filename, "") )
		return false;

	// 如果是绝对路径则转换为相对路径
	AString strRelative;
	if( strstr(filename, ":") )
		af_GetRelativePath(filename, strRelative);
	else
		strRelative = filename;

	AString lowStr = strRelative;
	lowStr.MakeLower();

	if( m_FileDup.find(lowStr) == m_FileDup.end() )
	{
		m_Filelist.push_back(strRelative);
		m_FileDup.insert(lowStr);
		return true;
	}

	return false;
}

// 清空结果
void FileAnalyse::Clear()
{
	m_Filelist.clear();
	m_FileDup.clear();
}