#pragma once
#include "TreeCtrlEx.h"
#include "BankDisplay.h"
#include "DynSizeCtrl.h"

// CDlgSelectAudio �Ի���

using AudioEngine::Audio;
using AudioEngine::AudioList;

class CDlgSelectAudio : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSelectAudio)

public:
	CDlgSelectAudio(bool bMultiSelect, const char* szTitle = "", CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSelectAudio();

// �Ի�������
	enum { IDD = IDD_SELECT_AUDIO };

public:
	AudioList	GetSelectAudioList() const { return m_listSelectAudio; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkTreeBank(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
protected:
	CTreeCtrlEx			m_wndTree;
	CImageList			m_ImageList;
	AudioList			m_listSelectAudio;
	CBankDisplay		m_bankDisplay;
	DynSizeCtrlMan		m_dscMan;
	bool				m_bMultiSelect;
	CString				m_csTitle;
};
