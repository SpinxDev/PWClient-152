// A2DDotLine.cpp: implementation of the A2DDotLine class.
//
//////////////////////////////////////////////////////////////////////
#include "AUI.h"
#include "A3DDevice.h"
#include "A3DTexture.h"
#include "A3DMacros.h"

#include "A2DDotLine.h"
#include "A2DDotLineMan.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

A2DDotLine::A2DDotLine(A2DDotLineMan* pMan) :
m_pMan(pMan),
m_pA3DDevice(NULL),
m_pA3DTexture(NULL),
m_nLineWidth(2)
{
}

A2DDotLine::~A2DDotLine()
{

}

void A2DDotLine::Release()
{
	A3DRELEASE(m_pA3DTexture);
	if (m_pMan)
		m_pMan->RemoveLine(this);
}

bool A2DDotLine::Create(A3DDevice* pDevice, const char* szFileName)
{
	Release();
	if (!m_pMan)
		return false;

	m_pA3DDevice = pDevice;
	m_pA3DTexture = new A3DTexture;
	m_pA3DTexture->Init(pDevice);
	if (!m_pA3DTexture->LoadFromFile(pDevice, szFileName, szFileName))
	{
		delete m_pA3DTexture;
		m_pA3DTexture = NULL;
		return false;
	}

	m_pMan->AddLine(this);
	return true;
}

void A2DDotLine::SetLineWidth(int nLineWidth /* = 2 */)
{
	m_nLineWidth = nLineWidth;
}

bool A2DDotLine::DrawLine(const A3DPOINT2& ptFrom, const A3DPOINT2& ptTo, A3DCOLOR color)
{
	if (!m_pMan || !m_pA3DTexture || !m_pA3DDevice)
		return false;

	int dx = ptTo.x - ptFrom.x;
	int dy = ptTo.y - ptFrom.y;
	if ((dx == 0 && dy == 0) 
		|| (m_nLineWidth <= 0) 
		)
		return true;

	float fDis = sqrt((float)dx*dx + (float)dy*dy);
	float fSinT = dy / fDis;
	float fCosT = dx / fDis;
	int nTexW = 0, nTexH = 0;
	if (!m_pA3DTexture->GetDimension(&nTexW, &nTexH))
		return false;
	if (nTexW == 0)
		return true;

	/*
		0 --- 1
		'     '
		'     '
		3 --- 2
	*/
	float fDx = m_nLineWidth * 0.5f * fSinT;
	float fDy = m_nLineWidth * 0.5f * fCosT;
	float x0 = ptFrom.x - fDx;
	float y0 = ptFrom.y + fDy;
	float x3 = ptFrom.x + fDx;
	float y3 = ptFrom.y - fDy;	
	float x1 = ptTo.x - fDx;
	float y1 = ptTo.y + fDy;
	float x2 = ptTo.x + fDx;
	float y2 = ptTo.y - fDy;
	
	A3DCOLOR specular = A3DCOLORRGBA(0, 0, 0, 255);
	m_aVtxBuffer.push_back(A3DTLVERTEX(A3DVECTOR4(x0, y0, 0, 1), color, specular, 0,	0));
	m_aVtxBuffer.push_back(A3DTLVERTEX(A3DVECTOR4(x1, y1, 0, 1), color, specular, fDis/nTexW, 0));
	m_aVtxBuffer.push_back(A3DTLVERTEX(A3DVECTOR4(x2, y2, 0, 1), color, specular, fDis/nTexW, 1));
	m_aVtxBuffer.push_back(A3DTLVERTEX(A3DVECTOR4(x3, y3, 0, 1), color, specular, 0,	1));

	return true;
}

void A2DDotLine::_PresetRenderState()
{
	if (!m_pA3DDevice)
		return;
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetLightingEnable(false);
}

void A2DDotLine::_PostSetRenderState()
{
	if (!m_pA3DDevice)
		return;
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
}

void A2DDotLine::Flush()
{
	if (!m_pMan || !m_pA3DTexture || !m_pA3DDevice)
		return;

	_PresetRenderState();
	m_pA3DTexture->Appear(0);

	m_pMan->RenderStream(sizeof(A3DTLVERTEX), m_aVtxBuffer.size() >> 2, m_aVtxBuffer.begin());

	m_pA3DTexture->Disappear(0);
	_PostSetRenderState();

	m_aVtxBuffer.clear();
}