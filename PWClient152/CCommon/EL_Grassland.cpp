/*
 * FILE: EL_Grassland.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2004/10/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"

#include <A3DDevice.h>
#include <A3DFuncs.h>
#include <A3DCamera.h>
#include <A3DEngine.h>
#include <A3DTexture.h>
#include <A3DTextureMan.h>
#include <A3DTerrain2.h>
#include <A3DViewport.h>
#include <A3DVertexShader.h>
#include <A3DPixelShader.h>
#include <A3DShaderMan.h>
#include <A3DFont.h>
#include <AFile.h>
#include <A3DStream.h>

#include "EL_GrassLand.h"
#include "EL_GrassType.h"

CELGrassLand::CELGrassLand()
{
	m_pA3DDevice		= NULL;
	m_pA3DTerrain		= NULL;
	m_pLogFile			= NULL;

	m_bProgressLoad		= false;
	m_bForceRebuild		= false;

	m_vTime				= 0.0f;

	m_vecWindDir		= A3DVECTOR3(1.0f, -0.5f, 0.0f);

	m_pStream			= NULL;

	m_pGrassVertexShader	= NULL;
	m_pGrassRenderShader	= NULL;
	m_pGrassRefractVertexShader = NULL;

	m_bIsRenderForRefract = false;

	memset(m_vecWindOffset, 0, sizeof(A3DVECTOR4) * WIND_BUF_LEN);

	m_vLODLevel			= 1.0f;
	m_fDNFactor			= 0.0f;
}

CELGrassLand::~CELGrassLand()
{
}

bool CELGrassLand::Init(A3DDevice * pA3DDevice, A3DTerrain2 * pTerrain, ALog * pLogFile)
{
	m_pA3DDevice	= pA3DDevice;
	m_pA3DTerrain	= pTerrain;
	m_pLogFile		= pLogFile;

	m_vTime			= 0.0f;
	m_wind.Init(256, 1.0f, 256, 0.95f, 8, 0x12345);
	m_wind.SetTurbulence(true);

	// now load the vertex shader
#ifdef ANGELICA_2_2
	static const D3DVERTEXELEMENT9 aVertexShaderDecl[] =
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		D3DDECL_END()
	};
#else
	DWORD	dwVSDecl[20];
	int		n = 0;				
	dwVSDecl[n ++] = D3DVSD_STREAM(0);					// begin tokens.
	dwVSDecl[n ++] = D3DVSD_REG(0, D3DVSDT_FLOAT3);		// x, y, z in world
	dwVSDecl[n ++] = D3DVSD_REG(1, D3DVSDT_FLOAT3);		// index, w1, w2
	dwVSDecl[n ++] = D3DVSD_REG(2, D3DVSDT_FLOAT2);		// u, v coords.
	dwVSDecl[n ++] = D3DVSD_END();						// end tokens.
#endif // ANGELICA_2_2

#ifdef ANGELICA_2_2
	m_pGrassVertexShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(SHADER_DIR"vs\\grass_wave.txt", false, (D3DVERTEXELEMENT9*)aVertexShaderDecl);
#else
	m_pGrassVertexShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(SHADER_DIR"vs\\grass_wave.txt", false, dwVSDecl);
#endif // ANGELICA_2_2

	if( NULL == m_pGrassVertexShader )
	{
		m_pLogFile->Log("CELGrassLand::Init(), failed to load grass_wave.txt");
	}

	m_pGrassRenderShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADER_DIR"ps\\grassrender.txt", false);
	if( NULL == m_pGrassRenderShader )
	{
		m_pLogFile->Log("CELGrassLand::Init(), failed to load grass_render.txt");
	}
	
#ifdef ANGELICA_2_2
	m_pGrassRefractVertexShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(SHADER_DIR"vs\\grass_wave_refract.txt", false, (D3DVERTEXELEMENT9*)aVertexShaderDecl);
#else
	m_pGrassRefractVertexShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(SHADER_DIR"vs\\grass_wave_refract.txt", false, dwVSDecl);
#endif // ANGELICA_2_2

	if( NULL == m_pGrassRefractVertexShader )
	{
		m_pLogFile->Log("CELGrassLand::Init(), failed to load grass_wave.txt");
	}

	m_nMaxVerts = 20000;
	m_nMaxFaces = 20000;

	// now create the stream for render
	DWORD dwVertexFlags = A3DSTRM_REFERENCEPTR;
	DWORD dwIndexFlags = A3DSTRM_REFERENCEPTR;

	m_pStream = new A3DStream();
	if( !m_pStream->Init(m_pA3DDevice, sizeof(GRASSVERTEX), GRASS_FVF_VERTEX, m_nMaxVerts, m_nMaxFaces * 3, dwVertexFlags, dwIndexFlags) )
	{
		m_pLogFile->Log("CELGrassLand::Init(), failed to create stream for rendering!");
		return false;
	}

	m_pSoftStream = new A3DStream();
	if( !m_pSoftStream->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nMaxVerts, m_nMaxFaces * 3, dwVertexFlags, dwIndexFlags) )
	{
		m_pLogFile->Log("CELGrassLand::Init(), failed to create softstream for rendering!");
		return false;
	}
	return true;
}

bool CELGrassLand::Release()
{
	if( m_pSoftStream )
	{
		m_pSoftStream->Release();
		delete m_pSoftStream;
		m_pSoftStream = NULL;
	}

	if( m_pStream )
	{
		m_pStream->Release();
		delete m_pStream;
		m_pStream = NULL;
	}

	if( m_pGrassRenderShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pGrassRenderShader);
		m_pGrassRenderShader = NULL;
	}

	if( m_pGrassVertexShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pGrassVertexShader);
		m_pGrassVertexShader = NULL;
	}

	if( m_pGrassRefractVertexShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pGrassRefractVertexShader);
		m_pGrassRefractVertexShader = NULL;
	}

	int nNumGrassType = m_GrassTypes.GetSize();

	for(int i=0; i<nNumGrassType; i++)
	{
		CELGrassType * pGrassType = m_GrassTypes[i];

		if( pGrassType )
		{
			pGrassType->Release();
			delete pGrassType;
			pGrassType = NULL;
		}
	}

	m_GrassTypes.RemoveAll();

	return true;
}

bool CELGrassLand::Update(const A3DVECTOR3& vecCenter, DWORD dwDeltaTime)
{
	int i;

	m_dwUpdateStart = a_GetTime();

	m_vTime += dwDeltaTime * 0.001f;

	// now update wind parameters
	for(i=0; i<WIND_BUF_LEN - 1; i++)
		m_vecWindOffset[i] = m_vecWindOffset[i + 1];

	float vWind;
	m_wind.GetValue(m_vTime * 20.0f, &vWind, 1);
	
	m_windWaver.SetWindForce(m_vecWindDir * vWind);
	m_windWaver.Tick(dwDeltaTime);
	A3DVECTOR3 offset = m_windWaver.GetPos();
	m_vecWindOffset[WIND_BUF_LEN - 1] = A3DVECTOR4(offset.x, offset.y, offset.z, 0.0f);
	
	// now update each type of grass
	int nNumGrassType = m_GrassTypes.GetSize();
	for(i=0; i<nNumGrassType; i++)
	{
		CELGrassType * pGrassType = m_GrassTypes[i];

		if( pGrassType )
			pGrassType->Update(vecCenter);
	}

	return true;
}

bool CELGrassLand::Render(A3DViewport * pViewport, bool bRenderAlpha)
{
	D3DXPLANE		cp;
	D3DXPLANE		hcp;

	DWORD dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_CLIPPLANEENABLE);
	if( dwState == D3DCLIPPLANE0 && m_pGrassVertexShader && m_pGrassRefractVertexShader ) // only one clip plane supported now
	{
		m_pA3DDevice->GetD3DDevice()->GetClipPlane(0, (float *)&cp);
		A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
		matVP.InverseTM();
		matVP.Transpose();
		D3DXPlaneTransform(&hcp, &cp, (D3DXMATRIX *) &matVP);

		m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float*) &hcp);
	}
	
	int nNumGrassType = m_GrassTypes.GetSize();

	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSpecularEnable(false);
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	
	m_pA3DDevice->SetTextureCoordIndex(0, 0);
	m_pA3DDevice->SetTextureCoordIndex(1, 1);
	m_pA3DDevice->SetTextureCoordIndex(2, 2);

	bool bFogTableEnable = m_pA3DDevice->GetFogTableEnable();
	if( !m_bIsRenderForRefract )
		m_pA3DDevice->SetFogTableEnable(true);
	else
		m_pA3DDevice->SetFogTableEnable(false);

	int i;

	// set pixel shader constants.
	if( m_pGrassRenderShader )
	{
		A3DCOLORVALUE c0;
		c0.a = m_fDNFactor;
		m_pA3DDevice->SetPixelShaderConstants(0, &c0, 1);
	}

	A3DVertexShader * pVertexShader = NULL;
	if( !m_bIsRenderForRefract )
	{
		pVertexShader = m_pGrassVertexShader;
	}
	else
	{
		pVertexShader = m_pGrassRefractVertexShader;
		A3DVECTOR4 c91 = A3DVECTOR4(1.0f, m_vRefractSurfaceHeight, 0.075f, 0.0f);
		m_pA3DDevice->SetVertexShaderConstants(91, &c91, 1);
	}

	// now set vertex shader's constants.
	if( pVertexShader )
	{
		A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
		matVP.Transpose();
		m_pA3DDevice->SetVertexShaderConstants(1, &matVP, 4);
		A3DMATRIX4 matLM = IdentityMatrix();
		matLM.Transpose();
		m_pA3DDevice->SetVertexShaderConstants(5, &matLM, 4);
		A3DVECTOR4 vecCamPos = A3DVECTOR4(pViewport->GetCamera()->GetPos());
		m_pA3DDevice->SetVertexShaderConstants(9, &vecCamPos, 1);
		
		for(i=0; i<WIND_BUF_LEN; i++)
			m_pA3DDevice->SetVertexShaderConstants(i + 10, &m_vecWindOffset[i], 1);
	}

	m_nGrassCount = 0;
	if( !bRenderAlpha )
	{
		// render grasses here
		// first we render grasses that are not using alpha blending
		for(i=0; i<nNumGrassType; i++)
		{
			CELGrassType * pGrassType = m_GrassTypes[i];

			if( pGrassType && !pGrassType->GetDefineData().bAlphaBlendEnable )
				pGrassType->RenderGrasses(pViewport);
		}
	}
	else
	{
		// then we render grasses that are using alpha blending
		for(i=0; i<nNumGrassType; i++)
		{
			CELGrassType * pGrassType = m_GrassTypes[i];

			if( pGrassType && pGrassType->GetDefineData().bAlphaBlendEnable )
				pGrassType->RenderGrasses(pViewport);
		}
	}

	m_pA3DDevice->ClearVertexShader();
	m_pA3DDevice->ClearPixelShader();

	m_pA3DDevice->SetFogTableEnable(bFogTableEnable);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);

	if( dwState == D3DCLIPPLANE0 ) // only one clip plane supported now
	{
		m_pA3DDevice->GetD3DDevice()->SetClipPlane(0, (float*) &cp);
	}

	return true;
}

void CELGrassLand::SetDNFactor(float f)
{
	m_fDNFactor = f;
	if( m_fDNFactor < 0.0f )
		m_fDNFactor = 0.0f;
	if( m_fDNFactor > 1.0f )
		m_fDNFactor = 1.0f;
}

bool CELGrassLand::Load(const char * szGrassLandFile)
{
	AFile fileToLoad;
	if( !fileToLoad.Open(szGrassLandFile, AFILE_OPENEXIST) )
	{
		m_pLogFile->Log("CELGrassLand::Load(), failed to open file [%s]", szGrassLandFile);
		return true;
	}

	DWORD dwReadLen;
	
	// then read types count
	int nNumTypes;
	if( !fileToLoad.Read(&nNumTypes, sizeof(int), &dwReadLen) )
		goto READFAIL;

	int i;
	// now load each grass's information out
	for(i=0; i<nNumTypes; i++)
	{
		// first type id of the grass type
		DWORD dwTypeID;
		if( !fileToLoad.Read(&dwTypeID, sizeof(DWORD), &dwReadLen) )
			goto READFAIL;
		
		// grass file's name
		char szGrassFile[256];
		if( !fileToLoad.Read(szGrassFile, 256, &dwReadLen) )
			goto READFAIL;

		// max visible count
		int nMaxVisibleCount;
		if( !fileToLoad.Read(&nMaxVisibleCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		// then grass type's definition data
		CELGrassType::GRASSDEFINE_DATA data;
		if( !fileToLoad.Read(&data, sizeof(data), &dwReadLen) )
			goto READFAIL;	

		// initialize a new grass type object
		CELGrassType * pGrassType = new CELGrassType();
		if( !pGrassType->Init(m_pA3DDevice, this, dwTypeID, nMaxVisibleCount, szGrassFile, m_pLogFile, &data) )
		{
			m_pLogFile->Log("CELGrassLand::Load(), failed to init CELGrassType!");
			return false;
		}

		// then load the grass type's data (grass bits map)
		if( !pGrassType->Load(&fileToLoad) )
		{
			m_pLogFile->Log("CELGrassLand::Load(), failed to call CELGrassType::Load()!");
			return false;
		}

		// add this type
		m_GrassTypes.Add(pGrassType);
	}

	fileToLoad.Close();
	return true;

READFAIL:
	m_pLogFile->Log("CELGrassLand::Load(), Read from file failed!");
	return false;
}

bool CELGrassLand::Save(const char * szGrassLandFile)
{
	AFile fileToSave;
	if( !fileToSave.Open(szGrassLandFile, AFILE_CREATENEW | AFILE_BINARY) )
	{
		m_pLogFile->Log("CELGrassLand::Save(), failed to create file [%s]", szGrassLandFile);
		return false;
	}

	DWORD dwWriteLen;
	
	// then write types count
	int nNumTypes = m_GrassTypes.GetSize();
	if( !fileToSave.Write(&nNumTypes, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;

	int i;
	// now write each grass's information out
	for(i=0; i<nNumTypes; i++)
	{
		CELGrassType * pGrassType = m_GrassTypes[i];

		// first type id of the grass type
		DWORD dwTypeID = pGrassType->GetTypeID();
		if( !fileToSave.Write(&dwTypeID, sizeof(DWORD), &dwWriteLen) )
			goto WRITEFAIL;
		
		// grass file's name
		if( !fileToSave.Write((void *)pGrassType->GetGrassFile(), 256, &dwWriteLen) )
			goto WRITEFAIL;

		// max visible count
		int nMaxVisibleCount = 0;
		if( !fileToSave.Write(&nMaxVisibleCount, sizeof(int), &dwWriteLen) )
			goto WRITEFAIL;

		// then grass type's definition data
		CELGrassType::GRASSDEFINE_DATA data = pGrassType->GetDefineData();
		if( !fileToSave.Write(&data, sizeof(data), &dwWriteLen) )
			goto WRITEFAIL;

		if( !pGrassType->Save(&fileToSave) )
		{
			m_pLogFile->Log("CELGrassLand::Save(), failed to call CELGrassType::Save()!");
			return false;
		}
	}

	fileToSave.Close();
	return true;

WRITEFAIL:
	m_pLogFile->Log("CELGrassLand::Save(), Write to the file failed!");
	return false;
}


CELGrassType * CELGrassLand::GetGrassTypeByID(DWORD dwTypeID)
{
	int nNumType = m_GrassTypes.GetSize();

	for(int i=0; i<nNumType; i++)
	{
		CELGrassType * pGrassType = m_GrassTypes[i];

		if( pGrassType->GetTypeID() == dwTypeID )
			return pGrassType;
	}

	return NULL;
}

bool CELGrassLand::AddGrassType(DWORD dwTypeID, CELGrassType::GRASSDEFINE_DATA& defData, int nMaxVisibleCount, const char * szGrassFile, CELGrassType ** ppGrassType)
{
	CELGrassType * pGrassType = new CELGrassType();
	if( !pGrassType->Init(m_pA3DDevice, this, dwTypeID, nMaxVisibleCount, szGrassFile, m_pLogFile, &defData) )
	{
		m_pLogFile->Log("CELGrassLand::AddGrassType(), failed to init CELGrassType!");
		return false;
	}

	m_GrassTypes.Add(pGrassType);

	*ppGrassType = pGrassType;
	return true;
}

bool CELGrassLand::DeleteGrassType(CELGrassType * pGrassType)
{
	// first find it in the array
	int nNumTypes = m_GrassTypes.GetSize();

	for(int i=0; i<nNumTypes; i++)
	{
		if( pGrassType == m_GrassTypes[i] )
		{
			pGrassType->Release();
			delete pGrassType;

			m_GrassTypes.RemoveAt(i);
			return true;
		}
	}
	
	return false;
}

bool CELGrassLand::SetLODLevel(float level)
{
	m_vLODLevel = level;

	int i;
	// now update each type of grass
	int nNumGrassType = m_GrassTypes.GetSize();
	for(i=0; i<nNumGrassType; i++)
	{
		CELGrassType * pGrassType = m_GrassTypes[i];

		if( pGrassType )
			pGrassType->AdjustForLODLevel();
	}
	
	return true;
}

// Delete all grass types, by jdl
bool CELGrassLand::DeleteAllGrassTypes()
{
	// first find it in the array
	int nNumTypes = m_GrassTypes.GetSize();

	for(int i=0; i<nNumTypes; i++)
	{
		A3DRELEASE(m_GrassTypes[i]);
	}

	m_GrassTypes.RemoveAll();

	return true;
}

bool CELGrassLand::RenderForRefract(A3DViewport* pViewport, float vRefractSurfaceHeight)
{
	m_bIsRenderForRefract = true;
	m_vRefractSurfaceHeight = vRefractSurfaceHeight;

	Render(pViewport, false);

	m_bIsRenderForRefract = false;
	return true;
}

///////////////////////////////////////////////////////////////////////////
// class to wave in the wind
///////////////////////////////////////////////////////////////////////////
CELWindWaver::CELWindWaver()
{
	m_k1 = 2.0f;
	m_k2 = 0.0f;
	m_f = 0.1f;

	m_vMass = 0.1f;
	m_vecPos = A3DVECTOR3(0.0f);
	m_vecAccel = A3DVECTOR3(0.0f);
	m_vecVelocity = A3DVECTOR3(0.0f);
	m_vecSpringForce = A3DVECTOR3(0.0f);
	m_vecWindForce = A3DVECTOR3(0.0f);
}

CELWindWaver::~CELWindWaver()
{
}

bool CELWindWaver::CalculateForce(const A3DVECTOR3& vecPos, const A3DVECTOR3& vecVelocity)
{
	m_vecSpringForce = m_k1 * (-vecPos) + m_k2 * (-vecPos) - m_f * vecVelocity;
	m_vecForceCombined = m_vecWindForce + m_vecSpringForce;

	return true;
}

bool CELWindWaver::UpdatePos(float dt)
{
	m_vecAccel = m_vecForceCombined / m_vMass;
	m_vecPos = m_vecPos + m_vecVelocity * dt + m_vecAccel * (0.5f * dt * dt);
	m_vecVelocity = m_vecVelocity + m_vecAccel * dt;

	return true;
}

bool CELWindWaver::Tick(int nDeltaTime)
{
	float dt = nDeltaTime * 0.001f;

	m_vecSpringForce = m_k1 * (-m_vecPos) + m_k2 * (-m_vecPos) - m_f * m_vecVelocity;

	// L-K Method 2-level
	A3DVECTOR3		vecNewPos;

	// k1;
	A3DVECTOR3 vecOldPos = m_vecPos;
	A3DVECTOR3 vecK1 = m_vecVelocity;

	// k2;
	vecNewPos = vecOldPos + dt * vecK1;
	CalculateForce(vecNewPos, m_vecVelocity);
	UpdatePos(dt);
	A3DVECTOR3 vecK2 = m_vecVelocity;

	m_vecVelocity = (vecK1 + vecK2) * 0.5f;
	m_vecPos = vecOldPos + m_vecVelocity * dt; 

	return true;
}


