// Filename	: DlgMonsterSpirit.cpp
// Creator	: Han Guanghui
// Date		: 2013/10/12

#include "DlgMonsterSpirit.h"
#include "ExpTypes.h"
#include "elementdataman.h"
#include "EC_Global.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_UIConfigs.h"
#include "EC_Configs.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgMonsterSpirit, CDlgBase)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgMonsterSpirit, CDlgBase)

AUI_END_EVENT_MAP()

CDlgMonsterSpirit::CDlgMonsterSpirit():
m_bInit(false),
m_pList(NULL)
{
}

CDlgMonsterSpirit::~CDlgMonsterSpirit()
{
	DropSpiritMonster::iterator iter = m_Monsters.begin();
	DropSpiritMonster::iterator iterEnd = m_Monsters.end();
	while (iter != iterEnd) {
		delete iter->second;
		++iter;
	}
	m_Monsters.clear();
}

bool CDlgMonsterSpirit::OnInitDialog()
{
	DDX_Control("List_Monster", m_pList);
	Update();
	return true;
}
void CDlgMonsterSpirit::OnShowDialog()
{
	GenerateSpiritDroppingMonster();
}
void CDlgMonsterSpirit::OnTick()
{

}
void CDlgMonsterSpirit::GenerateSpiritDroppingMonster()
{
	if (!m_bInit){
		DropSpiritMonster::iterator iter;
		elementdataman *pDataMan = g_pGame->GetElementDataMan();
		DATA_TYPE DataType;
		unsigned int tid = pDataMan->get_first_data_id(ID_SPACE_ESSENCE,DataType);
		m_Monsters.clear();
		m_Keys.clear();
		while(tid) {
			if(DataType == DT_MONSTER_ESSENCE) {
				const MONSTER_ESSENCE* pData = (const MONSTER_ESSENCE*)
					pDataMan->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
				int key = pData->drop_mine_condition_flag;
				float drop = pData->drop_mine_probability; 
				if(pData && drop > 0) {
					iter = m_Monsters.find(key);
					if (iter == m_Monsters.end()) {
						MonsterContainer* pContainer = new MonsterContainer;
						pContainer->push_back(pData->id);
						m_Monsters[key] = pContainer;
						if (key) m_Keys.push_back(key);
					} else iter->second->push_back(pData->id);
				}
			}
			tid = pDataMan->get_next_data_id(ID_SPACE_ESSENCE, DataType);
		}
		m_bInit = true;
	}
	GetGameSession()->c2s_CmdQueryUniqueData(m_Keys);
}
void CDlgMonsterSpirit::AddOneMonsterDisplay(int id)
{
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	const MONSTER_ESSENCE* pData = (const MONSTER_ESSENCE*)pDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, DataType);
	const MINE_ESSENCE* pMine = NULL;
	if (pData && DataType == DT_MONSTER_ESSENCE) {
		ACString strText;
		ACString strName[2];
		unsigned int name_index = 0;
		unsigned int mine_index = 0;
		unsigned int max_mine_drop = sizeof(pData->drop_mines) / sizeof(pData->drop_mines[0]);
		while (name_index < 2 && mine_index < max_mine_drop) {
			if (pData->drop_matters[mine_index].id) {
				pMine = (const MINE_ESSENCE*)pDataMan->get_data_ptr(
					pData->drop_mines[mine_index].id, ID_SPACE_ESSENCE, DataType);
				if (pMine && DataType == DT_MINE_ESSENCE)
					strName[name_index++] = pMine->name;
			}
			++mine_index;
		}
		strText.Format(_AL("%s\t%3.0f%%\t%s\t%s"), pData->name, pData->drop_mine_probability * 100.f, strName[0], strName[1]);
		
		m_pList->AddString(strText);
	}
}
void CDlgMonsterSpirit::Update()
{
	m_pList->ResetContent();
	DropSpiritMonster::iterator iter = m_Monsters.begin();
	DropSpiritMonster::iterator iterEnd = m_Monsters.end();
	MonsterContainer::iterator iterMonster;
	MonsterContainer::iterator iterMonsterEnd;
	while (iter != iterEnd) {
		int history_key_id = iter->first;
		MonsterContainer* pContainer = iter->second;
		CECGameRun::unique_data* data = NULL;
		if (history_key_id != 0) data = GetGameRun()->GetUniqueData(history_key_id);
		if (history_key_id == 0 || 
			(data && pContainer && data->type == 1 && data->GetValueAsInt() == 1)) {
			iterMonster = pContainer->begin();
			iterMonsterEnd = pContainer->end();
			while (iterMonster != iterMonsterEnd) {
				AddOneMonsterDisplay(*iterMonster);
				++iterMonster;
			}
		}
		++iter;
	}
	PAUIOBJECT pObj = GetDlgItem("Txt_LastTime");
	int nMaxGainTimes = CECUIConfig::Instance().GetGameUI().nMonsterSpiritGatherTimesPerWeekMax;
	if (pObj) {
		ACString strText;
		strText.Format(_AL("%d/%d"), a_Max(nMaxGainTimes - GetHostPlayer()->GetCardHolder().gain_times, 0), nMaxGainTimes);
		pObj->SetText(strText);
	}
}
