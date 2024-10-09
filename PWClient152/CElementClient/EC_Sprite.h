/*
 * FILE: EC_Sprite.h
 *
 * DESCRIPTION: Sprite follow a player
 *
 * CREATED BY: Hedi, 2005/12/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EC_SPRITE_H_
#define _EC_SPRITE_H_

#include "A3DTypes.h"
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

class CECModel;
class CECPlayer;
class CECViewport;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECFace
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// 
// 精灵的类别
//
///////////////////////////////////////////////////////////////////////////
class CECSprite
{
public:
	enum SPRITE_ID
	{
		SPRITE_NULL = 0,		// 无
		SPRITE_GOD1,			// 仙精灵1
		SPRITE_GOD2,			// 仙精灵2
		SPRITE_GOD3,			// 仙精灵3
		SPRITE_EVIL1,			// 魔精灵1
		SPRITE_EVIL2,			// 魔精灵2
		SPRITE_EVIL3,			// 魔精灵3
	};

	enum SPRITE_STATE
	{
		SPRITE_STATE_NULL = 0,	// 无
		SPRITE_STATE_IDLE,		// 歇着
		SPRITE_STATE_CHASE,		// 追主人
		SPRITE_STATE_TRICK1,	// 第一种花样
		SPRITE_STATE_TRICK2,	// 第二种花样
		SPRITE_STATE_TRICK3,	// 第三种花样
		SPRITE_STATE_TRICK4,	// 第四种花样
		SPRITE_STATE_TRICK5,	// 第五种花样
		SPRITE_STATE_TRICK6,	// 第六种花样
	};

private:
	SPRITE_ID		m_idSprite;			// sprite type;
	A3DCOLOR		m_color;			// sprite color
	CECPlayer *		m_pPlayer;			// player that this sprite is following
	CECModel *		m_pModelSprite;		// sprite model

	SPRITE_STATE	m_curState;			// current state;
	DWORD			m_dwStateTicks;		// current state ticks.
	DWORD			m_dwIdleAllowed;	// idle ticks that can be allowed

	A3DVECTOR3		m_vecPos;			// current position
	A3DVECTOR3		m_vecDir;			// current direction

	float			m_vSpeed;			// current moving speed;
	float			m_vSpeedMax;		// max speed that can use
	float			m_vAcceleration;	// acceleration
	float			m_vDeceleration;	// deceleration

public:		//	Constructor and Destructor
	
	CECSprite();
	virtual ~CECSprite();

	inline const A3DVECTOR3& GetPos()				{ return m_vecPos; }
	inline const A3DVECTOR3& GetDir()				{ return m_vecDir; }
	inline SPRITE_ID GetSpriteID()					{ return m_idSprite; }
	inline A3DCOLOR GetSpriteColor()				{ return m_color; }
	inline CECModel* GetModel()						{ return m_pModelSprite;}
public:		//	Operations

	//	Initialize object
	bool Init(SPRITE_ID idSprite, CECPlayer * pPlayer);
	//	Release object
	void Release();

	//	Tick routinue
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render(CECViewport* pViewport);

	//	Set target position
	bool SetPos(const A3DVECTOR3& vecPos);
	//  Set transparent
	void SetTransparent(float fTransparent);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif//_EC_SPRITE_H_

