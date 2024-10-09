// EditerBezierSegment.cpp: implementation of the CEditerBezierSegment class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "EditerBezierSegment.h"
#include "EditerBezier.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CEditerBezierSegment::CEditerBezierSegment()
{
	m_int_link			= INVALIDOBJECTID;
	m_nHeadID           = INVALIDOBJECTID;
	m_nTailID           = INVALIDOBJECTID;
	m_av3_anchor1		= A3DVECTOR3(0.0f, 0.0f, 0.0f);
	m_av3_anchor1		= A3DVECTOR3(0.0f, 0.0f, 0.0f);
	m_av3_head			= A3DVECTOR3(0.0f, 0.0f, 0.0f);
	m_av3_tail			= A3DVECTOR3(0.0f, 0.0f, 0.0f);
}

CEditerBezierSegment::~CEditerBezierSegment()
{
}

int CEditerBezierSegment::GetLink()
{
	return m_int_link;
}

void CEditerBezierSegment::SetLink(int link)
{
	m_int_link = link;
}

A3DVECTOR3 CEditerBezierSegment::GetHead()
{
	return m_av3_head;
}

void CEditerBezierSegment::SetHead(const A3DVECTOR3 &head)
{
	m_av3_head = head;
	SetStartPos(A3DVECTOR3(head.x, head.y, head.z));
}

A3DVECTOR3 CEditerBezierSegment::GetTail()
{
	return m_av3_tail;
}

void CEditerBezierSegment::SetTail(const A3DVECTOR3 &tail)
{
	m_av3_tail = tail;
	SetEndPos(A3DVECTOR3(tail.x, tail.y, tail.z));
}

A3DVECTOR3 CEditerBezierSegment::GetAnchor1()
{	
	return m_av3_anchor1;
}

void CEditerBezierSegment::SetAnchor1(const A3DVECTOR3 & anchor)
{
	m_av3_anchor1 = anchor;
	SetStartCtrl(A3DVECTOR3(anchor.x, anchor.y, anchor.z));
}

A3DVECTOR3 CEditerBezierSegment::GetAnchor2()
{
	return m_av3_anchor2;
}

void CEditerBezierSegment::SetAnchor2(const A3DVECTOR3 &anchor)
{
	m_av3_anchor2 = anchor;
	SetEndCtrl(A3DVECTOR3(anchor.x, anchor.y, anchor.z));
}

bool CEditerBezierSegment::Init(A3DDevice * pA3DDevice, 
								A3DVECTOR3& vecStart, 
								A3DVECTOR3& vecEnd,
								A3DVECTOR3& vecStartCtrl, 
								A3DVECTOR3& vecEndCtrl, 
								float vSegmentRadius, 
								float vControlRadius,
								A3DCOLOR colorSegment, 
								A3DCOLOR colorControl)
{
	m_av3_head = vecStart;
	m_av3_tail = vecEnd;
	m_av3_anchor1 = vecStartCtrl;
	m_av3_anchor2 = vecEndCtrl;

	return A3DBezierSegment::Init(
		pA3DDevice, 
		vecStart, 
		vecEnd, 
		vecStartCtrl, 
		vecEndCtrl, 
		vSegmentRadius, 
		vControlRadius,
		colorSegment, 
		colorControl);
}

A3DVECTOR3 CEditerBezierSegment::Bezier(float u)
{
	// BEZIER 曲线计算的相关数据
	A3DVECTOR3	pt1			= m_av3_head,
				control1	= m_av3_anchor1, 
				control2	= m_av3_anchor2, 
				pt2			= m_av3_tail;


	float a[3], b[3], c[3];
	for(int i = 0; i < 3;i ++)
	{
		c[i] = 3 * (control1.m[i] - pt1.m[i]);
		b[i] = 3 * (control2.m[i] - control1.m[i]) - c[i];
		a[i] = pt2.m[i] - pt1.m[i] - c[i] - b[i];
	}

	A3DVECTOR3 pos;
	for(i = 0; i < 3; i ++)
	{
		pos.m[i] = (a[i] * u * u * u) + (b[i] * u * u) + (c[i] * u) + pt1.m[i];
	}

	return pos;
}

void CEditerBezierSegment::ReCalculateLength()
{
	// 计算段长度
	float start =0.0f;
	float end = 1.0f;
	if(start == 0.0f) m_av3_saves[0] = m_av3_head;
	else m_av3_saves[0] = Bezier(start);

	if(end == 1.0f) m_av3_saves[CALCLENGTHDIVIDE - 1] = m_av3_tail;
	else m_av3_saves[CALCLENGTHDIVIDE - 1] = Bezier(end);

	for(int i = 1;i < (CALCLENGTHDIVIDE - 1); i ++)
	{
		m_av3_saves[i] = Bezier(start + (end - start) * ((float)i / CALCLENGTHDIVIDE));
	}

	float length = 0.0f;
	for(i = 0;i < (CALCLENGTHDIVIDE - 1);i ++)
	{
		// 计算 i ~ i + 1点的长度
		A3DVECTOR3 temp = (m_av3_saves[i + 1] - m_av3_saves[i]);
		length += temp.Magnitude();
	}

	m_fLenght =  length;
}

A3DVECTOR3 CEditerBezierSegment::Vector(float u)
{
	A3DVECTOR3 headspot = m_av3_head_spot,
			   tailspot = m_av3_tail_spot;

	if(u <= 0.0f) return headspot;
	if(u >= 1.0f) return tailspot;

	float m1 = headspot.Magnitude(),
		  m2 = tailspot.Magnitude();

	float dot = DotProduct(headspot.Normalize(), tailspot.Normalize());

	if(dot > 1.0f)
	{
		return headspot;
	}

	if(dot < -1.0f) dot = -1.0f;

	float r = (float)acos(dot);
	bool isinv = _isnan(r) ? true : false;
	float deg = RAD2DEG(r);
	r = r * u;

	A3DVECTOR3 dir = CrossProduct(headspot, tailspot);
	A3DMATRIX4 matrix;
	matrix.RotateAxis(dir, r);
	headspot = matrix * headspot;
	return headspot.Normalize() * (m1 + (m2 - m1) * u);
}