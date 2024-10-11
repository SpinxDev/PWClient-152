/*
 * FILE: AUIClockIcon.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUICLOCKICON_H
#define _AUICLOCKICON_H

#include "A3DVector.h"

class A3DTexture;
class A3DStream;

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
//	Class AUIClockIcon
//	
///////////////////////////////////////////////////////////////////////////

enum 
{
	AUICLOCK_ROTATE,
	AUICLOCK_UP,
	AUICLOCK_DOWN
};

class AUIClockIcon
{
public:		//	Types

public:		//	Constructor and Destructor

	AUIClockIcon();
	virtual ~AUIClockIcon();

public:		//	Attributes

public:		//	Operations

	bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char* pszClockImage = NULL);
	void Release();

	bool SetClockImage(const char* pszClockImage);

	//	Render routine
	bool Render();

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

	//	Set ClockType
	void SetClockType(int iType) {m_iClockType = iType;}
	int  GetClockType() {return m_iClockType;}

protected:
	bool RenderRotate();

	bool RenderUpdown();

protected:	//	Attributes

	A3DEngine *m_pA3DEngine;
	A3DDevice *m_pA3DDevice;
	A3DTexture *m_pA3DTexture;
	A3DStream *m_pA3DStream;

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

	int		m_iClockType;

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

#endif //	_AUICLOCKICON_H