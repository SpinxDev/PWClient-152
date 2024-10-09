// EditerBezierPoint.cpp: implementation of the CEditerBezierPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "EditerBezierPoint.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEditerBezierPoint::CEditerBezierPoint()
{
	m_int_id  = INVALIDOBJECTID;
	m_av3_pos = A3DVECTOR3(0.0f, 0.0f, 0.0f);
	m_av3_dir = A3DVECTOR3(0.0f, 0.0f, 1.0f);
}

CEditerBezierPoint::~CEditerBezierPoint()
{

}

A3DVECTOR3 CEditerBezierPoint::GetPos()
{
	return m_av3_pos;
}

void CEditerBezierPoint::SetPos(const A3DVECTOR3 &pos)
{
	m_av3_pos = pos;
	SetPosition(pos);
}

int CEditerBezierPoint::GetID()
{
	return m_int_id;
}

void CEditerBezierPoint::SetID(int id)
{
	m_int_id = id;
}

bool CEditerBezierPoint::Init(A3DDevice * pA3DDevice, 
							  const A3DVECTOR3& vecPos, 
							  A3DCOLOR color, 
							  float vRadius)
{
	m_av3_pos = vecPos;
	return A3DBezierPoint::Init(pA3DDevice, vecPos, color, vRadius);
}
