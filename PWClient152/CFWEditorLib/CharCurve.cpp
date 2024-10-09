// Curve.cpp: implementation of the CCharCurve class.
//
//////////////////////////////////////////////////////////////////////

#include "CharCurve.h"
#include "CodeTemplate.h"
#include "FWAlgorithm.h"
#include "Global.h"

using namespace FWAlgorithm;

#define new A_DEBUG_NEW



CCharCurve::CCharCurve()
{
	m_pCharVerticesBuffer = new CCharVerticesBuffer();
}

CCharCurve::~CCharCurve()
{
	Destroy();
	SAFE_DELETE(m_pCharVerticesBuffer);
}

// note : the created curve does not contain the pStartPoint
bool CCharCurve::CreateFromFont(double *pStartPoint, TTPOLYCURVE *pCurveHeader)
{
	ASSERT(pCurveHeader);
	ASSERT(!m_pCharVerticesBuffer->IsCreated());


	// convert to cubic bezier line
	if (TT_PRIM_QSPLINE == pCurveHeader->wType)
	{
		BuildQBLine(pStartPoint, pCurveHeader, 1);
		//BuildPolyline(pStartPoint, pCurveHeader);
	}
	else if (TT_PRIM_CSPLINE == pCurveHeader->wType)
	{
		BuildPolyline(pStartPoint, pCurveHeader);
	}
	else
	{
		BuildPolyline(pStartPoint, pCurveHeader);
	}
	

	return true;
}

void CCharCurve::Destroy()
{
	m_pCharVerticesBuffer->Destroy();
}

bool CCharCurve::BuildQBLine(double *pStartPoint, TTPOLYCURVE *pCurveHeader, int nDeepth)
{
//	m_pCharVerticesBuffer->Create(1);
//	m_nCurrentIndex = 0;
//	m_pCharVerticesBuffer->SetVerticesAt(m_nCurrentIndex++, pStartPoint);
//	m_pCharVerticesBuffer->m_paVertices[m_nCurrentIndex][0] = 
//		FIXEDToDouble(pCurveHeader->apfx[pCurveHeader->cpfx - 1].x);
//	m_pCharVerticesBuffer->m_paVertices[m_nCurrentIndex][1] = 
//		FIXEDToDouble(pCurveHeader->apfx[pCurveHeader->cpfx - 1].y);
//	return true;


	if (pCurveHeader->cpfx < 2)
		return false;

	m_pCharVerticesBuffer->Create((pCurveHeader->cpfx - 1) * 2 * nDeepth + 1);
	
	// convert quadratic segments to cubic
	m_nCurrentIndex = 0;

	// add start point
	//m_pCharVerticesBuffer->SetVerticesAt(m_nCurrentIndex++, pStartPoint);

	double p[3][2];
	p[0][0] = pStartPoint[0];
	p[0][1] = pStartPoint[1];
	for (int i = 0; i < pCurveHeader->cpfx;)
	{
		p[1][0] = FIXEDToDouble(pCurveHeader->apfx[i].x);
		p[1][1] = FIXEDToDouble(pCurveHeader->apfx[i].y);
		if (++i == pCurveHeader->cpfx - 1)
		{
			p[2][0] = FIXEDToDouble(pCurveHeader->apfx[i].x);
			p[2][1] = FIXEDToDouble(pCurveHeader->apfx[i].y);
			i++;
		}
		else
		{
			p[2][0] = (FIXEDToDouble(pCurveHeader->apfx[i - 1].x) + FIXEDToDouble(pCurveHeader->apfx[i].x)) / 2;
			p[2][1] = (FIXEDToDouble(pCurveHeader->apfx[i - 1].y) + FIXEDToDouble(pCurveHeader->apfx[i].y)) / 2;
		}
		ConvertQBToCB(p[0], p[1], p[2], nDeepth);
	
		p[0][0] = p[2][0];
		p[0][1] = p[2][1];
	}
	m_pCharVerticesBuffer->SetVerticesAt(m_nCurrentIndex++, p[2]);
	
	return true;
}

void CCharCurve::ConvertQBToCB(double *p0, double *p1, double *p2, int nDeepth)
{
	double newTwo[2][2];

	newTwo[0][0] = p0[0] + (p1[0] - p0[0]) / 2;
	newTwo[0][1] = p0[1] + (p1[1] - p0[1]) / 2;
	
	newTwo[1][0] = p1[0] + (p2[0] - p1[0]) / 2;
	newTwo[1][1] = p1[1] + (p2[1] - p1[1]) / 2;

	if (1 == nDeepth)
	{
		m_pCharVerticesBuffer->SetVerticesAt(m_nCurrentIndex++, newTwo[0]);
		m_pCharVerticesBuffer->SetVerticesAt(m_nCurrentIndex++, newTwo[1]);
	}
	else
	{
		double newCenter[2];
		newCenter[0] = newTwo[0][0] + (newTwo[1][0] - newTwo[0][0]) / 2;
		newCenter[1] = newTwo[0][1] + (newTwo[1][1] - newTwo[0][1]) / 2;

		ConvertQBToCB(p0, newTwo[0], newCenter, nDeepth - 1);
		ConvertQBToCB(newCenter, newTwo[1], p2, nDeepth - 1);
	}
}

bool CCharCurve::BuildPolyline(double *pStartPoint, TTPOLYCURVE *pCurveHeader)
{
	m_pCharVerticesBuffer->Create(pCurveHeader->cpfx);
	// simply use each point as vertex
	for (int i = 0; i < pCurveHeader->cpfx; i++)
	{
		m_pCharVerticesBuffer->m_paVertices[i][0] = FIXEDToDouble(pCurveHeader->apfx[i].x);
		m_pCharVerticesBuffer->m_paVertices[i][1] = FIXEDToDouble(pCurveHeader->apfx[i].y);
	}
	return true;
}
