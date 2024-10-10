/*
 * FILE: A3DSimpleQuad.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: liyi 2012, 3, 15
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
 */

#include "A3DSimpleQuad.h"
#include "A3DPI.h"
#include "A3DStream.h"
#include "A3DVertexShader.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DMacros.h"
#include "A3DHLSL.h"

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
//	Implement A3DSimpleQuad
//	
///////////////////////////////////////////////////////////////////////////

A3DSimpleQuad::A3DSimpleQuad() :
m_pA3DEngine(NULL),
m_pA3DDevice(NULL),
m_pStream(NULL),
m_pvd(NULL),
m_pShaderFillColor(NULL)
{
}

A3DSimpleQuad::~A3DSimpleQuad()
{
}

//	Initialize object
bool A3DSimpleQuad::Init(A3DEngine* pA3DEngine)
{
	m_pA3DEngine	= pA3DEngine;
	m_pA3DDevice	= pA3DEngine->GetA3DDevice();

	m_pStream = new A3DStream;
	if (!m_pStream || !m_pStream->Init(m_pA3DDevice, sizeof (QUAD_VERTEX), 0, 4, 6, A3DSTRM_STATIC, A3DSTRM_STATIC))
	{
		a_LogOutput(1, "A3DSimpleQuad::Init, Failed to create vertex stream");
		return false;
	}

	//	Stream indices
	WORD aQuadIndices[6] = {0, 1, 2, 1, 3, 2};

	//	Initialize stream vertices
	m_aQuadVerts[0].vPos.Set(-1.0f, 1.0f, 0.0f, 1.0f);
	m_aQuadVerts[0].uv[0] = 0.0f;
	m_aQuadVerts[0].uv[1] = 0.0f;
	m_aQuadVerts[1].vPos.Set(1.0f, 1.0f, 0.0f, 1.0f);
	m_aQuadVerts[1].uv[0] = 1.0f;
	m_aQuadVerts[1].uv[1] = 0.0f;
	m_aQuadVerts[2].vPos.Set(-1.0f, -1.0f, 0.0f, 1.0f);
	m_aQuadVerts[2].uv[0] = 0.0f;
	m_aQuadVerts[2].uv[1] = 1.0f;
	m_aQuadVerts[3].vPos.Set(1.0f, -1.0f, 0.0f, 1.0f);
	m_aQuadVerts[3].uv[0] = 1.0f;
	m_aQuadVerts[3].uv[1] = 1.0f;

	int i, j;
	for (i=0; i < 4; i++)
	{
		for (j=0; j < MAX_NUM_VDATA; j++)
			m_aQuadVerts[i].data[j].Clear();
	}

	m_pStream->SetVerts(m_aQuadVerts, 4);
	m_pStream->SetIndices(aQuadIndices, 6);

	//	Create vertex declaration
	D3DVERTEXELEMENT9 aDecls[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 24, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		{0, 40, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
		D3DDECL_END()
	};

	m_pvd = new A3DVertexDecl;
	if (!m_pvd || !m_pvd->Init(m_pA3DDevice, aDecls))
		return false;

	A3DHLSLMan* pHLSLMan = m_pA3DEngine->GetA3DHLSLMan();
	if(pHLSLMan)
	{
		m_pShaderFillColor = pHLSLMan->LoadShader(	"shaders\\2.2\\HLSL\\Common\\SimpleQuad.hlsl",
											"vs_SimpleQuad",
											"shaders\\2.2\\HLSL\\Common\\SimpleQuad.hlsl",
											"ps_FillColor",
											NULL,
											0);
		if(!m_pShaderFillColor)
		{
			g_A3DErrLog.Log("A3DSimpleQuad::Init(), Failed to load shaders!");
			return false;
		}
															
	}
	return true;
}

//	Release object
void A3DSimpleQuad::Release()
{
	A3DHLSLMan* pHLSLMan = m_pA3DEngine->GetA3DHLSLMan();
	if( pHLSLMan)
	{
		if(m_pShaderFillColor )
		{
			pHLSLMan->ReleaseShader(m_pShaderFillColor);
			m_pShaderFillColor = NULL;
		}
	}

	A3DRELEASE(m_pStream);
	A3DRELEASE(m_pvd);
}

//	Set data that is bound with vertex
//	vi: vertex index [0, 3]
//	di: data index [0, MAX_NUM_VDATA-1]
//	data: data
void A3DSimpleQuad::SetVertexData(int vi, int di, const A3DVECTOR4& data)
{
	ASSERT(vi >= 0 && vi < 4);
	ASSERT(di >= 0 && di < MAX_NUM_VDATA);
	m_aQuadVerts[vi].data[di] = data;
}

//	Set vertex z value
void A3DSimpleQuad::SetVertexZ(int vi, float z)
{
	ASSERT(vi >= 0 && vi < 4);
	m_aQuadVerts[vi].vPos.z = z;
}

const A3DVECTOR4& A3DSimpleQuad::GetVertexData(int vi, int di)
{
	ASSERT(vi >= 0 && vi < 4);
	ASSERT(di >= 0 && di < MAX_NUM_VDATA);
	return m_aQuadVerts[vi].data[di];
}

//	Apply all user data to stream
void A3DSimpleQuad::ApplyVertexData()
{
	ASSERT(m_pStream);
	m_pStream->SetVertsDynamic(m_aQuadVerts, 4);
}

//	Apply stream and vertex declaration to device
bool A3DSimpleQuad::ApplyStream()
{
	m_pStream->Appear(0, false);
	m_pvd->Appear();
	return true;
}

//	Draw quad with specified pShader
bool A3DSimpleQuad::Draw(A3DHLSL* pShader)
{
	pShader->Appear();
	RawDraw();
	pShader->Disappear();
	return true;
}

//	Draw quad without any FX material
bool A3DSimpleQuad::RawDraw()
{
	ApplyStream();

	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	return true;
}

//	Draw with specified color, this can be used to clear render target
bool A3DSimpleQuad::FillColor(const A3DVECTOR4& col)
{
	if (!m_pShaderFillColor)
		return false;

	m_pShaderFillColor->SetValue4f("g_colClear", &col);
	m_pShaderFillColor->Appear();

	return Draw(m_pShaderFillColor);
	m_pShaderFillColor->Disappear();
}

//	Set render target size
void A3DSimpleQuad::SetRTSize(int iWid, int iHei)
{
	ASSERT(iWid > 0 && iHei > 0);
	m_pShaderFillColor->SetValue2i("g_vRTSize", iWid, iHei);
	
}


