/*
 * FILE: A3DFlash.cpp
 *
 * DESCRIPTION: An electric flash between two point, 
				from the start point rushing to the end point and have enough detail random
				when it running
 *
 * CREATED BY: Hedi, 2002/10/18
 *
 * HISTORY:
 *
 * Copyright (c) 2002 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DFlash.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DFuncs.h"
#include "A3DTextureMan.h"
#include "A3DViewport.h"
#include "A3DCamera.h"
#include "A3DTexture.h"
#include "A3DDevice.h"
#include "A3DMathUtility.h"
#include "APerlinNoise1D.h"
#include "AMemory.h"
#include "AAssist.h"

#define new A_DEBUG_NEW

A3DFlash::A3DFlash()
{
	m_pA3DDevice	= NULL;

	m_pVecStrikePath= NULL;
	m_pVertexBuffer = NULL;
	m_pIndexBuffer  = NULL;

	m_pPerlinNoise  = NULL;
	m_pTexture		= NULL;

	m_color			= 0xffffffff;
}

A3DFlash::~A3DFlash()
{
}

bool A3DFlash::Init(A3DDevice * pDevice, APerlinNoise1D * pNoise, float vSize, int nSteps, A3DCOLOR color)
{
	m_pA3DDevice		= pDevice;
	m_pPerlinNoise		= pNoise;
	m_nNumSegments		= nSteps;
	m_vSize				= vSize;
	m_color				= color;

	if( m_nNumSegments <= 0 )
	{
		g_A3DErrLog.Log("A3DFlash::Init(), invalid segments number [%d] for this flash!", m_nNumSegments);
		return false;
	}

	if( m_nNumSegments > A3DFLASH_MAX_SEGEMENT )
	{
		g_A3DErrLog.Log("A3DFlash::Init(), Too many segments [%d] for this flash!", m_nNumSegments);
		return false;
	}

	m_pVecStrikePath = (A3DVECTOR3 *)a_malloc((m_nNumSegments + 1) * sizeof(A3DVECTOR3));
	if( NULL == m_pVecStrikePath )
	{
		g_A3DErrLog.Log("A3DFlash::Init(), Not enough memory!");
		return false;
	}

	m_pVertexBuffer = (A3DLVERTEX *)a_malloc(m_nNumSegments * 4 * sizeof(A3DLVERTEX));
	if( NULL == m_pVertexBuffer )
	{
		g_A3DErrLog.Log("A3DFlash::Init(), Not enough memory!");
		return false;
	}

	m_pIndexBuffer = (WORD *)a_malloc(m_nNumSegments * 6 * sizeof(WORD));
	if( NULL == m_pIndexBuffer )
	{
		g_A3DErrLog.Log("A3DFlash::Init(), Not enough memory!");
		return false;
	}

	int nStartVert = 0;
	int nStartIndex = 0;
	for(int i=0; i<m_nNumSegments; i++)
	{
		m_pIndexBuffer[nStartIndex + 0] = nStartVert + 0;
		m_pIndexBuffer[nStartIndex + 1] = nStartVert + 1;
		m_pIndexBuffer[nStartIndex + 2] = nStartVert + 2;
		m_pIndexBuffer[nStartIndex + 3] = nStartVert + 1;
		m_pIndexBuffer[nStartIndex + 4] = nStartVert + 3;
		m_pIndexBuffer[nStartIndex + 5] = nStartVert + 2;
		nStartVert += 4;
		nStartIndex += 6;
	}

	return true;
}

bool A3DFlash::Release()
{
	if( m_pVertexBuffer )
	{
		a_free(m_pVertexBuffer);
		m_pVertexBuffer = NULL;
	}

	if( m_pIndexBuffer )
	{
		a_free(m_pIndexBuffer);
		m_pIndexBuffer = NULL;
	}

	if( m_pVecStrikePath )
	{
		a_free(m_pVecStrikePath);
		m_pVecStrikePath = NULL;
	}

	if( m_pTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexture);
		m_pTexture = NULL;		
	}

	return true;
}

//
//							  end
//	 --------			   0--------1  .
//	 |		|			  /		  /	  / \
//	 |	BMP	|	   =>	 /		 /	   |
//	 |		|			/		/
//	 |		|		   /	   /
//	 --------		 2---------3
//						start
bool A3DFlash::Render(A3DViewport * pCurrentViewport)
{
	A3DVECTOR3 vecCamDir = pCurrentViewport->GetCamera()->GetDir();

	// first build the strike mesh
	A3DVECTOR3 vecAxis, vecP;
	A3DVECTOR3 vecLastPos = m_pVecStrikePath[0];
	A3DVECTOR3 vecPos;
	A3DLVERTEX * pVerts = m_pVertexBuffer;
	
	for(int i=0; i<m_nNumSegments; i++)
	{
		vecPos		= m_pVecStrikePath[i + 1];
		vecAxis		= vecPos - vecLastPos;
		
		vecP = Normalize(CrossProduct(vecCamDir, vecAxis)) * m_vSize;
		
		pVerts[0] = A3DLVERTEX(vecPos - vecP, m_color, 0xff000000, 0.0f, 0.0f);
		pVerts[1] = A3DLVERTEX(vecPos + vecP, m_color, 0xff000000, 1.0f, 0.0f);
		pVerts[2] = A3DLVERTEX(vecLastPos - vecP, m_color, 0xff000000, 0.0f, 1.0f);
		pVerts[3] = A3DLVERTEX(vecLastPos + vecP, m_color, 0xff000000, 1.0f, 1.0f);
		
		pVerts += 4;
		vecLastPos	= vecPos;
	}

	// now, render it to screen;
	m_pA3DDevice->SetFVF(A3DFVF_A3DLVERTEX);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_ONE);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetFogEnable(false);
	//m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());

	if( m_pTexture )
		m_pTexture->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);

	m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, m_nNumSegments * 4, m_nNumSegments * 2, m_pIndexBuffer, A3DFMT_INDEX16, m_pVertexBuffer, sizeof(A3DLVERTEX));
	
	m_pA3DDevice->SetZWriteEnable(true);
	//m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetFogEnable(true);
	m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	
	return true;
}

bool A3DFlash::TickAnimation()
{
	return true;
}

bool A3DFlash::SetTextureMap(char * szTextureMap)
{
	if( m_pTexture )
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexture);

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTextureMap, "Gfx\\Textures", &m_pTexture) )
	{
		// just a warning message
		g_A3DErrLog.Log("A3DFlash::SetTextureMap(), fail!");
		m_pTexture = NULL;
	}

	return true;
}

bool A3DFlash::BuildStrikePath()
{
	m_pVecStrikePath[0] = m_vecStart;
	m_pVecStrikePath[m_nNumSegments] = m_vecEnd;
	
	A3DVECTOR3 vecDelta = m_vecEnd - m_vecStart;
	FLOAT	   vDis = vecDelta.Magnitude();
	A3DVECTOR3 vecDir = vecDelta / (vDis + 1e-6f);
	A3DVECTOR3 vecUp;
	FLOAT	   vMinL, vMaxL;
	
	vMinL = vDis / m_nNumSegments * 0.4f;
	vMaxL = vDis / m_nNumSegments * 1.4f;

	if( fabs(vecDir.y) < 0.9f )
		vecUp = Normalize(CrossProduct(vecDir, A3DVECTOR3(0.0f, 1.0f, 0.0f)));
	else
		vecUp = Normalize(CrossProduct(vecDir, A3DVECTOR3(1.0f, 0.0f, 0.0f)));
	A3DMATRIX4 matCoord = a3d_TransformMatrix(vecDir, vecUp, m_vecStart);

	A3DVECTOR3	vecPert;
	A3DVECTOR3	vecNewPos;
	FLOAT		vDeg, vPitch;
	FLOAT		vLengthAxis = 0.0f;

	FLOAT		vSeed = a_Random(0.0f, 1024.0f);
	for(int i=1; i<m_nNumSegments; i++)
	{
		m_pPerlinNoise->GetValue(vSeed + 2.0f * i, vecPert.m, 2);
		vDeg = vecPert.x * 180.0f;
		vPitch = vecPert.y * 180.0f / 6.0f; // about -30 ~ 30 degree
		if( i > m_nNumSegments / 2 )
		{
			vPitch *= 1.0f - 1.0f * i / m_nNumSegments;
		}

		vecNewPos.z = g_A3DMathUtility.COS(vPitch);
		vecNewPos.x = g_A3DMathUtility.SIN(vPitch) * g_A3DMathUtility.COS(vDeg);
		vecNewPos.y = g_A3DMathUtility.SIN(vPitch) * g_A3DMathUtility.SIN(vDeg);

		vLengthAxis += a_Random(vMinL, vMaxL); 
		vecNewPos = vecNewPos * (vLengthAxis / g_A3DMathUtility.COS(vPitch));
		m_pVecStrikePath[i] = vecNewPos * matCoord;
	}

	return true;
}

bool A3DFlash::RebuildStrikePath(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecEnd)
{
	m_vecStart			= vecStart;
	m_vecEnd			= vecEnd;
	
	return BuildStrikePath();
}