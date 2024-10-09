// EleDataManView.cpp : implementation of the CEleDataManView class
//

#include "stdafx.h"
#include "EleDataMan.h"

#include "EleDataManDoc.h"
#include "EleDataManView.h"
#include "PrintDataDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEleDataManView

IMPLEMENT_DYNCREATE(CEleDataManView, CView)

BEGIN_MESSAGE_MAP(CEleDataManView, CView)
	//{{AFX_MSG_MAP(CEleDataManView)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_VIEW_SHOWDATA, OnViewShowdata)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEleDataManView construction/destruction

CEleDataManView::CEleDataManView()
{
	// TODO: add construction code here
	m_fSearchTime = 0;
	m_nTimes = 0;
}

CEleDataManView::~CEleDataManView()
{
}

BOOL CEleDataManView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CEleDataManView drawing

void CEleDataManView::OnDraw(CDC* pDC)
{
	CEleDataManDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	CString str;
	str.Format("Search time = %f ms", m_fSearchTime);
//	str.Format("Search counts = %d \n\r Search time = %d ms", m_nTimes, m_nSearchTime);
	pDC->TextOut(10,30, str);

	str.Format("Search counts = %d", m_nTimes);
	pDC->TextOut(10,10, str);

}

/////////////////////////////////////////////////////////////////////////////
// CEleDataManView printing

BOOL CEleDataManView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CEleDataManView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CEleDataManView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CEleDataManView diagnostics

#ifdef _DEBUG
void CEleDataManView::AssertValid() const
{
	CView::AssertValid();
}

void CEleDataManView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEleDataManDoc* CEleDataManView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEleDataManDoc)));
	return (CEleDataManDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEleDataManView message handlers

void CEleDataManView::OnLButtonDown(UINT nFlags, CPoint point) 
{
/*	// TODO: Add your message handler code here and/or call default
	m_nTimes = 1000000;
//	m_nSearchTime = GetDocument()->SearchTest(m_nTimes);
	m_fSearchTime = GetDocument()->GenerateTest(m_nTimes);

	CString str;
	str.Format(_T("Search counts = %d\nSearch time = %f ms"), m_nTimes, m_fSearchTime);
	AfxMessageBox(str);
	
	CView::OnLButtonDown(nFlags, point);
	Invalidate(TRUE);
*/
}

void CEleDataManView::OnViewShowdata() 
{
	// TODO: Add your command handler code here
	CPrintDataDlg dlg;
	dlg.DoModal();
	
}
