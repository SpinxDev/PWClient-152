// Filename	: DlgMessageBox.cpp
// Creator	: Xu Wenbin
// Date		: 2012/8/7

#include "DlgMessageBox.h"

#include <AUITextArea.h>
#include <AUIManager.h>

CDlgMessageBox::CDlgMessageBox()
: m_pTxt_Message(NULL)
, m_pBtn_YES(NULL)
, m_pBtn_NO(NULL)
, m_pBtn_OK(NULL)
, m_messageDefaultHeight(0)
{
}

bool CDlgMessageBox::OnInitDialog()
{
	ImportStringTable("Interfaces\\MsgBox.stf");

	DDX_Control("Txt_Message", m_pTxt_Message);
	DDX_Control("Btn_Yes", m_pBtn_YES);
	DDX_Control("Btn_No", m_pBtn_NO);
	DDX_Control("Btn_OK", m_pBtn_OK);
	
	m_pTxt_Message->SetForceRenderScroll(true);
	m_messageDefaultHeight = m_pTxt_Message->GetDefaultSize().cy;

	//	设置 AUIDIALOG_TYPE_MESSAGEBOX 类型，以便能进入 CECGameUIMan::OnMessageBox 统一处理
	SetType(AUIDIALOG_TYPE_MESSAGEBOX);

	//	取消 ESC 键导致 IDCANCEL
	SetCanEscClose(false);

	return true;
}

void CDlgMessageBox::SetMessage(const ACHAR *szText)
{
	m_pTxt_Message->SetText(szText ? szText : _AL(""));
	ResizeByContent();
}

const ACHAR * CDlgMessageBox::GetMessage()const
{
	return m_pTxt_Message->GetText();
}

PAUIOBJECT	CDlgMessageBox::GetButton(Button btn)
{
	PAUIOBJECT pObj(NULL);
	switch (btn)
	{
	case BTN_YES:	pObj = m_pBtn_YES; break;
	case BTN_NO:	pObj = m_pBtn_NO; break;
	case BTN_OK:	pObj = m_pBtn_OK; break;
	}
	return pObj;
}

void CDlgMessageBox::SetShow(Button btn, bool bShow)
{
	PAUIOBJECT pObj = GetButton(btn);
	if (pObj) pObj->Show(bShow);
}

void CDlgMessageBox::SetText(Button btn, const ACHAR *szText)
{
	PAUIOBJECT pObj = GetButton(btn);
	if (pObj) pObj->SetText(szText ? szText : _AL(""));
}

void CDlgMessageBox::Resize(A3DRECT rcOld, A3DRECT rcNew)
{
	CDlgBase::Resize(rcOld, rcNew);
	ResizeByContent();
}

void CDlgMessageBox::OnChangeLayoutEnd(bool bAllDone)
{
	ResizeByContent();
}

int CDlgMessageBox::GetContentMaxHeight(){
	return 200;
}

int CDlgMessageBox::GetContentRequiredHeight(){
	int scaledHeight = m_pTxt_Message->GetRequiredHeight();
	float fWindowScale = GetAUIManager()->GetWindowScale() * GetScale();
	return (int)ceilf(scaledHeight / fWindowScale);
}

int CDlgMessageBox::CalculateContentHeight(){
	int result = GetContentRequiredHeight();
	a_Clamp(result, m_messageDefaultHeight, GetContentMaxHeight());
	return result;
}

int CDlgMessageBox::GetCotentCurrentHeight(){
	return m_pTxt_Message->GetDefaultSize().cy;
}

bool CDlgMessageBox::GetShowContentScroll(){
	return GetContentRequiredHeight() > CalculateContentHeight();
}

void CDlgMessageBox::AdjustContent(int deltaHeight){
	m_pTxt_Message->SetForceRenderScroll(GetShowContentScroll());
	
	SIZE size = m_pTxt_Message->GetDefaultSize();
	size.cy += deltaHeight;
	m_pTxt_Message->SetDefaultSize(size.cx, size.cy);
}

void CDlgMessageBox::AdjustButtons(int deltaHeight){
	PAUIOBJECT pObjectsToAdjust[] = {m_pBtn_YES, m_pBtn_NO, m_pBtn_OK};
	for (int i(0); i < sizeof(pObjectsToAdjust)/sizeof(pObjectsToAdjust[0]); ++ i){
		PAUIOBJECT pObj = pObjectsToAdjust[i];
		POINT point = pObj->GetDefaultPos();
		point.y += deltaHeight;
		pObj->SetDefaultPos(point.x, point.y);
	}
}

void CDlgMessageBox::AdjustDialog(int deltaHeight){
	SIZE size = GetDefaultSize();
	size.cy += deltaHeight;
	SetDefaultSize(size.cx, size.cy);
}

void CDlgMessageBox::ResizeByContent()
{
	int deltaHeight = CalculateContentHeight() - GetCotentCurrentHeight();
	AdjustContent(deltaHeight);
	AdjustButtons(deltaHeight);
	AdjustDialog(deltaHeight);
}