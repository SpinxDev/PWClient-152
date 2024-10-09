#include "DlgGMParam.h"
#include "AUIListBox.h"
#include "AUIEditBox.h"
#include "AUICheckBox.h"
#include "AUILabel.h"
#include "AUIComboBox.h"
#include "GMCommand.h"
#include "EC_GameUIMan.h"
#include "DlgGMConsole.h"
#include "GMCommandListBox.h"
#include "GMCommandInGame.h"

AUI_BEGIN_COMMAND_MAP(CDlgGMParam, CDlgBase)

AUI_ON_COMMAND("btnOK", OnCommandBtnOK)
AUI_ON_COMMAND("btnCancel", OnCommandBtnCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgGMParam, CDlgBase)

AUI_ON_EVENT("lbxParam", WM_LBUTTONDOWN, OnLButtonDownListBoxParam)

AUI_END_EVENT_MAP()

CDlgGMParam::CDlgGMParam() :
	m_pLbxParam(NULL),
	m_pEbxValue(NULL),
	m_pChkValue(NULL),
	m_pLblChkValueForChk(NULL),
	m_pCbxValue(NULL),
	m_pGMCommand(NULL),
	m_pGMCommandListBox(NULL),
	m_nCurSel(-1)
{
}


CDlgGMParam::~CDlgGMParam()
{

}

void CDlgGMParam::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("lbxParam", m_pLbxParam);
	DDX_Control("ebxValue", m_pEbxValue);
	DDX_Control("chkValue", m_pChkValue);
	DDX_Control("lblValueForChk", m_pLblChkValueForChk);
	DDX_Control("cbxValue", m_pCbxValue);
}

void CDlgGMParam::SetAssocData(CGMCommand *pGMCommand, CGMCommandListBox *pGMCommandListBox)
{
	m_pGMCommandListBox = pGMCommandListBox;

	m_pGMCommand = pGMCommand;
	m_paramArray = pGMCommand->GetParamArray();

	// fill param listbox
	m_pLbxParam->ResetContent();
	CParamArray::iterator iter = m_paramArray.begin();
	for (; m_paramArray.end() != iter; ++iter)
	{
		CParam *pParam = *iter;
		int nIndex = m_pLbxParam->AddString(pParam->GetName()) - 1;
		m_pLbxParam->SetItemDataPtr(nIndex, pParam);
	}

	m_pEbxValue->SetText(_AL(""));
	m_nCurSel = -1;
	m_pLbxParam->SetCurSel((m_pLbxParam->GetCount() > 0) ? 0 : -1);
	OnLButtonDownListBoxParam(NULL, NULL, NULL);

	bool bEnable = (dynamic_cast<CGMCommandCloseActivity*>(pGMCommand) != NULL) || (dynamic_cast<CGMCommandOpenActivity*>(pGMCommand) != NULL);
	m_pEbxValue->Enable(!bEnable);
	GetDlgItem("Lbl_List")->SetText(GetStringFromTable(bEnable ? 6386:6385));
	GetDlgItem("Lbl_Number")->SetText(GetStringFromTable(bEnable ? 6388:6387));
}


void CDlgGMParam::OnLButtonDownListBoxParam(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SaveValue();

	int nOldSel = m_nCurSel;
	m_nCurSel = m_pLbxParam->GetCurSel();
	if (m_nCurSel != nOldSel) 
		LoadValue();
}

void CDlgGMParam::LoadValue()
{
	// show value of selected param
	if (m_nCurSel == -1 || m_nCurSel >= m_pLbxParam->GetCount())
	{
		ShowValueControl(-1);
		return;
	}

	CParam *pParam = static_cast<CParam *>(m_pLbxParam->GetItemDataPtr(m_nCurSel));
	ASSERT(pParam);

	ShowValueControl(pParam->GetDisplayType());
	switch(pParam->GetDisplayType()) 
	{
	case CParam::TYPE_NORMAL:
		m_pEbxValue->SetText(pParam->GetStringValue());
		break;
	case CParam::TYPE_BOOL:
		m_pLblChkValueForChk->SetText(pParam->GetName());
		m_pChkValue->Check(GetParamTrueValue<bool>(pParam));
		break;
	case CParam::TYPE_LIST:
		{
			FillListWithDisplayValues(pParam->GetDisplayValues());
			SelectInValueList(*pParam);
		}
		break;
	default:
		ASSERT(!"invalid display type for gm command param");
		break;
	}
}

void CDlgGMParam::SaveValue()
{
	// save value to selected param
	if (m_nCurSel == -1 || m_nCurSel >= m_pLbxParam->GetCount())
		return;

	CParam *pParam = static_cast<CParam *>(m_pLbxParam->GetItemDataPtr(m_nCurSel));
	ASSERT(pParam);


	switch (pParam->GetDisplayType())
	{
	case CParam::TYPE_NORMAL:
		pParam->SetStringValue(m_pEbxValue->GetText());
		break;
	case CParam::TYPE_BOOL:
		SetParamTrueValue<bool>(pParam, m_pChkValue->IsChecked());
		break;
	case CParam::TYPE_LIST:
		{
			int nSel = m_pCbxValue->GetCurSel();
			ASSERT(nSel >= 0 && nSel < m_pCbxValue->GetCount());
			const CParam *pSelParam = static_cast<CParam *>(m_pCbxValue->GetItemDataPtr(nSel));
			ASSERT(pSelParam);
			pParam->AssignValue(*pSelParam);
		}
		break;
	default:
		ASSERT(!"invalid display type for gm command param");
		break;
	}

	LoadValue();
}

void CDlgGMParam::OnCommandBtnOK(const char * szCommand)
{
	SaveValue();
	m_pGMCommand->SetParamArray(m_paramArray);
	m_pGMCommand->SetCurSelParam(m_nCurSel);
	if (m_pGMCommandListBox->ExecuteCommand(m_pGMCommand))
	{
		m_pGMCommand = NULL;
		m_paramArray.ClearParams();
		Show(false);
	}
}

void CDlgGMParam::OnCommandBtnCancel(const char * szCommand)
{
	m_pGMCommand = NULL;
	m_paramArray.ClearParams();
	Show(false);
}


void CDlgGMParam::ShowValueControl(int nType)
{
	m_pEbxValue->Show(false);
	m_pChkValue->Show(false);
	m_pLblChkValueForChk->Show(false);
	m_pCbxValue->Show(false);

	switch (nType) 
	{
	case CParam::TYPE_NORMAL:
		m_pEbxValue->Show(true);
		break;
	case CParam::TYPE_BOOL:
		m_pChkValue->Show(true);
		m_pLblChkValueForChk->Show(true);
		break;
	case CParam::TYPE_LIST:
		m_pCbxValue->Show(true);
		break;
	}
}

void CDlgGMParam::FillListWithDisplayValues(const CParamArray &displayValues)
{
	m_pCbxValue->ResetContent();
	CParamArray::const_iterator iter = displayValues.begin();
	for (; displayValues.end() != iter; ++iter)
	{
		const CParam *pParam = *iter;
		int nIndex = m_pCbxValue->AddString(pParam->GetName()) - 1;
		m_pCbxValue->SetItemDataPtr(nIndex, static_cast<void *>(const_cast<CParam *>(pParam)));
	}
	m_pCbxValue->SortItems(AUICOMBOBOX_SORT_ASCENT);
}

void CDlgGMParam::SelectInValueList(const CParam &param)
{
	int nCount = m_pCbxValue->GetCount();
	ASSERT(nCount > 0);
	for (int nIndex = 0; nIndex < nCount; ++nIndex)
	{
		const CParam *pParam = 
			static_cast<const CParam *>(m_pCbxValue->GetItemDataPtr(nIndex));
		ASSERT(pParam);
		if (pParam->IsEqualTo(*pParam))
		{
			m_pCbxValue->SetCurSel(nIndex);
			return;
		}
	}
	m_pCbxValue->SetCurSel(0);
}

