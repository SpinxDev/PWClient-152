// FWDialogCustom.cpp: implementation of the FWDialogCustom class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogCustom.h"
#include "FWAssemblyCache.h"
#include "AUIListBox.h"
#include "FWModifierCreator.h"
#include "AUIEditBox.h"
#include "FWDialogParam.h"


#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNCREATE(FWDialogCustom, FWDialogBase)

BEGIN_EVENT_MAP_FW(FWDialogCustom, FWDialogPropBase)

ON_EVENT_FW("LIST_MODIFIER", WM_LBUTTONDOWN, FWDialogCustom::OnLButtonDownListModifier)
ON_EVENT_FW("BTN_MODIFIER", WM_LCLICK, FWDialogCustom::OnClickBtnModifier)

END_EVENT_MAP_FW()




FWDialogCustom::FWDialogCustom() :
	m_pModifier(NULL),
	m_pListModifier(NULL),
	m_nLastSelIndexModifier(-1)
{
	
}

FWDialogCustom::~FWDialogCustom()
{
	SAFE_DELETE(m_pModifier);
}

const FWModifier * FWDialogCustom::GetModifier()
{
	return m_pModifier;
}

void FWDialogCustom::SetModifier(const FWModifier * pModifier)
{
	SAFE_DELETE(m_pModifier);
	m_pModifier = pModifier->Clone();
}

int FWDialogCustom::GetProfileType()
{
	return FWAssemblyCache::TYPE_COMPONENT;
}

void FWDialogCustom::OnLButtonDownListModifier(WPARAM wParam, LPARAM lParam)
{
	OnLButtonDownListboxTemplate(
		wParam, 
		lParam,
		m_pListModifier,
		&m_nLastSelIndexModifier,
		(FuncOnSelChangeListbox) &FWDialogCustom::OnSelChangeListModifier);
}

void FWDialogCustom::OnOK()
{
	// update member from UI

	// update count
	m_nCount = ACString(m_pEditCount->GetText()).ToInt();
	if (m_nCount < 0) m_nCount = 0;

	// update modifier
	CheckListbox(m_pListModifier, (FuncOnSelChangeListbox) &FWDialogCustom::OnSelChangeListModifier);

	FWDialogPropBase::OnOK();
}

bool FWDialogCustom::Init(AUIDialog * pDlg)
{
	if (!FWDialogPropBase::Init(pDlg)) return false;

	m_pListModifier = (AUIListBox *) GetDlgItem("LIST_MODIFIER");
	ASSERT(m_pListModifier);

	m_pEditCount = (AUIEditBox *) GetDlgItem("EDIT_COUNT");
	ASSERT(m_pEditCount);

	InitModifierList();

	return true;
}

void FWDialogCustom::OnSelChangeListModifier(int nSelIndex)
{
	if (nSelIndex == -1 || nSelIndex >= m_pListModifier->GetCount()) 
	{
		m_pListModifier->SetCurSel(0);
		nSelIndex = 0;
	}

	int nID = m_pListModifier->GetItemData(nSelIndex);
	
	SAFE_DELETE(m_pModifier);
	m_pModifier = FWModifierCreator::GetInstance()->CreateModifier(nID);
}

void FWDialogCustom::InitModifierList()
{
	FWModifierCreator *pCreator = FWModifierCreator::GetInstance();
	for (int i = 0; i < FWModifierCreator::ID_MODIFIER_MAX; i++)
	{
		const FWModifierInfo *pInfo = pCreator->FindByID(i);
		m_pListModifier->AddString(AS2S(pInfo->Name));
		m_pListModifier->SetItemData(i, pInfo->ID);
	}
}

void FWDialogCustom::OnShowDialog()
{
	// set count editbox
	ACString tmp;
	tmp.Format(_AL("%d"), m_nCount);
	m_pEditCount->SetText(tmp);
	
	
	// selelect in modifierlist
	m_pListModifier->SetCurSel(-1);
	int nID = m_pModifier->GetID();
	for (int i = 0; i < m_pListModifier->GetCount(); i++)
	{
		if ((int)m_pListModifier->GetItemData(i) == nID)
		{
			m_pListModifier->SetCurSel(i);
			break;
		}
	}
	m_nLastSelIndexModifier = m_pListModifier->GetCurSel();

	FWDialogPropBase::OnShowDialog();
}


void FWDialogCustom::OnClickBtnModifier(WPARAM wParam, LPARAM lParam)
{
	CheckListbox(m_pListModifier, (FuncOnSelChangeListbox) &FWDialogCustom::OnSelChangeListModifier);

	FWDialogParam *pDlg = GetFWDialog2(FWDialogParam);
	
	pDlg->m_aryParam.RemoveAll(false);
	m_pModifier->FillParamArray(pDlg->m_aryParam);

	pDlg->DoModal(OnDoModalModifierCallback, this);
}

void FWDialogCustom::OnDoModalModifierCallback(void *pData)
{
	FWDialogCustom *pThis = (FWDialogCustom *) pData;

	FWDialogParam *pDlg = 
		static_cast<FWDialogParam *>(pThis->GetFWDialog(TO_STRING(FWDialogParam)));
	pThis->m_pModifier->UpdateFromParamArray(pDlg->m_aryParam);
}
