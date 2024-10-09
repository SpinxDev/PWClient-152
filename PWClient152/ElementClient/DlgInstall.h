// File		: DlgInstall.h
// Creator	: Xiao Zhou
// Date		: 2005/8/16

#pragma once

#include "DlgBase.h"

class AUIImagePicture;
class AUIEditBox;
class CECIvtrItem;

class CDlgInstall : public CDlgBase  
{
	friend class CECGameUIMan;

	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgInstall();
	virtual ~CDlgInstall();

	void OnCommandCANCEL(const char * szCommand);
	void OnCommandStart(const char * szCommand);
	void OnEventLButtonDown_Equipment(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Stone(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();
	virtual bool OnInitDialog();

	void ClearItem(PAUIOBJECT pObj);
	void ClearEquipment();
	void ClearStone();
	void SetEquipmentName(const ACHAR *szName);
	void SetStoneName(const ACHAR *szName);
	
	AUIImagePicture *	m_pItema;
	AUIImagePicture *	m_pItemb;
	PAUIOBJECT		m_pTxtGold;
};
