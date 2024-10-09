/*
 * FILE: EC_LoadProgress.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/5/4
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AArray.h"
#include "ARect.h"
#include "vector.h"
#include "AWString.h"
#include "AUILuaManager.h"
/*#include "A3D.h"*/

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

class A2DSprite;
class AUIManager;


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

class CECLoadingUIMan : public AUILuaManager
{
public:
	virtual void CalcWindowScale();
};


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECLoadProgress
//	
///////////////////////////////////////////////////////////////////////////

class CECLoadProgress
{
public:		//	Constructor and Destructor

	CECLoadProgress(bool bSkipRender);
	virtual ~CECLoadProgress();

public:		//	Attributes

public:		//	Operations
	
	//	Initialize object
	bool Init(float Min, float Max);
	//	Release object
	void Release();
	//	Render routine
	bool Render(bool forceUpdate);

	//	Gets the current position. 
	float GetProgressPos() { return m_Pos; }
	//	Gets the lower and upper limits of the range 
	float GetProgressRange(float* pMin, float* pMax) { *pMin=m_Min; *pMax=m_Max; }
	//	Advances the current position
	float OffsetProgressPos(float Offset);
	//	Sets the current position 
	void SetProgressPos(float Pos);
	//	Sets the minimum and maximum ranges
	void SetProgressRange(float Min, float Max); 
	//	Specifies the step increment
	void SetProgressStep(float Step) { m_Step = Step; }
	//	Advances the current position by the step increment
	float StepProgress();

protected:	//	Attributes

	// return true means need to update the bar view
	// use this to avoid too many rendering call
	bool	UpdateBar();
	POINT	m_NextValid;

	float	m_Pos;		//	Current position
	float	m_Min;		//	Minimum position
	float	m_Max;		//	Maximum position
	float	m_Step;		//	Step size

	ARectI	m_rcBar;	//	Rectangle of progress bar

	A2DSprite*	m_pBackPic;		//	Background picture
	A2DSprite*	m_pBarPic;		//	Progress bar picture
	AUIManager *m_pUIManager;

	bool	m_bSkipRender;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



