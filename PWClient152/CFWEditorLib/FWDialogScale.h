#ifndef _FWDIALOGSCALE_H_
#define _FWDIALOGSCALE_H_

#include "FWDialogBase.h"

class AUIEditBox;

class FWDialogScale : public FWDialogBase
{
	FW_DECLARE_DYNCREATE(FWDialogScale)
	DECLARE_EVENT_MAP_FW()
protected:
	AUIEditBox * m_pEditScale;
public:
	float m_fScale;
public:
	FWDialogScale();
	virtual ~FWDialogScale();

	virtual bool Init(AUIDialog * pDlg);
	virtual void Show (bool bShow, bool bModal = false);
	virtual void OnOK();
	virtual void OnCancel();
};

#endif 