#include "StdAfx.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DCDS.h"
#include "A3DFuncs.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"
#include "A3DConfig.h"
#include "A3DGFXExMan.h"
#include "A3DLight.h"
#include "A3DGFXElement.h"
#include "A3DDecalEx.h"
#include "A3DParaboloid.h"
#include "A3DTrail.h"
#include "A3DLtnTrail.h"
#include "A3DParticleSystemEx.h"
#include "A3DGFXLight.h"
#include "A3DGFXRing.h"
#include "A3DGFXLightning.h"
#include "A3DGFXLightningEx.h"
#include "A3DLtnBolt.h"
#include "A3DGFXModel.h"
#include "A3DGFXContainer.h"
#include "A3DGFXSound.h"
#include "A3DGFXExMan.h"
#include "A3DGridDecalEx.h"
#include "A3DPhysElement.h"
#include "A3DGFXECModel.h"
#include "A3DRibbon.h"
#include "A3DGFXModelProxy.h"
#include "A3DGFXPIFuncs.h"
#include "A3DTrailEx.h"
#include "A3DEnvironment.h"

static const char* _gfx_load_format		= "GFXELEMENTID: %d";
static const char* _format_name			= "Name: %s";
static const char* _format_srcbld		= "SrcBlend: %d";
static const char* _format_destbld		= "DestBlend: %d";
static const char* _format_repeatcount	= "RepeatCount: %d";
static const char* _format_repeatdelay	= "RepeatDelay: %d";
static const char* _format_texpath		= "TexFile: %s";
static const char* _format_texrow		= "TexRow: %d";
static const char* _format_texcol		= "TexCol: %d";
static const char* _format_texint		= "TexInterval: %d";
static const char* _format_bind			= "BindEle: %s";
static const char* _format_zenable		= "ZEnable: %d";
static const char* _format_matchgrnd	= "MatchGrnd: %d";
static const char* _ground_height		= "GroundHeight: %d";
static const char* _format_priority		= "Priority: %d";
static const char* _format_is_dummy		= "IsDummy: %d";
static const char* _format_dummy_ele	= "DummyEle: %s";
static const char* _format_warp			= "Warp: %d";
static const char* _format_tile_mode	= "TileMode: %d";
static const char* _format_tile_speed	= "TexSpeed: %f";
static const char* _format_u_reverse	= "UReverse: %d";
static const char* _format_v_reverse	= "VReverse: %d";
static const char* _format_uv_exchg		= "UVExchg: %d";
static const char* _format_layer		= "RenderLayer: %d";
static const char* _format_nodownsample	= "NoDownSample: %d";
static const char* _format_resetloopend = "ResetLoopEnd: %d";
static const char* _format_texanimmaxtime = "TexAnimMaxTime: %d";
static const char* _format_ps_version	= "PSFileVersion: %d";
static const char* _format_shaderpath	= "ShaderFile: %s";
static const char* _format_shadertex	= "ShaderTex: %s";
static const char* _format_psconstcount	= "PSConstCount: %d";
static const char* _format_psindex		= "PSConstIndex: %d";
static const char* _format_psvalue		= "PSConstValue: %f, %f, %f, %f";
static const char* _format_pstargetcount= "PSTargetCount: %d";
static const char* _format_psinterval	= "PSInterval: %d";
static const char* _format_psloop		= "PSLoopCount: %d";
static const char* _format_candofadeout	= "CanDoFadeOut: %d";
static const char* _format_softedge 	= "SoftEdge: %d";
static const char* _format_useinnerlight= "UseInnerLight: %d";

static const char* _format_sfx_ver		= "SoundVer: %d";
static const char* _format_sfx_2d		= "Force2D: %d";
static const char* _format_sfx_loop		= "IsLoop: %d";
static const char* _format_sfx_volume	= "Volume: %d";
static const char* _format_sfx_min_dist	= "MinDist: %f";
static const char* _format_sfx_max_dist	= "MaxDist: %f";
static const char* _format_sfx_volumemin= "VolMin: %d";
static const char* _format_sfx_volumemax= "VolMax: %d";
static const char* _format_sfx_absolutevolume = "AbsoluteVolume: %d";
static const char* _format_sfx_pitchmin	= "PitchMin: %f";
static const char* _format_sfx_pitchmax	= "PitchMax: %f";
static const char* _format_sfx_fixspeed = "FixSpeed: %d";
static const char* _format_sfx_silentheader = "SilentHeader: %d";
static const char* _format_sfx_percentage = "PercentStart: %f";
static const char* _format_audiogroup		= "Group: %d";

#define GFX_PIXEL_SHADER_VERSION		1

void g_GfxApplyPixelShaderConsts(A3DDevice* pA3DDevice, const GfxPSConstVec& ConstsVec, DWORD dwPSTickTime, int nIndexOffset)
{
	DWORD dwTickTime;
	size_t i, j;
	const size_t dwPSConstsCount = ConstsVec.size();

	for (i = 0; i < dwPSConstsCount; i++)
	{
		dwTickTime = 0;
		const GfxPixelShaderConst& c = ConstsVec[i];
		const size_t sz = c.target_vals.size();

		if (sz)
		{
			DWORD dwCurTotalTime;

			if (c.total_time != 0) // 没有无限时间的target
			{
				int cur_loop = dwPSTickTime / c.total_time;

				if (c.loop_count < 0 || cur_loop < c.loop_count) // 可以无限循环，或循环没超时
					dwCurTotalTime = dwPSTickTime % c.total_time;
				else // 循环超时
				{
					pA3DDevice->SetPixelShaderConstants(c.index + nIndexOffset, (const void*)&c.target_vals[sz-1].value, 1);
					continue;
				}
			}
			else // 有无限时间元素
				dwCurTotalTime = dwPSTickTime;

			for (j = 0; j < sz ; j++)
			{
				const GfxPixelShaderTargetValue& t = c.target_vals[j];
				float fInterval;

				if (t.interval == 0)
				{
					const A3DCOLORVALUE& c0 = (j == 0 ? c.init_val : c.target_vals[j-1].value);
					A3DCOLORVALUE v = c0 + t.value * (dwCurTotalTime * .001f);
					pA3DDevice->SetPixelShaderConstants(c.index + nIndexOffset, (const void*)&v, 1);
					break;
				}
				else if (int(t.interval) < 0)
				{
					fInterval = float(-int(t.interval));
					dwTickTime -= t.interval;
				}
				else
				{
					fInterval = (float)t.interval;
					dwTickTime += t.interval;
				}

				if (dwCurTotalTime > dwTickTime)
					continue;

				if (int(t.interval) < 0)
				{
					const A3DCOLORVALUE& c0 = (j == 0 ? c.init_val : c.target_vals[j-1].value);
					pA3DDevice->SetPixelShaderConstants(c.index + nIndexOffset, (const void*)&c0, 1);
				}
				else
				{
					DWORD dwRemain = dwTickTime - dwCurTotalTime;
					float co = dwRemain / fInterval;
					float rco = 1.0f - co;
					const A3DCOLORVALUE& c0 = (j == 0 ? c.init_val : c.target_vals[j-1].value);
					A3DCOLORVALUE v = c0 * co + t.value * rco;
					pA3DDevice->SetPixelShaderConstants(c.index + nIndexOffset, (const void*)&v, 1);
				}

				break;
			}

			if (j == sz)
				pA3DDevice->SetPixelShaderConstants(c.index + nIndexOffset, (const void*)&c.target_vals[sz-1].value, 1);
		}
		else
			pA3DDevice->SetPixelShaderConstants(c.index + nIndexOffset, (const void*)&c.init_val, 1);
	}
}

void g_GfxLoadPixelShaderConsts(AFile* pFile, AString& strPixelShader, AString& strShaderTex, GfxPSConstVec& vecPSConsts)
{
	char szLine[AFILE_LINEMAXLEN];
	char szBuf[AFILE_LINEMAXLEN];
	szBuf[0] = '\0';
	DWORD dwReadLen;
	DWORD dwVersion = 0;

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_ps_version, &dwVersion);

	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_shaderpath, szBuf);
	strPixelShader = szBuf;

	szBuf[0] = '\0';
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_shadertex, szBuf);
	strShaderTex = szBuf;

	int i, j;
	int nPSConstCount = 0;
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
	sscanf(szLine, _format_psconstcount, &nPSConstCount);
	vecPSConsts.reserve(nPSConstCount);

	int nTargetCount;

	for (i = 0; i < nPSConstCount; i++)
	{
		GfxPixelShaderConst psconst;
		A3DCOLORVALUE& value = psconst.init_val;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_psindex, &psconst.index);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_psvalue, &value.r, &value.g, &value.b, &value.a);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_psloop, &psconst.loop_count);

		nTargetCount = 0;
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_pstargetcount, &nTargetCount);
		psconst.target_vals.reserve(nTargetCount);

		for (j = 0; j < nTargetCount; j++)
		{
			GfxPixelShaderTargetValue tv;
			A3DCOLORVALUE& value = tv.value;

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_psinterval, &tv.interval);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_psvalue, &value.r, &value.g, &value.b, &value.a);

			psconst.target_vals.push_back(tv);
		}

		psconst.CalcTotalTime();
		vecPSConsts.push_back(psconst);
	}
}

void g_GfxSavePixelShaderConsts(AFile* pFile, const AString& strPixelShader, const AString& strShaderTex, const GfxPSConstVec& vecPSConsts)
{
	char szLine[AFILE_LINEMAXLEN];

	sprintf(szLine, _format_ps_version, GFX_PIXEL_SHADER_VERSION);
	pFile->WriteLine(szLine);

	sprintf(szLine, _format_shaderpath, strPixelShader);
	pFile->WriteLine(szLine);

	sprintf(szLine, _format_shadertex, strShaderTex);
	pFile->WriteLine(szLine);

	int i, j;
	const int nPSConstCount = vecPSConsts.size();
	sprintf(szLine, _format_psconstcount, nPSConstCount);
	pFile->WriteLine(szLine);

	for (i = 0; i < nPSConstCount; i++)
	{
		const GfxPixelShaderConst& psconst = vecPSConsts[i];
		const A3DCOLORVALUE& value = psconst.init_val;

		sprintf(szLine, _format_psindex, psconst.index);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_psvalue, value.r, value.g, value.b, value.a);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_psloop, psconst.loop_count);
		pFile->WriteLine(szLine);

		const int nTargetCount = psconst.target_vals.size();
		sprintf(szLine, _format_pstargetcount, nTargetCount);
		pFile->WriteLine(szLine);

		for (j = 0; j < nTargetCount; j++)
		{
			const GfxPixelShaderTargetValue& tv = psconst.target_vals[j];
			const A3DCOLORVALUE& value = tv.value;

			sprintf(szLine, _format_psinterval, tv.interval);
			pFile->WriteLine(szLine);

			sprintf(szLine, _format_psvalue, value.r, value.g, value.b, value.a);
			pFile->WriteLine(szLine);
		}
	}
}

/*
 * Version History
 * V2 10.5.10
 * Remark: add volume min / max, add pitch min / max, remove old volume
 * V3 11.12.26
 * Remark: add bFixSpeed
 * V4 12.2.13
 * Remark: add bAbsoluteVolume
 * V5 12.2.24
 * Remark: add silentHeader
 * V6 12.4.20
 * Remark: add sfx start percentage
 * V7 12.6.20
 * Remark: add sfx group
 */

#define GFX_SOUND_VERSION 7

//////////////////////////////////////////////////////////////////////////
//
//	Implement GfxSoundParamInfo
//
//////////////////////////////////////////////////////////////////////////

bool GfxSoundParamInfo::LoadSoundParamInfo(AFile* pFile)
{
	DWORD dwReadLen;
	int nVer = 0, nRead;

	if (pFile->IsBinary())
	{
		pFile->Read(&nVer, sizeof(nVer), &dwReadLen);
		ASSERT(nVer != 0);
		pFile->Read(&m_bForce2D, sizeof(m_bForce2D), &dwReadLen);
		pFile->Read(&m_bLoop, sizeof(m_bLoop), &dwReadLen);
//		pFile->Read(&m_dwVolume, sizeof(m_dwVolume), &dwReadLen);
		pFile->Read(&m_fMinDist, sizeof(m_fMinDist), &dwReadLen);
		pFile->Read(&m_fMaxDist, sizeof(m_fMaxDist), &dwReadLen);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_sfx_ver, &nVer);

		if (nVer != 0) 
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);

		sscanf(szLine, _format_sfx_2d, &nRead);
		m_bForce2D = (nRead != 0);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_sfx_loop, &nRead);
		m_bLoop = (nRead != 0);

		if (nVer <= 1)
		{
			DWORD dwVolume = 100;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_sfx_volume, &dwVolume);
			m_dwVolumeMin = m_dwVolumeMax = dwVolume;
		}
		else // >= 2
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_sfx_volumemin, &m_dwVolumeMin);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_sfx_volumemax, &m_dwVolumeMax);
		}

		if (nVer >= 4)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_sfx_absolutevolume, &nRead);
			m_bAbsoluteVolume = nRead != 0;
		}

		if (nVer >= 2)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_sfx_pitchmin, &m_fPitchMin);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_sfx_pitchmax, &m_fPitchMax);
		}

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_sfx_min_dist, &m_fMinDist);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_sfx_max_dist, &m_fMaxDist);

		if (nVer >= 3)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_sfx_fixspeed, &nRead);
			m_bFixSpeed = nRead != 0;
		}

		if (nVer >= 5)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_sfx_silentheader, &m_iSilentHeader);
		}

		if (nVer >= 6)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_sfx_percentage, &m_fPertentStart);
		}

		if (nVer >= 7)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_audiogroup, &m_nGroup);
		}
	}

	return true;
}

bool GfxSoundParamInfo::SaveSoundParamInfo(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
	
		int nVer = GFX_SOUND_VERSION;
		pFile->Write(&nVer, sizeof(nVer), &dwWrite);
		pFile->Write(&m_bForce2D, sizeof(m_bForce2D), &dwWrite);
		pFile->Write(&m_bLoop, sizeof(m_bLoop), &dwWrite);
//		pFile->Write(&m_dwVolume, sizeof(m_dwVolume), &dwWrite);
		pFile->Write(&m_fMinDist, sizeof(m_fMinDist), &dwWrite);
		pFile->Write(&m_fMaxDist, sizeof(m_fMaxDist), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_sfx_ver, GFX_SOUND_VERSION);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_sfx_2d, (int)m_bForce2D);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_sfx_loop, (int)m_bLoop);
		pFile->WriteLine(szLine);

		// removed in GFX_SOUND_VERSION 2
// 		sprintf(szLine, _format_sfx_volume, m_dwVolume);
// 		pFile->WriteLine(szLine);

		// add in GFX_SOUND_VERSION 2
		sprintf(szLine, _format_sfx_volumemin, m_dwVolumeMin);
		pFile->WriteLine(szLine);

		// add in GFX_SOUND_VERSION 2
		sprintf(szLine, _format_sfx_volumemax, m_dwVolumeMax);
		pFile->WriteLine(szLine);

		// add in GFX_SOUND_VERSION 4
		sprintf(szLine, _format_sfx_absolutevolume, (int)m_bAbsoluteVolume);
		pFile->WriteLine(szLine);

		// add in GFX_SOUND_VERSION 2
		sprintf(szLine, _format_sfx_pitchmin, m_fPitchMin);
		pFile->WriteLine(szLine);

		// add in GFX_SOUND_VERSION 2
		sprintf(szLine, _format_sfx_pitchmax, m_fPitchMax);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_sfx_min_dist, m_fMinDist);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_sfx_max_dist, m_fMaxDist);
		pFile->WriteLine(szLine);

		// add in GFX_SOUND_VERSION 3
		sprintf(szLine, _format_sfx_fixspeed, m_bFixSpeed);
		pFile->WriteLine(szLine);

		// add in GFX_SOUND_VERSION 5
		sprintf(szLine, _format_sfx_silentheader, m_iSilentHeader);
		pFile->WriteLine(szLine);

		// add in GFX_SOUND_VERSION 6
		sprintf(szLine, _format_sfx_percentage, m_fPertentStart);
		pFile->WriteLine(szLine);

		// add in GFX_SOUND_VERSION 7
		sprintf(szLine, _format_audiogroup, m_nGroup);
		pFile->WriteLine(szLine);
	}

	return true;
}

void GfxSoundParamInfo::Clone(const GfxSoundParamInfo* pSrc)
{
	if (pSrc == this)
		return;

	*this = *pSrc;
}

GfxSoundParamInfo& GfxSoundParamInfo::operator = (const GfxSoundParamInfo& src)
{
	if (&src == this)
		return *this;
	
	m_bForce2D	= src.m_bForce2D;
	m_bLoop		= src.m_bLoop;
	//m_dwVolume	= src.m_dwVolume;
	m_dwVolumeMin = src.m_dwVolumeMin;
	m_dwVolumeMax = src.m_dwVolumeMax;
	m_bAbsoluteVolume = src.m_bAbsoluteVolume;
	m_fPitchMin	= src.m_fPitchMin;
	m_fPitchMax = src.m_fPitchMax;
	m_fMinDist	= src.m_fMinDist;
	m_fMaxDist	= src.m_fMaxDist;
	m_bFixSpeed = src.m_bFixSpeed;
	m_iSilentHeader = src.m_iSilentHeader;
	m_fPertentStart = src.m_fPertentStart;
	return *this;
}

void GfxSoundParamInfo::SetVolume(DWORD dwVolumeMin, DWORD dwVolumeMax) 
{ 
	if (dwVolumeMax < dwVolumeMin)
		a_Swap(dwVolumeMin, dwVolumeMax);

	a_Clamp(dwVolumeMin, 0ul, 100ul);
	a_Clamp(dwVolumeMax, 0ul, 100ul);
	m_dwVolumeMin = dwVolumeMin; 
	m_dwVolumeMax = dwVolumeMax; 
}

void GfxSoundParamInfo::SetSoundPitch(float fPitchMin, float fPitchMax) 
{ 
	if (fPitchMax < fPitchMin)
		a_Swap(fPitchMin, fPitchMax);

	a_Clamp(fPitchMin, -12.f, 12.f);
	a_Clamp(fPitchMax, -12.f, 12.f);

	m_fPitchMin = fPitchMin;
	m_fPitchMax = fPitchMax;
}

DWORD GfxSoundParamInfo::GetRandVolume() const
{
	if (m_dwVolumeMin == m_dwVolumeMax)
		return m_dwVolumeMin;

	return a_Random((int)m_dwVolumeMin, (int)m_dwVolumeMax);
}

float GfxSoundParamInfo::GetRandPitch() const
{
	if (m_fPitchMin == m_fPitchMax)
		return m_fPitchMin;

	return a_Random(m_fPitchMin, m_fPitchMax);
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement GFXEleHLSLInfo
//
//////////////////////////////////////////////////////////////////////////

static const int GFXELEHLSLINFO_VERSION = 1;
static const char* format_gfxEleHLSLInfo_tagAndVersion = "gfxEleHLSLInfo version: %d";

bool GFXEleHLSLInfo::Save(AFile *file) const
{
	char line[AFILE_LINEMAXLEN];
	sprintf(line, format_gfxEleHLSLInfo_tagAndVersion, GFXELEHLSLINFO_VERSION);
	file->WriteLine(line);

	return m_pHLPixelShader->Save(file) && m_animationTrack.Save(file);
}
bool GFXEleHLSLInfo::Load(AFile *file)
{
	char line[AFILE_LINEMAXLEN];
	DWORD readLength;
	file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
	int version;
	if (sscanf(line, format_gfxEleHLSLInfo_tagAndVersion, &version) != 1 || version != GFXELEHLSLINFO_VERSION)
	{
		a_LogOutput(1, "GFXEleHLSLInfo::Load, tag or Version");
		return false;
	}

	return m_pHLPixelShader->Load(file) && m_animationTrack.Load(file);
}

//////////////////////////////////////////////////////////////////////////
//
//	Implement A3DGFXElement
//
//////////////////////////////////////////////////////////////////////////

A3DGFXElement::A3DGFXElement(A3DGFXEx* pGfx)
{
	m_pGfx				= pGfx;
	m_KeyPointSet.SetEle(this);
	m_nEleType			= ID_ELE_TYPE_NONE;
	m_pDevice			= NULL;
	m_Shader.SrcBlend	= A3DBLEND_SRCALPHA;
	m_Shader.DestBlend	= A3DBLEND_ONE;
	m_bVisible			= true;
	m_pTexture			= NULL;
	m_pBind				= NULL;
	m_pDummy			= NULL;
	m_bTotalBind		= false;
	m_bZEnable			= true;
	m_bGroundNormal		= false;
	m_bGroundHeight		= false;
	m_bNeedCalcTex		= false;
	m_dwTexTimeRemain	= 0;
	m_dwTexInterval		= 50;
	m_bTexNoDownSample	= false;
	m_bTileMode			= false;
	m_fUOffsetSpeed		= 0;
	m_fVOffsetSpeed		= 0;
	m_nTexRow			= 1;
	m_nTexCol			= 1;
	m_nCurTexRow		= 0;
	m_nCurTexCol		= 0;
	m_fTexU				= 0;
	m_fTexV				= 0;
	m_fTexWid			= 1.f;
	m_fTexHei			= 1.f;
	m_bUVInterchange	= false;
	m_bUReverse			= false;
	m_bVReverse			= false;
	m_nPriority			= 4;
	m_bDummy			= false;
	m_pDummyMat			= NULL;
	m_bWarp				= false;
	m_bTLVert			= true;
	m_bAddedToWarpQueue = false;
	m_bAddedToPostQueue = false;
	m_dwRenderSlot		= 0;
	m_nRenderLayer		= GFX_RENDER_LAYER_NORMAL;
	m_bInit				= false;
	m_bIsPhysEle		= false;
	m_bResetWhenResumeLoop	= false;
	m_bCanDoFadeOut		= true;
    m_bSoftEdge         = false;
	m_bAbsTexPath		= false;
	m_bUseInnerLight	= false;
	m_dwTexTickCount	= 0;
	m_dwTexTimeTotal	= 0;
	m_dwEleTickTime		= 0;

	m_AABB.Clear();
}

A3DGFXElement::~A3DGFXElement()
{
	delete m_pDummyMat;

	if (m_bAddedToWarpQueue)
		AfxGetGFXExMan()->RemoveWarpEle(this);

	if (m_bAddedToPostQueue)
		AfxGetGFXExMan()->RemoveDelayedEle(this);
}

int A3DGFXElement::m_nUnnamedCount	= 1;

A3DGFXElement* A3DGFXElement::CreateEmptyElement(
	A3DDevice* pDev,
	A3DGFXEx* pGfx,
	int nEleId)
{
	A3DGFXElement* pEle = NULL;

	switch (nEleId)
	{
	case ID_ELE_TYPE_DECAL_3D:
	case ID_ELE_TYPE_DECAL_2D:
	case ID_ELE_TYPE_DECAL_BILLBOARD:
		pEle = new A3DDecalEx(pGfx, nEleId);
		break;
	case ID_ELE_TYPE_TRAIL:
		pEle = new A3DTrail(pGfx);
		break;
    case ID_ELE_TYPE_TRAIL_EX:
        pEle = new A3DTrailEx(pGfx);
        break;
	case ID_ELE_TYPE_LTNTRAIL:
		pEle = new A3DLtnTrail(pGfx);
		break;
	case ID_ELE_TYPE_PARTICLE_POINT:
	case ID_ELE_TYPE_PARTICLE_BOX:
	case ID_ELE_TYPE_PARTICLE_MULTIPLANE:
	case ID_ELE_TYPE_PARTICLE_ELLIPSOID:
	case ID_ELE_TYPE_PARTICLE_CYLINDER:
	case ID_ELE_TYPE_PARTICLE_CURVE:
	case ID_ELE_TYPE_PARTICLE_SKELETON:
		pEle = new A3DParticleSystemEx(pGfx, nEleId);
		break;
	case ID_ELE_TYPE_LIGHT:
		pEle = new A3DGFXLight(pGfx);
		break;
	case ID_ELE_TYPE_RING:
		pEle = new A3DGFXRing(pGfx);
		break;
	case ID_ELE_TYPE_PARABOLOID:
		pEle = new A3DParaboloid(pGfx);
		break;
	case ID_ELE_TYPE_LIGHTNING:
		pEle = new A3DGFXLightning(pGfx);
		break;
	case ID_ELE_TYPE_LIGHTNINGEX:
		pEle = new A3DGFXLightningEx(pGfx);
		break;
	case ID_ELE_TYPE_LTNBOLT:
		pEle = new A3DGFXLtnBolt(pGfx);
		break;
	case ID_ELE_TYPE_MODEL:
		pEle = new A3DGFXModel(pGfx);
		break;
	case ID_ELE_TYPE_ECMODEL:
		pEle = new A3DGFXECModel(pGfx);
		break;
	case ID_ELE_TYPE_SOUND:
		pEle = new A3DGFXSound(pGfx);
		break;
	case ID_ELE_TYPE_GFX_CONTAINER:
		pEle = new A3DGFXContainer(pGfx);
		break;
	case ID_ELE_TYPE_GRID_DECAL_2D:
	case ID_ELE_TYPE_GRID_DECAL_3D:
		pEle = new A3DGridDecalEx(pGfx, nEleId);
		break;
#ifdef A3D_PHYSX
	case ID_ELE_TYPE_PHYS_EMITTER:
	case ID_ELE_TYPE_PHYS_POINTEMITTER:
		pEle = A3DPhysElement::CreatePhysElement(pGfx, nEleId);
		break;
#endif
	case ID_ELE_TYPE_RIBBON:
		pEle = new A3DRibbon(pGfx);
		break;
	case ID_ELE_TYPE_MODEL_PROXY:
		pEle = new A3DGFXModelProxy(pGfx);
		break;
	default:
		return NULL;
	}

	if (pEle && !pEle->Init(pDev))
	{
		delete pEle;
		return NULL;
	}

	return pEle;
}

//static
A3DGFXElement* A3DGFXElement::LoadElementFromFile(A3DDevice* pDev, A3DGFXEx* pGfx, AFile* pFile, DWORD dwVersion)
{
	A3DGFXElement*	pEle = NULL;
	char			szLine[AFILE_LINEMAXLEN];
	DWORD			dwReadLen;
	int				nEleId;
	int				nCount;
	DWORD			dwDelay;
	char			szBuf[MAX_PATH];

	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&nEleId, sizeof(nEleId), &dwRead);

		pEle = CreateEmptyElement(pDev, pGfx, nEleId);
		if (pEle == NULL) return NULL;
		pEle->m_nEleType = nEleId;

		pEle->m_strName.Empty();
		pFile->ReadString(pEle->m_strName);
		pFile->Read(&pEle->m_Shader.SrcBlend, sizeof(pEle->m_Shader.SrcBlend), &dwRead);
		pFile->Read(&pEle->m_Shader.DestBlend, sizeof(pEle->m_Shader.DestBlend), &dwRead);
		pFile->Read(&nCount, sizeof(nCount), &dwRead);
		pFile->Read(&dwDelay, sizeof(dwDelay), &dwRead);
		pFile->ReadString(pEle->m_strTexture);
		pFile->ReadString(pEle->m_strBind);
		pFile->Read(&pEle->m_bZEnable, sizeof(pEle->m_bZEnable), &dwRead);
		pFile->Read(&pEle->m_nTexRow, sizeof(pEle->m_nTexRow), &dwRead);
		pFile->Read(&pEle->m_nTexCol, sizeof(pEle->m_nTexCol), &dwRead);
		pFile->Read(&pEle->m_dwTexInterval, sizeof(pEle->m_dwTexInterval), &dwRead);
		pFile->Read(&pEle->m_nPriority, sizeof(pEle->m_nPriority), &dwRead);
		pFile->Read(&pEle->m_bDummy, sizeof(pEle->m_bDummy), &dwRead);
		pFile->ReadString(pEle->m_strDummy);
		pFile->Read(&pEle->m_bWarp, sizeof(pEle->m_bWarp), &dwRead);
		pFile->Read(&pEle->m_bTileMode, sizeof(pEle->m_bTileMode), &dwRead);
		pFile->Read(&pEle->m_fUOffsetSpeed, sizeof(float), &dwRead);
		pFile->Read(&pEle->m_fVOffsetSpeed, sizeof(float), &dwRead);
		pFile->Read(&pEle->m_nRenderLayer, sizeof(int), &dwRead);
	}
	else
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _gfx_load_format, &nEleId);

		pEle = CreateEmptyElement(pDev, pGfx, nEleId);
		if (pEle == NULL) return NULL;
		pEle->m_nEleType = nEleId;

		szBuf[0] = '\0';
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_name, szBuf);
		pEle->m_strName = szBuf;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_srcbld, &pEle->m_Shader.SrcBlend);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_destbld, &pEle->m_Shader.DestBlend);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_repeatcount, &nCount);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_repeatdelay, &dwDelay);

		szBuf[0] = '\0';
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_texpath, szBuf);
		pEle->m_strTexture = szBuf;

		if (dwVersion >= 4)
		{
			szBuf[0] = '\0';
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_bind, szBuf);
			pEle->m_strBind = szBuf;
		}

		if (dwVersion >= 8)
		{
			int nBool;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_zenable, &nBool);
			pEle->m_bZEnable = (nBool != 0);

			if (dwVersion < 76)
				pEle->m_bZEnable = true;
		}

		if (dwVersion >= 22)
		{
			int nBool;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_matchgrnd, &nBool);
			pEle->m_bGroundNormal = (nBool != 0);
		}

		if (dwVersion >= 57)
		{
			int nBool;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _ground_height, &nBool);
			pEle->m_bGroundHeight = (nBool != 0);
		}

		if (dwVersion >= 9)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_texrow, &pEle->m_nTexRow);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_texcol, &pEle->m_nTexCol);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_texint, &pEle->m_dwTexInterval);
		}

		if (dwVersion >= 28)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_priority, &pEle->m_nPriority);
		}

		if (dwVersion >= 32)
		{
			int nBool;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_is_dummy, &nBool);
			pEle->m_bDummy = (nBool != 0);

			szBuf[0] = '\0';
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_dummy_ele, szBuf);
			pEle->m_strDummy = szBuf;
		}

		if (dwVersion >= 33)
		{
			int nBool;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_warp, &nBool);
			pEle->m_bWarp = (nBool != 0);
		}

		if (dwVersion >= 34)
		{
			int nBool;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_tile_mode, &nBool);
			pEle->m_bTileMode = (nBool != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_tile_speed, &pEle->m_fUOffsetSpeed);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_tile_speed, &pEle->m_fVOffsetSpeed);
		}

		pEle->CalcTexAnimation();

		if (dwVersion >= 41)
		{
			int nBool;

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_u_reverse, &nBool);
			pEle->m_bUReverse = (nBool != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_v_reverse, &nBool);
			pEle->m_bVReverse = (nBool != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_uv_exchg, &nBool);
			pEle->m_bUVInterchange = (nBool != 0);
		}

		if (dwVersion >= 45)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_layer, &pEle->m_nRenderLayer);
		}

		if (dwVersion >= 58)
		{
			int nBool;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_nodownsample, &nBool);
			pEle->m_bTexNoDownSample = (nBool != 0);
		}

		if (dwVersion >= 74)
		{
			int nBool = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_resetloopend, &nBool);
			pEle->m_bResetWhenResumeLoop = (nBool != 0);
		}

		if (dwVersion >= 75)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_texanimmaxtime, &pEle->m_dwTexTimeTotal);
		}

		if (dwVersion >= 91)
		{
			if (dwVersion >= 95)
				g_GfxLoadPixelShaderConsts(pFile, pEle->m_strPixelShader, pEle->m_strShaderTex, pEle->m_vecPSConsts);
			else
			{
				szBuf[0] = '\0';
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_shaderpath, szBuf);
				pEle->m_strPixelShader = szBuf;

				szBuf[0] = '\0';
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_shadertex, szBuf);
				pEle->m_strShaderTex = szBuf;

				int i, j;
				int nPSConstCount = 0;
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_psconstcount, &nPSConstCount);
				pEle->m_vecPSConsts.reserve(nPSConstCount);

				int nTargetCount;

				for (i = 0; i < nPSConstCount; i++)
				{
					GfxPixelShaderConst psconst;
					A3DCOLORVALUE& value = psconst.init_val;

					pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_psindex, &psconst.index);

					pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_psvalue, &value.r, &value.g, &value.b, &value.a);

					if (dwVersion >= 93)
					{
						pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLine, _format_psloop, &psconst.loop_count);
					}
					else
						psconst.loop_count = 1;

					nTargetCount = 0;
					pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
					sscanf(szLine, _format_pstargetcount, &nTargetCount);
					psconst.target_vals.reserve(nTargetCount);

					for (j = 0; j < nTargetCount; j++)
					{
						GfxPixelShaderTargetValue tv;
						A3DCOLORVALUE& value = tv.value;

						pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLine, _format_psinterval, &tv.interval);

						if (int(tv.interval) < 0 && dwVersion < 93)
							tv.interval = 0;

						pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
						sscanf(szLine, _format_psvalue, &value.r, &value.g, &value.b, &value.a);

						psconst.target_vals.push_back(tv);
					}

					psconst.CalcTotalTime();
					pEle->m_vecPSConsts.push_back(psconst);
				}
			}
		}

		if (dwVersion >= 92)
		{
			int nBool = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_candofadeout, &nBool);
			pEle->m_bCanDoFadeOut = (nBool != 0);
		}

        if (dwVersion >= 104)
        {
            int nBool = 0;
            pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
            sscanf(szLine, _format_softedge, &nBool);
            pEle->m_bSoftEdge = (nBool != 0);
        }

		if (dwVersion >= 118)
		{
			int nBool = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_useinnerlight, &nBool);
			pEle->m_bUseInnerLight = (nBool != 0);
		}

		if (dwVersion >= 105)
		{
			if (!pEle->m_HLSLInfo.Load(pFile))
				return false;
		}

		if (dwVersion >= 109)
		{
			if (!pEle->m_AnimatedProperty.Load(pFile))
				return false;
		}
    }

	pEle->m_KeyPointSet.SetRepeatCount(nCount);
	pEle->m_KeyPointSet.SetRepeatDelay(dwDelay);

	if (!pEle->Load(pDev, pFile, dwVersion))
	{
		delete pEle;
		return NULL;
	}

	pEle->m_KeyPointSet.Load(pFile, dwVersion);
	return pEle;
}

bool A3DGFXElement::SaveElementToFile(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_nEleType, sizeof(m_nEleType), &dwWrite);
		pFile->WriteString(m_strName);
		pFile->Write(&m_Shader.SrcBlend, sizeof(m_Shader.SrcBlend), &dwWrite);
		pFile->Write(&m_Shader.DestBlend, sizeof(m_Shader.DestBlend), &dwWrite);
		int nRepeatCount = m_KeyPointSet.GetRepeatCount();
		pFile->Write(&nRepeatCount, sizeof(nRepeatCount), &dwWrite);
		int nRepeatDelay = m_KeyPointSet.GetRepeatDelay();
		pFile->Write(&nRepeatDelay, sizeof(nRepeatDelay), &dwWrite);
		pFile->WriteString(m_strTexture);
		pFile->WriteString(m_strBind);
		pFile->Write(&m_bZEnable, sizeof(m_bZEnable), &dwWrite);
		pFile->Write(&m_nTexRow, sizeof(m_nTexRow), &dwWrite);
		pFile->Write(&m_nTexCol, sizeof(m_nTexCol), &dwWrite);
		pFile->Write(&m_dwTexInterval, sizeof(m_dwTexInterval), &dwWrite);
		pFile->Write(&m_nPriority, sizeof(m_nPriority), &dwWrite);
		pFile->Write(&m_bDummy, sizeof(m_bDummy), &dwWrite);
		pFile->WriteString(m_strDummy);
		pFile->Write(&m_bWarp, sizeof(m_bWarp), &dwWrite);
		pFile->Write(&m_bTileMode, sizeof(m_bTileMode), &dwWrite);
		pFile->Write(&m_fUOffsetSpeed, sizeof(float), &dwWrite);
		pFile->Write(&m_fVOffsetSpeed, sizeof(float), &dwWrite);
		pFile->Write(&m_nRenderLayer, sizeof(int), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _gfx_load_format, m_nEleType);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_name, m_strName);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_srcbld, m_Shader.SrcBlend);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_destbld, m_Shader.DestBlend);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_repeatcount, m_KeyPointSet.GetRepeatCount());
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_repeatdelay, m_KeyPointSet.GetRepeatDelay());
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_texpath, m_strTexture);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_bind, m_strBind);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_zenable, m_bZEnable);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_matchgrnd, m_bGroundNormal);
		pFile->WriteLine(szLine);

		sprintf(szLine, _ground_height, m_bGroundHeight);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_texrow, m_nTexRow);
		pFile->WriteLine(szLine);	

		sprintf(szLine, _format_texcol, m_nTexCol);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_texint, m_dwTexInterval);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_priority, m_nPriority);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_is_dummy, (int)m_bDummy);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_dummy_ele, m_strDummy);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_warp, (int)m_bWarp);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_tile_mode, m_bTileMode);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_tile_speed, m_fUOffsetSpeed);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_tile_speed, m_fVOffsetSpeed);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_u_reverse, m_bUReverse);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_v_reverse, m_bVReverse);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_uv_exchg, m_bUVInterchange);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_layer, m_nRenderLayer);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_nodownsample, m_bTexNoDownSample);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_resetloopend, m_bResetWhenResumeLoop);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_texanimmaxtime, m_dwTexTimeTotal);
		pFile->WriteLine(szLine);

		g_GfxSavePixelShaderConsts(pFile, m_strPixelShader, m_strShaderTex, m_vecPSConsts);

		sprintf(szLine, _format_candofadeout, m_bCanDoFadeOut);
		pFile->WriteLine(szLine);

        sprintf(szLine, _format_softedge, m_bSoftEdge);
        pFile->WriteLine(szLine);

		sprintf(szLine, _format_useinnerlight, m_bUseInnerLight);
		pFile->WriteLine(szLine);

		if (!m_HLSLInfo.Save(pFile))
			return false;

		if (!m_AnimatedProperty.Save(pFile))
			return false;
	}

	if (!Save(pFile))
		return false;

	m_KeyPointSet.Save(pFile);
	return true;
}

bool A3DGFXElement::Init(A3DDevice* pDevice)
{
	assert(pDevice);
	m_pDevice = pDevice;
	return true;
}

void A3DGFXElement::Release()
{
	if (m_dwRenderSlot)
	{
		AfxGetGFXExMan()->GetRenderMan().UnregisterSlot(m_dwRenderSlot);
		m_dwRenderSlot = 0;
	}

	ReleaseTex();
	m_HLSLInfo.Release();
}

void A3DGFXElement::InitBaseData()
{
	if (m_bWarp)
		ChangeTexture();

	// init hlsl
	if (m_HLSLInfo.HasPixelShader())
	{
		//	Only if has pixel shader(file path exist), then should be reloaded
		m_HLSLInfo.Release();
		m_HLSLInfo.PixelShader()->LoadShader(m_pDevice);
#ifdef _ANGELICA21
		m_HLSLInfo.PixelShader()->SetShaderNoReleaseFlag(true);
#endif
	}

	m_dwTexTickCount = 0;
	m_dwEleTickTime = 0;
}

bool A3DGFXElement::Play()
{
	// Init Once
	if (!IsInit())
	{
		InitBaseData();
		SetInit(true);
	}
	
	return true;
}

bool A3DGFXElement::Stop()
{
	if (m_bNeedCalcTex)
	{
		m_fTexU			= 0;
		m_fTexV			= 0;
				
		m_nCurTexRow	= 0;
		m_nCurTexCol	= 0;
			
		m_dwTexTimeRemain = 0;
		m_dwTexTickCount	= 0;
	}

	m_dwEleTickTime = 0;
	m_KeyPointSet.Resume();

	return true;
}

bool A3DGFXElement::Reload()
{
	InitBaseData();
	SetInit(true);
	return true;
}

bool A3DGFXElement::ChangeTexture()
{
	if (m_strTexture.IsEmpty())
		return false;

	if (m_pTexture)
		m_pDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexture);

	m_pTexture = AfxGetGFXTexMan().QueryTexture(m_strTexture, m_bTexNoDownSample, m_bAbsTexPath);
	return m_pTexture != NULL;
}

void A3DGFXElement::ResumeLoop()
{
	// Why test this condition?
	// Because we do nothing in this function before version 74 in which this m_bResetWhenResumeLoop is added.
	if (!m_bResetWhenResumeLoop)
		return;

	if (m_bNeedCalcTex) {

		m_nCurTexRow	= 0;
		m_nCurTexCol	= 0;
		
		m_dwTexTimeRemain = 0;
		m_dwTexTickCount = 0;
	}

	m_dwEleTickTime = 0;
}

bool A3DGFXElement::TickAnimation(DWORD dwTickTime)
{
	m_KeyPointSet.TickAnimation(dwTickTime);

	if (!m_KeyPointSet.IsActive())
		return false;

	if (m_pBind && !m_pBind->m_KeyPointSet.IsActive())
		return false;
	
	if (m_bNeedCalcTex)
	{
		m_dwTexTickCount += dwTickTime;

		//	以前的情况是只要m_bNeedCalcTex为真，就调用TexAnimation
		//	现在需要添加纹理动画最大播放时间(当已播放的纹理动画时间超出这个最大时间时，停留在最后一帧上)
		//	且该时间在TileMode下不起作用，该时间被设置为零时也不起作用，因此增加了以下判断: 
		//	if (m_bTileMode || m_dwTexTimeTotal == 0 || m_dwTexTimeTotal > m_dwTexTickCount)
		if (m_bTileMode || m_dwTexTimeTotal == 0 || m_dwTexTimeTotal > m_dwTexTickCount)
			TexAnimation(dwTickTime);
	}

	m_dwEleTickTime += dwTickTime;
	m_AnimatedProperty.Apply(m_dwEleTickTime, this);
	return IsInit();
}

void A3DGFXElement::UpdateTex()
{
	SetInit(false);
}

void A3DGFXElement::GetTexTrans(float vTexTrans[4]) const
{
	// only wzyx swizzle supported in ps
	// mad r1.xy, c8, t0, c8.wzyx
	// texld r1, r1, s1
	float& u0 = vTexTrans[0];
	float& v0 = vTexTrans[1];
	float& v1 = vTexTrans[2];
	float& u1 = vTexTrans[3];
	float rw = 1.0f / m_fTexWid;
	float rh = 1.0f / m_fTexHei;

	if (m_bUReverse)
	{
		u0 = -rw;
		u1 = 1.0f + m_fTexU * rw;
	}
	else
	{
		u0 = rw;
		u1 = -m_fTexU * rw;
	}

	if (m_bVReverse)
	{
		v0 = -rh;
		v1 = 1.0f + m_fTexV * rh;
	}
	else
	{
		v0 = rh;
		v1 = -m_fTexV * rh;
	}

	if (m_bUVInterchange)
	{
		gfx_swap(u0, v0);
		gfx_swap(u1, v1);
	}
}

static A3DVECTOR4 GetLightDirInScreenSpace(A3DEngine* pA3DEngine, A3DViewport* pView)
{
	A3DMATRIX4 matWorldToScreenTM = pView->GetCamera()->GetVPTM();
	matWorldToScreenTM.Transpose();
	A3DVECTOR4 vScreenLightDir;
#ifdef _ANGELICA21
	A3DVECTOR4 vSunLightDir = pA3DEngine->GetSceneRenderConfig()->GetSunLightDir();
	A3DVECTOR3 vLightDir(vSunLightDir.x, vSunLightDir.y, vSunLightDir.z);
	A3DVECTOR3 vScreenDir(DotProduct(vLightDir, matWorldToScreenTM.GetRow(0)), 
		DotProduct(vLightDir, matWorldToScreenTM.GetRow(1)),
		DotProduct(vLightDir, matWorldToScreenTM.GetRow(2)));
	vScreenDir.Normalize();
	return A3DVECTOR4(vScreenDir.x, vScreenDir.y, vScreenDir.z, 0);
#else
	A3DVECTOR3 vLightDir = pA3DEngine->GetA3DEnvironment()->GetDirLightParam()->Direction;
	A3DVECTOR3 vScreenDir(DotProduct(vLightDir, matWorldToScreenTM.GetRow(0)), 
		DotProduct(vLightDir, matWorldToScreenTM.GetRow(1)),
		DotProduct(vLightDir, matWorldToScreenTM.GetRow(2)));
	vScreenDir.Normalize();
	return A3DVECTOR4(vScreenDir.x, vScreenDir.y, vScreenDir.z, 0);
#endif
}

void A3DGFXElement::ApplyPSConstants(A3DViewport* pView)
{
	m_HLSLInfo.AnimationTrack().Apply(m_dwEleTickTime, this);
	float vTexTrans[4];
	GetTexTrans(vTexTrans);
	m_HLSLInfo.PixelShader()->SetConstValueFloat4(HLSL_CONST_TEXTRANS, vTexTrans[0], vTexTrans[1], vTexTrans[2], vTexTrans[3]);
	m_HLSLInfo.PixelShader()->SetConstValueFloat4(HLSL_CONST_TIME, m_KeyPointSet.GetTimeElapsed() / 1000.0f, 0, 0, 0);
	m_HLSLInfo.PixelShader()->SetConstValueFloat4(HLSL_CONST_SCREENSIZE, static_cast<float>(pView->GetParam()->Width), static_cast<float>(pView->GetParam()->Height), 0, 0);
	A3DVECTOR4 vLightDirInScreenSpace = GetLightDirInScreenSpace(m_pDevice->GetA3DEngine(), pView);
	m_HLSLInfo.PixelShader()->SetConstValueFloat4(HLSL_CONST_SCREENLIGHTDIR, vLightDirInScreenSpace.x, vLightDirInScreenSpace.y, vLightDirInScreenSpace.z, vLightDirInScreenSpace.w);

	
#if defined (_ANGELICA22)
	KEY_POINT kp;
	if(GetCurKeyPoint(&kp))
	{
		m_HLSLInfo.PixelShader()->SetConstValueFloat4(HLSL_CONST_GFX_DIFFUSE, 
			A3DCOLOR_GETRED(kp.m_color) / 255.0f, 
			A3DCOLOR_GETGREEN(kp.m_color) / 255.0f,
			A3DCOLOR_GETBLUE(kp.m_color) / 255.0f,
			A3DCOLOR_GETALPHA(kp.m_color) / 255.0f);
	}
	else
	{
		m_HLSLInfo.PixelShader()->SetConstValueFloat4(HLSL_CONST_GFX_DIFFUSE, 1.0f, 1.0f, 1.0f, 1.0f);
	}
#endif

}

void A3DGFXElement::ApplyPixelShader(A3DViewport* pView)	
{
	ApplyPSConstants(pView);
	m_HLSLInfo.PixelShader()->Appear();
}

static const char* GetFrameBufferParamName() 
{
#ifdef _ANGELICA21
	return "g_FrameBuffer";
#else
	return ":FrameBuffer_GFX_Replace";
#endif
}

bool A3DGFXElement::IsEleShouldBeDelayRendered() const
{
	return HasPixelShader() && GetHLSLInfo().PixelShader()->IsConstantExist(GetFrameBufferParamName());
}

void A3DGFXElement::ApplyPSConstantsWithFrameBuffer(A3DViewport* pView, A3DRenderTarget* pFrameBuffer)
{
	ApplyPSConstants(pView);
	m_HLSLInfo.PixelShader()->SetTextureFromRT(GetFrameBufferParamName(), pFrameBuffer);
}

void A3DGFXElement::RestorePixelShader()
{
	m_HLSLInfo.PixelShader()->Disappear();
}

class GFXReplaceHLSL : public A3DReplaceHLSL
{
	A3DHLPixelShader* m_pHLSL;
	AString m_strPostfix;
	GFXReplaceHLSL(const GFXReplaceHLSL&);
	GFXReplaceHLSL& operator = (const GFXReplaceHLSL&);
public:
	GFXReplaceHLSL(A3DHLPixelShader* pHLSL)
		: m_pHLSL(pHLSL)
		, m_strPostfix("_GFX_Replace")
	{

	}

	void RestoreRelpacedTexs()
	{
		// 清空所有被SkinModel替换上来的贴图
		// 对SkinModel提供的贴图不做保留，因为不知道下一回再使用的时候，之前的SKinModel是否已经被干掉了
		RepParams aParams = GetNeedRepTextureParams();
		for (RepParams::const_iterator itr = aParams.begin()
			; itr != aParams.end()
			; ++itr)
		{
			m_pHLSL->SetTexture((*itr).strOriginalParam, NULL);
		}
	}

protected:
	virtual A3DHLSL* GetReplaceHLSL() const { return m_pHLSL->GetHLSL(); }

#ifdef _ANGELICA22
	virtual bool IsNeedReplaceTexture(const AString& strSrcParam, const AString& strSrcSemantic, AString* pReqParamName) const
	{
		if (strSrcSemantic.IsEmpty() || strSrcSemantic == GetFrameBufferParamName())
			return false;

		if (strSrcSemantic.Find(m_strPostfix, strSrcSemantic.GetLength() - m_strPostfix.GetLength()) < 0)
			return false;

		*pReqParamName = strSrcSemantic.Left(strSrcSemantic.GetLength() - m_strPostfix.GetLength());
		return true;
	}
#elif _ANGELICA21
	virtual bool SetupAlphaMapMask(float vMasks[4]) const 
	{
		return m_pHLSL->SetConstValueFloat4(HLSL_CONST_ALPHAMASK, vMasks[0], vMasks[1], vMasks[2], vMasks[3]);
	}
	virtual bool IsNeedReplaceTexture(const AString& strSrcParam, const AString& strSrcSemantic, AString* pReqParamName) const
	{
		if (strSrcParam.IsEmpty() || strSrcParam == GetFrameBufferParamName())
			return false;

		if (strSrcParam.Find(m_strPostfix, strSrcParam.GetLength() - m_strPostfix.GetLength()) < 0)
			return false;

		*pReqParamName = strSrcParam.Left(strSrcParam.GetLength() - m_strPostfix.GetLength());
		return true;
	}
#endif

};

void A3DGFXElement::SkinModelRenderAtOnce(A3DSkinModel* pSkinModel, A3DViewport* pView, DWORD dwFlags, A3DReplaceHLSL* pRepHLSL)
{
#ifdef _ANGELICA21

	if (m_bUseInnerLight)
	{
		A3DSkinModel::LIGHTINFO info = pSkinModel->GetLightInfo();
		int nEleCount = m_pGfx->GetElementCount();
		bool bLightOk = false;

		for (int i = 0; i < nEleCount; i++)
		{
			A3DGFXElement* pEle = m_pGfx->GetElement(i);

			if (pEle->GetEleTypeId() != ID_ELE_TYPE_LIGHT)
				continue;

			if (!pEle->IsActive())
			{
				bLightOk = false;
				break;
			}
			else
				bLightOk = true;

			const A3DLIGHTPARAM& param = ((A3DGFXLight*)pEle)->GetLightParam();

			if (param.Type == A3DLIGHT_POINT)
			{
				info.bPtLight = true;
				info.colPtDiff = param.Diffuse;
				info.fPtRange = param.Range;
				info.vPtAtten = A3DVECTOR3(0, 6.f / param.Range, 0);
				info.vPtLightPos = param.Position;
			}
			else if (param.Type == A3DLIGHT_DIRECTIONAL)
			{
				info.colDirDiff = param.Diffuse;
				info.colDirSpec = param.Specular;
				info.colAmbient = param.Ambient;
				info.vLightDir = param.Direction;
			}
		}

		if (bLightOk)
		{
			pSkinModel->SetLightInfo(info);
			dwFlags |= A3DSkinModel::RAO_USE_MODEL_LIGHT;
		}
	}

	pSkinModel->RenderAtOnce(pView, dwFlags, !m_pGfx->Is2DRender(), pRepHLSL);
#else
	pSkinModel->RenderAtOnce(pView, dwFlags, true, pRepHLSL);
#endif
}

void A3DGFXElement::RenderSkinModel(A3DViewport* pView, A3DSkinModel* pSkinModel, A3DRenderTarget* pFrameBuffer)
{
	PrepareRenderSkinModel();

	if (HasPixelShader() && m_HLSLInfo.PixelShader()->GetHLSL())
	{
		if (pFrameBuffer)
			ApplyPSConstantsWithFrameBuffer(pView, pFrameBuffer);
		else
			ApplyPSConstants(pView);
		
		// 这里不设置PS，等到SkinModel RenderAtOnce里会设置
		// 但是要设置贴图，不然美术设置好的贴图就没地方设置了，进入RenderAtOnce之后会再做替换

		m_HLSLInfo.PixelShader()->AppearTextures();

		GFXReplaceHLSL repHLSL(m_HLSLInfo.PixelShader());
		SkinModelRenderAtOnce(pSkinModel, pView, A3DGFXPIFuncs::GetSkinModelNoPixelShaderFlag(), &repHLSL);
		
		// SkinModel可能会设置一堆贴图，离开之前要清理掉
		repHLSL.RestoreRelpacedTexs();
		RestorePixelShader();
	}
	else
	{
		SkinModelRenderAtOnce(pSkinModel, pView, 0, NULL);
	}

	RestoreRenderSkinModel();
}

const A3DMATRIX4& A3DGFXElement::GetParentTM() const
{
	if (m_pGfx->MatchGrnd())
	{
		if (m_bGroundHeight)
		{
			if (m_bGroundNormal)
				return m_pGfx->GetGrndNormalTM();
			else
				return m_pGfx->GetGrndHeightTM();
		}
		else if (m_bGroundNormal && m_pGfx->IsCloseToGrnd())
			return m_pGfx->GetGrndNormalTM();
	}

	return m_pGfx->GetParentTM();
}

const A3DQUATERNION& A3DGFXElement::GetParentDir() const
{
	if (m_pGfx->MatchGrnd())
	{
		if (m_bGroundNormal)
		{
			if (m_bGroundHeight || m_pGfx->IsCloseToGrnd())
				return m_pGfx->GetMatchGrndDir();

		}
	}

	return m_pGfx->GetDir();
}

A3DVECTOR3 A3DGFXElement::GetAABBCenter()
{
#ifdef GFX_EDITOR

	KEY_POINT kp;

	if (!GetCurKeyPoint(&kp))
		return A3DVECTOR3(0);

	A3DMATRIX4 mat;
	mat.Scale(kp.m_fScale, kp.m_fScale, kp.m_fScale);
	mat = mat * GetTranMatrix(kp, _unit_y) * GetParentTM();

	return mat.GetRow(3);

#else

	return A3DVECTOR3(0);

#endif

}

bool A3DGFXElement::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch (nOp)
	{
	case ID_GFXOP_ALPHA_MODE:
		m_Shader = gfx_shader_from_mode((int)prop);
		break;
	case ID_GFXOP_REPEATCOUNT:
		m_KeyPointSet.SetRepeatCount(prop);
		break;
	case ID_GFXOP_REPEATDELAY:
		m_KeyPointSet.SetRepeatDelay(prop);
		break;
	case ID_GFXOP_ELE_NAME:
		m_strName = prop;
		break;
	case ID_GFXOP_TEX_PATH:
		m_strTexture = prop;
		break;
	case ID_GFXOP_TEX_ROWS:
		m_nTexRow = prop;
		CalcTexAnimation();
		break;
	case ID_GFXOP_TEX_COLS:
		m_nTexCol = prop;
		CalcTexAnimation();
		break;
	case ID_GFXOP_TEX_INTERVAL:
		m_dwTexInterval = prop;
		break;
	case ID_GFXOP_BIND:
		m_strBind = prop;
		break;
	case ID_GFXOP_ZTESTENABLE:
		m_bZEnable = prop;
		break;
	case ID_GFXOP_GROUND_NORMAL:
		m_bGroundNormal = prop;
		m_pGfx->UpdateMatchGround();
		break;
	case ID_GFXOP_GROUND_HEIGHT:
		m_bGroundHeight = prop;
		m_pGfx->UpdateMatchGround();
		break;
	case ID_GFXOP_PRIORITY:
		m_nPriority = prop;
		break;
	case ID_GFXOP_IS_DUMMY:
		m_bDummy = prop;
		break;
	case ID_GFXOP_DUMMY_ELE:
		m_strDummy = prop;
		break;
	case ID_GFXOP_WARP:
		m_bWarp = prop;
		break;
	case ID_GFXOP_TILE_MODE:
		m_bTileMode = prop;
		CalcTexAnimation();
		break;
	case ID_GFXOP_U_SPEED:
		m_fUOffsetSpeed = prop;
		CalcTexAnimation();
		break;
	case ID_GFXOP_V_SPEED:
		m_fVOffsetSpeed = prop;
		CalcTexAnimation();
		break;
	case ID_GFXOP_UV_INTERCHANGE:
		m_bUVInterchange = prop;
		break;
	case ID_GFXOP_U_REVERSE:
		m_bUReverse = prop;
		break;
	case ID_GFXOP_V_REVERSE:
		m_bVReverse = prop;
		break;
	case ID_GFXOP_RENDER_LAYER:
		m_nRenderLayer = prop;
		break;
	case ID_GFXOP_TEX_NODOWNSAMPLE:
		m_bTexNoDownSample = prop;
		break;
	case ID_GFXOP_RESETONLOOPEND:
		m_bResetWhenResumeLoop = prop;
		break;
	case ID_GFXOP_TEXANIMMAXTIME:
		m_dwTexTimeTotal = prop;
		break;
	case ID_GFXOP_PIXELSHADERPATH:
		m_strPixelShader = prop;
		break;
	case ID_GFXOP_SHADER_TEX:
		m_strShaderTex = prop;
		break;
	case ID_GFXOP_CAN_DO_FADE_OUT:
		m_bCanDoFadeOut = prop;
		break;
    case ID_GFXOP_SOFT_EDGE:
        m_bSoftEdge = prop;
        break;
	case ID_GFXOP_USE_INNER_LIGHT:
		m_bUseInnerLight = prop;
		break;
	default:
		return false;
	}
	return true;
}

GFX_PROPERTY A3DGFXElement::GetProperty(int nOp) const
{
	switch (nOp)
	{
	case ID_GFXOP_ALPHA_MODE:
		return GFX_PROPERTY(gfx_mode_from_shader(m_Shader)).SetType(GFX_VALUE_SHADER_MODE);
	case ID_GFXOP_RENDER_LAYER:
		return GFX_PROPERTY(m_nRenderLayer).SetType(GFX_VALUE_RENDER_LAYER);
	case ID_GFXOP_REPEATCOUNT:
		return GFX_PROPERTY(m_KeyPointSet.GetRepeatCount());
	case ID_GFXOP_REPEATDELAY:
		return GFX_PROPERTY((int)m_KeyPointSet.GetRepeatDelay());
	case ID_GFXOP_ELE_NAME:
		return GFX_PROPERTY(m_strName);
	case ID_GFXOP_TEX_PATH:
		return GFX_PROPERTY(m_strTexture, GFX_VALUE_PATH_TEX);
	case ID_GFXOP_TEX_ROWS:
		return GFX_PROPERTY(m_nTexRow);
	case ID_GFXOP_TEX_COLS:
		return GFX_PROPERTY(m_nTexCol);
	case ID_GFXOP_TEX_INTERVAL:
		return GFX_PROPERTY((int)m_dwTexInterval);
	case ID_GFXOP_BIND:
		return GFX_PROPERTY(m_strBind);
	case ID_GFXOP_ZTESTENABLE:
		return GFX_PROPERTY(m_bZEnable);
	case ID_GFXOP_GROUND_NORMAL:
		return GFX_PROPERTY(m_bGroundNormal);
	case ID_GFXOP_GROUND_HEIGHT:
		return GFX_PROPERTY(m_bGroundHeight);
	case ID_GFXOP_PRIORITY:
		return GFX_PROPERTY(m_nPriority);
	case  ID_GFXOP_IS_DUMMY:
		return GFX_PROPERTY(m_bDummy);
	case  ID_GFXOP_DUMMY_ELE:
		return GFX_PROPERTY(m_strDummy);
	case ID_GFXOP_WARP:
		return GFX_PROPERTY(m_bWarp);
	case ID_GFXOP_TILE_MODE:
		return GFX_PROPERTY(m_bTileMode);
	case ID_GFXOP_U_SPEED:
		return GFX_PROPERTY(m_fUOffsetSpeed);
	case ID_GFXOP_V_SPEED:
		return GFX_PROPERTY(m_fVOffsetSpeed);
	case ID_GFXOP_UV_INTERCHANGE:
		return GFX_PROPERTY(m_bUVInterchange);
	case ID_GFXOP_U_REVERSE:
		return GFX_PROPERTY(m_bUReverse);
	case ID_GFXOP_V_REVERSE:
		return GFX_PROPERTY(m_bVReverse);
	case ID_GFXOP_TEX_NODOWNSAMPLE:
		return GFX_PROPERTY(m_bTexNoDownSample);
	case ID_GFXOP_RESETONLOOPEND:
		return GFX_PROPERTY(m_bResetWhenResumeLoop);
	case ID_GFXOP_TEXANIMMAXTIME:
		return GFX_PROPERTY(m_dwTexTimeTotal).SetType(GFX_VALUE_INT);
	case ID_GFXOP_PIXELSHADERPATH:
		return GFX_PROPERTY(m_strPixelShader, GFX_VALUE_SHADER_PATH);
	case ID_GFXOP_SHADER_TEX:
		return GFX_PROPERTY(m_strShaderTex, GFX_VALUE_PATH_TEX);
	case ID_GFXOP_CAN_DO_FADE_OUT:
		return GFX_PROPERTY(m_bCanDoFadeOut);
    case ID_GFXOP_SOFT_EDGE:
        return GFX_PROPERTY(m_bSoftEdge);
	case ID_GFXOP_USE_INNER_LIGHT:
		return GFX_PROPERTY(m_bUseInnerLight);
	}
	return GFX_PROPERTY();
}

void A3DGFXElement::SetBindEle(A3DGFXElement* pBind)
{
	m_pBind = pBind;

#ifdef GFX_EDITOR

	if (pBind)
		m_strBind = pBind->GetName();
	else
		m_strBind.Empty();

#endif
}

void A3DGFXElement::SetDummyEle(A3DGFXElement* pDummy)
{
	if (pDummy && !pDummy->IsDummyEle())
		return;

	m_pDummy = pDummy;

#ifdef GFX_EDITOR

	if (pDummy)
		m_strDummy = pDummy->GetName();
	else
		m_strDummy.Empty();

#endif
}
