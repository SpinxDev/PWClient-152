/*
 * FILE: FaceAnimation.cpp
 *
 * DESCRIPTION: Class for control face animation
 *
 * CREATED BY: Jiangdalong, 2005/02/24
 *
 * HISTORY:
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.	
 */

#include "FaceAnimation.h"
#include "FaceBoneController.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFaceAnimation::CFaceAnimation()
{
	m_pAnimation	= NULL;
	m_pWeights		= NULL;
	m_status		= STOP;

	m_bInited		= false;
}

CFaceAnimation::~CFaceAnimation()
{
	Release();
}

// Release
void CFaceAnimation::Release()
{
	if (m_pAnimation)
	{
		delete[] m_pAnimation;
		m_pAnimation = NULL;
	}

	if (m_pWeights)
	{
		delete[] m_pWeights;
		m_pWeights = NULL;
	}

	m_bInited = false;
}

// Init animation
bool CFaceAnimation::Init(int nMaxExpCount, CExpression* pExpression, float* pWeight)
{
	// Release animation
	Release();
	
	// alloc memory for new animation
	m_nNumExpression = nMaxExpCount;
	if (nMaxExpCount > 0)
	{
		m_pAnimation = new CExpression[nMaxExpCount];
		if (!m_pAnimation)
			return false;
		m_pWeights = new float[nMaxExpCount];
		if (!m_pWeights)
			return false;

		// Assign expressions to animation
		for (int i=0; i<nMaxExpCount; i++)
		{
			m_pAnimation[i].CopyExpression(&pExpression[i]);
		}

		memcpy(m_pWeights, pWeight, nMaxExpCount*sizeof(float));
	}

	// now determine the bone affecting flag
	memset(m_bBoneAffectedFlags, 0, sizeof(bool) * TOTAL_BONE_COUNT);
	for(int i=0; i<nMaxExpCount; i++)
	{
		int nBoneCount = m_pAnimation[i].GetAffectedBoneCount();
		for(int j=0; j<nBoneCount; j++)
		{
			m_bBoneAffectedFlags[m_pAnimation[i].GetBoneIndex(j)] = true;
		}
	}

	m_bInited = true;
	return true;
}

// Begin to play animtion
bool CFaceAnimation::Play(int nAppearTime, int nKeepTime, int nDisappearTime, int nRestTime, int nNumRepeat)
{
	ASSERT(nAppearTime >= 0);
	ASSERT(nKeepTime >= 0);
	ASSERT(nDisappearTime >= 0);
	ASSERT(nRestTime >= 0);

	m_nAppearTime = nAppearTime;
	m_nKeepTime = nKeepTime;
	m_nDisappearTime = nDisappearTime;
	m_nRestTime = nRestTime;
	m_nPassedTime = 0;

	m_nMaxRepeat = nNumRepeat;
	m_nNumRepeat = 0;

	if ((!m_pAnimation) || (!m_pWeights)) 
		return false;

	// Get other expressions' affection on the bones that this animation will take effects on.
	ResetAffectedBones();

	AddExpression(m_pFaceController, m_nNumExpression, m_pAnimation, m_pWeights, m_nAppearTime);
	m_status = APPEAR;

	return true;
}

// Animation tick
bool CFaceAnimation::Tick(DWORD dwTickTime)
{
	switch (m_status)
	{
	case STOP:
		break;
	case APPEAR:
		if (m_nPassedTime >= m_nAppearTime)// Appear time is over
			m_status = KEEP;
		else
		{
			break;
		}

	case KEEP:
		if (m_nPassedTime >= m_nAppearTime + m_nKeepTime) // Keep time is over
		{
			// reset affected bones
			ResetAffectedBones();
			// Update affected bones so this animation will disappear.
			UpdateAffectedBones(m_nDisappearTime);
			m_status = DISAPPEAR;
		}
		else
		{
			break;
		}

	case DISAPPEAR:
		if (m_nPassedTime >= m_nAppearTime + m_nKeepTime + m_nDisappearTime)// Disappear time is over
		{
			m_status = REST;
		}
		else
		{
			break;
		}

	case REST:
		{
			int nTime;
			(m_nRestTime > 0) ? nTime = (m_nRestTime >> 2) : 0;
			if (m_nPassedTime + rand() % nTime >= m_nAppearTime + m_nKeepTime + m_nDisappearTime + m_nRestTime)// Rest time is over
			{
				m_nNumRepeat++;
				if ((m_nMaxRepeat == 0) || (m_nNumRepeat < m_nMaxRepeat))
				{
					// Reset affected bones' face shape
					ResetAffectedBones();
					
					// Repeat animation
					AddExpression(m_pFaceController, m_nNumExpression, m_pAnimation, m_pWeights, m_nAppearTime);
					m_nPassedTime = 0;
					m_status = APPEAR;
				}
				else
				{
					Stop();
				}
			}
		}

		break;
	}

	m_nPassedTime += dwTickTime;

	return true;
}

// Stop animation
bool CFaceAnimation::Stop()
{
	m_status = STOP;
	// Reset affected bones' face shape
	ResetAffectedBones();
	// Update affected bones so this animation will disappear.
	UpdateAffectedBones(m_nDisappearTime);
	return true;
}

// Reset all affected bones' to their stored TM
bool CFaceAnimation::ResetAffectedBones()
{
	for(int i=0; i<TOTAL_BONE_COUNT; i++)
	{
		if( m_bBoneAffectedFlags[i] )
		{
			m_pFaceController->GetFaceBone()[i].RestoreShapeTM();
		}
	}

	return true;
}

// Update all affected bones
bool CFaceAnimation::UpdateAffectedBones(int nTransTime)
{
	for(int i=0; i<TOTAL_BONE_COUNT; i++)
	{
		if( m_bBoneAffectedFlags[i] )
		{
			m_pFaceController->GetFaceBone()[i].Update(false, nTransTime);
		}
	}

	return true;
}
