// Bezier.cpp: implementation of the FWGlyphBezier class.
//
//////////////////////////////////////////////////////////////////////

#include "FWGlyphBezier.h"
#include "FWAlgorithm.h"
#include "CharContour.h"
#include "CharCurve.h"
#include "CharOutline.h"
#include "FWTextMesh.h"
#include "CodeTemplate.h"
#include "FWArchive.h"
#include <ocidl.h>

#ifdef _FW_EDITOR
	#include "FWFlatCollector.h"
	#include "FWView.h"
#endif 

using namespace FWAlgorithm;

#define new A_DEBUG_NEW


#pragma warning (disable : 4244)

FW_IMPLEMENT_SERIAL(FWGlyphBezier, FWGlyphGraphBase)

FWGlyphBezier::FWGlyphBezier() :
	m_bModifyBrother(true)
{
}

FWGlyphBezier::~FWGlyphBezier()
{

}

CCharOutline * FWGlyphBezier::CreateOutline() const
{
	CCharContour *pContour = new CCharContour;
	CCharOutline *pOutline = new CCharOutline;

	// need four groups at least, or we can't form a closed curve
	if (GetHandleCount() < 6)
	{
		delete pContour;
		return pOutline;
	}
	
	// C(3, 0), C(3, 1), C(3, 2), C(3, 3)
	// pre-compute them here for later use
	static int n = 3;
	static int combinations[4] = 
	{
		1, 3, 3, 1
	};

	// four control points
	APointI pt[4];
	// destination to save each bezier curve
	APtrList<CCharCurve *> lstCurve;
	// process each segment
	for (int i = 2; i < GetHandleCount(); i += 3)
	{
		CCharCurve *pCurve = new CCharCurve;

		if (i == GetHandleCount() - 1)
		{
			// first group used as the last group
			pt[0] = GetHandle(i);
			pt[1] = GetHandle(i + 1);
			pt[2] = GetHandle(1);
			pt[3] = GetHandle(2);
		}
		else
		{
			// normal situation, read next group
			for (int j = 0; j < 4; j++)
				pt[j] = GetHandle(i + j);
		}

		// how many points should we compute for bezier
		// we determine this by using the lenght between 
		// two adjacent main handles
		const int step = (int)GetLengthWithTwoPoint(pt[0], pt[3]);
		if (step == 0)
		{
			// too close the two adjacent main handles are
			// the bezier is just a point
			pCurve->GetCharVerticesBuffer()->Create(1);
			Point2DoubleArray(pt[0], pCurve->GetCharVerticesBuffer()->m_paVertices[0]);
		}
		else
		{
			// reserve space
			pCurve->GetCharVerticesBuffer()->Create(step);

			// do compute
			// formula :
			// p(t) = Sum(B(t) * P(i), i = 0 -> n)
			// B(t) = C(n, i) * t^i * (1 - t) ^ (n - i)
			float t = 0;
			for (int k = 0; k < step; k++, t += 1 / float(step + 1))
			{
				// compute each p(t)
				POINTF pf = {0.f, 0.f};
				for (int i = 0; i <= n; i++)
				{
					// compute C(n, i)
					float b = float(combinations[i]);
					
					// compute (t^i * (1 - t) ^ (n - i))
					// following codes is equal to pow(t, i) * pow(1-t, n-i)
					// current style is just for precise
					if (i < n - i)
					{
						int c(0);
						for (c = 0; c < i; c++)
						{
							b *= t;
							b *= (1 - t);
						}
						for (; c < n - i; c++)
							b *= 1 - t;
					}
					else
					{
						int c(0);
						for (c = 0; c < n - i; c++)
						{
							b *= t;
							b *= (1 - t);
						}
						for (; c < i; c++)
							b *= t;
					}

					// Sum
					pf.x += pt[i].x * b;
					pf.y += pt[i].y * b;
				} // end for (int i = 0; i <= n; i++)

				// save p(t)
				pCurve->GetCharVerticesBuffer()->m_paVertices[k][0] = pf.x;
				pCurve->GetCharVerticesBuffer()->m_paVertices[k][1] = pf.y;
			} // end for (int k = 0; k < step; k++, t += 1 / float(step + 1))
		} // end if
		lstCurve.AddTail(pCurve);
	} // end for (int i = 2; i < GetHandleCount(); i += 3)

	// connect segment to a full closed curve
	pContour->BuildVerticesBufferFromCurveList(&lstCurve);

	// add to outline set
	pOutline->GetCharContoursList()->AddTail(pContour);

	// simplify for efficiency
	pOutline->Simplify(.99f);

	// do clean
	ALISTPOSITION pos = lstCurve.GetHeadPosition();
	while (pos)
	{
		CCharCurve *p = lstCurve.GetNext(pos);
		delete p;
	}

	// return result
	return pOutline;
}


void FWGlyphBezier::MoveHandleTo(int nHandle, APointI newPoint)
{
	// set glyph to unrotated
	if (GetAngle())
	{
		CopyPointArray(m_aryHandle, m_aryHandleRotated);
		SetAngle(0);
	}

	bool bModifyBrother = false;

	int nMode = GetMode(nHandle);
	
	int nMainIndex = ConvertToMainIndex(nHandle);
	int nPreMainIndex = nMainIndex - 3;
	if (nPreMainIndex < 0) nPreMainIndex = GetHandleCount() - 1;
	int nNextMainIndex = nMainIndex + 3;
	if (nNextMainIndex > GetHandleCount()) nNextMainIndex = 2;

	if (nMode == MODE_SHARP)
	{
		// sharp mode
		// non-main handle will be automatically adjusted
		// algorithm : 
		// 1. we connect two adjacent main handls (p0 and p4) with a beeline segment
		// 2. there are two non-main handle( p1 and p2 ) between p0, p4. 
		// place p1, p2 to cut line (p0, p4) to three shorter beeline segment
		// with equal length

		if (nHandle == nMainIndex)
			SetHandle(nMainIndex, newPoint);
		APointI pointMain = GetHandle(nMainIndex);
		if (GetHandleCount() >= 6)
		{
			int x, y;

			// adjust non-main handle of current group
			x = pointMain.x + (GetHandle(nPreMainIndex).x - pointMain.x) / 3;
			y = pointMain.y + (GetHandle(nPreMainIndex).y - pointMain.y) / 3;
			SetHandle(nMainIndex - 1, APointI(x, y));

			x = pointMain.x + (GetHandle(nNextMainIndex).x - pointMain.x) / 3;
			y = pointMain.y + (GetHandle(nNextMainIndex).y - pointMain.y) / 3;
			SetHandle(nMainIndex + 1, APointI(x, y));
			
			// adjust the adjacent group
			// in fact , only the non-main handle adjacent to current
			// group will be changed
			bModifyBrother = true;
		}
	}
	else if (nMode == MODE_SMOOTH)
	{
		// mode smooth
		// non-main handle will be automatically adjusted
		// algorithm : 
		// we need at least 3 group, called g0, g1, g2, where g1 is 
		// the current group, and the handls in the group are called
		// p00, p01, p02, p10, p11, p12, p20, p21, p22, where p01, p11, p21
		// is the main handle
		// 1. connect p01, p21 with a beeline(l02)
		// 2. make a parallel (l1) of l02 through p11
		// 3. create a segment in l1. make its length equal to 1/3 of length
		// of l02, using p11 as its center point. then p10, p12 should at the 
		// two end points

		if (nHandle == nMainIndex)
			SetHandle(nMainIndex, newPoint);
		APointI pointMain = GetHandle(nMainIndex);
		if (GetHandleCount() >= 9) // a corner need at least 3 groups of handle
		{
			APointI pointPre = GetHandle(nPreMainIndex);
			APointI pointNext = GetHandle(nNextMainIndex);

			A3DVECTOR3 v((float)pointPre.x - (float)pointNext.x, (float)pointPre.y - (float)pointNext.y, 0.f);
			v.x /= 6;
			v.y /= 6;
			SetHandle(nMainIndex - 1, APointI(pointMain.x + v.x, pointMain.y + v.y));
			
			v.x *= -1;
			v.y *= -1;
			SetHandle(nMainIndex + 1, APointI(pointMain.x + v.x, pointMain.y + v.y));

			// adjust the adjacent group
			// in fact , only the non-main handles will be changed
			bModifyBrother = true;
		}
	}
	else if (nMode == MODE_BEZIER)
	{
		// mode bezier
		// 1) when main handle is moved, the non-main handles of the
		// the same group will be moved for the same offset
		// 2) when non-main handle is moved
		// main handle keep unmove, but the other non-main handle will 
		// be automatically adjusted
		// algorithm : 
		// if the moved non-main handle is p0, main handle is p1, the
		// other non-main handle is p2, 
		//     1. before move p0, record the proportion between length of 
		//     (p0, p1) and length of (p1, p2), we call it factor
		//     2. move p0
		//     3. make a beeline through p0, p1
		//     4. place p2 in the beeline (p0, p1) at opposite side of p0, 
		//     using p1 as reference point. p2 should be at a position that 
		//     can make the factor remain unchanged.

		if (nHandle == nMainIndex)
		{
			APointI offset = newPoint - GetHandle(nHandle);
			for (int i = nHandle - 1; i <= nHandle + 1; i++)
				m_aryHandle[i - 1] += offset;
		}
		else
		{
			int nIndex1, nIndex2;
			nIndex1 = nHandle;
			if (nHandle == nMainIndex - 1)
				nIndex2 = nMainIndex + 1;
			else
				nIndex2 = nMainIndex - 1;
			
			APointI pt1 = m_PointMoveHandleBak1; // old position 
			APointI pt2 = m_PointMoveHandleBak2; // old position
			APointI pointMain = GetHandle(nMainIndex);
			APointI pt1_ = newPoint; // new position the handle pt1 will move to

			if (pt1 == pointMain)
			{
				m_PointMoveHandleBak1 = pt1_;
				SetHandle(nIndex1, pt1_);
			}
			else if (pt2 == pointMain)
			{
				SetHandle(nIndex1, pt1_);
			}
			else if (pt1_ == pointMain)
			{
				SetHandle(nIndex1, pt1_);
			}
			else
			{
				// opposite point of pt1 using pointMain as center
				APointI pt1_op;
				pt1_op.x = pointMain.x - (pt1_.x - pointMain.x);
				pt1_op.y = pointMain.y - (pt1_.y - pointMain.y);
				
				// the angle from pt2 to pt1_op
				A3DVECTOR3 vpt2(pt2.x - pointMain.x, pt2.y - pointMain.y, 0.f);
				A3DVECTOR3 vpt1_op(pt1_op.x - pointMain.x, pt1_op.y - pointMain.y, 0.f);
				float angle = acos(DotProduct(vpt2, vpt1_op) / vpt1_op.Magnitude() / vpt2.Magnitude());
				if (CrossProduct(vpt2, vpt1_op).z < 0)	angle *= -1;
				
				// adjust length of |pt2 -> center|
				APointI pt2_ = pt2;
				float a = GetLengthWithTwoPoint(pt1, pointMain);
				float a_ = GetLengthWithTwoPoint(pt1_, pointMain);
				float factor = a_ / a;
				pt2_.x = pointMain.x + (pt2_.x - pointMain.x) * factor;
				pt2_.y = pointMain.y + (pt2_.y - pointMain.y) * factor;
				
				// rotate to pt2 to final position
				POINTF center = {pointMain.x, pointMain.y};
				pt2_ = RotatePoint(pt2_, center, angle);
				
				// commit change
				SetHandle(nIndex1, pt1_);
				SetHandle(nIndex2, pt2_);
			}
		}

	}
	else if (nMode == MODE_BEZIER_CORNER)
	{
		// bezier corner
		// user can move each handle independently
		// progam make no disturbance to it
		SetHandle(nHandle, newPoint);
		if (nHandle == nMainIndex)
			bModifyBrother = true;
	}

	if (bModifyBrother && m_bModifyBrother)
	{
		// stop the modify-brother operation to spread
		m_bModifyBrother = false;

		// modify brother
		MoveHandleTo(nPreMainIndex, GetHandle(nPreMainIndex));
		MoveHandleTo(nNextMainIndex, GetHandle(nNextMainIndex));
		
		// reset to default for next move handle call
		m_bModifyBrother = true;
	}

	ComputeSizeRect();
}

ARectI FWGlyphBezier::GetSizeRect() const
{
	return m_SizeRect;
}

void FWGlyphBezier::SetSizeRect(ARectI rect)
{
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
	ARectI minRect = ARectI(0, 0, 6, 6) + GetCentralPoint();
	if (rect.Height() < minRect.Height())
		rect.bottom	= rect.top + minRect.Height();
	if (rect.Width() < minRect.Width())
		rect.right = rect.left + minRect.Width();
	
	ARectI old = GetSizeRect();
	int x1 = old.left, x2 = old.right, y1 = old.top, y2 = old.bottom;
	int x1a = rect.left, x2a = rect.right, y1a = rect.top, y2a = rect.bottom;
	int x, y, xa, ya;
	double z;
	
	for(int i = 0; i < GetHandleCount(); i++)
	{
		APointI &pt = m_aryHandle[i];
		if(pt.x != x2)
		{
			x = pt.x;
			z = (static_cast<double>(x) - x1) / (x2 - x);
			xa = static_cast<int>((z * x2a+ x1a)/(1 + z));
			pt.x = xa;
		}
		else
		{
			pt.x = x2a;
		}
		
		if(pt.y != y2)
		{
			y = pt.y;
			z = (static_cast<double>(y) - y1) / (y2 - y);
			ya = static_cast<int>((z * y2a+ y1a)/(1 + z));
			pt.y = ya;
		}
		else
		{
			pt.y = y2a;
		}
	}
	
	SetAngle(GetAngle());
	ComputeSizeRect();
}

ARectI FWGlyphBezier::GetTrueRect() const
{
	if (GetAngle())
	{
		return m_SizeRectRotated;
	}
	else
	{
		return m_SizeRect;
	}
}


ARectI FWGlyphBezier::GetHandleRect(int nIndex) const
{
	int nMainIndex = ConvertToMainIndex(nIndex);
	if (IsMainHandle(nIndex))
	{
		APointI point = GetHandle(nIndex);
		int nHandleRadius = DEFAULT_HANDLE_RADIUS;
		return ARectI(point.x - nHandleRadius, point.y - nHandleRadius, 
			point.x + nHandleRadius, point.y + nHandleRadius);
	}
	else
	{
		if (GetMode(nIndex) == MODE_BEZIER || GetMode(nIndex) == MODE_BEZIER_CORNER)
		{
			// non-main hanlde is visible
			APointI point = GetHandle(nIndex);
			int nHandleRadius = DEFAULT_HANDLE_RADIUS;
			return ARectI(point.x - nHandleRadius, point.y - nHandleRadius, 
				point.x + nHandleRadius, point.y + nHandleRadius);
		}
		else
		{
			// non-main hanlde is unvisible
			return ARectI(-1, -1, -1, -1);
		}

	}
}

int FWGlyphBezier::GetHandleCount() const
{
	if (GetAngle())
		return m_aryHandleRotated.GetSize();
	else
		return m_aryHandle.GetSize();
}

APointI FWGlyphBezier::GetHandle(int nPoint) const
{
	if (GetAngle())
		return m_aryHandleRotated[nPoint - 1];
	else
		return m_aryHandle[nPoint - 1];
}


bool FWGlyphBezier::IsHit(const APointI& point) const
{
	const FWPointArray *pHandles = GetAngle() ? &m_aryHandleRotated : &m_aryHandle;

	POINT *pts = new POINT[pHandles->GetSize()];
	for (int i = 0; i < pHandles->GetSize(); i++)
	{
		pts[i].x = (*pHandles)[i].x;
		pts[i].y = (*pHandles)[i].y;
	}
	HRGN hRgn = ::CreatePolygonRgn(pts, pHandles->GetSize(), ALTERNATE);
	SAFE_DELETE(pts);
	

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

void FWGlyphBezier::ComputeSizeRect()
{
	if (GetHandleCount() == 0)
	{
		m_SizeRect.Clear();
		m_SizeRectRotated.Clear();
		return;
	}

	int delta = DEFAULT_HANDLE_RADIUS * 2 + 1;

	m_SizeRect = GetMinRect(m_aryHandle);
	m_SizeRect.Inflate(delta, delta, delta, delta);;

	if (GetAngle())
	{
		m_SizeRectRotated = GetMinRect(m_aryHandleRotated);	
		m_SizeRectRotated.Inflate(delta, delta, delta, delta);;
	}
}

bool FWGlyphBezier::IsMainHandle(int nIndex) const
{
	return ((nIndex - 1) % 3 == 1);
}

void FWGlyphBezier::SetAngle(double angle)
{
	FWGlyph::SetAngle(angle);

	if (angle == 0)
		return;

	APointI tmp = GetCentralPoint();
	POINTF center; center.x = tmp.x; center.y = tmp.y;
	RotatePoint(m_aryHandle, m_aryHandleRotated, center, -angle);
	ComputeSizeRect();
}

void FWGlyphBezier::StartMoveHandleInState(int nHandle)
{
	if (IsMainHandle(nHandle)) return;

	if (GetAngle())
	{
		CopyPointArray(m_aryHandle, m_aryHandleRotated);
		SetAngle(0);
	}

	APointI *p1, *p2;
	p1 = &m_aryHandle[nHandle - 1];
	if (nHandle % 3 == 1)
	{
		p2 = &m_aryHandle[nHandle + 1];
	}
	else
	{
		p2 = &m_aryHandle[nHandle - 3];
	}
	
	m_PointMoveHandleBak1 = *p1;
	m_PointMoveHandleBak2 = *p2;
}

void FWGlyphBezier::EndMoveHandleInState(int nHandle)
{
}

void FWGlyphBezier::InitModeArray(int nMode)
{
	int nCount = GetHandleCount();
	if (m_aryMode.GetSize() != nCount)
		m_aryMode.SetSize(nCount, 10);
	for (int i = 0; i < nCount; i++)
		m_aryMode[i] = nMode;
}

int FWGlyphBezier::ConvertToModeIndex(int nHandle) const
{
	return (nHandle - 1) / 3;
}

int FWGlyphBezier::GetMode(int nHandle) const
{
	return m_aryMode[ConvertToModeIndex(nHandle)];
}

void FWGlyphBezier::SetMode(int nHandle, int nMode)
{
	int nModeIndex = ConvertToModeIndex(nHandle);
	m_aryMode[nModeIndex] = nMode;
	int nPreHandle = nModeIndex * 3 + 1;
	StartMoveHandleInState(nPreHandle);
	MoveHandleTo(nPreHandle, GetHandle(nPreHandle));
}

void FWGlyphBezier::SetAllMode(int nMode)
{
	ASSERT(m_aryMode.GetSize() == GetHandleCount());

	for (int i = 1; i <= GetHandleCount(); i += 3)
	{
		SetMode(i, nMode);
	}
}

void FWGlyphBezier::SetCentralPoint(APointI point)
{
	APointI old = GetCentralPoint();
	APointI offset = point - old;
	for(int i = 0; i < GetHandleCount(); i++)
	{
		m_aryHandle[i] += offset;
	}
	SetAngle(GetAngle());
	ComputeSizeRect();
}

void FWGlyphBezier::SetHandle(int nHandle, APointI point)
{
	m_aryHandle[nHandle - 1] = point;
}

int FWGlyphBezier::ConvertToMainIndex(int nHandle) const
{
	return (nHandle - 1) / 3 * 3 + 2;
}

int FWGlyphBezier::GetHitHandle(APointI point) const
{
	for (int i = 1; i < GetHandleCount(); i += 3)
	{
		if (GetHandleRect(i).PtInRect(point))
			return i;
		else if (GetHandleRect(i + 2).PtInRect(point))
			return i + 2;
		else if (GetHandleRect(i + 1).PtInRect(point))
			return i + 1;
	}
	return 0;
}

void FWGlyphBezier::Serialize(FWArchive &ar)
{
	if (GetAngle())
	{
		CopyPointArray(m_aryHandle, m_aryHandleRotated);
		SetAngle(0);
	}

	FWGlyphGraphBase::Serialize(ar);


	if (ar.IsStoring())
	{
		int nCount = m_aryHandle.GetSize();
		ar << (short) nCount;
		int i(0);
		for (i = 0; i < nCount; i++)
		{
			APointI &pt = m_aryHandle[i];
			ar << (short) pt.x << (short) pt.y;
		}

		nCount = m_aryMode.GetSize();
		ar << (short) nCount;
		for (i = 0; i < nCount; i++)
			ar << (short) m_aryMode[i];
	}
	else
	{
		short tmp = 0;
		int nCount = 0;
		ar >> tmp; nCount = tmp;
		m_aryHandle.SetSize(nCount, 10);
		int i(0);
		for (i = 0; i < nCount; i++)
		{
			APointI &pt = m_aryHandle[i];
			ar >> tmp; pt.x = tmp;
			ar >> tmp; pt.y = tmp;
		}

		ar >> tmp; nCount = tmp;
		m_aryMode.SetSize(nCount, 10);
		for (i = 0; i < nCount; i++)
		{
			ar >> tmp; 
			if (tmp != MODE_BEZIER && tmp != MODE_BEZIER_CORNER &&
				tmp != MODE_SHARP && tmp != MODE_SMOOTH)
				tmp = MODE_SMOOTH;
			m_aryMode[i] = tmp;
		}

		// build other member
		ComputeSizeRect();
		if (GetAngle())
			SetAngle(GetAngle());
	}
}

void FWGlyphBezier::RemoveHandle(int nHandle, APointArray *pArrayHandleDeleted, void **ppDataOut)
{
	ASSERT(GetHandleCount() >= 12);

	FWPointArray *pHandleArray = GetAngle() ? &m_aryHandleRotated : &m_aryHandle;

	int nIndex = ConvertToMainIndex(nHandle) - 1 - 1;
	
	if (pArrayHandleDeleted)
	{
		pArrayHandleDeleted->SetSize(3, 10);
		pArrayHandleDeleted->SetAt(0, (*pHandleArray)[nIndex]);
		pArrayHandleDeleted->SetAt(1, (*pHandleArray)[nIndex + 1]);
		pArrayHandleDeleted->SetAt(2, (*pHandleArray)[nIndex + 2]);
	}
	if (ppDataOut)
	{
		int *p = new int;
		*p = m_aryMode[ConvertToModeIndex(nHandle)];
		*ppDataOut = p;
	}

	(*pHandleArray).RemoveAt(nIndex, 3);
	m_aryMode.RemoveAt(ConvertToModeIndex(nHandle));

	if (GetAngle())
		UpdateFromRotatedData();
}

void FWGlyphBezier::DuplicateHandle(int nHandle)
{
	int nOldBegin = ConvertToMainIndex(nHandle) - 1 - 1;

	FWPointArray *pHandleArray = GetAngle() ? &m_aryHandleRotated : &m_aryHandle;

	APointArray aryHandles;
	aryHandles.SetSize(3, 10);
	aryHandles[0] = (*pHandleArray)[nOldBegin];
	aryHandles[1] = (*pHandleArray)[nOldBegin + 1];
	aryHandles[2] = (*pHandleArray)[nOldBegin + 2];
	aryHandles[0].x += DEFAULT_HANDLE_RADIUS * 2;
	aryHandles[1].x += DEFAULT_HANDLE_RADIUS * 2;
	aryHandles[2].x += DEFAULT_HANDLE_RADIUS * 2;

	int mode = m_aryMode[ConvertToModeIndex(nHandle)];

	InsertHandleBefore(nHandle, aryHandles, &mode);
}

void FWGlyphBezier::InsertHandleBefore(int nHandle, const APointArray &aryHandle, void *pDataIn)
{
	int nIndex = ConvertToMainIndex(nHandle) - 1 - 1;

	FWPointArray *pHandleArray = GetAngle() ? &m_aryHandleRotated : &m_aryHandle;

	(*pHandleArray).InsertAt(nIndex, aryHandle[2]);
	(*pHandleArray).InsertAt(nIndex, aryHandle[1]);
	(*pHandleArray).InsertAt(nIndex, aryHandle[0]);

	if (pDataIn)
	{
		int *p = (int *)pDataIn;
		m_aryMode.InsertAt(ConvertToModeIndex(nHandle), p[0]);
	}

	if (GetAngle())
		UpdateFromRotatedData();
}

void FWGlyphBezier::UpdateFromRotatedData()
{
	m_fAngle = 0;
	int nCount = m_aryHandleRotated.GetSize();
	m_aryHandle.SetSize(nCount, 10);
	for (int i = 0; i < nCount; i++)
		m_aryHandle[i] = m_aryHandleRotated[i];
	ComputeSizeRect();
}

#ifdef _FW_EDITOR

void FWGlyphBezier::DrawHandles(FWView *pView) const
{
	const FWPointArray *pAryHandle = NULL;
	if (GetAngle())
		pAryHandle = &m_aryHandleRotated;
	else
		pAryHandle = &m_aryHandle;
	
	FWFlatCollector *pCollector = pView->GetFlatCollector();
	for (int i = 2; i <= GetHandleCount(); i += 3)
	{
		ARectI rectHandle;
		rectHandle = GetHandleRect(i);
		pView->LPtoDP(&rectHandle);
		// draw main handle
		pCollector->DrawRectangle2D(
			rectHandle, 
			COLOR_BLACK,
			true);
		
		// draw non-main handle if they should be visible
		if (GetMode(i) == MODE_BEZIER || GetMode(i) == MODE_BEZIER_CORNER)
		{
			APointI p1, p2;
			
			p1 = GetHandle(i - 1); p2 = GetHandle(i);
			pView->LPtoDP(&p1); pView->LPtoDP(&p2);
			pCollector->DrawLine(p1, p2);
			
			p1 = GetHandle(i + 1); p2 = GetHandle(i);
			pView->LPtoDP(&p1); pView->LPtoDP(&p2);
			pCollector->DrawLine(p1, p2);
			
			ARectI rect;
			rect = GetHandleRect(i - 1); pView->LPtoDP(&rect);
			pCollector->DrawRectangle2D(rect, COLOR_BLACK, true);
			
			rect = GetHandleRect(i + 1); pView->LPtoDP(&rect);
			pCollector->DrawRectangle2D(rect, COLOR_BLACK, true);
		}
	}
}


const char * FWGlyphBezier::GetContextMenu()
{
	return TO_STRING(FWDialogBezierMenu);
}

#endif



