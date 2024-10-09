// Filename	: DlgMailWrite.h
// Creator	: Xiao Zhou
// Date		: 2005/10/31

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

#include <vector.h>

class CECIvtrItem;

class CDlgMailWrite : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgMailWrite();
	virtual ~CDlgMailWrite();

	void OnCommandCancel(const char* szCommand);
	void OnCommandSend(const char* szCommand);

	void OnEvent_LButtonDown_AttachItem(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	typedef abase::vector<int> MassReceivers;
	void CreateNewMail(const ACHAR *strMailto = NULL, ACString szSubject = _AL(""), int nCost=500, const MassReceivers &mass=MassReceivers());
	void SetAttachItem(CECIvtrItem *pItem, int nItemNumber, int nItemPos);
	void SendMail(int idPlayer);
	void SendMailRe(void *pData);
	void AttachGold(int money);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual bool OnInitDialog();
	virtual void OnTick();

	bool	IsSendToMass();

	PAUIOBJECT				m_pTxt_MailTo;
	PAUIOBJECT				m_pTxt_Subject;
	PAUIOBJECT				m_pTxt_Cost;
	PAUIEDITBOX				m_pTxt_Content;
	PAUIEDITBOX				m_pTxt_AttachGold;
	PAUIIMAGEPICTURE		m_pImg_AttachItem;
	PAUILABEL				m_pTxt_PackGold;
	PAUISTILLIMAGEBUTTON	m_pBtn_Send;
	int						m_idItem;
	int						m_nItemNumber;
	int						m_nItemPos;
	int						m_nCost;				//	消耗金钱
	MassReceivers			m_massReceivers;		//	群发邮件时接收者列表

};
