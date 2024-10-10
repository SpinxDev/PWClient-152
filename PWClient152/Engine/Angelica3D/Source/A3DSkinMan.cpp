 /*
 * FILE: A3DSkinMan.cpp
 *
 * DESCRIPTION: A3D skin manager class
 *
 * CREATED BY: duyuxin, 2003/9/22
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DSkinMan.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DVSDef.h"
#include "A3DSkin.h"
#include "A3DSkinModel.h"
#include "A3DSkinMesh.h"
#include "A3DRigidMesh.h"
#include "A3DMorphRigidMesh.h"
#include "A3DMorphSkinMesh.h"
#include "A3DMuscleMesh.h"
#include "A3DTexture.h"
#include "A3DMaterial.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DVertexShader.h"
#include "A3DCameraBase.h"
#include "A3DLight.h"
#include "A3DViewport.h"
#include "A3DShader.h"
#include "A3DShaderMan.h"
#include "A3DSkeleton.h"
#include "A3DSkinRender.h"
#include "A3DSkinRenderWithEffect.h"
#include "A3DSkinMeshMan.h"
#include "AIniFile.h"
#include "AFileImage.h"
#include "AFI.h"
#include "AAssist.h"
#include "AMemory.h"
#include "ACSWrapper.h"
#include "A3DHLSL.h"
#include "A3DEnvironment.h"

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

//extern A3DCOMMONCONSTDESC s_aSkinRenderCommConst[];
//extern size_t g_cbSizeOfSkinRenderCommConst;

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
//	Implement of A3DSkinMan
//
///////////////////////////////////////////////////////////////////////////

A3DSkinMan::A3DSkinMan() : m_SkeletonTab(256), m_ShaderReplaceTab(16)
{
	m_pA3DEngine	= NULL;
	m_pA3DDevice	= NULL;
	m_psr			= NULL;
	m_pdsr			= NULL;
	m_pDirLight		= NULL;
	m_pDPointLight	= NULL;
	m_pMeshMan		= NULL;

	m_pSpecularMap	= NULL;

	m_vWorldCenter.Clear();

	memset(m_aVS, 0, sizeof (m_aVS));

	InitializeCriticalSection(&m_csSkeleton);
}

A3DSkinMan::~A3DSkinMan()
{
	DeleteCriticalSection(&m_csSkeleton);
}

//	Initialize object
bool A3DSkinMan::Init(A3DEngine* pA3DEngine)
{
	ASSERT(pA3DEngine);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DEngine->GetA3DDevice();

	//	Create skin mesh manager
	if (!(m_pMeshMan = new A3DSkinMeshMan))
	{
		g_A3DErrLog.Log("A3DSkinMan::Init, Not enough memory !");
		return false;
	}

	if (!m_pMeshMan->Init(pA3DEngine))
	{
		g_A3DErrLog.Log("A3DSkinMan::Init, Failed to initialize mesh manager !");
		return false;
	}

	//	Create default skin render
	if ((m_pdsr = pA3DEngine->CreateSkinRender(true)) == NULL)
	{
		g_A3DErrLog.Log("A3DSkinMan::Init, Not enough memory !");
		return false;
	}

	//m_pdsr->Init(pA3DEngine);
	m_psr = m_pdsr;

	// now create the specular map
	if( m_pA3DDevice->TestPixelShaderVersion(1, 4) )
	{
		m_pSpecularMap = new A3DTexture();
		if( !m_pSpecularMap->Create(m_pA3DDevice, 256, 1, A3DFMT_A8R8G8B8, 1) )
		{
			g_A3DErrLog.Log("A3DSkinMan::Init, failed to create specular map!");
			return false;
		}
		void * pData = NULL;
		int iPitch;
		if( !m_pSpecularMap->LockRect(NULL, &pData, &iPitch, 0) )
		{
			g_A3DErrLog.Log("A3DSkinMan::Init, failed to lock specular map!");
			return false;
		}
		DWORD * pColor = (DWORD *) pData;
		float p = 8.0f;
		for(int i=0; i<256; i++)
		{
			float v = i / 255.0f;
			v = (float)pow(v, p);
			BYTE c = BYTE(v * 255.0f);
			pColor[i] = A3DCOLORRGBA(c, c, c, 255);
		}
		m_pSpecularMap->UnlockRect();
	}
	return true;
}

//	Release object
void A3DSkinMan::Release()
{
	if( m_pSpecularMap )
	{
		m_pSpecularMap->Release();
		delete m_pSpecularMap;
		m_pSpecularMap = NULL;
	}
	
	//	Release all resources
	ReleaseAllResources();

	A3DRELEASE(m_pdsr);
	A3DRELEASE(m_pMeshMan);

	m_pA3DEngine = NULL;
	m_pA3DDevice = NULL;
}

//	Reset object
bool A3DSkinMan::Reset()
{
	//	Release all resources
	ReleaseAllResources();

	return true;
}

//	Release all skins
void A3DSkinMan::ReleaseAllResources()
{
	//	Release all skeletons
	if (m_SkeletonTab.size())
	{
		DWORD dwCount = 0;
		ACSWrapper csa(&m_csSkeleton);

		SkeletonTable::iterator it = m_SkeletonTab.begin();
		for (; it != m_SkeletonTab.end(); ++it)
		{
			SKELETONNODE* pNode = *it.value();
			dwCount += pNode->aSkeletons.GetSize();

			for (int i=0; i < pNode->aSkeletons.GetSize(); i++)
			{
				pNode->aSkeletons[i]->Release();
				delete pNode->aSkeletons[i];
			}

			FreeSkeletonNode(pNode);
		}

		m_SkeletonTab.clear();

		//	All skeletons should have been released before skin manager is released.
		if (dwCount)
		{
			ASSERT(0 && "There are skeleton not released !!");
			g_A3DErrLog.Log("A3DSkinMan::ReleaseAllSkins, %d skeleton(s) released by manager.", dwCount);
		}
	}

	m_ShaderReplaceTab.clear();

	//	Reset default render
    if (m_pdsr)
    {
	    m_pdsr->ResetRenderInfo(true);
	    m_pdsr->ResetRenderInfo(false);
    }

}

//	Allocate a skeleton node
A3DSkinMan::SKELETONNODE* A3DSkinMan::AllocSkeletonNode()
{
	SKELETONNODE* pNode = new SKELETONNODE;
	if (!pNode)
	{
		g_A3DErrLog.Log("A3DSkinMan::AllocSkeletonNode, Not enough memory !");
		return NULL;
	}

	return pNode;
}

//	Release skeleton node
void A3DSkinMan::FreeSkeletonNode(SKELETONNODE* pNode)
{
	delete pNode;
}

/*	Create skin from file

	Return skin object's address for success, otherwise return NULL.

	szFile: skin file name
*/
A3DSkin* A3DSkinMan::LoadSkinFile(const char* szFile)
{
	return CreateNewSkin((void*)szFile, true);
}

A3DSkin* A3DSkinMan::LoadSkinFile(AFile* pFile)
{
	return CreateNewSkin(pFile, false);
}

//	Apply light information to skin model
void A3DSkinMan::ApplyModelLightTo( A3DSkinModel* pSkinModel )
{
	pSkinModel->ApplyModelLight(m_pDirLight, m_pDPointLight);
}

//	Create new skin from file data
A3DSkin* A3DSkinMan::CreateNewSkin(void* pData, bool bFileName)
{
	//	Create a new skin
	A3DSkin* pSkin = new A3DSkin;
	if (!pSkin)
	{
		g_A3DErrLog.Log("A3DSkinMan::CreateNewSkin, Not enough memory");
		return NULL;	
	}

	if (!pSkin->Init(m_pA3DEngine))
	{
		delete pSkin;
		g_A3DErrLog.Log("A3DSkinMan::CreateNewSkin, Falied to initlaize A3DSkin");
		return NULL;
	}

	if (bFileName)
	{
		const char* szFileName = (const char*)pData;
		if (!pSkin->Load(szFileName))
		{
			pSkin->Release();
			delete pSkin;
			g_A3DErrLog.Log("A3DSkinMan::CreateNewSkin, Falied to load skin %s", szFileName);
			return NULL;
		}
	}
	else
	{
		AFile* pFile = (AFile*)pData;
		if (!pSkin->Load(pFile))
		{
			pSkin->Release();
			delete pSkin;
			g_A3DErrLog.Log("A3DSkinMan::CreateNewSkin, Falied to load skin %s", pFile->GetRelativeName());
			return NULL;
		}
	}

	return pSkin;
}

//	Release skin object
void A3DSkinMan::ReleaseSkin(A3DSkin** ppSkin)
{
	A3DSkin* pSkin = *ppSkin;
	if (!pSkin)
		return;

	pSkin->Release();
	delete pSkin;
	*ppSkin = NULL;
}

/*	Load skin model vertex shaders

	Return true for success, otherwise return false.

	szDescFile: Describle file name. It's a .ini file 
*/
bool A3DSkinMan::LoadSkinModelVertexShaders(const char* szDescFile)
{
	if (!m_pA3DDevice || !m_pA3DDevice->TestVertexShaderVersion(1, 1))
		return true;

	AIniFile IniFile;

	//	Open describe file
	if (!IniFile.Open(szDescFile))
	{
		g_A3DErrLog.Log("A3DSkinMan::LoadSkinModelVertexShaders, Falied to open ini file %s", szDescFile);
		return false;
	}

	A3DShaderMan* pShaderMan = m_pA3DEngine->GetA3DShaderMan();
	A3DHLSLMan* pHLSLMan = m_pA3DEngine->GetA3DHLSLMan();
	A3DEnvironment* pEnvironment = m_pA3DEngine->GetA3DEnvironment();
	A3DCCDBinder* pBinder = pEnvironment->GetCCDBinder();

	bool bBinary = IniFile.GetValueAsInt("general", "binary", 0) ? true : false;

	static char* aSects[VST_NUM] = {"spec", "nonspec", "spec_dlt", "nonspec_dlt"};
	AString strFile;

	for (int i=0; i < VST_NUM; i++)
	{
#ifdef SKIN_BUMP_ENABLE
		strFile = IniFile.GetValueAsString(aSects[i], "skinnormalmesh4");
		if (strFile.GetLength())
		{
			DWORD	dwVSDecl[20];
			int		n = 0;
			dwVSDecl[n ++] = D3DVSD_STREAM(0);					// begin tokens
			dwVSDecl[n ++] = D3DVSD_REG(0, D3DVSDT_FLOAT3);		// x, y, z in init space
			dwVSDecl[n ++] = D3DVSD_REG(1, D3DVSDT_FLOAT3);		// weights
			dwVSDecl[n ++] = D3DVSD_REG(2, D3DVSDT_D3DCOLOR);	// matrix indices
			dwVSDecl[n ++] = D3DVSD_REG(3, D3DVSDT_FLOAT3);		// nx, ny, nz in init space
			dwVSDecl[n ++] = D3DVSD_REG(4, D3DVSDT_FLOAT2);		// u, v coords.
			dwVSDecl[n ++] = D3DVSD_REG(5, D3DVSDT_FLOAT3);		// tangent coords in init space.
			dwVSDecl[n ++] = D3DVSD_REG(6, D3DVSDT_FLOAT3);		// binormal coords in init space.
			dwVSDecl[n ++] = D3DVSD_END();						// end tokens.
	
			m_aVS[i][VS_SKINMESH4] = pShaderMan->LoadVertexShader(strFile, bBinary, dwVSDecl);
		}
#else
		m_aVS[i][VS_SKINMESH4] = LoadShaderFromProfile(IniFile, pHLSLMan, pShaderMan, pBinder, bBinary, aSects[i], "skinmesh4", A3DVT_BVERTEX3);
		//strFile = IniFile.GetValueAsString(aSects[i], "skinmesh4");
		//if (strFile.GetLength())
		//{
		//	if(_strcmpi((const char*)strFile + strlen(strFile) - 5, ".hlsl") == 0)
		//	{
		//		m_aVS[i][VS_SKINMESH4] = pHLSLMan->LoadShader(strFile, "vs_main", NULL, NULL, NULL, NULL);
		//		((A3DHLSL*)m_aVS[i][VS_SKINMESH4])->BindCommConstData(pBinder);
		//	}
		//	else
		//		m_aVS[i][VS_SKINMESH4] = pShaderMan->LoadVertexShader(strFile, bBinary, A3DVT_BVERTEX3);
		//}
		m_aVS[i][VS_SKINMESH4TAN] = LoadShaderFromProfile(IniFile, pHLSLMan, pShaderMan, pBinder, bBinary, aSects[i], "skinmesh4tan", A3DVT_BVERTEX3TAN);
		//strFile = IniFile.GetValueAsString(aSects[i], "skinmesh4tan");
		//if (strFile.GetLength())
		//{
		//	if(_strcmpi((const char*)strFile + strlen(strFile) - 5, ".hlsl") == 0)
		//	{
		//		m_aVS[i][VS_SKINMESH4TAN] = pHLSLMan->LoadShader(strFile, "vs_main", NULL, NULL, NULL, NULL/*, A3DVT_BVERTEX3TAN*/);
		//		((A3DHLSL*)m_aVS[i][VS_SKINMESH4TAN])->BindCommConstData(pBinder);
		//	}
		//	else
		//		m_aVS[i][VS_SKINMESH4TAN] = pShaderMan->LoadVertexShader(strFile, bBinary, A3DVT_BVERTEX3TAN);
		//}
#endif

		//strFile = IniFile.GetValueAsString(aSects[i], "rigidmesh");
		//if (strFile.GetLength())
		//	m_aVS[i][VS_RIGIDMESH] = pShaderMan->LoadVertexShader(strFile, bBinary, A3DVT_VERTEX);
		m_aVS[i][VS_RIGIDMESH] = LoadShaderFromProfile(IniFile, pHLSLMan, pShaderMan, pBinder, bBinary, aSects[i], "rigidmesh", A3DVT_VERTEX);
		m_aVS[i][VS_RIGIDMESHTAN] = LoadShaderFromProfile(IniFile, pHLSLMan, pShaderMan, pBinder, bBinary, aSects[i], "rigidmeshtan", A3DVT_VERTEXTAN);

		m_aVS[i][VS_MORPHSKINMESH0]	= m_aVS[i][VS_SKINMESH4];
		
#ifdef SKIN_BUMP_ENABLE
		strFile = IniFile.GetValueAsString(aSects[i], "morphskinnormalmesh4");
		if (strFile.GetLength())
			m_aVS[i][VS_MORPHSKINMESH4]	= pShaderMan->LoadMorphMeshVertexShader(strFile, bBinary, 4, true);
#else
		strFile = IniFile.GetValueAsString(aSects[i], "morphskinmesh4");
		if (strFile.GetLength())
			m_aVS[i][VS_MORPHSKINMESH4]	= NULL;//pShaderMan->LoadMorphMeshVertexShader(strFile, bBinary, 4, true);
#endif

		strFile = IniFile.GetValueAsString(aSects[i], "morphrigidmesh0");
		if (strFile.GetLength())
			m_aVS[i][VS_MORPHRIGIDMESH0] = pShaderMan->LoadVertexShader(strFile, bBinary, A3DVT_VERTEX);

		strFile = IniFile.GetValueAsString(aSects[i], "morphrigidmesh4");
		if (strFile.GetLength())
			m_aVS[i][VS_MORPHRIGIDMESH4] = NULL;//pShaderMan->LoadMorphMeshVertexShader(strFile, bBinary, 4, false);
	}

	IniFile.Close();

	return true;
}

A3DObject* A3DSkinMan::LoadShaderFromProfile(AIniFile& IniFile, A3DHLSLMan* pHLSLMan, A3DShaderMan* pShaderMan, A3DCCDBinder* pBinder, bool bBinary, const char* szSect, const char* szKey, DWORD dwFVF)
{
	A3DObject* pShader = NULL;
	AString strFile = IniFile.GetValueAsString(szSect, szKey);
	AStringArray aPart;
	SplitString(strFile, '|', aPart);
	strFile = aPart[0];

	if (strFile.GetLength())
	{
		if(_strcmpi((const char*)strFile + strlen(strFile) - 5, ".hlsl") == 0)
		{
			const char* szMacros = (aPart.size() > 1) 
				? (aPart[1].GetLength() > 0 ? aPart[1] : NULL) 
				: NULL;

			pShader = pHLSLMan->LoadShader(strFile, "vs_main", NULL, NULL, szMacros, NULL);
			if(pShader != NULL) {
				((A3DHLSL*)pShader)->BindCommConstData(pBinder);
			}
			else {
				ASSERT(0);
			}
		}
		else
		{
			pShader = pShaderMan->LoadVertexShader(strFile, bBinary, dwFVF);
		}
	}
	return pShader;
}
/*	Set vertex shader

	Return previously used vertex shader.

	iVSType: A3DSkinMan::VST_??? 
	iIndex: vertex shader index, A3DSkinMan::VS_???
	pNewShader: new shader object
*/
A3DObject* A3DSkinMan::SetVertexShader(int iVSType, int iIndex, A3DVertexShader* pNewShader)
{
	ASSERT(iVSType >= 0 && iVSType < VST_NUM);
	ASSERT(iIndex >= 0 && iIndex < VS_NUM);

	A3DObject* pOld = m_aVS[iVSType][iIndex];
	m_aVS[iVSType][iIndex] = pNewShader;

	return pOld;
}

//	Get vertex shader
A3DObject* A3DSkinMan::GetVertexShader(int iVSType, int iIndex)
{
	ASSERT(iVSType >= 0 && iVSType < VST_NUM);
	ASSERT(iIndex >= 0 && iIndex < VS_NUM);
	return m_aVS[iVSType][iIndex];
}

//	Create skeleton from file
A3DSkeleton* A3DSkinMan::LoadSkeletonFile(const char* szFile)
{
	ACSWrapper csa(&m_csSkeleton);

	//	Skeleton has existed ?
	SKELETONNODE* pNode = SearchSkeleton(szFile, 0);
	if (pNode)
	{
		A3DSkeleton* pSkeleton = pNode->aSkeletons[0]->Clone();
		if (!pSkeleton)
		{
			g_A3DErrLog.Log("A3DSkinMan::LoadSkeletonFile, Failed to clone skeleton");
			return NULL;
		}

		int iIndex = pNode->aSkeletons.Add(pSkeleton);
		pSkeleton->SetSkinManFlag(iIndex);
		return pSkeleton;
	}

	return CreateNewSkeleton((void*)szFile, true);
}

//	Create skeleton from file
A3DSkeleton* A3DSkinMan::LoadSkeletonFile(AFile* pFile)
{
	ACSWrapper csa(&m_csSkeleton);
	
	//	Skeleton has existed ?
	SKELETONNODE* pNode = SearchSkeleton(pFile->GetRelativeName(), 0);
	if (pNode)
	{
		A3DSkeleton* pSkeleton = pNode->aSkeletons[0]->Clone();
		if (!pSkeleton)
		{
			g_A3DErrLog.Log("A3DSkinMan::LoadSkeletonFile, Failed to clone skeleton");
			return NULL;
		}

		int iIndex = pNode->aSkeletons.Add(pSkeleton);
		pSkeleton->SetSkinManFlag(iIndex);
		return pSkeleton;
	}

	return CreateNewSkeleton(pFile, false);
}

/*	Search a skeleton by it's file name

	szFile: skeleton file's relative name, if this parameter is NULL, dwSkeletonID will
			be used.
	dwSkeletonID: skeleton's ID, used when szFile == NULL.
*/
A3DSkinMan::SKELETONNODE* A3DSkinMan::SearchSkeleton(const char* szFile, DWORD dwSkeletonID)
{
	char szRelFile[MAX_PATH];
	szRelFile[0] = '\0';

	if (szFile)
	{
		af_GetRelativePath(szFile, szRelFile);
		dwSkeletonID = a_MakeIDFromLowString(szRelFile);
	}

	ACSWrapper csa(&m_csSkeleton);

	SkeletonTable::pair_type Pair = m_SkeletonTab.get((int)dwSkeletonID);
	if (!Pair.second)
		return NULL;	//	Counldn't find this skeleton

	SKELETONNODE* pNode = *Pair.first;
	ASSERT(pNode->aSkeletons[0]);

	if (szFile)
	{
		if (_stricmp(pNode->aSkeletons[0]->GetFileName(), szRelFile))
		{
			ASSERT(0);
			g_A3DErrLog.Log("A3DSkinMan::SearchSkeleton, skeleton file name collision !");
			return NULL;
		}
	}

	return pNode;
}

//	Create a new skeleton form file data
A3DSkeleton* A3DSkinMan::CreateNewSkeleton(void* pData, bool bFileName)
{
	A3DSkeleton* pSkeleton = new A3DSkeleton;
	if (!pSkeleton)
	{
		g_A3DErrLog.Log("A3DSkinMan::LoadSkeletonFile, Not enough memory");
		return NULL;
	}

	if (!pSkeleton->Init())
	{
		delete pSkeleton;
		g_A3DErrLog.Log("A3DSkinMan::LoadSkeletonFile, Falied to initlaize A3DSkeleton");
		return NULL;
	}

	if (bFileName)
	{
		const char* szFileName = (const char*)pData;
		if (!pSkeleton->Load(szFileName))
		{
			pSkeleton->Release();
			delete pSkeleton;
			g_A3DErrLog.Log("A3DSkinMan::LoadSkeletonFile, Falied to load skeleton %s", szFileName);
			return NULL;
		}
	}
	else
	{
		AFile* pFile = (AFile*)pData;
		if (!pSkeleton->Load(pFile))
		{
			pSkeleton->Release();
			delete pSkeleton;
			g_A3DErrLog.Log("A3DSkinMan::LoadSkeletonFile, Falied to load skeleton %s", pFile->GetRelativeName());
			return NULL;
		}
	}

	SKELETONNODE* pNode = AllocSkeletonNode();
	if (!pNode)
	{
		pSkeleton->Release();
		delete pSkeleton;
		return NULL;
	}

	int iIndex = pNode->aSkeletons.Add(pSkeleton);
	pSkeleton->SetSkinManFlag(iIndex);

	m_SkeletonTab.put((int)pSkeleton->GetSkeletonID(), pNode);

	return pSkeleton;
}

//	Release skeleton object
void A3DSkinMan::ReleaseSkeleton(A3DSkeleton** ppSkeleton)
{
	A3DSkeleton* pSkeleton = *ppSkeleton;
	if (!pSkeleton)
		return;

	ACSWrapper csa(&m_csSkeleton);

	SKELETONNODE* pNode = SearchSkeleton(pSkeleton->GetFileName(), (int)pSkeleton->GetSkeletonID());
	if (!pNode)
	{
		ASSERT(0);
		return;
	}

	int iIndex = pSkeleton->GetSkinManFlag();
	if (pNode->aSkeletons[iIndex] != pSkeleton)
	{
		ASSERT(pNode->aSkeletons[iIndex] == pSkeleton);
		return;
	}

	pNode->aSkeletons.RemoveAtQuickly(iIndex);
	if (pNode->aSkeletons.GetSize())
	{
		if (iIndex < pNode->aSkeletons.GetSize())
		{
			A3DSkeleton* pTemp = pNode->aSkeletons[iIndex];
			pTemp->SetSkinManFlag(iIndex);
		}
	}
	else	//	No skeleton left
	{
		//	Remove node from table
		FreeSkeletonNode(pNode);
		m_SkeletonTab.erase((int)pSkeleton->GetSkeletonID());
	}

	pSkeleton->Release();
	delete pSkeleton;
	*ppSkeleton = NULL;
}

// Set a replace shader
bool A3DSkinMan::SetReplaceShaderFile(const char * szShaderFile, DWORD dwFlags)
{
	DWORD dwThreadID = GetCurrentThreadId();

	SHADERREPLACE sdrReplace;
	if( dwFlags == SHADERREPLACE_USERDEFINE )
		sdrReplace.strFile.Format("%x", szShaderFile);
	else
		sdrReplace.strFile = szShaderFile;
	sdrReplace.dwFlags = dwFlags;

	ShaderReplaceTable::pair_type Pair = m_ShaderReplaceTab.get((int)dwThreadID);
	if (!Pair.second)
	{
		m_ShaderReplaceTab.put((int)dwThreadID, sdrReplace);
	}
	else
	{
		*Pair.first = sdrReplace;
	}

	return true;
}

const char * A3DSkinMan::GetReplaceShaderFile(DWORD * pdwFlags)
{ 
	DWORD dwThreadID = GetCurrentThreadId();

	ShaderReplaceTable::pair_type Pair = m_ShaderReplaceTab.get((int)dwThreadID);
	if (!Pair.second)
		return NULL;	

	if( pdwFlags )
		*pdwFlags = (*Pair.first).dwFlags;

	if( *pdwFlags == SHADERREPLACE_USERDEFINE )
	{
		DWORD pfnUserAddr;
		sscanf((*Pair.first).strFile, "%x", &pfnUserAddr);
		return (const char*) pfnUserAddr;
	}
	else
		return (*Pair.first).strFile; 
}
