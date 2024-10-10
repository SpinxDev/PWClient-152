/*
* FILE: EC_CameraKeyPoint.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2009/11/05
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "EC_CameraKeyPoint.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

inline static float 
_InterpFloat(float fLeft, float fRight, float fRatio)
{
	return fLeft + (fRight - fLeft) * fRatio;
}

inline static float 
_InterpWithAcc(float fStart, float fEnd, float fTotalTime, float fCurTime, float fAcc)
{
	float fTotalDelta = fEnd - fStart;
	float fInitVec = fTotalDelta / fTotalTime - 0.5f * fAcc * fTotalTime;
	return fStart + fInitVec * fCurTime + 0.5f * fAcc * fCurTime * fCurTime;
}

inline static float
_InterpFloatWithAcc(float fLeft, float fRight, float fRatio, float fAcc)
{
	float fResult = (fRight - fLeft) * fRatio + 0.5f * fAcc * fRatio * (fRatio - 1);
	return fResult < 0 ? fLeft : fLeft + fResult;
}

inline static void
_InterpVector3(float fRatio, const A3DVECTOR3& vLeft, const A3DVECTOR3& vRight, A3DVECTOR3& vOut)
{
	vOut = vLeft * (1 - fRatio) + vRight * fRatio;
}

inline static float
_AutoInterpFloat(float fRatio, float fL, float fR, float fAcc)
{
	if (fAcc == 0)
		return _InterpFloat(fL, fR, fRatio);
	else
		return _InterpFloatWithAcc(fL, fR, fRatio, fAcc);
};

void GenBezierPath(CAMERA_BEZIER_PT*& pBezierPts, unsigned int& iBezierCount, const CameraControlEvent::CameraControlParam* pParam)
{
	if (pParam->GetVertCount() < 3)
	{
		pBezierPts = NULL;
		iBezierCount = 0;
		return;
	}

	static const int _div_num = 5;

	size_t uBufSz = 3 * pParam->GetVertCount() - 2;
	size_t uVertSz = pParam->GetVertCount();

	A3DVECTOR3* pCorner = new A3DVECTOR3[uBufSz];
	pCorner[0] = pParam->GetVert(0);
	pCorner[uBufSz - 1] = pParam->GetVert(uVertSz - 1);

	size_t i;
	for (i = 1; i < pParam->GetVertCount() - 1; i++)
	{
		A3DVECTOR3 vDiff = (pParam->GetVert(i + 1) - pParam->GetVert(i - 1)) / 6.f;
		size_t uBase = 2 + (i - 1) * 3;
		pCorner[uBase] = pParam->GetVert(i) - vDiff;
		pCorner[uBase + 1] = pParam->GetVert(i);
		pCorner[uBase + 2] = pParam->GetVert(i) + vDiff;
	}

	pCorner[1] = pCorner[2] + (pParam->GetVert(0) - pParam->GetVert(1)) / 3.f;
	pCorner[uBufSz-2] = pCorner[uBufSz-3] + (pParam->GetVert(uVertSz-1) - pParam->GetVert(uVertSz-2)) / 3.f;

	iBezierCount = (uVertSz - 1) * _div_num + uVertSz;
	pBezierPts = new CAMERA_BEZIER_PT[iBezierCount];
	CAMERA_BEZIER_PT* pBezier = pBezierPts;

	for (i = 0; i < uVertSz - 1; i++)
	{
		size_t uBase = i * 3;
		size_t uInsert = i * (_div_num + 1);

		pBezier[uInsert].m_vPos = pParam->GetVert(i);
		for (size_t j = 1; j <= _div_num; j++)
		{
			pBezier[uInsert+j].m_vPos = Bezier4(
				pCorner[uBase],
				pCorner[uBase+1],
				pCorner[uBase+2],
				pCorner[uBase+3],
				j / (float)(_div_num + 1));
		}
	}

	pBezier[iBezierCount-1].m_vPos = pParam->GetVert(uVertSz - 1);
	pBezier[0].m_fRatio = 0;

	for (i = 1; i < iBezierCount; i++)
	{
		A3DVECTOR3 vDir = pBezier[i].m_vPos - pBezier[i-1].m_vPos;
		float fMag = vDir.Normalize();
		pBezier[i].m_fRatio = pBezier[i-1].m_fRatio + fMag;
	}

	float fTotalLen = pBezier[iBezierCount - 1].m_fRatio;

	for (i = 1; i < iBezierCount; i++)
		pBezier[i].m_fRatio /= fTotalLen;

	delete[] pCorner;
}

static void 
_GenCamBezierPath(CAMERA_KEYDATA* pKeyData, const CameraControlEvent::CameraControlParam* pParam)
{
	ASSERT(pParam->GetVertCount() >= 3);
	if (pKeyData->m_pBezierVerts)
	{
		delete [] pKeyData->m_pBezierVerts;
		pKeyData->m_pBezierVerts = NULL;
		pKeyData->m_iBezierVertCount = 0;
	}

	GenBezierPath(pKeyData->m_pBezierVerts, pKeyData->m_iBezierVertCount, pParam);
}

static bool
_Interp2(float fRatio, CAMERA_KEYPOINT* pLeft, CAMERA_KEYPOINT* pRight, A3DVECTOR3* pOutCameraPos, A3DVECTOR3* pOutTargetPos)
{
	ASSERT(pLeft && pRight && pOutCameraPos && pOutTargetPos);
	a_Clamp(fRatio, 0.f, 1.f);

	// Left is a bezier
	if (pLeft->m_Data.m_pBezierVerts && pLeft->m_Data.m_iBezierVertCount)
	{
		unsigned int iBezierCount = pLeft->m_Data.m_iBezierVertCount;
		CAMERA_BEZIER_PT* pBezierVerts = pLeft->m_Data.m_pBezierVerts;
		if (iBezierCount < 2)
			return false;

		unsigned int iBezierIdx;
		for (iBezierIdx = 0; iBezierIdx + 1 < iBezierCount; ++iBezierIdx)
		{
			if (fRatio <= pBezierVerts[iBezierIdx + 1].m_fRatio)
				break;
		}

		if (iBezierIdx + 1 >= iBezierCount)
		{
			*pOutCameraPos = pBezierVerts[iBezierCount - 1].m_vPos;
		}
		else
		{
			float fBezierRatio = fRatio;
			fBezierRatio -= pBezierVerts[iBezierIdx].m_fRatio;
			fBezierRatio /= (pBezierVerts[iBezierIdx + 1].m_fRatio - pBezierVerts[iBezierIdx].m_fRatio);
			_InterpVector3(fBezierRatio, pBezierVerts[iBezierIdx].m_vPos, pBezierVerts[iBezierIdx + 1].m_vPos, *pOutCameraPos);
			*pOutCameraPos += pLeft->m_Data.m_vCamera;
		}

		*pOutTargetPos = pLeft->m_Data.m_vTarget;
		return true;
	}

	// Left is not bezier
	// Right is not interp
	if (!pRight->m_bIsInterp)
	{
		*pOutCameraPos = pLeft->m_Data.m_vCamera;
		*pOutTargetPos = pLeft->m_Data.m_vTarget;
		return true;
	}

	A3DVECTOR3 vDirLeft(pLeft->m_Data.m_vTarget - pLeft->m_Data.m_vCamera);
	A3DVECTOR3 vDirRight(pRight->m_Data.m_vTarget - pRight->m_Data.m_vCamera);
	const float fDistLeft = vDirLeft.Normalize();
	const float fDistRight = vDirRight.Normalize();

	A3DVECTOR3 vAxis = CrossProduct(vDirLeft, vDirRight);
 	float fCheckParallel = vAxis.Normalize();
	// 检查如果两个方向平行，不能再继续走下面的方向插值代码了
	if (fCheckParallel <= 1e-5)
	{
		A3DVECTOR3 vCurTarget;
		_InterpVector3(fRatio, pLeft->m_Data.m_vTarget, pRight->m_Data.m_vTarget, vCurTarget);

		float fCurDist = _AutoInterpFloat(fRatio, fDistLeft, fDistRight, pLeft->m_fLinearAcc);
		
		*pOutCameraPos = vCurTarget - vDirLeft * fCurDist;
		*pOutTargetPos = vCurTarget;
		return true;
	}

	float fCosAngle = DotProduct(vDirLeft, vDirRight);
	float fAngle = acosf(fCosAngle);

	float fCurAngle;
	if (pLeft->m_fAngleAcc == 0)
		fCurAngle = _InterpFloat(0, fAngle, fRatio);
	else
		fCurAngle = _InterpFloatWithAcc(0, fAngle, fRatio, DEG2RAD(pLeft->m_fAngleAcc));

 	A3DQUATERNION q(vAxis, fCurAngle);
 	q.Normalize();
 	A3DVECTOR3 vCurDir = q * vDirLeft;
 	vCurDir.Normalize();

	A3DVECTOR3 vCurTarget;
	_InterpVector3(fRatio, pLeft->m_Data.m_vTarget, pRight->m_Data.m_vTarget, vCurTarget);

	const float fCurDist = _AutoInterpFloat(fRatio, fDistLeft, fDistRight, pLeft->m_fLinearAcc);

	*pOutCameraPos = vCurTarget - vCurDir * fCurDist;
	*pOutTargetPos = vCurTarget;
	return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement CAMERA_KEYDATA::CAMERA_KEYDATA
//
///////////////////////////////////////////////////////////////////////////

CAMERA_KEYDATA::CAMERA_KEYDATA()
: m_pBezierVerts(NULL)
, m_iBezierVertCount(0)
, m_vCamera(0)
, m_vTarget(0)
{
	
}

CAMERA_KEYDATA::~CAMERA_KEYDATA()
{
	delete [] m_pBezierVerts;
	m_pBezierVerts = NULL;
	m_iBezierVertCount = 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement CECCameraKeyPointMan
//
///////////////////////////////////////////////////////////////////////////

CECCameraKeyPointMan::CECCameraKeyPointMan(void)
: m_bIsStartPosExist(false)
, m_dwActionLen(0)
{

}

CECCameraKeyPointMan::~CECCameraKeyPointMan()
{
	Release();
}

bool CECCameraKeyPointMan::Init(A3DCombinedAction* pCombinedAction)
{
	Release();

	if (!pCombinedAction || !pCombinedAction->GetEventCountByType(EVENT_TYPE_CAM_PT))
		return false;

	if (pCombinedAction->GetMinComActTimeSpan() == 0)
		pCombinedAction->CalcMinComActTimeSpan();
	
	m_dwActionLen = pCombinedAction->GetMinComActTimeSpan();

	APtrList<CameraControlEvent*> camCtrlList;
	for (int iEventIdx = 0; iEventIdx < pCombinedAction->GetEventCount(); ++iEventIdx)
	{
		EVENT_INFO* pInfo = pCombinedAction->GetEventInfo(iEventIdx);
		if (!pInfo || pInfo->GetType() != EVENT_TYPE_CAM_PT)
			continue;

		if (pInfo->GetStartTime() > m_dwActionLen)
			continue;

		CameraControlEvent* pCamCtrlEvent = (CameraControlEvent*)pInfo;

		ALISTPOSITION pos = camCtrlList.GetHeadPosition();
		while (pos)
		{
			CameraControlEvent* pEvent = camCtrlList.GetAt(pos);
			if (pEvent->GetStartTime() > pInfo->GetStartTime())
				break;

			camCtrlList.GetNext(pos);
		}

		if (pos)
			camCtrlList.InsertBefore(pos, pCamCtrlEvent);
		else
			camCtrlList.AddTail(pCamCtrlEvent);
	}

	ALISTPOSITION pos = camCtrlList.GetHeadPosition();
	while (pos)
	{
		CameraControlEvent* pCamCtrlEvent = camCtrlList.GetNext(pos);

		const CameraControlEvent::CameraControlParam* pParam = pCamCtrlEvent->GetCameraControlParam();

		CAMERA_KEYPOINT* pKeyPoint	= new CAMERA_KEYPOINT;
		pKeyPoint->m_dwStartTime	= pCamCtrlEvent->GetStartTime();
		pKeyPoint->m_bIsInterp		= pParam->m_bIsInterp;
		pKeyPoint->m_fLinearAcc		= pParam->m_fCameraLinearAcc;
		pKeyPoint->m_fAngleAcc		= pParam->m_fCameraAngleAcc;
		pKeyPoint->m_Data.m_vCamera = pParam->m_vCamera;
		pKeyPoint->m_Data.m_vTarget = pParam->m_vTarget;

		if (pParam->GetVertCount() == 0 || pParam->GetVertCount() < 3)
		{
			pKeyPoint->m_Data.m_pBezierVerts = NULL;
			pKeyPoint->m_Data.m_iBezierVertCount = 0;
		}
		else
		{
			_GenCamBezierPath(&pKeyPoint->m_Data, pParam);
		}

		m_aCamKeyPoints.Add(pKeyPoint);
	}

	return true;
}

void CECCameraKeyPointMan::Release()
{
	for (int iCamKeyPointIdx = 0; iCamKeyPointIdx < m_aCamKeyPoints.GetSize(); ++iCamKeyPointIdx)
	{
		delete m_aCamKeyPoints[iCamKeyPointIdx];
	}

	m_aCamKeyPoints.RemoveAll();
}

void CECCameraKeyPointMan::SetStartPos(const A3DVECTOR3* pStartCamPos, const A3DVECTOR3* pStartTargetPos)
{
	if (!pStartCamPos || !pStartTargetPos)
	{
		m_bIsStartPosExist = false;
		return;
	}
	
	m_StartEndKey.m_vCamera = *pStartCamPos;
	m_StartEndKey.m_vTarget = *pStartTargetPos;
	m_StartEndKey.m_pBezierVerts = NULL;
	m_StartEndKey.m_iBezierVertCount = 0;
	m_bIsStartPosExist = true;
}

bool CECCameraKeyPointMan::GetInterpPosition(int iTime, A3DVECTOR3* pOutCameraPos, A3DVECTOR3* pOutTargetPos)
{
	if (!pOutCameraPos || !pOutTargetPos || m_aCamKeyPoints.GetSize() == 0)
		return false;
	
	if (iTime > (int)m_dwActionLen)
		return false;

	int iLeftKey = -1;
	int iRightKey = -1;
	for (int iCamKeyPointIdx = 0; iCamKeyPointIdx < m_aCamKeyPoints.GetSize(); ++iCamKeyPointIdx)
	{
		if ((int)m_aCamKeyPoints[iCamKeyPointIdx]->m_dwStartTime > iTime)
		{
			iLeftKey = iCamKeyPointIdx - 1;
			iRightKey = iCamKeyPointIdx;
			break;
		}
	}

	if (iLeftKey == -1)
	{
		if (iRightKey == -1)
		{
			// the end

			CAMERA_KEYPOINT _StartKey;
			_StartKey.m_bIsInterp = m_bIsStartPosExist;
			_StartKey.m_dwStartTime = m_dwActionLen;
			_StartKey.m_Data = m_StartEndKey;

			CAMERA_KEYPOINT* pLeft = m_aCamKeyPoints[m_aCamKeyPoints.GetSize() - 1];
			DWORD dwLeftTime = pLeft->m_dwStartTime;
			float fRatio = (float)(iTime - dwLeftTime) / (m_dwActionLen - dwLeftTime);
			return _Interp2(fRatio, pLeft, &_StartKey, pOutCameraPos, pOutTargetPos);
		}
		else
		{
			// the begin
			
			CAMERA_KEYPOINT _StartKey;
			_StartKey.m_bIsInterp = true;
			_StartKey.m_dwStartTime = 0;
			_StartKey.m_Data = m_StartEndKey;

			float fRatio = (float)iTime / (m_aCamKeyPoints[iRightKey]->m_dwStartTime);
			return _Interp2(fRatio, &_StartKey, m_aCamKeyPoints[iRightKey], pOutCameraPos, pOutTargetPos);
		}
	}
	
	ASSERT(iLeftKey != -1 && iRightKey != -1);
	CAMERA_KEYPOINT* pLeft = m_aCamKeyPoints[iLeftKey];
	CAMERA_KEYPOINT* pRight = m_aCamKeyPoints[iRightKey];
	float fRatio = (float)(iTime - pLeft->m_dwStartTime) / (pRight->m_dwStartTime - pLeft->m_dwStartTime);


	return _Interp2(fRatio, pLeft, pRight, pOutCameraPos, pOutTargetPos);
}