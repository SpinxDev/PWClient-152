/*
 * FILE: EC_ImageRes.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "A3DTypes.h"
#include "AArray.h"

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
class A2DSpriteBuffer;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECImageRes
//	
///////////////////////////////////////////////////////////////////////////

class CECImageRes
{
public:		//	Types

	//	Image index
	enum
	{
		IMG_POPUPNUM = 0,
		IMG_HITMISSED,
		IMG_FACTION,
		IMG_PATEQUEST,
		IMG_LEVELUP,
		IMG_GOTEXP,
		IMG_GOTMONEY,
		IMG_DEADLYSTRIKE,
		IMG_GOTSP,
		IMG_INVALIDHIT,
		IMG_TEAMLEADER,
		IMG_BOOTHBAR,
		IMG_HPWARN,
		IMG_MPWARN,
		IMG_RETORT,
		IMG_IMMUNE,
		IMG_TEAMMATE,
		IMG_PKSTATE,
		IMG_GMFLAG,
		IMG_ATTACKLOSE,
		IMG_SUCCESS,
		IMG_REBOUND,
		IMG_BEAT_BACK,
		IMG_ADD,
		IMG_DODGE_DEBUFF,
		IMG_KING,
		NUM_IMAGE,
	};

	//	Images
	struct IMAGE
	{
		A2DSprite*	pImage;		//	Image
		int			iNumItem;	//	Number of image item
		int			iCurItem;	//	Current item
		int			iWidth;		//	Image width
		int			iHeight;	//	Image height

		A2DSpriteBuffer*	pImageBuf;	//	Image buffer
	};

	//	Registered draw
	struct REGDRAW
	{
		DWORD	dwSlot;
		int		x;
		int		y;
		DWORD	dwCol;
		int		iItem;
		float	z;
		int		iDstWid;
		int		iDstHei;
	};

public:		//	Constructor and Destructor

	CECImageRes();
	virtual ~CECImageRes();

public:		//	Attributes

public:		//	Operations

	//	Load all images
	bool LoadAllImages();
	//	Release all images
	void Release();

	//	Get image item size
	void GetImageItemSize(DWORD dwSlot, int iItem, int* piWid, int* piHei);
	//	Draw image item to backbuffer
	void DrawImage(DWORD dwSlot, int x, int y, A3DCOLOR col, int iItem=0, float z=0.0f, int iDstWid=0, int iDstHei=0);
	//	Draw image item to backbuffer without scale
	void DrawImageNoScale(DWORD dwSlot, int x, int y, A3DCOLOR col, int iItem=0, float z=0.0f);
	//	Flush all rendered images
	void Flush();

	//	Register draw
	void RegisterDraw(DWORD dwSlot, int x, int y, A3DCOLOR col, int iItem=0, float z=0.0f, int iDstWid=0, int iDstHei=0);
	//	Present all registered draws
	void PresentDraws();

protected:	//	Attributes

	IMAGE	m_aImages[NUM_IMAGE];	//	Image array

	APtrArray<REGDRAW*>		m_aRegDraws;	//	Registered draw

protected:	//	Operations

	//	Release speicfied image
	void ReleaseImage(DWORD dwSlot);

	//	Load image from file
	bool LoadImage(DWORD dwSlot, char* szFile, int iWidth, int iHeight, int iNumItem, A3DRECT* aRects, int iNumElem=0);
	//	Load image which has only one item
	bool LoadImage(DWORD dwSlot, char* szFile, int iWidth, int iHeight, int iNumElem=0);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



