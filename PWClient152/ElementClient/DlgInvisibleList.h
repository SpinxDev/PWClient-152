// Filename	: DlgInvisibleList.h
// Creator	: Xu Wenbin
// Date		: 2011/7/29

#pragma once

#include "DlgBase.h"
#include <AUIListBox.h>

namespace	S2C
{
	struct cmd_invisible_obj_list;
}

class CDlgInvisibleList : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgInvisibleList();

	void UpdateList(const S2C::cmd_invisible_obj_list *pCmd);

protected:
	virtual bool OnInitDialog();
	virtual void OnTick();

	void UpdateUI();
	bool AppendLine(PAUILISTBOX pLstBox, int id);
	void UpdateLine(PAUILISTBOX pLstBox, int line);

private:

	//	列表每列的含义
	enum LST_COLUMN_TYPE
	{
		LST_COLUMN_NAME,
	};

	//	列表每行所存自定义数据
	enum LST_DATA_INDEX
	{
		LST_INDEX_PLAYER,				//	id
		LST_INDEX_NEED_PLAYER,	//	是否需要查询玩家名称
	};

	//	界面控件
	PAUILISTBOX							m_pLst_Player;

	//	不可见玩家的信息
	struct PlayerInfo 
	{
		int	id;
	};
	typedef abase::vector<PlayerInfo>	PlayerList;
	PlayerList									m_playerList;

	DWORD									m_dwOutOfDateMoment;			//	上次更新的列表失效时刻
};
