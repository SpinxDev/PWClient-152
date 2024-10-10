// AudioEditorView.cpp : implementation of the CAudioEditorView class
//

#include "stdafx.h"
#include "AudioEditor.h"

#include "AudioEditorDoc.h"
#include "AudioEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAudioEditorView

IMPLEMENT_DYNCREATE(CAudioEditorView, CView)

BEGIN_MESSAGE_MAP(CAudioEditorView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CAudioEditorView construction/destruction

CAudioEditorView::CAudioEditorView()
{
	// TODO: add construction code here

}

CAudioEditorView::~CAudioEditorView()
{
}

BOOL CAudioEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CAudioEditorView drawing

void CAudioEditorView::OnDraw(CDC* /*pDC*/)
{
	CAudioEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

//	ShowWindow(SW_HIDE);

	// TODO: add draw code for native data here
}


// CAudioEditorView printing

void CAudioEditorView::OnFilePrintPreview()
{
	BCGPPrintPreview (this);
}

BOOL CAudioEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAudioEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAudioEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CAudioEditorView diagnostics

#ifdef _DEBUG
void CAudioEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CAudioEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAudioEditorDoc* CAudioEditorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAudioEditorDoc)));
	return (CAudioEditorDoc*)m_pDocument;
}
#endif //_DEBUG

void CAudioEditorView::SetCurView(VIEW_TYPE type)
{	
	switch (type)
	{
	case VIEW_BANK:
		m_wndBankView.ShowWindow(SW_SHOWNORMAL);
		m_wndEventView.ShowWindow(SW_HIDE);
		m_wndSoundDefView.ShowWindow(SW_HIDE);
		break;
	case VIEW_SOUNDDEF:
		m_wndSoundDefView.ShowWindow(SW_SHOWNORMAL);
		m_wndBankView.ShowWindow(SW_HIDE);
		m_wndEventView.ShowWindow(SW_HIDE);
		break;
	case VIEW_EVENT:
		m_wndEventView.ShowWindow(SW_SHOWNORMAL);
		m_wndBankView.ShowWindow(SW_HIDE);
		m_wndSoundDefView.ShowWindow(SW_HIDE);
		break;
	}
}

int CAudioEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_wndEventView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0,0,0,0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create event view\n");
		return -1;
	}
	m_wndEventView.ShowWindow(SW_HIDE);

	if(!m_wndSoundDefView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0,0,0,0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create sound def view\n");
		return -1;
	}
	m_wndSoundDefView.ShowWindow(SW_HIDE);


	if(!m_wndBankView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0,0,0,0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create bank view\n");
		return -1;
	}
	m_wndBankView.ShowWindow(SW_HIDE);

	return 0;
}

void CAudioEditorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_wndEventView.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndSoundDefView.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndBankView.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
}
