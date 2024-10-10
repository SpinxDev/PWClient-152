/*
 * FILE: A3DLitModel.cpp
 *
 * DESCRIPTION: Lit static model in Angelica Engine, usually used for building
 *
 * CREATED BY: Hedi, 2004/11/5
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DTypes.h"
#include "A3DFuncs.h"
#include "A3DVertex.h"
#include "A3DGeometry.h"
#include "A3DObject.h"
#include "A3DTexture.h"
#include "A3DTextureMan.h"
#include "A3DPixelShader.h"
#include "A3DShaderMan.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "A3DStream.h"
#include "A3DFrame.h"
#include "A3DMesh.h"
#include "A3DCamera.h"
#include "A3DCollision.h"
#include "A3DPI.h"
#include "AF.h"
#include "A3DShadowMap.h"
#include "A3DLitModelRender.h"
#include "A3DOcclusionMan.h"
#include "A3DConfig.h"

#include "A3DLitModel.h"
#include "A3DHLSL.h"
#include "A3DEffect.h"
#include "A3DEnvironment.h"
#include "A3DWireCollector.h"
#include "A3DLitModelSharedData.h"
#include "A3DLitModelSharedDataMan.h"

//////////////////////////////////////////////////////////////////////////
// class A3DLitMesh
//	
//		A mesh that was pre-lit
//////////////////////////////////////////////////////////////////////////

int A3DLITMESH_TOTAL_COUNT = 0;
A3DLitMesh::A3DLitMesh(A3DLitModel * pLitModel)
{
	m_pParentModel			= pLitModel;

	m_pA3DDevice			= NULL;
	m_pA3DTexture			= NULL;
	m_pA3DReflectTexture	= NULL;
	
	m_pVerts				= NULL;
	m_pIndices				= NULL;
	m_pNormals				= NULL;

	m_pDayColors			= NULL;
	m_pNightColors			= NULL;
	m_fDNFactor				= -1.0f;
	
	m_bHasExtraColors		= false;
	m_pDayColorsExtra		= NULL;
	m_pNightColorsExtra		= NULL;

	memset(m_szTextureMap, 0, sizeof(m_szTextureMap));
	
	m_nVertCount			= 0;
	m_nFaceCount			= 0;

	m_bVisible				= true;
	m_nAlphaValue			= 255;

	m_vDisToCam				= 999999.0f;

	m_bSupportLM			= false;
	m_bUseLM				= false;
	m_pLMCoords				= NULL;
    m_pStream               = NULL;

    m_pDetailTexture        = NULL;
    m_fDetailTile           = 5.0f;
    m_fDetailHardness       = 1.0f;
	//m_dwCurrentEffectMacroFlag = 0;
	A3DLITMESH_TOTAL_COUNT++;
}

A3DLitMesh::~A3DLitMesh()
{
	A3DLITMESH_TOTAL_COUNT--;
	Release();
}

// Create/Release a lit mesh
bool A3DLitMesh::Create(A3DDevice * pA3DDevice, int nVertCount, int nFaceCount, const char * szTextureMap, bool bSupportLM)
{
	m_pA3DDevice		= pA3DDevice;
	m_nVertCount		= nVertCount;
	m_nFaceCount		= nFaceCount;

	if(szTextureMap)
		strcpy(m_szTextureMap, szTextureMap);

	if( !CreateVertexBuffers() )
	{
		g_A3DErrLog.Log("A3DLitMesh::Create(), failed to call CreateVertexBuffers()");
		return false;
	}

	if( !LoadTexture() )
	{
		g_A3DErrLog.Log("A3DLitMesh::Create(), failed to load texture!");
		return false;
	}

	A3DMATERIALPARAM param;
	memset(&param, 0, sizeof(param));
	param.Diffuse = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
	param.Ambient = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
	param.Specular = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
	param.Emissive = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
	param.Power = 0.0f;
	m_Material.Init(pA3DDevice, param);

	//Light map
	m_bSupportLM = bSupportLM;
	if( bSupportLM)
	{
		if(!CreateLMCoords())
		{
			return false;
		}
	}

	return true;
}

bool A3DLitMesh::Release()
{
	ReleaseTexture();
	ReleaseVertexBuffers();
	ReleaseLMCoords();
    ReleaseStream();
	return true;
}

bool A3DLitMesh::MergeMesh(A3DLitMesh * pMeshToBeMerged)
{
	int nNewVertCount = m_nVertCount + pMeshToBeMerged->GetNumVerts();
	int nNewFaceCount = m_nFaceCount + pMeshToBeMerged->GetNumFaces();

	A3DLMVERTEX *		pVerts;
	WORD *				pIndices;
	A3DVECTOR3 *		pNormals;
	A3DCOLOR *			pDayColors;
	A3DCOLOR *			pNightColors;

	pVerts = new A3DLMVERTEX[nNewVertCount];
	pIndices = new WORD[nNewFaceCount * 3];
	pNormals = new A3DVECTOR3[nNewVertCount];
	pDayColors = new A3DCOLOR[nNewVertCount];
	pNightColors = new A3DCOLOR[nNewVertCount];

	memcpy(pVerts, m_pVerts, sizeof(A3DLMVERTEX) * m_nVertCount);
	memcpy(pVerts + m_nVertCount, pMeshToBeMerged->GetVerts(), sizeof(A3DLMVERTEX) * pMeshToBeMerged->GetNumVerts());

	memcpy(pNormals, m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount);
	memcpy(pNormals + m_nVertCount, pMeshToBeMerged->GetNormals(), sizeof(A3DVECTOR3) * pMeshToBeMerged->GetNumVerts());

	memcpy(pDayColors, m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount);
	memcpy(pDayColors + m_nVertCount, pMeshToBeMerged->GetDayColors(), sizeof(A3DCOLOR) * pMeshToBeMerged->GetNumVerts());

	memcpy(pNightColors, m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount);
	memcpy(pNightColors + m_nVertCount, pMeshToBeMerged->GetNightColors(), sizeof(A3DCOLOR) * pMeshToBeMerged->GetNumVerts());

	memcpy(pIndices, m_pIndices, sizeof(WORD) * m_nFaceCount * 3);
	WORD * pIndices2 = pMeshToBeMerged->GetIndices();
	int nNumIndices1 = m_nFaceCount * 3;
	int nNumIndices2 = pMeshToBeMerged->GetNumFaces() * 3;
	for(int i=0; i<nNumIndices2; i++)
		pIndices[i + nNumIndices1] = pIndices2[i] + m_nVertCount;
	
	ReleaseVertexBuffers();

	//merged light map coords
	if( m_bSupportLM && pMeshToBeMerged->IsSupportLightMap())
	{
		A3DLIGHTMAPCOORD* pLMCoords = new A3DLIGHTMAPCOORD[nNewVertCount];
		memcpy(pLMCoords, m_pLMCoords, sizeof(A3DLIGHTMAPCOORD) * m_nVertCount);
		memcpy(pLMCoords + m_nVertCount, pMeshToBeMerged->GetLightMapCoords(), sizeof(A3DLIGHTMAPCOORD)* pMeshToBeMerged->GetNumVerts());
		ReleaseLMCoords();
	}

	m_pVerts = pVerts;
	m_pIndices = pIndices;
	m_pNormals = pNormals;
	m_pDayColors = pDayColors;
	m_pNightColors = pNightColors;

	m_nVertCount = nNewVertCount;
	m_nFaceCount = nNewFaceCount;

	m_aabb.Merge(pMeshToBeMerged->GetAABB());

	
    CreateStream();

	return true;
}

bool A3DLitMesh::LoadTexture()
{ 
	if(m_pA3DTexture != NULL && m_pA3DTexture->GetClassID() == A3D_CID_HLSLSHADER) {
		return true;
	}

	// first release old texture
	ReleaseTexture();
	AString strTextureMapName = m_szTextureMap;
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szTextureMap, &m_pA3DTexture))
	{
		return false;	//上面实际上永远返回true, 不会走进来.
		g_A3DErrLog.Log("A3DLitMesh::LoadTexture(), failed to load texture [%s]!", m_szTextureMap);
	}
	

	if(m_pA3DTexture == NULL || 
       (!m_pA3DTexture->IsShaderTexture() && m_pA3DTexture->GetD3DBaseTexture() == NULL))
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pA3DTexture);
		m_pA3DTexture = NULL;

		strTextureMapName = "shaders\\textures\\ErrDiffuse.dds";

		bool bLoadErrorTexture = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()
			->LoadTextureFromFile(strTextureMapName, &m_pA3DTexture);

		if(!bLoadErrorTexture)
			return false;

	}

	if(m_pA3DTexture->GetClassID() == A3D_CID_TEXTURE2D && g_pA3DConfig->GetFlagHLSLEffectEnable())
	{
		A3DEffect* pEffect = NULL;
		pEffect = A3DEffect::CreateHLSLShader(m_pA3DDevice, strTextureMapName, 
			"shaders\\2.2\\hlsl\\LitModel\\litmodel_mtl_default.hlsl"); 
		if(pEffect)
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pA3DTexture);
			m_pA3DTexture = pEffect;
			//m_dwCurrentEffectMacroFlag = 0;
		}
		else
		{
			g_A3DErrLog.Log("A3DLitMesh::LoadTexture(), failed to load litmodel_mtl_default.hlsl", m_szTextureMap);
		}

	}

	if( strlen(m_szTextureMap) > 4 && strstr(m_szTextureMap, "nrf_") )
	{
		char szFileName[MAX_PATH];
		af_GetFileTitle(m_szTextureMap, szFileName, MAX_PATH);
		sprintf(m_szReflectTextureMap, "Shaders\\textures\\nrf\\%s", szFileName + 1);  
		if( !af_IsFileExist(m_szReflectTextureMap) )
			strcpy(m_szReflectTextureMap, "Shaders\\textures\\nrf\\rf_default.dds");
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szReflectTextureMap, &m_pA3DReflectTexture) )
		{
			g_A3DErrLog.Log("A3DLitMesh::LoadTexture(), failed to load texture [%s]!", m_szReflectTextureMap);
			return false;
		}
	}

	return true;
}

bool A3DLitMesh::ReleaseTexture()
{
	if( m_pA3DTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pA3DTexture);
		m_pA3DTexture = NULL;
	}

	if( m_pA3DReflectTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pA3DReflectTexture);
		m_pA3DReflectTexture = NULL;
	}

    if( m_pDetailTexture )
    {
        m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pDetailTexture);
        m_pDetailTexture = NULL;
    }

	return true;
}

bool A3DLitMesh::CreateVertexBuffers()
{
	bool bHasExtraColors = m_bHasExtraColors;

	// try to release old ones.
	ReleaseVertexBuffers();

	m_pVerts = new A3DLMVERTEX[m_nVertCount];
	m_pIndices = new WORD[m_nFaceCount * 3];
	m_pNormals = new A3DVECTOR3[m_nVertCount];
	m_pDayColors = new A3DCOLOR[m_nVertCount];
	m_pNightColors = new A3DCOLOR[m_nVertCount];

	memset(m_pVerts, 0, sizeof(A3DLMVERTEX) * m_nVertCount);
	memset(m_pIndices, 0, sizeof(WORD) * m_nFaceCount * 3);
	memset(m_pNormals, 0, sizeof(A3DVECTOR3) * m_nVertCount);
	memset(m_pDayColors, 0, sizeof(A3DCOLOR) * m_nVertCount);
	memset(m_pNightColors, 0, sizeof(A3DCOLOR) * m_nVertCount);

	if (bHasExtraColors)
	{
		m_bHasExtraColors = bHasExtraColors;
		m_pDayColorsExtra = new A3DCOLOR[m_nVertCount];
		m_pNightColorsExtra = new A3DCOLOR[m_nVertCount];
		memset(m_pDayColorsExtra, 0, sizeof(A3DCOLOR) * m_nVertCount);
		memset(m_pNightColorsExtra, 0, sizeof(A3DCOLOR) * m_nVertCount);
	}

	return true;
}

bool A3DLitMesh::ReleaseVertexBuffers()
{
	if( m_pNightColors )
	{
		delete [] m_pNightColors;
		m_pNightColors = NULL;
	}

	if( m_pDayColors )
	{
		delete [] m_pDayColors;
		m_pDayColors = NULL;
	}

	if( m_pNormals )
	{
		delete [] m_pNormals;
		m_pNormals = NULL;
	}

	if( m_pIndices )
	{
		delete [] m_pIndices;
		m_pIndices = NULL;
	}

	if( m_pVerts )
	{
		delete [] m_pVerts;
		m_pVerts = NULL;
	}

	if (m_bHasExtraColors)
	{
		m_bHasExtraColors = false;

		if( m_pNightColorsExtra )
		{
			delete [] m_pNightColorsExtra;
			m_pNightColorsExtra = NULL;
		}

		if( m_pDayColorsExtra )
		{
			delete [] m_pDayColorsExtra;
			m_pDayColorsExtra = NULL;
		}
	}
	return true;
}

bool A3DLitMesh::CreateLMCoords()
{
	ReleaseLMCoords();
	m_pLMCoords = new A3DLIGHTMAPCOORD[m_nVertCount];
	memset(m_pVerts, 0, sizeof(A3DLMVERTEX) * m_nVertCount);
	return true;
}
bool A3DLitMesh::ReleaseLMCoords()
{
	if( m_pLMCoords != NULL)
	{
		delete[] m_pLMCoords;
		m_pLMCoords = NULL;
	}
	return true;
}

bool A3DLitMesh::UpdateColors()
{
    if (m_fDNFactor < 0 || m_fDNFactor > 1)
        return false;

	unsigned int d, n;
	n = (unsigned int)(m_fDNFactor * 255.0f);
	d = 255 - n;

    
    float fDayScale = m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetLightMapParam()->MaxLight;
    float fNightScale = m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetLightMapParam()->MaxLightNight;
    unsigned int iDayScale = (unsigned int)(fDayScale * 255);
    unsigned int iNightScale = (unsigned int)(fNightScale * 255);
    d *= iDayScale;
    n *= iNightScale;
    unsigned int iDiv = a_Max(iDayScale, iNightScale) * 255;
    for(int i = 0; i < m_nVertCount; i++)
    {
        unsigned int iDayColor[3] = 
            {(m_pDayColors[i] & 0xFF0000) >> 16, (m_pDayColors[i] & 0x00FF00) >> 8, (m_pDayColors[i] & 0x0000FF)};
        unsigned int iNightColor[3] = 
            {(m_pNightColors[i] & 0xFF0000) >> 16, (m_pNightColors[i] & 0x00FF00) >> 8, (m_pNightColors[i] & 0x0000FF)};
        unsigned int iRed   = (iDayColor[0] * d + iNightColor[0] * n) / iDiv;
        unsigned int iGreen = (iDayColor[1] * d + iNightColor[1] * n) / iDiv;
        unsigned int iBlue  = (iDayColor[2] * d + iNightColor[2] * n) / iDiv;
        ASSERT(iRed >= 0 && iRed < 256);
        ASSERT(iGreen >= 0 && iGreen < 256);
        ASSERT(iBlue >= 0 && iBlue < 256);
        m_pVerts[i].diffuse = 0xff000000 | (iRed << 16) | (iGreen << 8) | iBlue;
        
    }
    
    /*
	for(int i=0; i<m_nVertCount; i++)
	{
		A3DCOLOR c0, c1, c2, c3;
		c0 = m_pDayColors[i] & 0x00ff00ff;
		c1 = m_pDayColors[i] & 0x0000ff00;
		c2 = m_pNightColors[i] & 0x00ff00ff;
		c3 = m_pNightColors[i] & 0x0000ff00;

		c0 = ((c0 * d + c2 * n) >> 8) & 0x00ff00ff;
		c1 = ((c1 * d + c3 * n) >> 8) & 0x0000ff00;
		m_pVerts[i].diffuse = 0xff000000 | c0 | c1;
	}
    */
    if (m_pStream && (m_vertFormat == A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_DIFFUSE_UV))
        UpdateStream(true);
	return true;
}

// Register for render
bool A3DLitMesh::RegisterForRender(A3DViewport * pViewport)
{
	if( !m_bVisible )
		return true;

	m_vDisToCam = sqrtf(a3d_DistSquareToAABB(pViewport->GetCamera()->GetPos(), m_pParentModel->GetModelAABB()));

	if (GetTexturePtr()->IsShaderTexture() || GetAlphaValue() < 255) //alpha texture
	{
    	m_pA3DDevice->GetA3DEngine()->GetA3DLitModelRender()->RegisterRenderMesh(pViewport, this);
		return true;
	}

	if( m_bSupportLM && m_bUseLM) //lightmap
	{
		//m_pA3DDevice->GetA3DEngine()->GetA3DLitModelRender()->RegisterLightMapMesh(pViewport, this);
	}
	else 
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DLitModelRender()->RegisterVertLitMesh(pViewport, this);
	}


	return true;
}

// below function is in the A3DMesh.cpp
extern bool RayIntersectTriangle(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3 * pTriVerts, float * pvFraction, A3DVECTOR3 * pVecPos);

bool A3DLitMesh::RayTraceMesh(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, int * pIdFace)
{
	float			vFractionThis;
	A3DVECTOR3		vecHitPosThis;
	float			vMinFraction = 99999.9f;
	A3DVECTOR3		v0, v1, v2;

	int				n0, n1, n2;
	A3DVECTOR3		v[3];
	for(int i=0; i<m_nFaceCount*3; i+=3)
	{
		n0 = m_pIndices[i];
		n1 = m_pIndices[i + 1];
		n2 = m_pIndices[i + 2];

		v[0] = m_pVerts[n0].pos;
		v[1] = m_pVerts[n1].pos;
		v[2] = m_pVerts[n2].pos;

		if( RayIntersectTriangle(vecStart, vecDelta, v, &vFractionThis, &vecHitPosThis) )
		{
			if( vFractionThis < vMinFraction )
			{
				*pvFraction = vMinFraction = vFractionThis;
				vecHitPos = vecHitPosThis;
				v0 = v[0]; v1 = v[1]; v2 = v[2];
				if( pIdFace )
					*pIdFace = i/3;
			}
		}
	}

	if( vMinFraction < 1.0f )
	{
		// Should calculate normal here;
		vecNormal = Normalize(CrossProduct(v1 - v0, v2 - v1));
		return true;
	}

	return false;
}

// Save and load
bool A3DLitMesh::Save(AFile * pFileToSave)
{
	DWORD dwWriteLen;
	int i;

	A3DLMVERTEX_WITHOUTNORMAL * pVerts = NULL;

	// first output the version
	DWORD dwVersion = A3DLITMESH_VERSION;
	if( m_bSupportLM)
	{
		dwVersion = A3DLITMESH_SUPPORT_LIGHTMAP_VERSION; // A3DLITMESH_SUPPORT_LIGHTMAP_VERSION_01
	}
	if( !pFileToSave->Write(&dwVersion, sizeof(DWORD), &dwWriteLen) )
		goto WRITEFAIL;

	// now, output the name of this mesh
	char szName[64];
	szName[63] = '\0';
	strncpy(szName, GetName(), 63);
	if(	!pFileToSave->Write(szName, 64, &dwWriteLen) )
		goto WRITEFAIL;

	if( !pFileToSave->Write(m_szTextureMap, 256, &dwWriteLen) )
		goto WRITEFAIL;

	// Save Effect Param
	// TODO ...

	// now output the vertex count and face count
	if( !pFileToSave->Write(&m_nVertCount, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_nFaceCount, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;

	if (!m_bSupportLM)
	{
		if( !pFileToSave->Write(&m_bHasExtraColors, sizeof(bool), &dwWriteLen) )
			goto WRITEFAIL;
	}

	// then output the vertex and indices
	pVerts = new A3DLMVERTEX_WITHOUTNORMAL[m_nVertCount];
	for(i=0; i<m_nVertCount; i++)
	{
		pVerts[i].pos = m_pVerts[i].pos;
		pVerts[i].diffuse = m_pVerts[i].diffuse;
		pVerts[i].u = m_pVerts[i].u;
		pVerts[i].v = m_pVerts[i].v;
	}
	if( !pFileToSave->Write(pVerts, sizeof(A3DLMVERTEX_WITHOUTNORMAL) * m_nVertCount, &dwWriteLen) )
		goto WRITEFAIL;
	delete [] pVerts;

	if( !pFileToSave->Write(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount, &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount, &dwWriteLen) )
		goto WRITEFAIL;

	if (!m_bSupportLM)
	{
		if (m_bHasExtraColors)
		{
			if( !pFileToSave->Write(m_pDayColorsExtra, sizeof(A3DCOLOR) * m_nVertCount, &dwWriteLen) )
				goto WRITEFAIL;
			if( !pFileToSave->Write(m_pNightColorsExtra, sizeof(A3DCOLOR) * m_nVertCount, &dwWriteLen) )
				goto WRITEFAIL;
		}
	}

	// output the aabb
	if( !pFileToSave->Write(&m_aabb, sizeof(A3DAABB), &dwWriteLen) )
		goto WRITEFAIL;

	// output the material of this mesh
	if( !m_Material.Save(pFileToSave) )
		goto WRITEFAIL;

	//Save light map coords
	if( m_bSupportLM)
	{
		if( !pFileToSave->Write(m_pLMCoords, sizeof( A3DLIGHTMAPCOORD) * m_nVertCount, &dwWriteLen))
			goto WRITEFAIL;
	
		if( !pFileToSave->Write(&m_bUseLM, sizeof(bool), &dwWriteLen) )
			goto WRITEFAIL;
	}

    return true;

WRITEFAIL:
	g_A3DErrLog.Log("A3DLitMesh::Save(), write to the file failed!");
	return false;
}

bool A3DLitMesh::DummyLoad(AFile * pFileToLoad)
{
	DWORD dwReadLen;

	// first read the version
	DWORD dwVersion;
	if( !pFileToLoad->Read(&dwVersion, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;

	if( dwVersion == 0x10000001 )
	{
		return false;
	}
	else if( dwVersion == 0x10000002 )
	{
		// now, skip the name of this mesh
		pFileToLoad->Seek(sizeof(char) * 64, AFILE_SEEK_CUR);
		// now, skip the texturemap of this mesh
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		// then skip the vertex and indices
		pFileToLoad->Seek(sizeof(A3DLVERTEX) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(WORD) * nFaceCount * 3, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * nVertCount, AFILE_SEEK_CUR);

		// skip aabb
		pFileToLoad->Seek(sizeof(A3DAABB), AFILE_SEEK_CUR);
	}
	else if( dwVersion == 0x10000003 )
	{
		// now, skip the name of this mesh
		pFileToLoad->Seek(sizeof(char) * 64, AFILE_SEEK_CUR);
		// now, skip the texturemap of this mesh
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		// then skip the vertex and indices and colors
		pFileToLoad->Seek(sizeof(A3DLVERTEX) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(WORD) * nFaceCount * 3, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);

		// skip aabb
		pFileToLoad->Seek(sizeof(A3DAABB), AFILE_SEEK_CUR);
	}
	else if( dwVersion == 0x10000004 )
	{
		// now, skip the name of this mesh
		pFileToLoad->Seek(sizeof(char) * 64, AFILE_SEEK_CUR);
		// now, skip the texturemap of this mesh
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		// then skip the vertex and indices and colors
		pFileToLoad->Seek(sizeof(A3DLMVERTEX_WITHOUTNORMAL) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(WORD) * nFaceCount * 3, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);

		// skip aabb
		pFileToLoad->Seek(sizeof(A3DAABB), AFILE_SEEK_CUR);
	}
	else if( dwVersion == 0x10000005 )
	{
		// now, skip the name of this mesh
		pFileToLoad->Seek(sizeof(char) * 64, AFILE_SEEK_CUR);
		// now, skip the texturemap of this mesh
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		// then skip the vertex and indices and colors
		pFileToLoad->Seek(sizeof(A3DLMVERTEX_WITHOUTNORMAL) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(WORD) * nFaceCount * 3, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);

		// skip aabb
		pFileToLoad->Seek(sizeof(A3DAABB), AFILE_SEEK_CUR);


		A3DMaterial material;
		// load the material
		if( !material.Load(NULL, pFileToLoad) )
			goto READFAIL;
	}
	else if( dwVersion == 0x10000006 )
	{
		// now, skip the name of this mesh
		pFileToLoad->Seek(sizeof(char) * 64, AFILE_SEEK_CUR);
		// now, skip the texturemap of this mesh
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);

		int nVertCount, nFaceCount;
		bool bHasExtraColor;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		if( !pFileToLoad->Read(&bHasExtraColor, sizeof(bool), &dwReadLen) )
			goto READFAIL;

		// then skip the vertex and indices and colors
		pFileToLoad->Seek(sizeof(A3DLMVERTEX_WITHOUTNORMAL) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(WORD) * nFaceCount * 3, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);

		if (bHasExtraColor)
		{
			pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
			pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		}

		// skip aabb
		pFileToLoad->Seek(sizeof(A3DAABB), AFILE_SEEK_CUR);


		A3DMaterial material;
		// load the material
		if( !material.Load(NULL, pFileToLoad) )
			goto READFAIL;
	}
	else if( dwVersion == 0x10000100 )
	{
		// now, skip the name of this mesh
		pFileToLoad->Seek(sizeof(char) * 64, AFILE_SEEK_CUR);
		// now, skip the texturemap of this mesh
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);
		
		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		
		// then skip the vertex and indices and colors
		pFileToLoad->Seek(sizeof(A3DLMVERTEX_WITHOUTNORMAL) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(WORD) * nFaceCount * 3, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		
		// skip aabb
		pFileToLoad->Seek(sizeof(A3DAABB), AFILE_SEEK_CUR);
		
		
		A3DMaterial material;
		// load the material
		if( !material.Load(NULL, pFileToLoad) )
			goto READFAIL;

		//skip light map coords
		pFileToLoad->Seek(sizeof(A3DLIGHTMAPCOORD)* nVertCount, AFILE_SEEK_CUR);

	}
	else if( dwVersion == A3DLITMESH_SUPPORT_LIGHTMAP_VERSION )
	{
		// now, skip the name of this mesh
		pFileToLoad->Seek(sizeof(char) * 64, AFILE_SEEK_CUR);
		// now, skip the texturemap of this mesh
		pFileToLoad->Seek(sizeof(char) * 256, AFILE_SEEK_CUR);

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		// then skip the vertex and indices and colors
		pFileToLoad->Seek(sizeof(A3DLMVERTEX_WITHOUTNORMAL) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(WORD) * nFaceCount * 3, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);
		pFileToLoad->Seek(sizeof(A3DCOLOR) * nVertCount, AFILE_SEEK_CUR);

		// skip aabb
		pFileToLoad->Seek(sizeof(A3DAABB), AFILE_SEEK_CUR);


		A3DMaterial material;
		// load the material
		if( !material.Load(NULL, pFileToLoad) )
			goto READFAIL;

		//skip light map coords
		pFileToLoad->Seek(sizeof(A3DLIGHTMAPCOORD)* nVertCount, AFILE_SEEK_CUR);

		//skip bUselight
		if( !pFileToLoad->Seek(sizeof(bool), AFILE_SEEK_CUR) )
			goto READFAIL;
		
	}
	else
	{
		g_A3DErrLog.Log("A3DLitMesh::DummyLoad(), Unknown version [%x]!", dwVersion);
		return false;
	}
	return true;

READFAIL:
	g_A3DErrLog.Log("A3DLitMesh::DummyLoad(), read from the file failed!");
	return false;
}

bool A3DLitMesh::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
{
	ReleaseStream();
    DWORD dwReadLen;

	// first read the version
	DWORD dwVersion;
	if( !pFileToLoad->Read(&dwVersion, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;

	if( dwVersion == 0x10000001 )
	{
		return false;
	}
	else if( dwVersion == 0x10000002 )
	{
		// now, read in the name of this mesh
		char szName[64];
		if(	!pFileToLoad->Read(szName, 64, &dwReadLen) )
			goto READFAIL;
		SetName(szName);

		char szTextureMap[256];
		if( !pFileToLoad->Read(szTextureMap, 256, &dwReadLen) )
			goto READFAIL;

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		if( !Create(pA3DDevice, nVertCount, nFaceCount, szTextureMap) )
		{
			g_A3DErrLog.Log("A3DLitMesh::Load(), failed to call Create method!");
			return false;
		}

		A3DLVERTEX * pVerts = new A3DLVERTEX[m_nVertCount];
		// then read in the vertex and indices
		if( !pFileToLoad->Read(pVerts, sizeof(A3DLVERTEX) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwReadLen) )
			goto READFAIL;

		// now set day colors to those colors in m_pVerts;
		for(int i=0; i<m_nVertCount; i++)
		{
			m_pVerts[i].pos = A3DVECTOR3(pVerts[i].x, pVerts[i].y, pVerts[i].z);
			m_pVerts[i].normal = m_pNormals[i];
			m_pVerts[i].u = pVerts[i].tu;
			m_pVerts[i].v = pVerts[i].tv;
			
			m_pDayColors[i] = m_pVerts[i].diffuse;
			m_pNightColors[i] = m_pVerts[i].diffuse;
		}

		delete [] pVerts;

		m_fDNFactor = -1.0f;

		// last read in the aabb
		if( !pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &dwReadLen) )
			goto READFAIL;

	}
	else if( dwVersion == 0x10000003 )
	{
		// now, read in the name of this mesh
		char szName[64];
		if(	!pFileToLoad->Read(szName, 64, &dwReadLen) )
			goto READFAIL;
		SetName(szName);

		char szTextureMap[256];
		if( !pFileToLoad->Read(szTextureMap, 256, &dwReadLen) )
			goto READFAIL;

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		if( !Create(pA3DDevice, nVertCount, nFaceCount, szTextureMap) )
		{
			g_A3DErrLog.Log("A3DLitMesh::Load(), failed to call Create method!");
			return false;
		}

		A3DLVERTEX * pVerts = new A3DLVERTEX[m_nVertCount];
		// then read in the vertex and indices
		if( !pFileToLoad->Read(pVerts, sizeof(A3DLVERTEX) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;

		// now set day colors to those colors in m_pVerts;
		for(int i=0; i<m_nVertCount; i++)
		{
			m_pVerts[i].pos = A3DVECTOR3(pVerts[i].x, pVerts[i].y, pVerts[i].z);
			m_pVerts[i].normal = m_pNormals[i];
			m_pVerts[i].u = pVerts[i].tu;
			m_pVerts[i].v = pVerts[i].tv;
		}

		delete [] pVerts;
		m_fDNFactor = -1.0f;

		// last read in the aabb
		if( !pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &dwReadLen) )
			goto READFAIL;

	}
	else if( dwVersion == 0x10000004 )
	{
		// now, read in the name of this mesh
		char szName[64];
		if(	!pFileToLoad->Read(szName, 64, &dwReadLen) )
			goto READFAIL;
		SetName(szName);

		char szTextureMap[256];
		if( !pFileToLoad->Read(szTextureMap, 256, &dwReadLen) )
			goto READFAIL;

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		if( !Create(pA3DDevice, nVertCount, nFaceCount, szTextureMap) )
		{
			g_A3DErrLog.Log("A3DLitMesh::Load(), failed to call Create method!");
			return false;
		}

		// then read in the vertex and indices
		A3DLMVERTEX_WITHOUTNORMAL * pVerts = new A3DLMVERTEX_WITHOUTNORMAL[m_nVertCount];
		if( !pFileToLoad->Read(pVerts, sizeof(A3DLMVERTEX_WITHOUTNORMAL) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		for(int i=0; i<m_nVertCount; i++)
		{
			m_pVerts[i].pos = pVerts[i].pos;
			m_pVerts[i].diffuse = pVerts[i].diffuse;
			m_pVerts[i].u = pVerts[i].u;
			m_pVerts[i].v = pVerts[i].v;
			m_pVerts[i].normal = m_pNormals[i];
		}
		delete [] pVerts;

		m_fDNFactor = -1.0f;

		// last read in the aabb
		if( !pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &dwReadLen) )
			goto READFAIL;
	}
	else if( dwVersion == 0x10000005 )
	{
		// now, read in the name of this mesh
		char szName[64];
		if(	!pFileToLoad->Read(szName, 64, &dwReadLen) )
			goto READFAIL;
		SetName(szName);

		char szTextureMap[256];
		if( !pFileToLoad->Read(szTextureMap, 256, &dwReadLen) )
			goto READFAIL;

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		if( !Create(pA3DDevice, nVertCount, nFaceCount, szTextureMap) )
		{
			g_A3DErrLog.Log("A3DLitMesh::Load(), failed to call Create method!");
			return false;
		}

		// then read in the vertex and indices
		A3DLMVERTEX_WITHOUTNORMAL * pVerts = new A3DLMVERTEX_WITHOUTNORMAL[m_nVertCount];
		if( !pFileToLoad->Read(pVerts, sizeof(A3DLMVERTEX_WITHOUTNORMAL) * m_nVertCount, &dwReadLen) )
			goto READFAIL;

		if( !pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		for(int i=0; i<m_nVertCount; i++)
		{
			m_pVerts[i].pos = pVerts[i].pos;
			m_pVerts[i].diffuse = pVerts[i].diffuse;
			m_pVerts[i].u = pVerts[i].u;
			m_pVerts[i].v = pVerts[i].v;
			m_pVerts[i].normal = m_pNormals[i];
		}
		delete [] pVerts;

		m_fDNFactor = -1.0f;

		// read in the aabb
		if( !pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &dwReadLen) )
			goto READFAIL;

		// load the material
		if( !m_Material.Load(pA3DDevice, pFileToLoad) )
			goto READFAIL;
	}
	else if (dwVersion == A3DLITMESH_VERSION)
	{
		// now, read in the name of this mesh
		char szName[64];
		if(	!pFileToLoad->Read(szName, 64, &dwReadLen) )
			goto READFAIL;
		SetName(szName);

		char szTextureMap[256];
		if( !pFileToLoad->Read(szTextureMap, 256, &dwReadLen) )
			goto READFAIL;

		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;

		if( !pFileToLoad->Read(&m_bHasExtraColors, sizeof(bool), &dwReadLen) )
			goto READFAIL;

		if( !Create(pA3DDevice, nVertCount, nFaceCount, szTextureMap) )
		{
			g_A3DErrLog.Log("A3DLitMesh::Load(), failed to call Create method!");
			return false;
		}

		// then read in the vertex and indices
		A3DLMVERTEX_WITHOUTNORMAL * pVerts = new A3DLMVERTEX_WITHOUTNORMAL[m_nVertCount];
		if( !pFileToLoad->Read(pVerts, sizeof(A3DLMVERTEX_WITHOUTNORMAL) * m_nVertCount, &dwReadLen) )
			goto READFAIL;

		if( !pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;

		if (m_bHasExtraColors)
		{
			if( !pFileToLoad->Read(m_pDayColorsExtra, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
				goto READFAIL;
			if( !pFileToLoad->Read(m_pNightColorsExtra, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
				goto READFAIL;
		}

		for(int i=0; i<m_nVertCount; i++)
		{
			m_pVerts[i].pos = pVerts[i].pos;
			m_pVerts[i].diffuse = pVerts[i].diffuse;
			m_pVerts[i].u = pVerts[i].u;
			m_pVerts[i].v = pVerts[i].v;
			m_pVerts[i].normal = m_pNormals[i];
		}
		delete [] pVerts;

		m_fDNFactor = -1.0f;

		// read in the aabb
		if( !pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &dwReadLen) )
			goto READFAIL;

		// load the material
		if( !m_Material.Load(pA3DDevice, pFileToLoad) )
			goto READFAIL;
	}
	else if( dwVersion == 0x10000100 ) //old light map litmesh
	{
		// now, read in the name of this mesh
		char szName[64];
		if(	!pFileToLoad->Read(szName, 64, &dwReadLen) )
			goto READFAIL;
		SetName(szName);
		
		char szTextureMap[256];
		if( !pFileToLoad->Read(szTextureMap, 256, &dwReadLen) )
			goto READFAIL;
		
		int nVertCount, nFaceCount;
		// now read the vertex count and face count
		if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
			goto READFAIL;
		
		if( !Create(pA3DDevice, nVertCount, nFaceCount, szTextureMap, true) )
		{
			g_A3DErrLog.Log("A3DLitMesh::Load(), failed to call Create method!");
			return false;
		}
		
		// then read in the vertex and indices
		A3DLMVERTEX_WITHOUTNORMAL * pVerts = new A3DLMVERTEX_WITHOUTNORMAL[m_nVertCount];
		if( !pFileToLoad->Read(pVerts, sizeof(A3DLMVERTEX_WITHOUTNORMAL) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		
		if( !pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		for(int i=0; i<m_nVertCount; i++)
		{
			m_pVerts[i].pos = pVerts[i].pos;
			m_pVerts[i].diffuse = pVerts[i].diffuse;
			m_pVerts[i].u = pVerts[i].u;
			m_pVerts[i].v = pVerts[i].v;
			m_pVerts[i].normal = m_pNormals[i];
		}
		delete [] pVerts;
		
		m_fDNFactor = -1.0f;
		
		// read in the aabb
		if( !pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &dwReadLen) )
			goto READFAIL;
		
		// load the material
		if( !m_Material.Load(pA3DDevice, pFileToLoad) )
			goto READFAIL;

		// load light map coords
		if( !pFileToLoad->Read( m_pLMCoords, sizeof(A3DLIGHTMAPCOORD) * m_nVertCount, &dwReadLen))
			goto READFAIL;
		m_bUseLM = true;
		
	}
	else if( dwVersion == A3DLITMESH_SUPPORT_LIGHTMAP_VERSION || 
		dwVersion == A3DLITMESH_SUPPORT_LIGHTMAP_VERSION_01)
	{
		// now, read in the name of this mesh
		char szName[64];
		if(	!pFileToLoad->Read(szName, 64, &dwReadLen) )
			goto READFAIL;
		SetName(szName);

		char szTextureMap[256];
		if( !pFileToLoad->Read(szTextureMap, 256, &dwReadLen) )
			goto READFAIL;

        int nVertCount, nFaceCount;
        // now read the vertex count and face count
        if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
            goto READFAIL;
        if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
            goto READFAIL;

        // Load Effect Param
		if(dwVersion == A3DLITMESH_SUPPORT_LIGHTMAP_VERSION_01)
		{

		}

        if( !Create(pA3DDevice, nVertCount, nFaceCount, szTextureMap, true) )
		{
			g_A3DErrLog.Log("A3DLitMesh::Load(), failed to call Create method!");
			return false;
		}

		// then read in the vertex and indices
		A3DLMVERTEX_WITHOUTNORMAL * pVerts = new A3DLMVERTEX_WITHOUTNORMAL[m_nVertCount];
		if( !pFileToLoad->Read(pVerts, sizeof(A3DLMVERTEX_WITHOUTNORMAL) * m_nVertCount, &dwReadLen) )
			goto READFAIL;

		if( !pFileToLoad->Read(m_pIndices, sizeof(WORD) * m_nFaceCount * 3, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNormals, sizeof(A3DVECTOR3) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pDayColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(m_pNightColors, sizeof(A3DCOLOR) * m_nVertCount, &dwReadLen) )
			goto READFAIL;
		for(int i=0; i<m_nVertCount; i++)
		{
			m_pVerts[i].pos = pVerts[i].pos;
			m_pVerts[i].diffuse = pVerts[i].diffuse;
			m_pVerts[i].u = pVerts[i].u;
			m_pVerts[i].v = pVerts[i].v;
			m_pVerts[i].normal = m_pNormals[i];
		}
		delete [] pVerts;

		m_fDNFactor = -1.0f;

		// read in the aabb
		if( !pFileToLoad->Read(&m_aabb, sizeof(A3DAABB), &dwReadLen) )
			goto READFAIL;

		// load the material
		if( !m_Material.Load(pA3DDevice, pFileToLoad) )
			goto READFAIL;

		// load light map coords
		if( !pFileToLoad->Read( m_pLMCoords, sizeof(A3DLIGHTMAPCOORD) * m_nVertCount, &dwReadLen))
			goto READFAIL;

		if( !pFileToLoad->Read(&m_bUseLM, sizeof(bool), &dwReadLen) )
			goto READFAIL;
	}
	else
	{
		g_A3DErrLog.Log("A3DLitMesh::Load(), Unknown version [%x]!", dwVersion);
		return false;
	}

    if (!CreateStream())
    {
        g_A3DErrLog.Log("A3DLitMesh::Load(), create stream error!");
        return false;
    }

    return true;

READFAIL:
	g_A3DErrLog.Log("A3DLitMesh::Load(), read from the file failed!");
	return false;
}

// Change texture map
bool A3DLitMesh::ChangeTextureMap(const char * szTextureMap)
{
	strcpy(m_szTextureMap, szTextureMap);

	return LoadTexture();
}

const A3DMATRIX4& A3DLitMesh::GetAbsMatrix()
{ 
	return m_pParentModel->m_matAbsoluteTM;
}

A3DTexture*  A3DLitMesh::GetLightMap()
{ 
	return m_pParentModel->GetLightMap();
}
A3DTexture* A3DLitMesh::GetNightLightMap()
{
	return m_pParentModel->GetNightLightMap();
}

const char*  A3DLitMesh::GetLightMapName() 
{ 
	return m_pParentModel->GetLightMapName();
}

const char*  A3DLitMesh::GetNightLightMapName() 
{ 
	return m_pParentModel->GetNightLightMapName();
}

void A3DLitMesh::SetExtraColors(bool bApplay)
{
	if (bApplay)
	{
		if (m_bHasExtraColors)
			return;

		m_bHasExtraColors = true;
		m_pDayColorsExtra = new A3DCOLOR[m_nVertCount];
		m_pNightColorsExtra = new A3DCOLOR[m_nVertCount];
		memset(m_pDayColorsExtra, 0, sizeof(A3DCOLOR) * m_nVertCount);
		memset(m_pNightColorsExtra, 0, sizeof(A3DCOLOR) * m_nVertCount);
	}
	else
	{
		m_bHasExtraColors = false;
		delete[] m_pDayColorsExtra;
		m_pDayColorsExtra = NULL;
		delete[] m_pNightColorsExtra;
		m_pNightColorsExtra = NULL;
	}
}

bool A3DLitMesh::CreateStream()
{
    ReleaseStream();

    m_pStream = new A3DStream;

    if (m_bSupportLM && m_bUseLM && m_pLMCoords)  // Light map stream
    {
        if (!m_pStream || !m_pStream->Init(m_pA3DDevice, sizeof(A3DLMVERTEX_POS_NORMAL_UV_UV), 
            A3DLMVERTEX_LIGHTMAP_NOVERT_FVF, m_nVertCount, m_nFaceCount * 3, A3DSTRM_STATIC, A3DSTRM_STATIC))
        {
            A3DRELEASE(m_pStream);
            g_A3DErrLog.Log("A3DLitMesh::CreateStream(), stream init failed!");
            return false;
        }
		m_vertFormat = A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_UV1_UV2;
    }
    else    // Vertex light
    {
        if (!m_pStream || !m_pStream->Init(m_pA3DDevice, sizeof(A3DLMVERTEX_POS_NORMAL_DIFFUSE_UV), 
            A3DLMVERTEX_WITHOUTNORMAL_FVF, m_nVertCount, m_nFaceCount * 3, A3DSTRM_STATIC, A3DSTRM_STATIC))
        {
            A3DRELEASE(m_pStream);
            g_A3DErrLog.Log("A3DLitMesh::CreateStream(), stream init failed!");
            return false;
        }
		m_vertFormat = A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_DIFFUSE_UV;
    }

    return UpdateStream(false);
}

void A3DLitMesh::ReleaseStream()
{
    A3DRELEASE(m_pStream);
}


bool A3DLitMesh::UpdateStream(bool bDynamic)
{
    if (m_bSupportLM && m_bUseLM && m_pLMCoords)  // Light map stream
    {
		A3DLMVERTEX_POS_NORMAL_UV_UV* pVertexBuffer = NULL;
        if (m_vertFormat != (A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_UV1_UV2))
            return CreateStream();

        if (bDynamic)
        {
            if (!m_pStream || !m_pStream->LockVertexBufferDynamic(0, sizeof(A3DLMVERTEX_POS_NORMAL_UV_UV) * m_nVertCount, (BYTE**)&pVertexBuffer, 0))
            {
                A3DRELEASE(m_pStream);
                g_A3DErrLog.Log("A3DLitMesh::CreateStream(), LockVertexBufferDynamic failed!");
                return false;
            }
        }
        else
        {
            if (!m_pStream || !m_pStream->LockVertexBuffer(0, sizeof(A3DLMVERTEX_POS_NORMAL_UV_UV) * m_nVertCount, (BYTE**)&pVertexBuffer, 0))
            {
                A3DRELEASE(m_pStream);
                g_A3DErrLog.Log("A3DLitMesh::CreateStream(), LockVertexBuffer failed!");
                return false;
            }
        }

        for (int i = 0; i < m_nVertCount; i++)
        {
            pVertexBuffer[i].pos = m_pVerts[i].pos;
			pVertexBuffer[i].normal = m_pVerts[i].normal;
            pVertexBuffer[i].u = m_pVerts[i].u;
            pVertexBuffer[i].v = m_pVerts[i].v;
            pVertexBuffer[i].lm_u = m_pLMCoords[i].u;
            pVertexBuffer[i].lm_v = m_pLMCoords[i].v;
        }
        if (bDynamic)
            m_pStream->UnlockVertexBufferDynamic();
        else
            m_pStream->UnlockVertexBuffer();
    }
    else // vertex light
    {
        A3DLMVERTEX_POS_NORMAL_DIFFUSE_UV* pVertexBuffer = NULL;
        if (m_vertFormat != (A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_DIFFUSE_UV))
            return CreateStream();

        if (bDynamic)
        {
            if (!m_pStream || !m_pStream->LockVertexBufferDynamic(0, sizeof(A3DLMVERTEX_POS_NORMAL_DIFFUSE_UV) * m_nVertCount, (BYTE**)&pVertexBuffer, 0))
            {
                A3DRELEASE(m_pStream);
                g_A3DErrLog.Log("A3DLitMesh::CreateStream(), LockVertexBufferDynamic failed!");
                return false;
            }
        }
        else
        {
            if (!m_pStream || !m_pStream->LockVertexBuffer(0, sizeof(A3DLMVERTEX_POS_NORMAL_DIFFUSE_UV) * m_nVertCount, (BYTE**)&pVertexBuffer, 0))
            {
                A3DRELEASE(m_pStream);
                g_A3DErrLog.Log("A3DLitMesh::CreateStream(), LockVertexBuffer failed!");
                return false;
            }
        }
        for (int i = 0; i < m_nVertCount; i++)
        {
            pVertexBuffer[i].pos = m_pVerts[i].pos;
			pVertexBuffer[i].normal = m_pVerts[i].normal;
            pVertexBuffer[i].diffuse = m_pVerts[i].diffuse;
            pVertexBuffer[i].u = m_pVerts[i].u;
            pVertexBuffer[i].v = m_pVerts[i].v;
        }
        if (bDynamic)
            m_pStream->UnlockVertexBufferDynamic();
        else
            m_pStream->UnlockVertexBuffer();
    }

    WORD* pIndexBuffer = NULL;

    if (bDynamic)
    {
        if (!m_pStream->LockIndexBufferDynamic(0, sizeof(WORD) * m_nFaceCount * 3, (BYTE**)&pIndexBuffer, 0))
        {
            A3DRELEASE(m_pStream);
            g_A3DErrLog.Log("A3DLitMesh::CreateStream(), LockIndexBuffer failed!");
            return false;
        }
    }
    else 
    {
        if (!m_pStream->LockIndexBuffer(0, sizeof(WORD) * m_nFaceCount * 3, (BYTE**)&pIndexBuffer, 0))
        {
            A3DRELEASE(m_pStream);
            g_A3DErrLog.Log("A3DLitMesh::CreateStream(), LockIndexBuffer failed!");
            return false;
        }
    }

    for (int i = 0; i < m_nFaceCount * 3; i++)
    {
        pIndexBuffer[i] = m_pIndices[i];
    }

    if (bDynamic)
        m_pStream->UnlockIndexBufferDynamic();
    else
        m_pStream->UnlockIndexBuffer();

    return true;
}

bool A3DLitMesh::SetTexturePtr(A3DTexture* pNewTexture)
{
	if( m_pA3DTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pA3DTexture);
		m_pA3DTexture = NULL;
	}

	m_pA3DTexture = pNewTexture;
	return true;
}

void A3DLitMesh::SetUseLightMap(bool bUseLM)
{
    if (m_bSupportLM && m_bUseLM != bUseLM)
    {
		m_bUseLM = bUseLM;
        CreateStream();
    }
    m_bUseLM = bUseLM;
}

void A3DLitMesh::UpdateColorByDayNight()
{
    float fDNFactor = m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetDNFactor();
    float delta = (float)fabs(fDNFactor - m_fDNFactor);
    // see if we need update the diffuse colors in m_pVerts
    if( delta > 1.0f / 256.0f )
    {
        m_fDNFactor = fDNFactor;
        UpdateColors();
    }
}

A3DLitMeshSharedData* A3DLitMesh::ExportSharedData() const
{
    A3DLitMeshSharedData* pShared = new A3DLitMeshSharedData;
    pShared->Init(m_nVertCount, m_nFaceCount * 3, m_bSupportLM);
    for (int i = 0; i < m_nVertCount; i++)
    {
        pShared->GetPositions()[i] = m_pVerts[i].pos;
        pShared->GetNormals()[i] = m_pVerts[i].normal;
        pShared->GetUVs()[i * 2] = m_pVerts[i].u;
        pShared->GetUVs()[i * 2 + 1] = m_pVerts[i].v;
        if (m_bSupportLM)
        {
            pShared->GetLMUVs()[i * 2] = m_pLMCoords[i].u;
            pShared->GetLMUVs()[i * 2 + 1] = m_pLMCoords[i].v;
        }
    }
    memcpy(pShared->GetIndices(), m_pIndices, sizeof(WORD) * m_nFaceCount * 3);
    pShared->SetTextureMap(m_szTextureMap);
    pShared->SetMaterial(m_Material);
    return pShared;
}

void A3DLitMesh::SetDetailTexture(const char* szDetailMap)
{
    m_strDetailMap = szDetailMap;
    A3DTexture* pOldTex = m_pDetailTexture;
    m_pDetailTexture = NULL;
    if (szDetailMap && szDetailMap[0])
    {

        if (!m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(szDetailMap, &m_pDetailTexture, 0, 0))
        {
            g_A3DErrLog.Log("A3DLitMesh::SetDetailTexture(), Load detail texture failed!");
        }
    }
    m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(pOldTex);
}

//////////////////////////////////////////////////////////////////////////
// class A3DLitModel
//		
//		A model that was pre-lit
//////////////////////////////////////////////////////////////////////////
int A3DLITMODEL_TOTAL_COUNT = 0;
A3DLitModel::A3DLitModel()
: m_vecScale(1.0f, 1.0f, 1.0f), m_vecPos(0.0f), m_vecDir(0.0f, 0.0f, 1.0f), m_vecUp(0.0f, 1.0f, 0.0f)
{
	m_pA3DDevice			= NULL;
    m_pA3DEngine            = NULL;
	m_bVisible				= true;

	m_ModelAABB.Clear();
	m_matAbsoluteTM.Identity();
	m_matNormalizedTM.Identity();
	m_bNoTransform			= true;

	memset(m_szLightMap, 0, sizeof(m_szLightMap));
	m_pLMTexture			= NULL;

	memset(m_szNightLightMap, 0, sizeof(m_szNightLightMap));
	m_pNightLMTexture		= NULL;
    
    m_pOccProxy = NULL;
	A3DLITMODEL_TOTAL_COUNT++;
}

A3DLitModel::~A3DLitModel()
{
	A3DLITMODEL_TOTAL_COUNT--;
	Release();
}

	
// Adjust scale factor
bool A3DLitModel::SetScale(float vScaleX, float vScaleY, float vScaleZ)
{
	m_vecScale.x = vScaleX;
	m_vecScale.y = vScaleY;
	m_vecScale.z = vScaleZ;

	return UpdateAbsoluteTM();
}

// Initialize and finalize
bool A3DLitModel::Init(A3DDevice * pA3DDevice)
{
	// try to release old ones.
	Release();

	m_pA3DDevice		= pA3DDevice;
    m_pA3DEngine        = pA3DDevice->GetA3DEngine();

	return true;
}

bool A3DLitModel::Release()
{
	int nNumMeshes = m_Meshes.GetSize();

	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];

		pMesh->Release();
		delete pMesh;
	}

	m_Meshes.RemoveAll();

	ReleaseLightMap();

    if (m_pOccProxy)
    {
        if (m_pA3DEngine && m_pA3DEngine->GetA3DOcclusionMan())
            m_pA3DEngine->GetA3DOcclusionMan()->ReleaseQuery(m_pOccProxy);
        m_pOccProxy = NULL;
    }

	return true;
}

// Render methods
bool A3DLitModel::Render(A3DViewport * pViewport, bool bUpdateShadowMap)
{
	//register render model
	if (!pViewport->GetCamera()->AABBInViewFrustum(m_ModelAABB.Mins, m_ModelAABB.Maxs) ||
		!m_bVisible)
		return true;

    //occlusion culling
    if (m_pOccProxy && g_pA3DConfig->RT_GetUseOcclusionCullingFlag())
    {
        if (m_pOccProxy->IsOccluded(pViewport))
            return true;
    }

	//// now register this model's meshes in lit model renderer
	//int nNumMeshes = m_Meshes.GetSize();
	//for(int i=0; i<nNumMeshes; i++)
	//{
	//	A3DLitMesh * pMesh = m_Meshes[i];
	//	const A3DAABB& aabb = pMesh->GetAABB();

	//	if( m_bNoTransform && !pViewport->GetCamera()->AABBInViewFrustum(aabb.Mins, aabb.Maxs) )
	//		continue;
	//	
	//	pMesh->RegisterForRender(pViewport);
	//}
    
    m_pA3DEngine->GetA3DLitModelRender()->RegisterRenderModel(pViewport, this);

	//show bounding box
	if (g_pA3DConfig->RT_GetShowBoundBoxFlag())
	{
        A3DWireCollector* pWireCollector = m_pA3DDevice->GetA3DEngine()->GetA3DWireCollector();
		pWireCollector->AddAABB(m_ModelAABB, A3DCOLORRGBA(0, 255, 0, 255));
	}

	return true;
}

// Visibility Adjustion
void A3DLitModel::SetVisibility(bool bVisible)
{
	m_bVisible = bVisible;
	return;
}

bool A3DLitModel::NormalizeTransforms()
{
	if( m_bNoTransform )
		return true;

	int nNumMeshes = m_Meshes.GetSize();
	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];
		A3DLMVERTEX * pVerts = pMesh->GetVerts();
		A3DVECTOR3 * pNormals = pMesh->GetNormals();

		A3DAABB		aabb;
		aabb.Clear();

		// first transfrom all verts and normals.
		for(int n=0; n<pMesh->GetNumVerts(); n++)
		{
			A3DVECTOR3 vecPos = pVerts[n].pos * m_matAbsoluteTM;
			pVerts[n].pos = vecPos;
			pNormals[n] = Normalize(a3d_VectorMatrix3x3(pNormals[n], m_matAbsoluteTM));
			pVerts[n].normal = pNormals[n];
			aabb.AddVertex(vecPos);
		}

		aabb.CompleteCenterExts();
		pMesh->SetAABB(aabb);
        pMesh->UpdateStream(false);
	}

	m_matNormalizedTM = m_matNormalizedTM * m_matAbsoluteTM;
	m_matAbsoluteTM = IdentityMatrix();
	m_bNoTransform = true;
    
	return true;
}

bool A3DLitModel::UpdateAbsoluteTM()
{
	m_matAbsoluteTM = InverseTM(m_matNormalizedTM) * Scaling(m_vecScale.x, m_vecScale.y, m_vecScale.z) * TransformMatrix(m_vecDir, m_vecUp, m_vecPos);

	// now update the model's aabb information
	int nNumMeshes = m_Meshes.GetSize();
	m_ModelAABB.Clear();

	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];
		const A3DAABB& aabb = pMesh->GetAABB();

		A3DVECTOR3 verts[8];
		aabb.GetVertices(verts, NULL, false);

		for(int j=0; j<8; j++)
		{
			verts[j] = verts[j] * m_matAbsoluteTM;
			m_ModelAABB.AddVertex(verts[j]);
		}
	}

	m_ModelAABB.CompleteCenterExts();

	if( m_matAbsoluteTM == IdentityMatrix() )
		m_bNoTransform = true;
	else
		m_bNoTransform = false;

    // Update occlusion proxy

    if (m_pOccProxy == NULL)
    {
        A3DOcclusionMan* pOccMan = m_pA3DEngine->GetA3DOcclusionMan();
        if (pOccMan)
        {
            m_pOccProxy = pOccMan->CreateQuery();
        }
    }

    if (m_pOccProxy)
        m_pOccProxy->SetAABB(m_ModelAABB);

	return true;
}

// Direct Set the position and orientation methods
bool A3DLitModel::SetPos(const A3DVECTOR3& vecPos)
{
	m_vecPos = vecPos;

	return UpdateAbsoluteTM();
}

bool A3DLitModel::SetDirAndUp(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp)
{
	m_vecDir = vecDir;
	m_vecUp = vecUp;

	return UpdateAbsoluteTM();
}

// Relatively adjust orientation and position methods
bool A3DLitModel::Move(const A3DVECTOR3& vecDeltaPos)
{
	m_vecPos = m_vecPos + vecDeltaPos;

	return UpdateAbsoluteTM();
}

bool A3DLitModel::RotateX(float vDeltaRad)
{
	A3DMATRIX4 matOrient = TransformMatrix(m_vecDir, m_vecUp, A3DVECTOR3(0.0f));

	matOrient = a3d_RotateX(vDeltaRad) * matOrient;

	m_vecDir = matOrient.GetRow(2);
	m_vecUp = matOrient.GetRow(1);

	return UpdateAbsoluteTM();
}

bool A3DLitModel::RotateY(float vDeltaRad)
{
	A3DMATRIX4 matOrient = TransformMatrix(m_vecDir, m_vecUp, A3DVECTOR3(0.0f));

	matOrient = a3d_RotateY(vDeltaRad) * matOrient;

	m_vecDir = matOrient.GetRow(2);
	m_vecUp = matOrient.GetRow(1);

	return UpdateAbsoluteTM();
}

bool A3DLitModel::RotateZ(float vDeltaRad)
{
	A3DMATRIX4 matOrient = TransformMatrix(m_vecDir, m_vecUp, A3DVECTOR3(0.0f));

	matOrient = a3d_RotateZ(vDeltaRad) * matOrient;

	m_vecDir = matOrient.GetRow(2);
	m_vecUp = matOrient.GetRow(1);

	return UpdateAbsoluteTM();
}

bool A3DLitModel::RayTraceAABB(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal)
{
	// first see if can hit the model's AABB
	if( !CLS_RayToAABB3(vecStart, vecDelta, m_ModelAABB.Mins, m_ModelAABB.Maxs, vecHitPos, pvFraction, vecNormal) )
		return false;

	A3DVECTOR3 vecLocalStart, vecLocalDelta;
	A3DMATRIX4 matIntoModel = m_matAbsoluteTM;
	matIntoModel.InverseTM();

	vecLocalStart = vecStart * matIntoModel;
	vecLocalDelta = a3d_VectorMatrix3x3(vecDelta, matIntoModel);

	float vMinFraction = 99999.9f;
	float vFractionThis = 0.0f;
	A3DVECTOR3 vecLocalHit;
	A3DVECTOR3 vecLocalNormal;

	int nNumMeshes = m_Meshes.GetSize();
	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];

		A3DAABB aabb = pMesh->GetAABB();
		if( CLS_RayToAABB3(vecLocalStart, vecLocalDelta, aabb.Mins, aabb.Maxs, vecLocalHit, &vFractionThis, vecLocalNormal) )
		{
			// see if nearer than the nearest one
			if( vFractionThis < vMinFraction )
			{
				vMinFraction = vFractionThis;
				vecHitPos = vecLocalHit;
				vecNormal = vecLocalNormal;
			}
		}
	}

	if( vMinFraction < 1.0f )
	{
		vecHitPos = vecHitPos * m_matAbsoluteTM;
		vecNormal = a3d_VectorMatrix3x3(vecNormal, m_matAbsoluteTM);
		*pvFraction = vMinFraction;
		return true;
	}

	return false;
}

bool A3DLitModel::RayTraceMesh(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, int * pIdMesh, int * pIdFace)
{
	// first see if can hit the model's AABB
	if( !CLS_RayToAABB3(vecStart, vecDelta, m_ModelAABB.Mins, m_ModelAABB.Maxs, vecHitPos, pvFraction, vecNormal) )
		return false;

	A3DVECTOR3 vecLocalStart, vecLocalDelta;
	A3DMATRIX4 matIntoModel = m_matAbsoluteTM;
	matIntoModel.InverseTM();

	vecLocalStart = vecStart * matIntoModel;
	vecLocalDelta = a3d_VectorMatrix3x3(vecDelta, matIntoModel);

	float vMinFraction = 99999.9f;
	float vFractionThis = 0.0f;
	A3DVECTOR3 vecLocalHit;
	A3DVECTOR3 vecLocalNormal;
	int idMesh = -1;
	int idFace = -1;

	int nNumMeshes = m_Meshes.GetSize();
	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];
		int idFaceThis = -1;

		A3DAABB aabb = pMesh->GetAABB();
		if( CLS_RayToAABB3(vecLocalStart, vecLocalDelta, aabb.Mins, aabb.Maxs, vecLocalHit, &vFractionThis, vecLocalNormal) )
		{
			// then take more accurate test to mesh
			if( pMesh->RayTraceMesh(vecLocalStart, vecLocalDelta, vecLocalHit, &vFractionThis, vecLocalNormal, &idFaceThis) )
			{
				// see if nearer than the nearest one
				if( vFractionThis < vMinFraction )
				{
					vMinFraction = vFractionThis;
					vecHitPos = vecLocalHit;
					vecNormal = vecLocalNormal;
					idMesh = i;
					idFace = idFaceThis;
				}
			}
		}
	}

	if( vMinFraction < 1.0f )
	{
		vecHitPos = vecHitPos * m_matAbsoluteTM;
		vecNormal = a3d_VectorMatrix3x3(vecNormal, m_matAbsoluteTM);
		*pvFraction = vMinFraction;
		if( pIdMesh )
			*pIdMesh = idMesh;
		if( pIdFace )
			*pIdFace = idFace;
		return true;
	}

	return false;
}

// Load and Save
bool A3DLitModel::Save(const char * szFileName)
{
	AFile fileToSave;
	if( !fileToSave.Open(szFileName, AFILE_CREATENEW | AFILE_BINARY) )
	{
		g_A3DErrLog.Log("A3DLitModel::Save(), failed to open file [%s]!", szFileName);
		return false;
	}

	Save(&fileToSave);

	fileToSave.Close();
	return true;
}

bool A3DLitModel::Load(A3DDevice * pA3DDevice, const char * szFileName)
{
	AFileImage fileToLoad;
	if( !fileToLoad.Open(szFileName, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY) )
	{
		g_A3DErrLog.Log("A3DLitModel::Load(), failed to open file [%s]!", szFileName);
		return false;
	}

	Load(pA3DDevice, &fileToLoad);

	fileToLoad.Close();
	return true;
}

bool A3DLitModel::Save(AFile * pFileToSave)
{
	DWORD dwWriteLen;

	// first of all, output the file version
	DWORD dwVersion = A3DLITMODEL_VERSION_EX;
	bool bSupportLM = false;
	int nNumMeshes;
	nNumMeshes = m_Meshes.GetSize();
	for( int nMesh = 0; nMesh < nNumMeshes; nMesh++)
	{
		A3DLitMesh * pMesh = m_Meshes[nMesh];
		if(pMesh->IsSupportLightMap())
		{
			bSupportLM = true;
			break;
		}
	}
	
	if( !pFileToSave->Write(&dwVersion, sizeof(DWORD), &dwWriteLen) )
		goto WRITEFAIL;

	// then output some basic information of this model
	if( !pFileToSave->Write(&m_vecScale, sizeof(A3DVECTOR3), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_vecDir, sizeof(A3DVECTOR3), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_vecUp, sizeof(A3DVECTOR3), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_vecPos, sizeof(A3DVECTOR3), &dwWriteLen) )
		goto WRITEFAIL;

	// now output the number of meshes in this model

	if( !pFileToSave->Write(&nNumMeshes, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;

    // Flags
    bool bLightMap = bSupportLM;
    if( !pFileToSave->Write(&bLightMap, sizeof(bool), &dwWriteLen) )
        goto WRITEFAIL;

    bool bSharedData = false;
    if( !pFileToSave->Write(&bSharedData, sizeof(bool), &dwWriteLen) )
        goto WRITEFAIL;


    for (int iMesh = 0; iMesh < nNumMeshes; iMesh++)
    {
        A3DLitMesh * pMesh = GetMesh(iMesh);

        if( !pFileToSave->Write(&pMesh->m_bSupportLM, sizeof(bool), &dwWriteLen) )
            goto WRITEFAIL;
        if( !pFileToSave->Write(&pMesh->m_bUseLM, sizeof(bool), &dwWriteLen) )
            goto WRITEFAIL;

        bool bVertexLight = !pMesh->IsSupportLightMap() || !pMesh->IsUseLightMap();

        if (!pFileToSave->WriteString(pMesh->m_strName))
            goto WRITEFAIL;
        AString strTextureMap;
        if (!pFileToSave->WriteString(pMesh->m_szTextureMap))
            goto WRITEFAIL;

        int nVertCount = pMesh->m_nVertCount;
        int nFaceCount = pMesh->m_nFaceCount;
        if( !pFileToSave->Write(&nVertCount, sizeof(int), &dwWriteLen) )
            goto WRITEFAIL;
        if( !pFileToSave->Write(&nFaceCount, sizeof(int), &dwWriteLen) )
            goto WRITEFAIL;

        // then output the vertex and indices
        A3DLMVERTEX_WITHOUTNORMAL* pVerts = new A3DLMVERTEX_WITHOUTNORMAL[nVertCount];
        for (int i = 0; i < nVertCount; i++)
        {
            pVerts[i].pos = pMesh->m_pVerts[i].pos;
            pVerts[i].diffuse = pMesh->m_pVerts[i].diffuse;
            pVerts[i].u = pMesh->m_pVerts[i].u;
            pVerts[i].v = pMesh->m_pVerts[i].v;
        }
        if( !pFileToSave->Write(pVerts, sizeof(A3DLMVERTEX_WITHOUTNORMAL) * nVertCount, &dwWriteLen) )
            goto WRITEFAIL;
        delete [] pVerts;

        if( !pFileToSave->Write(pMesh->m_pIndices, sizeof(WORD) * nFaceCount * 3, &dwWriteLen) )
            goto WRITEFAIL;
        if( !pFileToSave->Write(pMesh->m_pNormals, sizeof(A3DVECTOR3) * nVertCount, &dwWriteLen) )
            goto WRITEFAIL;      

        // load the material
        if (!pMesh->m_Material.Save(pFileToSave))
            goto WRITEFAIL;

        if (pMesh->m_bSupportLM)
        {
            // load light map coords
            if (!pFileToSave->Write( pMesh->m_pLMCoords, sizeof(A3DLIGHTMAPCOORD) * nVertCount, &dwWriteLen))
                goto WRITEFAIL;
        }
        if (bVertexLight)
        {
            if( !pFileToSave->Write(pMesh->m_pDayColors, sizeof(A3DCOLOR) * pMesh->m_nVertCount, &dwWriteLen) )
                goto WRITEFAIL;
            if( !pFileToSave->Write(pMesh->m_pNightColors, sizeof(A3DCOLOR) * pMesh->m_nVertCount, &dwWriteLen) )
                goto WRITEFAIL;
        }
        pMesh->m_fDNFactor = -1.0f;

        // output the aabb
        if (!pFileToSave->Write(&pMesh->m_aabb, sizeof(A3DAABB), &dwWriteLen))
            goto WRITEFAIL;

        // Save extra data
        DWORD dwOldPos = pFileToSave->GetPos();

        DWORD dwExtraSize = 0;
        if( !pFileToSave->Write(&dwExtraSize, sizeof(DWORD), &dwWriteLen) )
            goto WRITEFAIL;

        if( !pFileToSave->Write(&pMesh->m_fDetailTile, sizeof(float), &dwWriteLen) )
            goto WRITEFAIL;
        if( !pFileToSave->Write(&pMesh->m_fDetailHardness, sizeof(float), &dwWriteLen) )
            goto WRITEFAIL;
        if( !pFileToSave->WriteString(pMesh->m_strDetailMap) )
            goto WRITEFAIL;

        DWORD dwPos = pFileToSave->GetPos();
        pFileToSave->Seek(dwOldPos, AFILE_SEEK_SET);
        dwExtraSize = dwPos - dwOldPos;
        if( !pFileToSave->Write(&dwExtraSize, sizeof(DWORD), &dwWriteLen) )
            goto WRITEFAIL;
        pFileToSave->Seek(dwPos, AFILE_SEEK_SET);
    }

	//Save light map
	if(bSupportLM)
	{
        if (!pFileToSave->WriteString(m_szLightMap == NULL ? "" : m_szLightMap))
            goto WRITEFAIL;
        if (!pFileToSave->WriteString(m_szNightLightMap == NULL ? "" : m_szNightLightMap))
            goto WRITEFAIL;
	}

    // Save extra data
    DWORD dwOldPos = pFileToSave->GetPos();

    DWORD dwExtraSize = 0;
    if( !pFileToSave->Write(&dwExtraSize, sizeof(DWORD), &dwWriteLen) )
        goto WRITEFAIL;

    DWORD dwPos = pFileToSave->GetPos();
    pFileToSave->Seek(dwOldPos, AFILE_SEEK_SET);
    dwExtraSize = dwPos - dwOldPos;
    if( !pFileToSave->Write(&dwExtraSize, sizeof(DWORD), &dwWriteLen) )
        goto WRITEFAIL;
    pFileToSave->Seek(dwPos, AFILE_SEEK_SET);


	return true;

WRITEFAIL:
	g_A3DErrLog.Log("A3DLitModel::Save(), failed to write into the file!");
	return false;
}

bool A3DLitModel::SaveWithSharedFormat(AFile* pFileToSave, const char* szSharedFileName)
{
    DWORD dwWriteLen;

    // first of all, output the file version
    DWORD dwVersion = A3DLITMODEL_VERSION_EX;
    if( !pFileToSave->Write(&dwVersion, sizeof(DWORD), &dwWriteLen) )
        goto WRITEFAIL;

    // then output some basic information of this model
    if (!pFileToSave->Write(&m_vecScale, sizeof(A3DVECTOR3), &dwWriteLen))
        goto WRITEFAIL;
    if (!pFileToSave->Write(&m_vecDir, sizeof(A3DVECTOR3), &dwWriteLen))
        goto WRITEFAIL;
    if (!pFileToSave->Write(&m_vecUp, sizeof(A3DVECTOR3), &dwWriteLen))
        goto WRITEFAIL;
    if (!pFileToSave->Write(&m_vecPos, sizeof(A3DVECTOR3), &dwWriteLen))
        goto WRITEFAIL;

    bool bSupportLM = false;
    int nNumMeshes;
    nNumMeshes = m_Meshes.GetSize();
    for( int nMesh = 0; nMesh < nNumMeshes; nMesh++)
    {
        A3DLitMesh * pMesh = m_Meshes[nMesh];
        if(pMesh->IsSupportLightMap() && pMesh->IsUseLightMap())
        {
            bSupportLM = true;
            break;
        }
    }

    // now output the number of meshes in this model
    if (!pFileToSave->Write(&nNumMeshes, sizeof(int), &dwWriteLen))
        goto WRITEFAIL;

    // Flags
    bool bLightMap = bSupportLM;
    if( !pFileToSave->Write(&bLightMap, sizeof(bool), &dwWriteLen) )
        goto WRITEFAIL;

    bool bSharedData = true;
    if( !pFileToSave->Write(&bSharedData, sizeof(bool), &dwWriteLen) )
        goto WRITEFAIL;

    // output shared data file name
    if (!pFileToSave->WriteString(szSharedFileName))
        goto WRITEFAIL;

    if (!m_pA3DEngine->GetA3DLitModelSharedDataMan()->FindSharedData(szSharedFileName))
    {
        A3DLitModelSharedData* pShared = ExportSharedData();
        m_pA3DEngine->GetA3DLitModelSharedDataMan()->SaveSharedData(szSharedFileName, pShared);
    }

    for ( int i=0; i < nNumMeshes; i++)
    {
        A3DLitMesh * pMesh = m_Meshes[i];

        bool bVertexLight = !pMesh->IsSupportLightMap() || !pMesh->IsUseLightMap();

        if (!pFileToSave->Write(&pMesh->m_bSupportLM, sizeof(bool), &dwWriteLen))
            goto WRITEFAIL;

        if (!pFileToSave->Write(&pMesh->m_bUseLM, sizeof(bool), &dwWriteLen))
            goto WRITEFAIL;

        if (bVertexLight)
        {
            if (!pFileToSave->Write(pMesh->GetDayColors(), sizeof(A3DCOLOR) * pMesh->m_nVertCount, &dwWriteLen))
                goto WRITEFAIL;
            if (!pFileToSave->Write(pMesh->GetNightColors(), sizeof(A3DCOLOR) * pMesh->m_nVertCount, &dwWriteLen))
                goto WRITEFAIL;
        }

        // output the aabb
        if (!pFileToSave->Write(&pMesh->m_aabb, sizeof(A3DAABB), &dwWriteLen))
            goto WRITEFAIL;

        // Save extra data
        DWORD dwOldPos = pFileToSave->GetPos();

        DWORD dwExtraSize = 0;
        if( !pFileToSave->Write(&dwExtraSize, sizeof(DWORD), &dwWriteLen) )
            goto WRITEFAIL;

        if( !pFileToSave->Write(&pMesh->m_fDetailTile, sizeof(float), &dwWriteLen) )
            goto WRITEFAIL;
        if( !pFileToSave->Write(&pMesh->m_fDetailHardness, sizeof(float), &dwWriteLen) )
            goto WRITEFAIL;
        if( !pFileToSave->WriteString(pMesh->m_strDetailMap) )
            goto WRITEFAIL;

        DWORD dwPos = pFileToSave->GetPos();
        pFileToSave->Seek(dwOldPos, AFILE_SEEK_SET);
        dwExtraSize = dwPos - dwOldPos;
        if( !pFileToSave->Write(&dwExtraSize, sizeof(DWORD), &dwWriteLen) )
            goto WRITEFAIL;
        pFileToSave->Seek(dwPos, AFILE_SEEK_SET);
    }

    //Save light map
    if (bSupportLM)
    {
        if (!pFileToSave->WriteString(m_szLightMap == NULL ? "" : m_szLightMap))
            goto WRITEFAIL;
        if (!pFileToSave->WriteString(m_szNightLightMap == NULL ? "" : m_szNightLightMap))
            goto WRITEFAIL;
    }

    // Save extra data
    DWORD dwOldPos = pFileToSave->GetPos();

    DWORD dwExtraSize = 0;
    if( !pFileToSave->Write(&dwExtraSize, sizeof(DWORD), &dwWriteLen) )
        goto WRITEFAIL;

    DWORD dwPos = pFileToSave->GetPos();
    pFileToSave->Seek(dwOldPos, AFILE_SEEK_SET);
    dwExtraSize = dwPos - dwOldPos;
    if( !pFileToSave->Write(&dwExtraSize, sizeof(DWORD), &dwWriteLen) )
        goto WRITEFAIL;
    pFileToSave->Seek(dwPos, AFILE_SEEK_SET);

    return true;

WRITEFAIL:
    g_A3DErrLog.Log("A3DLitModel::Save(), failed to write into the file!");
    return false;
}

bool A3DLitModel::SaveWithSharedFormat(const char* szFileName, const char* szSharedFileName)
{
    AFile fileToSave;
    if( !fileToSave.Open(szFileName, AFILE_CREATENEW | AFILE_BINARY) )
    {
        g_A3DErrLog.Log("A3DLitModel::SaveWithSharedFormat(), failed to open file [%s]!", szFileName);
        return false;
    }

    SaveWithSharedFormat(&fileToSave, szSharedFileName);

    fileToSave.Close();
    return true;

}

bool A3DLitModel::DummyLoad(AFile * pFileToLoad)
{
	DWORD dwReadLen;

	// first of all, read the file version
	DWORD dwVersion;
	if( !pFileToLoad->Read(&dwVersion, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;

	if( dwVersion == 0x10000001 )
	{
		// then skip some basic information of this model
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * 4, AFILE_SEEK_CUR);

		// now read the number of meshes in this model
		int nNumMeshes;
		if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
			goto READFAIL;

		int i;
		for(i=0; i<nNumMeshes; i++)
		{
			A3DLitMesh * pMesh = new A3DLitMesh(this);
			pMesh->DummyLoad(pFileToLoad);

			pMesh->Release();
			delete pMesh;
		}
	}
	else if( dwVersion == 0x10000002 )
	{
		// then skip some basic information of this model
		pFileToLoad->Seek(sizeof(A3DVECTOR3) * 4, AFILE_SEEK_CUR);

		// now read the number of meshes in this model
		int nNumMeshes;
		if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
			goto READFAIL;

		int i;
		for(i=0; i<nNumMeshes; i++)
		{
			A3DLitMesh * pMesh = new A3DLitMesh(this);
			pMesh->DummyLoad(pFileToLoad);

			pMesh->Release();
			delete pMesh;
		}
	}
	else
	{
		g_A3DErrLog.Log("A3DLitModel::DummyLoad(), Wrong version!");
		return false;
	}

	return true;

READFAIL:
	g_A3DErrLog.Log("A3DLitModel::DummyLoad(), failed to read from the file!");
	return false;
}

bool A3DLitModel::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
{
	if( !Init(pA3DDevice) )
		return false;

	DWORD dwReadLen;

	// first of all, read the file version
	DWORD dwVersion;
	if( !pFileToLoad->Read(&dwVersion, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;

	if( dwVersion == 0x10000001 )
	{
		// then output some basic information of this model
		if( !pFileToLoad->Read(&m_vecScale, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecDir, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecUp, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecPos, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;

		// now read the number of meshes in this model
		int nNumMeshes;
		if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
			goto READFAIL;

		int i;
		for(i=0; i<nNumMeshes; i++)
		{
			A3DLitMesh * pMesh = new A3DLitMesh(this);
			pMesh->Load(pA3DDevice, pFileToLoad);

			// now find if there is a same textured mesh, if so just merge them here
			int n=0;
			for(n=0; n<m_Meshes.GetSize(); n++)
			{
				if( pMesh->GetTexturePtr() == m_Meshes[n]->GetTexturePtr() && pMesh->GetMaterial().Match(&m_Meshes[n]->GetMaterial()) )
					break;
			}

			if( n == m_Meshes.GetSize() )
				m_Meshes.Add(pMesh);
			else
			{
				m_Meshes[n]->MergeMesh(pMesh);
				pMesh->Release();
				delete pMesh;
				pMesh = NULL;
			}
		}
	}
	else if( dwVersion == 0x10000002 )
	{
		// then output some basic information of this model
		if( !pFileToLoad->Read(&m_vecScale, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecDir, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecUp, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecPos, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;

		// now read the number of meshes in this model
		int nNumMeshes;
		if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
			goto READFAIL;

		int i;
	
		for(i=0; i<nNumMeshes; i++)
		{
			A3DLitMesh * pMesh = new A3DLitMesh(this);
			pMesh->Load(pA3DDevice, pFileToLoad);

			// now find if there is a same textured mesh, if so just merge them here
			int n=0;
			for(n=0; n<m_Meshes.GetSize(); n++)
			{
				if( pMesh->GetTexturePtr() == m_Meshes[n]->GetTexturePtr() && pMesh->GetMaterial().Match(&m_Meshes[n]->GetMaterial()) )
					break;
			}

			if( n == m_Meshes.GetSize() )
				m_Meshes.Add(pMesh);
			else
			{
				m_Meshes[n]->MergeMesh(pMesh);
				pMesh->Release();
				delete pMesh;
				pMesh = NULL;
			}
			
		}
	}
	else if( dwVersion == 0x10000100 )
	{
		// then output some basic information of this model
		if( !pFileToLoad->Read(&m_vecScale, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecDir, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecUp, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecPos, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		
		// now read the number of meshes in this model
		int nNumMeshes;
		if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
			goto READFAIL;
		
		int i;
		
		for(i=0; i<nNumMeshes; i++)
		{
			A3DLitMesh * pMesh = new A3DLitMesh(this);
			pMesh->Load(pA3DDevice, pFileToLoad);	
			m_Meshes.Add(pMesh);		
		}

		//load light map name
		if( !pFileToLoad->Read( m_szLightMap, 256, &dwReadLen))
			goto READFAIL;

		if(!LoadLightMap(true, false))
		{
			return false;
		}
	}
	else if( dwVersion == A3DLITMODEL_LIGHTMAP_VERSION)
	{
		// then output some basic information of this model
		if( !pFileToLoad->Read(&m_vecScale, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecDir, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecUp, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecPos, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;
		
		// now read the number of meshes in this model
		int nNumMeshes;
		if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
			goto READFAIL;
		
		int i;
		
		for(i=0; i<nNumMeshes; i++)
		{
			A3DLitMesh * pMesh = new A3DLitMesh(this);
			pMesh->Load(pA3DDevice, pFileToLoad);	
			m_Meshes.Add(pMesh);		
		}
		
		//load light map name
		if( !pFileToLoad->Read( m_szLightMap, MAX_PATH, &dwReadLen))
			goto READFAIL;
		if( !pFileToLoad->Read(m_szNightLightMap, MAX_PATH, &dwReadLen))
			goto READFAIL;
		
		if(!LoadLightMap(true, true))
		{
			return false;
		}
	}
    else if( dwVersion == A3DLITMODEL_SHARED_VERSION)
    {
        if( !pFileToLoad->Read(&m_vecScale, sizeof(A3DVECTOR3), &dwReadLen) )
            goto READFAIL;
        if( !pFileToLoad->Read(&m_vecDir, sizeof(A3DVECTOR3), &dwReadLen) )
            goto READFAIL;
        if( !pFileToLoad->Read(&m_vecUp, sizeof(A3DVECTOR3), &dwReadLen) )
            goto READFAIL;
        if( !pFileToLoad->Read(&m_vecPos, sizeof(A3DVECTOR3), &dwReadLen) )
            goto READFAIL;

        bool bLightMap;
        if( !pFileToLoad->Read(&bLightMap, sizeof(bool), &dwReadLen) )
            goto READFAIL;

        AString strShareFile;
        if( !pFileToLoad->ReadString(strShareFile) )
            goto READFAIL;

        A3DLitModelSharedData* pShare = m_pA3DEngine->GetA3DLitModelSharedDataMan()->LoadSharedData(strShareFile); //TODO
        if (!pShare)
        {
            goto READFAIL;
        }

        // now read the number of meshes in this model
        int nNumMeshes;
        if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
            goto READFAIL;

        if (pShare->GetNumMeshes() != nNumMeshes)
        {
			g_A3DErrLog.Log("A3DLitModel::Load(), share data not match! FileName:%s", strShareFile.GetBuffer(0));
            goto READFAIL;
        }

        for (int i = 0; i < nNumMeshes; i++)
        {
            A3DLitMesh * pMesh = new A3DLitMesh(this);
            A3DLitMeshSharedData* pShareMesh = pShare->GetMesh(i);
            bool bSupportLightMap;
            bool bUseLightMap;

            if( !pFileToLoad->Read(&bSupportLightMap, sizeof(bool), &dwReadLen) )
                goto READFAIL;

            if( !pFileToLoad->Read(&bUseLightMap, sizeof(bool), &dwReadLen) )
                goto READFAIL;

            if( !pMesh->Create(pA3DDevice, pShareMesh->GetNumVertex(), pShareMesh->GetNumIndex(), pShareMesh->GetTextureMap(), bSupportLightMap) )
            {
                g_A3DErrLog.Log("A3DLitModel::Load(), failed to create mesh method!");
                return false;
            }

            for (int iVert =0; iVert < pMesh->m_nVertCount; iVert++)
            {
                pMesh->m_pVerts[iVert].pos = pShareMesh->GetPositions()[iVert];;
                pMesh->m_pVerts[iVert].diffuse = 0xFFFFFFFF;
                pMesh->m_pVerts[iVert].u = pShareMesh->GetUVs()[iVert * 2 + 0];
                pMesh->m_pVerts[iVert].v = pShareMesh->GetUVs()[iVert * 2 + 1];
                pMesh->m_pVerts[iVert].normal = pShareMesh->GetNormals()[iVert];
                pMesh->m_pNormals[iVert] = pMesh->m_pVerts[iVert].normal;
                if (pMesh->m_pLMCoords && pShareMesh->GetLMUVs())
                {
                    pMesh->m_pLMCoords[iVert].u = pShareMesh->GetLMUVs()[iVert * 2 + 0];
                    pMesh->m_pLMCoords[iVert].v = pShareMesh->GetLMUVs()[iVert * 2 + 1];
                }
            }

            memcpy(pMesh->GetIndices(), pShareMesh->GetIndices(), sizeof(WORD) * pShareMesh->GetNumIndex());

            if (!bSupportLightMap || !bUseLightMap)
            {
                if( !pFileToLoad->Read(pMesh->m_pDayColors, sizeof(A3DCOLOR) * pMesh->m_nVertCount, &dwReadLen) )
                    goto READFAIL;
                if( !pFileToLoad->Read(pMesh->m_pNightColors, sizeof(A3DCOLOR) * pMesh->m_nVertCount, &dwReadLen) )
                    goto READFAIL;
            }
            pMesh->m_fDNFactor = -1.0f;

            // read in the aabb
            if (!pFileToLoad->Read(&pMesh->m_aabb, sizeof(A3DAABB), &dwReadLen))
                goto READFAIL;

            pMesh->m_bUseLM = bUseLightMap;
            pMesh->m_Material = pShareMesh->GetMaterial();

            m_Meshes.Add(pMesh);

            if (!pMesh->CreateStream())
            {
                g_A3DErrLog.Log("A3DLitModel::Load(), create stream error!");
                return false;
            }
        }
        
        //load light map name
        if (bLightMap)
        {
            AString strLightMap;
            AString strNightLightMap;
            if (!pFileToLoad->ReadString(strLightMap))
                goto READFAIL;
            if (!pFileToLoad->ReadString(strNightLightMap))
                goto READFAIL;
            strcpy(m_szLightMap, strLightMap);
            strcpy(m_szNightLightMap, strNightLightMap);
            if (!LoadLightMap(true, true))
            {
                return false;
            }
        }
    }
    else if( dwVersion == A3DLITMODEL_VERSION_EX)
    {
        // Read basic information
        if( !pFileToLoad->Read(&m_vecScale, sizeof(A3DVECTOR3), &dwReadLen) )
            goto READFAIL;
        if( !pFileToLoad->Read(&m_vecDir, sizeof(A3DVECTOR3), &dwReadLen) )
            goto READFAIL;
        if( !pFileToLoad->Read(&m_vecUp, sizeof(A3DVECTOR3), &dwReadLen) )
            goto READFAIL;
        if( !pFileToLoad->Read(&m_vecPos, sizeof(A3DVECTOR3), &dwReadLen) )
            goto READFAIL;

        // Read the number of meshes in this model
        int nNumMeshes;
        if( !pFileToLoad->Read(&nNumMeshes, sizeof(int), &dwReadLen) )
            goto READFAIL;

        // Flags
        bool bLightMap;
        if( !pFileToLoad->Read(&bLightMap, sizeof(bool), &dwReadLen) )
            goto READFAIL;

        bool bSharedData;
        if( !pFileToLoad->Read(&bSharedData, sizeof(bool), &dwReadLen) )
            goto READFAIL;

        A3DLitModelSharedData* pShare = NULL;
        AString strShareFile;

        // Shared data
        if (bSharedData)
        {
            if( !pFileToLoad->ReadString(strShareFile) )
                goto READFAIL;

            pShare = m_pA3DEngine->GetA3DLitModelSharedDataMan()->LoadSharedData(strShareFile); //TODO
            if (!pShare)
            {
                goto READFAIL;
            }

            if (pShare->GetNumMeshes() != nNumMeshes)
            {
                g_A3DErrLog.Log("A3DLitModel::Load(), share data not match!");
                goto READFAIL;
            }
        }
        
        for (int i = 0; i < nNumMeshes; i++)
        {
            A3DLitMesh * pMesh = new A3DLitMesh(this);

            bool bSupportLightMap;
            bool bUseLightMap;
            if( !pFileToLoad->Read(&bSupportLightMap, sizeof(bool), &dwReadLen) )
                goto READFAIL;

            if( !pFileToLoad->Read(&bUseLightMap, sizeof(bool), &dwReadLen) )
                goto READFAIL;

            // with shared data
            if (bSharedData)
            {
                A3DLitMeshSharedData* pShareMesh = pShare->GetMesh(i);

                if( !pMesh->Create(pA3DDevice, pShareMesh->GetNumVertex(), pShareMesh->GetNumIndex() / 3, pShareMesh->GetTextureMap(), bSupportLightMap) )
                {
                    g_A3DErrLog.Log("A3DLitModel::Load(), failed to create mesh method!");
                    return false;
                }
                for (int iVert =0; iVert < pMesh->m_nVertCount; iVert++)
                {
                    pMesh->m_pVerts[iVert].pos = pShareMesh->GetPositions()[iVert];;
                    pMesh->m_pVerts[iVert].diffuse = 0xFFFFFFFF;
                    pMesh->m_pVerts[iVert].u = pShareMesh->GetUVs()[iVert * 2 + 0];
                    pMesh->m_pVerts[iVert].v = pShareMesh->GetUVs()[iVert * 2 + 1];
                    pMesh->m_pVerts[iVert].normal = pShareMesh->GetNormals()[iVert];
                    pMesh->m_pNormals[iVert] = pMesh->m_pVerts[iVert].normal;
                    if (pMesh->m_pLMCoords && pShareMesh->GetLMUVs())
                    {
                        pMesh->m_pLMCoords[iVert].u = pShareMesh->GetLMUVs()[iVert * 2 + 0];
                        pMesh->m_pLMCoords[iVert].v = pShareMesh->GetLMUVs()[iVert * 2 + 1];
                    }
                }
                memcpy(pMesh->GetIndices(), pShareMesh->GetIndices(), sizeof(WORD) * pShareMesh->GetNumIndex());
                pMesh->m_Material = pShareMesh->GetMaterial();
            }
            else
            {
                // without shared data
                // now, read in the name of this mesh
                if (!pFileToLoad->ReadString(pMesh->m_strName))
                    goto READFAIL;
                AString strTextureMap;
                if (!pFileToLoad->ReadString(strTextureMap))
                    goto READFAIL;

                int nVertCount, nFaceCount;
                // now read the vertex count and face count
                if( !pFileToLoad->Read(&nVertCount, sizeof(int), &dwReadLen) )
                    goto READFAIL;
                if( !pFileToLoad->Read(&nFaceCount, sizeof(int), &dwReadLen) )
                    goto READFAIL;

                if( !pMesh->Create(pA3DDevice, nVertCount, nFaceCount, strTextureMap, bSupportLightMap) )
                {
                    g_A3DErrLog.Log("A3DLitModel::Load(), failed to call Create method!");
                    return false;
                }

                // then read in the vertex and indices
                A3DLMVERTEX_WITHOUTNORMAL * pVerts = new A3DLMVERTEX_WITHOUTNORMAL[nVertCount];
                if( !pFileToLoad->Read(pVerts, sizeof(A3DLMVERTEX_WITHOUTNORMAL) * nVertCount, &dwReadLen) )
                    goto READFAIL;

                if( !pFileToLoad->Read(pMesh->m_pIndices, sizeof(WORD) * nFaceCount * 3, &dwReadLen) )
                    goto READFAIL;
                if( !pFileToLoad->Read(pMesh->m_pNormals, sizeof(A3DVECTOR3) * nVertCount, &dwReadLen) )
                    goto READFAIL;
                for (int i = 0; i < nVertCount; i++)
                {
                    pMesh->m_pVerts[i].pos = pVerts[i].pos;
                    pMesh->m_pVerts[i].diffuse = pVerts[i].diffuse;
                    pMesh->m_pVerts[i].u = pVerts[i].u;
                    pMesh->m_pVerts[i].v = pVerts[i].v;
                    pMesh->m_pVerts[i].normal = pMesh->m_pNormals[i];
                }
                delete [] pVerts;

                // load the material
                if (!pMesh->m_Material.Load(pA3DDevice, pFileToLoad))
                    goto READFAIL;

                if (bSupportLightMap)
                {
                    // load light map coords
                    if (!pFileToLoad->Read( pMesh->m_pLMCoords, sizeof(A3DLIGHTMAPCOORD) * nVertCount, &dwReadLen))
                        goto READFAIL;
                }
            }

            if (!bSupportLightMap || !bUseLightMap)
            {
                if( !pFileToLoad->Read(pMesh->m_pDayColors, sizeof(A3DCOLOR) * pMesh->m_nVertCount, &dwReadLen) )
                    goto READFAIL;
                if( !pFileToLoad->Read(pMesh->m_pNightColors, sizeof(A3DCOLOR) * pMesh->m_nVertCount, &dwReadLen) )
                    goto READFAIL;
            }
            pMesh->m_fDNFactor = -1.0f;

            // read in the aabb
            if (!pFileToLoad->Read(&pMesh->m_aabb, sizeof(A3DAABB), &dwReadLen))
                goto READFAIL;

            pMesh->m_bUseLM = bUseLightMap;

            // Read extra data of mesh

            // size of extra data
            DWORD dwExtraSize;
            DWORD dwPos = pFileToLoad->GetPos();

            if( !pFileToLoad->Read(&dwExtraSize, sizeof(DWORD), &dwReadLen) )
                goto READFAIL;

            if( !pFileToLoad->Read(&pMesh->m_fDetailTile, sizeof(float), &dwReadLen) )
                goto READFAIL;
            if( !pFileToLoad->Read(&pMesh->m_fDetailHardness, sizeof(float), &dwReadLen) )
                goto READFAIL;
            AString strDetailMap;
            if( !pFileToLoad->ReadString(strDetailMap) )
                goto READFAIL;
            pMesh->SetDetailTexture(strDetailMap);
            // skip extra data
            pFileToLoad->Seek(dwPos + dwExtraSize, AFILE_SEEK_SET);

            m_Meshes.Add(pMesh);

            if (!pMesh->CreateStream())
            {
                g_A3DErrLog.Log("A3DLitModel::Load(), create stream error!");
                return false;
            }
        }

        //load light map name
        if (bLightMap)
        {
            AString strLightMap;
            AString strNightLightMap;
            if (!pFileToLoad->ReadString(strLightMap))
                goto READFAIL;
            if (!pFileToLoad->ReadString(strNightLightMap))
                goto READFAIL;
            strcpy(m_szLightMap, strLightMap);
            strcpy(m_szNightLightMap, strNightLightMap);
            if (!LoadLightMap(true, true))
            {
                return false;
            }
        }

        // Read extra data of model

        // size of extra data
        DWORD dwPos = pFileToLoad->GetPos();
        DWORD dwExtraSize;
        if( !pFileToLoad->Read(&dwExtraSize, sizeof(DWORD), &dwReadLen) )
            goto READFAIL;

        // skip extra data
        pFileToLoad->Seek(dwPos + dwExtraSize, AFILE_SEEK_SET);

        if (bSharedData)
        {
            m_pA3DEngine->GetA3DLitModelSharedDataMan()->ReleaseSharedData(strShareFile);
        }

    }
    else
	{
		g_A3DErrLog.Log("A3DLitModel::Load(), Wrong version!");
		return false;
	}

	UpdateAbsoluteTM();

	return true;

READFAIL:
	g_A3DErrLog.Log("A3DLitModel::Load(), failed to read from the file!");
	return false;
}

bool A3DLitModel::LoadFromMox(A3DDevice * pA3DDevice, const char * szMoxFile)
{
	if( !Init(pA3DDevice) )
		return false;
	
	AFileImage file;

	if( !file.Open(szMoxFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY) )
	{
		g_A3DErrLog.Log("A3DLitModel::LoadFromMox(), failed to open file [%s]!", szMoxFile);
		return false;
	}

	A3DFrame * pFrame = new A3DFrame();

	// load from a mox file
	if( !pFrame->Load(m_pA3DDevice, &file) )
	{
		g_A3DErrLog.Log("A3DLitModel::LoadFromMox(), failed to load mox file!");
		return false;
	}
	file.Close();

	AddMeshFromFrame(pFrame);

	// no longer use frame, so delete it now.
	pFrame->Release();
	delete pFrame;

	file.Close();

	UpdateAbsoluteTM();
	return true;
}

bool A3DLitModel::AddMeshFromFrame(A3DFrame * pFrame)
{
	A3DMesh * pA3DMesh = pFrame->GetFirstMesh();
	
	A3DMATRIX4 matAbsoluteTM = pFrame->GetAbsoluteTM();

	int id = 0;
	while( pA3DMesh )
	{
		// vertex and face count
		int nVertCount = pA3DMesh->GetVertCount();
		int nFaceCount = pA3DMesh->GetIndexCount() / 3;

		bool bSupportLightMap = pA3DMesh->IsSupportLightMap();	

		A3DLitMesh * pLitMesh = AddNewMesh(nVertCount, nFaceCount, pA3DMesh->GetTextureMap(), bSupportLightMap);
		A3DLMVERTEX * pLitVerts = pLitMesh->GetVerts();
		A3DCOLOR *	 pDayColors = pLitMesh->GetDayColors();
		A3DCOLOR *   pNightColors = pLitMesh->GetNightColors();
		WORD *		 pLitIndices = pLitMesh->GetIndices();
		A3DVECTOR3 * pLitNormals = pLitMesh->GetNormals();

		
		A3DLIGHTMAPCOORD* pLitMeshLMCoords = pLitMesh->GetLightMapCoords();
		//copy light map coords
		if( bSupportLightMap)	
			memcpy( pLitMeshLMCoords, pA3DMesh->GetLightMapCoords(0), sizeof(A3DLIGHTMAPCOORD)* nVertCount);

		// first set the name of this LitMesh
		char szName[64];
		szName[63] = '\0';
		if( id == 0 )
			strncpy(szName, pFrame->GetName(), 63);
		else
			sprintf(szName, "%s-%d", pFrame->GetName(), id);
		pLitMesh->SetName(szName);

		A3DAABB		aabb;
		aabb.Clear();

		// retrieve vertex data
		int i;
		A3DVERTEX * pVerts = pA3DMesh->GetVerts(0);
		for(i=0; i<nVertCount; i++)
		{
			A3DVECTOR3 vecPos = A3DVECTOR3(pVerts[i].x, pVerts[i].y, pVerts[i].z) * matAbsoluteTM;
			A3DVECTOR3 vecNormal = a3d_VectorMatrix3x3(A3DVECTOR3(pVerts[i].nx, pVerts[i].ny, pVerts[i].nz), matAbsoluteTM);

			aabb.AddVertex(vecPos);

			pLitVerts[i].pos = vecPos;
			pLitVerts[i].normal = vecNormal;
			pLitVerts[i].diffuse = 0xff808080;
			pLitVerts[i].u = pVerts[i].tu;
			pLitVerts[i].v = pVerts[i].tv;

			pDayColors[i] = 0xff808080;
			pNightColors[i] = 0xff808080;

			pLitNormals[i] = vecNormal;
		}

		


		aabb.CompleteCenterExts();
		pLitMesh->SetAABB(aabb);

		// now set those material properties that we are interested in.
		pLitMesh->GetMaterial().SetEmissive(pA3DMesh->GetMaterial()->GetEmissive());
		if( pA3DMesh->GetMaterial()->Is2Sided() )
		{
			// for lit model, 2sided material just means leaves of the trees.
			pLitMesh->GetMaterial().Set2Sided(true);
		}
	
		// copy indices;
		memcpy(pLitIndices, pA3DMesh->GetIndices(), sizeof(WORD) * nFaceCount * 3);
        pLitMesh->CreateStream();
		pA3DMesh = pFrame->GetNextMesh();
		id ++;
	}

	A3DFrame * pChildFrame = pFrame->GetFirstChildFrame();
	while( pChildFrame )
	{
		AddMeshFromFrame(pChildFrame);

		pChildFrame = pFrame->GetNextChildFrame();
	}

	return true;
}

// mesh manipulation
A3DLitMesh * A3DLitModel::AddNewMesh(int nVertCount, int nFaceCount, const char * szTextureMap, bool bSupportLightMap)
{
	A3DLitMesh * pNewMesh = new A3DLitMesh(this);
	if( !pNewMesh->Create(m_pA3DDevice, nVertCount, nFaceCount, szTextureMap, bSupportLightMap) )
	{
		delete pNewMesh;
		return NULL;
	}

	m_Meshes.Add(pNewMesh);
	return pNewMesh;
}

bool A3DLitModel::DeleteMesh(A3DLitMesh * pMesh)
{
	int nNumMeshes = m_Meshes.GetSize();

	for(int i=0; i<nNumMeshes; i++)
	{
		A3DLitMesh * pMeshThis = m_Meshes[i];

		if( pMeshThis == pMesh )
		{
			pMesh->Release();
			delete pMesh;

			m_Meshes.RemoveAt(i);
			return true;
		}
	}

	return true;
}

bool A3DLitModel::ChangeLitmeshTexture(const char* szMeshName, const char* szTexFile)
{

	int nNumMeshes = m_Meshes.GetSize();
	for(int i = 0; i < nNumMeshes; i++)
	{
		A3DLitMesh * pMesh = m_Meshes[i];
		
		const char* szCurMeshName = pMesh->GetName();
		if( _stricmp(szCurMeshName, szMeshName) == 0) 
		{
			return pMesh->ChangeTextureMap(szTexFile);
		}
	}

	return false;
}

bool A3DLitModel::SetLightMap(const char* szLightMap)
{	
	if( szLightMap == NULL || szLightMap[0]=='\0')
		return false;

	ReleaseLightMap(true, false);
	strcpy(m_szLightMap, szLightMap);
	if(!LoadLightMap(true, false))
		return false;	
	return true;
	
}

bool A3DLitModel::SetNightLightMap(const char* szNightLightMap)
{
	if( szNightLightMap == NULL || szNightLightMap[0]=='\0')
		return false;
	
	ReleaseLightMap(false, true);
	strcpy(m_szNightLightMap, szNightLightMap);
	if(!LoadLightMap(false, true))
		return false;
	
	return true;
}

bool A3DLitModel::LoadLightMap(bool bDay, bool bNight)
{	
	if( bDay)
	{
		if( strlen(m_szLightMap) <= 0 || m_szLightMap[0] == '\0' ||strstr(m_szLightMap, LIGHT_MAP_VERSION) == 0)
		{
			strcpy(m_szLightMap, ERROR_LIGHT_MAP);
		}

		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szLightMap, &m_pLMTexture) )
		{
			strcpy(m_szLightMap, ERROR_LIGHT_MAP);
			if(!m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szLightMap, &m_pLMTexture))
			{
				g_A3DErrLog.Log("A3DLitModel::LoadLightMap(), failed to load texture [%s]!", m_szLightMap);
				return false;
			}
		}
	}

	if( bNight)
	{
		if( strlen(m_szNightLightMap) <= 0 || m_szNightLightMap[0] == '\0' || strstr(m_szNightLightMap, LIGHT_MAP_VERSION) == 0)
		{
			strcpy(m_szNightLightMap, ERROR_LIGHT_MAP);
		}

		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szNightLightMap, &m_pNightLMTexture) )
		{
			strcpy(m_szNightLightMap, ERROR_LIGHT_MAP);
			if(!m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szNightLightMap, &m_pNightLMTexture))
			{
				g_A3DErrLog.Log("A3DLitModel::LoadLightMap(), failed to load texture [%s]!", m_szNightLightMap);
				return false;
			}
			
		}
	}
	
	return true;
}

bool A3DLitModel::ReleaseLightMap(bool bDay , bool bNight )
{
	if(bDay)
	{
		if( m_pLMTexture )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pLMTexture);
			m_pLMTexture = NULL;
		}
	}

	if(bNight)
	{
		if( m_pNightLMTexture )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pNightLMTexture);
			m_pNightLMTexture = NULL;
		}
	}

	return true;
	
}

A3DLitModelSharedData* A3DLitModel::ExportSharedData() const
{
    A3DLitModelSharedData* pShared = new A3DLitModelSharedData;
    pShared->Init();
    for (int i = 0; i < m_Meshes.GetSize(); i++)
    {
        A3DLitMesh* pMesh = m_Meshes[i];
        pShared->AddMesh(pMesh->ExportSharedData());
    }

    return pShared;
}

