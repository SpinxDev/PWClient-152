/*
 * FILE: EL_TerrainOutline.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2005/4/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include <A3DTypes.h>
#include <A3DFuncs.h>
#include <A3DViewport.h>
#include <A3DStream.h>
#include <A3DDevice.h>
#include <A3DEngine.h>
#include <AFileImage.h>
#include <A3DVertex.h>
#include <A3DCamera.h>
#include <A3DTexture.h>
#include <A3DTextureMan.h>

#include "EL_TerrainOutline.h"

CELTerrainOutline::CELTerrainOutline()
{
	m_pA3DDevice		= NULL;

	m_vCellSize			= 2.0f;
	m_nWidth			= 0;
	m_nHeight			= 0;

	m_pLowResHeights	= NULL;
	m_pStreams[0]		= NULL;
	m_pStreams[1]		= NULL;
	m_pStreams[2]		= NULL;
	m_pTexture			= NULL;

	m_nVertCount[0]		= 0;
	m_nVertCount[1]		= 0;
	m_nVertCount[2]		= 0;
	m_nFaceCount[0]		= 0;
	m_nFaceCount[1]		= 0;
	m_nFaceCount[2]		= 0;
}

CELTerrainOutline::~CELTerrainOutline()
{
	Release();
}

bool CELTerrainOutline::Init(A3DDevice * pA3DDevice, const char * szFile)
{
	m_pA3DDevice = pA3DDevice;
	
	AFileImage fileImage;

	if( !fileImage.Open(szFile, AFILE_OPENEXIST) )
		return false;

	DWORD dwReadLen;

	if( !fileImage.Read(&m_vCellSize, sizeof(DWORD), &dwReadLen) )
		return false;
	if( !fileImage.Read(&m_nWidth, sizeof(int), &dwReadLen) )
		return false;
	if( !fileImage.Read(&m_nHeight, sizeof(int), &dwReadLen) )
		return false;

	m_pLowResHeights = new float[(m_nWidth + 1) * (m_nHeight + 1)];
	if( !fileImage.Read(m_pLowResHeights, sizeof(float) * (m_nWidth + 1) * (m_nHeight + 1), &dwReadLen) )
		return false;

	fileImage.Close();

	m_x = -m_nWidth / 2 * m_vCellSize;
	m_z = m_nHeight / 2 * m_vCellSize;

	if( m_nWidth > m_nHeight )
	{
		m_vTOX = m_x;
		m_vTOZ = -m_x;
		m_vTS = 1.0f / (m_nWidth * m_vCellSize);
	}
	else
	{
		m_vTOX = -m_z;
		m_vTOZ = m_z;
		m_vTS = 1.0f / (m_nHeight * m_vCellSize);
	}

	SetStartAndEnd(500.0f, 2000.0f);

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile("surfaces\\maps\\world.dds", &m_pTexture) )
		return false;

	return true;
}

bool CELTerrainOutline::Release()
{
	if( m_pLowResHeights )
	{
		delete m_pLowResHeights;
		m_pLowResHeights = NULL;
	}

	if( m_pStreams[0] )
	{
		m_pStreams[0]->Release();
		delete m_pStreams[0];
		m_pStreams[0] = NULL;
	}

	if( m_pStreams[1] )
	{
		m_pStreams[1]->Release();
		delete m_pStreams[1];
		m_pStreams[1] = NULL;
	}

	if( m_pStreams[2] )
	{
		m_pStreams[2]->Release();
		delete m_pStreams[2];
		m_pStreams[2] = NULL;
	}

	if( m_pTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexture);
		m_pTexture = NULL;
	}

	return true;
}

bool CELTerrainOutline::Render(A3DViewport * pViewport, bool bUpdate)
{
	A3DCameraBase * pCamera = pViewport->GetCamera();

	A3DVIEWPORTPARAM param = *pViewport->GetParam();

	A3DVECTOR3 vecCamPos = pCamera->GetPos();
	int cx = int((vecCamPos.x - m_x) / m_vCellSize);
	int cy = int((m_z - vecCamPos.z) / m_vCellSize);

	A3DVECTOR3 vecLeft, vecRight, vecCenter;
	vecLeft.x = 0.0f;
	vecLeft.y = param.Height / 2.0f;
	vecLeft.z = 0.0f;
	vecRight.x = param.Width - 1.0f;
	vecRight.y = param.Height / 2.0f;
	vecRight.z = 0.0f;
	vecCenter.x = param.Width / 2.0f;
	vecCenter.y	= param.Height / 2.0f;
	vecCenter.z = 0.0f;

	pViewport->InvTransform(vecLeft, vecLeft);
	pViewport->InvTransform(vecRight, vecRight);
	pViewport->InvTransform(vecCenter, vecCenter);

	int x1, x2, y1, y2;
	
	float lx = vecLeft.x - vecCamPos.x;
	float ly = vecCamPos.z - vecLeft.z;
	float rx = vecRight.x - vecCamPos.x;
	float ry = vecCamPos.z - vecRight.z;	
	float mx = vecCenter.x - vecCamPos.x;
	float my = vecCamPos.z - vecCenter.z;

	float vFront, vBack;
	vFront = pCamera->GetZFront();
	vBack = pCamera->GetZBack();
	pCamera->SetZFrontAndBack(m_vStart * 0.8f, m_vEnd * 1.0f, true);
	pCamera->Active();

	m_pA3DDevice->ClearPixelShader();
	m_pA3DDevice->ClearVertexShader();

	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	m_pTexture->Appear(0);
	
	// first we render left part on the screen
	if( lx < 0.0f )
	{
		x1 = cx - m_nView;
		x2 = cx;
	}
	else
	{
		x1 = cx;
		x2 = cx + m_nView;
	}

	if( ly < 0.0f )
	{
		y1 = cy - m_nView;
		y2 = cy;
	}
	else
	{
		y1 = cy;
		y2 = cy + m_nView;
	}

	DrawPart(0, x1, x2, y1, y2, bUpdate);

	if( lx * rx > 0.0f && ly * ry > 0.0f )
	{
		m_nVertCount[1] = 0;
		m_nFaceCount[1] = 0;
		goto EXIT;
	}

	// now we render right part on the screen;
	if( rx < 0.0f )
	{
		x1 = cx - m_nView;
		x2 = cx;
	}
	else
	{
		x1 = cx;
		x2 = cx + m_nView;
	}

	if( ry < 0.0f )
	{
		y1 = cy - m_nView;
		y2 = cy;
	}
	else
	{
		y1 = cy;
		y2 = cy + m_nView;
	}

	DrawPart(1, x1, x2, y1, y2, bUpdate);

	if( (lx * mx > 0.0f && ly * my > 0.0f) || (rx * mx > 0.0f && ry * my > 0.0f) )
	{
		m_nVertCount[2] = 0;
		m_nFaceCount[2] = 0;
		goto EXIT;
	}

	if( mx < 0.0f )
	{
		x1 = cx - m_nView;
		x2 = cx;
	}
	else
	{
		x1 = cx;
		x2 = cx + m_nView;
	}

	if( my < 0.0f )
	{
		y1 = cy - m_nView;
		y2 = cy;
	}
	else
	{
		y1 = cy;
		y2 = cy + m_nView;
	}

	DrawPart(2, x1, x2, y1, y2, bUpdate);

EXIT:
	m_pTexture->Disappear(0);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->Clear(D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	pCamera->SetZFrontAndBack(vFront, vBack, true);
	pCamera->Active();
	return true;
}

bool CELTerrainOutline::SetStartAndEnd(float vStart, float vEnd)
{
	m_vStart = vStart;
	m_vEnd = vEnd;

	m_nView = (int)(m_vEnd / m_vCellSize);

	if( m_nView > 256 )
		m_nView = 256;

	if( m_pStreams[0] )
	{
		m_pStreams[0]->Release();
		delete m_pStreams[0];
		m_pStreams[0] = NULL;
	}

	if( m_pStreams[1] )
	{
		m_pStreams[1]->Release();
		delete m_pStreams[1];
		m_pStreams[1] = NULL;
	}

	if( m_pStreams[2] )
	{
		m_pStreams[2]->Release();
		delete m_pStreams[2];
		m_pStreams[2] = NULL;
	}

	m_pStreams[0] = new A3DStream();
	if( !m_pStreams[0]->Init(m_pA3DDevice, A3DVT_LVERTEX, (m_nView + 1) * (m_nView + 1), m_nView * m_nView * 6,  
		A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR) ) 
		return false;

	m_pStreams[1] = new A3DStream();
	if( !m_pStreams[1]->Init(m_pA3DDevice, A3DVT_LVERTEX, (m_nView + 1) * (m_nView + 1), m_nView * m_nView * 6,  
		A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR) ) 
		return false;

	m_pStreams[2] = new A3DStream();
	if( !m_pStreams[2]->Init(m_pA3DDevice, A3DVT_LVERTEX, (m_nView + 1) * (m_nView + 1), m_nView * m_nView * 6,  
		A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR) ) 
		return false;

	return true;
}

bool CELTerrainOutline::DrawPart(int sid, int x1, int x2, int y1, int y2, bool bUpdate)
{
	A3DStream * pStream = m_pStreams[sid];

	if( bUpdate )
	{
		if( x1 < 0 )
			x1 = 0;
		if( x2 > m_nWidth )
			x2 = m_nWidth;

		if( y1 < 0 )
			y1 = 0;
		if( y2 > m_nHeight )
			y2 = m_nHeight;

		if( x1 >= x2 || y1 >= y2 )
			return true;

		int nx = x2 - x1;
		int ny = y2 - y1;
		
		A3DLVERTEX * pVerts;
		if( !pStream->LockVertexBuffer(0, 0, (BYTE **)&pVerts, 0) )
			return false;

		WORD * pIndices;
		if( !pStream->LockIndexBuffer(0, 0, (BYTE **)&pIndices, 0) )
			return false;

		float vZ = m_z - m_vCellSize * y1;
		float * pHeights;
		for(int y=0; y<=ny; y++)
		{
			float vX = m_x + m_vCellSize * x1;
			pHeights = m_pLowResHeights + (y + y1) * (m_nWidth + 1) + x1;
			for(int x=0; x<=nx; x++)
			{
				*pVerts = A3DLVERTEX(A3DVECTOR3(vX, *pHeights, vZ), 0xffffffff, 0xff000000, (vX - m_vTOX) * m_vTS, (m_vTOZ - vZ) * m_vTS);

				if( y != ny && x != nx )
				{
					pIndices[0] = y * (nx + 1) + x;
					pIndices[1] = pIndices[0] + 1;
					pIndices[2] = pIndices[0] + (nx + 1);

					pIndices[3] = pIndices[2];
					pIndices[4] = pIndices[1];
					pIndices[5] = pIndices[0] + 1 + nx + 1;
					pIndices += 6;
				}

				vX += m_vCellSize;
				pHeights ++;
				pVerts ++;
				
			}

			vZ -= m_vCellSize;
		}

		pStream->UnlockIndexBuffer();
		pStream->UnlockVertexBuffer();

		m_nVertCount[sid] = (ny + 1) * (nx + 1);
		m_nFaceCount[sid] = ny * nx * 2;
	}
	
	if( m_nVertCount[sid] > 0 && m_nFaceCount[sid] > 0 )
	{
		pStream->Appear();
		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCount[sid], 0, m_nFaceCount[sid]);
	}

	return true;
}


