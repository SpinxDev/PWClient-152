// Filename	: DlgWikiMonster.h
// Creator	: Xu Wenbin
// Date		: 2010/04/06

#include "DlgWikiMonster.h"
#include "DlgWikiMonsterDrop.h"
#include "DlgWikiMonsterTask.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "WikiSearchCommand.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiMonster, CDlgWikiByNameBase)
AUI_ON_COMMAND("drop", OnCommand_Drop)
AUI_ON_COMMAND("task", OnCommand_Task)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiMonster, CDlgWikiByNameBase)
AUI_ON_EVENT("List_Monster",	WM_LBUTTONDOWN,	OnEventLButtonDown_ListMonster)
AUI_ON_EVENT("List_Monster",	WM_KEYDOWN,		OnEventKeyDown_ListMonster)
AUI_END_EVENT_MAP()

// =======================================================================
// Link Command
// =======================================================================
CDlgWikiMonster::ShowSpecficLinkCommand::ShowSpecficLinkCommand(unsigned int monster_id, const ACString& name)
:m_MonsterID(monster_id)
,m_Name(name)
{
}

CDlgNameLink::LinkCommand* CDlgWikiMonster::ShowSpecficLinkCommand::Clone() const
{
	return new ShowSpecficLinkCommand(m_MonsterID, m_Name);
}

bool CDlgWikiMonster::ShowSpecficLinkCommand::operator()(P_AUITEXTAREA_NAME_LINK pLink)
{
	PAUITEXTAREA pObj = GetTxtArea();

	// goto search item
	CDlgWikiMonster* pDlg = dynamic_cast<CDlgWikiMonster*>(g_pGame->GetGameRun()->GetUIManager()->
		GetInGameUIMan()->GetDialog("Win_WikiMonsterSearch"));
	ASSERT(pDlg); // should always get this dialog
	if(pDlg)	
	{
		// ensure the provider is correct
		pDlg->SetContentProvider(&WikiElementDataProvider(ID_SPACE_ESSENCE));
		pDlg->RefreshByNewCommand(&WikiSearchBySpecificID(m_MonsterID), true);
		return true;
	}

	return false;
}

ACString CDlgWikiMonster::ShowSpecficLinkCommand::GetLinkText() const
{
	ACString strLinkedName;
	
	strLinkedName.Format(_AL("^00FF00&%s&^FFFFFF"), m_Name);
	
	return strLinkedName;
}

// =======================================================================
CDlgWikiMonster::CDlgWikiMonster()
{
	m_pList_Monster = NULL;
	
	m_pTxt_Link_MonsterName = NULL;
	m_pTxt_FiveProperties = NULL;
	m_pTxt_Level = NULL;
	m_pTxt_Hp = NULL;
	m_pTxt_Stealth = NULL;
	m_pTxt_AntiImplicit = NULL;
	m_pTxt_Exp = NULL;
	m_pTxt_Sp = NULL;
	m_pTxt_AttackType = NULL;
	m_pTxt_Egg = NULL;
	m_pTxt_HabitAt = NULL;

	m_pBtn_Drop = NULL;
	m_pBtn_Task = NULL;
}

bool CDlgWikiMonster::OnInitDialog()
{
	// 调用父类初始化
	if (!CDlgWikiByNameBase::OnInitDialog())
		return false;

	// 初始化独有成员
	DDX_Control("List_Monster", m_pList_Monster);
	
	DDX_Control("Txt_Link_MonsterName", m_pTxt_Link_MonsterName);
	DDX_Control("Txt_FiveProperties", m_pTxt_FiveProperties);
	DDX_Control("Txt_Level", m_pTxt_Level);
	DDX_Control("Txt_Hp", m_pTxt_Hp);
	DDX_Control("Txt_Stealth", m_pTxt_Stealth);
	DDX_Control("Txt_AntiImplicit", m_pTxt_AntiImplicit);
	DDX_Control("Txt_Exp", m_pTxt_Exp);
	DDX_Control("Txt_Sp", m_pTxt_Sp);
	DDX_Control("Txt_AttackType", m_pTxt_AttackType);
	DDX_Control("Txt_Egg", m_pTxt_Egg);
	DDX_Control("Txt_HabitAt", m_pTxt_HabitAt);
	
	DDX_Control("Btn_Drop", m_pBtn_Drop);
	DDX_Control("Btn_Task", m_pBtn_Task);
	
	return true;
}

bool CDlgWikiMonster::Release(void)
{
	return CDlgWikiByNameBase::Release();
}

void CDlgWikiMonster::ResetSearchCommand()
{
	CDlgWikiByNameBase::ResetSearchCommand();
	// default search command
	SetContentProvider(&WikiElementDataProvider(ID_SPACE_ESSENCE));
	SetSearchCommand(&WikiSearchMonsterByName());
}

void CDlgWikiMonster::OnBeginSearch()
{
	// 准备进行搜索，需要清空当前列表及其它相关内容
	//
	m_pList_Monster->ResetContent();

	// 更新选中项为空
	OnEventLButtonDown_ListMonster(0, 0, m_pList_Monster);
}

bool CDlgWikiMonster::OnAddSearch(WikiEntityPtr p)
{
	WikiElementDataProvider::Entity* pEE = dynamic_cast<WikiElementDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;
	
	unsigned int id = pEE->GetID();

	// 向列表中添加一行

	ASSERT(m_pList_Monster->GetCount() < GetPageSize());

	// get data from entity first
	const MONSTER_ESSENCE* pEssence = NULL;
	if(!pEE->GetConstData(pEssence))
	{
		pEssence = GetEssence(id);
	}
	ASSERT(pEssence);
	if (!pEssence) return false;

	ACString strItem;	
	strItem.Format(_AL("%s\t%d"), pEssence->name, pEssence->level);
	m_pList_Monster->AddString(strItem);
	m_pList_Monster->SetItemData(m_pList_Monster->GetCount()-1, id);

	return true;
}

void CDlgWikiMonster::OnEndSearch()
{
	CDlgWikiByNameBase::OnEndSearch();

	if (m_pList_Monster->GetCount() <= 0)
	{
		// 搜索结果为空
		return;
	}

	// 设置第一项为默认选中项
	m_pList_Monster->SetCurSel(0);
	
	// 并更新选中项相关的界面显示
	OnEventLButtonDown_ListMonster(0, 0, m_pList_Monster);
}

void CDlgWikiMonster::OnEventLButtonDown_ListMonster(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	// 根据当前选中项更新界面
	//

	// 隐藏任务界面、掉落界面，便于
	GetGameUIMan()->m_pDlgWikiMonsterDrop->OnCommand_CANCEL("");
	GetGameUIMan()->m_pDlgWikiMonsterTask->OnCommand_CANCEL("");

	const MONSTER_ESSENCE *pEssence = GetCurEssence();
	GetGameUIMan()->m_pDlgWikiMonsterDrop->SetSearchDataPtr((void *)pEssence);
	GetGameUIMan()->m_pDlgWikiMonsterTask->SetSearchDataPtr((void *)pEssence);
	if (pEssence)
	{
		ACString szTxt;

		BindLinkCommand(m_pTxt_Link_MonsterName, NULL,
						&CDlgNameLink::MoveToLinkCommand(pEssence->id, pEssence->name));

		m_pTxt_FiveProperties->SetText(GetStringFromTable(517 + a_atoi(pEssence->prop)));
		m_pTxt_Level->SetText(szTxt.Format(_AL("%d"), pEssence->level));
		m_pTxt_Hp->SetText(szTxt.Format(_AL("%d"), pEssence->life));
		m_pTxt_Stealth->SetText(szTxt.Format(_AL("%d"), pEssence->invisible_lvl + (pEssence->invisible_lvl?pEssence->level:0)));
		m_pTxt_AntiImplicit->SetText(szTxt.Format(_AL("%d"), pEssence->uninvisible_lvl<0?0:(pEssence->uninvisible_lvl+pEssence->level)));
		m_pTxt_Exp->SetText(szTxt.Format(_AL("%d"), pEssence->exp));
		m_pTxt_Sp->SetText(szTxt.Format(_AL("%d"), pEssence->skillpoint));
		m_pTxt_AttackType->SetText(GetStringFromTable(8700 + pEssence->aggressive_mode));
		m_pTxt_HabitAt->SetText(GetStringFromTable(8702 + pEssence->inhabit_type));
		if (pEssence->id_pet_egg_captured != 0)
		{
			DATA_TYPE dataType = DT_INVALID;
			const PET_EGG_ESSENCE* pEggData = (const PET_EGG_ESSENCE*)
				GetGame()->GetElementDataMan()->get_data_ptr(pEssence->id_pet_egg_captured, ID_SPACE_ESSENCE, dataType);
			if (!pEggData || dataType != DT_PET_EGG_ESSENCE)
			{
				ASSERT(false);
			}
			else
			{
				m_pTxt_Egg->SetText(pEggData->name);
			}
		}
		else
		{
			m_pTxt_Egg->SetText(GetStringFromTable(8720));
		}
		
		m_pBtn_Drop->Enable(!GetGameUIMan()->m_pDlgWikiMonsterDrop->TestEmpty());
		m_pBtn_Task->Enable(!GetGameUIMan()->m_pDlgWikiMonsterTask->TestEmpty());
	}
	else
	{
		m_pTxt_Link_MonsterName->SetText(_AL(""));
		m_pTxt_FiveProperties->SetText(_AL(""));
		m_pTxt_Level->SetText(_AL(""));
		m_pTxt_Hp->SetText(_AL(""));
		m_pTxt_Stealth->SetText(_AL(""));
		m_pTxt_AntiImplicit->SetText(_AL(""));
		m_pTxt_Exp->SetText(_AL(""));
		m_pTxt_Sp->SetText(_AL(""));
		m_pTxt_AttackType->SetText(_AL(""));
		m_pTxt_Egg->SetText(_AL(""));
		m_pTxt_HabitAt->SetText(_AL(""));
		
		m_pBtn_Drop->Enable(false);
		m_pBtn_Task->Enable(false);
	}
}

void CDlgWikiMonster::OnEventKeyDown_ListMonster(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (wParam == VK_UP ||
		wParam == VK_DOWN)
	{
		// 上下翻页时保持选取项与界面一致
		OnEventLButtonDown_ListMonster(0, 0, NULL);
	}
}

const MONSTER_ESSENCE * CDlgWikiMonster::GetEssence(unsigned int id)const
{
	DATA_TYPE dataType = DT_INVALID;
	const MONSTER_ESSENCE *pEssence = (const MONSTER_ESSENCE *)g_pGame->GetElementDataMan()->get_data_ptr(id, ID_SPACE_ESSENCE, dataType);
	if (dataType == DT_MONSTER_ESSENCE)
	{
		// 类型正确，返回指针
		return pEssence;
	}

	return NULL;
}

const MONSTER_ESSENCE * CDlgWikiMonster::GetCurEssence()const
{
	const MONSTER_ESSENCE *pEssence = NULL;
	
	if (m_pList_Monster->GetCount() > 0)
	{		
		// 从当前选中项、查找 id 及 essence 数据
		//
		int nCurSel = m_pList_Monster->GetCurSel();
		if (nCurSel >= 0 && nCurSel < m_pList_Monster->GetCount())
		{
			unsigned int id = m_pList_Monster->GetItemData(nCurSel);
			pEssence = GetEssence(id);
		}
	}
	return pEssence;
}

void CDlgWikiMonster::OnCommand_Drop(const char *szCommand)
{
	CDlgWikiMonsterDrop *pDlg = GetGameUIMan()->m_pDlgWikiMonsterDrop;
	pDlg->Show(!pDlg->IsShow());
}

void CDlgWikiMonster::OnCommand_Task(const char *szCommand)
{
	CDlgWikiMonsterTask *pDlg = GetGameUIMan()->m_pDlgWikiMonsterTask;
	pDlg->Show(!pDlg->IsShow());
}

void CDlgWikiMonster::OnCommandCancel()
{
	GetGameUIMan()->m_pDlgWikiMonsterDrop->OnCommand_CANCEL("");
	GetGameUIMan()->m_pDlgWikiMonsterTask->OnCommand_CANCEL("");
	CDlgWikiByNameBase::OnCommandCancel();
}
