#include "DlgCurve.h"
#include "CurvePoint.h"
#include "CodeTemplate.h"

AUI_BEGIN_EVENT_MAP(CDlgCurve, AUIDialog)

AUI_ON_EVENT("lblViewStub", WM_LBUTTONDOWN, OnLButtonDownGraph)
AUI_ON_EVENT("lblViewStub", WM_LBUTTONUP, OnLButtonUpGraph)
AUI_ON_EVENT("lblViewStub", WM_MOUSEMOVE, OnMouseMoveGraph)

AUI_END_EVENT_MAP()


AUI_BEGIN_COMMAND_MAP(CDlgCurve, AUIDialog)

AUI_ON_COMMAND("IDOK", OnCommandIDOK)

AUI_END_COMMAND_MAP()


#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp)) 
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp)) 

#include "Home/CurveView.h"


CDlgCurve::CDlgCurve() :
	m_pCurveView(NULL),
	m_pTempCurvePoint(NULL),
	m_pOutterCurvePoint(NULL)
{
	
}

CDlgCurve::~CDlgCurve()
{
	delete m_pCurveView;
	delete m_pTempCurvePoint;
}

void CDlgCurve::Attach(CCurvePoint *pCurvePoint, bool bFirstHandleDisabled)
{
	SAFE_DELETE(m_pTempCurvePoint);
	m_pTempCurvePoint = new CCurvePoint;
	m_pTempCurvePoint->SetPointValues(pCurvePoint->GetNumPoints(), pCurvePoint->GetPointValues());

	m_pOutterCurvePoint = pCurvePoint;

	m_pCurveView->Attach(m_pTempCurvePoint, bFirstHandleDisabled);
}

bool CDlgCurve::Render()
{
	if (!AUIDialog::Render())
		return false;

	m_pCurveView->SetRect(GetViewRect());
	m_pCurveView->Render();

	return true;
}

void CDlgCurve::OnLButtonDownGraph(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	SetCanMove(false);
	SetCapture(true, x, y);
	m_pCurveView->BeginDrag(x, y);
}

void CDlgCurve::OnLButtonUpGraph(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	
	SetCanMove(true);
	SetCapture(false, x, y);
	m_pCurveView->EndDrag(x, y);
}

void CDlgCurve::OnMouseMoveGraph(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	m_pCurveView->OnDrag(x, y);
}

A3DRECT CDlgCurve::GetViewRect()
{
	AUIObject *pPlaceHolder = GetDlgItem("lblViewStub");
	ASSERT(pPlaceHolder);
	A3DRECT rect = pPlaceHolder->GetRect();

	return rect;
}

bool CDlgCurve::OnInitDialog()
{
	if (!AUIDialog::OnInitDialog())
		return false;

	m_pCurveView = new CCurveView(GetViewRect());
	return true;
}

void CDlgCurve::OnCommandIDOK(const char * szCommand)
{
	m_pOutterCurvePoint->SetPointValues(
		m_pTempCurvePoint->GetNumPoints(),
		m_pTempCurvePoint->GetPointValues());
	Show(false);
}

void CDlgCurve::OnShowDialog()
{
	SetCanMove(false);
}