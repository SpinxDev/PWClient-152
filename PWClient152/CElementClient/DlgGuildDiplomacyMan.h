// Filename	: DlgGuildDiplomacyMan.h
// Creator	: Xu Wenbin
// Date		: 2010/11/10

#pragma once

#include "DlgBase.h"
#include <AUIListBox.h>

namespace GNET
{
	class FactionListRelation_Re;
}

enum FACTIONRELATION_APPLY_TYPE
{
	ALLIANCE_FROM_OTHER = 0,		//	其他人发来的同盟申请
	ALLIANCE_TO_OTHER,				//	发给其他人的同盟申请
	HOSTILE_FROM_OTHER,				//	其他人发来的敌对申请
	HOSTILE_TO_OTHER,				//	发给其他人的敌对申请
	REMOVE_RELATION_FROM_OTHER,		//	其他人发来的解除关系申请
	REMOVE_RELATION_TO_OTHER,		//	发给其他人的解除关系申请
};

class CDlgGuildDiplomacyMan : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgGuildDiplomacyMan();

	void OnCommand_KillAlliance(const char *szCommand);
	void OnCommand_Force_KillAlliance(const char *szCommand);
	void OnCommand_KillHostility(const char *szCommand);
	void OnCommand_Force_KillHostility(const char *szCommand);
	void OnCommand_Reply_Agree(const char *szCommand);
	void OnCommand_Reply_Refuse(const char *szCommand);
	void OnCommand_ApplyFaction(const char *szCommand);
	void OnCommand_ApplyType(const char *szCommand);
	void OnCommand_ApplyEndTime(const char *szCommand);
	void OnCommand_DiplomacyApply(const char *szCommand);
	void OnCommand_Refresh(const char *szCommand);
	void OnCommand_CANCEL(const char *szCommand);

	void OnPrtcFactionListRelation_Re(GNET::FactionListRelation_Re *pProtocol);

	void OnEventLButtonDown_List_Apply(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void Refresh();
	
	//	操作冷却计时，与类CDlgGuildDiplomacyApply共用
	int    GetLastSendTime();
	void SetLastSendTime(int t);
	bool CanSend();
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	enum
	{
		ALLIANCE_OK_INDEX = 0,
		ALLIANCE_FACTION_INDEX = 1,
		ALLIANCE_TIME_INDEX = 2,
		HOSTILITY_OK_INDEX = ALLIANCE_OK_INDEX,
		HOSTILITY_FACTION_INDEX = ALLIANCE_FACTION_INDEX,
		HOSTILITY_TIME_INDEX = ALLIANCE_TIME_INDEX,
		APPLY_OK_INDEX = 0,
		APPLY_FACTION_INDEX = 1,
		APPLY_TYPE_INDEX = 2,
		APPLY_TIME_INDEX = 3,
	};

	bool GetAllianceLineString(int fid, int leftTime, ACString &strText, bool bRequestFromServer = false);
	bool GetHostilityLineString(int fid, int leftTime, ACString &strText, bool bRequestFromServer = false);
	bool GetApplyLineString(int fid, int type, int leftTime, ACString &strText, bool bRequestFromServer = false);

	int	 GetFactionAllianceSel();
	int  GetFactionHostilitySel();
	bool GetReplyFactionAndType(int &applyFaction, int &applyType);
	bool CanDoAction();

private:
	PAUILISTBOX	m_pList_Alliance;		//	当前同盟帮派列表
	PAUILISTBOX	m_pList_Hostility;		//	当前敌对帮派列表
	PAUILISTBOX	m_pList_Apply;			//	待处理的相关申请

	PAUIOBJECT	m_pBtn_Agree;
	PAUIOBJECT	m_pBtn_Refuse;
	PAUIOBJECT	m_pBtn_Alliance01;
	PAUIOBJECT	m_pBtn_Alliance02;
	PAUIOBJECT	m_pBtn_Hostility01;
	PAUIOBJECT	m_pBtn_Hostility02;
	PAUIOBJECT	m_pBtn_DiplomacyApply;

	int						m_iLastSendTime;	//	操作冷却计时器，避免频繁向服务器发送
};