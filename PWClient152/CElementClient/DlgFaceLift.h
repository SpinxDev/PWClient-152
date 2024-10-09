// Filename	: DlgFaceLift.h
// Creator	: Tom Zhou
// Date		: October 30, 2005

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"

class CDlgFaceLift : public CDlgBase  
{
	friend class CECGameUIMan;

	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgFaceLift();
	virtual ~CDlgFaceLift();

	void OnCommand_start(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	PAUIIMAGEPICTURE m_pImgItem;

	virtual bool OnInitDialog();
};
