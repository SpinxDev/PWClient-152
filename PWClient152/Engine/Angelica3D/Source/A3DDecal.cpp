/*
 * FILE: A3DDecal.cpp
 *
 * DESCRIPTION: Routines for decal
 *
 * CREATED BY: duyuxin, 2001/11/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DDecal.h"
#include "A3DPI.h"
#include "A3DViewport.h"
#include "A3DVertexCollector.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DStream.h"
#include "A3DTexture.h"
#include "A3DTextureMan.h"
#include "A3DConfig.h"
#include "A3DGFXMan.h"
#include "A3DCameraBase.h"
#include "A3DCDS.h"
#include "AAssist.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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
//	Implement
//
///////////////////////////////////////////////////////////////////////////

A3DDecal::A3DDecal()
{
	m_dwClassID		= A3D_CID_DECAL;
	m_pA3DDevice	= NULL;
	m_pA3DStream	= NULL;
	m_pTexture		= NULL;
	m_iType			= 0;
	m_bVisible		= false;
	m_fRotateDeg	= 0.0f;
	m_hTexInfo		= 0;
	m_bOptimized	= false;
	m_fMinWidth		= 0.0f;
	m_fMinHeight	= 0.0f;
	m_fMaxWidth		= 99999.0f;
	m_fMaxHeight	= 99999.0f;

	m_fWidth		= 1.0f;
	m_fHeight		= 1.0f;
	m_fTexU			= 0.0f;
	m_fTexV			= 0.0f;
	m_fTexWid		= 1.0f;
	m_fTexHei		= 1.0f;
	m_vCenter		= A3DVECTOR3(0.0f);

	memset(m_aColors, 0xff, sizeof (m_aColors));

	m_aIndices[0]	= 0;
	m_aIndices[1]	= 1;
	m_aIndices[2]	= 2;
	m_aIndices[3]	= 1;
	m_aIndices[4]	= 3;
	m_aIndices[5]	= 2;

	m_Shader.SrcBlend	= A3DBLEND_SRCCOLOR;
	m_Shader.DestBlend	= A3DBLEND_ONE;
	m_matParentTM.Identity();
}

A3DDecal::~A3DDecal()
{
	m_pA3DDevice = NULL;
}

/*	Initialize object.

	Return true for success, otherwise return false.

	iType: decal type, defined in A3DDecal.h
	pDevice: A3D object's address.
	szTexName: decal texture, NULL means no texture
	bForceRender: true, force to use decal itself render routine, but not vertex collector
				  false, use which render is depending on g_pA3DConfig->GetRunEnv()
*/
bool A3DDecal::Init(int iType, A3DDevice* pDevice, char* szTexName, bool bForceRender/* false */)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	if( !pDevice )
		return true;

	//	Load texture
	if (szTexName)
	{
		m_strTextureMap = szTexName;
		if( !pDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTexName, pDevice->GetA3DEngine()->GetA3DGFXMan()->GetTextureFolder(), &m_pTexture, A3DTF_MIPLEVEL, 1) )
			return false;
	}

	if (bForceRender || g_pA3DConfig->GetRunEnv() == A3DRUNENV_GFXEDITOR)
		m_bOptimized = false;
	else
		m_bOptimized = pDevice->GetA3DEngine()->IsOptimizedEngine();

	if (!m_bOptimized)
	{
		//	Create vertex and index stream
		if (!(m_pA3DStream = new A3DStream))
		{
			g_A3DErrLog.Log("A3DDecal::Init() not enough memory for stream!");
			return false;
		}
		
		if (!m_pA3DStream->Init(pDevice, A3DVT_TLVERTEX, 4, 6, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC))
		{
			g_A3DErrLog.Log("A3DDecal::Init(), Failed to initialize stream!");
			return false;
		}
		
		//	fill indices
		WORD* pIndices;
		if (!m_pA3DStream->LockIndexBuffer(0, 0, (BYTE**)&pIndices, 0))
		{
			g_A3DErrLog.Log("A3DDecal::Init(), Failed to lock index buffer!");
			return false;
		}
		
		pIndices[0] = 0;
		pIndices[1] = 1;
		pIndices[2] = 2;
		pIndices[3] = 1;
		pIndices[4] = 3;
		pIndices[5] = 2;
		
		if (!m_pA3DStream->UnlockIndexBuffer())
		{
			g_A3DErrLog.Log("A3DDecal::Init(), Failed to unlock index buffer!");
			return false;
		}
	}
	else
	{
		A3DVertexCollector* pCollector = pDevice->GetA3DEngine()->GetVertexCollector();
		m_hTexInfo = pCollector->RegisterTexture(A3DVT_TLVERTEX, m_pTexture, NULL, &m_Shader);
	}

	m_vCenter	 = A3DVECTOR3(0.0f);
	m_pA3DDevice = pDevice;
	m_iType		 = iType;

	return true;
}

//	Release all resources
void A3DDecal::Release()
{
	if (m_pA3DStream)
	{
		m_pA3DStream->Release();
		delete m_pA3DStream;
		m_pA3DStream = NULL;
	}

	if (m_pTexture)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexture);
		m_pTexture = NULL;
	}

	m_pA3DDevice = NULL;
}

/*	Change decal's texture. If this operation failed, original texture will be remained

	Return true for success, otherwise return false.

	szTexName: texture file's name.
*/
bool A3DDecal::ChangeTexture(char* szTexName)
{
	if( !m_pA3DDevice )	return true;

	m_strTextureMap = szTexName;
	if (!m_pA3DDevice) 
		return true;

	// Release old texture
	if (m_pTexture)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexture);
		m_pTexture = NULL;
	}
	
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTexName, m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->GetTextureFolder(), &m_pTexture, A3DTF_MIPLEVEL, 1) )
		return false;
	
	if (m_bOptimized)
	{
		A3DVertexCollector* pCollector = m_pA3DDevice->GetA3DEngine()->GetVertexCollector();
		m_hTexInfo = pCollector->RegisterTexture(A3DVT_TLVERTEX, m_pTexture, NULL, &m_Shader);
	}

	return true;
}

/*	Update decal with specified viewport.

	Return true for success, otherwise return false.
	
	pView: specified viewport.
*/
bool A3DDecal::Update(A3DViewport* pView)
{
	if (!m_pA3DDevice)
		return true;

	A3DCameraBase* pCamera = pView->GetCamera();
	A3DVECTOR3 vCenter;
	float fWidth, fHeight;

	m_bVisible = false;

	A3DVIEWPORTPARAM* pViewPara = pView->GetParam();

	//	Set all vertices position on screen
	if (m_iType == DECALTYPE_PURE3D)
	{
		A3DVECTOR3 vUp	  = pCamera->GetUp();
		A3DVECTOR3 vRight = pCamera->GetRight();
		A3DVECTOR3 vExts  = m_vCenter + vRight * m_fWidth + vUp * m_fHeight;

		//	Transfrom center from world to screen
		if (pView->Transform(m_vCenter, vCenter))
			return true;	//	Be clipped

		pView->Transform(vExts, vExts);

		fWidth	= vExts.x - vCenter.x;
		fHeight	= vCenter.y - vExts.y;

		a_Clamp(fWidth, m_fMinWidth, m_fMaxWidth);
		a_Clamp(fHeight, m_fMinHeight, m_fMaxHeight);
	}
	else if (m_iType == DECALTYPE_CENTER3D)
	{
		//	Transfrom center from world to screen
		if (pView->Transform(m_vCenter, vCenter))
			return true;	//	Be clipped

		vCenter.z	= 0.0f;
		fWidth		= m_fWidth;
		fHeight		= m_fHeight;
	}
	else	//	m_iType == DECALTYPE_PURE2D
	{
		vCenter.x	= m_vCenter.x + pViewPara->X;
		vCenter.y	= m_vCenter.y + pViewPara->Y;
		vCenter.z	= 0.0f;
		fWidth		= m_fWidth;
		fHeight		= m_fHeight;
	}

	//	If decal is out of screen, don't render it
	if (vCenter.x < pViewPara->X - fWidth || 
		vCenter.x >= pViewPara->X + pViewPara->Width + fWidth ||
		vCenter.y < pViewPara->Y - fHeight ||
		vCenter.y >= pViewPara->Y + pViewPara->Height + fHeight)
		return true;

	//	Set vertices' position on screen
	A3DVECTOR3 aVerts[4];
	aVerts[0] = A3DVECTOR3(-fWidth, -fHeight, vCenter.z);
	aVerts[1] = A3DVECTOR3(fWidth, -fHeight, vCenter.z);
	aVerts[2] = A3DVECTOR3(-fWidth, fHeight, vCenter.z);
	aVerts[3] = A3DVECTOR3(fWidth, fHeight, vCenter.z);
	
	if (m_fRotateDeg)	//	Need rotating ?
	{
		float fRadian = DEG2RAD(m_fRotateDeg);

		float fSin = (float)sin(fRadian);
		float fCos = (float)cos(fRadian);

		for (int i=0; i < 4; i++)
		{
			m_aVerts[i].x = vCenter.x + aVerts[i].x * fCos + aVerts[i].y * fSin;
			m_aVerts[i].y = vCenter.y + aVerts[i].y * fCos - aVerts[i].x * fSin;
			m_aVerts[i].z = aVerts[i].z;
		}
	}
	else
	{
		for (int i=0; i < 4; i++)
		{
			m_aVerts[i].x = vCenter.x + aVerts[i].x;
			m_aVerts[i].y = vCenter.y + aVerts[i].y;
			m_aVerts[i].z = aVerts[i].z;
		}
	}

	A3DCOLOR Specular = A3DCOLORRGBA(0, 0, 0, 255);

	//	Engine not use vertex collector
	if (!m_bOptimized)
	{
		A3DTLVERTEX* pVerts;
		if (!m_pA3DStream->LockVertexBufferDynamic(0, 0, (BYTE**)&pVerts, 0))
			return false;

		//	Left-up vertex
		pVerts[0].x			= m_aVerts[0].x;
		pVerts[0].y			= m_aVerts[0].y;
		pVerts[0].z			= m_aVerts[0].z;
		pVerts[0].rhw		= 1.0f;
		pVerts[0].tu		= m_fTexU;
		pVerts[0].tv		= m_fTexV;
		pVerts[0].diffuse	= m_aColors[0];
		pVerts[0].specular	= Specular;
		
		//	Right-up vertex
		pVerts[1].x			= m_aVerts[1].x;
		pVerts[1].y			= m_aVerts[1].y;
		pVerts[1].z			= m_aVerts[1].z;
		pVerts[1].rhw		= 1.0f;
		pVerts[1].tu		= m_fTexU + m_fTexWid;
		pVerts[1].tv		= m_fTexV;
		pVerts[1].diffuse	= m_aColors[1];
		pVerts[1].specular	= Specular;
		
		//	Left-bottom vertex
		pVerts[2].x			= m_aVerts[2].x;
		pVerts[2].y			= m_aVerts[2].y;
		pVerts[2].z			= m_aVerts[2].z;
		pVerts[2].rhw		= 1.0f;
		pVerts[2].tu		= m_fTexU;
		pVerts[2].tv		= m_fTexV + m_fTexHei;
		pVerts[2].diffuse	= m_aColors[2];
		pVerts[2].specular	= Specular;
		
		//	Right-bottom vertex
		pVerts[3].x			= m_aVerts[3].x;
		pVerts[3].y			= m_aVerts[3].y;
		pVerts[3].z			= m_aVerts[3].z;
		pVerts[3].rhw		= 1.0f;
		pVerts[3].tu		= m_fTexU + m_fTexWid;
		pVerts[3].tv		= m_fTexV + m_fTexHei;
		pVerts[3].diffuse	= m_aColors[3];
		pVerts[3].specular	= Specular;
		
		if (!m_pA3DStream->UnlockVertexBufferDynamic())
			return false;
	}
	else	//	Use vertex collector
	{
		//	Left-up vertex
		m_aVerts[0].rhw		= 1.0f;
		m_aVerts[0].tu		= m_fTexU;
		m_aVerts[0].tv		= m_fTexV;
		m_aVerts[0].diffuse	= m_aColors[0];
		m_aVerts[0].specular= Specular;
		
		//	Right-up vertex
		m_aVerts[1].rhw		= 1.0f;
		m_aVerts[1].tu		= m_fTexU + m_fTexWid;
		m_aVerts[1].tv		= m_fTexV;
		m_aVerts[1].diffuse	= m_aColors[1];
		m_aVerts[1].specular= Specular;
		
		//	Left-bottom vertex
		m_aVerts[2].rhw		= 1.0f;
		m_aVerts[2].tu		= m_fTexU;
		m_aVerts[2].tv		= m_fTexV + m_fTexHei;
		m_aVerts[2].diffuse	= m_aColors[2];
		m_aVerts[2].specular= Specular;
		
		//	Right-bottom vertex
		m_aVerts[3].rhw		= 1.0f;
		m_aVerts[3].tu		= m_fTexU + m_fTexWid;
		m_aVerts[3].tv		= m_fTexV + m_fTexHei;
		m_aVerts[3].diffuse	= m_aColors[3];
		m_aVerts[3].specular= Specular;
	}

	m_bVisible = true;

	return true;
}

//	Render decal
bool A3DDecal::Render(A3DViewport* pView, bool bCheckVis/* true */)
{
	if (!m_pA3DDevice)
		return true;

	if (bCheckVis && (m_iType == DECALTYPE_PURE3D || m_iType == DECALTYPE_CENTER3D))
	{
		//	Check whether decal is visible
		A3DCDS* pCDS = m_pA3DDevice->GetA3DEngine()->GetA3DCDS();
		if (pCDS)
		{
			if (!pCDS->PosIsVisible(m_vCenter, VISOBJ_PARTICAL, 0))
				return true;
		}
	}

	Update(pView);

	if (!m_bVisible)
		return true;

	if (!m_bOptimized)
	{
		m_pA3DDevice->SetZWriteEnable(false);
		m_pA3DDevice->SetSourceAlpha(m_Shader.SrcBlend);
		m_pA3DDevice->SetDestAlpha(m_Shader.DestBlend);
		
		if (m_pTexture)
			m_pTexture->Appear(0);
		else
			m_pA3DDevice->ClearTexture(0);
		
		m_pA3DStream->Appear();
		
		if (!m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2))
			return false;
		
		if (m_pTexture)
			m_pTexture->Disappear(0);
		
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
		m_pA3DDevice->SetZWriteEnable(true);
	}
	else
	{
		A3DVertexCollector* pCollector = m_pA3DDevice->GetA3DEngine()->GetVertexCollector();
		if (!pCollector->PushVertices(m_hTexInfo, A3DVT_TLVERTEX, m_aVerts, 4, m_aIndices, 6))
			return false;
	}

	return true;
}

void A3DDecal::UpdateParentTM(const A3DMATRIX4& matParentTM)
{
	m_matParentTM = matParentTM;
	return;
}

