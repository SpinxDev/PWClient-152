/*
 * FILE: EC_ManNPC.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_Manager.h"
#include "EC_GPDataType.h"
#include "EC_Counter.h"
#include "hashtab.h"
#include "AArray.h"
#include "aabbcd.h"
#include "EC_Observer.h"

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
class A3DSkin;
class CECNPC;
class CECPet;

class CECFactionMan;
typedef CECObserver<CECFactionMan>	CECFactionManObserver;

using namespace CHBasedCD;

//	NPC laod result structure used by CECNPC::LoadNPCModel
struct EC_NPCLOADRESULT
{
	CECModel*	pModel;
	A3DSkin*	pSkin;
	A3DSkin*	pLowSkin;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECNPCMan
//	
///////////////////////////////////////////////////////////////////////////

class CECNPCMan : public CECManager, public CECFactionManObserver
{
public:		//	Types

	//	Loaded NPC model information
	struct NPCMODEL
	{
		int	nid;	//	NPC's id
		DWORD dwBornStamp;
		EC_NPCLOADRESULT Ret;	//	Model loaded result
	};

	typedef abase::hashtab<CECNPC*, int, abase::_hash_function>	NPCTable;
	typedef abase::hashtab<int, int, abase::_hash_function> UkNPCTable;

public:		//	Constructor and Destructor

	CECNPCMan(CECGameRun* pGameRun);
	virtual ~CECNPCMan();

public:		//	Attributes

public:		//	Operations

	//	Release manager
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport);
	//	RenderForReflected routine
	virtual bool RenderForReflect(CECViewport * pViewport);
	//	RenderForRefract routine
	virtual bool RenderForRefract(CECViewport * pViewport);

	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg);

	//	On entering game world
	virtual bool OnEnterGameWorld();
	//	On leaving game world
	virtual bool OnLeaveGameWorld();
	
	//	Ray trace
	virtual bool RayTrace(ECRAYTRACE* pTraceInfo);
	
	//	派生自 CECFactionManObserver
	virtual void OnModelChange(const CECFactionMan *p, const CECObservableChange *q);

	// Trace for CD
	bool TraceWithBrush(BrushTraceInfo * pInfo);

	//	Get NPC of specified ID
	CECNPC* GetNPC(int nid, DWORD dwBornStamp=0);
	CECNPC* GetNPCFromAll(int nid);
	//	Get NPC's name
	const wchar_t* GetNPCName(int nid);
	//	Get NPC number
	int GetNPCNum() { return (int)m_NPCTab.size(); }
	//	Get NPCs should appear in mini-map
	const APtrArray<CECNPC*>& GetNPCsInMiniMap() { return m_aMMNPCs; }
	//	When world's loading center changed
	void OnLoadCenterChanged();
	//	Get pet of specified nid
	CECPet* GetPetByID(int nid);
	
	//	Get npc candidates whom can be auto-selected by 'TAB' key
	void TabSelectCandidates(int idCurSel, APtrArray<CECNPC*>& aCands);

	//	Load NPC model in loading thread
	bool ThreadLoadNPCModel(int nid, DWORD dwBornStamp, int tid, const char* szFile);
	
	bool ISNPCModelNeedLoad(int nid, DWORD dwBornStamp);

	void OptimizeShowExtendStates();
	void OptimizeWeaponStoneGfx();
	void OptimizeArmorStoneGfx();
	void OptimizeSuiteGfx();

	CECNPC* FindNPCByTempleID(int templeId);	//找到具有相同模板ID的NPC，如有多个返回找到的第一个
	CECNPC* GetMouseOnPateTextNPC(int x, int y);

protected:	//	Attributes

	NPCTable	m_NPCTab;		//	NPC table
	UkNPCTable	m_UkNPCTab;		//	Unknown NPC table
	CECCounter	m_QueryCnt;		//	Query unknown NPC time counter

	APtrArray<CECNPC*>		m_aMMNPCs;			//	NPCs who appear in mini-map
	APtrArray<NPCMODEL*>	m_aLoadedModels;	//	Loaded NPC models
	APtrArray<CECNPC*>		m_aDisappearNPCs;	//	NPCs who will disappear soon
	CRITICAL_SECTION		m_csLoad;			//	Loaded models lock

	APtrArray<CECNPC*>		m_aTabSels;			//	'Tab' key selected monsters

protected:	//	Operations

	//	Create a NPC
	CECNPC* CreateNPC(const S2C::info_npc& Info, bool bBornInSight);
	//	Release a NPC
	void ReleaseNPC(CECNPC* pNPC);
	//	Put NPC into disappear table
	void NPCDisappear(int nid);
	//	One NPC enter view area
	bool NPCEnter(const S2C::info_npc& Info, bool bBornInSight);
	//	One NPC leave view area
	void NPCLeave(int nid, bool bUpdateMMArray=true, bool bRelease=true);
	//	Update NPCs in various ranges (Active, visible, mini-map etc.)
	void UpdateNPCInRanges(DWORD dwDeltaTime);
	//	Remove NPC from m_aMMNPCs array
	void RemoveNPCFromMiniMap(CECNPC* pNPC);
	//	Seek out NPC with specified id, if NPC doesn't exist, try to get from server
	CECNPC* SeekOutNPC(int nid);
	//	Update unknown NPC table
	void UpdateUnknownNPCs();

	//	Remove npc from tab-selected array
	void RemoveNPCFromTabSels(CECNPC* pNPC);

	//	Deliver loaded NPC models
	void DeliverLoadedNPCModels();

	//	Messages handlers
	bool OnMsgNPCInfo(const ECMSG& Msg);
	bool OnMsgNPCMove(const ECMSG& Msg);
	bool OnMsgNPCRunOut(const ECMSG& Msg);
	bool OnMsgNPCDied(const ECMSG& Msg);
	bool OnMsgNPCDisappear(const ECMSG& Msg);
	bool OnMsgNPCOutOfView(const ECMSG& Msg);
	bool OnMsgNPCStopMove(const ECMSG& Msg);
	bool OnMsgInvalidObject(const ECMSG& Msg);
	bool OnMsgForbidBeSelected(const ECMSG& Msg);

	//	Transmit message
	bool TransmitMessage(const ECMSG& Msg);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

