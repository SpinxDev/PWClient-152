#include "FWGlyphRectBase.h"
#include "FWAlgorithm.h"

#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNAMIC(FWGlyphRectBase, FWGlyphGraphBase)

bool FWGlyphRectBase::IsHit(const APointI& point) const
{
	if (GetAngle() == 0)
		return m_rect.PtInRect(point);
	else
	{
		HRGN hRgn = ::CreatePolygonRgn(m_pPoint, 4, WINDING);
		return ::PtInRegion(hRgn, point.x, point.y) != 0;
	}
}

int FWGlyphRectBase::GetHandleCount() const
{
	return 8;
}

APointI FWGlyphRectBase::GetHandle(int nPoint) const
{

	APointI handle;
	APointI centre = m_rect.CenterPoint();

	switch (nPoint)
	{
	default:
		ASSERT(false);

	case 1:
		handle.x = m_rect.left;
		handle.y = m_rect.top;
		break;

	case 2:
		handle.x = centre.x;
		handle.y = m_rect.top;
		break;

	case 3:
		handle.x = m_rect.right;
		handle.y = m_rect.top;
		break;

	case 4:
		handle.x = m_rect.right;
		handle.y = centre.y;
		break;

	case 5:
		handle.x = m_rect.right;
		handle.y = m_rect.bottom;
		break;

	case 6:
		handle.x = centre.x;
		handle.y = m_rect.bottom;
		break;

	case 7:
		handle.x = m_rect.left;
		handle.y = m_rect.bottom;
		break;

	case 8:
		handle.x = m_rect.left;
		handle.y = centre.y;
		break;
	}

	if(GetAngle() != 0)
	{
		handle = FWAlgorithm::RotatePoint(handle, GetCentralPointF(), -GetAngle());
	}
	
	return handle;
}


ARectI FWGlyphRectBase::GetSizeRect() const
{
	return m_rect;
}

void FWGlyphRectBase::SetSizeRect(ARectI rect)
{
	if (rect.Height() < GetMinimalRect().Height())
		rect.bottom	= rect.top + GetMinimalRect().Height();
	if (rect.Width() < GetMinimalRect().Width())
		rect.right = rect.left + GetMinimalRect().Width();
	m_rect = rect;
	RecalcPoint();
}


ARectI FWGlyphRectBase::GetTrueRect() const
{
	int nEnlarge;
	if (GetAngle() == 0)
	{
		nEnlarge = DEFAULT_HANDLE_RADIUS;
		ARectI rect = m_rect;
		rect.Inflate(nEnlarge, nEnlarge);
		return rect;
	}
	else
	{
		nEnlarge = DEFAULT_HANDLE_RADIUS * 2;
		ARectI rect = FWAlgorithm::GetMinRect(m_pPoint, 4);
		rect.Inflate(nEnlarge, nEnlarge);
		return rect;
	}
}

ARectI FWGlyphRectBase::GetHandleRect(int nIndex) const
{
	APointI upleftPoint = GetHandle(nIndex);	
	APointI bottomrightPoint = upleftPoint;
	upleftPoint.Offset(-DEFAULT_HANDLE_RADIUS, -DEFAULT_HANDLE_RADIUS);
	bottomrightPoint.Offset(DEFAULT_HANDLE_RADIUS, DEFAULT_HANDLE_RADIUS);
	return ARectI(upleftPoint.x, upleftPoint.y, bottomrightPoint.x, bottomrightPoint.y);
}

void FWGlyphRectBase::MoveHandleTo(int nHandle, APointI newPoint)
{
	if (GetAngle() != 0)
	{
		// if angle != 0, central point should not be moved
		// so use this value to store the old central point
		POINTF oldCenter = GetCentralPointF();
		newPoint = FWAlgorithm::RotatePoint(newPoint, oldCenter, GetAngle());

		double minHeight = GetMinimalRect().Height() / 2;
		double minWidth = GetMinimalRect().Width() / 2;
		ARectI oldRect = GetSizeRect();
	
		switch(nHandle)
		{
		default:
			ASSERT(false);

		case 1:
			m_rect.left = newPoint.x;
			m_rect.top = newPoint.y;
			if ( oldCenter.x - m_rect.left < minWidth )
				m_rect.left = (long)(oldCenter.x - minWidth);
			if ( oldCenter.y - m_rect.top < minHeight )
				m_rect.top = (long)(oldCenter.y - minHeight);
			m_rect.right += oldRect.left - m_rect.left;
			m_rect.bottom += oldRect.top - m_rect.top;
			break;

		case 2:
			m_rect.top = newPoint.y;
			if ( oldCenter.y - m_rect.top < minHeight )
				m_rect.top = (long)(oldCenter.y - minHeight);
			m_rect.bottom += oldRect.top - m_rect.top;
			break;

		case 3:
			m_rect.right = newPoint.x;
			m_rect.top = newPoint.y;
			if ( oldCenter.y - m_rect.top < minHeight )
				m_rect.top = (long)(oldCenter.y - minHeight);
			if ( m_rect.right - oldCenter.x < minWidth )
				m_rect.right = (long)(oldCenter.x + minWidth);
			m_rect.bottom += oldRect.top - m_rect.top;
			m_rect.left += oldRect.right - m_rect.right;
			break;

		case 4:
			m_rect.right = newPoint.x;
			if ( m_rect.right - oldCenter.x < minWidth )
				m_rect.right = (long)(oldCenter.x + minWidth);
			m_rect.left += oldRect.right - m_rect.right;
			break;

		case 5:
			m_rect.right = newPoint.x;
			m_rect.bottom = newPoint.y;
			if ( m_rect.right - oldCenter.x < minWidth )
				m_rect.right = (long)(oldCenter.x + minWidth);
			if ( m_rect.bottom - oldCenter.y < minHeight )
				m_rect.bottom = (long)(oldCenter.y + minHeight);
			m_rect.left += oldRect.right - m_rect.right;
			m_rect.top += oldRect.bottom - m_rect.bottom;
			break;

		case 6:
			m_rect.bottom = newPoint.y;
			if ( m_rect.bottom - oldCenter.y < minHeight )
				m_rect.bottom = (long)(oldCenter.y + minHeight);
			m_rect.top += oldRect.bottom - m_rect.bottom;
			break;

		case 7:
			m_rect.left = newPoint.x;
			m_rect.bottom = newPoint.y;
			if ( oldCenter.x - m_rect.left < minWidth )
				m_rect.left = (long)(oldCenter.x - minWidth);
			if ( m_rect.bottom - oldCenter.y < minHeight )
				m_rect.bottom = (long)(oldCenter.y + minHeight);
			m_rect.top += oldRect.bottom - m_rect.bottom;
			m_rect.right += oldRect.left - m_rect.left;
			break;

		case 8:
			m_rect.left = newPoint.x;
			if ( oldCenter.x - m_rect.left < minWidth )
				m_rect.left = (long)(oldCenter.x - minWidth);
			m_rect.right += oldRect.left - m_rect.left;
			break;
		}
	}
	else
	{
		switch(nHandle)
		{
		default:
			ASSERT(false);

		case 1:
			m_rect.left = newPoint.x;
			m_rect.top = newPoint.y;
			if (m_rect.Height() < GetMinimalRect().Height())
				m_rect.top = m_rect.bottom - GetMinimalRect().Height();
			if (m_rect.Width() < GetMinimalRect().Width())
				m_rect.left = m_rect.right - GetMinimalRect().Width();
			break;

		case 2:
			m_rect.top = newPoint.y;
			if (m_rect.Height() < GetMinimalRect().Height())
				m_rect.top = m_rect.bottom - GetMinimalRect().Height();
			break;

		case 3:
			m_rect.right = newPoint.x;
			m_rect.top = newPoint.y;
			if (m_rect.Height() < GetMinimalRect().Height())
				m_rect.top = m_rect.bottom - GetMinimalRect().Height();
			if (m_rect.Width() < GetMinimalRect().Width())
				m_rect.right = m_rect.left + GetMinimalRect().Width();
			break;

		case 4:
			m_rect.right = newPoint.x;
			if (m_rect.Width() < GetMinimalRect().Width())
				m_rect.right= m_rect.left + GetMinimalRect().Width();
			break;

		case 5:
			m_rect.right = newPoint.x;
			m_rect.bottom = newPoint.y;
			if (m_rect.Height() < GetMinimalRect().Height())
				m_rect.bottom = m_rect.top + GetMinimalRect().Height();
			if (m_rect.Width() < GetMinimalRect().Width())
				m_rect.right = m_rect.left + GetMinimalRect().Width();
			break;

		case 6:
			m_rect.bottom = newPoint.y;
			if (m_rect.Height() < GetMinimalRect().Height())
				m_rect.bottom = m_rect.top + GetMinimalRect().Height();
			break;

		case 7:
			m_rect.left = newPoint.x;
			m_rect.bottom = newPoint.y;
			if (m_rect.Height() < GetMinimalRect().Height())
				m_rect.bottom = m_rect.top + GetMinimalRect().Height();
			if (m_rect.Width() < GetMinimalRect().Width())
				m_rect.left = m_rect.right - GetMinimalRect().Width();
			break;

		case 8:
			m_rect.left = newPoint.x;
			if (m_rect.Width() < GetMinimalRect().Width())
				m_rect.left = m_rect.right - GetMinimalRect().Width();
			break;
		}
	}

	RecalcPoint();
}


void FWGlyphRectBase::Serialize(FWArchive &ar)
{
	FWGlyphGraphBase::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << (short) m_rect.left << (short) m_rect.top << (short) m_rect.right << (short) m_rect.bottom;
	}
	else
	{
		short tmp = 0;
		ar >> tmp; m_rect.left = tmp;
		ar >> tmp; m_rect.top = tmp;
		ar >> tmp; m_rect.right = tmp;
		ar >> tmp; m_rect.bottom = tmp;
		RecalcPoint();
	}
}

FWGlyphRectBase::FWGlyphRectBase() :
	m_pPoint(NULL), 
	m_rect(0, 0, 0, 0)
{
}

FWGlyphRectBase::~FWGlyphRectBase()
{
	delete []m_pPoint;
}

void FWGlyphRectBase::SetAngle(double angle)
{
	FWGlyph::SetAngle(angle);
	RecalcPoint();
}

void FWGlyphRectBase::RecalcPoint()
{
	if (GetAngle() == 0)
	{
		delete []m_pPoint;
		m_pPoint = NULL;
	}
	else
	{
		if(!m_pPoint)
			m_pPoint = new POINT[4];

		POINTF centre = GetCentralPointF();
		m_pPoint[0].x = m_rect.left; m_pPoint[0].y = m_rect.top;
		m_pPoint[1].x = m_rect.right; m_pPoint[1].y = m_rect.top;
		FWAlgorithm::RotatePoint(m_pPoint, m_pPoint, 2, centre, -GetAngle());

		m_pPoint[2].x = (long)(centre.x + (centre.x - m_pPoint[0].x));
		m_pPoint[2].y = (long)(centre.y + (centre.y - m_pPoint[0].y));
		m_pPoint[3].x = (long)(centre.x + (centre.x - m_pPoint[1].x));		
		m_pPoint[3].y = (long)(centre.y + (centre.y - m_pPoint[1].y));
	}
}
