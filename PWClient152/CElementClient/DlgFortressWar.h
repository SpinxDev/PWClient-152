// Filename	: DlgFortressWar.h
// Creator	: Xu Wenbin
// Date		: 2010/09/29

#pragma once

#include "DlgBase.h"
#include "AUIListbox.h"

namespace GNET
{
	class FactionFortressList_Re;
	class FactionFortressChallenge_Re;
	class FactionFortressGet_Re;
}

class CDlgFortressWar : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgFortressWar();
	
	void OnCommand_CANCEL(const char *szCommand);
	void OnCommand_Prev(const char *szCommand);
	void OnCommand_Next(const char *szCommand);
	void OnCommand_Refresh(const char *szCommand);
	void OnCommand_Search(const char *szCommand);
	void OnCommand_DeclareWar(const char *szCommand);
	
	void OnEventLButtonDown_Lst_Fortress(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void OnPrtcFactionFortressList_Re(const GNET::FactionFortressList_Re *);
	void OnPrtcFactionFortressChallenge_Re(const GNET::FactionFortressChallenge_Re *);
	void OnPrtcFactionFortressGet_Re(const GNET::FactionFortressGet_Re *);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void Refresh();							//	根据当前页码设定，更新列表
	void OnListSelChange();					//	基地列表选择变化导致界面更新
	int  GetDeclareWarTarget();				//	获取当前选中的可宣战目标基地
	bool GetFactionName(unsigned int uID, ACString &strName, bool bRequestFromServer);
	ACString GetBuildingString(const void *pBuf, size_t sz);

	enum { FORTRESS_LIST_SIZE = 16 };		//	基地列表每页的大小

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
		ST_INIT,			//	正在初始化
		ST_OPEN,			//	已开放
		ST_CHALLENGE,		//	宣战阶段
		ST_BATTLE_WAIT,		//	等待战斗阶段
		ST_BATTLE,			//	战斗阶段
	};
	
private:
	PAUIOBJECT	m_pEdit_Name;		//	直接查询帮派目标信息（以宣战）
	PAUILISTBOX	m_pLst_Fortress;	//	帮派基地列表
	PAUIOBJECT	m_pBtn_DeclareWar;	//	宣战按钮

	int			m_nBegin;			//	起始页项
	FORTRESS_STATE	m_status;		//	当前页的基地状态
	bool		m_bAllInfoReady;	//	当前列表中是否所有帮派信息都完整了
};