// Filename	: DlgGMConsoleForbid.h
// Creator	: Wang Yongdong
// Date		: 2012/04/26

#pragma once

#include "DlgBase.h"
#include "AUIListBox.h"
#include "AUIEditBox.h"
#include "AUILabel.h"
#include "AUISlider.h"
#include <hashmap.h>

namespace GNET
{
	class GMGetPlayerConsumeInfo_Re;
}

class GMCDBMan;

class CDlgGMConsoleForbid : public CDlgBase  
{
public:
	
	enum
	{
		CONSUMEINFO_EMPTY = 0,
		CONSUMEINFO_DONE
	};
	enum
	{
		LISTINDEX_ID = 0,
		LISTINDEX_DONE ,
	};
	
	struct player_info
	{
		player_info(){ id = 0;level =0;prof =0; bLoadFlag = false;ip =0; fCash_add = 0.0f;fContrib = 0.0f;avg_time = 0; dist = 0.0f;}
		
		bool bLoadFlag;
		int id;
		short level;
		short prof;		
		int ip;
//		int consume;
		float fCash_add;
//		int contrib;
		float fContrib;
		int avg_time;	
		float dist;
		ACString name;
	};
	typedef abase::hash_map<int,player_info*> PlayerInfoMap;

private:
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgGMConsoleForbid();
	virtual ~CDlgGMConsoleForbid();

	virtual void DoDataExchange(bool bSave);

	void OnCommand_Name(const char *szCommand);
	void OnCommand_Job(const char *szCommand);
	void OnCommand_Level(const char* szCommand);
	void OnCommand_IPAddr(const char* szCommand);
	void OnCommand_Pay(const char* szCommand);
	void OnCommand_Contri(const char* szCommand);
	void OnCommand_PTime(const char* szCommand);
	void OnCommand_ChangeRange(const char *szCommand);
	void OnCommand_ForbidUser(const char* szCommand);

	void ReleasePlayerInfoMap();
	void GetPlayerInfoMap();
	void GetPlayerIDVecByDist(float dist, abase::vector<int>& playervec);

	
	void ForbidRole(int id);
	void ForbidMultiRole();
	void OnPrtcGMGetConsumeInfo_Re(GNET::GMGetPlayerConsumeInfo_Re *pProtocol);

protected:
	void FillPlayerList(float fDist);
	void GetLineString(player_info* pInfo,ACString& str);

	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();

private:
	PAUILISTBOX	m_pList_Info;
	PAUIEDITBOX m_pTimeEdit;
	PAUIEDITBOX m_pReasonEdit;
	PAUILABEL m_pLabelMax;
	PAUILABEL m_pLabelMin;
	PAUISLIDER m_pSliderRange;

	int m_iDist;
	int m_duration;
	ACString m_reason;

	PlayerInfoMap m_playerInfoMap;

	bool m_bTickFlag;
};

//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////

class CGMCommandKickOutRole_Consume
{
public:
	CGMCommandKickOutRole_Consume(const CDlgGMConsoleForbid::player_info* pInfo,int duration,ACString reason);
	virtual bool Execute(ACString &strErr, ACString &strDetail);
	virtual ACString GetDisplayName();

protected:
	const CDlgGMConsoleForbid::player_info* m_pInfo;
	int m_duration;
	ACString m_reason;
};