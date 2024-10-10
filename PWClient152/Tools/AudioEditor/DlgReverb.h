#pragma once
#include "bcgplistbox.h"
#include "PropListCtrl/PropertyInterface.h"

class CBCGPPropertyListWrapper;
class CPropertyArray;
class CPropertyUpdateListener;

enum
{
	idReverbName,
	idRoom,
	idDecayTime,
	idHFDecayRatio,
	idPreDelay,
	idLateDelay,
	idEarlyReflections,
	idLateReflections,
	idDiffusion,
	idDensity,
	idRoomHF,
	idRoomLF,
	idHFCrossover,
	idLFCrossover
};


// CDlgReverb dialog

class CDlgReverb : public CBCGPDialog, public CPropertyInterface
{
	DECLARE_DYNAMIC(CDlgReverb)

public:
	CDlgReverb(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgReverb();

// Dialog Data
	enum { IDD = IDD_REVERB };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnPropertyChanged (WPARAM,LPARAM);
	afx_msg void OnLbnSelchangeListReverb();
	afx_msg void OnBnClickedNewReverb();
	afx_msg void OnBnClickedDelReberb();
	afx_msg void OnBnClickedReverbSave();
	DECLARE_MESSAGE_MAP()
protected:
	void buildProperty();
	void updateListBox();
	bool save();
	bool load();
	virtual void UpdateProperty(bool bSave, bool bIsCommit = false);
protected:
	CBCGPListBox m_wndListBox;
	bool m_bModified;
	CBCGPPropertyListWrapper* m_pBCGPPropListWrapper;
	CPropertyArray* m_pProperties;
	CPropertyUpdateListener* m_pPropertyListener;
public:
	afx_msg void OnBnClickedOk();
};
