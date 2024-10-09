/*
 * FILE: EC_ClockIcon.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "A3DVector.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECClockIcon
//	
///////////////////////////////////////////////////////////////////////////

class CECClockIcon
{
public:		//	Types

public:		//	Constructor and Destructor

	CECClockIcon();
	virtual ~CECClockIcon();

public:		//	Attributes

public:		//	Operations

	//	Render routine
	void Render();

	//	Set icon position and size
	void MoveIcon(int x, int y, int cx, int cy);
	//	Get icon position
	void GetIconPos(int* px, int* py) { *px=m_x; *py=m_y; }
	//	Set icon position
	void SetIconPos(int x, int y) { m_x=x; m_y=y; OnIconMove(); }
	//	Get icon size
	void GetIconSize(int* picx, int* picy) { *picx=m_cx; *picy=m_cy; }
	//	Set icon size
	void SetIconSize(int cx, int cy) { m_cx=cx; m_cy=cy; OnIconMove(); }
	//	Set z value
	void SetZValue(float z) { m_fz=z; OnIconMove(); }
	//	Get z value
	float GetZValue() { return m_fz; }

	//	Gets the current position. 
	int GetProgressPos() { return m_iPos; }
	//	Gets the lower and upper limits of the range 
	int GetProgressRange(int* piMin, int* piMax) { *piMin=m_iMin; *piMax=m_iMax; }
	//	Advances the current position
	int OffsetProgressPos(int iOffset);
	//	Sets the current position 
	void SetProgressPos(int iPos);
	//	Sets the minimum and maximum ranges
	void SetProgressRange(int iMin, int iMax); 
	//	Specifies the step increment
	void SetProgressStep(int iStep) { m_iStep = iStep; }
	//	Advances the current position by the step increment
	int StepProgress();

	//	Set color
	void SetColor(DWORD dwCol) { m_dwCol = dwCol; }
	//	Get color
	DWORD GetColor() { return m_dwCol; }

protected:	//	Attributes

	static WORD m_aIndices[24];		//	Indices

	int		m_x;		//	Icon position on screen
	int		m_y;
	int		m_cx;		//	Icon size on screen
	int		m_cy;
	float	m_fz;		//	z value
	int		m_iPos;		//	Current position
	int		m_iMin;		//	Minimum position
	int		m_iMax;		//	Maximum position
	int		m_iStep;	//	Step size
	DWORD	m_dwCol;	//	Color of icon

	A3DVECTOR3	m_aPos[10];

protected:	//	Operations

	//	Update icon position and size
	void OnIconMove();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

