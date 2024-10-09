// Algorithm.cpp: implementation of the Algorithm class.
//
//////////////////////////////////////////////////////////////////////

#include "FWAlgorithm.h"
#include "CodeTemplate.h"
#include "A3DGFXEditorInterface.h"
#include "FWArchive.h"
#include "CharContour.h"
#include "CharOutline.h"
#include "CharVerticesBuffer.h"
#include "A3DGFXElement.h"
#include "A3DGFXEx.h"
#include "A3DParticleSystemEx.h"
#include <ocidl.h>

#define new A_DEBUG_NEW

ARectI FWAlgorithm::GetMinRect(const FWArray<APointI, APointI &> & PointArray)
{
	int size = PointArray.GetSize();
	if(size == 0)
		return ARectI(0, 0, 0, 0);
	int MinX = INT_MAX, MinY = INT_MAX, MaxX = INT_MIN, MaxY = INT_MIN;
	for(int i = 0; i < size; i++)
	{
		if(PointArray[i].x < MinX)
			MinX = PointArray[i].x;
		if(PointArray[i].y < MinY)
			MinY = PointArray[i].y;
		if(PointArray[i].x > MaxX)
			MaxX = PointArray[i].x;
		if(PointArray[i].y > MaxY)
			MaxY = PointArray[i].y;
	}
	return ARectI(MinX, MinY, MaxX, MaxY);
}

ARectI FWAlgorithm::GetMinRect(const POINT* PointArray, int size)
{
	if(size == 0)
		return ARectI(0, 0, 0, 0);
	int MinX = INT_MAX, MinY = INT_MAX, MaxX = INT_MIN, MaxY = INT_MIN;
	for(int i = 0; i < size; i++)
	{
		if(PointArray[i].x < MinX)
			MinX = PointArray[i].x;
		if(PointArray[i].y < MinY)
			MinY = PointArray[i].y;
		if(PointArray[i].x > MaxX)
			MaxX = PointArray[i].x;
		if(PointArray[i].y > MaxY)
			MaxY = PointArray[i].y;
	}
	return ARectI(MinX, MinY, MaxX, MaxY);
}


APointI FWAlgorithm::RotatePoint(APointI original, POINTF centre, double angle)
{
	//Calculate the new point use the following matrix:
	//	(cos¦È, -sin¦È, centre.x(1-cos¦È) + centre.y * sin¦È
	//	 sin¦È, cos¦È,  centre.y(1-cos¦È) - centre.x * sin¦È
	//	 0,		0,		1                                   )
	
	APointI result;
	double sinAngle = sin(angle);
	double cosAngle = cos(angle);
	double originalX = original.x;
	double originalY = original.y;
	double centreX = centre.x;
	double centreY = centre.y;
	result.x = (long)(originalX * cosAngle - originalY * sinAngle 
				+ centreX * ( 1 - cosAngle ) + centreY * sinAngle);
	result.y = (long)(originalX * sinAngle + originalY * cosAngle
				+ centreY * ( 1 - cosAngle ) - centreX * sinAngle);
	return result;	
}

void FWAlgorithm::RotatePoint(const POINT* original, POINT* result, int count, POINTF centre, double angle)
{
	double sinAngle = sin(angle);
	double cosAngle = cos(angle);
	double centreX = centre.x;
	double centreY = centre.y;
	double offsetX = centreX * ( 1 - cosAngle ) + centreY * sinAngle;
	double offsetY = centreY * ( 1 - cosAngle ) - centreX * sinAngle;

	double originalX;
	double originalY;
	for(int i = 0; i < count; i++, original++, result++)
	{
		originalX = original->x;
		originalY = original->y;
		result->x = (long)(originalX * cosAngle - 
			originalY * sinAngle + offsetX);
		result->y = (long)(originalX * sinAngle +
			originalY * cosAngle + offsetY);
	}
}

bool FWAlgorithm::IsPointsInLine(const FWArray<APointI, APointI&>& pointArray)
{
	int nSize = pointArray.GetSize();
	if (nSize <= 2)  
		return true;
	
	APointI ptFirst = pointArray.GetAt(0);
	APointI ptSecond = pointArray.GetAt(1);

	double fAngleFirst = atan2(double(ptSecond.y - ptFirst.y), double(ptSecond.x - ptFirst.x));
	for (int i = 2; i < nSize; ++i)
	{
		ptFirst = ptSecond;
		ptSecond = pointArray.GetAt(i);
		double fAngle = atan2(double(ptSecond.y - ptFirst.y), double(ptSecond.x - ptFirst.x));
		if (fabs(fAngle - fAngleFirst) > 0.001) 
			return false;
	}
	return true;
}

void FWAlgorithm::RotatePoint(const FWArray<APointI, APointI &> & original, FWArray<APointI, APointI&> & result, 
	const POINTF & centre, double angle)
{
	if (result.GetSize() != original.GetSize())
		result.SetSize(original.GetSize(), 0);

	double sinAngle = sin(angle);
	double cosAngle = cos(angle);
	double centreX = centre.x;
	double centreY = centre.y;
	double offsetX = centreX * ( 1 - cosAngle ) + centreY * sinAngle;
	double offsetY = centreY * ( 1 - cosAngle ) - centreX * sinAngle;

	double originalX;
	double originalY;
	int count = original.GetSize();
	for(int i = 0; i < count; i++)
	{
		originalX = original[i].x;
		originalY = original[i].y;
		result[i].x = (long)(originalX * cosAngle - 
			originalY * sinAngle + offsetX);
		result[i].y = (long)(originalX * sinAngle +
			originalY * cosAngle + offsetY);
	}

}

void FWAlgorithm::CopyPointArray(FWArray<APointI, APointI &> & dest, const FWArray<APointI, APointI &> & original)
{
	dest.RemoveAll();
	dest.Append(original);
}

void FWAlgorithm::Point2DoubleArray(const APointI& point, double * array)
{
	array[0] = point.x;
	array[1] = point.y;
}

void FWAlgorithm::Point2DoubleArray(const POINT& point, double * array)
{
	array[0] = point.x;
	array[1] = point.y;
}

void FWAlgorithm::ClampPointToRect(APointI &pt, const ARectI &rect)
{
	if (pt.x < rect.left)
		pt.x = rect.left;
	else if (pt.x > rect.right)
		pt.x = rect.right;

	if (pt.y < rect.top)
		pt.y = rect.top;
	else if (pt.y > rect.bottom)
		pt.y = rect.bottom;
}

void FWAlgorithm::ClampRectToRect(ARectI &rect, const ARectI &standard)
{
	if (rect.top < standard.top)
		rect.Offset(0, standard.top - rect.top);
	else if (rect.bottom > standard.bottom)
		rect.Offset(0, standard.bottom - rect.bottom);

	if (rect.left < standard.left)
		rect.Offset(standard.left - rect.left, 0);
	else if (rect.right > standard.right)
		rect.Offset(standard.right - rect.right, 0);
}

void FWAlgorithm::SerializeGFX_PROPERTY(GFX_PROPERTY & prop, FWArchive &ar)
{
	// note : prop.GetType() must be the same for storing and loading
	if (ar.IsStoring())
	{
		switch(prop.GetType()) {
		case GFX_VALUE_STRING:
			ar << prop.m_string;
			break;
		case GFX_VALUE_INT:
			ar << prop.m_Data.i;
			break;
		case GFX_VALUE_FLOAT:
			ar << prop.m_Data.f;
			break;
		default:
			ASSERT(false);
			break;
		}
	}
	else
	{
		switch(prop.GetType()) {
		case GFX_VALUE_STRING:
			ar >> prop.m_string;
			break;
		case GFX_VALUE_INT:
			ar >> prop.m_Data.i;
			break;
		case GFX_VALUE_FLOAT:
			ar >> prop.m_Data.f;
			break;
		default:
			ASSERT(false);
			break;
		}
	}
}


APointI FWAlgorithm::GetRelativePointToRect(const ARectI * lpRect, APointI * lpPoint)
{
	APointI pt = *lpPoint;
	pt.x -= lpRect->left;
	pt.y -= lpRect->top;
	
	return pt;
}

double FWAlgorithm::FIXEDToDouble(FIXED f)
{
	return double(f.value) + double(f.fract) / 0x10000;
}

int FWAlgorithm::DoubleToInt(double d)
{
	if (d - int(d) > 0.5)
		return int(d) + 1;
	else
		return int(d);
}


ARectI FWAlgorithm::GetOutlineRect(const CCharOutline *pOutline)
{
	float xmin = FLT_MAX, ymin = FLT_MAX, xmax = FLT_MIN, ymax = FLT_MIN;
	const APtrList<CCharContour *> * pContourList = pOutline->GetCharContoursList();
	ALISTPOSITION pos = pContourList->GetHeadPosition();
	while (pos)
	{
		CCharContour *pContour = pContourList->GetNext(pos);
		CCharVerticesBuffer* pBuffer = pContour->GetCharVerticesBuffer();
		for (int i = 0; i < pBuffer->m_nVerticesCount; i++)
		{
			if (pBuffer->m_paVertices[i][0] < xmin)
				xmin = pBuffer->m_paVertices[i][0];
			if (pBuffer->m_paVertices[i][0] > xmax)
				xmax = pBuffer->m_paVertices[i][0];
			if (pBuffer->m_paVertices[i][1] < ymin)
				ymin = pBuffer->m_paVertices[i][1];
			if (pBuffer->m_paVertices[i][1] > ymax)
				ymax = pBuffer->m_paVertices[i][1];
		}
	}
	
	return ARectI(xmin, ymin, xmax, ymax);
}

bool FWAlgorithm::IsParticleSystem(const A3DGFXElement *pElement)
{
	return (pElement->GetEleTypeId() >= 120 && pElement->GetEleTypeId() <= 124);
}

int FWAlgorithm::ComputeParticalCount(const FWAssembly *pAssembly)
{
	int nPartCount = 0;
	
	int nEleCount = pAssembly->GetElementCount();
	for (int i = 0; i < nEleCount; i++)
	{
		const A3DGFXElement *pElement = pAssembly->GetElement(i);
		if (IsParticleSystem(pElement))
		{
			const A3DParticleSystemEx *pPartSys = 
				static_cast<const A3DParticleSystemEx *>(pElement);
			
			
			int nPartCountByQuota = pPartSys->GetQuota();
			
			const A3DParticleEmitter *pEmitter = pPartSys->GetEmitter();
			float fRate = pEmitter->GetProperty(ID_GFXOP_EMITTER_RATE);
			float fTTL = float(pEmitter->GetProperty(ID_GFXOP_EMITTER_TTL)) / 1000.f;
			int nPartCountByEmmiter = fRate * fTTL;
			
			if (nPartCountByQuota != -1 && nPartCountByQuota < nPartCountByEmmiter)
				nPartCount += nPartCountByQuota;
			else
				nPartCount += nPartCountByEmmiter;
		}
	}
	
	return nPartCount;
}

bool FWAlgorithm::IsGfxSound(const A3DGFXElement *pElement)
{
	return pElement->GetEleTypeId() == ID_ELE_TYPE_SOUND;
}

int FWAlgorithm::ComputeSoundCount(const FWAssembly *pAssembly)
{
	int nSoundCount = 0;
	int nEleCount = pAssembly->GetElementCount();
	for (int i = 0; i < nEleCount; i++)
	{
		const A3DGFXElement *pEle = pAssembly->GetElement(i);
		if (FWAlgorithm::IsGfxSound(pEle))
			nSoundCount++;
	}
	
	return nSoundCount;
}

void FWAlgorithm::RemoveSoundElement(FWAssembly *pAssembly)
{
	int nCount = pAssembly->GetElementCount();
	for (int i = 0; i < nCount; i++)
	{
		A3DGFXElement *pEle = pAssembly->GetElement(i);
		if (FWAlgorithm::IsGfxSound(pEle))
		{
			pAssembly->RemoveElement(pEle);
			nCount--;
			i = 0;
		}
	}
}


double FWAlgorithm::GetLengthWithTwoPoint(const APointI &ptFirst, const APointI &ptSecond)
{
	return sqrt(double(ptFirst.x - ptSecond.x) * (ptFirst.x - ptSecond.x) + 
		(ptFirst.y - ptSecond.y) * (ptFirst.y - ptSecond.y));
}
