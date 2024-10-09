// Filename	: DlgAutoHPMP.cpp
// Creator	: Xu Wenbin
// Date		: 2012/8/8

#include "DlgAutoHPMP.h"
#include "EC_ComputerAid.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "EC_ShortcutMgr.h"
#include <AUICTranslate.h>

AUI_BEGIN_EVENT_MAP(CDlgAutoHPMP, CDlgBase)
AUI_ON_EVENT("Img*",	WM_RBUTTONDOWN,		OnEventRButtonDown)
AUI_END_EVENT_MAP()

AUI_BEGIN_COMMAND_MAP(CDlgAutoHPMP, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Confirm",	OnCommand_Confirm)
AUI_ON_COMMAND("Chk*",			OnCommand_Enable)
AUI_END_COMMAND_MAP()

CDlgAutoHPMP::CDlgAutoHPMP()
{
	::ZeroMemory(m_pChk, sizeof(m_pChk));
	::ZeroMemory(m_pImg, sizeof(m_pImg));
	::ZeroMemory(m_pEdt, sizeof(m_pEdt));
}

bool CDlgAutoHPMP::OnInitDialog()
{
	AString strName;
	for (int i = 0; i < COUNT; ++ i)
	{
		strName.Format("Chk%d", i+1);
		DDX_Control(strName, m_pChk[i]);

		strName.Format("Img%d", i+1);
		DDX_Control(strName, m_pImg[i]);
		
		strName.Format("Edt%d", i+1);
		DDX_Control(strName, m_pEdt[i]);
	}
	
	typedef CECShortcutMgr::SimpleClickShortcut<CDlgAutoHPMP> AutoHPMPClickShortcut;
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new AutoHPMPClickShortcut(this));
	return true;
}

void CDlgAutoHPMP::OnShowDialog()
{
	Load();

	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Inventory");
	if (!pDlg->IsShow()) pDlg->Show(true);
}

void CDlgAutoHPMP::OnHideDialog()
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Inventory");
	if (pDlg->IsShow()) pDlg->Show(false);
}

void CDlgAutoHPMP::OnTick()
{
	UpdateUI();
}

void CDlgAutoHPMP::SwitchShow()
{
	if (!IsShow() && !CEComputerAid::Instance().CanUse())
		return;

	CDlgBase::SwitchShow();
}

void CDlgAutoHPMP::Load()
{
	const CEComputerAid &ca = CEComputerAid::Instance();
	const EC_COMPUTER_AIDED_SETTING & cas = ca.Get();
	for (int i=0; i < COUNT; ++ i)
	{
		const EC_COMPUTER_AIDED_SETTING::ITEM_GROUP &group = cas.group[i];

		SetEnable(i, group.enable);
		SetItem(i, group.slot);
		m_pEdt[i]->SetText(ACString().Format(_AL("%d"), group.percent));
	}
}

void CDlgAutoHPMP::UpdateUI()
{
	if (!CEComputerAid::Instance().CanUse())
	{
		OnCommand_CANCEL("");
		return;
	}

	//	更新界面现有物品的状态，以反应包裹位置修改、物品消失、物品冷却等状态
	EC_COMPUTER_AIDED_SETTING::ITEM_GROUP group;
	for (int i=0; i < COUNT; ++ i)
	{
		int tid = GetItemID(i);
		short slot = GetItemSlot(i);

		group.enable = GetEnable(i);
		group.item = tid;
		group.slot = slot;
		group.percent = GetPercent(i);

		if (CEComputerAid::Instance().MakeValid(i, group))
		{
			//	重用 MakeValid 检查物品位置修改、消失等问题
			slot = group.item == tid ? group.slot : -1;
		}

		SetItem(i, slot);
	}
}


void CDlgAutoHPMP::OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int group = GetNameIndex(pObj, "Img")-1;
	if (group < 0 || group >= COUNT)
	{
		ASSERT(false);
		return;
	}
	ClearItem(group);
}

void CDlgAutoHPMP::OnCommand_CANCEL(const char *szCommand)
{
	Show(false);
}

int	CDlgAutoHPMP::GetItemID(int group)
{
	ASSERT(group >= 0 && group < COUNT);
	return (int)m_pImg[group]->GetData();
}

short CDlgAutoHPMP::GetItemSlot(int group)
{
	ASSERT(group>=0 && group < COUNT);
	return (short)m_pImg[group]->GetDataPtr();
}

void CDlgAutoHPMP::SetItemID(int group, int item_id)
{
	ASSERT(group>=0 && group < COUNT);
	m_pImg[group]->SetData(item_id);
}

void CDlgAutoHPMP::SetItemSlot(int group, short slot)
{
	ASSERT(group>=0 && group < COUNT);
	m_pImg[group]->SetDataPtr((void*)slot);
}

char CDlgAutoHPMP::GetPercent(int group)
{
	ASSERT(group>=0 && group < COUNT);
	ACString strText = m_pEdt[group]->GetText();
	return (char)strText.ToInt();
}

bool CDlgAutoHPMP::GetEnable(int group)
{
	ASSERT(group>=0 && group < COUNT);
	return m_pChk[group]->IsChecked();
}

void CDlgAutoHPMP::ClearItem(int group)
{
	ASSERT(group>=0 && group < COUNT);
	m_pImg[group]->ClearCover();
	m_pImg[group]->SetText(_AL(""));
	m_pImg[group]->SetHint(_AL(""));
	m_pImg[group]->SetColor(A3DCOLORRGB(255, 255, 255));	
	AUIClockIcon *pClock = m_pImg[group]->GetClockIcon();
	pClock->SetProgressRange(0, 1);
	pClock->SetProgressPos(1);
	SetItemID(group, 0);
	SetItemSlot(group, -1);
}

void CDlgAutoHPMP::SetEnable(int group, bool bEnable)
{
	ASSERT(group>=0 && group < COUNT);
	if (GetEnable(group) != bEnable)
		m_pChk[group]->Check(bEnable);
}

void CDlgAutoHPMP::SetItem(int group, short slot)
{
	CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(slot);
	if (!pItem)
	{
		ClearItem(group);
	}
	else
	{
		GetGameUIMan()->SetCover(m_pImg[group], pItem->GetIconFile());
		SetItemID(group, pItem->GetTemplateID());
		SetItemSlot(group, slot);
		
		ACString strCount;
		if (pItem->GetCount() > 1)
			strCount.Format(_AL("%d"), pItem->GetCount());
		m_pImg[group]->SetText(strCount);		
		
		AUICTranslate trans;
		AWString strHint = trans.Translate(pItem->GetDesc());
		m_pImg[group]->SetHint(strHint);
		
		m_pImg[group]->SetColor(pItem->IsUseable() && !pItem->CheckUseCondition() ? A3DCOLORRGB(192, 0, 0) : A3DCOLORRGB(255, 255, 255));
		
		AUIClockIcon *pClock = m_pImg[group]->GetClockIcon();
		int nMax(0);
		if (pItem->GetCoolTime(&nMax) > 0)
		{
			pClock->SetProgressRange(0, nMax);
			pClock->SetProgressPos(nMax - pItem->GetCoolTime());
			pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
		}
		else
		{
			pClock->SetProgressRange(0, 1);
			pClock->SetProgressPos(1);
		}
	}
}

void CDlgAutoHPMP::OnCommand_Confirm(const char *szCommand)
{
	CEComputerAid &ca = CEComputerAid::Instance();
	EC_COMPUTER_AIDED_SETTING cas = ca.Get();
	for (int i(0); i < COUNT; ++ i)
	{
		EC_COMPUTER_AIDED_SETTING::ITEM_GROUP temp;
		
		temp.enable = GetEnable(i);
		temp.item = GetItemID(i);
		temp.slot = GetItemSlot(i);
		temp.percent = GetPercent(i);

		switch (ca.Validate(i, temp))
		{
		case 0: break;
		case 1: ClearItem(i); SetEnable(i, false); return;
		case 2: ChangeFocus(m_pEdt[i]); return;
		default: ASSERT(false); return;
		}

		cas.group[i] = temp;
	}
	if (!ca.Set(cas)) return;
	OnCommand_CANCEL("");
}

void CDlgAutoHPMP::OnCommand_Enable(const char *szCommand)
{
	int group = GetNameIndex(szCommand, "Chk")-1;
	if (group < 0 || group >= COUNT)
	{
		ASSERT(false);
		return;
	}
	SetEnable(group, GetEnable(group));
}

void CDlgAutoHPMP::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	int group = -1;
	if (pObjOver)
	{
		//	直接拖拽到指定图标上
		group = GetNameIndex(pObjOver, "Img")-1;
		if (group < 0 || group >= COUNT)
		{
			ASSERT(false);
			return;
		}
		
		if (!CEComputerAid::Instance().ValidateSlot(group, iSrc))
			return;
	}
	else
	{
		//	自动搜索合适位置
		int iFirst(-1);
		int iFirstEmpty(-1);
		for (int i(0); i < COUNT; ++ i)
		{
			if (CEComputerAid::Instance().ValidateSlot(i, iSrc))
			{
				//	记录第一个合适位置
				if (iFirst < 0) iFirst = i;

				//	记录第一个空位置
				if (iFirstEmpty < 0 && GetItemSlot(i) < 0) iFirstEmpty = i;
			}
		}
		if (iFirst < 0) return;
		group = iFirstEmpty >= 0 ? iFirstEmpty : iFirst;
	}

	SetItem(group, iSrc);
	SetEnable(group, true);
}