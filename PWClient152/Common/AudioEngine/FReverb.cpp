#include "FReverb.h"
#include "FEventSystem.h"
#include <fmod.hpp>

using namespace AudioEngine;

Reverb::Reverb(void)
: m_pEventSystem(0)
, m_pReverb(0)
, m_pGeometry(0)
, m_iShape(REVERB_SHAPE_NONE)
{
}

Reverb::~Reverb(void)
{
	release();
}

bool Reverb::Init(EventSystem* pEventSystem)
{
	if(!pEventSystem)
		return false;
	m_pEventSystem = pEventSystem;
	FMOD::System* pSystem = m_pEventSystem->GetSystem();
	if(!pSystem)
		return false;
	if(FMOD_OK != pSystem->createReverb(&m_pReverb))
		return false;
	return true;
}

bool Reverb::SetProperty(REVERB_PROPERTY prop)
{
	m_ReverbProperty = prop;
	FMOD_REVERB_PROPERTIES fmod_prop = {0};
	if(sizeof(fmod_prop) != sizeof(m_ReverbProperty))
		return false;
	memcpy(&fmod_prop, &m_ReverbProperty, sizeof(fmod_prop));
	if(FMOD_OK != m_pReverb->setProperties(&fmod_prop))
		return false;
	return true;
}

void Reverb::GetProperty(REVERB_PROPERTY& prop) const
{
	prop = m_ReverbProperty;
}

bool Reverb::SetActive(bool bActive)
{
	if(!m_pReverb)
		return false;
	if(FMOD_OK != m_pReverb->setActive(bActive))
		return false;
	if(!m_pGeometry)
		return true;
	if(FMOD_OK != m_pGeometry->setActive(bActive))
		return false;
	return true;
}

void Reverb::SetName(const char* szName)
{
	m_strName = szName;
}

const char* Reverb::GetName() const
{
	return m_strName.c_str();
}

bool Reverb::Set3DAttributes(const VECTOR& pos, float fMinDistance, float fMaxDistance)
{
	if(!m_pReverb)
		return false;
	FMOD_VECTOR fmod_pos = {pos.x, pos.y, pos.z};
	if(FMOD_OK != m_pReverb->set3DAttributes(&fmod_pos, fMinDistance, fMaxDistance))
		return false;
	if(m_pGeometry && (m_iShape == REVERB_SHAPE_SPHERE || m_iShape == REVERB_SHAPE_CUBIOD))
	{
		if(FMOD_OK != m_pGeometry->setPosition(&fmod_pos))
			return false;
	}
	return true;
}

void Reverb::release()
{
	if(m_pReverb)
		m_pReverb->release();
	m_pReverb = 0;
	if(m_pGeometry)
		m_pGeometry->release();
	m_pGeometry = 0;
}

bool Reverb::SetShapeSphere(const VECTOR& center, float radius)
{
	m_iShape = REVERB_SHAPE_SPHERE;
	if(m_pGeometry && FMOD_OK != m_pGeometry->release())
		return false;
	m_pGeometry = 0;
	FMOD::System* pSystem = m_pEventSystem->GetSystem();
	if(!pSystem)
		return false;
	const static int cs_iDeg = 45;
	int iCircleVertNum = 360/cs_iDeg;
	int iCircleNum = 180/cs_iDeg;

	if(FMOD_OK != pSystem->createGeometry(iCircleNum*iCircleVertNum, (iCircleNum-2)*iCircleVertNum*4+iCircleVertNum*2*3, &m_pGeometry))
		return false;	
	const static float cs_pi = 3.1415926f;
	int index3[3] = {0, 1, 2};
	int index4[4] = {0, 1, 2, 3};
	FMOD_VECTOR vertex3[3] = {0};
	FMOD_VECTOR vertex4[4] = {0};
	FMOD_VECTOR top = {0, radius, 0};
	FMOD_VECTOR bottom = {0, -radius, 0};
	float heady = cosf(cs_pi*cs_iDeg/180)*radius;
	float headradius = sinf(cs_pi*cs_iDeg/180)*radius;
	for (int i=0; i<iCircleVertNum; ++i)
	{		
		FMOD_VECTOR head0 = {cosf(cs_pi*cs_iDeg*i/180)*headradius, heady, sinf(cs_pi*cs_iDeg*i/180)*headradius};
		FMOD_VECTOR head1 = {cosf(cs_pi*cs_iDeg*(i+1)/180)*headradius, heady, sinf(cs_pi*cs_iDeg*(i+1)/180)*headradius};
		FMOD_VECTOR tail0 = head0;
		tail0.y = -heady;
		FMOD_VECTOR tail1 = head1;
		tail1.y = -heady;
		vertex3[0] = top;
		vertex3[1] = head0;
		vertex3[2] = head1;
		if(FMOD_OK != m_pGeometry->addPolygon(1.0f, 1.0f, true, 3, vertex3, index3))
			return false;

		vertex3[0] = bottom;
		vertex3[1] = tail0;
		vertex3[2] = tail1;
		if(FMOD_OK != m_pGeometry->addPolygon(1.0f, 1.0f, true, 3, vertex3, index3))
			return false;
	}

	for (int i=1; i<iCircleNum-1; ++i)
	{
		float heady = cosf(cs_pi*cs_iDeg*i/180)*radius;
		float taily = cosf(cs_pi*cs_iDeg*(i+1)/180)*radius;

		float headradius = sinf(cs_pi*cs_iDeg*i/180)*radius;
		float tailradius = sinf(cs_pi*cs_iDeg*(i+1)/180)*radius;

		for (int j=0; j<iCircleVertNum; ++j)
		{
			FMOD_VECTOR head0 = {cosf(cs_pi*cs_iDeg*j/180)*headradius, heady, sinf(cs_pi*cs_iDeg*j/180)*headradius};
			FMOD_VECTOR head1 = {cosf(cs_pi*cs_iDeg*(j+1)/180)*headradius, heady, sinf(cs_pi*cs_iDeg*(j+1)/180)*headradius};
			
			FMOD_VECTOR tail0 = {cosf(cs_pi*cs_iDeg*j/180)*tailradius, taily, sinf(cs_pi*cs_iDeg*j/180)*tailradius};
			FMOD_VECTOR tail1 = {cosf(cs_pi*cs_iDeg*(j+1)/180)*tailradius, taily, sinf(cs_pi*cs_iDeg*(j+1)/180)*tailradius};

			vertex4[0] = head1;
			vertex4[1] = head0;
			vertex4[2] = tail0;
			vertex4[3] = tail1;
			if(FMOD_OK != m_pGeometry->addPolygon(1.0f, 1.0f, true, 4, vertex4, index4))
				return false;
		}
	}

	FMOD_VECTOR fmod_pos = {center.x, center.y, center.z};
	if(FMOD_OK != m_pGeometry->setPosition(&fmod_pos))
		return false;	
	return true;
}

bool Reverb::SetShapeCubiod(const VECTOR& center, float length, float width, float hight)
{
	m_iShape = REVERB_SHAPE_CUBIOD;
	if(m_pGeometry && FMOD_OK != m_pGeometry->release())
		return false;
	m_pGeometry = 0;
	if(!m_pEventSystem)
		return false;
	FMOD::System* pSystem = m_pEventSystem->GetSystem();
	if(!pSystem)
		return false;
	if(FMOD_OK != pSystem->createGeometry(6, 24, &m_pGeometry))
		return false;
/*
top
a	b
c	d

A	B
C	D

a===>A ...
*/
	FMOD_VECTOR a = {-length/2.0f, +hight/2.0f, +width/2.0f};
	FMOD_VECTOR b = {+length/2.0f, +hight/2.0f, +width/2.0f};
	FMOD_VECTOR c = {-length/2.0f, +hight/2.0f, -width/2.0f};
	FMOD_VECTOR d = {+length/2.0f, +hight/2.0f, -width/2.0f};

	FMOD_VECTOR A = {-length/2.0f, -hight/2.0f, +width/2.0f};
	FMOD_VECTOR B = {+length/2.0f, -hight/2.0f, +width/2.0f};
	FMOD_VECTOR C = {-length/2.0f, -hight/2.0f, -width/2.0f};
	FMOD_VECTOR D = {+length/2.0f, -hight/2.0f, -width/2.0f};
	
	int index[4] = {0, 1, 2, 3};

	FMOD_VECTOR vertex[4] = {0};


	vertex[0] = a;
	vertex[1] = b;
	vertex[2] = d;
	vertex[3] = c;	
	if(FMOD_OK != m_pGeometry->addPolygon(1.0f, 1.0f, true, 4, vertex, index))
		return false;

	vertex[0] = A;
	vertex[1] = a;
	vertex[2] = c;
	vertex[3] = C;
	if(FMOD_OK != m_pGeometry->addPolygon(1.0f, 1.0f, true, 4, vertex, index))
		return false;

	vertex[0] = B;
	vertex[1] = A;
	vertex[2] = C;
	vertex[3] = D;
	if(FMOD_OK != m_pGeometry->addPolygon(1.0f, 1.0f, true, 4, vertex, index))
		return false;

	vertex[0] = d;
	vertex[1] = b;
	vertex[2] = B;
	vertex[3] = D;
	if(FMOD_OK != m_pGeometry->addPolygon(1.0f, 1.0f, true, 4, vertex, index))
		return false;

	vertex[0] = c;
	vertex[1] = d;
	vertex[2] = D;
	vertex[3] = C;
	if(FMOD_OK != m_pGeometry->addPolygon(1.0f, 1.0f, true, 4, vertex, index))
		return false;

	vertex[0] = b;
	vertex[1] = a;
	vertex[2] = A;
	vertex[3] = B;
	if(FMOD_OK != m_pGeometry->addPolygon(1.0f, 1.0f, true, 4, vertex, index))
		return false;

	FMOD_VECTOR fmod_pos = {center.x, center.y, center.z};
	if(FMOD_OK != m_pGeometry->setPosition(&fmod_pos))
		return false;
	return true;
}

bool Reverb::SetShapePolygon(VECTOR* pPoints, int iPtNum, float fBottom, float fTop)
{
	m_iShape = REVERB_SHAPE_POLYGON;
	if(iPtNum < 3)
		return false;
	if(fBottom == fTop)
		return false;

	if(m_pGeometry && FMOD_OK != m_pGeometry->release())
		return false;
	m_pGeometry = 0;
	FMOD::System* pSystem = m_pEventSystem->GetSystem();
	if(!pSystem)
		return false;
	if(FMOD_OK != pSystem->createGeometry(iPtNum*2+2*(iPtNum-2), (iPtNum*2+2*(iPtNum-2))*3, &m_pGeometry))
		return false;

	FMOD_VECTOR vertex[3] = {0};
	int index[3] = {0, 1, 2};
	for (int i=0; i<iPtNum-2; ++i)
	{
		vertex[0].x = pPoints[0].x;
		vertex[0].z = pPoints[0].z;
		vertex[1].x = pPoints[i+1].x;
		vertex[1].z = pPoints[i+1].z;
		vertex[2].x = pPoints[i+2].x;
		vertex[2].z = pPoints[i+2].z;
		vertex[0].y = vertex[1].y = vertex[2].y = fBottom;
		if(FMOD_OK != m_pGeometry->addPolygon(1.0f, 1.0f, true, 3, vertex, index))
			return false;
	}

	for (int i=0; i<iPtNum-2; ++i)
	{
		vertex[0].x = pPoints[0].x;
		vertex[0].z = pPoints[0].z;
		vertex[2].x = pPoints[i+1].x;
		vertex[2].z = pPoints[i+1].z;
		vertex[1].x = pPoints[i+2].x;
		vertex[1].z = pPoints[i+2].z;
		vertex[0].y = vertex[1].y = vertex[2].y = fTop;
		if(FMOD_OK != m_pGeometry->addPolygon(1.0f, 1.0f, true, 3, vertex, index))
			return false;
	}
	for (int i=0; i<iPtNum; ++i)
	{
		vertex[0].x = pPoints[i].x;
		vertex[0].y = fBottom;
		vertex[0].z = pPoints[i].z;
		vertex[1] = vertex[0];
		vertex[1].y = fTop;
		int idx = (i+1)%iPtNum;
		vertex[2].x = pPoints[idx].x;
		vertex[2].y = fTop;
		vertex[2].z = pPoints[idx].z;
		if(FMOD_OK != m_pGeometry->addPolygon(1.0f, 1.0f, true, 3, vertex, index))
			return false;
		vertex[1] = vertex[2];
		vertex[2].x = pPoints[idx].x;
		vertex[2].y = fBottom;
		vertex[2].z = pPoints[idx].z;
		if(FMOD_OK != m_pGeometry->addPolygon(1.0f, 1.0f, true, 3, vertex, index))
			return false;
	}

	FMOD_VECTOR fmod_pos = {0, 0, 0};
	if(FMOD_OK != m_pGeometry->setPosition(&fmod_pos))
		return false;

	return true;
}