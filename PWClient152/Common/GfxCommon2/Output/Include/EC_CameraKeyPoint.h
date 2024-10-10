/*
* FILE: EC_CameraKeyPoint.h
*
* DESCRIPTION: 这个文件用于摄像机控制事件点，EVENT_MASK_CAM_PT，在ecm文件中编辑的摄像机控制点可以进行插值，获得摄像机控制动画。
*
* CREATED BY: Zhangyachuan, 2009/11/05
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _ECCAMERAKEYPOINT_H_
#define _ECCAMERAKEYPOINT_H_

#include "A3DCombinedAction.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

struct CAMERA_BEZIER_PT;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

void GenBezierPath(CAMERA_BEZIER_PT*& pBezierPts, unsigned int& iBezierCount, const CameraControlEvent::CameraControlParam* pParam);

///////////////////////////////////////////////////////////////////////////
//	
//	struct CAMERA_KEYDATA
//	
///////////////////////////////////////////////////////////////////////////

struct CAMERA_BEZIER_PT
{
	A3DVECTOR3 m_vPos;
	float m_fRatio;
};

struct CAMERA_KEYDATA
{
	CAMERA_KEYDATA();
	~CAMERA_KEYDATA();

	A3DVECTOR3 m_vCamera;
	A3DVECTOR3 m_vTarget;
	CAMERA_BEZIER_PT * m_pBezierVerts;	// For Bezier key point this is valid, or else be NULL
	unsigned int m_iBezierVertCount;	// For Bezier key point this is > 0, or else be 0
};

///////////////////////////////////////////////////////////////////////////
//	
//	struct CAMERA_KEYPOINT
//	
///////////////////////////////////////////////////////////////////////////

struct CAMERA_KEYPOINT
{
	CAMERA_KEYPOINT()
		: m_fAngleAcc(0)
		, m_fLinearAcc(0)
		, m_dwStartTime(0)
		, m_bIsInterp(false)
	{
		
	}
	DWORD m_dwStartTime;
	bool m_bIsInterp;
	float m_fAngleAcc;
	float m_fLinearAcc;
	CAMERA_KEYDATA m_Data;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECCameraKeyPointMan
//	
///////////////////////////////////////////////////////////////////////////

class CECCameraKeyPointMan
{
public:

	CECCameraKeyPointMan(void);
	virtual ~CECCameraKeyPointMan();

public:

	virtual bool Init(A3DCombinedAction* pCombinedAction);
	virtual void Release();

	virtual void Tick(DWORD dwTickTime) {}
	virtual void SetStartPos(const A3DVECTOR3* pStartCamPos, const A3DVECTOR3* pStartTargetPos);
	virtual bool GetInterpPosition(int iTime, A3DVECTOR3* pOutCameraPos, A3DVECTOR3* pOutTargetPos);

private:
	
	CAMERA_KEYDATA m_StartEndKey;
	bool m_bIsStartPosExist;
	DWORD m_dwActionLen;
	APtrArray<CAMERA_KEYPOINT*> m_aCamKeyPoints;

private:
	
	CECCameraKeyPointMan(const CECCameraKeyPointMan&);
	CECCameraKeyPointMan& operator = (const CECCameraKeyPointMan&);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



#endif	//	_ECCAMERAKEYPOINT_H_


