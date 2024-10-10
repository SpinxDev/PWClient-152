#pragma once

#include "EventPropList.h"
#include "SoundDefPropList.h"
#include "PropListCtrl/PropertyInterface.h"

typedef enum
{
	PROP_NONE,
	PROP_BANK,
	PROP_BANK_ARCHIVE,
	PROP_BANK_AUDIO,
	PROP_SOUNDDEF,
	PROP_SOUNDDEF_ARCHIVE,
	PROP_SOUNDDEF_AUDIO,
	PROP_SOUNDDEF_AUDIO_GROUP,
	PROP_EVENT_GROUP,
	PROP_EVENT
}PROP_TYPE;

enum
{
	idBankName,
	idBankType,
	idBankNote
};

enum
{
	idBankArchiveName,
	idBankArchiveNote
};

enum
{
	idBankAudioName,
	idBankAudioAccurateTime,
	idBankAudioNote
};

enum
{
	idSoundDefArchiveName,
	idSoundDefArchiveNote
};

enum
{
	idAudioGroupPlayMode
};

enum
{
	idAudioVolume,
	idAudioVolumeRandom,
	idAudioPitch,
	idAudioPitchRandom,
	idAudioNote
};

enum
{
	idEventGroupName,
	idThreshold,
	idAttack,
	idRelease,
	idMakeUpGain,
	idEventGroupNote
};

class CWndLabel;
class CBCGPPropertyListWrapper;
class CPropertyArray;
class CPropertyUpdateListener;

class CPropertyBar : public CBCGPDockingControlBar, public CPropertyInterface
{
public:
	CPropertyBar(void);
	~CPropertyBar(void);
protected:
	//{{AFX_MSG(CPropertyBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	virtual void UpdateProperty(bool bSave, bool bIsCommit = false);
protected:
	PROP_TYPE m_curPropType;
	HTREEITEM m_hCurItem;
	CWndLabel* m_pWndLabel;
	CEventPropList m_EventPropList;
	CSoundDefPropList m_SoundDefPropList;	
	CBCGPPropertyListWrapper* m_pBCGPPropListWrapper;
	CPropertyArray* m_pProperties;
	CPropertyUpdateListener* m_pPropertyListener;
public:
	bool BuildProperty(PROP_TYPE type, HTREEITEM hItem);
};
