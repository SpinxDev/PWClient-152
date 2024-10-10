#pragma once
#include "treectrlex.h"
#include "SoundDefDisplay.h"
#include "DynSizeCtrl.h"

// CDlgSelectSoundDef �Ի���
using AudioEngine::SoundDefList;

class CDlgSelectSoundDef : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSelectSoundDef)

public:
	CDlgSelectSoundDef(bool bMultiSelect, const char* szTitle = "", CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSelectSoundDef();

// �Ի�������
	enum { IDD = IDD_SELECT_SOUNDDEF };
public:
	SoundDefList GetSelectSoundDefList() const { return m_listSelectSoundDef; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkTreeSounddef(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
protected:
	CImageList m_ImageList;
	CTreeCtrlEx m_wndTree;
	CSoundDefDisplay m_soundDefDisplay;
	SoundDefList m_listSelectSoundDef;
	DynSizeCtrlMan m_dscMan;
	bool m_bMultiSelect;
	CString m_csTitle;
};
