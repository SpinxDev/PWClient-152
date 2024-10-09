// Polygon.cpp: implementation of the FWGlyphPolygon class.
//
//////////////////////////////////////////////////////////////////////

#include "FWGlyphPolygon.h"
#include "fwalgorithm.h"
#include "CharOutline.h"
#include "CharContour.h"
#include "CharVerticesBuffer.h"
#include "FWArchive.h"
#include "CodeTemplate.h"
#include "FWDialogPolygonMenu.h"
#include <ocidl.h>

using namespace FWAlgorithm;

#define new A_DEBUG_NEW




FW_IMPLEMENT_SERIAL(FWGlyphPolygon, FWGlyphGraphBase)

FWGlyphPolygon::FWGlyphPolygon()
{
}

FWGlyphPolygon::~FWGlyphPolygon()
{

}


void FWGlyphPolygon::ResetHandles(const FWArray<APointI, APointI&> *HandleArray)
{
	m_HandleArray.RemoveAll();
	m_HandleArray.Append(*HandleArray);

	SetAngle(GetAngle());
}

void FWGlyphPolygon::MoveHandleTo(int nHandle, APointI newPoint)
{
	if (GetAngle())
	{
		m_RotatedHandleArray[nHandle - 1] = newPoint;
		m_HandleArray.RemoveAll();
		m_HandleArray.Append(m_RotatedHandleArray);
		SetAngle(0);
	}
	else
	{
		m_HandleArray[nHandle - 1] = newPoint;
		ComputeSizeRect();
	}
}

ARectI FWGlyphPolygon::GetSizeRect() const
{
	return m_SizeRect;
}

void FWGlyphPolygon::SetSizeRect(ARectI rect)
{
	ARectI minRect = ARectI(0, 0, 6, 6) + GetCentralPoint();
	if (rect.Height() < minRect.Height())
		rect.bottom	= rect.top + minRect.Height();
	if (rect.Width() < minRect.Width())
		rect.right = rect.left + minRect.Width();

	/* ========= Algorithm ===========
	1. Variable
		x1,x2 is the left and right value of oldrect
		x1a, x2a is the left and right value of newrect
		x is the old value of a handle
		xa is the new value of a handle
	2. Formula
		(x - x1) / (x2 - x) = (xa - x1a)/(x2a - xa)
	3. Comment
		set z = (x - x1) / (x2 - x)
		so xa = (x * x2a+ x1a)/(1 + z)
	*/

	ARectI old = GetSizeRect();
	int x1 = old.left, x2 = old.right, y1 = old.top, y2 = old.bottom;
	int x1a = rect.left, x2a = rect.right, y1a = rect.top, y2a = rect.bottom;
	int x, y, xa, ya;
	double z;

	for(int i = 0; i < GetHandleCount(); i++)
	{
		if(m_HandleArray[i].x != x2)
		{
			x = m_HandleArray[i].x;
			z = (static_cast<double>(x) - x1) / (x2 - x);
			xa = static_cast<int>((z * x2a+ x1a)/(1 + z));
			m_HandleArray[i].x = xa;
		}
		else
		{
			m_HandleArray[i].x = x2a;
		}

		if(m_HandleArray[i].y != y2)
		{
			y = m_HandleArray[i].y;
			z = (static_cast<double>(y) - y1) / (y2 - y);
			ya = static_cast<int>((z * y2a+ y1a)/(1 + z));
			m_HandleArray[i].y = ya;
		}
		else
		{
			m_HandleArray[i].y = y2a;
		}
	}

	SetAngle(GetAngle());
}

void FWGlyphPolygon::SetAngle(double angle)
{
	FWGlyph::SetAngle(angle);
	
	if (GetAngle())
	{
		ComputeSizeRect();
		POINTF center = {GetCentralPoint().x, GetCentralPoint().y};
		RotatePoint(m_HandleArray, m_RotatedHandleArray, center, -GetAngle());
	}

	ComputeSizeRect();
}

ARectI FWGlyphPolygon::GetTrueRect() const
{
	ARectI rect;
	if (GetAngle())
		rect = m_RotatedSizeRect;
	else
		rect = m_SizeRect;

	rect.Inflate(DEFAULT_HANDLE_RADIUS, DEFAULT_HANDLE_RADIUS);
	
	return rect;
}

void FWGlyphPolygon::SetCentralPoint(APointI point)
{
	APointI old = GetCentralPoint();
	int dx = point.x - old.x;
	int dy = point.y - old.y;
	for(int i = 0; i < GetHandleCount(); i++)
	{
		m_HandleArray[i].x += dx;
		m_HandleArray[i].y += dy;
	}

	SetAngle(GetAngle());
}

int FWGlyphPolygon::GetHandleCount() const
{
	return m_HandleArray.GetSize();
}

APointI FWGlyphPolygon::GetHandle(int nPoint) const
{
	if(GetAngle() == 0)
		return m_HandleArray[nPoint - 1];
	else
		return m_RotatedHandleArray[nPoint - 1];
}

bool FWGlyphPolygon::IsHit(const APointI& point) const
{
	int nHandleCount = GetHandleCount();
	HRGN hRgn;
	POINT *tmp = new POINT[nHandleCount];
	if(GetAngle() == 0)
	{
		for(int i = 0; i < nHandleCount; i++)
		{
			tmp[i].x = m_HandleArray[i].x;
			tmp[i].y = m_HandleArray[i].y;
		}
		hRgn = ::CreatePolygonRgn(tmp, nHandleCount, ALTERNATE);
		delete [] tmp;
	}
	else
	{
		for(int i = 0; i < nHandleCount; i++)
		{
			tmp[i].x = m_RotatedHandleArray[i].x;
			tmp[i].y = m_RotatedHandleArray[i].y;
		}
		hRgn = ::CreatePolygonRgn(tmp, nHandleCount, ALTERNATE);
		delete [] tmp;
	}
	
	if (::PtInRegion(hRgn, point.x, point.y))
		return true;
	APointI pt = APointI(point.x - DEFAULT_HANDLE_RADIUS, point.y);
	if (::PtInRegion(hRgn, pt.x, pt.y))
		return true;
	pt = APointI(point.x + DEFAULT_HANDLE_RADIUS, point.y);
	if (::PtInRegion(hRgn, pt.x, pt.y))
		return true;
	pt = APointI(point.x, point.y - DEFAULT_HANDLE_RADIUS);
	if (::PtInRegion(hRgn, pt.x, pt.y))
		return true;
	pt = APointI(point.x, point.y + DEFAULT_HANDLE_RADIUS);
	if (::PtInRegion(hRgn, pt.x, pt.y))
		return true;

	return false;
}


void FWGlyphPolygon::Serialize(FWArchive &ar)
{
	FWGlyphGraphBase::Serialize(ar);

	if (ar.IsStoring())
	{
		int nCount = m_HandleArray.GetSize();
		ar << (short) nCount;
		for (int i = 0; i < nCount; i++)
		{
			APointI &pt = m_HandleArray[i];
			ar << (short)pt.x << (short)pt.y;
		}
	}
	else
	{
		short tmp = 0;
		int nCount = 0;
		ar >> tmp; nCount = tmp;
		m_HandleArray.SetSize(nCount, 0);
		for (int i = 0; i < nCount; i++)
		{
			APointI &pt = m_HandleArray[i];
			ar >> tmp; pt.x = tmp;
			ar >> tmp; pt.y = tmp;
		}

		SetAngle(GetAngle());
	}
}

CCharOutline * FWGlyphPolygon::CreateOutline() const
{
	CCharOutline *pOutline = new CCharOutline;
	CCharContour *pContour = new CCharContour;
	
	if (GetHandleCount() < 3)
	{
		delete pContour;
		return pOutline;
	}

	CCharVerticesBuffer *pBuffer = pContour->GetCharVerticesBuffer();
	pBuffer->Create(GetHandleCount());
	
	FWArray<APointI, APointI&> * pHandles = NULL;
	if (GetAngle() == 0)
	{
		pHandles = const_cast<FWArray<APointI, APointI&> *>(&m_HandleArray);	
	}
	else
	{
		pHandles = const_cast<FWArray<APointI, APointI&> *>(&m_RotatedHandleArray);
	}
	for (int i = 0; i < GetHandleCount(); i++)
	{
		FWAlgorithm::Point2DoubleArray(pHandles->GetAt(i), pBuffer->m_paVertices[i]);
	}
	
	pOutline->GetCharContoursList()->AddTail(pContour);
	
	return pOutline;
}

void FWGlyphPolygon::ComputeSizeRect()
{
	m_SizeRect = GetMinRect(m_HandleArray);
	m_RotatedSizeRect = GetMinRect(m_RotatedHandleArray);
}

void FWGlyphPolygon::InsertHandleBefore(int nHandle, const APointArray &aryHandle, void *pDataIn)
{
	ASSERT(aryHandle.GetSize() == 1);

	int nIndex = nHandle - 1;

	if (GetAngle())
	{
		m_RotatedHandleArray.InsertAt(nIndex, aryHandle[0]);
		ResetHandles(&m_RotatedHandleArray);
	}
	else
	{
		m_HandleArray.InsertAt(nIndex, aryHandle[0]);
		ComputeSizeRect();
	}
}

void FWGlyphPolygon::RemoveHandle(int nHandle, APointArray *pArrayHandleDeleted, void **ppDataOut)
{
	if (pArrayHandleDeleted)
	{
		pArrayHandleDeleted->SetSize(1, 0);
		pArrayHandleDeleted->SetAt(0, GetHandle(nHandle));
	}

	if (GetAngle())
	{
		m_RotatedHandleArray.RemoveAt(nHandle - 1);
		ResetHandles(&m_RotatedHandleArray);
	}
	else
	{
		m_HandleArray.RemoveAt(nHandle - 1);
		ComputeSizeRect();
	}

}

void FWGlyphPolygon::DuplicateHandle(int nHandle)
{
	APointI pt = GetHandle(nHandle);
	pt.x += DEFAULT_HANDLE_RADIUS * 2; 

	APointArray ary;
	ary.Add(pt);

	InsertHandleBefore(nHandle, ary);
}


const char * FWGlyphPolygon::GetContextMenu()
{
	return TO_STRING(FWDialogPolygonMenu);
}
