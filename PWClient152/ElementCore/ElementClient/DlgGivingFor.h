/********************************************************************
	created:	2013/3/1
	created:	12:11:2012   16:31
	file base:	DlgGivingFor
	file ext:	h
	author:		zhougaomin01305
	
*********************************************************************/


#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUIEditBox.h"
#include "AUIListBox.h"
#include "gnetdef.h"
#include "Alist2.h"
#include <list>
#include "EC_GFXCaster.h"

namespace GNET
{
	class Protocol;
	class Octets;
}

struct SortData
{
	ACString roleName;
	int roleid;
	int level;
	int prof;
	ACString showString;
};

// 商城赠送索取界面
class CDlgGivingFor : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();
protected:
	enum
	{
		FRIEND_TYPE,
		FACTION_TYPE,
		UPPER_GIVING = 20,
		UPPER_ASKFOR = 5,
	};
	PAUIEDITBOX m_pEditBoxName;
	PAUILISTBOX m_pListBoxLeft;
	PAUILISTBOX m_pListBoxRight;
	PAUILABEL   m_pLblTitle;
	PAUILABEL   m_pLblUpper;
	PAUISTILLIMAGEBUTTON m_pBtnConfirm;
	PAUISTILLIMAGEBUTTON m_pBtnSearch;
	int m_iPlayerType;
	bool m_bGiving;
	int m_itemId;
	int m_price;
	int m_index;
	int m_slot;
	abase::vector<int> m_mailAskForDatas;
	bool m_bShowGfx;

	std::list<SortData> m_leftSortDatas;
	std::list<SortData> m_rightSortDatas;

protected:

	virtual void OnShowDialog();
	virtual bool OnInitDialog();

	void AddPlayerInfo(bool bLeft, int roleid, const ACHAR* szRoleName, int level, int profession,bool bOnline = true);
	void RemovePlayerInfo(int index, bool bLeft);
	void BuildLeftList();
	void UpdateUpper();
	void DoSendGivePresentProtocol();
	void DoSendAskForPresentProtocol();
	bool AddFriendFactionRole(int roleid, bool bLeft);
	void MultiSelectDeal(bool bLeft);
	void AddSearchedPlayer(int roleid, const ACHAR *szName);
public:
	void ShowGfx(bool bSend);
	void BuildFactionList();
	void PlayerNameUpdate(int roleid, ACString strNewName, int level, int profession);
	bool CheckAskForTaskLimitedItem(int itemid);
	void SetMailAskForData(int roleid, int mailid, int itemid, int index, int slot);
	void SetType(bool bGiving, int itemid, int price, int index, int slot);
	void DoSendProtocol();
	void DoSendMailGivingFor(bool bSend);

	void OnCommandSearch(const char * szCommand);
	void OnCommandRightTurn(const char *szCommand);
	void OnCommandLeftTurn(const char *szCommand);
	void OnCommandConfirm(const char *szCommand);
	void OnCommandRadioFriend(const char *szCommand);

	void OnEventLButtonDBClick_LeftList(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDBClick_RightList(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void OnPrtcPresentRe(GNET::Protocol* pProtocol);

	void OnPrtcGetPlayerIDByNameRe(GNET::Protocol* pProtocol);

	void OnCommandCancel(const char * szCommand);

	void OnCommandLeftSortName(const char * szCommand);
	void OnCommandLeftSortLevel(const char * szCommand);
	void OnCommandLeftSortProf(const char * szCommand);

	void OnCommandRightSortName(const char * szCommand);
	void OnCommandRightSortLevel(const char * szCommand);
	void OnCommandRightSortProf(const char * szCommand);

	void ResetList(bool bLeft,bool bUp);

	CDlgGivingFor();
	virtual ~CDlgGivingFor();
};
