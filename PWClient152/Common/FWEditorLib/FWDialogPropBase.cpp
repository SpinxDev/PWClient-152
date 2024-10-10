// FWDialogPropBase.cpp: implementation of the FWDialogPropBase class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogPropBase.h"
#include "AUIEditBox.h"
#include "AUIListBox.h"
#include "FWAssemblyCache.h"
#include "FWDialogParam.h"


#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNAMIC(FWDialogPropBase, FWDialogBase)

BEGIN_EVENT_MAP_FW(FWDialogPropBase, FWDialogBase)

ON_EVENT_FW("LIST_PROFILE", WM_LBUTTONDOWN, FWDialogPropBase::OnLButtonDownListProfile)
ON_EVENT_FW("BTN_PROFILE", WM_LCLICK, FWDialogPropBase::OnClickBtnProfile)

END_EVENT_MAP_FW()



FWDialogPropBase::FWDialogPropBase() :
	m_nLastSelIndexProfile(-1),
	m_bModified(false)
{
	
}

FWDialogPropBase::~FWDialogPropBase()
{

}

void FWDialogPropBase::OnOK()
{
	m_strName = m_pEditName->GetText();
	m_fTime = ACString(m_pEditTime->GetText()).ToFloat();
	Show(false);

	CheckListbox(m_pListProfile, (FuncOnSelChangeListbox) &FWDialogPropBase::OnSelChangeListProfile);

	FWDialogBase::OnOK();
}

void FWDialogPropBase::OnCancel()
{
	Show(false);

	FWDialogBase::OnCancel();
}

bool FWDialogPropBase::Init(AUIDialog * pDlg)
{
	if (!FWDialogBase::Init(pDlg)) return false;
	
	m_pEditName = (AUIEditBox *) GetDlgItem("EDIT_NAME");
	ASSERT(m_pEditName);

	m_pEditTime = (AUIEditBox *) GetDlgItem("EDIT_TIME");
	ASSERT(m_pEditTime);
	
	m_pListProfile = (AUIListBox *) GetDlgItem("LIST_PROFILE");
	ASSERT(m_pListProfile);

	InitProfileList();

	return true;
}

void FWDialogPropBase::InitProfileList()
{
	FWAssemblyCache *pCache = FWAssemblyCache::GetInstance();
	
	int nType = GetProfileType();

	int nIndex = 0;
	ALISTPOSITION pos = pCache->GetHeadPosition();
	while (pos)
	{
		FWAssemblyInfo *pInfo = pCache->GetNext(pos);
		
		if (pInfo->Type != nType) continue;
		
		m_pListProfile->AddString(AS2S(pInfo->Name));
		m_pListProfile->SetItemData(nIndex++, (DWORD) pInfo->ID);
	}
}

void FWDialogPropBase::OnLButtonDownListProfile(WPARAM wParam, LPARAM lParam)
{
	OnLButtonDownListboxTemplate(
		wParam, 
		lParam, 
		m_pListProfile, 
		&m_nLastSelIndexProfile, 
		(FuncOnSelChangeListbox) &FWDialogPropBase::OnSelChangeListProfile);
}

void FWDialogPropBase::OnSelChangeListProfile(int nSelIndex)
{
	int nID = m_pListProfile->GetItemData(nSelIndex);
	m_profile.Init(nID);
}

void FWDialogPropBase::Show(bool bShow, bool bModal)
{
	if (bShow) OnShowDialog();

	FWDialogBase::Show(bShow, bModal);
}

int FWDialogPropBase::GetProfileType()
{
	return FWAssemblyCache::TYPE_DEFAULT;
}

void FWDialogPropBase::OnShowDialog()
{
	// fill name editbox
	m_pEditName->SetText(m_strName);

	// fill time editbox
	ACString strTime;
	strTime.Format(_AL("%.3f"), m_fTime);
	m_pEditTime->SetText(strTime);
	
	// selelect in list
	m_pListProfile->SetCurSel(-1);
	for (int i = 0; i < m_pListProfile->GetCount(); i++)
	{
		if ((int)m_pListProfile->GetItemData(i) == m_profile.GetAssemblyID())
		{
			m_pListProfile->SetCurSel(i);
			break;
		}
	}
	
	m_nLastSelIndexProfile = m_pListProfile->GetCurSel();

}

void FWDialogPropBase::CheckListbox(AUIListBox * pListbox, FuncOnSelChangeListbox pFunc)
{
	int nSelIndex = pListbox->GetCurSel();
	if (nSelIndex == -1 || nSelIndex >= m_pListProfile->GetCount())
	{
		pListbox->SetCurSel(0);
		(this->*pFunc)(0);
	}
}

void FWDialogPropBase::OnClickBtnProfile(WPARAM wParam, LPARAM lParam)
{
	CheckListbox(m_pListProfile, (FuncOnSelChangeListbox) &FWDialogPropBase::OnSelChangeListProfile);
	
	FWDialogParam *pDlg = GetFWDialog2(FWDialogParam);

	pDlg->m_aryParam.RemoveAll(false);
	m_profile.FillParamArray(pDlg->m_aryParam);

	pDlg->DoModal(OnDoModalProfileCallback, this);
}

void FWDialogPropBase::OnDoModalProfileCallback(void *pData)
{
	FWDialogPropBase *pThis = (FWDialogPropBase *) pData;
	
	FWDialogParam *pDlg = 
		static_cast<FWDialogParam *>(pThis->GetFWDialog(TO_STRING(FWDialogParam)));
	pThis->m_profile.UpdateFromParamArray(pDlg->m_aryParam);
}

bool FWDialogPropBase::IsModified()
{
	return m_bModified;
}
