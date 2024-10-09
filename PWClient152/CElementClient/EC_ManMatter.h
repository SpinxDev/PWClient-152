/*
 * FILE: EC_ManMatter.h
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

#include "EC_Manager.h"
#include "EC_GPDataType.h"
#include "AArray.h"
#include "hashtab.h"

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

class CECMatter;
class CECModel;

namespace CHBasedCD
{
	class BrushTraceInfo;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECMatterMan
//	
///////////////////////////////////////////////////////////////////////////

class CECMatterMan : public CECManager
{
public:		//	Types

	//	Loaded matter model
	struct MATTERMODEL
	{
		int			mid;		//	Matter ID
		CECModel*	pModel;		//	Matter's model object
	};

	typedef abase::hashtab<CECMatter*, int, abase::_hash_function>	MatterTable;

public:		//	Constructor and Destructor

	CECMatterMan(CECGameRun* pGameRun);
	virtual ~CECMatterMan();

public:		//	Attributes

public:		//	Operations

	//	Release manager
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport);

	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg);

	//	On entering game world
	virtual bool OnEnterGameWorld();
	//	On leaving game world
	virtual bool OnLeaveGameWorld();

	//	Ray trace
	virtual bool RayTrace(ECRAYTRACE* pTraceInfo);

	//	Get matter throuth it's ID
	CECMatter* GetMatter(int mid, DWORD dwBornStamp=0);
	//	Find a matter which is near enough to host player
	CECMatter* FindMatterNearHost(float fDist, bool bPickCheck);
	//  Find some matters which is near enough to host player
	bool FindMattersInRange(float fDist, bool bPickCheck, abase::vector<CECMatter*>& matters);
	//	When world's loading center changed
	void OnLoadCenterChanged();
	//	Remove a matter
	void RemoveMatter(int idMatter) { MatterLeave(idMatter); }
	//	Get matter number
	int GetMatterNum() { return (int)m_MatterTab.size(); }

	//	Load matter model in loading thread
	bool ThreadLoadMatterModel(int mid, const char* szFile);

	bool TraceWithBrush(CHBasedCD::BrushTraceInfo * pInfo);

protected:	//	Attributes

	MatterTable		m_MatterTab;	//	Matter table
	MatterTable		m_DynModelTab;

	AArray<MATTERMODEL, MATTERMODEL&>	m_aLoadedMats;	//	Loaded matter models
	CRITICAL_SECTION					m_csLoad;		//	m_aLoadedMats lock

protected:	//	Operations

	//	Create a matter
	CECMatter* CreateMatter(const S2C::info_matter& Info);
	//	Release a matter
	void ReleaseMatter(CECMatter* pMatter);
	//	One matter enter view area
	bool MatterEnter(const S2C::info_matter& Info);
	//	One matter leave view area
	void MatterLeave(int mid);
	//	Update matters in various ranges (Active, visible, etc.)
	void UpdateMatterInRanges(DWORD dwDeltaTime);

	//	Deliver loaded matter models
	void DeliverLoadedMatterModels();

	//	Messages handlers
	bool OnMsgMatterInfo(const ECMSG& Msg);
	bool OnMsgMatterDisappear(const ECMSG& Msg);
	bool OnMsgMatterEnterWorld(const ECMSG& Msg);
	bool OnMsgMatterOutOfView(const ECMSG& Msg);
	bool OnMsgInvalidObject(const ECMSG& Msg);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

