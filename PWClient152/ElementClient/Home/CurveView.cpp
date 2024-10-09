#include "CurveView.h"
#include <AAssist.h>
#include "CodeTemplate.h"
#include "CurvePoint.h"
#include "../../CFWEditorLib/FWFlatCollector.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include <A3DMacros.h>


#define  HANDLE_WIDTH_EXT	4
#define  CURVE_DRAW_RES		5

CCurveView::CCurveView(A3DRECT rect) :
	m_pCurvePoint(NULL),
	m_Rect(rect),
	m_bOnDrag(false),
	m_nClickHandle(-1),
	m_bFirstHandleDisabled(false)
{
	m_pFlatCollector = new FWFlatCollector;
	m_pFlatCollector->Init(g_pGame->GetA3DEngine());
}

CCurveView::~CCurveView()
{
	A3DRELEASE(m_pFlatCollector);
}

void CCurveView::SetRect(A3DRECT rect)
{
	m_Rect = rect;
}

void CCurveView::Attach(CCurvePoint * pCurvePoint, bool bFirstHandleDisabled)
{
	ASSERT(pCurvePoint);
	ASSERT(pCurvePoint->GetNumPoints() > 0);

	m_pCurvePoint = pCurvePoint;
	m_bFirstHandleDisabled = bFirstHandleDisabled;
}

void CCurveView::Render()
{
	if (!m_pCurvePoint)
		return;

	// draw background
	A3DRECT rect = GetTrueRect();
	m_pFlatCollector->DrawRectangle2D(rect, COLOR_WHITE, true);

	int i = 0;
	// draw grid
	for (i = 0; i < m_pCurvePoint->GetNumPoints(); i++)
	{
		APointF logicPos = GetLogicHandlePos(i);
		APointI pos = Logic2Screen(logicPos.x, logicPos.y);
		m_pFlatCollector->DrawLine(APointI(pos.x, rect.top), APointI(pos.x, rect.bottom));
	}

	// draw curve
	float step = 1.f / rect.Width() * CURVE_DRAW_RES;
	APointF lastPos = GetLogicHandlePos(0);
	for (float x = step; x <= 1; x += step)
	{
		APointF curPos(x, m_pCurvePoint->GetValue(x));

		APointI p1 = Logic2Screen(lastPos.x, lastPos.y);
		APointI p2 = Logic2Screen(curPos.x, curPos.y);
		m_pFlatCollector->DrawLine(p1, p2);
		lastPos = curPos;
	}
	
	
	// draw handle
	for (i = 1; i < m_pCurvePoint->GetNumPoints(); i++)
	{
		m_pFlatCollector->DrawRectangle2D(GetHandleRect(i), COLOR_BLACK, true);
	}
	if (m_bFirstHandleDisabled)
	{
		m_pFlatCollector->DrawRectangle2D(GetHandleRect(0), COLOR_LIGHT_GRAY, true, COLOR_LIGHT_GRAY);
	}
	else
	{
		m_pFlatCollector->DrawRectangle2D(GetHandleRect(0), COLOR_BLACK, true);
	}

	// flush
	m_pFlatCollector->Flush();
}

void CCurveView::BeginDrag(int x, int y)
{
	if (!m_pCurvePoint)
		return;

	m_bOnDrag = true;

	// find clicked handle
	int i(0);
	for (i = 0; i < m_pCurvePoint->GetNumPoints(); i++)
	{
		if (GetHandleRect(i).PtInRect(x, y))
			break;
	}
	m_nClickHandle = i;

	if (0 == m_nClickHandle && m_bFirstHandleDisabled)
		m_nClickHandle = -1;
}

void CCurveView::EndDrag(int x, int y)
{
	if (!m_pCurvePoint)
		return;

	m_bOnDrag = false;
	m_nClickHandle = -1;
}

void CCurveView::OnDrag(int x, int y)
{
	if (!m_pCurvePoint)
		return;

	if (!m_bOnDrag)
		return;

	if (0 <= m_nClickHandle && m_nClickHandle < m_pCurvePoint->GetNumPoints())
	{
		A3DRECT rect = GetTrueRect();
		// note : x have no use here but must have a valid 
		// value for Screen2Logic()
		a_Clamp(x, rect.left, rect.right);
		a_Clamp(y, rect.top, rect.bottom);
		APointF logicPos = Screen2Logic(x, y);
		m_pCurvePoint->GetPointValues()[m_nClickHandle] = logicPos.y;
	}
}

bool CCurveView::HitTest(int x, int y)
{
	return m_Rect.PtInRect(x, y);
}



APointI CCurveView::Logic2Screen(float x, float y)
{
	ASSERT(0 <= x && x <= 1);
	ASSERT(0 <= y && y <= 1);

	APointI output;
	A3DRECT rect = GetTrueRect();
	output.x = static_cast<int>(x * rect.Width()) + rect.left;
	output.y = static_cast<int>(y * rect.Height()) + rect.top;
	return output;
}

APointF CCurveView::Screen2Logic(int x, int y)
{
	A3DRECT rect = GetTrueRect();
	ASSERT(rect.left <= x && x <= rect.right);
	ASSERT(rect.top <= y && y <= rect.bottom);

	APointF output;
	output.x = static_cast<float>(x - rect.left) / rect.Width();
	output.y = static_cast<float>(y - rect.top) / rect.Height();
	return output;
}

APointF CCurveView::GetLogicHandlePos(int index)
{
	ASSERT(m_pCurvePoint);
	ASSERT(0 <= index && index < m_pCurvePoint->GetNumPoints());

	APointF output;
	output.x = 1.f / (m_pCurvePoint->GetNumPoints() - 1) * index;
	output.y = m_pCurvePoint->GetPointValues()[index];
	return output;
}

A3DRECT CCurveView::GetHandleRect(int index)
{
	ASSERT(m_pCurvePoint);
	ASSERT(0 <= index && index < m_pCurvePoint->GetNumPoints());
	APointF logicPos = GetLogicHandlePos(index);
	APointI pos = Logic2Screen(logicPos.x, logicPos.y);

	A3DRECT handleRect;
	handleRect.left = pos.x - HANDLE_WIDTH_EXT;
	handleRect.right = pos.x + HANDLE_WIDTH_EXT;
	handleRect.top = pos.y - HANDLE_WIDTH_EXT;
	handleRect.bottom = pos.y + HANDLE_WIDTH_EXT;

	return handleRect;
}
A3DRECT CCurveView::GetTrueRect()
{
	ASSERT(m_pCurvePoint);
	A3DRECT rect = m_Rect;
	rect.Deflate(HANDLE_WIDTH_EXT, HANDLE_WIDTH_EXT, HANDLE_WIDTH_EXT, HANDLE_WIDTH_EXT);
	return rect;
}

int CCurveView::GetHandleCount()
{
	ASSERT(m_pCurvePoint);
	return m_pCurvePoint->GetNumPoints();
}

const float * CCurveView::GetPoints()
{
	ASSERT(m_pCurvePoint);
	return m_pCurvePoint->GetPointValues();
}
