// Filename	: DlgSettingQuickKey.cpp
// Creator	: zhangyitian
// Date		: 2014/07/22

// 快捷栏界面，放入设置界面中，代码多来自原先的DlgQuickKey


#include "DlgSettingQuickKey.h"

#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_HostInputFilter.h"

#include "AUIStillImageButton.h"
#include "AFI.h"

#define new A_DEBUG_NEW

AUI_BEGIN_EVENT_MAP(CDlgSettingQuickKey, CDlgSetting)
AUI_ON_EVENT("Lst_KeyList", WM_RBUTTONUP, OnEventRButtonUp_KeyList)
AUI_END_EVENT_MAP()

AUI_BEGIN_COMMAND_MAP(CDlgSettingQuickKey, CDlgSetting)
AUI_ON_COMMAND("confirm", OnCommandConfirm)
AUI_ON_COMMAND("Btn_Keyboard", OnCommandKeyboard)
AUI_END_COMMAND_MAP()

CDlgSettingQuickKey::CDlgSettingQuickKey()
{
	m_pLst_Keylist = NULL;
	m_nLastSelected = -1;
	
	m_iUsageCustomize = 0;
	m_iUsage = 0;
	m_nHotKey = 0;
	m_dwModifier = 0;
}

bool CDlgSettingQuickKey::OnInitDialog()
{
	DDX_Control("Lst_Keylist", m_pLst_Keylist);
	return true;
}

void CDlgSettingQuickKey::OnShowDialog()
{
	CDlgSetting::OnShowDialog();

	m_pBtnSystem->SetPushed(false);
	m_pBtnGame->SetPushed(false);
	m_pBtnVideo->SetPushed(false);
	m_pBtnQuickKey->SetPushed(true);

	UpdateHotKeyDisplay();
}

void CDlgSettingQuickKey::OnHideDialog() {
	GetGameRun()->GetHostInputFilter()->SetCurrentCustomize(0, true);
	Show(false);
}

void CDlgSettingQuickKey::OnTick()
{
	int nSel = m_pLst_Keylist->GetCurSel();
	int nCount = m_pLst_Keylist->GetCount();
	if (nSel>=0 && nSel<nCount)
	{
		// Update display
		UpdateHotKeyDisplay(nSel);

		// Update with selection
		int iUsage = m_pLst_Keylist->GetItemData(nSel);
		GetGameRun()->GetHostInputFilter()->SetCurrentCustomize(iUsage);
	}
	
	// Restore old highlighted item
	if (nSel != m_nLastSelected)
	{
		// Restore old selected to normal state
		if (m_nLastSelected>=0 && m_nLastSelected<nCount)
			m_pLst_Keylist->SetItemTextColor(m_nLastSelected, A3DCOLORRGB(255, 255, 255), 1);

		m_nLastSelected = nSel;
	}

	// Update new highlighted item
	if (nSel>=0 && nSel<nCount)
	{
		// Highlight selected item by changing alpha value continuously
		//
		A3DCOLOR color = m_pLst_Keylist->GetItemTextColor(nSel, 1);

		A3DCOLORVALUE value(color);
		value.r = 1;
		value.g = value.b = 0;
		value.a -= 0.05f;
		if (value.a < 0)
			value.a = 1.0f;
		color = value.ToRGBAColor();

		m_pLst_Keylist->SetItemTextColor(nSel, color, 1);
	}
}

void CDlgSettingQuickKey::UpdateHotKeyDisplay(int nSel/* =-1 */)
{	
	CECHostInputFilter *pHostInputFilter = GetGameRun()->GetHostInputFilter();	
	ACString strDesc, strTemp;
	if (nSel == -1)
	{
		m_pLst_Keylist->ResetContent();
		m_nLastSelected = -1;

		for (int iUsage=1; iUsage<NUM_LKEY; ++iUsage)
		{
			if (pHostInputFilter->IsUsageCustomizable(iUsage))
			{
				// Get usage description string from file
				strDesc = GetStringFromTable(8150+iUsage-1);
				
				// Get usage current hot key 
				pHostInputFilter->GetUsageShowDescription(iUsage, strTemp, false);
				strDesc += _AL("\t");
				strDesc += strTemp;
				
				// Add to list
				m_pLst_Keylist->AddString(strDesc);
				
				// Set list item data for customize
				m_pLst_Keylist->SetItemData(m_pLst_Keylist->GetCount()-1, iUsage);
			}
		}

		int defaultSel = m_pLst_Keylist->GetCurSel();
		if (defaultSel>=0 && defaultSel<m_pLst_Keylist->GetCount())
			m_nLastSelected = defaultSel;
	}
	else
	{
		if (nSel>=0 && nSel<m_pLst_Keylist->GetCount())
		{
			int iUsage = m_pLst_Keylist->GetItemData(nSel);
			
			// Get usage description string from file
			strDesc = GetStringFromTable(8150+iUsage-1);
			
			// Get usage current hot key 
			pHostInputFilter->GetUsageShowDescription(iUsage, strTemp, false);
			strDesc += _AL("\t");
			strDesc += strTemp;
			
			// Update content
			m_pLst_Keylist->SetText(nSel, strDesc);
		}
	}
}

int CDlgSettingQuickKey::FindItemWithUsage(int iUsage)
{
	int nSel(0), nCount(m_pLst_Keylist->GetCount());
	while (nSel<nCount)
	{
		if ((int)m_pLst_Keylist->GetItemData(nSel) == iUsage)
			break;
		++nSel;
	}
	return nSel;
}

bool CDlgSettingQuickKey::CustomizeHotKey(int iUsageCustomize, int nHotKey, DWORD dwModifier)
{
	// Test if hot key and modifier can be used for given usage
	// Return true if dialog pop up or usage accepted immediately
	//
	bool processed(false);

	while (true)
	{
		CECHostInputFilter *pHostInputFilter = GetGameRun()->GetHostInputFilter();
		
		if (!pHostInputFilter->IsHotKeyValid(nHotKey, dwModifier))
		{
			// Hot key is reserved for system
			GetGameUIMan()->MessageBox("", GetStringFromTable(8301), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			processed = true;
			break;
		}

		int iUsage(0);		
		if (!pHostInputFilter->IsHotKeyUsed(nHotKey, dwModifier, &iUsage))
		{
			// Hot key accepted immediately
			pHostInputFilter->CustomizeHotKey(iUsageCustomize, nHotKey, dwModifier);
			processed = true;
			break;
		}

		if (!pHostInputFilter->IsUsageCustomizable(iUsage))
		{
			// Hot key is used by system or reserved
			GetGameUIMan()->MessageBox("", GetStringFromTable(8301), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			processed = true;
			break;
		}

		if (iUsage == iUsageCustomize)
		{
			// Hot key used by itself
			processed = true;
			break;
		}

		// Hot key used by other usage customizable
		// Pop up dialog for confirm
		//
		ACString strConfirm;
		strConfirm.Format(GetStringFromTable(8300), GetStringFromTable(8150+iUsage-1));
		
		PAUIDIALOG pMsgBox(NULL);
		GetGameUIMan()->MessageBox("HotKey_Customize", strConfirm, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);

		// Record some data for confirm
		pMsgBox->SetData(iUsageCustomize);
		pMsgBox->SetDataPtr((void *)iUsage);

		m_iUsageCustomize = iUsageCustomize;
		m_iUsage = iUsage;
		m_nHotKey = nHotKey;
		m_dwModifier = dwModifier;
		processed = true;
		break;
	}

	return processed;
}

void CDlgSettingQuickKey::CustomizeHotKey(int iUsageCustomize, int iUsage)
{
	if (m_iUsageCustomize == iUsageCustomize &&
		m_iUsage == iUsage)
	{
		CECHostInputFilter *pHostInputFilter = GetGameRun()->GetHostInputFilter();
		pHostInputFilter->DeleteHotKeyCustomize(m_iUsage);
		pHostInputFilter->CustomizeHotKey(m_iUsageCustomize, m_nHotKey, m_dwModifier);

		UpdateHotKeyDisplay(FindItemWithUsage(m_iUsage));
		UpdateHotKeyDisplay(FindItemWithUsage(m_iUsageCustomize));
	}
	
	m_iUsageCustomize = 0;
	m_iUsage = 0;
	m_nHotKey = 0;
	m_dwModifier = 0;
}

void CDlgSettingQuickKey::ConfirmHotKeyCustomize(bool hideAfterConfirm/* =false */)
{
	// Apply customize
	GetGameRun()->GetHostInputFilter()->SetCurrentCustomize(0);

	if (hideAfterConfirm)
	{
		// Hide
		Show(false);
	}
	else
	{
		// Bring window to top for further process
		GetGameUIMan()->BringWindowToTop(this);
	}
}

void CDlgSettingQuickKey::OnEventRButtonUp_KeyList(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int nSel = m_pLst_Keylist->GetCurSel();
	if (nSel>=0 && nSel<m_pLst_Keylist->GetCount())
	{
		int iUsage = m_pLst_Keylist->GetItemData(nSel);
		GetGameRun()->GetHostInputFilter()->SetCurrentCustomize(iUsage);
		GetGameRun()->GetHostInputFilter()->DeleteHotKeyCustomize(iUsage);
	}
}

void CDlgSettingQuickKey::Apply()
{
	CECHostInputFilter *pHostInputFilter = GetGameRun()->GetHostInputFilter();
	if (pHostInputFilter->IsExistEmptyCustomizeHotKey() &&
		pHostInputFilter->IsCustomizeHotKeyChanged())
	{
		AUIDialog *pMsgBox = NULL;
		GetGameUIMan()->MessageBox(
			"HotKey_Apply", GetGameUIMan()->GetStringFromTable(8302),
			MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(1);
	}
	else
	{
		ConfirmHotKeyCustomize(false);
	}
}

void CDlgSettingQuickKey::OnCommandConfirm(const char *szCommand)
{
	CECHostInputFilter *pHostInputFilter = GetGameRun()->GetHostInputFilter();
	if (pHostInputFilter->IsExistEmptyCustomizeHotKey() &&
		pHostInputFilter->IsCustomizeHotKeyChanged())
	{
		AUIDialog *pMsgBox = NULL;
		GetGameUIMan()->MessageBox(
			"HotKey_Apply", GetGameUIMan()->GetStringFromTable(8302),
			MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(2);
	}
	else
	{
		ConfirmHotKeyCustomize(true);
	}
}

void CDlgSettingQuickKey::SetToDefault()
{
	GetGameRun()->GetHostInputFilter()->DefaultHotKey();
	UpdateHotKeyDisplay();
}

void CDlgSettingQuickKey::OnCommandKeyboard(const char *szCommand) {
	AUIDialog* dlg = GetGameUIMan()->GetDialog("Win_Keyboard");
	if (!dlg->IsShow()) {
		this->Show(false);
	}
	dlg->Show(!dlg->IsShow());
	
}
