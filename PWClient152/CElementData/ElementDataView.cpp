// ElementDataView.cpp : implementation of the CElementDataView class
//

#include "stdafx.h"
#include "ElementData.h"

#include "ElementDataDoc.h"
#include "ElementDataView.h"

#include "PropertyList.h"
#include "AObject.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CElementDataView

IMPLEMENT_DYNCREATE(CElementDataView, CFormView)

BEGIN_MESSAGE_MAP(CElementDataView, CFormView)
	//{{AFX_MSG_MAP(CElementDataView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CElementDataView construction/destruction

CElementDataView::CElementDataView()
	: CFormView(CElementDataView::IDD)
{
	//{{AFX_DATA_INIT(CElementDataView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	bInited = false;
	m_pList = NULL;
}

CElementDataView::~CElementDataView()
{
	if(m_pList) delete m_pList;
}

void CElementDataView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CElementDataView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CElementDataView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CElementDataView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	ResizeParentToFit();

	m_pList = new CPropertyList();
	CRect rc;
	GetClientRect(&rc);
	BOOL bResult = m_pList->Create("属性表", WS_CHILD | WS_VISIBLE, rc, this, 101);
	if(bResult)
	{
		CElementDataDoc* doc = GetDocument();
		m_pList->AttachDataObject(doc->m_pProperty);
	}

	bInited = true;
}

/////////////////////////////////////////////////////////////////////////////
// CElementDataView printing

BOOL CElementDataView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CElementDataView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CElementDataView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CElementDataView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CElementDataView diagnostics

#ifdef _DEBUG
void CElementDataView::AssertValid() const
{
	CFormView::AssertValid();
}

void CElementDataView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CElementDataDoc* CElementDataView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CElementDataDoc)));
	return (CElementDataDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CElementDataView message handlers

int CElementDataView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CElementDataView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	if(bInited)
	{
		if(m_pList)
		{
			m_pList->DestroyWindow();
			delete m_pList;
			m_pList = NULL;
		}
		m_pList = new CPropertyList();
		CRect rc;
		GetClientRect(&rc);
		BOOL bResult = m_pList->Create("属性表", WS_CHILD | WS_VISIBLE, rc, this, 101);
		CElementDataDoc* doc = GetDocument();
		m_pList->AttachDataObject(doc->m_pProperty);
		ASSERT(bResult);
	}
}

void CElementDataView::UpdateData()
{
	if(m_pList)	
		m_pList->UpdateData(true);
}

void CElementDataView::OnKillFocus(CWnd* pNewWnd) 
{
	CFormView::OnKillFocus(pNewWnd);
	// TODO: Add your message handler code here
}
