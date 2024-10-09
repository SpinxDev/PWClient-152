// Filename	: DlgTokenShop.h
// Creator	: Han Guanghui
// Date		: 2013/10/12

#pragma once

#include "DlgBase.h"
#include "hashmap.h"
#include "vector.h"
#include "AUIListBox.h"

class CDlgMonsterSpirit : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgMonsterSpirit();
	virtual ~CDlgMonsterSpirit();

	void					Update();
protected:

	virtual bool			OnInitDialog();
	virtual void			OnShowDialog();
	virtual void			OnTick();
	void					GenerateSpiritDroppingMonster();
	void					AddOneMonsterDisplay(int id);

	typedef abase::vector<int> MonsterContainer;
	typedef abase::vector<int> HistoryKeyContainer;
	typedef abase::hash_map<int, MonsterContainer*> DropSpiritMonster;
	DropSpiritMonster		m_Monsters;
	HistoryKeyContainer		m_Keys;
	bool					m_bInit;
	PAUILISTBOX				m_pList;
};
