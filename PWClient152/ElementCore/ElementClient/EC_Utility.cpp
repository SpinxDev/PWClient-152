/*
 * FILE: EC_Utility.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#define _WIN32_WINDOWS 0x0410 

#include "EC_Global.h"
#include "EC_Utility.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_CDS.h"
#include "EC_Viewport.h"
#include "EC_SoundCache.h"
#include "EC_GPDataType.h"
#include "EC_Configs.h"
#include "EC_InputCtrl.h"
#include "EC_ShadowRender.h"
#include "EC_SceneLights.h"
#include "EC_Resource.h"
#include "EC_UIManager.h"
#include "EC_WinAPI.h"

#include "A3DGfxExMan.h"
#include "A3DMacros.h"
#include "A3DLight.h"
#include "A3DFuncs.h"
#include "AC.h"
#include "A3DMathUtility.h"
#include "A3DCamera.h"
#include "AMiniDump.h"
#include "A3DDevice.h"
#include "A3DSkinModel.h"
#include "A3DSkin.h"
#include "A3DShader.h"
#include "A3DShaderMan.h"
#include "AFI.h"
#include <AUILuaManager.h>


//	#include <time.h>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define	MAX_MOVECLIP_PLANES		10

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////

bool g_bExceptionOccured = false;
DWORD g_dwFatalErrorFlag = 0;

///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

/*	Clip velocity along specified plane.

	vInVel: original velocity.
	vNormal: plane's normal
	vOutVel: result velocity.
	fOverBounce: over bounce.
*/
static void _ClipVelocity(const A3DVECTOR3& vInVel, const A3DVECTOR3& vNormal, A3DVECTOR3& vOutVel, float fOverBounce)
{
	float fBackOff = DotProduct(vInVel, vNormal);
	
	if (fBackOff < 0.0f)
		fBackOff *= fOverBounce;
	else
		fBackOff = 0.0f;// /= fOverBounce;
	
	vOutVel = vInVel - vNormal * fBackOff;

	if (Magnitude(vOutVel) > 1000)
		ASSERT(0);
}

//	Format the specified FILETIME to output in a human readable format,
//	without using the C run time.
static void _FormatTime(LPTSTR output, FILETIME TimeToPrint)
{
	output[0] = _AL('\0');
	WORD Date, Time;
	if (FileTimeToLocalFileTime(&TimeToPrint, &TimeToPrint) &&
		FileTimeToDosDateTime(&TimeToPrint, &Date, &Time))
	{
		wsprintf(output, _AL("%d-%d-%d-%02d-%02d-%02d"),
				(Date / 512) + 1980, (Date / 32) & 15, Date & 31,
				(Time >> 11), (Time >> 5) & 0x3F, (Time & 0x1F) * 2);
	}
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement
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

bool glb_WriteString(FILE* fp, const char* str)
{
	ASSERT(str);

	//	Write length of string
	int iLen = strlen(str);
	fwrite(&iLen, 1, sizeof (int), fp);

	//	Write string data
	if (iLen)
		fwrite(str, 1, iLen, fp);

	return true;
}

ACString glb_GetExeDirectory()
{
	//	获取当前 exe 所有目录全路径，末尾带 \ 字符
	ACString strDir;

	TCHAR szModulePath[MAX_PATH];
	if (GetModuleFileName(NULL, szModulePath, MAX_PATH))
	{
		strDir = szModulePath;
		int nSlashPos = strDir.ReverseFind(_AL('\\'));
		if (nSlashPos < 0)
			nSlashPos = strDir.ReverseFind(_AL('/'));
		if (nSlashPos >= 0)
			strDir.CutRight(strDir.GetLength() - nSlashPos - 1);
	}

	return strDir;
}

void glb_GetExeDirectory(char *buffer, int bufferSize){
	AString strExeDir = AC2AS(glb_GetExeDirectory());
	strncpy(buffer, strExeDir, a_Min(bufferSize, strExeDir.GetLength()));
}

bool glb_PercentProbability(int iPercent)
{
	int iRand = a_Random() % 100;
	return iRand < iPercent ? true : false;
}

bool glb_PercentProbability(float fPercent)
{
	return a_Random(0.0f, 100.0f) < fPercent;
}

A3DVECTOR3 glb_RandomVectorH(int* piDegree/* NULL */)
{
	ASSERT(g_pA3DMathUtility);

	int iDegree = a_Random(0, 359);

	if (piDegree)
		*piDegree = iDegree;

	return A3DVECTOR3(g_pA3DMathUtility->COS((float)iDegree), 0.0f, g_pA3DMathUtility->SIN((float)iDegree));
}

A3DVECTOR3 glb_RandomVector()
{
	ASSERT(g_pA3DMathUtility);

	A3DVECTOR3 vVec;
	float fScale;

	int iDegree = a_Random(0, 179);
	vVec.y = g_pA3DMathUtility->SIN((float)iDegree);
	fScale = g_pA3DMathUtility->COS((float)iDegree);

	iDegree = a_Random(0, 359);
	vVec.x = g_pA3DMathUtility->COS((float)iDegree) * fScale;
	vVec.z = g_pA3DMathUtility->SIN((float)iDegree) * fScale;

	return vVec;
}

//	Compress horizontal direction to a byte
BYTE glb_CompressDirH(float x, float z)
{
	static const float fInvInter = 256.0f / 360.0f;

	if (fabs(x) < 0.00001)
	{
		if (z > 0.0f)
			return 64;
		else
			return 192;
	}
	else
	{
		float fDeg = RAD2DEG((float)atan2(z, x));
		return (BYTE)(fDeg * fInvInter);
	}
}

//	Decompress horizontal direction
A3DVECTOR3 glb_DecompressDirH(BYTE byDir)
{
	static const float fInter = 360.0f / 256.0f;

	float fRad = DEG2RAD(byDir * fInter);
	A3DVECTOR3 v;
	v.x = (float)cos(fRad);
	v.z = (float)sin(fRad);
	v.y = 0.0f;

	return v;
}

//	Integer compare function used by qsort
int glb_IntCompare(const void* arg1, const void* arg2)
{
	int i1 = *(int*)arg1;
	int i2 = *(int*)arg2;
	
	if (i1 < i2)
		return -1;
	else if (i1 > i2)
		return 1;
	else
		return 0;
}

int glb_WordCompare(const void* arg1, const void* arg2)
{
	WORD i1 = *(WORD*)arg1;
	WORD i2 = *(WORD*)arg2;
	
	if (i1 < i2)
		return -1;
	else if (i1 > i2)
		return 1;
	else
		return 0;
}

int glb_IconStateCompare(const void* arg1, const void* arg2)
{
	using S2C::IconState;
	IconState *s1 = (IconState *)arg1;
	IconState *s2 = (IconState *)arg2;

	if (s1->id < s2->id)
		return -1;
	else if (s1->id > s2->id)
		return 1;
	else
		return 0;
}

unsigned long glb_HandleException(LPEXCEPTION_POINTERS pExceptionPointers)
{
	a_LogOutput(1, "glb_HandleException is called.");

	if( IsDebuggerPresent() )
		return EXCEPTION_CONTINUE_SEARCH;

	/*
	//	Use current time to build file name
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	TCHAR szTimeBuffer[100];
	_FormatTime(szTimeBuffer, CurrentTime);
	
	TCHAR szFile[MAX_PATH];

#ifdef UNICODE
	wsprintf(szFile, _AL("%S\\Logs\\ec_%s.dmp"), g_szWorkDir, szTimeBuffer);
#else
	wsprintf(szFile, _AL("%s\\Logs\\ec_%s.dmp"), g_szWorkDir, szTimeBuffer);
#endif
	*/

	TCHAR szFile[MAX_PATH];
	extern DWORD GAME_BUILD;
	wsprintf(szFile, _AL("%S\\Logs\\ec_build%d.dmp"), g_szWorkDir, GAME_BUILD);

	if( GetVersion() < 0x80000000 )
	{
		// WinNT or Win2000
		AMiniDump::Create(NULL, pExceptionPointers, szFile, &g_csException);
	}
	else
	{
		// Win95 or Win98 or Win32		
	}

	// if dump success, we should launch bug report program here
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	CreateProcess(L"ReportBugs\\CReportBugs.exe", NULL, NULL, NULL, FALSE, 0, NULL, L"ReportBugs", &si, &pi);

	g_bExceptionOccured = true;

	return EXCEPTION_EXECUTE_HANDLER;
}

//	Get format local time
tm* glb_GetFormatLocalTime()
{
	time_t t;
	time(&t);
	return localtime(&t);
}

bool ECModelRenderForShadow(A3DViewport * pViewport, void * pArg)
{
	A3DSkinModel * pModel = (A3DSkinModel *) pArg;
	g_pGame->GetA3DDevice()->SetZTestEnable(true);
	g_pGame->GetA3DDevice()->SetZWriteEnable(true);
	g_pGame->GetA3DDevice()->SetAlphaTestEnable(true);
	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(false);
	g_pGame->GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATEREQUAL);
	g_pGame->GetA3DDevice()->SetAlphaRef(84);

	pModel->RenderAtOnce(pViewport, A3DSkinModel::RAO_NOTEXTURE | A3DSkinModel::RAO_NOMATERIAL, false);

	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(true);
	g_pGame->GetA3DDevice()->SetAlphaTestEnable(false);
	g_pGame->GetA3DDevice()->SetZTestEnable(true);
	g_pGame->GetA3DDevice()->SetZWriteEnable(true);
	return true;
}

//	Below funcions are used by GFX lib

A3DDevice* AfxGetA3DDevice()
{
	return g_pGame->GetA3DDevice();
}

float AfxGetAverageFrameRate()
{
	return g_pGame->GetAverageFrameRate();
}

A3DGFXExMan* AfxGetGFXExMan()
{
	return g_pGame->GetA3DGFXExMan();
}

CECModelMan* AfxGetECModelMan()
{
	return g_pGame->GetECModelMan();
}

A3DSkinModel* AfxLoadA3DSkinModel(const char* szFile, int iSkinFlag)
{
	return g_pGame->LoadA3DSkinModel(szFile, iSkinFlag);
}

void AfxReleaseA3DSkinModel(A3DSkinModel* pModel)
{
	g_pGame->ReleaseA3DSkinModel(pModel);
}

const A3DLIGHTPARAM& AfxGetLightparam()
{
	return g_pGame->GetDirLight()->GetLightparam();
}

float AfxGetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm)
{
	if (g_pGame->GetGameRun()->GetWorld())
		return g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(vPos, pNorm);

	//	退出游戏时，会先删除 World，然后调用GFX的Tick，进而会调用此函数
	if (pNorm)
		pNorm->Set(0.0f, 1.0f, 0.0f);
	return 0.0f;
}

bool AfxRayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel)
{
	CECCDS* pCDS = g_pGame->GetGameRun()->GetWorld()->GetCDS();
	RAYTRACERT TraceRt;

	return pCDS->RayTrace(vStart, vVel, 1.0f, &TraceRt, TRACEOBJ_LBTNCLICK, 0);
}

A3DVECTOR3 _cam_shake_offset(0);
bool _shaking_cam = false;
static int _shaking_count = 0;

A3DCamera * AfxGetA3DCamera(void)
{
	return g_pGame->GetViewport()->GetA3DCamera();
}

bool AfxGetModelUpdateFlag(void)
{
	return false;//g_pGame->GetConfigs()->GetModelUpdateFlag();
}

void AfxBeginShakeCam()
{
	_shaking_count++;
	_shaking_cam = true;
}

void AfxEndShakeCam()
{
	if (--_shaking_count == 0)
	{
		_shaking_cam = false;
		_cam_shake_offset.Clear();
	}
}

void AfxSetCamOffset(const A3DVECTOR3& vOffset)
{
	_cam_shake_offset += vOffset;
}

AM3DSoundBuffer* AfxLoadNonLoopSound(const char* szFile, int nPriority)
{
	return g_pGame->GetSoundCache()->LoadSound(szFile, enumSoundLoadNonLoop);
}

AM3DSoundBuffer* AfxLoadLoopSound(const char* szFile)
{
	return g_pGame->GetSoundCache()->LoadSound(szFile, enumSoundLoadLoop);
}

void AfxReleaseSoundNonLoop(AM3DSoundBuffer*& pSound)
{
	g_pGame->GetSoundCache()->ReleaseSound(pSound, enumSoundLoadNonLoop);
}

void AfxReleaseSoundLoop(AM3DSoundBuffer*& pSound)
{
	g_pGame->GetSoundCache()->ReleaseSound(pSound, enumSoundLoadLoop);
}

void AfxECModelAddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, A3DSkinModel* pA3DSkinModel)
{
	if (g_pGame->GetShadowRender())
		g_pGame->GetShadowRender()->AddShadower(aabb.Center, aabb, SHADOW_RECEIVER_TERRAIN, ECModelRenderForShadow, pA3DSkinModel);
}

void AfxSetA3DSkinModelSceneLightInfo(A3DSkinModel* pA3DSkinModel)
{
	CECSceneLights* pSceneLights = g_pGame->GetGameRun()->GetWorld()->GetSceneLights();
	A3DSkinModel::LIGHTINFO LightInfo = pA3DSkinModel->GetLightInfo();
	LightInfo.colAmbient	= g_pGame->GetA3DDevice()->GetAmbientValue();
	LightInfo.vLightDir		= g_pGame->GetDirLight()->GetLightparam().Direction;
	LightInfo.colDirDiff	= g_pGame->GetDirLight()->GetLightparam().Diffuse;
	LightInfo.colDirSpec	= g_pGame->GetDirLight()->GetLightparam().Specular;

	if( pSceneLights )
	{
		CECSceneLights::SCENELIGHT theLight;
		if( pSceneLights->GetSceneLight(pA3DSkinModel->GetAbsoluteTM().GetRow(3), theLight) )
		{
			LightInfo.bPtLight = true;
			LightInfo.colPtDiff = theLight.diffuse;
			LightInfo.colPtAmb =  theLight.ambient;
			LightInfo.fPtRange = theLight.vRange;
			LightInfo.vPtAtten = A3DVECTOR3(theLight.vAttenuation0, theLight.vAttenuation1, theLight.vAttenuation2);
			LightInfo.vPtLightPos = theLight.vecPos;
		}
		else
			LightInfo.bPtLight = false;
	}

	pA3DSkinModel->SetLightInfo(LightInfo);
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

//	LuaDlgApi 调用函数
AUILuaManager* GetAuiManager()
{
	AUILuaManager* ret = NULL;
	if (g_pGame	&& g_pGame->GetGameRun())
	{
		CECUIManager *pUIMan = g_pGame->GetGameRun()->GetUIManager();
		if (pUIMan)
			ret = (AUILuaManager *)pUIMan->GetBaseUIMan();
	}
	return ret;
}

//	Build pvp mask
BYTE glb_BuildPVPMask(bool bForceAttack)
{
	BYTE byMask = 0;
	if (bForceAttack)
		byMask |= GP_PVPMASK_FORCE;
	else
	{
		CECConfigs* pConfigs = g_pGame->GetConfigs();

		if (pConfigs->GetGameSettings().bAtk_Player)
		{
			byMask |= GP_PVPMASK_FORCE;

			if (pConfigs->GetGameSettings().bAtk_NoMafia)
				byMask |= GP_PVPMASK_NOMAFIA;
			
			if (pConfigs->GetGameSettings().bAtk_NoWhite)
				byMask |= GP_PVPMASK_NOWHITE;

			if (pConfigs->GetGameSettings().bAtk_NoAlliance)
				byMask |= GP_PVPMASK_NOALLIANCE;
			
			if(pConfigs->GetGameSettings().bAtk_NoForce)
				byMask |= GP_PVPMASK_NOFORCE;
		}
	}
	
	return byMask;
}
//  Build bless mask
BYTE glb_BuildBLSMask()
{
	BYTE byMask = 0;
	CECConfigs* pConfigs = g_pGame->GetConfigs();

	if(pConfigs->GetGameSettings().bBls_NoRed)
		byMask |= GP_BLSMASK_NORED;

	if(pConfigs->GetGameSettings().bBls_NoMafia)
		byMask |= GP_BLSMASK_NOMAFIA;

	if(pConfigs->GetGameSettings().bBls_Self)
		byMask |= GP_BLSMASK_SELF;

	if(pConfigs->GetGameSettings().bBls_NoAlliance)
		byMask |= GP_BLSMASK_NOALLIANCE;

	if(pConfigs->GetGameSettings().bBls_NoForce)
		byMask |= GP_BLSMASK_NOFORCE;

	return byMask;
}

BYTE glb_BuildRefuseBLSMask()
{
	BYTE byMask = 0;

	const EC_GAME_SETTING &gs = g_pGame->GetConfigs()->GetGameSettings();

	if (gs.bBlsRefuse_Neutral)
		byMask |= REFUSE_NEUTRAL_BLESS;
	if (gs.bBlsRefuse_NonTeammate)
		byMask |= REFUSE_NON_TEAMMATE_BLESS;

	return byMask;
}

//	Get force attack flag
bool glb_GetForceAttackFlag(const DWORD* pdwParam)
{
	bool bForceAttack = false;
	CECInputCtrl* pInputCtrl = g_pGame->GetGameRun()->GetInputCtrl();
	
	if (pdwParam)
		bForceAttack = pInputCtrl->IsCtrlPressed(*pdwParam);
	else
		bForceAttack = pInputCtrl->KeyIsBeingPressed(VK_CONTROL);

	return bForceAttack;
}

void glb_ArmorReplaceShader(A3DSkin * pSkin)
{
	if( !pSkin )
		return;

	int i;
	for(i=0; i < pSkin->GetTextureNum(); i++)
	{
		A3DTexture* pTex = pSkin->GetTexture(i);
		const char* szTexMap = pTex->GetMapFile();
		bool bAlphaTex = pTex->IsAlphaTexture();

		if (szTexMap && szTexMap[0])
		{
			char szFileTitle[MAX_PATH];
			af_GetFileTitle(szTexMap, szFileTitle, MAX_PATH);
			strlwr(szFileTitle);
			if( strstr(szFileTitle, "rw_") != szFileTitle )
			{
				A3DShader* pReplaceShader = g_pGame->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile("armor.sdr");
				if (pReplaceShader)
				{
					pReplaceShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szTexMap);
					pSkin->ChangeSkinTexturePtr(i, pReplaceShader);
					pReplaceShader->SetOnlyActiveStagesFlag(true);
				}
			}
			else
			{
				A3DShader* pReplaceShader = g_pGame->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile("rewu.sdr");
				if (pReplaceShader)
				{
					pReplaceShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szTexMap);
					
					if( pReplaceShader->GetGeneralProps().pPShader )
					{
						char szTexMap2[MAX_PATH];
						af_GetFilePath(szTexMap, szTexMap2, MAX_PATH);
						strcat(szTexMap2, "\\nb_");
						strcat(szTexMap2, szFileTitle);
						pReplaceShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szTexMap2);
					}
					else
					{
						pReplaceShader->ShowStage(2, false);
					}

					pSkin->ChangeSkinTexturePtr(i, pReplaceShader);
					pReplaceShader->SetOnlyActiveStagesFlag(true);
					pReplaceShader->SetAlphaTextureFlag(bAlphaTex);
				}
			}
		}
	}
}

void glb_BoothReplaceShader(A3DSkin * pSkin)
{
	if( !pSkin )
		return;
	
	int i;
	for(i=0; i < pSkin->GetTextureNum(); i++)
	{
		A3DTexture* pTex = pSkin->GetTexture(i);
		const char* szTexMap = pTex->GetMapFile();
		bool bAlphaTex = pTex->IsAlphaTexture();
		
		if (szTexMap && szTexMap[0])
		{
			char szFileTitle[MAX_PATH];
			af_GetFileTitle(szTexMap, szFileTitle, MAX_PATH);
			strlwr(szFileTitle);
			if( strstr(szFileTitle, "rw_") == szFileTitle )
			{
				A3DShader* pReplaceShader = g_pGame->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile("rewu.sdr");
				if (pReplaceShader)
				{
					pReplaceShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szTexMap);
					
					if( pReplaceShader->GetGeneralProps().pPShader )
					{
						char szTexMap2[MAX_PATH];
						af_GetFilePath(szTexMap, szTexMap2, MAX_PATH);
						strcat(szTexMap2, "\\nb_");
						strcat(szTexMap2, szFileTitle);
						pReplaceShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szTexMap2);
					}
					else
					{
						pReplaceShader->ShowStage(2, false);
					}
					
					pSkin->ChangeSkinTexturePtr(i, pReplaceShader);
					pReplaceShader->SetOnlyActiveStagesFlag(true);
					pReplaceShader->SetAlphaTextureFlag(bAlphaTex);
				}
			}
		}
	}
}

void glb_ArmorReplaceShader_ReflectPrefix(A3DSkin * pSkin)
{
	if( !pSkin )
		return;

	int i;
	for(i=0; i < pSkin->GetTextureNum(); i++)
	{
		A3DTexture* pTex = pSkin->GetTexture(i);
		const char* szTexMap = pTex->GetMapFile();
		bool bAlphaTex = pTex->IsAlphaTexture();

		if (szTexMap && szTexMap[0])
		{
			char szFileTitle[MAX_PATH];
			af_GetFileTitle(szTexMap, szFileTitle, MAX_PATH);
			strlwr(szFileTitle);
			if( strstr(szFileTitle, "rw_") != szFileTitle )
			{
				if( strstr(szFileTitle, "r_") != szFileTitle )
					continue;

				A3DShader* pReplaceShader = g_pGame->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile("armor.sdr");
				if (pReplaceShader)
				{
					pReplaceShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szTexMap);
					pSkin->ChangeSkinTexturePtr(i, pReplaceShader);
					pReplaceShader->SetOnlyActiveStagesFlag(true);
					pReplaceShader->SetAlphaTextureFlag(true);
				}
			}
			else
			{
				A3DShader* pReplaceShader = g_pGame->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile("rewu.sdr");
				if (pReplaceShader)
				{
					pReplaceShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szTexMap);
					
					if( pReplaceShader->GetGeneralProps().pPShader )
					{
						char szTexMap2[MAX_PATH];
						af_GetFilePath(szTexMap, szTexMap2, MAX_PATH);
						strcat(szTexMap2, "\\nb_");
						strcat(szTexMap2, szFileTitle);
						pReplaceShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szTexMap2);
					}
					else
					{
						pReplaceShader->ShowStage(2, false);
					}

					pSkin->ChangeSkinTexturePtr(i, pReplaceShader);
					pReplaceShader->SetOnlyActiveStagesFlag(true);
					pReplaceShader->SetAlphaTextureFlag(bAlphaTex);
				}
			}
		}
	}
}

void glb_FashionReplaceShader(A3DSkin * pSkin)
{
	if( !pSkin )
		return;

	int i;
	for(i=0; i < pSkin->GetTextureNum(); i++)
	{
		A3DTexture* pTex = pSkin->GetTexture(i);
		const char* szTexMap = pTex->GetMapFile();
		bool bAlphaTex = pTex->IsAlphaTexture();

		if (szTexMap && szTexMap[0])
		{
			char szFileTitle[MAX_PATH];
			af_GetFileTitle(szTexMap, szFileTitle, MAX_PATH);
			strlwr(szFileTitle);
			if( strstr(szFileTitle, "rw_") != szFileTitle )
			{
				A3DShader* pReplaceShader = g_pGame->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile("fashion.sdr");
				if (pReplaceShader)
				{
					pReplaceShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szTexMap);
					pSkin->ChangeSkinTexturePtr(i, pReplaceShader);
					pReplaceShader->SetOnlyActiveStagesFlag(true);
				}
			}
			else
			{
				A3DShader* pReplaceShader = g_pGame->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile("rewu.sdr");
				if (pReplaceShader)
				{
					pReplaceShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szTexMap);
					
					if( pReplaceShader->GetGeneralProps().pPShader )
					{
						char szTexMap2[MAX_PATH];
						af_GetFilePath(szTexMap, szTexMap2, MAX_PATH);
						strcat(szTexMap2, "\\nb_");
						strcat(szTexMap2, szFileTitle);
						pReplaceShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szTexMap2);
					}
					else
					{
						pReplaceShader->ShowStage(2, false);
					}

					pSkin->ChangeSkinTexturePtr(i, pReplaceShader);
					pReplaceShader->SetOnlyActiveStagesFlag(true);
					pReplaceShader->SetAlphaTextureFlag(bAlphaTex);
				}
			}
		}
	}
}

A3DShader* glb_LoadBodyShader(A3DSkin* pSkin, const char* szTexMap)
{
	if (!pSkin)
		return NULL;

	A3DShader* pBodyShader = NULL;

	if (szTexMap && szTexMap[0])
	{
		char szFileTitle[MAX_PATH];
		af_GetFileTitle(szTexMap, szFileTitle, MAX_PATH);
		strlwr(szFileTitle);
		if( strstr(szFileTitle, "rw_") != szFileTitle )
		{
			pBodyShader = g_pGame->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile(res_ShaderFile(RES_SHD_BODY));
			if (pBodyShader)
				pBodyShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szTexMap);
		}
		else
		{
			pBodyShader = g_pGame->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile("rewu.sdr");
			if (pBodyShader)
			{
				pBodyShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szTexMap);
				
				if( pBodyShader->GetGeneralProps().pPShader )
				{
					char szTexMap2[MAX_PATH];
					af_GetFilePath(szTexMap, szTexMap2, MAX_PATH);
					strcat(szTexMap2, "\\nb_");
					strcat(szTexMap2, szFileTitle);
					pBodyShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szTexMap2);
				}
				else
				{
					pBodyShader->ShowStage(2, false);
				}
			}
		}
	}

	return pBodyShader;
}

ACString A3DCOLOR_TO_STRING(A3DCOLOR clr)
{
	// 把 A3DCOLOR 颜色值转换为 UI 显示需要的 ^ffffff 格式
	//

	ACHAR pszColor[8] = {'^', '0', '0', '0', '0', '0', '0', '\0'};
	BYTE argb[3] = 
	{
		A3DCOLOR_GETRED(clr),
		A3DCOLOR_GETGREEN(clr),
		A3DCOLOR_GETBLUE(clr)
	};
	BYTE temp;
	ACHAR *p=pszColor+1;
	for (int i=0; i<3; ++i)
	{
		temp = argb[i]>>4;
		if (temp>9)
			*p = 'a'+(temp-10);
		else
			*p = '0'+temp;

		p++;

		temp = argb[i]%16;
		if (temp>9)
			*p = 'a'+(temp-10);
		else
			*p = '0'+temp;

		p++;
	}
	return pszColor;
}

bool STRING_TO_A3DCOLOR(ACString str, A3DCOLOR &clr)
{
	// 完成 A3DCOLOR_TO_STRING 的逆向转换
	// 

	bool ret(false);

	BYTE rgb[6];

	int nLen = str.GetLength();
	while (nLen==7)
	{
		// 把字母都转换成小写
		str.MakeLower();

		if (str[0] != '^')
			break;		
		
		ACHAR c;
		int i;
		for (i=1; i<nLen; ++i)
		{
			c = str[i];
			if (c>='0' && c<='9')
			{
				rgb[i-1]=c-'0';
			}
			else if (c>='a' && c<='f')
			{
				rgb[i-1]=10+(c-'a');
			}
			else
			{
				break;
			}
		}
		if (i<nLen)
			break;

		BYTE r = (rgb[0]<<4)+rgb[1];
		BYTE g = (rgb[2]<<4)+rgb[3];
		BYTE b = (rgb[4]<<4)+rgb[5];
		clr = A3DCOLORRGB(r, g, b);

		ret = true;
		break;
	}

	return ret;
}

bool glb_IsInMapForMiniClient()
{
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
	return pWorld ? pWorld->GetInstanceID() == 162 : false;
}

void glb_ExploreFile(const char *szFullPath){
	if (!szFullPath || !szFullPath[0]){
		ASSERT(false);
		return;
	}
	if (!af_IsFileExist(szFullPath)){
		a_LogOutput(1, "glb_ExploreFile(%s), does not exist.", szFullPath);
		return;
	}
	if (g_pILCreateFromPathA && g_pILFree){
		if (LPITEMIDLIST pIL = g_pILCreateFromPathA(szFullPath)){
			HRESULT hResult = SHOpenFolderAndSelectItems(pIL, 0, 0, 0);
			if (!SUCCEEDED(hResult)){
				a_LogOutput(1, "glb_ExploreFile(%s), SHOpenFolderAndSelectItems return %u", hResult);
			}
			g_pILFree(pIL);
		}
	}else{
		AString strParameter = AString("/select,\"")+szFullPath+AString("\"");
		SHELLEXECUTEINFOA si = {0};
		si.cbSize = sizeof(si);
		si.lpVerb = "open";
		si.lpFile = "explorer.exe";
		si.lpParameters = strParameter;
		si.nShow = SW_SHOWNORMAL;				
		if (FALSE == ::ShellExecuteExA(&si)){
			a_LogOutput(1, "glb_ExploreFile(%s) Failed, GetLastError()=%u", GetLastError());
		}
	}
}