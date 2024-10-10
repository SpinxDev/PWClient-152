#include "stdafx.h"
#include "A3DCombActDynData.h"
#include "A3DCombinedAction.h"
#include "EC_Model.h"

#ifdef _ANGELICA21
#include "EC_AnimNodeMan.h"
#include <A3DAnimNode.h>
#endif

//////////////////////////////////////////////////////////////////////////
//
//	Implememnt A3DCombinedAction
//
//////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4533)

int A3DCombActDynData::s_CombActFxArrayCount = 0;

A3DCombActDynData::A3DCombActDynData(A3DCombinedAction* pAct, CECModel* pECModel) :
m_pAct(pAct),
m_pECModel(pECModel),
m_nCurLoop(0),
m_nCurActLoop(0),
m_dwTimeSpan(0),
m_fWeight(1.0f),
m_nTransTime(200),
m_dwUserData(0),
m_bAbsTrack(false),
m_bNoFx(false),
m_bNoCamShake(false),
m_CurEventPos(0),
m_pParentInfo(0),
m_pParentModel(0),
m_bStopPrevAct(false),
m_dwDeltaTime(0),
m_nChannel(0),
m_dwEventMask(-1),
m_idCaster(0),
m_idCastTarget(0),
m_ptFixed(0),
m_SerialId(0),
m_IsAttackAct(false),
m_nCurActIndex(-1),
m_dwDynComActSpan(0),
m_bIsInfiniteComAct(false),
m_bResetPSWhenActionStop(false),
m_bSetSpeedWhenActStart(false),
m_fModelScale(1.0f)
{
	ActInfoList& actInfoList = pAct->m_ActLst;
	ALISTPOSITION pos = actInfoList.GetHeadPosition();
	while (pos)
	{
		PACTION_INFO pAct = actInfoList.GetNext(pos);
		int nDynLoopNum = pAct->CalcLoopNum();
		m_arrActLoopNum.Add(ACTIONDYN_DATA(nDynLoopNum, pAct));
		if (!m_bIsInfiniteComAct && nDynLoopNum < 0) m_bIsInfiniteComAct = true;
	}
	CalcDynComActSpan();
}

A3DCombActDynData::~A3DCombActDynData() 
{ 
	Release(); 
}

// A3DCombActDynData

void A3DCombActDynData::Play(int nChannel, float fWeight, int nTransTime, DWORD dwEventMask, bool bRestart, bool bAbsTrack, bool bNoFx, bool bNoCamShake)
{
	m_nChannel = nChannel;
	m_fWeight = fWeight;
	m_nTransTime = nTransTime;
	m_dwEventMask = dwEventMask;
	m_bAbsTrack = bAbsTrack;
	m_bNoFx = bNoFx;
	m_bNoCamShake = bNoCamShake;

	if (bRestart)
		m_nCurLoop = 0;

	m_fModelScale = m_pECModel->GetAllRootBonesScale();

	Resume();
	UpdateAct(0);
#ifdef _ANGELICA21
	UpdateEvent(0, 0);
#endif
}

void A3DCombActDynData::UpdateEvent(DWORD dwActTime, DWORD dwEventTime)
{
#ifndef GFX_EDITOR

	if (m_pAct->m_nEventChannel && m_pAct->m_nEventChannel != m_nChannel)
		return;

#endif

#ifdef _ANGELICA21
	if (m_bNoFx)
		return;
#else
	if (m_bNoFx || (!dwActTime && !dwEventTime))
		return;
#endif

	ExpireActFx();

	ALISTPOSITION posNext;
	EventInfoList& InfoList = m_pAct->m_EventInfoLst;

	if (m_CurEventPos == NULL)
		posNext = InfoList.GetHeadPosition();
	else
	{
		posNext = m_CurEventPos;
		InfoList.GetNext(posNext);
	}

	PACTION_INFO pCur;

	if (m_nCurActIndex >= 0 && m_nCurActIndex < m_arrActLoopNum.GetSize())
		pCur = m_arrActLoopNum[m_nCurActIndex].GetActInfo();
	else
		pCur = NULL;

	while (posNext)
	{
		ALISTPOSITION posPrev = posNext;
		EVENT_INFO* pNext = InfoList.GetNext(posNext);

		if (((1 << (pNext->GetType() - EVENT_TYPE_BASE)) & m_dwEventMask) == 0)
			continue;

		if (pNext->IsOnce() && pCur && m_nCurActLoop > 0)
			continue;

		// 从m_CurEventPos开始找，找到所有能够满足下面条件的Event
		// Event的起始时间正处在当前帧的时间间隔内
		if (m_dwTimeSpan >= pNext->GetStartTime() && m_dwTimeSpan <= pNext->GetStartTime() + dwActTime)
		{
			m_CurEventPos = posPrev;
			bool bStart = true;

			// 如果这个Event已经在播放的event中了，则跳过播放
			for (size_t i = 0; i < m_ActFxArray.size(); i++)
			{
				if (m_ActFxArray[i]->GetInfo() == pNext)
				{
					bStart = false;
					break;
				}
			}

			if (bStart)
				pNext->Start(this);
		}

		//	LoopCurAct 中将 m_CurEventPos 置空了，这个循环会从头开始遍历EventList，这样会导致不应该break的时候错误跳出的情形
		//else
		//	break;
	}

	UpdateFxParam(dwEventTime);
}

void A3DCombActDynData::UpdateAct(DWORD dwDeltaTime)
{
	m_dwTimeSpan += dwDeltaTime;

	// 如果是父模型动作上的“子模型动作”出发的本动作
	// 如果该“子模型动作”不是刀光
	// 如果该“子模型动作”的刀光时间不为0
	// 并且如果该动作已经Tick到了刀光定义的时间之后
	if (m_pParentInfo 
		&& !m_pParentInfo->IsTrailAct()
		&& m_pParentInfo->GetTrailTmSpan() > 0
		&& m_pParentInfo->GetStartTime() + m_pParentInfo->GetTrailTmSpan() < m_dwTimeSpan) 
	{
		// After stop channel action, we must return immediately, for this object is already deleted
		m_pECModel->StopChannelAction(m_nChannel, true, true);
		return;
	}

	if (IsActionStopped())
		return;

	PACTION_INFO pCur = NULL, pNext = NULL;
	if (m_arrActLoopNum.GetSize() == 0)
		return;

	if (m_nCurActIndex == -1)									// first
		pNext = m_arrActLoopNum[0].GetActInfo();
	else if (m_nCurActIndex == m_arrActLoopNum.GetSize() - 1)	// last
		pCur = m_arrActLoopNum[m_nCurActIndex].GetActInfo();
	else														// in middle
	{
		pCur = m_arrActLoopNum[m_nCurActIndex].GetActInfo();
		pNext = m_arrActLoopNum[m_nCurActIndex + 1].GetActInfo();
	}

	if (pCur)
	{
		if (m_dwTimeSpan >= pCur->GetEndTime()) // 当前动作超时
		{
 			m_nCurActLoop++;

			if (!IsCurActFinished(pCur)) //没有结束则重新循环
			{
				LoopCurAct(pCur, dwDeltaTime);
				return;
			}
			else if (!pNext) // 没有下一个动作
			{
				LoopNext(dwDeltaTime);
				return;
			}
			// else 下一个动作开始
		}
		else // 播放当前动作
		{
			return;
		}
	}

	if (pNext)
	{
		if (m_dwTimeSpan >= pNext->GetStartTime()) // 下一个动作时间到达
		{
			m_nCurActLoop = 0;
			m_nCurActIndex++;
			A3DSkinModel* pSkin = m_pECModel->GetA3DSkinModel();

			if (pSkin)
			{
				A3DSkinModelActionCore* pAction = pSkin->GetAction(pNext->GetName());

				if (pAction)
				{
					pAction->SetRank(m_nChannel, m_pAct->GetRank(m_nChannel));
					assert(pSkin->GetAction(pAction->GetName()));

					if (pSkin->PlayActionByName(pAction->GetName(), m_nChannel, m_arrActLoopNum[m_nCurActIndex].GetLoopNum(), pCur ? 100 : m_nTransTime, true, false, m_bAbsTrack))
						pSkin->GetActionChannel(m_nChannel)->SetWeight(m_fWeight);
				}
			}
		}
	}
	// else empty act, no next, no cur
}

void A3DCombActDynData::LoopCurAct(PACTION_INFO pCur, DWORD dwDeltaTime)
{
	m_dwTimeSpan = pCur->GetStartTime() + m_dwTimeSpan - pCur->GetEndTime(); // 把时间设回去
	m_CurEventPos = NULL;

	size_t i = 0;
	while (i < m_ActFxArray.size())
	{
		FX_BINDING_BASE* pBind = m_ActFxArray[i];

		if (pBind->IsStop())
		{
			m_ActFxArray.erase(m_ActFxArray.begin()+i);
			delete pBind;

			--s_CombActFxArrayCount;
		}
		else
			i++;
	}
}

void A3DCombActDynData::LoopNext(DWORD dwDeltaTime)
{
	m_nCurLoop++;

	if (!IsActionStopped())
	{
		RemoveAllActiveFx();
		m_nCurActIndex = -1;
		Play(m_nChannel, m_fWeight, m_nTransTime, m_dwEventMask, false, m_bAbsTrack, m_bNoFx);
	}
}

void A3DCombActDynData::CalcDynComActSpan()
{
	m_dwDynComActSpan = 0;
	int nIndex, nNum = m_arrActLoopNum.GetSize();
	for (nIndex = 0 ; nIndex < nNum ; ++nIndex) {
		if (m_arrActLoopNum[nIndex].GetLoopNum() > 0)
			m_dwDynComActSpan += m_arrActLoopNum[nIndex].GetTotalTime();
		else
			m_dwDynComActSpan += m_arrActLoopNum[nIndex].GetTimeSpan();
	}
}

void A3DCombActDynData::RemoveAllActiveFx(bool bForceStopFx)
{
	m_CurEventPos = NULL;

	for (size_t i = 0; i < m_ActFxArray.size(); i++)
	{
		if (bForceStopFx) m_ActFxArray[i]->Stop();
		delete m_ActFxArray[i];
		--s_CombActFxArrayCount;
	}

	m_ActFxArray.clear();
	ClearDynEventInfo();
}

void A3DCombActDynData::Resume()
{
	m_dwTimeSpan	= 0;
	m_nCurActIndex	= -1;
	m_nCurActLoop	= 0;
	RemoveAllActiveFx();
}

void A3DCombActDynData::Release() 
{ 
	Resume();
	ClearParentInfo();
}

void A3DCombActDynData::SetParentActInfo(const ChildActInfo* pParent, CECModel* pModel, DWORD dwDeltaTime)
{
	ClearParentInfo();
	m_pParentInfo	= (const ChildActInfo*)EVENT_INFO::CloneFrom(NULL, *pParent);
	m_pParentModel	= pModel;
	m_dwDeltaTime	= dwDeltaTime;
}

bool A3DCombActDynData::AddDynEventInfo(const char* szName, const EVENT_INFO* pInfo, bool bIsStart)
{
	EventInfoMap::iterator itr = m_mapDynAddedInfo.find(szName);
	if (itr != m_mapDynAddedInfo.end() && itr->second) {
		itr->second->Start(this);
		return false;
	}

	EVENT_INFO* pEvent = EVENT_INFO::CloneFrom(m_pAct, *pInfo);
	m_mapDynAddedInfo[szName] = pEvent;
	if (bIsStart)
		pEvent->Start(this);
	return true;
}

void A3DCombActDynData::ClearDynEventInfo()
{
	for (EventInfoMap::iterator itr = m_mapDynAddedInfo.begin()
		; itr != m_mapDynAddedInfo.end()
		; ++itr)
	{
		delete itr->second;
	}
	m_mapDynAddedInfo.clear();
}

static bool IsFxEnd(FX_BINDING_BASE* pFx, DWORD dwTimeSpan)
{
	ASSERT(pFx);
	const static DWORD dwMax = 0xFFFFFFFF;
	if (pFx->IsStop())
		return true;

	if (pFx->GetInfo()->GetTimeSpan() == dwMax)
		return false;

	return dwTimeSpan > pFx->GetInfo()->GetStartTime() + pFx->GetInfo()->GetTimeSpan();
}

void A3DCombActDynData::ExpireActFx()
{
	size_t nCount = 0;
	while (nCount < m_ActFxArray.size())
	{
		FX_BINDING_BASE* pFx = m_ActFxArray[nCount];

		if (IsFxEnd(pFx, m_dwTimeSpan))
		{
			m_ActFxArray.erase(m_ActFxArray.begin()+nCount);
			delete pFx;

			--s_CombActFxArrayCount;
		}
		else
			nCount++;
	}
}

void A3DCombActDynData::UpdateFxParam(DWORD dwDeltaTime)
{
	for (size_t i = 0; i < m_ActFxArray.size(); i++)
		m_ActFxArray[i]->UpdateParam(m_pECModel, dwDeltaTime);
}

void A3DCombActDynData::Stop(bool bStopAct, bool bForceStopFx)
{
	FlushDamageInfo();

	ClearParentInfo();
	m_nCurLoop = 0;
	m_nCurActLoop = 0;

	RemoveAllActiveFx(bForceStopFx);
	A3DSkinModel* pSkinModel = m_pECModel->GetA3DSkinModel();

	m_pECModel->GetStaticData()->OnScriptEndAction(m_pECModel, m_nChannel, m_pAct->GetName());

	if (IsResetPSWhenActStop())
	{
		m_pECModel->RemoveReplaceShader();
		SetResetPSWhenActStop(false);
	}

	if (m_pAct->GetResetMaterialScale())
		m_pECModel->ResetMaterialScale();

	if (m_pAct->GetStopChildrenAct())
		m_pECModel->StopChildrenAct();

	if (pSkinModel == NULL || !bStopAct)
		return;

	A3DSMActionChannel* pChannel = pSkinModel->GetActionChannel(m_nChannel);

	if (pChannel)
		pChannel->StopAction(m_pAct->GetRank(m_nChannel));
}

void A3DCombActDynData::Render(A3DViewport* pView)
{
	for (size_t i = 0; i < m_ActFxArray.size(); i++)
		m_ActFxArray[i]->Render(pView);
}

bool A3DCombActDynData::IsAllEventFinished() const 
{ 
#ifdef _ANGELICA21
	if( m_bNoFx )	// 下半身动作不播特效
		return true;
#endif

	return m_ActFxArray.size() == 0 
		&& (m_CurEventPos == m_pAct->m_EventInfoLst.GetTailPosition()
		/*|| m_CurEventPos == NULL*/);
}

void A3DCombActDynData::FlushDamageInfo()
{
	DWORD dwModifier = 0;
	for (size_t i = 0; i < m_TargetsInfo.size(); i++)
	{
		TARGET_DATA& td = m_TargetsInfo[i];
		dwModifier |= td.dwModifier;

		AfxSkillGfxShowDamage(m_idCaster, td.idTarget, td.nDamage, 1, td.dwModifier);
	}

	AfxSkillGfxShowCaster(m_idCaster, dwModifier);

	m_TargetsInfo.clear();
}

void A3DCombActDynData::ClearParentInfo()
{
	delete m_pParentInfo;
	m_pParentInfo = NULL;
}

A3DWalkRunDynData::A3DWalkRunDynData( A3DCombinedAction* pAct, CECModel* pECModel )
	: A3DCombActDynData( pAct, pECModel )
	, m_pWalkRunNode( NULL )
	, m_fTimeSpan(0)
	, m_fActTime(0)
	, m_fEventTime(0)
{
	m_bIsInfiniteComAct = true;
}

A3DWalkRunDynData::~A3DWalkRunDynData()
{
}

void A3DWalkRunDynData::Play( int iChannel, float fWeight, int iTransTime, DWORD dwEventMask, bool bRestart, bool bAbsTrack, bool bNoFx, bool bNoCamShake )
{
#ifdef _ANGELICA21

	m_nChannel		= iChannel;
	m_fWeight		= fWeight;
	m_nTransTime	= iTransTime;
	m_dwEventMask	= dwEventMask;
	m_bAbsTrack		= bAbsTrack;
	m_bNoFx			= bNoFx;

	Resume();

	A3DSkinModel* pSkinModel = m_pECModel->GetA3DSkinModel();
	if( ! pSkinModel )
		return;

	A3DAnimNode* pNode = m_pECModel->GetAnimNodeMan()->GetAnimNode( A3DCombinedAction::SPECIALTYPE_WALKRUN, iChannel );
	m_pWalkRunNode = dynamic_cast< A3DAnimNodeBlendWalkRun* >( pNode );
	if( ! m_pWalkRunNode )
		return;

	bool bRet = pSkinModel->PlayAnimNode( m_pWalkRunNode, iChannel, m_pAct->GetRank( iChannel ), -1, iTransTime, bRestart );
	if( ! bRet )
		return;

	pSkinModel->GetActionChannel( iChannel )->SetWeight( fWeight );

#endif
}

void A3DWalkRunDynData::UpdateAct( DWORD dwDeltaTime )
{
#ifdef _ANGELICA21

	InnerUpdateAct( dwDeltaTime );

#endif
}

void A3DWalkRunDynData::UpdateEvent( DWORD dwActTime, DWORD dwEventTime )
{
#ifdef _ANGELICA21

	if( ! m_pAct )
		return;

	int iEventFromCombAct = m_pAct->GetWalkRunEventFromCombAct();
	if( iEventFromCombAct < 0
		|| !m_pWalkRunNode
		|| iEventFromCombAct >= m_pWalkRunNode->GetChildrenNum() )
	{
		A3DCombActDynData::UpdateEvent( dwActTime, dwEventTime );
		return;
	}

	AnimBlendChild& child = m_pWalkRunNode->GetChild( iEventFromCombAct );
	if (child.mAnimNode)
	{
		float fPlaySpeed = child.mAnimNode->GetPlaySpeed();
		m_fActTime += dwActTime * fPlaySpeed;
		m_fEventTime += dwEventTime * fPlaySpeed;
		dwActTime = static_cast< DWORD >( m_fActTime );
		dwEventTime = static_cast< DWORD >( m_fEventTime );
		m_fActTime -= dwActTime;
		m_fEventTime -= dwEventTime;
	}

	A3DCombActDynData::UpdateEvent( dwActTime, dwEventTime );

#endif
}

bool A3DWalkRunDynData::IsActionStopped() const
{
#ifdef _ANGELICA21
	if( m_pWalkRunNode )
		return m_pWalkRunNode->IsStopped();
#endif

	return true;
}

void A3DWalkRunDynData::InnerUpdateAct( DWORD dwDeltaTime )
{
#ifdef _ANGELICA21

	if( ! m_pAct || ! m_pWalkRunNode )
		return;

	A3DCombinedAction* pEventFromCombAct = NULL;
	int iEventFromCombAct = m_pAct->GetWalkRunEventFromCombAct( & pEventFromCombAct );
	if( ! pEventFromCombAct
		|| iEventFromCombAct < 0
		|| iEventFromCombAct >= m_pWalkRunNode->GetChildrenNum())
		return;

	AnimBlendChild& child = m_pWalkRunNode->GetChild( iEventFromCombAct );
	if (child.mAnimNode)
	{
		float fPlaySpeed = child.mAnimNode->GetPlaySpeed();
		m_fTimeSpan += dwDeltaTime * fPlaySpeed;
		m_dwTimeSpan = static_cast< DWORD >( m_fTimeSpan );
	}

	if( IsActionStopped() )
		return;

	DWORD dwTimeSpan = pEventFromCombAct->GetMinComActTimeSpan();
	if( m_fTimeSpan >= dwTimeSpan )
	{
		++ m_nCurActLoop;

		m_fTimeSpan -= dwTimeSpan;
		m_dwTimeSpan = static_cast< DWORD >( m_fTimeSpan );

		m_CurEventPos = NULL;

		size_t i = 0;
		while( i < m_ActFxArray.size() )
		{
			FX_BINDING_BASE* pBind = m_ActFxArray[i];
			if( pBind->IsStop() )
			{
				m_ActFxArray.erase( m_ActFxArray.begin() + i );
				delete pBind;

				--s_CombActFxArrayCount;
			}
			else
				++ i;
		}
	}
#endif
}

bool A3DWalkRunUpDownDynData::s_bUseSlope = true;
float A3DWalkRunUpDownDynData::s_fSlopeAngle = 30.0f;
float A3DWalkRunUpDownDynData::s_fWeightTrans = 300.0f;

A3DWalkRunUpDownDynData::A3DWalkRunUpDownDynData( A3DCombinedAction* pAct, CECModel* pECModel )
	: A3DWalkRunDynData( pAct, pECModel )
	, m_pWalkRunUpDownBlendTree( NULL )
	, m_fWRWeight(1)
{
}

A3DWalkRunUpDownDynData::~A3DWalkRunUpDownDynData()
{
}

void A3DWalkRunUpDownDynData::Play( int iChannel, float fWeight, int iTransTime, DWORD dwEventMask, bool bRestart, bool bAbsTrack, bool bNoFx, bool bNoCamShake )
{
#ifdef _ANGELICA21

	m_nChannel		= iChannel;
	m_fWeight		= fWeight;
	m_nTransTime	= iTransTime;
	m_dwEventMask	= dwEventMask;
	m_bAbsTrack		= bAbsTrack;
	m_bNoFx			= bNoFx;

	Resume();

	A3DSkinModel* pSkinModel = m_pECModel->GetA3DSkinModel();
	if( ! pSkinModel )
		return;

	A3DAnimNode* pNode = m_pECModel->GetAnimNodeMan()->GetAnimNode( A3DCombinedAction::SPECIALTYPE_WALKRUNUPDOWN, iChannel );
	m_pWalkRunUpDownBlendTree = dynamic_cast< AnimBlendTree* >( pNode );
	if( ! m_pWalkRunUpDownBlendTree )
		return;

	AnimBlendNode* pBlendNode = m_pWalkRunUpDownBlendTree->FindNode( "walkrun" );
	if( ! pBlendNode )
		return;

	pNode = pBlendNode->GetAnimNode();
	m_pWalkRunNode = dynamic_cast< A3DAnimNodeBlendWalkRun* >( pNode );
	if( ! m_pWalkRunNode )
		return;

	bool bRet = pSkinModel->PlayAnimNode( m_pWalkRunUpDownBlendTree, iChannel, m_pAct->GetRank( iChannel ), -1, iTransTime, bRestart );
	if( ! bRet )
		return;

	pSkinModel->GetActionChannel( iChannel )->SetWeight( fWeight );

	UpdateEvent(0, 0);
#endif
}

//trans a to b
template<class T>
inline T a2bWithWeight(const T& a, const T& b, /*const T& Delta*/ float weight)
{
	a_Clamp(weight, 0.0f, 1.0f);
	return a + (b - a) * weight;
/*	if (a > b)
	{
		T val = a - Delta;
		if (val < b)
			val = b;
		return val;
	}
	else
	{
		T val = a + Delta;
		if (val > b)
			val = b;
		return val;
	}*/	
}

void A3DWalkRunUpDownDynData::UpdateAct( DWORD dwDeltaTime )
{
#ifdef _ANGELICA21

	InnerUpdateAct( dwDeltaTime );

	const CECModel::WALKRUNUPDOWNBLEND_INFO& c_info = m_pECModel->GetWalkRunUpDownBlendInfo();
	if( ! c_info.m_pCB )
		return;

	if( ! m_pECModel || ! m_pWalkRunUpDownBlendTree || ! m_pWalkRunNode )
		return;

	A3DSkinModel* pSkinModel = m_pECModel->GetA3DSkinModel();
	if( ! pSkinModel )
		return;
	
	float fTransWeight = /*(float)dwDeltaTime / 1000.f * 5.0f; //*/(float)dwDeltaTime / s_fWeightTrans;
	float fWalkRunWeight = m_pWalkRunUpDownBlendTree->GetNodeWeight("walkrun");
	float fWalkUpWeight = m_pWalkRunUpDownBlendTree->GetNodeWeight("walkup");
	float fWalkDownWeight = m_pWalkRunUpDownBlendTree->GetNodeWeight("walkdown");
	float fRunUpWeight = m_pWalkRunUpDownBlendTree->GetNodeWeight("runup");
	float fRunDownWeight = m_pWalkRunUpDownBlendTree->GetNodeWeight("rundown");

	if (!s_bUseSlope)
		goto FULLWALKRUN;

	bool bRet = c_info.m_pCB->IsWalkRunChannel( m_nChannel );
	if( bRet )
		goto FULLWALKRUN;
	
	static const A3DVECTOR3 sc_vAxisY(0,1,0);
	static const A3DVECTOR3 sc_vDelta(0,-2,0);

	A3DVECTOR3 vStart = pSkinModel->GetPos() + sc_vAxisY;
	A3DVECTOR3 vHitPos;
	A3DVECTOR3 vNormal;
	float fFraction;
	bRet = c_info.m_pCB->RayTrace( vStart, sc_vDelta, vHitPos, vNormal, fFraction );
	if( ! bRet ) // 浮空
		goto FULLWALKRUN;

	A3DVECTOR3 vDir = pSkinModel->GetDir();
	A3DVECTOR3 vOnSlop = vDir - vNormal * DotProduct( vDir, vNormal );
	vOnSlop.Normalize();

	const float fThAngle = DEG2RAD(s_fSlopeAngle); //A3D_PI / 6;

	float fWeight = 1 - asinf( fabsf( vOnSlop.y ) ) / fThAngle;

	//float fWRDeltaWeight = ( fWeight - m_fWRWeight ) / m_fWeightTrans;
	//m_fWRWeight += fWRDeltaWeight * dwDeltaTime;
	m_fWRWeight = a2bWithWeight(fWalkRunWeight, fWeight, fTransWeight);

	if( m_fWRWeight > 0.999f )
		m_fWRWeight = 1.f;
	else if( m_fWRWeight < 0.001f )
		m_fWRWeight = 0.f;

	m_pWalkRunUpDownBlendTree->SetNodeWeight( "walkrun", m_fWRWeight );

	float fWalkWeight = ( 1 - m_fWRWeight ) * m_pWalkRunNode->GetChild(0).mWeight;
	float fRunWeight  = ( 1 - m_fWRWeight ) * m_pWalkRunNode->GetChild(1).mWeight;

	////tmp code
	//if (m_pWalkRunNode->GetCurrentVelocity() < 3)
	//	goto FULLWALKRUN;
	//fWalkWeight = 0;
	//fRunWeight  = ( 1 - m_fWRWeight );
	////end tmp code


	if( vOnSlop.y < 0 )
	{
		m_pWalkRunUpDownBlendTree->SetNodeWeight( "walkup", a2bWithWeight(fWalkUpWeight, 0.0f, fTransWeight) );
		m_pWalkRunUpDownBlendTree->SetNodeWeight( "walkdown", a2bWithWeight(fWalkDownWeight, fWalkWeight, fTransWeight) );

		m_pWalkRunUpDownBlendTree->SetNodeWeight( "runup", a2bWithWeight(fRunUpWeight, 0.0f, fTransWeight) );
		m_pWalkRunUpDownBlendTree->SetNodeWeight( "rundown", a2bWithWeight(fRunDownWeight, fRunWeight, fTransWeight) );
	}
	else
	{
		m_pWalkRunUpDownBlendTree->SetNodeWeight( "walkup", a2bWithWeight(fWalkUpWeight, fWalkWeight, fTransWeight)  );
		m_pWalkRunUpDownBlendTree->SetNodeWeight( "walkdown", a2bWithWeight(fWalkDownWeight, 0.0f, fTransWeight) );

		m_pWalkRunUpDownBlendTree->SetNodeWeight( "runup", a2bWithWeight(fRunUpWeight, fRunWeight, fTransWeight) );
		m_pWalkRunUpDownBlendTree->SetNodeWeight( "rundown", a2bWithWeight(fRunDownWeight, 0.0f, fTransWeight) );
	}

	return;

FULLWALKRUN:
	m_pWalkRunUpDownBlendTree->SetNodeWeight( "walkrun", a2bWithWeight(fWalkRunWeight, 1.0f, fTransWeight) );
	m_pWalkRunUpDownBlendTree->SetNodeWeight( "walkup", a2bWithWeight(fWalkUpWeight, 0.0f, fTransWeight) );
	m_pWalkRunUpDownBlendTree->SetNodeWeight( "walkdown", a2bWithWeight(fWalkDownWeight, 0.0f, fTransWeight) );
	m_pWalkRunUpDownBlendTree->SetNodeWeight( "runup", a2bWithWeight(fRunUpWeight, 0.0f, fTransWeight) );
	m_pWalkRunUpDownBlendTree->SetNodeWeight( "rundown", a2bWithWeight(fRunDownWeight, 0.0f, fTransWeight) );
#endif
}

A3DRunTurnDynData::A3DRunTurnDynData( A3DCombinedAction* pAct, CECModel* pECModel )
	: A3DCombActDynData( pAct, pECModel )
	, m_pRunTurnNode( NULL )
	, m_fTimeSpan(0)
	, m_fActTime(0)
	, m_fEventTime(0)
{
	m_bIsInfiniteComAct = true;
}

A3DRunTurnDynData::~A3DRunTurnDynData()
{

}

void A3DRunTurnDynData::Play( int iChannel, float fWeight, int iTransTime, DWORD dwEventMask, bool bRestart, bool bAbsTrack, bool bNoFx, bool bNoCamShake )
{
#ifdef _ANGELICA21

	m_nChannel		= iChannel;
	m_fWeight		= fWeight;
	m_nTransTime	= iTransTime;
	m_dwEventMask	= dwEventMask;
	m_bAbsTrack		= bAbsTrack;
	m_bNoFx			= bNoFx;

	Resume();

	A3DSkinModel* pSkinModel = m_pECModel->GetA3DSkinModel();
	if( ! pSkinModel )
		return;

	A3DCombinedAction::Enum_SpecType eSpecType = m_pAct->GetSpecType();
	A3DAnimNode* pNode = m_pECModel->GetAnimNodeMan()->GetAnimNode( eSpecType, iChannel );
	m_pRunTurnNode = dynamic_cast< A3DAnimNodeBlend3Children* >( pNode );
	if( ! m_pRunTurnNode )
		return;

	if( ! pSkinModel->PlayAnimNode( m_pRunTurnNode, iChannel, m_pAct->GetRank( iChannel ), -1, iTransTime, bRestart ) )
		return;

	pSkinModel->GetActionChannel( iChannel )->SetWeight( fWeight );

#endif
}

void A3DRunTurnDynData::UpdateAct( DWORD dwDeltaTime )
{
#ifdef _ANGELICA21

	if( ! m_pAct || ! m_pRunTurnNode )
		return;

	A3DCombinedAction* pEventFromCombAct = NULL;
	int iEventFromCombAct = m_pAct->GetRunTurnEventFromCombAct( & pEventFromCombAct );
	if( ! pEventFromCombAct
		|| iEventFromCombAct < 0
		|| iEventFromCombAct >= m_pRunTurnNode->GetChildrenNum())
		return;

	AnimBlendChild& child = m_pRunTurnNode->GetChild( iEventFromCombAct );
	if (child.mAnimNode)
	{
		float fPlaySpeed = child.mAnimNode->GetPlaySpeed();
		m_fTimeSpan += dwDeltaTime * fPlaySpeed;
		m_dwTimeSpan = static_cast< DWORD >( m_fTimeSpan );
	}

	if( IsActionStopped() )
		return;

	DWORD dwTimeSpan = pEventFromCombAct->GetMinComActTimeSpan();
	if( m_fTimeSpan >= dwTimeSpan )
	{
		++ m_nCurActLoop;

		m_fTimeSpan -= dwTimeSpan;
		m_dwTimeSpan = static_cast< DWORD >( m_fTimeSpan );

		m_CurEventPos = NULL;

		size_t i = 0;
		while( i < m_ActFxArray.size() )
		{
			FX_BINDING_BASE* pBind = m_ActFxArray[i];
			if( pBind->IsStop() )
			{
				m_ActFxArray.erase( m_ActFxArray.begin() + i );
				delete pBind;

				--s_CombActFxArrayCount;
			}
			else
				++ i;
		}
	}

#endif
}

void A3DRunTurnDynData::UpdateEvent( DWORD dwActTime, DWORD dwEventTime )
{
#ifdef _ANGELICA21

	if( ! m_pAct )
		return;

	int iEventFromCombAct = m_pAct->GetWalkRunEventFromCombAct();
	if( iEventFromCombAct < 0
		|| !m_pRunTurnNode
		|| iEventFromCombAct >= m_pRunTurnNode->GetChildrenNum() )
	{
		A3DCombActDynData::UpdateEvent( dwActTime, dwEventTime );
		return;
	}

	AnimBlendChild& child = m_pRunTurnNode->GetChild( iEventFromCombAct );
	if (child.mAnimNode)
	{
		float fPlaySpeed = child.mAnimNode->GetPlaySpeed();
		m_fActTime += dwActTime * fPlaySpeed;
		m_fEventTime += dwEventTime * fPlaySpeed;
		dwActTime = static_cast< DWORD >( m_fActTime );
		dwEventTime = static_cast< DWORD >( m_fEventTime );
		m_fActTime -= dwActTime;
		m_fEventTime -= dwEventTime;
	}

	A3DCombActDynData::UpdateEvent( dwActTime, dwEventTime );

#endif
}

bool A3DRunTurnDynData::IsActionStopped() const
{
#ifdef _ANGELICA21
	if( m_pRunTurnNode )
		return m_pRunTurnNode->IsStopped();
#endif

	return true;
}