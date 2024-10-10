/*
 * FILE: A3DTexCoordCtrl.h
 *
 * DESCRIPTION: Routines used to management shader
 *
 * CREATED BY: liyi, 2011/12/26
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3D_TEX_COORD_CTRL_H_
#define _A3D_TEX_COORD_CTRL_H_

#include "A3DMathUtility.h"

class A3DFXTCCtrl
{
public:		//	Types

	//	Linear function
	struct LINEARFUNC
	{
		float	fBase;		//	Base value
		float	fSpeed;		//	Value changes per second
		float	fValue;		//	Accumulated change value

		LINEARFUNC() :
		fBase(0.0f),
			fSpeed(0.0f),
			fValue(0.0f)
		{
		}

		//	Tick routine
		void Tick(float fTime) { fValue += fTime * fSpeed; }
		//	Get current value
		float GetCurValue() const { return fBase + fValue; }
	};


protected:	//	Constructors and Destructors

	A3DFXTCCtrl();
	~A3DFXTCCtrl();

public:		//	Attributes

public:		//	Operations

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);

	//	Get/Set rotate base
	void SetRotateBase(float fRadian) { m_TCCRotate.fBase = fRadian; }
	float GetRotateBase() { return m_TCCRotate.fBase; }
	//	Get/Set rotate speed
	void SetRotateSpeed(float fRadian) { m_TCCRotate.fSpeed = fRadian; }
	float GetRotateSpeed() { return m_TCCRotate.fSpeed; }
	//	Get current rotate radian
	float GetCurRotate() { return m_TCCRotate.GetCurValue(); }
	//	Reset accumulated rotate value and restart again
	void ResetRotate() { m_TCCRotate.fValue = 0.0f; }

	//	Get/Set scroll base
	void SetScrollBase(float u, float v) { m_TCCScrollU.fBase = u; m_TCCScrollV.fBase = v; }
	void GetScrollBase(float& u, float& v) { u = m_TCCScrollU.fBase; v = m_TCCScrollV.fBase; }
	//	Get/Set scroll speed
	void SetScrollSpeed(float u, float v) { m_TCCScrollU.fSpeed = u; m_TCCScrollV.fSpeed = v; }
	void GetScrollSpeed(float& u, float& v) { u = m_TCCScrollU.fSpeed; v = m_TCCScrollV.fSpeed; }
	//	Get current scroll
	void GetCurScroll(float& u, float& v) { u = m_TCCScrollU.GetCurValue(); v = m_TCCScrollV.GetCurValue(); }
	//	Reset accumulated scroll value and restart again
	void ResetScroll() { m_TCCScrollU.fValue = 0.0f; m_TCCScrollV.fValue = 0.0f; }

	//	Get/Set scale parameter
	void SetScale(float u, float v) { m_fTCCScaleU = u; m_fTCCScaleV = v; }
	void GetScale(float& u, float& v) { u = m_fTCCScaleU; v = m_fTCCScaleV; }

	//	Get/Set stretch parameter
	void SetStretch(int func, float base, float amp, float phase, DWORD period);
	void GetStretch(int& func, float& base, float& amp, float& phase, DWORD& period);
	//	Clear stretch effect
	void ClearStretch();

	//	Get final transform matrix
	const A3DVECTOR3& GetTMCol0() { return m_vTMCol0; }
	const A3DVECTOR3& GetTMCol1() { return m_vTMCol1; }

protected:	//	Attributes
	A3DWaveFunc*	m_pTCCStretch;		//	Texture Coordinates stretch
	float			m_fTCCScaleU;		//	Texture Coordinates scale on u axis
	float			m_fTCCScaleV;		//	Texture Coordinates scale on v axis
	LINEARFUNC		m_TCCRotate;		//	Texture Coordinates rotate
	LINEARFUNC		m_TCCScrollU;		//	Texture Coordinates scroll on u axis
	LINEARFUNC		m_TCCScrollV;		//	Texture Coordinates scroll on v axis

	A3DVECTOR3		m_vTMCol0;			//	Final transform matrix's column 0
	A3DVECTOR3		m_vTMCol1;			//	Final transform matrix's column 1

protected:	//	Operations

};

#endif //_A3D_TEX_COORD_CTRL_H_