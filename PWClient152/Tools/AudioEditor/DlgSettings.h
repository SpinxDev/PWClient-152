#pragma once

#include "PropListCtrl/PropertyInterface.h"

// CDlgSettings dialog

class CBCGPPropertyListWrapper;
class CPropertyArray;
class CPropertyUpdateListener;

class CDlgSettings : public CBCGPDialog, public CPropertyInterface
{
	DECLARE_DYNAMIC(CDlgSettings)

public:
	CDlgSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSettings();

// Dialog Data
	enum { IDD = IDD_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
protected:
	virtual void UpdateProperty(bool bSave, bool bIsCommit = false);
	bool buildProperty();
protected:
	CBCGPPropertyListWrapper* m_pBCGPPropListWrapper;
	CPropertyArray* m_pProperties;
	CPropertyUpdateListener* m_pPropertyListener;
};
