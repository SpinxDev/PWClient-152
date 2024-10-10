#ifndef _FWDIALOGGRAPH_H_
#define _FWDIALOGGRAPH_H_

#include "FWDialogPropBase.h"

class FWDialogGraph : public FWDialogPropBase  
{
	FW_DECLARE_DYNCREATE(FWDialogGraph)
	DECLARE_EVENT_MAP_FW()
protected:
	virtual int GetProfileType();
	virtual void OnShowDialog();
	
	AUIEditBox * m_pEditThickness;
public:
	float m_fThickness;
public:
	FWDialogGraph();
	virtual ~FWDialogGraph();
	
	virtual bool Init(AUIDialog * pDlg);
	virtual void OnOK();
};

#endif 