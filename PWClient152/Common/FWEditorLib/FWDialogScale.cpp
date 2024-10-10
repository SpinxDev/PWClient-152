// FWDialogScale.cpp: implementation of the FWDialogScale class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogScale.h"
#include "AUIEditBox.h"
#include "FWView.h"
#include "FWDoc.h"

#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNCREATE(FWDialogScale, FWDialogBase)

BEGIN_EVENT_MAP_FW(FWDialogScale, FWDialogBase)

//ON_EVENT_FW("BTN_OK", WM_LCLICK, FWDialogScale::OnClickBtnOK)
//ON_EVENT_FW("BTN_CANCEL", WM_LCLICK, FWDialogScale::OnClickBtnCancel)

END_EVENT_MAP_FW()



FWDialogScale::FWDialogScale() :
	m_fScale(.5f)
{

}

FWDialogScale::~FWDialogScale()
{

}

void FWDialogScale::Show (bool bShow, bool bModal)
{
	if (bShow)
	{
		ACString str;
		str.Format(_AL("%.3f"), m_fScale);
		m_pEditScale->SetText(str);
	}

	FWDialogBase::Show(bShow, bModal);
}

void FWDialogScale::OnOK()
{
	m_fScale = ACString(m_pEditScale->GetText()).ToFloat();
	
	Show(false);

	FWDialogBase::OnOK();
}

void FWDialogScale::OnCancel()
{
	Show(false);

	FWDialogBase::OnCancel();
}

bool FWDialogScale::Init(AUIDialog * pDlg)
{
	FWDialogBase::Init(pDlg);

	m_pEditScale = (AUIEditBox *) GetDlgItem("EDIT_SCALE");
	ASSERT(m_pEditScale);

	return true;
}


