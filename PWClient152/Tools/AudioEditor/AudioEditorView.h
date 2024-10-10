// AudioEditorView.h : interface of the CAudioEditorView class
//

#pragma once

#include "EventView.h"
#include "SoundDefView.h"
#include "BankView.h"
#include "AudioEditorDoc.h"

typedef enum
{
	VIEW_NONE,
	VIEW_EVENT,
	VIEW_SOUNDDEF,
	VIEW_BANK
}VIEW_TYPE;

class CAudioEditorView : public CView
{
protected: // create from serialization only
	CAudioEditorView();
	DECLARE_DYNCREATE(CAudioEditorView)

// Attributes
public:
	CAudioEditorDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CAudioEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CEventView				m_wndEventView;
	CSoundDefView			m_wndSoundDefView;
	CBankView				m_wndBankView;

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	DECLARE_MESSAGE_MAP()
public:
	void					SetCurView(VIEW_TYPE type);
	CBankView*				GetBankView() { return &m_wndBankView; }
	CSoundDefView*			GetSoundDefView() { return &m_wndSoundDefView; }
	CEventView*				GetEventView() { return &m_wndEventView; }
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // debug version in AudioEditorView.cpp
inline CAudioEditorDoc* CAudioEditorView::GetDocument() const
   { return reinterpret_cast<CAudioEditorDoc*>(m_pDocument); }
#endif

