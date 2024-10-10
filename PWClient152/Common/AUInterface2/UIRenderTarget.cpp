/*
* FILE: UIRenderTarget.cpp
*
* DESCRIPTION: Class for respresenting the terrain in A3D Engine
*
* CREATED BY: Liyi, 2009/8/24
*
* HISTORY:
*
* Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
*/

#include "AUI.h"

#include "UIRenderTarget.h"
#include "A3DTexture.h"
#include "A3DDevice.h"
#include "A3DVertex.h"
#include "A3DStream.h"
#include "A3DSurface.h"
#include "A3DShaderMan.h"
#include "A3DPixelShader.h"
#include "A3DRenderTarget.h"
#include "A3DMacros.h"

//-------------------------------------------------------------------------------------------------
UIRenderTarget::UIRenderTarget()
{
	m_pA3DDevice	= NULL;
	m_pRenderTarget = NULL;
	m_pAlphaRT		= NULL;
	m_pCopyAlphaShader = NULL;
	m_pStream		= NULL;
	m_nWidth		= 0;
	m_nHeight		= 0;
	m_bNeedRender	= true;
}

//-------------------------------------------------------------------------------------------------
UIRenderTarget::~UIRenderTarget()
{
	Release();
}

bool UIRenderTarget::Init(A3DDevice* pA3DDevice, int nWidth, int nHeight)
{
	m_pA3DDevice = pA3DDevice;
	m_nHeight = nHeight;
	m_nWidth = nWidth;

	if( !CreateRenderTarget())
		return false;
	if( !CreateStream())
		return false;

	m_pCopyAlphaShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADERS_ROOT_DIR"ps\\copyalpha.txt", false);
	if( m_pCopyAlphaShader == NULL)
		return false;
	
	if(!pA3DDevice->AddUnmanagedDevObject(this))
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------
void UIRenderTarget::Release()
{
	if( m_pA3DDevice != NULL)
	{
		m_pA3DDevice->RemoveUnmanagedDevObject(this);
	}
	A3DRELEASE(m_pRenderTarget);
	A3DRELEASE(m_pAlphaRT);
	A3DRELEASE(m_pStream);
	
	if( m_pCopyAlphaShader != NULL)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pCopyAlphaShader);
		m_pCopyAlphaShader = NULL;
	}
}


//-------------------------------------------------------------------------------------------------
bool UIRenderTarget::CreateRenderTarget()
{	
#ifdef _ANGELICA2
	A3DDEVFMT devFmt;
	devFmt.bWindowed	= true;
	devFmt.nWidth		= m_nWidth;
	devFmt.nHeight		= m_nHeight;
	devFmt.fmtTarget	= A3DFMT_A8R8G8B8;
	devFmt.fmtDepth		= A3DFMT_D16;

	// if color surface is bigger than device's restore depth surface, then create a new depth surface
	bool bNewDepthSurface = false;
	if (m_pA3DDevice->GetRestoreToDepthBuffer())
	{
		D3DSURFACE_DESC desc;
		m_pA3DDevice->GetRestoreToDepthBuffer()->GetDesc(&desc);
		if (desc.Width < m_nWidth || desc.Height < m_nHeight)
			bNewDepthSurface = true;
	}

	m_pRenderTarget = new A3DRenderTarget;
	if( m_pRenderTarget == NULL)
		return false;

	if(!m_pRenderTarget->Init( m_pA3DDevice, devFmt, true, bNewDepthSurface))
		return false;

	m_pAlphaRT = new A3DRenderTarget;
	if( m_pAlphaRT == NULL)
		return false;

	if( !m_pAlphaRT->Init(m_pA3DDevice, devFmt, true, bNewDepthSurface))
	{
		return false;
	}

#else
	A3DRenderTarget::RTFMT rtFmt;
	rtFmt.iWidth	= m_nWidth;
	rtFmt.iHeight	= m_nHeight;
	rtFmt.fmtTarget = A3DFMT_A8R8G8B8;
	rtFmt.fmtDepth	= A3DFMT_D16;

	m_pRenderTarget = new A3DRenderTarget;
	if( m_pRenderTarget == NULL)
		return false;

	if(!m_pRenderTarget->Init(m_pA3DDevice, rtFmt, true, true))
		return false;

	m_pAlphaRT = new A3DRenderTarget;
	if( m_pAlphaRT == NULL)
		return false;

	if( !m_pAlphaRT->Init(m_pA3DDevice, rtFmt, true, true))
	{
		return false;
	}
#endif
 
	return true;
}

//-------------------------------------------------------------------------------------------------
bool UIRenderTarget::CreateStream()
{
	m_pStream = new A3DStream;
	if( m_pStream == NULL)
		return false;

	if( !m_pStream->Init(m_pA3DDevice, A3DVT_TLVERTEX, 4, 6, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC))
		return false;

	//set indices
	static WORD index[] = {0, 1, 2, 2, 1, 3};
	m_pStream->SetIndices(index, 6);

	return true;
}

//-------------------------------------------------------------------------------------------------
void UIRenderTarget::FillStream(int nX, int nY)
{
	A3DCOLOR	color = A3DCOLORRGBA(255, 255, 255, 255);
	A3DCOLOR	specular = A3DCOLORRGBA(0, 0, 0, 255);
	A3DTLVERTEX verts[4];
	float		l, t, r, b;
	
	l = (float) nX; 
	t = (float) nY;
	r = l + (float) m_nWidth;
	b = t + (float) m_nHeight;
	
	verts[0] = A3DTLVERTEX(A3DVECTOR4(l- 0.5f, t- 0.5f, 0.0f, 1.0f), color, specular, 0.0f, 0.0f);
	verts[1] = A3DTLVERTEX(A3DVECTOR4(r- 0.5f, t- 0.5f, 0.0f, 1.0f), color, specular, 1.0f, 0.0f);
	verts[2] = A3DTLVERTEX(A3DVECTOR4(l- 0.5f, b- 0.5f, 0.0f, 1.0f), color, specular, 0.0f, 1.0f);
	verts[3] = A3DTLVERTEX(A3DVECTOR4(r- 0.5f, b- 0.5f, 0.0f, 1.0f), color, specular, 1.0f, 1.0f);
	
	m_pStream->SetVerts((LPBYTE) verts, 4);
	return;
}

//-------------------------------------------------------------------------------------------------
bool UIRenderTarget::Render(int nX, int nY)
{
	if( m_pA3DDevice == NULL)
		return false;

	FillStream(nX, nY);
	m_pStream->Appear();

	m_pRenderTarget->AppearAsTexture(0);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);

	//Set render state
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_SRCALPHA);
	
	//Draw
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2) )
	{
		if(AUI_IsLogEnabled())
		{		
			a_LogOutput(1, "UIRenderTarget::Render() Failed to DrawIndexedPrimitive!");
		}
		return false;
	}

	//Restore render state
	m_pRenderTarget->DisappearAsTexture(0);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	
	return true;
}

//-------------------------------------------------------------------------------------------------
bool UIRenderTarget::BeginRenderRTAlpha()
{
	if( m_pAlphaRT == NULL || m_pA3DDevice == NULL)
		return false;
	
	
	//Set render target
#ifdef _ANGELICA2
	if(!m_pA3DDevice->SetRenderTarget(m_pAlphaRT))
		return false;
#else
	if(!m_pAlphaRT->ApplyToDevice())
		return false;
#endif
	
	//clear
	m_pA3DDevice->Clear(D3DCLEAR_TARGET , 0xFFFFFFFF, 1.0f, 0);
	
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	
	m_pA3DDevice->SetEnableSetAlphaBlend(true);
	
	//Set render target
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_ZERO);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	
	
	m_pA3DDevice->SetEnableSetAlphaBlend(false);
	
	return true;
}

//-------------------------------------------------------------------------------------------------
bool UIRenderTarget::BeginRenderRT()
{
	if( m_pRenderTarget == NULL || m_pA3DDevice == NULL)
		return false;

#ifdef _ANGELICA2
	//restore render target
	m_pA3DDevice->RestoreRenderTarget();
	//Set render target
	if(!m_pA3DDevice->SetRenderTarget(m_pRenderTarget))
		return false;
#else
	m_pAlphaRT->WithdrawFromDevice();
	if (!m_pRenderTarget->ApplyToDevice())
		return false;
#endif


	
	if(!CopyRTR2A())
		return false;


	m_pA3DDevice->SetEnableSetAlphaBlend(true);

	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	
	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);

	m_pA3DDevice->SetEnableSetAlphaBlend(false);

	return true;
}

//-------------------------------------------------------------------------------------------------
bool UIRenderTarget::EndRenderRT()
{
	m_pA3DDevice->SetEnableSetAlphaBlend(true);
	
	//restore render target
#ifdef _ANGELICA2
	m_pA3DDevice->RestoreRenderTarget();
#else
	m_pRenderTarget->WithdrawFromDevice();
#endif
	
	//restore render state
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
	
	return true;
}

bool UIRenderTarget::CopyRTR2A()
{

	m_pAlphaRT->AppearAsTexture(0);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);

	FillStream(0, 0);
	m_pStream->Appear();
	m_pCopyAlphaShader->Appear();

	//set render state
	m_pA3DDevice->SetEnableSetAlphaBlend(true);
	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->SetEnableSetAlphaBlend(false);

	//Draw
	if( !m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2) )
	{
		if(AUI_IsLogEnabled())
		{
			a_LogOutput(1, "UIRenderTarget::CopyRTR2A() Failed to DrawIndexedPrimitive!");
		}
		return false;
	}
	
	//Restore 
	m_pCopyAlphaShader->Disappear();
	m_pAlphaRT->DisappearAsTexture(0);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	return true;
}
bool UIRenderTarget::BeforeDeviceReset()
{
	m_bNeedRender = false;
	return true;
}

bool UIRenderTarget::AfterDeviceReset()
{
	m_bNeedRender = true;
	return true;
}

bool UIRenderTarget::NeedRender()
{
	return m_bNeedRender;
}

void UIRenderTarget::SetNeedRender(bool bNeedRender)
{
	m_bNeedRender = bNeedRender;
}

void UIRenderTarget::SaveRenderTarget(const char* szFileName)
{
	D3DXSaveTextureToFileA(szFileName, D3DXIFF_DDS, m_pRenderTarget->GetTargetTexture(), NULL);
}