/*
* FILE: A3DGFXSoundImp.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2011/7/27
*
* HISTORY: 
*
* Copyright (c) 2011 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "A3DGFXSoundImp.h"
#include "RandStringContainer.h"
#include "AMSoundEngine.h"
#include "AM3DSoundDevice.h"
#include "A3DGfxInterface.h"

#ifndef _ANGELICA21
#include <FCriticalSectionWrapper.h>
#endif

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

//	For Angelica2 and for non-audio mode of Angelica2.2
static const char* _format_pathnum = "PathNum: %d";
static const char* _format_path = "Path: %s";
static const DWORD _check_interv = 1000;

//	For Angelica 2.2
static const char* _format_usecustom = "UseCustom: %d";
static const char* _format_mindist = "MinDist: %f";
static const char* _format_maxdist = "MaxDist: %f";

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


#ifdef _ANGELICA22
using namespace AudioEngine;
inline VECTOR A3DVECTOR2AudioVECTOR(const A3DVECTOR3& v)
{
	VECTOR ret;
	ret.x = v.x;
	ret.y = v.y;
	ret.z = v.z;
	return ret;
}
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Implement GFXSOUNDIMP
//	
///////////////////////////////////////////////////////////////////////////

GFXSOUNDIMP::GFXSOUNDIMP()
: m_pBuf(NULL)
, m_bStart(false)
, m_bLoadFail(false)
, m_dwCheckTime(0)
, m_pFiles(new RandStringContainer(5))
, m_iSoundCount(0)
, m_dwRelativeVolume(0)
, m_dwLastRelativeVolume(0)
{

}

GFXSOUNDIMP::~GFXSOUNDIMP()
{
	delete m_pFiles;
	m_pFiles = NULL;
}

GFXSOUNDIMP::GFXSOUNDIMP(const GFXSOUNDIMP& rhs)
: m_ParamInfo(rhs.m_ParamInfo)
, m_pFiles(new RandStringContainer(*rhs.m_pFiles))
{
	
}

GFXSOUNDIMP& GFXSOUNDIMP::operator = (const GFXSOUNDIMP& rhs)
{
	if (&rhs == this)
		return *this;

	m_ParamInfo	= rhs.m_ParamInfo;
	*m_pFiles = *rhs.m_pFiles;
	return *this;
}

bool GFXSOUNDIMP::Load(AFile* pFile, DWORD dwVersion)
{
	char	szBuf[MAX_PATH];
	DWORD	dwReadLen;

	szBuf[0] = '\0';
	if (pFile->IsBinary())
	{
		if (dwVersion < 88)
		{
			pFile->ReadString(szBuf, AFILE_LINEMAXLEN, &dwReadLen);
			m_pFiles->UniqueAdd(szBuf);
		}
		else if (dwVersion >= 88)
		{
			int iNum = 0;
			pFile->Read(&iNum, sizeof(iNum), &dwReadLen);
			for (int iIdx = 0; iIdx < iNum; ++iIdx)
			{
				AString strTmp;
				if (!pFile->ReadString(strTmp))
					continue;

				m_pFiles->UniqueAdd(strTmp);
			}
		}
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		if (dwVersion < 88)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_path, szBuf);
			m_pFiles->UniqueAdd(szBuf);
		}
		else if (dwVersion >= 88)
		{
			int iNum = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_pathnum, &iNum);

			for (int iIdx = 0; iIdx < iNum; ++iIdx)
			{
				pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLine, _format_path, szBuf);
				m_pFiles->UniqueAdd(szBuf);
			}
		}
	}

	return m_ParamInfo.LoadSoundParamInfo(pFile);
}

bool GFXSOUNDIMP::Save(AFile* pFile)
{
	DWORD dwWriteLen;
	if (pFile->IsBinary())
	{
		int iNum = m_pFiles->GetSize();
		pFile->Write(&iNum, sizeof(iNum), &dwWriteLen);

		for (int iIdx = 0; iIdx < iNum; ++iIdx)
		{
			pFile->WriteString(m_pFiles->GetString(iIdx));
		}
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_pathnum, m_pFiles->GetSize());
		pFile->WriteLine(szLine);

		for (int iIdx = 0; iIdx < m_pFiles->GetSize(); ++iIdx)
		{
			_snprintf(szLine, AFILE_LINEMAXLEN, _format_path, m_pFiles->GetString(iIdx));
			pFile->WriteLine(szLine);
		}
	}

	return m_ParamInfo.SaveSoundParamInfo(pFile);
}

void GFXSOUNDIMP::PrePlay()
{
	m_bStart = false;
	m_dwCheckTime = _check_interv;
}

void GFXSOUNDIMP::InitData()
{

}

void GFXSOUNDIMP::PostPlay(A3DGFXSound* pSound)
{
	ReleaseSound(pSound);
}

void GFXSOUNDIMP::Stop(A3DGFXSound* pSound)
{
	ReleaseSound(pSound);
	m_bLoadFail = false;
}

void GFXSOUNDIMP::StopSound()
{
	if (m_pBuf)
		m_pBuf->Stop();
}

void GFXSOUNDIMP::ReleaseSound(A3DGFXSound* pSound)
{
	if (!m_pBuf)
		return;

	if (pSound->IsInfinite())
		AfxReleaseSoundLoop(m_pBuf);
	else
		AfxReleaseSoundNonLoop(m_pBuf);

	m_pBuf = NULL;
}

void GFXSOUNDIMP::ClearSoundFile()
{
	m_pFiles->RemoveAll();
}

bool GFXSOUNDIMP::TickSound(A3DGFXSound* pSound, DWORD dwTickTime)
{
#if defined(_ANGELICA21) && !defined(GFX_EDITOR)

	if (m_bLoadFail)
		return true;

#endif

	m_dwCheckTime += dwTickTime;

	if (m_dwCheckTime >= _check_interv)
	{
		m_dwCheckTime = 0;

		if (!m_pBuf)
		{
			if (ChangeSoundFile(pSound))
				UpdateSoundChange();
		}
		else if (!CheckDist(pSound))
		{
			ReleaseSound(pSound);
			m_bStart = false;
		}
	}

	if (!m_pBuf)
		return true;

#ifdef _ANGELICA21

	if (m_pBuf->IsToRelease())
	{
		ReleaseSound(pSound);
		m_bLoadFail = true;
		return true;
	}

	if (!m_pBuf->IsLoaded())
		return true;

	m_pBuf->CheckInitState();

#endif

	if (m_bStart)
		m_pBuf->CheckEnd();
	else
	{
		m_pBuf->Play(m_ParamInfo.GetLoop());
		m_bStart = true;
	}

	if (!m_pBuf->IsStopped())
	{
		KEY_POINT kp;
		pSound->GetCurKeyPoint(&kp);
		float fAlpha = pSound->GetGfx()->GetActualAlpha();
		DWORD dwRelativeVolume = (DWORD)(m_dwRelativeVolume * fAlpha);
		if (m_dwLastRelativeVolume != dwRelativeVolume)
		{
			m_pBuf->SetRelativeVolume(dwRelativeVolume);
			m_dwLastRelativeVolume = dwRelativeVolume;
		}

		m_pBuf->SetPosition(pSound->GetParentTM() * kp.m_vPos);
		m_pBuf->UpdateChanges();
	}

	return true;
}

bool GFXSOUNDIMP::CheckDist(A3DGFXSound* pSound)
{
	if (m_ParamInfo.GetForce2D())
		return true;
	
	A3DVECTOR3 vPos = pSound->GetDevice()->GetA3DEngine()->GetAMSoundEngine()->GetAM3DSoundDevice()->GetPosition();
	return ((pSound->GetParentGfx()->GetPos() - vPos).Normalize() < m_ParamInfo.GetMaxDist());
}

void GFXSOUNDIMP::ResumeLoop()
{
	m_bStart = false;
	m_dwCheckTime = _check_interv;
}

void GFXSOUNDIMP::UpdateSoundChange()
{
	if (!m_pBuf)
		return;

	if (AfxGetGFXExMan()->IsSfxVolumeEnable())
	{
		m_dwRelativeVolume = m_ParamInfo.GetRandVolume();
		m_pBuf->SetRelativeVolume(m_dwRelativeVolume);
		m_dwLastRelativeVolume = m_dwRelativeVolume;
	}

	m_pBuf->SetPlaySpeed((float)pow(2, m_ParamInfo.GetRandPitch() / 12.f));
	m_pBuf->SetForce2D(m_ParamInfo.GetForce2D());
	m_pBuf->SetMinDistance(m_ParamInfo.GetMinDist());
	m_pBuf->SetMaxDistance(m_ParamInfo.GetMaxDist());
}

const char* GFXSOUNDIMP::GetRandSfxFilePath() const
{
#ifdef _ANGELICA21

	if (m_pFiles->GetSize() == 0)
		return "";

	AList2<int> IndexList;

	int nIndexMinTTL = -1;
	int nMinTTL = 100000;
	A3DGFXExMan* pGfxMan = AfxGetGFXExMan();

	for (int i=0; i<m_pFiles->GetSize(); ++i)
	{
		int nttl;
		if (pGfxMan->IsInSfxFileList(m_pFiles->GetString(i), nttl))
		{
			if (nttl < nMinTTL)
			{
				nMinTTL = nttl;
				nIndexMinTTL = i;
			}
		}
		else
		{
			IndexList.AddTail(i);
		}
	}

	if (IndexList.GetCount() == 0)		//全部在最近的200ms内,选取ttl最小者
	{
		if (nIndexMinTTL == -1)
		{
			assert(nIndexMinTTL != -1);
			return "";
		}
		return m_pFiles->GetString(nIndexMinTTL);
	}
	else	//在剩余的sfx文件中随机
	{
		int idx = a_Random(0, IndexList.GetCount() - 1);
		int nCurrent = 0;

		ALISTPOSITION pos = IndexList.GetHeadPosition();
		while (pos)
		{
			int nIdx = IndexList.GetNext(pos);
			if (nCurrent == idx)
			{
				return m_pFiles->GetString(nIdx);
			}
			++nCurrent;
		}
	}

	assert(false);

#endif
	return m_pFiles->GetRandString();
}

bool GFXSOUNDIMP::ChangeSoundFile(A3DGFXSound* pSound)
{
	if (0 == m_pFiles->GetSize())
	{
		m_bLoadFail = true;
		return false;
	}

	AString strFile(GetRandSfxFilePath());
	CheckSoundFileChange(strFile);
	if (strFile.IsEmpty())
		return false;

	if (!CheckDist(pSound))
		return false;

	AfxGetGFXExMan()->PushSfxFile(strFile, 200);

	static const AString strPath = "Sfx\\";
	if (pSound->IsInfinite())
	{
		AM3DSoundBuffer* pSfx = AfxLoadLoopSound(strPath + strFile);

		if (pSfx == NULL)
		{
			m_bLoadFail = true;
			return false;
		}

		if (m_pBuf)
			AfxReleaseSoundLoop(m_pBuf);

		m_pBuf = pSfx;
	}
	else
	{
		AM3DSoundBuffer* pSfx = AfxLoadNonLoopSound(strPath + strFile, pSound->GetParentGfx()->GetSfxPriority());

		if (pSfx == NULL)
		{
			m_bLoadFail = true;
			return false;
		}

		if (m_pBuf)
			AfxReleaseSoundNonLoop(m_pBuf);

		m_pBuf = pSfx;
	}

	return true;
}

void GFXSOUNDIMP::CheckSoundFileChange(AString& strFile)
{
	if (m_pFiles->GetSize() <= 1)
		return;

	if (m_strLastSound == strFile)
	{
		if (++m_iSoundCount >= 2)
		{
			for (int iIdx = 0; iIdx < m_pFiles->GetSize(); ++iIdx)
			{
				if (m_pFiles->GetString(iIdx) == strFile)
					continue;

				strFile = m_pFiles->GetString(iIdx);
				UpdateLastSoundFile(strFile);
				return;
			}
		}
	}
	else
	{
		UpdateLastSoundFile(strFile);
	}
}

void GFXSOUNDIMP::UpdateLastSoundFile(const AString& strFile)
{
	m_strLastSound = strFile;
	m_iSoundCount = 0;
}

bool GFXSOUNDIMP::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch (nOp)
	{
	case ID_GFXOP_SOUND_FORCE2D:
		m_ParamInfo.SetForce2D(prop);
		break;
	case ID_GFXOP_SOUND_LOOP:
		m_ParamInfo.SetLoop(prop);
		break;
	case ID_GFXOP_SOUND_VOLUME_MIN:
		m_ParamInfo.SetVolume((DWORD)prop, m_ParamInfo.GetVolumeMax());
		break;
	case ID_GFXOP_SOUND_VOLUME_MAX:
		m_ParamInfo.SetVolume(m_ParamInfo.GetVolumeMin(), (DWORD)prop);
		break;
	case ID_GFXOP_SOUND_PITCH_MIN:
		m_ParamInfo.SetSoundPitch(prop, m_ParamInfo.GetPitchMax());
		break;
	case ID_GFXOP_SOUND_PITCH_MAX:
		m_ParamInfo.SetSoundPitch(m_ParamInfo.GetPitchMin(), prop);
		break;
	case ID_GFXOP_SOUND_MIN_DIST:
		m_ParamInfo.SetMinDist(prop);
		break;
	case ID_GFXOP_SOUND_MAX_DIST:
		m_ParamInfo.SetMaxDist(prop);
		break;
	case ID_GFXOP_SOUND_FILE:
		;
		break;
	default:
		return false;
	}

	return true;
}

bool GFXSOUNDIMP::GetProperty(int nOp, GFX_PROPERTY& prop) const
{
	switch (nOp)
	{
	case ID_GFXOP_SOUND_FORCE2D:
		prop = GFX_PROPERTY(m_ParamInfo.GetForce2D());
		return true;
	case ID_GFXOP_SOUND_LOOP:
		prop = GFX_PROPERTY(m_ParamInfo.GetLoop());
		return true;
	case ID_GFXOP_SOUND_VOLUME_MIN:
		prop = GFX_PROPERTY((int)m_ParamInfo.GetVolumeMin());
		return true;
	case ID_GFXOP_SOUND_VOLUME_MAX:
		prop = GFX_PROPERTY((int)m_ParamInfo.GetVolumeMax());
		return true;
	case ID_GFXOP_SOUND_PITCH_MIN:
		prop = GFX_PROPERTY(m_ParamInfo.GetPitchMin());
		return true;
	case ID_GFXOP_SOUND_PITCH_MAX:
		prop = GFX_PROPERTY(m_ParamInfo.GetPitchMax());
		return true;
	case ID_GFXOP_SOUND_MIN_DIST:
		prop = GFX_PROPERTY(m_ParamInfo.GetMinDist());
		return true;
	case ID_GFXOP_SOUND_MAX_DIST:
		prop = GFX_PROPERTY(m_ParamInfo.GetMaxDist());
		return true;
	case ID_GFXOP_SOUND_FILE:
		prop = GFX_PROPERTY(m_pFiles).SetType(GFX_VALUE_RAND_PATH_SOUND);
		return true;
	default:
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement GFXSOUNDIMP
//	
///////////////////////////////////////////////////////////////////////////

GFXSOUNDIMP22::GFXSOUNDIMP22()
: m_pEventInstance(NULL)
, m_vLastTickPos(0)
, m_fMinDist(1.0f)
, m_fMaxDist(100.0f)
, m_bUseCustom(false)
, m_bPosInited(false)
{

}

GFXSOUNDIMP22::~GFXSOUNDIMP22()
{
#ifdef _USEAUDIOENGINE
	CSController csc(&m_csForThreadSafe);
	if (m_pEventInstance)
	{
		AudioEngine::Event* pEvent = m_pEventInstance->GetEvent();
		if(!pEvent)
			return;
		CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
		CriticalSectionController cscei(m_pEventInstance->GetCriticalSectionWrapper());
		m_pEventInstance->RemoveListener(this);
	}
#endif
}

GFXSOUNDIMP22::GFXSOUNDIMP22(const GFXSOUNDIMP22& rhs)
{

}

GFXSOUNDIMP22& GFXSOUNDIMP22::operator = (const GFXSOUNDIMP22& rhs)
{
	if (this == &rhs)
		return *this;

	m_strEventPath = rhs.m_strEventPath;
	m_bUseCustom = rhs.m_bUseCustom;
	m_fMinDist = rhs.m_fMinDist;
	m_fMaxDist = rhs.m_fMaxDist;
	ReleaseEventInstance();
	return *this;
}

bool GFXSOUNDIMP22::Load(AFile* pFile, DWORD dwVersion)
{
	char	szBuf[MAX_PATH];
	DWORD	dwReadLen;

	szBuf[0] = '\0';

	char	szLine[AFILE_LINEMAXLEN];

	if (dwVersion >= 96)			//new sound system
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_path, szBuf);
		m_strEventPath = szBuf;

		int iRead = 0;
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_usecustom, &iRead);
		m_bUseCustom = iRead != 0;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_mindist, &m_fMinDist);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_maxdist, &m_fMaxDist);
	}

	return true;
}

bool GFXSOUNDIMP22::Save(AFile* pFile)
{
	char	szLine[AFILE_LINEMAXLEN];

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_path, m_strEventPath);
	pFile->WriteLine(szLine);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_usecustom, m_bUseCustom);
	pFile->WriteLine(szLine);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_mindist, m_fMinDist);
	pFile->WriteLine(szLine);

	_snprintf(szLine, AFILE_LINEMAXLEN, _format_maxdist, m_fMaxDist);
	pFile->WriteLine(szLine);

	return true;
}

void GFXSOUNDIMP22::PrePlay()
{

}

void GFXSOUNDIMP22::InitData()
{

}

void GFXSOUNDIMP22::PostPlay(A3DGFXSound* pSound)
{
	m_bPosInited = false; 

#ifdef _USEAUDIOENGINE
	CSController csc(&m_csForThreadSafe);
	ReleaseEventInstance();
	m_pEventInstance = AfxGetGFXExMan()->GetGfxInterface()->CreateAudioEventInstance(m_strEventPath, true);

	if (m_pEventInstance && m_bUseCustom)
	{
		AudioEngine::EVENT_PROPERTY prop;
		m_pEventInstance->GetProperty(prop);
		prop.fMinDistance = m_fMinDist;
		prop.fMaxDistance = m_fMaxDist;
		m_pEventInstance->SetProperty(prop);
	}

	if(m_pEventInstance)
		m_pEventInstance->AddListener(this);

#endif
}

void GFXSOUNDIMP22::Stop(A3DGFXSound* pSound)
{
	StopSound();
}

void GFXSOUNDIMP22::StopSound()
{
#ifdef _USEAUDIOENGINE
	CSController csc(&m_csForThreadSafe);
	if (m_pEventInstance)
	{
		AudioEngine::Event* pEvent = m_pEventInstance->GetEvent();
		if(!pEvent)
			return;
		CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
		CriticalSectionController cscei(m_pEventInstance->GetCriticalSectionWrapper());
		m_pEventInstance->Stop();
	}
#endif
}

void GFXSOUNDIMP22::ReleaseSound(A3DGFXSound* pSound)
{
	ReleaseEventInstance();
}

void GFXSOUNDIMP22::ReleaseEventInstance()
{
#ifdef _USEAUDIOENGINE
	CSController csc(&m_csForThreadSafe);
	if (m_pEventInstance)
	{
		AudioEngine::Event* pEvent = m_pEventInstance->GetEvent();
		if(!pEvent)
			return;
		CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
		CriticalSectionController cscei(m_pEventInstance->GetCriticalSectionWrapper());
		m_pEventInstance->RemoveListener(this);
		StopSound();
		AfxGetGFXExMan()->GetGfxInterface()->ReleaseAudioEventInstance(m_pEventInstance);
		m_pEventInstance = NULL;
	}
#endif
}

void GFXSOUNDIMP22::ClearSoundFile()
{

}

bool GFXSOUNDIMP22::TickSound(A3DGFXSound* pSound, DWORD dwTickTime)
{
#ifdef _USEAUDIOENGINE
	CSController csc(&m_csForThreadSafe);
	if (!m_pEventInstance)
		return false;
	Event* pEvent = m_pEventInstance->GetEvent();
	if(!pEvent)
		return false;
	CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
	CriticalSectionController cscei(m_pEventInstance->GetCriticalSectionWrapper());

	if (!m_pEventInstance->IsPlaying())
		m_pEventInstance->Start();

	A3DVECTOR3 vCurPos = GetCurPos(pSound);

	A3DVECTOR3 vDeltaPerSecond;
	if (!m_bPosInited)
	{
		vDeltaPerSecond.Clear();
		m_bPosInited = true;
	}
	else
	{
		vDeltaPerSecond = (vCurPos - m_vLastTickPos) * 1000 / (float)dwTickTime;
	}

	m_vLastTickPos = vCurPos;

	if(m_pEventInstance)
		m_pEventInstance->Set3DAttributes(A3DVECTOR2AudioVECTOR(vCurPos)
		, A3DVECTOR2AudioVECTOR(vDeltaPerSecond));
	A3DQUATERNION q = pSound->GetGfx()->GetDir();
	A3DMATRIX4 mat;
	mat.Identity();
	q.ConvertToMatrix(mat);
	if(m_pEventInstance)
		m_pEventInstance->Set3DConeOrientation(A3DVECTOR2AudioVECTOR(mat.GetRow(2)));
	m_vLastTickPos = vCurPos;

#endif
	return true;
}

A3DVECTOR3 GFXSOUNDIMP22::GetCurPos(A3DGFXSound* pSound)
{
	KEY_POINT kp;
	if(!pSound->GetCurKeyPoint(&kp))
		kp.m_vPos.Clear();
	return pSound->GetParentTM() * kp.m_vPos;
}

void GFXSOUNDIMP22::ResumeLoop()
{

}

bool GFXSOUNDIMP22::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch (nOp)
	{
	case ID_GFXOP_SOUND_FILE:
		m_strEventPath = (AString)prop; 
		break;
	case ID_GFXOP_SOUND_MIN_DIST:
		m_fMinDist = (float)prop;
		break;
	case ID_GFXOP_SOUND_MAX_DIST:
		m_fMaxDist = (float)prop;
		break;
	case ID_GFXOP_SOUND_USECUSTOM:
		m_bUseCustom = (bool)prop;
		break;
	default:
		return false;
	}
	return true;
}

bool GFXSOUNDIMP22::GetProperty(int nOp, GFX_PROPERTY& prop) const
{
	switch (nOp)
	{
	case ID_GFXOP_SOUND_FILE:
		prop = GFX_PROPERTY(m_strEventPath).SetType(GFX_VALUE_PATH_AUDIOEVENT);
		return true;
	case ID_GFXOP_SOUND_MIN_DIST:
		prop = GFX_PROPERTY(m_fMinDist);
		return true;
	case ID_GFXOP_SOUND_MAX_DIST:
		prop = GFX_PROPERTY(m_fMaxDist);
		return true;
	case ID_GFXOP_SOUND_USECUSTOM:
		prop = GFX_PROPERTY(m_bUseCustom);
		return true;
	default:
		return false;
	}
}

bool GFXSOUNDIMP22::OnStop( AudioEngine::EventInstance* pEventInstance )
{
#ifdef _USEAUDIOENGINE
	CSController csc(&m_csForThreadSafe);
	if (m_pEventInstance)
	{
		AudioEngine::Event* pEvent = m_pEventInstance->GetEvent();
		if(!pEvent)
			return false;
		CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
		CriticalSectionController cscei(m_pEventInstance->GetCriticalSectionWrapper());
		m_pEventInstance->RemoveListener(this);
		AfxGetGFXExMan()->GetGfxInterface()->ReleaseAudioEventInstance(m_pEventInstance);
		m_pEventInstance = NULL;
	}
#endif
	return true;
}