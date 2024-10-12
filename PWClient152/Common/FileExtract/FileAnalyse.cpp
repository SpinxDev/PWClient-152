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
#define ALL_CONTENT "ȫ��"

#define SHOW_ERRMSG(str) ::MessageBoxA(NULL, str, "����", MB_OK|MB_ICONERROR)

const char* ERR_OPENPCKFAIL = "��pck�ļ�ʧ�ܣ��뽫������ڿͻ���Ŀ¼�£�";
const char* ERR_OPENDATAFAIL = "��elements.dataʧ�ܣ�";
const char* ERR_INVALIDDESTDIR = "�ͻ���Ŀ¼���������ļ�����ѡ������Ŀ¼��";
const char* ERR_INVALIDNPCID = "�ⲻ��һ���Ϸ���NPCģ��ID��";
const char* ERR_INITA3DFAIL = "��ʼ��A3D����ʧ�ܣ�";
const char* ERR_INVALIDUIFILE = "��Ч�Ľ����ļ�������ʧ�ܣ�";
const char* ERR_INVALIDSKILL = "��Ч�ļ���ID����������ʧ�ܣ�";
const char* ERR_OPENAIFAIL = "���ز�������ʧ�ܣ�";
const char* ERR_INVALIDEQUIP = "��Ч��װ��ID������װ��ʧ�ܣ�";

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
	"����", "��ʦ", "��ʦ", "����", "����", "�̿�", "��â", "����", "����", "����", "ҹӰ", "����"
};

static const char* l_strPlayerModel[] =
{
	"Models\\Players\\����\\������\\����\\������.ecm", "Models\\Players\\����\\����Ů\\����\\����Ů.ecm",
	"Models\\Players\\����\\��ʦ��\\����\\��ʦ��.ecm", "Models\\Players\\����\\��ʦŮ\\����\\��ʦŮ.ecm",
	"Models\\Players\\����\\��ʦ��\\����\\��ʦ��.ecm", "Models\\Players\\����\\��ʦŮ\\����\\��ʦŮ.ecm",
	"", "Models\\Players\\����\\����\\����\\����.ecm",
	"Models\\Players\\����\\������\\����\\������.ecm", "",
	"Models\\Players\\����\\�̿���\\����\\�̿���.ecm", "Models\\Players\\����\\�̿�Ů\\����\\�̿�Ů.ecm",
	"Models\\Players\\����\\��â��\\����\\��â��.ecm", "Models\\Players\\����\\��âŮ\\����\\��âŮ.ecm",
	"Models\\Players\\����\\������\\����\\������.ecm", "Models\\Players\\����\\����Ů\\����\\����Ů.ecm",
	"Models\\Players\\����\\������\\����\\������.ecm", "Models\\Players\\����\\����Ů\\����\\����Ů.ecm",
	"Models\\Players\\����\\������\\����\\������.ecm", "Models\\Players\\����\\����Ů\\����\\����Ů.ecm",
	"Models\\Players\\����\\ҹӰ��\\����\\ҹӰ��.ecm", "Models\\Players\\����\\ҹӰŮ\\����\\ҹӰŮ.ecm",
	"Models\\Players\\����\\������\\����\\������.ecm", "Models\\Players\\����\\����Ů\\����\\����Ů.ecm",
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

// ��ȡʵ��
FileAnalyse& FileAnalyse::GetInstance()
{
	static FileAnalyse obj;
	return obj;
}

// ���캯��
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

// ��������
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

// ��ʼ���ļ�������
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

	// ����־
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

	// ����elements.data
	m_pDataMan = new elementdataman();
	if( !m_pDataMan || m_pDataMan->load_data("data\\elements.data") == -1 )
	{
		SHOW_ERRMSG(ERR_OPENDATAFAIL);
		a_LogOutput(1, "FileAnalyse::Init, Open the elements.data failed!");
		return false;
	}

	// ��ʼ��A3DEngine
	if( !InitA3DEngine() )
	{
		SHOW_ERRMSG(ERR_INITA3DFAIL);
		a_LogOutput(1, "FileAnalyse::Init, Init the A3DEngine failed!");
		return false;
	}

	AfxInitFWEditor(m_pA3DDevice);

	// ��ʼ��װ����Ч��
	if( !LoadEquipGfx() )
	{
		a_LogOutput(1, "FileAnalyse::Init, Load the equipment gfx failed!");
		return false;
	}

	// ���ز�������
	m_pPolicies = new CPolicyDataManager();
	if( !m_pPolicies || !m_pPolicies->Load("data\\aipolicy.data") )
	{
		SHOW_ERRMSG(ERR_OPENAIFAIL);
		a_LogOutput(1, "FileAnalyse::Init, Load the ai policy data failed!");
		return false;
	}

	// ��ʼ�����Թ�ϣ��
	for( i=0;i<m_pPolicies->GetPolicyNum();i++ )
	{
		CPolicyData* pPolicy = m_pPolicies->GetPolicy(i);
		m_PolicyMap[pPolicy->GetID()] = pPolicy;
	}

	return true;
}

// ��ʼ��A3DEngine
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

	// ��ʼ��Luaģ��
	g_LuaStateMan.Init();
	InitECMApi(g_LuaStateMan.GetConfigState());

	//  Create the model manager
	m_pModelMan = new CECModelMan();
	m_pModelMan->Init();

	return true;
}

// ��ʼ�����װ����Ч��
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

			// ��ȡequip_id
			int equip_id = 0, gfx_idx;
			sscanf(sf.m_szToken, "self.equip_id_table[%d][%d]", &equip_id, &gfx_idx);

			// ��gfx_path
			while(sf.GetNextToken(true))
			{
				if( !strcmp(sf.m_szToken, "gfx_path") )
				{
					sf.GetNextToken(true);	// ����=��
					sf.GetNextToken(true);	// ��Ч�ļ�·��

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

// ����ָ��NPC��UI����
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

// ����NPCģ����ص��ļ�
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

	// ecm�ļ�
	bRet = AnalyseECModelByName(strModelFile);

	// ski�ļ�
	char szSkin[MAX_PATH];
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();

	// Skin lod1
	strncpy(szSkin, strModelFile, MAX_PATH);
	glb_ChangeExtension(szSkin, "ski");
	A3DSkin* pSkin1 = pSkinMan->LoadSkinFile(szSkin);
	bRet = AnalyseSkin(pSkin1);

	// Skin lod2
	glb_ClearExtension(szSkin);
	strcat(szSkin, "����.ski");
	A3DSkin* pSkin2 = pSkinMan->LoadSkinFile(szSkin);
	if( pSkin2 ) bRet = AnalyseSkin(pSkin2);

	pSkinMan->ReleaseSkin(&pSkin1);
	pSkinMan->ReleaseSkin(&pSkin2);

	// ����������ص���Դ
	if( dt == DT_MONSTER_ESSENCE )
	{
		int i;
		MONSTER_ESSENCE* pMonster = (MONSTER_ESSENCE*)pData;

		// ����Ч��
		bRet = AnalyseGfxByName(pMonster->file_gfx_short);
		bRet = AnalyseGfxByName(pMonster->file_gfx_short_hit);

		if( pMonster->id_skill ) bRet = AnalyseSkill(pMonster->id_skill);

		// ��������
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
		// ��ͨ��������
		for( i=0;i<STRUCT_SIZE(pMonster->skills);i++ )
		{
			if( pMonster->skills[i].id_skill )
				bRet = AnalyseSkill(pMonster->skills[i].id_skill);
		}

		// ������ؼ���
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
		// ���＼�ܿ���ѧ�����Բ�ȷ�����ﶼ����Щ���ܣ�����Ͳ�������
		PET_ESSENCE* pPet = (PET_ESSENCE*)pData;

		// ���﹥��Ч��
		bRet = AnalyseGfxByName(pPet->file_gfx_short);
	}

	return bRet;
}

// �������ְҵ��ص�ģ������
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

// �������ʱװ��Ч�ű�
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
			sf.GetNextToken(true);		// ����=��
			sf.GetNextToken(true);		// ��Ч���ļ�·��

			bRet = AnalyseGfxByName(sf.m_szToken);
		}
	}

	sf.Close();
	file.Detach();
	return true;
}

// ����������ص���Դ
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

// ����BUFF��ص���Դ
bool FileAnalyse::AnalyseBuff(int buff_id)
{
	using namespace GNET;

	if( buff_id == -1 )
	{
		bool bRet = false;

		// ĿǰBuff ID���ֵ��1000����ңԶ����ʱ�������
		for( int i=0;i<1000;i++ )
		{
			const VisibleState* pState = VisibleState::Query(0, i);
			if( pState && pState->GetEffect() && strcmp(pState->GetEffect(), "") )
			{
				AString strPath = AString("�߻�����\\״̬Ч��\\") + pState->GetEffect();
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

		return AnalyseGfxByName(AString("�߻�����\\״̬Ч��\\") + pState->GetEffect());
	}
}

// ����װ����ص���Դ
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

	// ��ȡװ����Ӧ����Ч
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

// ����DCF�ļ�
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

// ����UI��ص��ļ�
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

// �������Դ�����
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

// ����GFX��ص��ļ�
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

// ����GFX��ص��ļ�
bool FileAnalyse::AnalyseGfx(A3DGFXEx* pGFX)
{
	bool bRet = true;
	if( !pGFX ) return false;

	// gfx�ļ�����
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

				// ��ȡA3DSkinModel�����Դ
				bRet = AnalyseSkinModel(pModel->GetSkinModel());
			}
			break;
		case ID_ELE_TYPE_GFX_CONTAINER:
			{
				A3DGFXContainer* pContainer = (A3DGFXContainer*)pElement;

				// ��ȡGfx����Դ
				bRet = AnalyseGfxByName(pContainer->GetGfxPath());
			}
			break;
		case ID_ELE_TYPE_ECMODEL:
			{
				A3DGFXECModel* pModel = (A3DGFXECModel*)pElement;

				// ��ȡCECModel�����Դ
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

// ����A3DSkin�����Դ
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

// ����A3DSkin�����Դ
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

// ����A3DSkinModel�����Դ
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

// ����CECModel�����Դ
bool FileAnalyse::AnalyseECModel(CECModel* pModel)
{
	int i;
	bool bRet = true;
	if( !pModel ) return false;

	// ecm�ļ�
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

// ����CECModel�����Դ
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

// ����SGC�ļ�
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

// �������б����Ź���NPCģ��
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

// ��������Ч��û�йҵ��ҵ��ϵ�װ��
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

// �����̻������Դ
bool FileAnalyse::AnalyseFirework(int tid)
{
	DATA_TYPE dt;
	FIREWORKS_ESSENCE* pData = (FIREWORKS_ESSENCE*)m_pDataMan->get_data_ptr(tid, ID_SPACE_ESSENCE, dt);
	if( !pData || dt != DT_FIREWORKS_ESSENCE )
		return false;

	// �����̻��ļ�
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

	// ������Ч��
	FWAssemblySet* pFW = new FWAssemblySet;
	if( !pFW->CreateFromTemplate(pTemplate) )
	{
		delete pFW;
		delete pTemplate;
		return false;
	}

	// ����������Ч
	ALISTPOSITION pos = pFW->GetHeadPosition();
	while(pos)
	{
		FWAssembly* pGFX = pFW->GetNext(pos);
		if( pGFX ) AnalyseGfx(pGFX);
	}

	// ����FW�ļ�
	AddFileRecord(pData->file_fw);

	delete pFW;
	delete pTemplate;
	return true;
}

// �����ļ��б�
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

// ����ָ���ļ�
void FileAnalyse::ExportFile(const char* szFile, const char* szDestDir)
{
	char szNewFile[MAX_PATH];
	_snprintf(szNewFile, MAX_PATH, "%s\\%s", szDestDir, szFile);

	// ����Ŀ��Ŀ¼
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

// ����ļ���¼
bool FileAnalyse::AddFileRecord(const char* filename)
{
	if( !filename || !stricmp(filename, "") )
		return false;

	// ����Ǿ���·����ת��Ϊ���·��
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

// ��ս��
void FileAnalyse::Clear()
{
	m_Filelist.clear();
	m_FileDup.clear();
}