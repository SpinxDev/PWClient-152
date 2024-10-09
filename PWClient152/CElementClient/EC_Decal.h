/*
 * FILE: EC_Decal.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "A3DTypes.h"
#include "AAssist.h"
#include "AArray.h"
#include "AList2.h"
#include "EC_Counter.h"

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

class CECViewport;
class A3DFont; 
class A2DSprite;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECDecal
//	
///////////////////////////////////////////////////////////////////////////

class CECDecal
{
public:		//	Types

	//	Class type
	enum
	{
		DCID_DECAL = 0,
		DCID_ICONDECAL,
		DCID_SPRITEDECAL,
		DCID_FACEDECAL,
	};

public:		//	Constructor and Destructor

	CECDecal();
	virtual ~CECDecal();

public:		//	Attributes

public:		//	Operations

	//	Create a decal object according to class ID
	static CECDecal* CreateDecal(int iCID);

	//	Render routine
	virtual bool Render(CECViewport* pViewport) { return true; }

	//	Get class ID
	int GetClassID() { return m_iCID; }
	//	Set / Get position
	void SetPos(const A3DVECTOR3& vPos) { m_vPos = vPos; }
	const A3DVECTOR3& GetPos() { return m_vPos; }

	//	Set color
	void SetColor(A3DCOLOR col)
	{
		//	Set color component and keep alpha component
		m_Color &= 0xff000000;
		m_Color |= (col & 0x00ffffff);
	}

	//	Get color
	A3DCOLOR GetColor() { return m_Color; }

	//	Set alpha
	void SetAlpha(float fAlpha)
	{
		DWORD dwAlpha = (DWORD)(fAlpha * 255.0f);
		if (dwAlpha > 255) dwAlpha = 255;
		m_Color &= 0x00ffffff;
		m_Color |= (dwAlpha << 24);
	}

protected:	//	Attributes

	int			m_iCID;			//	Class ID
	A3DCOLOR	m_Color;		//	Decal color
	A3DVECTOR3	m_vPos;			//	Position

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIconDecal
//	
///////////////////////////////////////////////////////////////////////////

class CECIconDecal : public CECDecal
{
public:		//	Types

	struct ICON
	{
		int		iImageSlot;
		int		iIcon;
		int		iExtX;
		DWORD	dwCol;
	};

public:		//	Constructor and Destructor

	CECIconDecal();
	virtual ~CECIconDecal();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool AddIcons(int iImageSlot, int iNumIcon, int* aIconIdx, A3DCOLOR col);
	bool AddIcon(int iImageSlot, int iIconIdx, A3DCOLOR col);
	bool AddNumIcons(int iImageSlot, int iNumber, A3DCOLOR col);

	//	Render routine
	virtual bool Render(CECViewport* pViewport);
	bool Render(int x, int y, float z);

	//	Set / Get scale on x and y axis
	void SetScaleX(float fScale) { m_fScaleX = fScale; }
	float GetScaleX() { return m_fScaleX; }
	void SetScaleY(float fScale) { m_fScaleY = fScale; }
	float GetScaleY() { return m_fScaleY; }
	//	Enable / Disable screen position
	void EnableScreenPos(bool bEnable) { m_bScreenPos = bEnable; }
	//	Set screen posiiton
	void SetScreenPos(int x, int y) { m_sx = x; m_sy = y; }

protected:	//	Attributes

	AArray<ICON, ICON&>	m_aIcons;	//	Icon indices

	float		m_fScaleX;
	float		m_fScaleY;

	bool		m_bScreenPos;	//	true, use screen position
	int			m_sx;
	int			m_sy;
	int			m_iExtX;
	int			m_iExtY;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSpriteDecal
//	
///////////////////////////////////////////////////////////////////////////

class CECSpriteDecal : public CECDecal
{
public:		//	Types

public:		//	Constructor and Destructor

	CECSpriteDecal();
	virtual ~CECSpriteDecal();

public:		//	Attributes

public:		//	Operations

	//	Change sprite object and icon index
	bool ChangeSpriteIcon(A2DSprite* pA2DSprite, int iIconIdx);

	//	Render routine
	virtual bool Render(CECViewport* pViewport);

	//	Set / Get scale on x and y axis
	void SetScaleX(float fScale) { m_fScaleX = fScale; }
	float GetScaleX() { return m_fScaleX; }
	void SetScaleY(float fScale) { m_fScaleY = fScale; }
	float GetScaleY() { return m_fScaleY; }
	//	Set / Get z position
	void SetZValue(float z) { m_fz = z; }
	float GetZValue() { return m_fz; }
	//	Set / Get position on screen
	void SetScreenPos(int x, int y) { m_ix = x; m_iy = y; }
	void GetScreenPos(int* px, int* py) { *px = m_ix; *py = m_iy; }
	//	Get extent
	void GetExtent(int* pix, int* piy) { *pix = m_iExtX, *piy = m_iExtY; }

protected:	//	Attributes

	A2DSprite*	m_pA2DSprite;
	int			m_iIconIdx;

	float		m_fScaleX;
	float		m_fScaleY;
	int			m_ix;
	int			m_iy;
	float		m_fz;
	int			m_iExtX;		//	Decal extent
	int			m_iExtY;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECFaceDecal
//	
///////////////////////////////////////////////////////////////////////////

class CECFaceDecal : public CECSpriteDecal
{
public:		//	Types

public:		//	Constructor and Destructor

	CECFaceDecal();
	virtual ~CECFaceDecal();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(int nEmotionSet, int iFace);

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport);

	//	Set / Get scale on x and y axis
	void SetScaleX(float fScale) { m_fScaleX = fScale; }
	float GetScaleX() { return m_fScaleX; }
	void SetScaleY(float fScale) { m_fScaleY = fScale; }
	float GetScaleY() { return m_fScaleY; }
	//	Set / Get z position
	void SetZValue(float z) { m_fz = z; }
	float GetZValue() { return m_fz; }
	//	Set / Get position on screen
	void SetScreenPos(int x, int y) { m_ix = x; m_iy = y; }
	void GetScreenPos(int* px, int* py) { *px = m_ix; *py = m_iy; }
	//	Get extent
	void GetExtent(int* pix, int* piy) { *pix = m_iExtX, *piy = m_iExtY; }

protected:	//	Attributes

	int			m_iStartFrame;
	int			m_iNumFrame;
	int			m_iFrameCnt;
	int			m_pFrameTick[40];

	int			m_iFace;
	CECCounter	m_AnimCnt;		//	Animation time counter
	
protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECBubbleDecal
//	
///////////////////////////////////////////////////////////////////////////

class CECBubbleDecal
{
public:		//	Types

	friend class CECDecalMan;
	
public:		//	Constructor and Destructor

	CECBubbleDecal(int iDCID);
	virtual ~CECBubbleDecal();

public:		//	Attributes

public:		//	Operations

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render(CECViewport* pViewport);

	//	Set / Get position
	void SetPos(const A3DVECTOR3& vPos) { m_pDecal->SetPos(vPos); }
	const A3DVECTOR3& GetPos() { return m_pDecal->GetPos(); }
	//	Get decal object
	CECDecal* GetDecal() { return m_pDecal; }

	//	Move decal from one place to another
	void SetMoveInfo(const A3DVECTOR3& vVel, DWORD dwMoveTime);
	//	Is latent ?
	bool IsLatent() { return m_bLatent; }
	//	Set latent flag
	void SetLatentFlag(bool bLatent) { m_bLatent = bLatent; }
	//	Set life time
	void SetLifeTime(DWORD dwLife) { m_LifeCnt.SetPeriod(dwLife); }
	//	Life time end ?
	bool IsDead() { return m_LifeCnt.IsFull() ? true : false; }

protected:	//	Attributes

	CECDecal*	m_pDecal;		//	Decal object
	A3DVECTOR3	m_vMoveVel;		//	Move velocity
	CECCounter	m_MoveCnt;		//	Move time counter
	CECCounter	m_LifeCnt;		//	Life time counter
	bool		m_bLatent;		//	Latent flag

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECBubbleDecalList
//	
///////////////////////////////////////////////////////////////////////////

class CECBubbleDecalList
{
public:		//	Types

public:		//	Constructor and Destructor

	CECBubbleDecalList();
	virtual ~CECBubbleDecalList();

public:		//	Attributes

public:		//	Operations

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);

	//	Add a bubble decal
	CECBubbleDecal* AddDecal(const A3DVECTOR3& vPos, int iDCID);

	void ClearDecalList();

	DWORD GetIntervalTime() const { return m_dwInterval; }
	void SetIntervalTime(DWORD dwTime) { m_dwInterval = dwTime; }
	DWORD GetLifeTime() const { return m_dwLifeTime; }
	void SetLifeTime(DWORD dwTime) { m_dwLifeTime = dwTime; }
	float GetSpeed() const { return m_fSpeed; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	DWORD GetMoveTime() const { return m_dwMoveTime; }
	void SetMoveTime(DWORD dwTime) { m_dwMoveTime = dwTime; }

protected:	//	Attributes

	DWORD	m_dwInterval;		//	Decal popup interval time
	DWORD	m_dwLifeTime;		//	Decal life time
	float	m_fSpeed;			//	Decal move speed
	DWORD	m_dwMoveTime;		//	Decal move time
	DWORD	m_dwTimeCnt;		//	Time counter

	APtrList<CECBubbleDecal*>	m_DecalList;	//	Latent decals

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



