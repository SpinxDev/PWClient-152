/*
 * FILE: A3DCubeRenderTarget.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2010/5/31
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
 */

#include "A3DCubeRenderTarget.h"
#include "A3DPI.h"
#include "A3DMatrix.h"
#include "A3DDevice.h"
#include "A3DMacros.h"
#include "A3DRenderTargetMan.h"
#include "A3DEngine.h"
#include "A3DDDSFile.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DCubeRenderTarget
//	
///////////////////////////////////////////////////////////////////////////

A3DCubeRenderTarget::A3DCubeRenderTarget() :
m_pA3DDevice(NULL),
m_pCubeTex(NULL),
m_iEdgeSize(0),
m_fmtColor(A3DFMT_UNKNOWN),
m_dwRentTime(0),
m_idAppied(-1)
{
}

A3DCubeRenderTarget::~A3DCubeRenderTarget()
{
}

//	Initialize object.
bool A3DCubeRenderTarget::Init(A3DDevice* pA3DDevice, int iEdgeSize, A3DFORMAT fmtColor)
{
	m_pA3DDevice	= pA3DDevice;
	m_iEdgeSize		= iEdgeSize;
	m_fmtColor		= fmtColor;

	if (!CreatePlains())
	{
		g_A3DErrLog.Log("A3DCubeRenderTarget::Init, failed to create plains");
		return false;
	}

	m_pA3DDevice->AddUnmanagedDevObject(this);
	
	

	return true;
}

//	Release object
void A3DCubeRenderTarget::Release()
{
	m_pA3DDevice->RemoveUnmanagedDevObject(this);

	//	Release planes
	ReleasePlains();
}

//	Create buffer plain
bool A3DCubeRenderTarget::CreatePlains()
{
	ASSERT(!m_pCubeTex);

	if (FAILED(m_pA3DDevice->GetD3DDevice()->CreateCubeTexture(m_iEdgeSize, 1,
              D3DUSAGE_RENDERTARGET, (D3DFORMAT)m_fmtColor, D3DPOOL_DEFAULT, &m_pCubeTex, NULL)))
	{
		g_A3DErrLog.Log("A3DCubeRenderTarget::CreatePlains(), Fail to create format[%d], edge[%d] cube texture", m_fmtColor, m_iEdgeSize);
		return false;
	}

	//	Get face surfaces
	for (int i=0; i < 6; i++)
	{
		m_pCubeTex->GetCubeMapSurface((D3DCUBEMAP_FACES)i, 0, &m_aSurfs[i].m_pD3DSurface);
	}

	m_pA3DDevice->GetRenderTargetMan()
		->IncRenderTargetSize(GetRenderTargetSize());

	return true;
}

//	Release buffer plain
void A3DCubeRenderTarget::ReleasePlains()
{
	
	//	Appied target shouldn't be released
	if (m_idAppied != -1)
	{
		ASSERT(m_idAppied == -1);
		return;
	}

	m_pA3DDevice->GetRenderTargetMan()
		->IncRenderTargetSize(-GetRenderTargetSize());


	int i;
	if(m_pA3DDevice->GetNeedResetFlag())
	{
		for (i=0; i < 6; i++)
		{
			A3DINTERFACERELEASE(m_aSurfs[i].m_pD3DSurface);	
		}
		A3DINTERFACERELEASE(m_pCubeTex);
	}
	else
	{
		for (i=0; i < 6; i++)
		{
			//multithread render
			if( m_aSurfs[i].m_pD3DSurface) 
			{
				m_pA3DDevice->ReleaseResource(m_aSurfs[i].m_pD3DSurface);
				m_aSurfs[i].m_pD3DSurface = NULL;
			}
		}
		//multithread render
		if( m_pCubeTex)
		{
			m_pA3DDevice->ReleaseResource(m_pCubeTex);
			m_pCubeTex = NULL;
		}
	}
}

//	Get face surfaces
A3DSurfaceProxy* A3DCubeRenderTarget::GetFaceSurface(int n)
{
	if (n >= 0 && n < 6)
		return &m_aSurfs[n];
	else
	{
		ASSERT(n >= 0 && n < 6);
		return NULL;
	}
}

//	Before device reset
bool A3DCubeRenderTarget::BeforeDeviceReset()
{
	ReleasePlains();
	return true;
}

//	After device reset
bool A3DCubeRenderTarget::AfterDeviceReset()
{
	CreatePlains();
	return true;
}

//	Apply as texture
bool A3DCubeRenderTarget::AppearAsTexture(int iLayer)
{
	if (!m_pA3DDevice || !m_pCubeTex)
		return false;

	m_pA3DDevice->raw_SetTexture(iLayer, m_pCubeTex);
	return true;
}

//	Apply view to device
bool A3DCubeRenderTarget::ApplyToDevice(int iFace, int iRTIndex/* 0 */)
{
	if (iFace < 0 || iFace >= 6)
	{
		ASSERT(iFace >= 0 && iFace < 6);
		return false;
	}

	//	Apply twice ?
	if (m_idAppied != -1)
	{
		ASSERT(m_idAppied == -1);
		return false;
	}

	A3DRenderTargetMan* pRTMan = m_pA3DDevice->GetRenderTargetMan();
	if (!pRTMan->PushRenderTarget(iRTIndex, &m_aSurfs[iFace], &m_DummySurf))
		return false;
	
	int iStackDepth = pRTMan->GetRTStackDepth(iRTIndex);
	m_idAppied = (iRTIndex << 16) | iStackDepth;

	return true;
}

//	Withdraw view from device
void A3DCubeRenderTarget::WithdrawFromDevice()
{
	if (m_idAppied != -1)
	{
		A3DRenderTargetMan* pRTMan = m_pA3DDevice->GetRenderTargetMan();
		int iStackDepth = m_idAppied & 0xffff;
		int iRTIndex = m_idAppied >> 16;

		if (pRTMan->GetRTStackDepth(iRTIndex) != iStackDepth)
		{
			ASSERT(0 && "Render target stack wrong !");
			return;
		}

		pRTMan->PopRenderTarget(iRTIndex);
		m_idAppied = -1;
	}
	else
	{
		ASSERT(m_idAppied != -1);
	}
}

int A3DCubeRenderTarget::GetRenderTargetSize()
{
	return (A3DDDSFile::BitsPerPixel((D3DFORMAT)m_fmtColor) * 
		m_iEdgeSize * m_iEdgeSize * 6) / 8;
}

