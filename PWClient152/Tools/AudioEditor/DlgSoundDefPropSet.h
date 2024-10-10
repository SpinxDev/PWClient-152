#pragma once

#include "FSoundDef.h"
#include "SoundDefPropList.h"
#include "PropListCtrl/PropertyInterface.h"

using AudioEngine::SOUND_DEF_PROPERTY;

class CBCGPPropertyListWrapper;
class CPropertyArray;
class CPropertyUpdateListener;

// CDlgSoundDefPropSet dialog

class CDlgSoundDefPropSet : public CBCGPDialog, public CPropertyInterface
{
	DECLARE_DYNAMIC(CDlgSoundDefPropSet)

public:
	CDlgSoundDefPropSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSoundDefPropSet();
public:
	SOUND_DEF_PROPERTY GetProp() const { return m_SoundDefProperty; }

// Dialog Data
	enum { IDD = IDD_SOUNDDEF_PROP_FRM };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual void UpdateProperty(bool bSave, bool bIsCommit = false);
protected:
	SOUND_DEF_PROPERTY m_SoundDefProperty;
	CSoundDefPropList m_SoundDefPropList;	
	CBCGPPropertyListWrapper* m_pBCGPPropListWrapper;
	CPropertyArray* m_pProperties;
	CPropertyUpdateListener* m_pPropertyListener;
};
