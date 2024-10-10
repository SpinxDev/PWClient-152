/*
 * FILE: A3DTexCoordCtrl.cpp
 *
 * DESCRIPTION: Routines used to management shader
 *
 * CREATED BY: liyi, 2011/12/26
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */
#include "A3DTexCoordCtrl.h"
#include "A3DFuncs.h"

A3DFXTCCtrl::A3DFXTCCtrl() :
m_pTCCStretch(NULL),
m_fTCCScaleU(1.0f),
m_fTCCScaleV(1.0f),
m_vTMCol0(1.0f, 0.0f, 0.0f),
m_vTMCol1(0.0f, 1.0f, 0.0f)
{
	
}

A3DFXTCCtrl::~A3DFXTCCtrl()
{
	if (m_pTCCStretch)
	{
		delete m_pTCCStretch;
		m_pTCCStretch = NULL;
	}
}

void A3DFXTCCtrl::SetStretch(int func, float base, float amp, float phase, DWORD period)
{
	if (!m_pTCCStretch)
		m_pTCCStretch = new A3DWaveFunc;

	m_pTCCStretch->iFunc	= func;
	m_pTCCStretch->fBase	= base;
	m_pTCCStretch->fAmp		= amp;
	m_pTCCStretch->fPhase	= phase;
	m_pTCCStretch->dwPeriod	= period;
}

void A3DFXTCCtrl::GetStretch(int& func, float& base, float& amp, float& phase, DWORD& period)
{
	if (m_pTCCStretch)
	{
		func	= m_pTCCStretch->iFunc;
		base	= m_pTCCStretch->fBase;
		amp		= m_pTCCStretch->fAmp;
		phase	= m_pTCCStretch->fPhase;
		period	= m_pTCCStretch->dwPeriod;
	}
	else
	{
		func	= A3DWaveFunc::WAVE_NONE;
		base	= 0.0f;
		amp		= 0.0f;
		phase	= 0.0f;
		period	= 0;
	}
}

//	Tick routine
bool A3DFXTCCtrl::Tick(DWORD dwDeltaTime)
{
	float fTime = dwDeltaTime * 0.001f;

	m_TCCScrollU.Tick(fTime);
	m_TCCScrollV.Tick(fTime);
	m_TCCRotate.Tick(fTime);

	if (m_pTCCStretch)
		m_pTCCStretch->Tick(dwDeltaTime);

	//	Build transform matrix
	A3DMATRIX3 mat3x3(A3DMATRIX3::IDENTITY);
	float fRotate = GetCurRotate();

	if (m_pTCCStretch || fRotate)
	{
		//	u -= 0.5f, v -= 0.5f
		mat3x3._31 -= 0.5f;
		mat3x3._32 -= 0.5f;

		if (m_pTCCStretch)
		{
			//	Scale u and v
			float fVal = m_pTCCStretch->GetValue();

			A3DMATRIX3 matScale(A3DMATRIX3::IDENTITY);
			matScale._11 = fVal;
			matScale._22 = fVal;
			mat3x3 *= matScale;
		}

		//	Rotate
		if (fRotate)
			mat3x3 = a3d_Matrix3Rotate(mat3x3, fRotate);

		//	u += 0.5f, v += 0.5f
		mat3x3 = a3d_Matrix3Translate(mat3x3, 0.5f, 0.5f);
	}

	//	Scale effect
	if (m_fTCCScaleU != 1.0f || m_fTCCScaleV != 1.0f)
	{
		//	Scale u and v
		A3DMATRIX3 matScale(A3DMATRIX3::IDENTITY);
		matScale._11 = m_fTCCScaleU;
		matScale._22 = m_fTCCScaleV;
		mat3x3 *= matScale;
	}

	//	Scroll effect
	float u_off=0.0f, v_off=0.0f;
	GetCurScroll(u_off, v_off);
	if (u_off || v_off)
		mat3x3 = a3d_Matrix3Translate(mat3x3, u_off, v_off);

	//	Apply matrix to effect
	m_vTMCol0 = mat3x3.GetCol(0);
	m_vTMCol1 = mat3x3.GetCol(1);
	//m_pFXParam0->SetValue(&m_vTMCol0, sizeof (m_vTMCol0));
	//m_pFXParam1->SetValue(&m_vTMCol1, sizeof (m_vTMCol1));

	return true;
}

//	Clear stretch effect
void A3DFXTCCtrl::ClearStretch()
{
	if (m_pTCCStretch)
	{
		delete m_pTCCStretch;
		m_pTCCStretch = NULL;
	}
}
