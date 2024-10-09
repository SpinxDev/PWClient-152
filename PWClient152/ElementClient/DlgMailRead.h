// Filename	: DlgMailRead.h
// Creator	: Xiao Zhou
// Date		: 2005/10/31

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUITextArea.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "AUIEditBox.h"

class CDlgMailRead : public CDlgBase  
{
	friend class CDlgMailList;

	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgMailRead();
	virtual ~CDlgMailRead();

	void OnCommandCancel(const char* szCommand);
	void OnCommandGetAttach(const char* szCommand);
	void OnCommandReply(const char* szCommand);

	void ReadMail(void *pData);
	void GetAttachRe(void *pData);

protected:
	virtual bool OnInitDialog();
	
	PAUILABEL				m_pTxt_MailFrom;
	PAUILABEL				m_pTxt_Subject;
	PAUITEXTAREA			m_pTxt_Content;
	PAUIOBJECT				m_pTxt_AttachGold;
	PAUIIMAGEPICTURE		m_pImg_AttachItem;
	PAUISTILLIMAGEBUTTON	m_pBtn_GetAttach;
	PAUISTILLIMAGEBUTTON	m_pBtn_Reply;

	int						m_idMail;
	int						m_idSender;

};
