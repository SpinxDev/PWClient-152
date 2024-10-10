// FWDialogParam.cpp: implementation of the FWDialogParam class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogParam.h"
#include "AUIEditBox.h"
#include "AUIListBox.h"
#include "AUILabel.h"

#define new A_DEBUG_NEW



FW_IMPLEMENT_DYNCREATE(FWDialogParam, FWDialogBase)

BEGIN_EVENT_MAP_FW(FWDialogParam, FWDialogBase)

ON_EVENT_FW("LIST_PARAM", WM_LBUTTONDOWN, FWDialogParam::OnLButtonDownListParam)

END_EVENT_MAP_FW()


FWDialogParam::FWDialogParam() :
	m_nLastSelInListParam(-1)
{

}

FWDialogParam::~FWDialogParam()
{

}

bool FWDialogParam::Init(AUIDialog * pDlg)
{
	if (!FWDialogBase::Init(pDlg)) return false;
	
	m_pListParam = (AUIListBox *) GetDlgItem("LIST_PARAM");
	ASSERT(m_pListParam);

	m_pLabelParam = (AUILabel *) GetDlgItem("LBL_PARAM");
	ASSERT(m_pLabelParam); 

	m_pEditParam = (AUIEditBox *) GetDlgItem("EDIT_PARAM");
	ASSERT(m_pEditParam);

	return true;
}

void FWDialogParam::OnOK()
{
	UpdateParam();

	Show(false);

	FWDialogBase::OnOK();
}

void FWDialogParam::OnCancel()
{
	Show(false);

	FWDialogBase::OnCancel();
}

void FWDialogParam::Show(bool bShow, bool bModal)
{
	m_pListParam->ResetContent();
	for (int i = 0; i < m_aryParam.GetSize(); i++)
	{
		m_pListParam->AddString(m_aryParam[i].GetName());
	}
	m_nLastSelInListParam = -1;

	m_pEditParam->SetDataPtr(NULL);

	if (m_pListParam->GetCount() > 0)
	{
		m_pListParam->SetCurSel(0);
		OnSelChangeListParam(0);
	}
	else
	{
		OnSelChangeListParam(-1);
	}

	FWDialogBase::Show(bShow, bModal);
}

void FWDialogParam::OnLButtonDownListParam(WPARAM wParam, LPARAM lParam)
{
	OnLButtonDownListboxTemplate(
		wParam, 
		lParam,
		m_pListParam,
		&m_nLastSelInListParam,
		(FuncOnSelChangeListbox) &FWDialogParam::OnSelChangeListParam);
}

void FWDialogParam::OnSelChangeListParam(int nSelIndex)
{
	UpdateParam();

	if (nSelIndex == -1 || nSelIndex >= m_pListParam->GetCount())
	{
		m_pEditParam->Show(false);
		m_pLabelParam->Show(false);
		return;
	}

	m_pEditParam->Show(true);
	m_pLabelParam->Show(true);
	
	FWParam *pParam = &m_aryParam[nSelIndex];

	m_pEditParam->SetDataPtr(pParam);

	ShowProp();
}

void FWDialogParam::ShowProp()
{
	FWParam *pParam = (FWParam *) m_pEditParam->GetDataPtr();
	
	if (!pParam) return;

	ACString tmp1, tmp2;
	
	switch(pParam->GetType()) 
	{
	case GFX_VALUE_INT:
		tmp1.Format(_AL("%s£º(%d ¡« %d)"), pParam->GetName(), (int)pParam->GetMin(), (int)pParam->GetMax());
		tmp2.Format(_AL("%d"), (int)pParam->GetVal());
		break;
	case GFX_VALUE_FLOAT:
		tmp1.Format(_AL("%s£º(%.3f ¡« %.3f)"), pParam->GetName(), (float)pParam->GetMin(), (float)pParam->GetMax());
		tmp2.Format(_AL("%.3f"), (float)pParam->GetVal());
		break;
	case GFX_VALUE_STRING:
		tmp1.Format(_AL("%s£º"), pParam->GetName());
		tmp2 = AS2S(pParam->GetVal().m_string);
		break;
#ifdef _DEBUG
	default:
		ASSERT(false);
		break;
#endif // _DEBUG
	}
	m_pLabelParam->SetText(tmp1);
	m_pEditParam->SetText(tmp2);
}

void FWDialogParam::UpdateParam()
{
	FWParam *pParam = (FWParam *) m_pEditParam->GetDataPtr();
	
	if (!pParam) return;
	
	ACString tmp = m_pEditParam->GetText();
	
	switch(pParam->GetType()) 
	{
	case GFX_VALUE_INT:
		{
			int i = tmp.ToInt();
			a_ClampFloor<int>(i, pParam->GetMin());
			a_ClampRoof<int>(i, pParam->GetMax());
			pParam->SetVal(i);
		}
		break;
	case GFX_VALUE_FLOAT:
		{
			float f = tmp.ToFloat();
			a_ClampFloor<float>(f, pParam->GetMin());
			a_ClampRoof<float>(f, pParam->GetMax());
			pParam->SetVal(f);
		}
		break;
	case GFX_VALUE_STRING:
		pParam->SetVal(GFX_PROPERTY(S2AS(tmp)));
		break;
#ifdef _DEBUG
	default:
		ASSERT(false);
		break;
#endif // _DEBUG
	}
}
