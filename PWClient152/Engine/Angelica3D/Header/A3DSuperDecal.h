/*
 * FILE: A3DSuperDecal.h
 *
 * DESCRIPTION: Routines for super decal which has more properties then normal decal
 *
 * CREATED BY: duyuxin, 2001/12/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSUPERDECAL_H_
#define _A3DSUPERDECAL_H_

#include "A3DTypes.h"
#include "A3DDecal.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define MAXNUM_SDKEYPOINT	16	//	Maximum number of super decal key point

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

struct SUPDECALKEYPOINT
{
	float		fWidth;		//	Width size
	float		fHeight;	//	Height size
	BYTE		r, g, b, a;	//	Color
	float		fRotate;	//	Rotate angle in degree
	A3DVECTOR3	vPos;		//	Position
	int			iTickStamp;	//	Tick stamp of this key point
};

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

class AFile;
class A3DDevice;

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSuperDecal
//
///////////////////////////////////////////////////////////////////////////

class A3DSuperDecal : public A3DDecal
{
public:		//	Types

	struct STEPLENGTH
	{
		float		fWidth;		//	Width size
		float		fHeight;	//	Height size
		float		r, g, b, a;	//	Color
		float		fRotate;	//	Rotate angle in degree
		A3DVECTOR3	vPos;		//	Position
	};

	//	State
	enum
	{
		ST_STOP		= 0,
		ST_PLAY		= 1,
		ST_PAUSE	= 2
	};

public:		//	Constructors and Destructors

	A3DSuperDecal();
	virtual ~A3DSuperDecal();

public:		//	Attributes

public:		//	Operations

	bool		Init(A3DDevice* pDevice, char* szTexName);	//	Initialize object
	void		Release();		//	Release object

	bool		SetTextureProp(int iRow, int iCol, int iInterval);			//	Set texture properties
	void		GetTextureProp(int* piRow, int* piCol, int* piInterval);	//	Get texture properties 
	bool		SetKeyPoint(int iIndex, SUPDECALKEYPOINT& KeyPoint);		//	Set a key point
	bool 		GetKeyPoint(int iIndex, SUPDECALKEYPOINT* pKeyPoint);		//	Get a key point

	void		Play();				//	Begin to play
	void		Pause(bool bPause);	//	Pause
	void		Stop();				//	Stop playing
	bool		TickAnimation();	//	Update function, called every tick
	virtual bool Render(A3DViewport* pView, bool bCheckVis=true); // Rneder function;

	void		SetKeyPointNum(int iNum)	{	if (iNum >= 1 && iNum <= MAXNUM_SDKEYPOINT)	m_iNumKey = iNum;	}
	int			GetKeyPointNum()			{	return m_iNumKey;	}

	void SetRepeatTime(int iTime)	
	{	
		m_bTexRepeat = iTime < 0 ? true : false;
		m_iRepTime = abs(iTime);
	}

	int	GetRepeatTime() { return (m_bTexRepeat ? -1 : 1) * m_iRepTime; }

	int			GetCurrentLoop()			{	return m_iRepCnt;	}
	void		SetScale(float fScale)		{	m_fScale = fScale;	}
	float		GetScale()					{	return m_fScale;	}
	bool		IsFinish()					{	return m_bFinish;	}
	float		GetSize()					{	return m_fSize; }

protected:	//	Attributes

	bool		m_bFinish;			//	Finish flag

	int			m_iLifeCnt;			//	Life counter
	int			m_iCurTick;			//	Current tick
	int			m_iRepTime;			//	Repeat time
	int			m_iRepCnt;			//	Repeat count
	bool		m_bTexRepeat;		//	Repeat Texture when playing sequence
	int			m_iState;			//	State
	float		m_fScale;			//	Size scale

	float		m_fSize;			//	Size of this decal;

	SUPDECALKEYPOINT	m_aKeys[MAXNUM_SDKEYPOINT];	//	Properties key points
	STEPLENGTH			m_CurProp;	//	Current properties
	STEPLENGTH			m_Step;		//	Step length of each property

	int			m_iNumKey;			//	Keys used in fact
	int			m_iCurKey;			//	Current key
	int			m_iNextKey;			//	Next key

	int			m_iTexInterval;		//	Tick interval between two textures tile
	int			m_iTexRow;			//	Rows of texture tile
	int			m_iTexCol;			//	Columns of texture tile
	int			m_iCurTexRow;		//	Current row
	int			m_iCurTexCol;		//	Current column

protected:	//	Operations

	void		CalculateStepLength();	//	Calculate step length
	int			GetNextKeyIndex();		//	Get next key's index

public:
	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pDevice, AFile * pFileToLoad);
};


#endif	//	_A3DSUPERDECAL_H_


