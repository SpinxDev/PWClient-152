/*
 * FILE: EC_ClockIcon.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_ClockIcon.h"
#include "EC_Game.h"

#include "AAssist.h"
#include "A3DEngine.h"
#include "A3DFlatCollector.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

//	Indices
WORD CECClockIcon::m_aIndices[24] = 
{
	8, 7, 0, 8, 0, 1, 
	8, 1, 2, 8, 2, 3, 
	8, 3, 4, 8, 4, 5, 
	8, 5, 6, 8, 6, 7,
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECClockIcon
//	
///////////////////////////////////////////////////////////////////////////

CECClockIcon::CECClockIcon()
{
	m_x		= 0;
	m_y		= 0;
	m_cx	= 16;
	m_cy	= 16;
	m_fz	= 0.0f;
	m_iPos	= 0;
	m_iMin	= 0;
	m_iMax	= 100;
	m_iStep	= 10;
	m_dwCol	= 0xffffffff;
}

CECClockIcon::~CECClockIcon()
{
}

//	Advances the current position
int CECClockIcon::OffsetProgressPos(int iOffset)
{
	m_iPos += iOffset;
	a_Clamp(m_iPos, m_iMin, m_iMax);
	return m_iPos;
}

//	Sets the current position
void CECClockIcon::SetProgressPos(int iPos)
{
	m_iPos = iPos;
	a_Clamp(m_iPos, m_iMin, m_iMax);
}

//	Sets the minimum and maximum ranges 
void CECClockIcon::SetProgressRange(int iMin, int iMax)
{
	ASSERT(iMin < iMax);
	m_iMin = iMin;
	m_iMax = iMax;
	a_Clamp(m_iPos, m_iMin, m_iMax);
}

//	Advances the current position by the step increment
int CECClockIcon::StepProgress()
{
	m_iPos += m_iStep;
	a_Clamp(m_iPos, m_iMin, m_iMax);
	return m_iPos;
}

//	Set icon position and size
void CECClockIcon::MoveIcon(int x, int y, int cx, int cy)
{
	m_x		= x;
	m_y		= y;
	m_cx	= cx;
	m_cy	= cy;
	
	OnIconMove();
}

//	Update icon position and size
void CECClockIcon::OnIconMove()
{
	float l = (float)m_x;
	float t = (float)m_y;
	float r = (float)m_x + m_cx - 1;
	float b = (float)m_y + m_cy - 1;
	float xc = (l + r) * 0.5f;
	float yc = (t + b) * 0.5f;

	m_aPos[0].x	= r;
	m_aPos[0].y	= t;
	m_aPos[0].z	= m_fz;

	m_aPos[1].x	= r;
	m_aPos[1].y	= yc;
	m_aPos[1].z	= m_fz;

	m_aPos[2].x	= r;
	m_aPos[2].y	= b;
	m_aPos[2].z	= m_fz;

	m_aPos[3].x	= xc;
	m_aPos[3].y	= b;
	m_aPos[3].z	= m_fz;

	m_aPos[4].x	= l;
	m_aPos[4].y	= b;
	m_aPos[4].z	= m_fz;

	m_aPos[5].x	= l;
	m_aPos[5].y	= yc;
	m_aPos[5].z	= m_fz;

	m_aPos[6].x	= l;
	m_aPos[6].y	= t;
	m_aPos[6].z	= m_fz;

	m_aPos[7].x	= xc;
	m_aPos[7].y	= t;
	m_aPos[7].z	= m_fz;

	m_aPos[8].x	= xc;
	m_aPos[8].y	= yc;
	m_aPos[8].z	= m_fz;
}

//	Render routine
void CECClockIcon::Render()
{
	A3DFlatCollector* pFC = g_pGame->GetA3DEngine()->GetA3DFlatCollector();
	ASSERT(pFC);

	//	Handle special progress position
	if (m_iPos == m_iMax)
	{
		return;
	}
	else if (m_iPos == m_iMin)
	{
		A3DVECTOR3 aPos[4];
		aPos[0] = m_aPos[0];
		aPos[1] = m_aPos[2];
		aPos[2] = m_aPos[4];
		aPos[3] = m_aPos[6];

		static const WORD aIndices[6] = {0, 1, 2, 0, 2, 3};
		pFC->AddRenderData_2D(aPos, 4, aIndices, 6, m_dwCol);
		return;
	}

	float fSegLen = (m_iMax - m_iMin) * 0.125f;
	float fCurSeg = (m_iPos - m_iMin) / fSegLen;

	int iSeg = (int)fCurSeg;
	a_Clamp(iSeg, 0, 7);
	float fResidue = fCurSeg - iSeg;

	float l = (float)m_x;
	float t = (float)m_y;
	float r = (float)m_x + m_cx - 1;
	float b = (float)m_y + m_cy - 1;
	float xc = (l + r) * 0.5f;
	float yc = (t + b) * 0.5f;
	float xhl = m_cx * 0.5f;
	float yhl = m_cy * 0.5f;

	A3DVECTOR3& v = m_aPos[9];
	v.z = m_fz;
	
	switch (iSeg)
	{
	case 0:	v.x = xc + xhl * fResidue; v.y = t;		break;
	case 1: v.x = r; v.y = t + yhl * fResidue;		break;
	case 2: v.x = r; v.y = yc + yhl * fResidue;		break;
	case 3: v.x = r - xhl * fResidue; v.y = b;		break;
	case 4: v.x = xc - xhl * fResidue; v.y = b;		break;
	case 5: v.x = l; v.y = b - yhl * fResidue;		break;
	case 6: v.x = l; v.y = yc - yhl * fResidue;		break;
	case 7: v.x = l + xhl * fResidue; v.y = t;		break;
	}

	WORD aIndices[24];
	aIndices[0] = 8;
	aIndices[1] = 9;
	aIndices[2] = iSeg;

	int iDst = 3;
	for (int i=(iSeg+1)*3; i < 24; i++, iDst++)
		aIndices[iDst] = m_aIndices[i];

	pFC->AddRenderData_2D(m_aPos, 10, aIndices, iDst, m_dwCol);
}



