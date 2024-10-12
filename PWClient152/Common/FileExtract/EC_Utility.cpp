/*
 * FILE: EC_Utility.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/6/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "StdAfx.h"
#include "resource.h"
#include "EC_Utility.h"
#include "FileAnalyse.h"
#include "ModelOptionDlg.h"
#include "elementdataman.h"

#include "AC.h"
#include "AFI.h"
#include "A3DMacros.h"
#include "A3DLight.h"
#include "A3DFuncs.h"
#include "A3DCamera.h"
#include "A3DDevice.h"
#include "A3DSkinModel.h"
#include "A3DSkin.h"
#include "A3DSkillGfxEvent.h"
#include "A3DGfxExMan.h"
#include "AMSoundEngine.h"
#include "AMSoundBufferMan.h"
#include "AMSoundBuffer.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


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

A3DVECTOR3	g_vOrigin(0.0f);
A3DVECTOR3	g_vAxisX(1.0f, 0.0f, 0.0f);
A3DVECTOR3	g_vAxisY(0.0f, 1.0f, 0.0f);
A3DVECTOR3	g_vAxisZ(0.0f, 0.0f, 1.0f);

static const char* _equipment_skin[] = 
{
	"Models\\Players\\装备\\男\\%s\\男通用%s",
	"Models\\Players\\装备\\女\\%s\\女通用%s",
	"Models\\Players\\装备\\男\\%s\\男通用%s",
	"Models\\Players\\装备\\女\\%s\\女通用%s",
	
	"Models\\Players\\装备\\男\\%s\\男通用%s",
	"Models\\Players\\装备\\女\\%s\\女通用%s",
	"",
	"Models\\Players\\装备\\女\\%s\\妖精%s",
	"Models\\Players\\装备\\男\\%s\\妖兽%s",
	"",
	
	"Models\\Players\\装备\\男\\%s\\男通用%s",
	"Models\\Players\\装备\\女\\%s\\女通用%s",
	
	"Models\\Players\\装备\\男\\%s\\男通用%s",
	"Models\\Players\\装备\\女\\%s\\女通用%s",
	"Models\\Players\\装备\\男\\%s\\男通用%s",
	"Models\\Players\\装备\\女\\%s\\女通用%s",
	
	"Models\\Players\\装备\\男\\%s\\男通用%s",
	"Models\\Players\\装备\\女\\%s\\女通用%s",
	"Models\\Players\\装备\\男\\%s\\男通用%s",
	"Models\\Players\\装备\\女\\%s\\女通用%s",
};


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

//	Create a directory
bool glb_CreateDirectory(const char* szDir)
{
	AString strDir = szDir;
	int iLen = strDir.GetLength();

	if (iLen <= 3)
		return true;

	//	Clear the last '\\'
	if (strDir[iLen-1] == '\\')
		strDir[iLen-1] = '\0';

	//	Save current directory
	char szCurDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szCurDir);

	AString strTemp;

	int iPos = strDir.Find('\\', 0);
	while (iPos > 0)
	{
		strTemp = strDir.Mid(0, iPos);
		CreateDirectoryA(strTemp, NULL);
		iPos = strDir.Find('\\', iPos+1);
	}

	CreateDirectoryA(szDir, NULL);

	//	Restore current directory
	SetCurrentDirectoryA(szCurDir);

	return true;
}

/*	Check whether specified file exist.

	Return true specified file exist, otherwise return false.

	szFile: file's full name
*/
bool glb_FileExist(const char* szFile)
{
	if (!szFile || !szFile[0])
		return false;

	FILE* fp = fopen(szFile, "rb");
	if (!fp)
		return false;

	fclose(fp);
	return true;
}

/*	Change file's extension

	szFile (in, out): file name.
	szNewExt: new extension of file
*/
void glb_ChangeExtension(char* szFile, char* szNewExt)
{
#ifdef _DEBUG
	
	int iLen = strlen(szFile);
	if (iLen <= 4)
	{
		ASSERT(iLen > 4);
		return;
	}

#endif

	char* pTemp = strrchr(szFile, '.');
	if (!pTemp)
	{
		strcat(szFile, ".");
		strcat(szFile, szNewExt);
	}
	else
	{
		strcpy(pTemp+1, szNewExt);
	}
}

void glb_ClearExtension(char* szFile)
{
	char* pTemp = strrchr(szFile, '.');
	if (pTemp)
		*pTemp = '\0';
}

bool ShowModelOption(const char* szModelPath)
{
	CModelOptionDlg dlg;
	dlg.m_szModelPath = szModelPath;
	return (dlg.DoModal() == IDOK);
}

// 生成装备的Skin路径
void GenSkinPath(const char* szEquipName, abase::vector<AString>& filepaths, int sex)
{
	static const char* suffix[] = {"一级", "二级", "三级"};

	filepaths.clear();
	for( int i=0;i<sizeof(_equipment_skin)/sizeof(const char*);i++ )
	{
		if( sex == 1 && (i & 1) ) continue;
		if( sex == 2 && !(i & 1) ) continue;
		if( !strcmp(_equipment_skin[i], "") ) continue;

		AString strPath;
		strPath.Format(_equipment_skin[i], szEquipName, szEquipName);
		for( int n=0;n<3;n++ )
		{
			AString strRealPath = strPath + suffix[n] + ".ski";
			filepaths.push_back(strRealPath);
		}
	}
}

// 获取当前所有NPC的数目
int GetNumAllNPCs()
{
	int iNum = 0;
	DATA_TYPE dt;
	elementdataman* pDataMan = FileAnalyse::GetInstance().GetDataMan();
	int id = pDataMan->get_first_data_id(ID_SPACE_ESSENCE, dt);
	while(id)
	{
		if( dt == DT_NPC_ESSENCE || dt == DT_MONSTER_ESSENCE || dt == DT_PET_ESSENCE )
			iNum++;
		id = pDataMan->get_next_data_id(ID_SPACE_ESSENCE, dt);
	}
	return iNum;
}

// 获取所有武器、飞剑、翅膀的个数
int GetNumAllEquips()
{
	int iNum = 0;
	DATA_TYPE dt;
	elementdataman* pDataMan = FileAnalyse::GetInstance().GetDataMan();
	int id = pDataMan->get_first_data_id(ID_SPACE_ESSENCE, dt);
	while(id)
	{
		if( dt == DT_WEAPON_ESSENCE )
		{
			WEAPON_ESSENCE* pWeapon = (WEAPON_ESSENCE*)pDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, dt);
			if( strcmp(pWeapon->file_model_left, "") ) iNum++;
			if( strcmp(pWeapon->file_model_right, "") ) iNum++;
		}
		else if( dt == DT_FLYSWORD_ESSENCE || dt == DT_WINGMANWING_ESSENCE )
			iNum++;
		id = pDataMan->get_next_data_id(ID_SPACE_ESSENCE, dt);
	}
	return iNum;
}

// 浮点相等判断
bool FloatEqual(float f1, float f2)
{
	const float EPSILON = 0.000001f;
	return fabs(f1 - f2) < EPSILON;
}


///////////////////////////////////////////////////////////////////////////

//	Below funcions are used by GFX lib

A3DDevice* AfxGetA3DDevice()
{
	return FileAnalyse::GetInstance().GetA3DDevice();
}

float AfxGetAverageFrameRate()
{
	return 30.0f;
}

A3DGFXExMan* AfxGetGFXExMan()
{
	return FileAnalyse::GetInstance().GetGFXExMan();
}

CECModelMan* AfxGetECModelMan()
{
	return FileAnalyse::GetInstance().GetModelMan();
}

A3DSkinModel* AfxLoadA3DSkinModel(const char* szFile, int iSkinFlag)
{
	A3DSkinModel* pModel = new A3DSkinModel;
	if (!pModel)
		return false;
	
	if (!pModel->Init(FileAnalyse::GetInstance().GetA3DEngine()))
	{
		delete pModel;
		return false;
	}
	
	if (!pModel->Load(szFile, iSkinFlag))
	{
		delete pModel;
		return false;
	}

	return pModel;
}

void AfxReleaseA3DSkinModel(A3DSkinModel* pModel)
{
	A3DRELEASE(pModel);
}

const A3DLIGHTPARAM& AfxGetLightparam()
{
	static A3DLIGHTPARAM param;
	memset(&param, 0, sizeof(A3DLIGHTPARAM));
	return param;
}

float AfxGetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm)
{
	if( pNorm )
		pNorm->Set(0.0f, 1.0f, 0.0f);
	return 0.0f;
}

bool AfxRayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel)
{
	return false;
}

A3DCamera* AfxGetA3DCamera(void)
{
	return FileAnalyse::GetInstance().GetA3DCamera();
}

bool AfxGetModelUpdateFlag(void)
{
	return false;
}

void AfxBeginShakeCam()
{
}

void AfxEndShakeCam()
{
}

void AfxSetCamOffset(const A3DVECTOR3& vOffset)
{
}

AM3DSoundBuffer* AfxLoadNonLoopSound(const char* szFile, int nPriority)
{
	AMSoundBufferMan* pMan = FileAnalyse::GetInstance().GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();
	AM3DSoundBuffer* pBuffer = pMan->Load3DSound(szFile, false);
	return pBuffer;
}

AM3DSoundBuffer* AfxLoadLoopSound(const char* szFile)
{
	AMSoundBufferMan* pMan = FileAnalyse::GetInstance().GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();
	AM3DSoundBuffer* pBuffer = pMan->Load3DSound(szFile, false);
	return pBuffer;
}

void AfxReleaseSoundNonLoop(AM3DSoundBuffer*& pSound)
{
	FileAnalyse::GetInstance().GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan()->Release3DSound(&pSound);
}

void AfxReleaseSoundLoop(AM3DSoundBuffer*& pSound)
{
	FileAnalyse::GetInstance().GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan()->Release3DSound(&pSound);
}

void AfxECModelAddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, A3DSkinModel* pA3DSkinModel)
{
}

void AfxSetA3DSkinModelSceneLightInfo(A3DSkinModel* pA3DSkinModel)
{
}

bool AfxPlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets)
{
	return true;
}

void AfxSkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier)
{
}

void AfxSkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier)
{
}

bool AfxSkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage)
{
	return true;
}

const char* AfxGetECMHullPath()
{
	return "grasses\\ECModelHull";
}

bool gGfxGetSurfaceData(const A3DVECTOR3& vCenter, float fRadus, A3DVECTOR3* pVerts, int& nVertCount, WORD* pIndices, int& nIndexCount)
{
	return false;
}

namespace _SGC
{
	A3DSkillGfxMan* AfxGetSkillGfxEventMan()
	{
		return NULL;
	}
}
