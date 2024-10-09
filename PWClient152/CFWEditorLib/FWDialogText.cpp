// FWDialogText.cpp: implementation of the FWDialogText class.
//
//////////////////////////////////////////////////////////////////////

#include <AString.h>
#include <AAssist.h>
#include "FWDialogText.h"
#include "AUIComboBox.h"
#include "FWFontEnum.h"
#include "AUIEditBox.h"
#include "AUIRadioButton.h"
#include "FWArtCreator.h"
#include "AUIListBox.h"
#include "FWArt.h"
#include "FWDialogParam.h"


#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNCREATE(FWDialogText, FWDialogBase)

BEGIN_EVENT_MAP_FW(FWDialogText, FWDialogGraph)

ON_EVENT_FW("LIST_ART", WM_LBUTTONDOWN, OnLButtonDownListArt)
ON_EVENT_FW("BTN_ART", WM_LCLICK, OnClickBtnArt)

END_EVENT_MAP_FW()



FWDialogText::FWDialogText()
{
	m_pArt = NULL;
	m_nLastSelIndexArt = -1;
}

FWDialogText::~FWDialogText()
{
	SAFE_DELETE(m_pArt);
}

bool FWDialogText::Init(AUIDialog * pDlg)
{
	if (!FWDialogGraph::Init(pDlg)) return false;

	m_pEditText = (AUIEditBox *) GetDlgItem("EDIT_TEXT");
	ASSERT(m_pEditText);

	m_pCmbFont = (AUIComboBox *) GetDlgItem("CMB_FONT");
	ASSERT(m_pCmbFont);

	m_pCmbPoint = (AUIComboBox *) GetDlgItem("CMB_POINT");
	ASSERT(m_pCmbPoint);

	for (int i = 0; i < 4; i++)
	{
		AString name;
		name.Format("RAD_STYLE%d", i);
		m_ppRadStyle[i] = (AUIRadioButton *) GetDlgItem(name);
		ASSERT(m_ppRadStyle[i]);
	}

	m_pListArt = (AUIListBox *) GetDlgItem("LIST_ART");
	ASSERT(m_pListArt);

	InitFontCombo();

	InitPointCombo();
	
	InitArtList();
	
	return true;
}

void FWDialogText::OnOK()
{
	m_strText = m_pEditText->GetText();
	
	if (m_pCmbFont->GetCurSel() != -1)
		m_strFont = m_pCmbFont->GetText(m_pCmbFont->GetCurSel());
	else
		m_strFont = _AL("ËÎÌו");

	if (m_pCmbPoint->GetCurSel() != -1)
	{
		ACString tmp = m_pCmbPoint->GetText(m_pCmbPoint->GetCurSel());
		m_nPointSize = tmp.ToInt();
	}
	else
	{
		m_nPointSize = 9;
	}

	for (int i = 0; i < 4; i++)
	{
		if (m_ppRadStyle[i]->IsChecked())
		{
			m_nFontStyle = Index2Style(i);
			break;
		}
	}

	FWDialogGraph::OnOK();
}

void FWDialogText::InitFontCombo()
{
	const AList2<ACString, ACString &> &lstFont = FWFontEnum::GetInstance()->GetFontList();
	ALISTPOSITION pos = lstFont.GetHeadPosition();
	while (pos)
	{
		m_pCmbFont->AddString(lstFont.GetNext(pos));
	}
}

void FWDialogText::InitPointCombo()
{
	ACString tmp;
	for (int i = 1; i < 100; i++)
	{
		tmp.Format(_AL("%d"), i);
		m_pCmbPoint->AddString(tmp);
	}
}

int FWDialogText::Style2Index(int nStyle)
{
	if (nStyle & STYLE_BOLD && nStyle & STYLE_ITALIC)
		return 3;
	else if (nStyle & STYLE_ITALIC)
		return 2;
	else if (nStyle & STYLE_BOLD)
		return 1;
	else
		return 0;
}

int FWDialogText::Index2Style(int nIndex)
{
	switch(nIndex) {
	case 0:
		return STYLE_NORMAL;
	case 1:
		return STYLE_BOLD;
	case 2:
		return STYLE_ITALIC;
	case 3:
		return STYLE_BOLD | STYLE_ITALIC;
	}
	ASSERT(false);
	return -1;
}

void FWDialogText::SelectInFontCombo()
{
	m_pCmbFont->SetCurSel(-1);
	for (int i = 0; i < m_pCmbFont->GetCount(); i++)
	{
		if (m_strFont == m_pCmbFont->GetText(i))
		{
			m_pCmbFont->SetCurSel(i);
			break;
		}
	}

}

void FWDialogText::SelectInPointCombo()
{
	m_pCmbPoint->SetCurSel(-1);
	for (int i = 0; i < m_pCmbPoint->GetCount(); i++)
	{
		ACString tmp = m_pCmbPoint->GetText(i);
		if (tmp.ToInt() == m_nPointSize)
		{
			m_pCmbPoint->SetCurSel(i);
			break;
		}
	}

}

void FWDialogText::OnShowDialog()
{
	m_pEditText->SetText(m_strText);
	
	SelectInFontCombo();
	
	SelectInPointCombo();
	
	m_ppRadStyle[Style2Index(m_nFontStyle)]->Check(true);

	SelectInArtList();
	m_nLastSelIndexArt = m_pListArt->GetCurSel();

	FWDialogGraph::OnShowDialog();
}

void FWDialogText::InitArtList()
{
	FWArtCreator *pCreator = FWArtCreator::GetInstance();
	
	for (int i = 0; i < FWArtCreator::ID_ART_COUNT; i++)
	{
		FWArtInfo *pInfo = pCreator->FindByID(i);
		m_pListArt->AddString(AS2S(pInfo->Name));
		m_pListArt->SetItemData(i, (DWORD) pInfo);
	}
}

void FWDialogText::SetArt(const FWArt *pArt)
{
	SAFE_DELETE(m_pArt);
	m_pArt = pArt->Clone();
}

const FWArt * FWDialogText::GetArt()
{
	return m_pArt;
}

void FWDialogText::SelectInArtList()
{
	m_pListArt->SetCurSel(-1);

	int nCount = m_pListArt->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		FWArtInfo * pInfo = (FWArtInfo *) m_pListArt->GetItemData(i);
		if (pInfo->pClass == m_pArt->GetRuntimeClass())
		{
			m_pListArt->SetCurSel(i);
			return;
		}
	}
}

void FWDialogText::OnLButtonDownListArt(WPARAM wParam, LPARAM lParam)
{
	OnLButtonDownListboxTemplate(
		wParam,
		lParam,
		m_pListArt,
		&m_nLastSelIndexArt,
		(FuncOnSelChangeListbox) &FWDialogText::OnSelChangeListArt);
}

void FWDialogText::OnSelChangeListArt(int nSelIndex)
{
	if (nSelIndex == -1 || nSelIndex >= m_pListArt->GetCount()) 
	{
		m_pListArt->SetCurSel(0);
		nSelIndex = 0;
	}
	
	FWArtInfo *pInfo = (FWArtInfo *) m_pListArt->GetItemData(nSelIndex);
	
	SAFE_DELETE(m_pArt);
	m_pArt = FWArtCreator::GetInstance()->CreateArt(pInfo->ID);
}

void FWDialogText::OnClickBtnArt(WPARAM wParam, LPARAM lParam)
{
	CheckListbox(m_pListArt, (FuncOnSelChangeListbox) &FWDialogText::OnSelChangeListArt);
	
	FWDialogParam *pDlg = GetFWDialog2(FWDialogParam);
	
	pDlg->m_aryParam.RemoveAll(false);
	m_pArt->FillParamArray(pDlg->m_aryParam);
	
	pDlg->DoModal(OnDoModalArtCallback, this);
}

void FWDialogText::OnDoModalArtCallback(void *pData)
{
	FWDialogText *pThis = (FWDialogText *) pData;
	
	FWDialogParam *pDlg = 
		static_cast<FWDialogParam *>(pThis->GetFWDialog(TO_STRING(FWDialogParam)));
	pThis->m_pArt->UpdateFromParamArray(pDlg->m_aryParam);
}