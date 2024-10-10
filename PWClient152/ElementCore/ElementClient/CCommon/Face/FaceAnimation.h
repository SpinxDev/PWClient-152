/*
 * FILE: FaceAnimation.h
 *
 * DESCRIPTION: Class for control face animation
 *
 * CREATED BY: Jiangdalong, 2005/02/24
 *
 * HISTORY:
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _FACEANIMATION_H_
#define _FACEANIMATION_H_

#include "Expression.h"
#include "FaceCommon.h"

class CFaceAnimation  
{
public:
	enum STATUS
	{
		STOP,
		APPEAR,
		KEEP,
		DISAPPEAR,
		REST,
	};
public:
	CFaceAnimation();
	virtual ~CFaceAnimation();

public:

	// Init animation
	bool Init(int nMaxExpCount, CExpression* pExpression, float* pWeight);
	// Play animtion
	// nNumRepeat: animation repeat times. If 0, the animation will keep repeat
	bool Play(int nAppearTime, int nKeepTime, int nDisappearTime, int nRestTime, int nNumRepeat);
	// Stop animation
	bool Stop();
	// Animation tick
	bool Tick(DWORD dwTickTime);

	// Release
	void Release();

	inline void SetFaceBoneController(CFaceBoneController* pController) { m_pFaceController = pController; }

protected:
	CFaceBoneController*				m_pFaceController;	// Face bone controller

	CExpression*						m_pAnimation;		// Expressions to be animated
	float*								m_pWeights;			// Expressions' weights

	STATUS		m_status;					// Is animating?

	bool		m_bInited;					// Has been initialized?

	int			m_nAppearTime;				// Appear time for animation
	int			m_nKeepTime;				// Keep time for animation
	int			m_nDisappearTime;			// Disappear time for animation
	int			m_nRestTime;				// Rest time until next loop
	int			m_nPassedTime;				// Passed time

	int			m_nNumExpression;			// Expression count int the animation

	int			m_nMaxRepeat;				// Total repeat time
	int			m_nNumRepeat;				// Real repeat time

	bool		m_bBoneAffectedFlags[TOTAL_BONE_COUNT];	// flag indicates whether one bone is affected in this animation

public:
	inline bool HasInited()		{ return m_bInited; }

protected:
	// Reset all affected bones' to their stored TM
	bool ResetAffectedBones();

	// Update all affected bones
	bool UpdateAffectedBones(int nTransTime);
};

#endif // #ifndef _FACEANIMATION_H_
