/*
 * FILE: EC_Matter.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_Object.h"
#include "EC_MsgDef.h"
#include "EC_GPDataType.h"
#include "A3DGeometry.h"
#include "aabbcd.h"

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
class CECModel;
class A3DGFXEx;
class CECPateText;

using namespace CHBasedCD;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECMatter
//	
///////////////////////////////////////////////////////////////////////////

class CECMatter : public CECObject
{
public:		//	Types

	//	Matter information got from server
	struct INFO
	{
		int		mid;	//	Matter id
		int		tid;	//	Template id
	};

	//	Matter type
	enum
	{
		MATTER_UNKNOWN		= 0,
		MATTER_ITEM			= 1,
		MATTER_MINE			= 2,
		MATTER_MONEY		= 3,
		MATTER_TYPEMASK		= 0xff,

		//	Item matter flags
		ITEMFLAG_EXTPROP	= 0x0100,
		ITEMFLAG_SUIT		= 0x0200,
		ITEMFLAG_UNIQUE		= 0x0400,
		ITEMFLAG_MYTHICAL	= 0x0800,
		ITEMFLAG_RARE		= 0x1000,
		ITEMFLAG_REWARD		= 0X2000,	//	Task reward
	};

	friend class CECMatterMan;

public:		//	Constructor and Destructor

	CECMatter(CECMatterMan* pMatterMan);
	virtual ~CECMatter();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(const S2C::info_matter& Info);
	//	Release object
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport) { return InternalRender(pViewport, false); }
	bool RenderHighLight(CECViewport* pViewport) { return InternalRender(pViewport, true); }
	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg);

	//	Set absolute position
	virtual void SetPos(const A3DVECTOR3& vPos);
	//	Set absolute forward and up direction
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);

	//	Set loaded model to matter object, this function is used in multithread loading process
	bool SetLoadedMatterModel(CECModel* pModel);

	//	Get matter information got from server
	INFO& GetMatterInfo() { return m_MatterInfo; }
	//	Get matter ID
	int GetMatterID() { return m_MatterInfo.mid; }
	//	Get matter template ID
	int GetTemplateID() { return m_MatterInfo.tid; }
	//	Get matter's aabb
	const A3DAABB& GetMatterAABB() { return m_aabb; } 
	//	Get distance to host player
	float GetDistToHost() { return m_fDistToHost; }
	float GetDistToHostH() { return m_fDistToHostH; }
	//	Get distance to camera
	float GetDistToCamera() { return m_fDistToCamera; }
	//	Set / Get name rendering flag
	void SetRenderNameFlag(bool bTrue) { m_bRenderName = bTrue; }
	bool GetRenderNameFlag() { return m_bRenderName; }
	//	Get level requirement
	int GetLevelReq() { return m_iLevelReq; }

	//	Is this matter a item ?
	bool IsItem() { return (m_dwMatterType & MATTER_TYPEMASK) == MATTER_ITEM; }
	//	Is this matter money ?
	bool IsMoney() { return (m_dwMatterType & MATTER_TYPEMASK) == MATTER_MONEY; }
	//	Is this matter mine ?
	bool IsMine() { return (m_dwMatterType & MATTER_TYPEMASK) == MATTER_MINE; }
	bool IsMonsterSpiritMine() { return m_bMonsterSpiritMine; }
	//	Is dyn model ?
	bool IsDynModel() const { return m_bDynModelObj; }

	//	Get matter name
	const wchar_t* GetName() { return m_szName; }
	//	Get model
	CECModel* GetModel() { return m_pModel; }

	bool TraceWithBrush(BrushTraceInfo * pInfo);

	float GetGatherDist() { return m_fGatherDist;}

	//	Load matter model
	static CECModel* LoadMatterModel(const char* szFile);
	//	Release matter model
	static void ReleaseMatterModel(CECModel* pModel);

protected:	//	Attributes

	CECMatterMan*	m_pMatterMan;		//	Matter manager
	INFO			m_MatterInfo;		//	Matter information got from server
	CECModel*		m_pModel;			//	Model object
	A3DGFXEx*		m_pGfx;				//	A3DGFXEx object 
	A3DAABB			m_aabb;				//	Matter's aabb
	DWORD			m_dwMatterType;		//	Matter flags
	int				m_iLevelReq;		//	Level requirement, used by mine

	float			m_fDistToHost;		//	Distance to host player
	float			m_fDistToHostH;		//	Horizontal distance to host player
	float			m_fDistToCamera;	//	Distance to camera

	const char*		m_szModelFile;		//	Matter model file name
	const wchar_t*	m_szName;			//	Matter name
	bool			m_bDynModelObj;

	bool			m_bRenderName;		//	Render name flag
	CECPateText*	m_pPateName;		//	NPC name

	int				m_nBrushes;	//	number of brush object used in collision
	CCDBrush**		m_ppBrushes;	//	Brush object used in collision
	bool			m_bMonsterSpiritMine;
	float			m_fGatherDist;

protected:	//	Operations

	//	Internal render routine
	bool InternalRender(CECViewport* pViewport, bool bHighLight);
	//	Render titles / names / talks above player's header
	bool RenderName(CECViewport* pViewport);
	//	Read matter data from database
	bool ReadDataFromDatabase(int tid);
	//	Load gfx from file
	bool LoadGFXFromFile(const char* szFile, BYTE byScale);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



