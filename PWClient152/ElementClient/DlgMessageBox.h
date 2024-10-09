// Filename	: DlgMessageBox.h
// Creator	: Xu Wenbin
// Date		: 2012/8/7

#ifndef _DLGMESSAGEBOX_H_
#define _DLGMESSAGEBOX_H_ 

#include "DlgBase.h"

class AUITextArea;
class CDlgMessageBox : public CDlgBase  
{
public:
	CDlgMessageBox();
	virtual ~CDlgMessageBox(){}

	enum Button
	{
		BTN_YES,
		BTN_NO,
		BTN_OK
	};

	void SetContext(const char *szText){ m_strContext = szText; }
	const char * GetContext()const{ return m_strContext; }

	void SetMessage(const ACHAR *szText);
	const ACHAR * GetMessage()const;

	void SetText(Button btn, const ACHAR *szText);
	void SetShow(Button btn, bool bShow);
	
protected:	
	virtual bool OnInitDialog();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);
	virtual void OnChangeLayoutEnd(bool bAllDone);
	
	int  GetContentMaxHeight();
	int	 GetContentRequiredHeight();
	int	 GetCotentCurrentHeight();
	int	 CalculateContentHeight();
	bool GetShowContentScroll();
	void AdjustContent(int deltaHeight);
	void AdjustButtons(int deltaHeight);
	void AdjustDialog(int deltaHeight);
	void ResizeByContent();

	PAUIOBJECT	GetButton(Button btn);
	
	AUITextArea*	m_pTxt_Message;
	PAUIOBJECT		m_pBtn_YES;
	PAUIOBJECT		m_pBtn_NO;
	PAUIOBJECT		m_pBtn_OK;

	AString			m_strContext;
	int				m_messageDefaultHeight;			//	m_pTxt_Message 的默认高度
};

#endif //_DLGMESSAGEBOX_H_ 