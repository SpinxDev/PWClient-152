// Filename	: DlgFortressWarList.h
// Creator	: Xu Wenbin
// Date		: 2010/10/12

#pragma once

#include "DlgBase.h"
#include "AUIListbox.h"

namespace GNET
{
	class FactionFortressBattleList_Re;
}

class CDlgFortressWarList : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgFortressWarList();
	
	void OnCommand_CANCEL(const char *szCommand);
	void OnCommand_Refresh(const char *szCommand);
	void OnCommand_Enter(const char *szCommand);
	void OnCommand_Lst_GvG(const char *szCommand);

	void OnPrtcFactionFortressBattleList_Re(const GNET::FactionFortressBattleList_Re *);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void OnListSelChange();					//	对战表列表选择变化导致界面更新

	int  GetFortressToEnter();				//	获取当前选中的可进入目标基地
	bool GetFactionName(unsigned int uID, ACString &strName, bool bRequestFromServer);

	enum
	{
		FORTRESS_LIST_OWNER_INDEX = 0,		//	基地拥有帮派id下标
		FORTRESS_LIST_OFFENSE_INDEX = 1,	//	基地进攻方帮派id下标
		FORTRESS_LIST_NAME1_INDEX = 2,		//	基地守方是否正确显示标识下标
		FORTRESS_LIST_NAME2_INDEX = 3,		//	基地攻方是否正确显示标识下标
	};	
	
	//	基地状态
	enum FORTRESS_STATE
	{
		ST_INIT,					//	正在初始化
		ST_OPEN,				//	已开放
		ST_CHALLENGE,		//	宣战阶段
		ST_BATTLE_WAIT,		//	等待战斗阶段
		ST_BATTLE,				//	战斗阶段
	};
	
private:
	PAUILISTBOX	m_pLst_GvG;			//	帮派对战列表
	PAUIOBJECT	m_pBtn_Enter;		//	进入基地按钮
	FORTRESS_STATE	m_status;		//	当前页的基地状态
	bool		m_bAllInfoReady;	//	当前列表中是否所有帮派信息都完整了
};