#pragma once
#include "afxwin.h"
#include "resource.h"

using AudioEngine::Event;
using AudioEngine::EventLayer;

// CDlgLayerProp �Ի���

class CDlgLayerProp : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgLayerProp)

public:
	CDlgLayerProp(Event* pEvent, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLayerProp();

// �Ի�������
	enum { IDD = IDD_LAYER_PROP};
public:
	void	SetEventLayer(EventLayer* pEventLayer) { m_pEventLayer = pEventLayer; }
	const char* GetName() const { return m_strName; }
	const char* GetParamName() const { return m_strParamName; }

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
protected:
	CString m_strName;
	CString m_strParamName;
	Event*		m_pEvent;
	EventLayer*	m_pEventLayer;
	CComboBox m_wndParamCombo;
public:
	afx_msg void OnBnClickedOk();
};
