/*
 * FILE: A3DSkinModelAct.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/6/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "A3DSkinModelAct.h"
#include "A3DPI.h"
#include "A3DSkinModel.h"
#include "A3DSkeleton.h"
#include "A3DJoint.h"
#include "A3DTrackMan.h"
#include "A3DBone.h"
#include "A3DFuncs.h"
#include "A3DAbsTrackMaker.h"
#include "AAssist.h"
#include "AMemory.h"
#include "AFile.h"
#include "AFI.h"
#include "ATime.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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

//	Action data in file. when SKMDFILE_VERSION < 6
struct FILEACTIONDATA
{
	int			iActGroup;		//	Action group ID
	int			iNumJoint;		//	Number of joint
	DWORD		dwFlags;		//	Action flags
};

//	Action data in file. when SKMDFILE_VERSION >= 6 && < 9
struct FILEACTIONDATA6
{
	float		fStartFrame;	//	Start frame
	float		fEndFrame;		//	End frame
};

//	Action data in file. when SKMDFILE_VERSION >= 9
struct FILEACTIONDATA9
{
	float		fStartFrame;	//	Start frame
	float		fEndFrame;		//	End frame
	int			iFrameRate;		//  the frame rate of this action

	FILEACTIONDATA9()
	{
		iFrameRate = 0;
	}

};


//	Joint action data in file
struct FILEJOINTACTION
{
	int			iJoint;			//	Joint's index
	float		fStartFrame;	//	Start frame
	float		fEndFrame;		//	End frame
	int			iNumLoop;		//	Loop number, -1 means loop forever
	float		fSpeedFactor;	//	Speed factor
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DSkinModelActionCore
//
///////////////////////////////////////////////////////////////////////////

A3DSkinModelActionCore::A3DSkinModelActionCore(int iFrameRate)
{
	m_dwClassID		= A3D_CID_SKINMODELACTIONCORE;
	m_fStartFrame	= 0.0f;
	m_fEndFrame		= 0.0f;
	m_dwStartTime	= 0;
	m_dwEndTime		= 0;
	m_dwNameID		= 0;
	m_iFrameRate	= iFrameRate;
	m_dwActTime		= 0;
	m_dwTckNameID	= 0;
	m_pTrackSet		= NULL;
	m_bOwnTckFile	= false;
	m_iRefCnt		= 0;
	m_dwRefTime		= 0;
	
	memset(m_aRanks, 0, sizeof (m_aRanks));
}

A3DSkinModelActionCore::~A3DSkinModelActionCore()
{
	//	Release track set
	if (m_pTrackSet && m_dwTckNameID && m_bOwnTckFile)
	{
		A3D::g_A3DTrackMan.ReleaseSkeletonTrackSet(m_dwTckNameID, m_strTrackName);
		m_pTrackSet = NULL;
	}

	if (m_iRefCnt)
	{
		ASSERT(!m_iRefCnt);
	}
}

//	Set name
void A3DSkinModelActionCore::SetName(const char* szName)
{
	ASSERT(szName);
	m_strName	= szName;
	m_dwNameID	= a_MakeIDFromLowString(szName);
}

//	Set start and end frame
void A3DSkinModelActionCore::SetStartAndEndFrame(float fStartFrame, float fEndFrame)
{
	m_fStartFrame	= fStartFrame;
	m_fEndFrame		= fEndFrame;
	m_dwStartTime	= FrameToTime(fStartFrame, m_iFrameRate);
	m_dwEndTime		= FrameToTime(fEndFrame, m_iFrameRate);
	m_dwActTime		= m_dwEndTime - m_dwStartTime + 1;
}

//	Load model from file
bool A3DSkinModelActionCore::Load(AFile* pFile, DWORD dwVer, const char* szTckDir)
{
	//	Read action name
	pFile->ReadString(m_strName);
	m_dwNameID = a_MakeIDFromLowString(m_strName);

	if (dwVer < 6)
	{
		//	Read action data
		FILEACTIONDATA ad;
		DWORD dwRead;

		if (!pFile->Read(&ad, sizeof (ad), &dwRead) || dwRead != sizeof (ad))
		{
			g_A3DErrLog.Log("A3DSkinModelActionCore::Load, Failed to read action data");
			return false;
		}

		//	Read joint action data
		if (ad.iNumJoint >= 1)
		{
			//	Read the first joint-act data and use it's data as action's data
			FILEJOINTACTION Src;
			if (!pFile->Read(&Src, sizeof (Src), &dwRead) || dwRead != sizeof (Src))
			{
				g_A3DErrLog.Log("A3DSkinModelActionCore::Load, Failed to load joint data");
				return false;
			}

			SetStartAndEndFrame(Src.fStartFrame, Src.fEndFrame);

			//	Skip left joint action data
			if (ad.iNumJoint > 1)
			{
				pFile->Seek((ad.iNumJoint - 1) * sizeof (FILEJOINTACTION), AFILE_SEEK_CUR);
			}
		}
		else
		{
			//	This is a very special case
			SetStartAndEndFrame(0.0f, 0.0f);
		}
	}
	else if(dwVer < 9)	
	{
		//	Read action data
		FILEACTIONDATA6 ad;
		DWORD dwRead;

		if (!pFile->Read(&ad, sizeof (ad), &dwRead) || dwRead != sizeof (ad))
		{
			g_A3DErrLog.Log("A3DSkinModelActionCore::Load, Failed to read action data");
			return false;
		}

		SetStartAndEndFrame(ad.fStartFrame, ad.fEndFrame);
	}
	else		// dwVer >= 9
	{
		//	Read action data
		FILEACTIONDATA9 ad;
		DWORD dwRead;

		if (!pFile->Read(&ad, sizeof (ad), &dwRead) || dwRead != sizeof (ad))
		{
			g_A3DErrLog.Log("A3DSkinModelActionCore::Load, Failed to read action data");
			return false;
		}

		if(ad.iFrameRate > 0) m_iFrameRate = ad.iFrameRate;
		SetStartAndEndFrame(ad.fStartFrame, ad.fEndFrame);
		
	}

	//	Load track set file name ...
	if (dwVer >= 7)
	{
		//	Read track file relative to skin model file
		AString str;
		pFile->ReadString(str);

		if (szTckDir[0])
		{
			str = "\\" + str;
			str = szTckDir + str;
		}
		
		SetTrackSetFileName(str, true);
	}
	else
		m_bOwnTckFile = false;

	return true;
}

//	Save model from file
bool A3DSkinModelActionCore::Save(AFile* pFile)
{
	//	Write action name
	pFile->WriteString(m_strName);

	FILEACTIONDATA9 ad;
	memset(&ad, 0, sizeof (ad));

	ad.fStartFrame	= m_fStartFrame;
	ad.fEndFrame	= m_fEndFrame;
	ad.iFrameRate	= m_iFrameRate;

	DWORD dwWrite;
	if (!pFile->Write(&ad, sizeof (ad), &dwWrite) || dwWrite != sizeof (ad))
	{
		g_A3DErrLog.Log("A3DSkinModelActionCore::Save, Failed to save action data");
		return false;
	}

	if (af_CheckFileExt(m_strTrackName, ".stck"))
	{
		//	Save track set file name (not including path) !
		AString strTitle;
		af_GetFileTitle(m_strTrackName, strTitle);
		pFile->WriteString(strTitle);
	}
	else
	{
		//	This shouldn't happen when SKMDFILE_VERSION >= 7
		ASSERT(0);
		pFile->WriteString("");
	}

	return true;
}

//	Skip action data in file
bool A3DSkinModelActionCore::SkipFileData(AFile* pFile, DWORD dwVer)
{
	AString str;

	//	Skip action name
	pFile->ReadString(str);

	if (dwVer < 6)
	{
		//	Skip action data
		FILEACTIONDATA ad;
		DWORD dwRead;

		if (!pFile->Read(&ad, sizeof (ad), &dwRead) || dwRead != sizeof (ad))
		{
			g_A3DErrLog.Log("A3DSkinModelActionCore::Load, Failed to read action data");
			return false;
		}

		//	Skip joint action data
		pFile->Seek(ad.iNumJoint * sizeof (FILEJOINTACTION), AFILE_SEEK_CUR);
	}
	else	//	dwVer >= 6
	{
		//	Skip action data
		FILEACTIONDATA6 ad;
		DWORD dwRead;

		if (!pFile->Read(&ad, sizeof (ad), &dwRead) || dwRead != sizeof (ad))
		{
			g_A3DErrLog.Log("A3DSkinModelActionCore::Load, Failed to read action data");
			return false;
		}
		
		if (dwVer >= 7)
		{
			//	Skip track set file name ...
			pFile->ReadString(str);
		}
	}

	return true;
}

//	Convert frame to time
DWORD A3DSkinModelActionCore::FrameToTime(float fFrame, int iFrameRate)
{
	if (iFrameRate <= 0)
		return 0;
	else
		return (DWORD)(fFrame * 1000.0f / iFrameRate);
}

//	Set track set name
void A3DSkinModelActionCore::SetTrackSetFileName(const char* szFileName, bool bOwnTckFile)
{
	m_strTrackName	= szFileName;
	m_dwTckNameID	= a_MakeIDFromLowString(szFileName);
	m_bOwnTckFile	= bOwnTckFile;
}

//	Try to bind track set
bool A3DSkinModelActionCore::BindTrackSet()
{
	m_dwRefTime = a_GetTime();

	if (!m_dwTckNameID)
		return false;

	if (!m_bOwnTckFile)
	{
		//	Track set is created when skeleton loaded and is released when
		//	skeleton released, so use GetSkeletonTrackSet() to avoid
		//	changing track set's reference counter
		m_pTrackSet = A3D::g_A3DTrackMan.GetSkeletonTrackSet(m_dwTckNameID, m_strTrackName);
	}
	else
	{
		//	Below 'if (m_pTrackSet)' statement is necessary, because CreateSkeletonTrackSet()
		//	will increase track set's reference counter.
		if (m_pTrackSet)
			return true;

		bool bNewTrackSet;
		m_pTrackSet = A3D::g_A3DTrackMan.CreateSkeletonTrackSet(m_dwTckNameID, 
								m_strTrackName, false, bNewTrackSet);
	}
	
	if (!m_pTrackSet)
	{
	//	ASSERT(0);
		return false;
	}

	return true;
}

//	Unbind track set
void A3DSkinModelActionCore::UnbindTrackSet()
{
	if (!m_bOwnTckFile)
	{
		ASSERT(m_bOwnTckFile);
		return;
	}

	if (m_pTrackSet)
	{
		A3D::g_A3DTrackMan.ReleaseSkeletonTrackSet(m_dwTckNameID, m_strTrackName);
		m_pTrackSet = NULL;
	}
}

//	Get position and orientation of specified time
bool A3DSkinModelActionCore::GetBoneStateOnTrack(int idJoint, int iAbsTime, 
					A3DVECTOR3& vPos, A3DQUATERNION& quRot, int* pPosRefKeyLoc /*= NULL*/, int* pQuatRefKeyLoc /*= NULL*/)
{
	if (!m_pTrackSet)
	{
		ASSERT(m_pTrackSet);
		return false;
	}
	
	A3DSklTrackSet::BONETRACK* pBoneTrack = m_pTrackSet->GetBoneTrack(idJoint);
	if (!pBoneTrack)
	{
		ASSERT(pBoneTrack);
		return false;
	}

	vPos = pBoneTrack->pPosTrack->GetKeyValue(iAbsTime, pPosRefKeyLoc);
	quRot = pBoneTrack->pRotTrack->GetKeyValue(iAbsTime, pQuatRefKeyLoc);
	quRot.Normalize();

	return true;
}

bool A3DSkinModelActionCore::GetBoneStateOnAbsTrack(int idJoint, int iAbsTime, 
					A3DVECTOR3& vPos, A3DQUATERNION& quRot, int* pPosRefKeyLoc /*= NULL*/, int* pQuatRefKeyLoc /*= NULL*/)
{
	if (!m_pTrackSet)
	{
		ASSERT(m_pTrackSet);
		return false;
	}
	
	A3DSklTrackSet::BONETRACK* pBoneTrack = m_pTrackSet->GetBoneTrack(idJoint);
	if (!pBoneTrack || !pBoneTrack->pAbsPosTrack || !pBoneTrack->pAbsRotTrack)
	{
		ASSERT(pBoneTrack);
		return false;
	}

	vPos = pBoneTrack->pAbsPosTrack->GetKeyValue(iAbsTime, pPosRefKeyLoc);
	quRot = pBoneTrack->pAbsRotTrack->GetKeyValue(iAbsTime, pQuatRefKeyLoc);
	quRot.Normalize();

	return true;
}

//	On start playing
void A3DSkinModelActionCore::OnStartPlaying()
{
	m_iRefCnt++;
}

//	On stop playing
void A3DSkinModelActionCore::OnStopPlaying()
{
	m_iRefCnt--;
	ASSERT(m_iRefCnt >= 0);
}

//	Check whether action can be removed from cache
bool A3DSkinModelActionCore::IsOutOfCacheDate(DWORD dwCurTime)
{
	if (!m_pTrackSet || !m_bOwnTckFile || m_iRefCnt > 0)
		return false;

	static const DWORD dwLifeTime = 300000;
	if (m_dwRefTime + dwLifeTime > dwCurTime)
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DSkinModelAction
//
///////////////////////////////////////////////////////////////////////////

A3DSkinModelAction::A3DSkinModelAction(A3DSkinModel* pSkinModel, A3DSkinModelActionCore* pCore)
{ 
	m_pA3DSkinModel	= pSkinModel;
	m_pCore			= pCore;
	m_pChannel		= NULL;
	m_iPlayCnt		= 0;
	m_dwTimeCnt		= 0;
	m_fDeltaTime	= 0.0f;
	m_iLoopTime		= 0;
	m_iLoopCnt		= 0;
	m_bStop			= true;
	m_bStopNextTick	= false;
	m_dwCurTime		= 0;
	m_dwTransTime	= 0;
	m_bAbsTrack		= false;

	m_pTrackBoneKeyLocCaches = NULL;
}

A3DSkinModelAction::~A3DSkinModelAction()
{
	if (!m_bStop)
	{
		//	Force stop action so that m_pCore's reference
		//	counter works right
		OnStop(STOP_FORCED);
	}
}

/*	Play action

	pChannel: action channel object
	iTransTime: Bone transition time (ms). Give some time to bones to do transition
			can make model action change more smoothly
	iNumLoop: number of loop, -1 means play forever
	bAbsTrack: true, use absolute track when necessary
*/
void A3DSkinModelAction::Play(A3DSMActionChannel* pChannel, int iTransTime, int iNumLoop, bool bAbsTrack)
{
	if (!pChannel || !iNumLoop || !m_pA3DSkinModel || !m_pA3DSkinModel->GetSkeleton())
		return;

	int iNumBone = pChannel->GetBoneNum();
	if (!iNumBone)
		return;

	//	Prepare bone to do transition if necessary
	if (iTransTime > 0)
		pChannel->PrepareBoneTransition(iTransTime);

	//	Notify action core
	m_pCore->OnStartPlaying();

	m_pChannel		= pChannel;
	m_iLoopTime		= iNumLoop;
	m_iLoopCnt		= iNumLoop;
	m_bStop			= false;
	m_bStopNextTick	= false;
	m_dwCurTime		= m_pCore->GetStartTime();
	m_dwTimeCnt		= 0;
	m_fDeltaTime	= 0.0f;
	m_dwTransTime	= (DWORD)iTransTime;
	m_bAbsTrack		= bAbsTrack;

	if(m_pTrackBoneKeyLocCaches) delete [] m_pTrackBoneKeyLocCaches;
	m_pTrackBoneKeyLocCaches = new TrackBoneKeyLocCache[iNumBone];
}

//	Stop action
void A3DSkinModelAction::Stop()
{
	OnStop(STOP_FORCED);
}

//	Update routine
bool A3DSkinModelAction::Update(int iDeltaTime, bool bApplyToBone/* true */)
{
	if (!m_pChannel)
		return false;

	if (!m_pCore->GetActionTime())
	{
		ASSERT(m_pCore->GetActionTime());
		return false;
	}

	if (IsStopped())
		return true;
		
//	m_fDeltaTime += pJointAct->fSpeedFactor * iDeltaTime;
	m_fDeltaTime += 1.0f * iDeltaTime;
	iDeltaTime = (int)m_fDeltaTime;
	m_fDeltaTime -= (float)iDeltaTime;

	DWORD dwAbsTime;

	//	Stop this tick ? m_bStopNextTick is always set in last tick
	if (m_bStopNextTick)
	{
		//	if this action is played exclusively, prepare to do a animation transition
		if (m_pChannel->GetPlayMode() == A3DSMActionChannel::PLAY_EXCLUSIVE && m_dwTransTime)
			m_pChannel->PrepareBoneTransition((int)m_dwTransTime);

		m_bStopNextTick = false;
		OnStop(STOP_FINISH);
		return true;
	}
	else
	{
		dwAbsTime = m_dwCurTime + iDeltaTime;

		while (dwAbsTime > m_pCore->GetEndTime())
		{
			if (m_iLoopCnt > 0)
				m_iLoopCnt--;

			if (!m_iLoopCnt)
			{
				m_bStopNextTick = true;		//	Stop next tick
				dwAbsTime = m_pCore->GetEndTime();	//	Keep last moment's state
				break;
			}
			else if (m_pCore->GetActionTime() <= 1)
			{
				//	m_pCore->GetActionTime() <= 1 means action's 
				//	start time == end time, so we can break quickly 
				//	if loop number < 0
				if (m_iLoopCnt < 0)
				{
					dwAbsTime = m_pCore->GetEndTime();
					break;
				}
			}
			else
				dwAbsTime -= m_pCore->GetActionTime();
		}
	}

	m_dwCurTime = dwAbsTime;

	//	Update joint action
	if (bApplyToBone)
		UpdateJointActions(dwAbsTime);

	m_dwTimeCnt += iDeltaTime;

	return true;
}

#define USE_KEY_LOCATION_CACHE 1
//	Update joint action
void A3DSkinModelAction::UpdateJointActions(DWORD dwAbsTime)
{
	int i, iNumBone = m_pChannel->GetBoneNum();
	float fWeight = m_pChannel->GetWeight();
	int iPlayMode = m_pChannel->GetPlayMode();
	AArray<int, int>& aAbsTracks = m_pA3DSkinModel->GetAbsTracks();

	A3DVECTOR3 vPos;
	A3DQUATERNION quRot;

	for (i=0; i < iNumBone; i++)
	{

		int iBone = m_pChannel->GetBoneIndex(i);

#if USE_KEY_LOCATION_CACHE

		int* pPosKeyLocCache = (m_pTrackBoneKeyLocCaches)?(&m_pTrackBoneKeyLocCaches[i].PosKeyLoc) : NULL;
		int* pQuatKeyLocCache = (m_pTrackBoneKeyLocCaches)?(&m_pTrackBoneKeyLocCaches[i].QuatKeyLoc) : NULL;
#else

		int* pPosKeyLocCache = NULL;
		int* pQuatKeyLocCache = NULL;

#endif
		
		A3DBone* pBone = m_pA3DSkinModel->GetSkeleton()->GetBone(iBone);
		if (!pBone || !pBone->IsAnimDriven())
			continue;

		int iJoint = pBone->GetFirstJoint();
		bool bUseAbsTrack = false;

		if (m_bAbsTrack && aAbsTracks.Find(iBone) >= 0)
		{
			//	Try to use absolute track
			if (m_pCore->GetBoneStateOnAbsTrack(iJoint, (int)dwAbsTime, vPos, quRot, pPosKeyLocCache, pQuatKeyLocCache))
				bUseAbsTrack = true;
		}

		if (bUseAbsTrack)
		{
			//	Use special play mode: BM_ABSTRACK !
			pBone->AddBlendState(quRot, vPos, fWeight, A3DBone::BM_ABSTRACK);
		}
		else
		{
			//	Update position and orientation
			m_pCore->GetBoneStateOnTrack(iJoint, (int)dwAbsTime, vPos, quRot, pPosKeyLocCache, pQuatKeyLocCache);
			
			//	See if is a combine action or a blend action
			if (iPlayMode == A3DSMActionChannel::PLAY_COMBINE)
			{
				A3DVECTOR3 vPos0;
				A3DQUATERNION quRot0;
				m_pCore->GetBoneStateOnTrack(iJoint, m_pCore->GetStartTime(), vPos0, quRot0);

				vPos = vPos - vPos0;
				quRot = quRot0.Conjugate() * quRot;
			}
		
			pBone->AddBlendState(quRot, vPos, fWeight, iPlayMode);
		}
	}
}

//	When action stopped
void A3DSkinModelAction::OnStop(int iReason)
{
	//	Check stop flag so that m_pCore's reference 
	//	counter works right
	if (m_bStop)
		return;

	//	Notify action core
	m_pCore->OnStopPlaying();

	m_bStop		= true;
	m_pChannel	= NULL;

	if(m_pTrackBoneKeyLocCaches)
	{
		delete [] m_pTrackBoneKeyLocCaches;
		m_pTrackBoneKeyLocCaches = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DSMActionChannel
//
///////////////////////////////////////////////////////////////////////////

A3DSMActionChannel::A3DSMActionChannel(A3DSkinModel* pSkinModel, int iChannel)
{
	m_pA3DSkinModel	= pSkinModel;
	m_fWeight		= 1.0f;
	m_iChannel		= iChannel;
	m_iPlayMode		= PLAY_NORMAL;
}

A3DSMActionChannel::~A3DSMActionChannel()
{
	StopAction();
}

//	Build joint index array
void A3DSMActionChannel::BuildBoneIndices(int iNumBone, int* aBones)
{
	//	Remove old joint indices
	m_aBones.RemoveAll();

	if (iNumBone && aBones)
	{
		m_aBones.SetSize(iNumBone, 10);

		for (int i=0; i < iNumBone; i++)
			m_aBones[i] = aBones[i];
	}
}

//	Get action who has the top rank
A3DSkinModelAction* A3DSMActionChannel::GetTopRankAction(int* piRank)
{
	if (!m_ActRankList.GetCount())
		return NULL;

	ACT_NODE* pNode = m_ActRankList.GetHead();
	if (piRank)
		*piRank = pNode->iRank;

	return pNode->pAction;
}

/*	Add action to rank list

	Return value:

		0: a same name action is being played, new action should be ignored
		1: new action is added to list and it has a new rank
		2: new action is added to list and it replaces an old action of same rank
*/
int A3DSMActionChannel::AddActionToRankList(A3DSkinModelAction* pAction, bool bRestart)
{
	ASSERT(pAction);

	//	Get new action's rank on this channel
	int iRank = pAction->GetCoreData()->GetRank(m_iChannel);

	ALISTPOSITION pos = m_ActRankList.GetHeadPosition();
	while (pos)
	{
		ALISTPOSITION posCur = pos;
		ACT_NODE* pNode = m_ActRankList.GetNext(pos);

		if (iRank > pNode->iRank)
		{
			ACT_NODE* pNewNode = new ACT_NODE;
			pNewNode->iRank = iRank;
			pNewNode->pAction = pAction;
			m_ActRankList.InsertBefore(posCur, pNewNode);
			return 1;
		}
		else if (iRank == pNode->iRank)
		{
			//	Check name
			if (!bRestart && !_stricmp(pNode->pAction->GetCoreData()->GetName(), pAction->GetCoreData()->GetName()))
				return 0;	//	Action is being played

			//	Replace old action of same rank
			StopAction(pNode->pAction);

			pNode->iRank = iRank;
			pNode->pAction = pAction;
			return 2;
		}
	}

	//	Add new action at list tail
	ACT_NODE* pNewNode = new ACT_NODE;
	pNewNode->iRank = iRank;
	pNewNode->pAction = pAction;
	m_ActRankList.AddTail(pNewNode);

	return 1;
}

//	Play action
bool A3DSMActionChannel::PlayActionByName(const char* szName, int iNumLoop/* 1 */, 
						int iTransTime/* 200 */, bool bRestart/* true */, bool bAbsTrack/* false */)
{
	if (!m_pA3DSkinModel)
		return false;

	A3DSkinModelActionCore* pActCore = m_pA3DSkinModel->GetAction(szName);
	if (!pActCore || !pActCore->BindTrackSet())
		return false;

	//	Build absolute track if necessary
	if (bAbsTrack && m_pA3DSkinModel->GetAbsTrackNum())
	{
		A3DAbsTrackMaker maker(m_pA3DSkinModel->GetSkeleton(), pActCore->GetTrackSet());
		AArray<int, int>& aAbsTracks = m_pA3DSkinModel->GetAbsTracks();
		maker.Make(aAbsTracks.GetSize(), aAbsTracks.GetData());
	}

	//	Create new action object
	A3DSkinModelAction* pNewAction = new A3DSkinModelAction(m_pA3DSkinModel, pActCore);
	if (!pNewAction)
		return false;
	
	//	Add new action to rank list
	int ret = AddActionToRankList(pNewAction, bRestart);
	if (ret == 0)
	{
		delete pNewAction;
		return true;
	}

	//	Get new action isn't top action, transition time should be clear
	A3DSkinModelAction* pTopAction = GetTopRankAction(NULL);
	if (pNewAction != pTopAction)
		iTransTime = 0;

	pNewAction->Play(this, iTransTime, iNumLoop, bAbsTrack);

	return true;
}

//	Stop action of specified rank.
//	iRank == -1 means stop all actions
void A3DSMActionChannel::StopAction(int iRank/* -1 */)
{
	if (iRank < 0)
	{
		ALISTPOSITION pos = m_ActRankList.GetHeadPosition();
		while (pos)
		{
			ACT_NODE* pNode = m_ActRankList.GetNext(pos);
			StopAction(pNode->pAction);
			delete pNode;
		}

		m_ActRankList.RemoveAll();
	}
	else
	{
		ALISTPOSITION pos = m_ActRankList.GetHeadPosition();
		while (pos)
		{
			ALISTPOSITION posCur = pos;
			ACT_NODE* pNode = m_ActRankList.GetNext(pos);
			if (pNode->iRank == iRank)
			{
				StopAction(pNode->pAction);
				delete pNode;
				m_ActRankList.RemoveAt(posCur);
				break;
			}
		}
	}
}

//	Stop specified action
void A3DSMActionChannel::StopAction(A3DSkinModelAction* pAction)
{
	if (pAction)
	{
		//	Must call Stop() so that action core's reference 
		//	counter can works right
		pAction->Stop();
		delete pAction;
	}
}

//	Update
bool A3DSMActionChannel::Update(int iDeltaTime)
{
	//	Update top rank action at first
	ALISTPOSITION pos = m_ActRankList.GetHeadPosition();
	if (!pos)
		return true;

	ACT_NODE* pNode = m_ActRankList.GetNext(pos);
	pNode->pAction->Update(iDeltaTime);

	bool bRankTransit = false;

	if (pNode->pAction->IsStopped())
	{
		bRankTransit = true;

		//	Remove stopped top action
		delete pNode->pAction;
		delete pNode;
		m_ActRankList.RemoveHead();

		pos = m_ActRankList.GetHeadPosition();
	}

	//	Update other actions but don't apply result to bones
	while (pos)
	{
		ALISTPOSITION posCur = pos;
		pNode = m_ActRankList.GetNext(pos);
		pNode->pAction->Update(iDeltaTime, false);

		if (pNode->pAction->IsStopped())
		{
			delete pNode->pAction;
			delete pNode;
			m_ActRankList.RemoveAt(posCur);
		}
	}

	//	If top rank action stops and a lower rank action exists, transit to it
	if (bRankTransit && m_ActRankList.GetHeadPosition())
		PrepareBoneTransition(200);

	return true;
}

//	Prepare bone transition
void A3DSMActionChannel::PrepareBoneTransition(int iTransTime)
{
	if (iTransTime <= 0)
		return;

	if (!m_pA3DSkinModel || !m_pA3DSkinModel->GetSkeleton())
		return;

	A3DSkeleton* pSkeleton = m_pA3DSkinModel->GetSkeleton();

	for (int i=0; i < m_aBones.GetSize(); i++)
	{
		A3DBone* pBone = pSkeleton->GetBone(m_aBones[i]);
		if (pBone)
			pBone->PrepareTransition(iTransTime);
	}
}

