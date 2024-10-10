// Ellipse.cpp: implementation of the FWGlyphEllipse class.
//
//////////////////////////////////////////////////////////////////////

#include "FWGlyphEllipse.h"
#include "CharOutline.h"
#include "CharContour.h"
#include "CharVerticesBuffer.h"
#include "FWAlgorithm.h"
#include "FWArchive.h"
#include "Global.h"
#include <A3DMacros.h>

using namespace FWAlgorithm;

#define new A_DEBUG_NEW


FW_IMPLEMENT_SERIAL(FWGlyphEllipse, FWGlyphRectBase)



FWGlyphEllipse::FWGlyphEllipse()
{
}

FWGlyphEllipse::~FWGlyphEllipse()
{

}

bool FWGlyphEllipse::IsHit(const APointI &point) const
{
	HRGN hRgn = NULL;
 	//return m_rect.PtInRect(point);
	if (GetAngle() == 0)
	{
		hRgn = ::CreateEllipticRgn(
			m_rect.left, 
			m_rect.top, 
			m_rect.right, 
			m_rect.bottom);
	}
	else
	{
		hRgn = ::CreatePolygonRgn((POINT*)m_rotatedList, POINT_COUNT, WINDING);
	}
	return ::PtInRegion(hRgn, point.x, point.y) != 0;
}


void FWGlyphEllipse::Reform()
{
	//the long axis of ellipse
	int rl = m_rect.Width() / 2;
	
	//the short axis of ellipse
	int rs = m_rect.Height() / 2;
	
	//center of ellipse
	APointI center = GetCentralPoint();
	int xc = center.x;
	int yc = center.y;

	int i(0);
	for(i = 0; i < POINT_COUNT / 4; i++)
	{
		m_ptList[i].x = (int)((double)rl * cos((double)(A3D_PI / (POINT_COUNT / 2) * i)) + (double)center.x);
		m_ptList[i].y = (int)((double)rs * sin((double)(A3D_PI / (POINT_COUNT / 2) * i)) + (double)center.y);
	}
	for(i = POINT_COUNT / 4; i < POINT_COUNT / 2; i++)
	{
		m_ptList[i].x = center.x * 2 - m_ptList[POINT_COUNT / 2 - 1 - i].x;
		m_ptList[i].y = m_ptList[POINT_COUNT / 2 -1 - i].y;
	}
	for(i = POINT_COUNT / 2; i < POINT_COUNT; i++)
	{
		m_ptList[i].x = xc * 2 - m_ptList[i - POINT_COUNT / 2].x;
		m_ptList[i].y = yc * 2 - m_ptList[i - POINT_COUNT / 2].y;
	}

	// if angle != 0, recalculate the rotated point of the ellipse
	if (GetAngle())
		Rotate(GetAngle());

	return;			
}

void FWGlyphEllipse::Rotate(double fDegree)
{
	POINTF centerF = GetCentralPointF();
	APointI center = GetCentralPoint();

	double radian = (double)(-1) * fDegree;

	RotatePoint(m_ptList, m_rotatedList, POINT_COUNT, centerF, radian);
	
	for(int i = POINT_COUNT / 2; i < POINT_COUNT; i++)
	{
		m_rotatedList[i].x = center.x * 2 - m_rotatedList[i - POINT_COUNT / 2].x;
		m_rotatedList[i].y = center.y * 2 - m_rotatedList[i - POINT_COUNT / 2].y;
	}
}

void FWGlyphEllipse::SetAngle(double angle)
{
	FWGlyphRectBase::SetAngle(angle);
	if (GetAngle())
		Rotate(angle);
}

void FWGlyphEllipse::SetSizeRect(ARectI rect)
{
	FWGlyphRectBase::SetSizeRect(rect);

	if (rect.Height() < GetMinimalRect().Height())
		rect.bottom	= rect.top + GetMinimalRect().Height();
	if (rect.Width() < GetMinimalRect().Width())
		rect.right = rect.left + GetMinimalRect().Width();
	m_rect = rect;
	Reform();
}

void FWGlyphEllipse::MoveHandleTo(int nHandle, APointI newPoint)
{
	FWGlyphRectBase::MoveHandleTo(nHandle, newPoint);
	Reform();
}

CCharOutline * FWGlyphEllipse::CreateOutline() const
{
	CCharOutline *pOutline = new CCharOutline;
	CCharContour *pContour = new CCharContour;
	
	CCharVerticesBuffer *pBuffer = pContour->GetCharVerticesBuffer();
	pBuffer->Create(POINT_COUNT);
	
	const POINT * pts = GetAngle() ? m_rotatedList : m_ptList;
	for (int i = 0; i < POINT_COUNT; i++)
	{
		Point2DoubleArray(pts[i], pBuffer->m_paVertices[i]);
	}

	pOutline->GetCharContoursList()->AddTail(pContour);

	pOutline->Simplify(.9f);

	return pOutline;
}


void FWGlyphEllipse::Serialize(FWArchive &ar)
{
	FWGlyphRectBase::Serialize(ar);

	if (ar.IsStoring())
	{
	}
	else
	{
		Reform();
	}
}
