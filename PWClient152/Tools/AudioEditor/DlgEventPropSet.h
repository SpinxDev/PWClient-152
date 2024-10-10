#pragma once

#include "FAudioEngine.h"
#include "EventPropList.h"
#include "PropListCtrl/PropertyInterface.h"

using AudioEngine::EVENT_PROPERTY;

class CBCGPPropertyListWrapper;
class CPropertyArray;
class CPropertyUpdateListener;

// CDlgEventPropSet dialog

class CDlgEventPropSet : public CBCGPDialog, public CPropertyInterface
{
	DECLARE_DYNAMIC(CDlgEventPropSet)

public:
	CDlgEventPropSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEventPropSet();
public:
	EVENT_PROPERTY GetProp() const { return m_EventProperty; }

// Dialog Data
	enum { IDD = IDD_EVENT_PROP };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual void UpdateProperty(bool bSave, bool bIsCommit = false);
protected:
	EVENT_PROPERTY m_EventProperty;
	CEventPropList m_EventPropList;	
	CBCGPPropertyListWrapper* m_pBCGPPropListWrapper;
	CPropertyArray* m_pProperties;
	CPropertyUpdateListener* m_pPropertyListener;
};
