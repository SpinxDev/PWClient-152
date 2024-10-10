    /*
 * FILE: A3DSkin.cpp
 *
 * DESCRIPTION: A3D skin class
 *
 * CREATED BY: duyuxin, 2003/9/19
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DSkin.h"
#include "A3DPI.h"
#include "A3DDevice.h"
#include "A3DTexture.h"
#include "A3DTextureMan.h"
#include "A3DSkinMesh.h"
#include "A3DRigidMesh.h"
#include "A3DMorphRigidMesh.h"
#include "A3DMorphSkinMesh.h"
#include "A3DSuppleMesh.h"
#include "A3DMuscleMesh.h"
#include "A3DClothMesh.h"
#include "A3DMaterial.h"
#include "A3DEngine.h"
#include "A3DSkinMan.h"
#include "A3DSkinMeshMan.h"
#include "A3DSkinModel.h"
#include "A3DFuncs.h"
#include "A3DShader.h"
#include "A3DShaderMan.h"
#include "A3DSkeleton.h"
#include "AMemory.h"
#include "AFileImage.h"
#include "AFI.h"
#include "A3DHLSL.h"
#include "A3DEffect.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

#define CHECKTANERROR_MTL_FILE "Shaders\\2.2\\HLSL\\Materials\\!CheckTangentError.hlsl"

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

#pragma pack (push, 1)

//	Skin file header
struct A3DSKINFILEHEADER
{
	DWORD	dwFlags;			//	File's identity, SKINFILE_IDENTIFY
	DWORD	dwVersion;			//	File's version, SKINFILE_VERSION
	int		iNumSkinMesh;		//	Number of skin mesh
	int		iNumRigidMesh;		//	Number of rigid mesh
	int		iNumMorphSkMesh;	//	Number of morph skin mesh
	int		iNumMorphRdMesh;	//	Number of morph rigid mesh
	int		iNumTexture;		//	Number of texture
	int		iNumMaterial;		//	Number of material
	int		iNumSkinBone;		//	Number of bone involved in this skin
	float	fMinWeight;			//	Minimum weight limit
	int		iNumSkeBone;		//	Number of bone of skeleton to which the skin attached
	int		iNumSuppleMesh;		//	Number of supple mesh
	int		iNumMuscleMesh;		//	Number of muscle mesh
	BYTE	aReserved[52];		//	Reserved
};

#pragma pack (pop)

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement of A3DSkin
//
///////////////////////////////////////////////////////////////////////////
int A3DSKIN_TOTAL_COUNT = 0;
A3DSkin::A3DSkin()
{
	m_dwClassID		= A3D_CID_SKIN;
	m_pA3DEngine	= NULL;
	m_pA3DDevice	= NULL;
	m_pMeshMan		= NULL;
	m_dwSkinID		= 0;
	m_iDataSize		= 0;
	m_bSoftRender	= false;
	m_dwLoadVer		= SKINFILE_VERSION;
	m_bEnableCloth	= false;
	m_iAlphaWeight	= 0;
	m_bBloomMesh = false;
	A3DSKIN_TOTAL_COUNT++;

	m_bBloomEnabled = false;
	m_vBloomCol.x	= 1.0f;
	m_vBloomCol.y	= 0.0f;
	m_vBloomCol.z	= 0.0f;
	m_vBloomCol.w	= 1.0f;
}

A3DSkin::~A3DSkin()
{
	A3DSKIN_TOTAL_COUNT--;
}

/*	Initialize object

	Return true for success, otherwise return false.
*/
bool A3DSkin::Init(A3DEngine* pA3DEngine)
{
	ASSERT(pA3DEngine);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DEngine->GetA3DDevice();

	if (pA3DEngine->GetA3DSkinMan())
		m_pMeshMan = pA3DEngine->GetA3DSkinMan()->GetSkinMeshMan();
	else
		m_pMeshMan = NULL;
		
	return true;
}

//	Release object
void A3DSkin::Release()
{
	int i;

	if (m_pMeshMan)
		m_pMeshMan->ReleaseAllSkinMesh(this);

	m_aSkinMeshes.RemoveAll();
	m_aRigidMeshes.RemoveAll();
	m_aMorphSkMeshes.RemoveAll();
	m_aMorphRdMeshes.RemoveAll();
	m_aSuppleMeshes.RemoveAll();
	m_aMuscleMeshes.RemoveAll();
	
	//	Release all cloth meshes
	ReleaseClothes();

	//	Release all textures
	if (m_pA3DDevice)
	{
		A3DTextureMan* pTextureMan = m_pA3DEngine->GetA3DTextureMan();

		for (i=0; i < m_aTextures.GetSize(); i++)
		{
			A3DTexture* pTexture = m_aTextures[i];
			if (pTexture)
			{
				pTextureMan->ReleaseTexture(pTexture);
			}
			if( i < m_aNormalMaps.GetSize() )
			{
				pTexture = m_aNormalMaps[i];
				if (pTexture)
				{
					pTextureMan->ReleaseTexture(pTexture);
				}
			}
		}
	}

	m_aTextures.RemoveAll();
	m_aNormalMaps.RemoveAll();

	//	Release all materials
	for (i=0; i < m_aMaterials.GetSize(); i++)
	{
		A3DMaterial* pMtl = m_aMaterials[i];
		pMtl->Release();
		delete pMtl;
	}

	m_aMaterials.RemoveAll();

	//	Release bone name array
	for (i=0; i < m_aBoneNames.GetSize(); i++)
		delete m_aBoneNames[i];

	m_aBoneNames.RemoveAll();

	m_pA3DEngine = NULL;
	m_pA3DDevice = NULL;
}

//	Load skin data form file
bool A3DSkin::Load(AFile* pFile)
{
	A3DSKINFILEHEADER Header;
	DWORD dwRead;

	//	Load file header
	if (!pFile->Read(&Header, sizeof (Header), &dwRead) || dwRead != sizeof (Header))
	{
		g_A3DErrLog.Log("A3DSkin::Load, Failed to read file header");
		return false;
	}

	//	Compare identity and version
	if (Header.dwFlags != SKINFILE_IDENTIFY || (Header.dwVersion > SKINFILE_VERSION 
												&& Header.dwVersion != SKINFILE_TANGENT_VERSION 
												&& Header.dwVersion != 100 ))
	{
		g_A3DErrLog.Log("A3DSkin::Load, Invalid file identity or version");
		return false;
	}

	m_fMinWeight	= Header.fMinWeight;
	m_iNumSkeBone	= Header.iNumSkeBone;
	m_strFileName	= pFile->GetRelativeName();
	m_dwSkinID		= a_MakeIDFromLowString(m_strFileName);
	m_dwLoadVer		= Header.dwVersion;

	int i;

	//	Load bone names before meshes loaded, because skin bone number
	//	will be used in the mesh loading process
	if (Header.dwVersion >= 9 && Header.iNumSkinBone)
	{
		m_aBoneNames.SetSize(0, Header.iNumSkinBone);

		for (i=0; i < Header.iNumSkinBone; i++)
		{
			AString* pstr = new AString;
			pFile->ReadString(*pstr);
			m_aBoneNames.Add(pstr);
		}
	}

	//	Load textures
	if (Header.iNumTexture)
	{
		if (m_pA3DDevice)
		{
			//	Make texture path
			AString strRelativePath, strFileTitle;
			af_GetFilePath(m_strFileName, strRelativePath);
			af_GetFileTitle(m_strFileName, strFileTitle);
			int iTemp = strFileTitle.ReverseFind('.');
			if (iTemp >= 0)
				strFileTitle.CutRight(strFileTitle.GetLength() - iTemp);

			AString strTexPath1 = strRelativePath + "\\Tex_" + strFileTitle + "\\";
			AString strTexPath2 = strRelativePath + "\\Textures\\";
			AString strPathFile, strTexture;

			A3DTextureMan* pTexMan = m_pA3DEngine->GetA3DTextureMan();

			//	Create textures
			for (i=0; i < Header.iNumTexture; i++)
			{
				A3DTexture* pTexture = NULL;

				//	Load texture name, try path 2 at first.
				pFile->ReadString(strTexture);
				strPathFile = strTexPath2 + strTexture;
				if ( ! pTexMan->LoadTextureFromFile(strPathFile, &pTexture, A3DTF_LOADASMTL) ||
					 (! pTexture->GetD3DTexture() && pTexture->GetClassID() != A3D_CID_HLSLSHADER))
				{
					A3DTexture* pTexture2 = NULL;
					if (pTexture)
					{
						pTexture2 = pTexture;
						pTexture = NULL;
					}

					//	Then try path 1.
					strPathFile = strTexPath1 + strTexture;
					if ( ! pTexMan->LoadTextureFromFile(strPathFile, &pTexture, A3DTF_LOADASMTL) ||
						 (! pTexture->GetD3DTexture() && pTexture->GetClassID() != A3D_CID_HLSLSHADER))
					{
						//	Ignore the error.
						pTexMan->ReleaseTexture(pTexture);
						pTexture = pTexture2;
						pTexture2 = NULL;
					}
					else
					{
						pTexMan->ReleaseTexture(pTexture2);
						pTexture2 = NULL;
					}
				}
				m_aTextures.Add(pTexture);
			}
		}
		else
		{
			for (i=0; i < Header.iNumTexture; i++)
				m_aTextures.Add(NULL);
		}
	}

	//	Load materials
	for (i=0; i < Header.iNumMaterial; i++)
	{
		A3DMaterial* pMaterial = new A3DMaterial;
		if (!pMaterial)
		{
			g_A3DErrLog.Log("A3DSkin::Load, Not enough memory !");
			return false;
		}

		if (!pMaterial->Load(m_pA3DDevice, pFile))
			return false;

		//	Set the alpha component of ambient and emissive to zero. this
		//	ensure we can use diffuse's alpha to control model's tranparence
		A3DCOLORVALUE c = pMaterial->GetAmbient();
		pMaterial->SetAmbient(c.r, c.g, c.b, 0.0f);
		c = pMaterial->GetEmissive();
		pMaterial->SetEmissive(c.r, c.g, c.b, 0.0f);

		m_aMaterials.Add(pMaterial);
	}

	//	Load skin meshes
	if (Header.iNumSkinMesh)
	{
		m_aSkinMeshes.SetSize(Header.iNumSkinMesh, 4);

		for (i=0; i < Header.iNumSkinMesh; i++)
		{
			A3DMeshBase* pMesh = m_pMeshMan->LoadMeshObject(pFile, this, A3D_CID_SKINMESH, i, Header.iNumSkinMesh,m_dwLoadVer);
			if (!pMesh)
			{
				g_A3DErrLog.Log("A3DSkin::Load, Failed to load skin mesh !");
				return false;
			}

			A3DSkinMeshRef& Ref = m_aSkinMeshes[i];
			Ref.pMesh = pMesh;		
			Ref.bSWRender = m_bSoftRender;
		}
	}

	//	Load rigid meshes
	if (Header.iNumRigidMesh)
	{
		m_aRigidMeshes.SetSize(Header.iNumRigidMesh, 4);
		
		for (i=0; i < Header.iNumRigidMesh; i++)
		{
			A3DMeshBase* pMesh = m_pMeshMan->LoadMeshObject(pFile, this, A3D_CID_RIGIDMESH, i, Header.iNumRigidMesh, m_dwLoadVer);
			if (!pMesh)
			{
				g_A3DErrLog.Log("A3DSkin::Load, Failed to load rigid mesh !");
				return false;
			}

			A3DSkinMeshRef& Ref = m_aRigidMeshes[i];
			Ref.pMesh = pMesh;		
		}
	}

	//#if 0	// Ìæ»»HLSLÎÆÀí
	if(g_pA3DConfig->GetFlagHLSLEffectEnable())
	{
		int nTexCount = m_aTextures.GetSize();
		for(i = 0; i < nTexCount; i++)
		{
			A3DTexture* pTexture = m_aTextures[i];
			if(pTexture->GetClassID() != A3D_CID_HLSLSHADER)
			{
				A3DEffect* pEffect = NULL;
                
                const char* strScriptFile = (Header.dwVersion < 100) ? 
                    "shaders\\2.2\\hlsl\\materials\\default_old.hlsl" : "shaders\\2.2\\hlsl\\materials\\default.hlsl";

				pEffect = A3DEffect::CreateHLSLShader(m_pA3DDevice, pTexture->GetMapFile(), strScriptFile);
				m_pA3DEngine->GetA3DTextureMan()->ReleaseTexture(pTexture);
				m_aTextures[i] = pEffect;
			}
		}
	}
	//#endif // Ìæ»»HLSLÎÆÀí

	//	Load morph skin meshes
	if (Header.iNumMorphSkMesh)
	{
		m_aMorphSkMeshes.SetSize(Header.iNumMorphSkMesh, 4);

		for (i=0; i < Header.iNumMorphSkMesh; i++)
		{
			A3DMeshBase* pMesh = m_pMeshMan->LoadMeshObject(pFile, this, A3D_CID_MORPHSKINMESH, i, Header.iNumMorphSkMesh, m_dwLoadVer);
			if (!pMesh)
			{
				g_A3DErrLog.Log("A3DSkin::Load, Failed to load morph skin mesh !");
				return false;
			}

			A3DSkinMeshRef& Ref = m_aMorphSkMeshes[i];
			Ref.pMesh = pMesh;		
		}
	}

	//	Load morph rigid meshes
	if (Header.iNumMorphRdMesh)
	{
		m_aMorphRdMeshes.SetSize(Header.iNumMorphRdMesh, 4);

		for (i=0; i < Header.iNumMorphRdMesh; i++)
		{
			A3DMeshBase* pMesh = m_pMeshMan->LoadMeshObject(pFile, this, A3D_CID_MORPHRIGIDMESH, i, Header.iNumMorphRdMesh, m_dwLoadVer);
			if (!pMesh)
			{
				g_A3DErrLog.Log("A3DSkin::Load, Failed to load morph rigid mesh !");
				return false;
			}

			A3DSkinMeshRef& Ref = m_aMorphRdMeshes[i];
			Ref.pMesh = pMesh;		
		}
	}

	//	Load supple meshes
	if (Header.iNumSuppleMesh)
	{
		m_aSuppleMeshes.SetSize(Header.iNumSuppleMesh, 4);

		for (i=0; i < Header.iNumSuppleMesh; i++)
		{
			A3DMeshBase* pMesh = m_pMeshMan->LoadMeshObject(pFile, this, A3D_CID_SUPPLEMESH, i, Header.iNumSuppleMesh, m_dwLoadVer);
			if (!pMesh)
			{
				g_A3DErrLog.Log("A3DSkin::Load, Failed to load supple mesh !");
				return false;
			}

			A3DSkinMeshRef& Ref = m_aSuppleMeshes[i];
			Ref.pMesh = pMesh;
		}
	}

	//	Load muscle meshes
	if (Header.iNumMuscleMesh)
	{
		m_aMuscleMeshes.SetSize(Header.iNumMuscleMesh, 4);

		for (i=0; i < Header.iNumMuscleMesh; i++)
		{
			A3DMeshBase* pMesh = m_pMeshMan->LoadMeshObject(pFile, this, A3D_CID_MUSCLEMESH, i, Header.iNumMuscleMesh, m_dwLoadVer);
			if (!pMesh)
			{
				g_A3DErrLog.Log("A3DSkin::Load, Failed to load muscle mesh !");
				return false;
			}

			A3DSkinMeshRef& Ref = m_aMuscleMeshes[i];
			Ref.pMesh = pMesh;
		}
	}

	//	Now see if we should replace the skin's texture by current replace shader
	DWORD dwFlags;
	const char* szShaderFile = m_pA3DEngine->GetA3DSkinMan()->GetReplaceShaderFile(&dwFlags);
	if( szShaderFile )
	{
		if( dwFlags == A3DSkinMan::SHADERREPLACE_USERDEFINE )
		{
			A3DSkinMan::UserReplaceShaderProc pfn = (A3DSkinMan::UserReplaceShaderProc) szShaderFile;
			if( pfn )
				(*pfn)(this);
		}
		else
		{
			if (szShaderFile[0] != '\0')
			{
				for(i=0; i < m_aTextures.GetSize(); i++)
				{
					A3DTexture* pTex = m_aTextures[i];
					const char* szTexMap = pTex->GetMapFile();
					if (szTexMap && szTexMap[0])
					{
						if( dwFlags & A3DSkinMan::SHADERREPLACE_REFLECTPREFIX )
						{
							char szFileTitle[MAX_PATH];
							af_GetFileTitle(szTexMap, szFileTitle, MAX_PATH);
							_strlwr(szFileTitle);
							if( strstr(szFileTitle, "r_") != szFileTitle )
								continue;
						}
						
						A3DShader* pReplaceShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile(szShaderFile);
						if (pReplaceShader)
						{
							pReplaceShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szTexMap);
							ChangeSkinTexturePtr(i, pReplaceShader);
							pReplaceShader->SetOnlyActiveStagesFlag(true);
						}

		#ifdef SKIN_BUMP_ENABLE
						if( dwFlags & A3DSkinMan::SHADERREPLACE_NORMALMAPNEEDED )
						{
							// we load normal maps here
							char szNormalMap[MAX_PATH];
							strncpy(szNormalMap, szTexMap, MAX_PATH);
							char * pExt = strrchr(szTexMap, '.');
							if( pExt )
							{
								szNormalMap[pExt - szTexMap] = '\0';
								strcat(szNormalMap, "_normal");
								strcat(szNormalMap, pExt);
								A3DTexture * pNormalMap;
								m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(szNormalMap, &pNormalMap);
								m_aNormalMaps.Add(pNormalMap);
							}
						}
		#endif
					}
				}
			}
		}
	}

	return true;
}

bool A3DSkin::Load(const char* szFile)
{
	AFileImage File;

	if (!File.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		File.Close();
		g_A3DErrLog.Log("A3DSkin::Load, Cannot open file %s!", szFile);
		return false;
	}

	if (!Load(&File))
	{
		File.Close();
		return false;
	}

	File.Close();

	return true;
}

//	Save skin data from file
bool A3DSkin::Save(AFile* pFile, int nVersion, int& nReturnValue)
{
	m_dwLoadVer = nVersion;
	//	Fill header information
	A3DSKINFILEHEADER Header;
	memset(&Header, 0, sizeof (Header));

	Header.dwFlags			= SKINFILE_IDENTIFY;
	Header.dwVersion		= nVersion;
	Header.iNumSkinMesh		= m_aSkinMeshes.GetSize();
	Header.iNumRigidMesh	= m_aRigidMeshes.GetSize();
	Header.iNumMorphSkMesh	= m_aMorphSkMeshes.GetSize();
	Header.iNumMorphRdMesh	= m_aMorphRdMeshes.GetSize();
	Header.iNumTexture		= m_aTextures.GetSize();
	Header.iNumMaterial		= m_aMaterials.GetSize();
	Header.iNumSkinBone		= m_aBoneNames.GetSize();
	Header.fMinWeight		= m_fMinWeight;
	Header.iNumSkeBone		= m_iNumSkeBone;
	Header.iNumSuppleMesh	= m_aSuppleMeshes.GetSize();
	Header.iNumMuscleMesh	= m_aMuscleMeshes.GetSize();
	
	//	Write file header
	DWORD dwWrite;
	if (!pFile->Write(&Header, sizeof (Header), &dwWrite) || dwWrite != sizeof (Header))
	{
		g_A3DErrLog.Log("A3DSkin::Save, Failed to write file header!");
		return false;
	}

	int i;
	char szTitle[MAX_PATH];

	//	Save bone names
	for (i=0; i < m_aBoneNames.GetSize(); i++)
		pFile->WriteString(*m_aBoneNames[i]);

	//	Write all textures' name
	for (i=0; i < m_aTextures.GetSize(); i++)
	{
		A3DTexture* pTexture = m_aTextures[i];
		af_GetFileTitle(pTexture->GetMapFile(), szTitle, MAX_PATH);
		pFile->WriteString(szTitle);
	}

	//	Write all materials
	for (i=0; i < m_aMaterials.GetSize(); i++)
		m_aMaterials[i]->Save(pFile);

	bool bFindErr = false;
	//	Write all skin meshes
	for (i=0; i < m_aSkinMeshes.GetSize(); i++)
	{	
		m_aSkinMeshes[i].pMesh->Save(pFile, nVersion, nReturnValue);
		if(nReturnValue == A3DMeshBase::RV_UV_MIRROR_ERR)
			bFindErr = true;

	}

	//	Wrtie all rigid meshes
	for (i=0; i < m_aRigidMeshes.GetSize(); i++)
	{
		m_aRigidMeshes[i].pMesh->Save(pFile, nVersion, nReturnValue);
		if(nReturnValue == A3DMeshBase::RV_UV_MIRROR_ERR)
			bFindErr = true;
	}

	//	Write all morph skin meshes
	for (i=0; i < m_aMorphSkMeshes.GetSize(); i++)
		m_aMorphSkMeshes[i].pMesh->Save(pFile, nVersion, nReturnValue);

	//	Write all morph rigid meshes
	for (i=0; i < m_aMorphRdMeshes.GetSize(); i++)
		m_aMorphRdMeshes[i].pMesh->Save(pFile, nVersion, nReturnValue);

	//	Write all supple meshes
	for (i=0; i < m_aSuppleMeshes.GetSize(); i++)
		m_aSuppleMeshes[i].pMesh->Save(pFile, nVersion, nReturnValue);

	//	Write all muscle meshes
	for (i=0; i < m_aMuscleMeshes.GetSize(); i++)
		m_aMuscleMeshes[i].pMesh->Save(pFile, nVersion, nReturnValue);

	if( strlen(m_strFileName) == 0 )
	{
		// retrieve file name from the file that saved to.
		char szRelativeName[MAX_PATH];
		af_GetFileTitle(pFile->GetFileName(), szRelativeName, MAX_PATH);
		m_strFileName = szRelativeName;
	}

	if(bFindErr)
		nReturnValue = A3DMeshBase::RV_UV_MIRROR_ERR;

	return true;
}

bool A3DSkin::Save(const char* szFile, int nVersion, int& nReturnValue)
{
	AFile File;

	if (!File.Open(szFile, AFILE_BINARY | AFILE_CREATENEW))
	{
		File.Close();
		g_A3DErrLog.Log("A3DSkin::Save, Cannot create file %s!", szFile);
		return false;
	}

	m_dwLoadVer = nVersion;
	if (!Save(&File, nVersion, nReturnValue))
	{
		File.Close();
		return false;
	}

	File.Close();

	return true;
}

//	Set/Get number of bone involved in this skin
int A3DSkin::GetSkinBoneNum()
{
	return (m_dwLoadVer >= 9) ? m_aBoneNames.GetSize() : m_iNumSkeBone;
}

//	Set currently active morph channel
void A3DSkin::SetActiveMorphChannelNum(int iNum)
{
	int i;

	//	Handle all morph skin meshes
	for (i=0; i < m_aMorphSkMeshes.GetSize(); i++)
	{
		A3DMorphSkinMesh* pMesh = (A3DMorphSkinMesh*)m_aMorphSkMeshes[i].pMesh;
		pMesh->m_MorphData.SetActiveMorphChannelNum(iNum);
	}

	//	Handle all morph rigid meshes
	for (i=0; i < m_aMorphRdMeshes.GetSize(); i++)
	{
		A3DMorphRigidMesh* pMesh = (A3DMorphRigidMesh*)m_aMorphRdMeshes[i].pMesh;
		pMesh->m_MorphData.SetActiveMorphChannelNum(iNum);
	}
}

//	Calculate approximate data size
void A3DSkin::CalcDataSize()
{
	m_iDataSize = 0;

	int i;

	for (i=0; i < m_aSkinMeshes.GetSize(); i++)
		m_iDataSize += m_aSkinMeshes[i].pMesh->GetDataSize();

	for (i=0; i < m_aRigidMeshes.GetSize(); i++)
		m_iDataSize += m_aRigidMeshes[i].pMesh->GetDataSize();

	for (i=0; i < m_aMorphSkMeshes.GetSize(); i++)
		m_iDataSize += m_aMorphSkMeshes[i].pMesh->GetDataSize();

	for (i=0; i < m_aMorphRdMeshes.GetSize(); i++)
		m_iDataSize += m_aMorphRdMeshes[i].pMesh->GetDataSize();

	for (i=0; i < m_aSuppleMeshes.GetSize(); i++)
		m_iDataSize += m_aSuppleMeshes[i].pMesh->GetDataSize();

	for (i=0; i < m_aMuscleMeshes.GetSize(); i++)
		m_iDataSize += m_aMuscleMeshes[i].pMesh->GetDataSize();

	for (i=0; i < m_aClothMeshes.GetSize(); i++)
		m_iDataSize += m_aClothMeshes[i].pMesh->GetDataSize();
}

//	Change skin texture
bool A3DSkin::ChangeSkinTexture(int iIndex, const char* szTexFile, bool bDelOld/* true */)
{
	A3DTextureMan* pTexMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();
	A3DTexture* pNewTexture = NULL;
	if (!pTexMan->LoadTextureFromFile(szTexFile, &pNewTexture))
		return false;

	return ChangeSkinTexturePtr(iIndex, pNewTexture, bDelOld);
}

bool A3DSkin::ChangeSkinTexturePtr(int iIndex, A3DTexture* pTexture, bool bDelOld/* true */)
{
	A3DTexture* pOldTexture = m_aTextures[iIndex];
	m_aTextures[iIndex] = pTexture;

	//	Last release old texture
	if (bDelOld && pOldTexture)
	{
		A3DTextureMan* pTexMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();
		pTexMan->ReleaseTexture(pOldTexture);
	}

	return true;
}

//	Get skin mesh by name
A3DSkinMeshRef* A3DSkin::GetSkinMeshRefByName(const char* szName)
{
	//	Write all skin meshes
	for (int i=0; i < m_aSkinMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = m_aSkinMeshes[i].pMesh;
		if (!_stricmp(pMesh->GetName(), szName))
			return &m_aSkinMeshes[i];
	}

	return NULL;
}

//	Get rigid mesh by name
A3DSkinMeshRef* A3DSkin::GetRigidMeshRefByName(const char* szName)
{
	//	Wrtie all rigid meshes
	for (int i=0; i < m_aRigidMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = m_aRigidMeshes[i].pMesh;
		if (!_stricmp(pMesh->GetName(), szName))
			return &m_aRigidMeshes[i];
	}

	return NULL;
}

//	Get initial aabb of all meshes
A3DAABB A3DSkin::GetInitMeshAABB()
{
	int i;
	A3DAABB aabb;

	aabb.Clear();

	//	Write all skin meshes
	for (i=0; i < m_aSkinMeshes.GetSize(); i++)
	{
		const A3DAABB& aabb2 = m_aSkinMeshes[i].pMesh->GetInitMeshAABB();
		aabb.Mins = a3d_VecMin(aabb.Mins, aabb2.Mins);
		aabb.Maxs = a3d_VecMax(aabb.Maxs, aabb2.Maxs);
	}

	//	Wrtie all rigid meshes
	for (i=0; i < m_aRigidMeshes.GetSize(); i++)
	{
		const A3DAABB& aabb2 = m_aRigidMeshes[i].pMesh->GetInitMeshAABB();
		aabb.Mins = a3d_VecMin(aabb.Mins, aabb2.Mins);
		aabb.Maxs = a3d_VecMax(aabb.Maxs, aabb2.Maxs);
	}

	//	Write all morph skin meshes
	for (i=0; i < m_aMorphSkMeshes.GetSize(); i++)
	{
		const A3DAABB& aabb2 = m_aMorphSkMeshes[i].pMesh->GetInitMeshAABB();
		aabb.Mins = a3d_VecMin(aabb.Mins, aabb2.Mins);
		aabb.Maxs = a3d_VecMax(aabb.Maxs, aabb2.Maxs);
	}

	//	Write all morph rigid meshes
	for (i=0; i < m_aMorphRdMeshes.GetSize(); i++)
	{
		const A3DAABB& aabb2 = m_aMorphRdMeshes[i].pMesh->GetInitMeshAABB();
		aabb.Mins = a3d_VecMin(aabb.Mins, aabb2.Mins);
		aabb.Maxs = a3d_VecMax(aabb.Maxs, aabb2.Maxs);
	}

	//	Write all supple meshes
	for (i=0; i < m_aSuppleMeshes.GetSize(); i++)
	{
		const A3DAABB& aabb2 = m_aSuppleMeshes[i].pMesh->GetInitMeshAABB();
		aabb.Mins = a3d_VecMin(aabb.Mins, aabb2.Mins);
		aabb.Maxs = a3d_VecMax(aabb.Maxs, aabb2.Maxs);
	}

	//	Write all muscle meshes
	for (i=0; i < m_aMuscleMeshes.GetSize(); i++)
	{
		const A3DAABB& aabb2 = m_aMuscleMeshes[i].pMesh->GetInitMeshAABB();
		aabb.Mins = a3d_VecMin(aabb.Mins, aabb2.Mins);
		aabb.Maxs = a3d_VecMax(aabb.Maxs, aabb2.Maxs);
	}

	aabb.CompleteCenterExts();
	return aabb;
}

//	Set transparent value of model
void A3DSkin::SetTransparent(float fTransparent)
{
	int i;

	//	skin meshes
	for (i=0; i < m_aSkinMeshes.GetSize(); i++)
		m_aSkinMeshes[i].fTrans = fTransparent;

	//	Rigid meshes
	for (i=0; i < m_aRigidMeshes.GetSize(); i++)
		m_aRigidMeshes[i].fTrans = fTransparent;

	//	Skin meshes
	for (i=0; i < m_aMorphSkMeshes.GetSize(); i++)
		m_aMorphSkMeshes[i].fTrans = fTransparent;

	//	Morph rigid meshes
	for (i=0; i < m_aMorphRdMeshes.GetSize(); i++)
		m_aMorphRdMeshes[i].fTrans = fTransparent;

	//	Supple meshes
	for (i=0; i < m_aSuppleMeshes.GetSize(); i++)
		m_aSuppleMeshes[i].fTrans = fTransparent;

	//	Muscle meshes
	for (i=0; i < m_aMuscleMeshes.GetSize(); i++)
		m_aMuscleMeshes[i].fTrans = fTransparent;

	//	Cloth meshes
	for (i=0; i < m_aClothMeshes.GetSize(); i++)
		m_aClothMeshes[i].fTrans = fTransparent;
}

//	Bind this skin with specify skeleton
bool A3DSkin::BindSkeleton(A3DSkeleton* pSkeleton)
{
	//	Check bone number at first
	if (GetSkinBoneNum() > pSkeleton->GetBoneNum())
		return false;

	//	Build bone index map
	if (m_dwLoadVer < 9)
	{
		m_aBoneIdxMap.SetSize(m_iNumSkeBone+1, 16);

		for (int i=0; i < m_iNumSkeBone; i++)
			m_aBoneIdxMap[i] = i;

		//	The last bone is just skeleton itself
		m_aBoneIdxMap[m_iNumSkeBone] = pSkeleton->GetBoneNum();
	}
	else	//	m_dwLoadVer >= 9
	{
		int iIndex, iNumBone = m_aBoneNames.GetSize();
		m_aBoneIdxMap.SetSize(iNumBone+1, 16);

		for (int i=0; i < iNumBone; i++)
		{
			iIndex = -1;
			A3DBone* pBone = pSkeleton->GetBone(*m_aBoneNames[i], &iIndex);
			if (!pBone)
			{
				g_A3DErrLog.Log("A3DSkin::BindSkeleton, skin [%s] couldn't be bound with skeleton [%s]", m_strFileName, pSkeleton->GetFileName());
				return false;
			}

			m_aBoneIdxMap[i] = iIndex;
		}

		//	The last bone is just skeleton itself
		m_aBoneIdxMap[iNumBone] = pSkeleton->GetBoneNum();
	}

	return true;
}

//	Create a cloth mesh
A3DClothMeshImp* A3DSkin::CreateClothMesh(A3DMeshBase* pCoreMesh)
{
	A3DClothMeshImp* pClothMesh = new A3DClothMeshImp(pCoreMesh);
	if (!pClothMesh)
		return NULL;

	if (!pClothMesh->Init(m_pA3DEngine))
	{
		pClothMesh->Release();
		delete pClothMesh;
		return NULL;
	}

	return pClothMesh;
}

//	Generate cloth meshes
bool A3DSkin::GenerateClothes(int iNumRigidMesh, int* aRigidMeshes, int iNumSkinMesh, int* aSkinMeshes)
{
	if ((iNumRigidMesh && !aRigidMeshes) ||	(iNumSkinMesh && !aSkinMeshes))
		return false;

	int i;
	bool aFlags[256];
	ASSERT(m_aSkinMeshes.GetSize() <= 256 && m_aRigidMeshes.GetSize() <= 256);

	//	Reset cloth mesh array
	m_aClothMeshes.RemoveAll(false);

	//	Create cloth for skin meshes
	memset(aFlags, 0, sizeof (aFlags));
	for (i=0; i < iNumSkinMesh; i++)
		aFlags[aSkinMeshes[i]] = true;

	GenerateClothes(m_aSkinMeshes, aFlags);

	//	Create cloth for rigid meshes
	memset(aFlags, 0, sizeof (aFlags));
	for (i=0; i < iNumRigidMesh; i++)
		aFlags[aRigidMeshes[i]] = true;

	GenerateClothes(m_aRigidMeshes, aFlags);

	return true;
}

//	Generate cloth meshes
bool A3DSkin::GenerateClothes(MeshRef& aMeshRefs, bool* aFlags)
{
	int i;

	for (i=0; i < aMeshRefs.GetSize(); i++)
	{
		A3DSkinMeshRef& ref = aMeshRefs[i];
		if (aFlags[i])
		{
			//	If mesh already has cloth data, just reset them, otherwise we need to create new cloth data
			if (ref.pClothImp)
				ref.pClothImp->Reset();
			else
			{
				A3DClothMeshImp* pClothMesh = CreateClothMesh(ref.pMesh);
				if (!pClothMesh)
				{
					g_A3DErrLog.Log("A3DSkin::GenerateClothes, failed to generate cloth for skin mesh in skin: %s", m_strFileName);
					continue;
				}

				ref.pClothImp = pClothMesh;
			}

			A3DSkinMeshRef ClothRef;
			ClothRef.pMesh = ref.pClothImp;
			m_aClothMeshes.Add(ClothRef);
		}
		else if (ref.pClothImp)
		{
			//	Release old cloth mesh
			ref.pClothImp->Release();
			delete ref.pClothImp;
			ref.pClothImp = NULL;
		}
	}

	return true;
}

//	Release cloth meshes
void A3DSkin::ReleaseClothes()
{
	for (int i=0; i < m_aClothMeshes.GetSize(); i++)
	{
		A3DSkinMeshRef& ref = m_aClothMeshes[i];
		A3DRELEASE(ref.pMesh);
	}

	m_aClothMeshes.RemoveAll();
}

//	Set vertex shader for all skin meshes and rigid meshes
void A3DSkin::SetVertexShader(A3DVertexShader* pvsSkinMesh, A3DVertexShader* pvsRigidMesh)
{
	int i;

	for (i=0; i < m_aSkinMeshes.GetSize(); i++)
	{
		A3DSkinMeshRef& ref = m_aSkinMeshes[i];
		ref.pvsBackup = ref.pvs;
		ref.pvs = pvsSkinMesh;
	}

	for (i=0; i < m_aRigidMeshes.GetSize(); i++)
	{
		A3DSkinMeshRef& ref = m_aRigidMeshes[i];
		ref.pvsBackup = ref.pvs;
		ref.pvs = pvsRigidMesh;
	}
}

//	Restore vertex shader for all skin meshes and rigid meshes
void A3DSkin::RestoreVertexShader()
{
	int i;

	for (i=0; i < m_aSkinMeshes.GetSize(); i++)
		m_aSkinMeshes[i].pvs = m_aSkinMeshes[i].pvsBackup;

	for (i=0; i < m_aRigidMeshes.GetSize(); i++)
		m_aRigidMeshes[i].pvs = m_aRigidMeshes[i].pvsBackup;
}

bool A3DSkin::CheckTangentError()
{
	if( ! g_pA3DConfig->GetFlagHLSLEffectEnable()) {
		return true;
	}

	int i;
	int nSkinNum = m_aSkinMeshes.GetSize();
	bool bval = true;
	for(i = 0; i < nSkinNum; i++)
	{
		A3DSkinMesh* pMesh = (A3DSkinMesh*)m_aSkinMeshes.GetAt(i).pMesh;
		SKIN_VERTEX_TANGENT* pTanVer = pMesh->GetOriginTanVertexBuf();
		if(pTanVer != NULL)
		{
			bool bError = false;
			int nVertNum = pMesh->GetVertexNum();
			for(int n = 0; n < nVertNum; n++)
			{
				if((pTanVer[n].vNormal[0] == 0 && pTanVer[n].vNormal[1] == 0 && pTanVer[n].vNormal[2] == 0) &&
					pTanVer[n].tangents.w == 0)
				{
					bError = true;
					bval = false;
					break;
				}
			}

			if(bError == true)
			{
				A3DEffect* pEffect = NULL;
				A3DTexture* pTexture = m_aTextures.GetAt(pMesh->GetTextureIndex());
				if(pTexture != NULL) {
					pEffect = A3DEffect::CreateHLSLShader(m_pA3DDevice, pTexture->GetMapFile(), CHECKTANERROR_MTL_FILE, true);
					m_pA3DEngine->GetA3DTextureMan()->ReleaseTexture(pTexture);
					m_aTextures[pMesh->GetTextureIndex()] = pEffect;
				}
			}
		}
	}

	int nRigidNum = m_aRigidMeshes.GetSize();
	for(i = 0; i < nRigidNum; i++)
	{
		A3DRigidMesh* pMesh = (A3DRigidMesh*)m_aRigidMeshes.GetAt(i).pMesh;
		A3DVERTEX_TANGENT* pTanVer = pMesh->GetOriginTanVertexBuf();
		if(pTanVer != NULL)
		{
			bool bError = false;
			int nVertNum = pMesh->GetVertexNum();
			for(int n = 0; n < nVertNum; n++)
			{
				if((pTanVer[n].nx == 0 && pTanVer[n].ny == 0 && pTanVer[n].nz == 0) &&
					pTanVer[n].tangents.w == 0)
				{
					bError = true;
					bval = false;
					break;
				}
			}

			if(bError == true)
			{
				A3DEffect* pEffect = NULL;
				A3DTexture* pTexture = m_aTextures.GetAt(pMesh->GetTextureIndex());
				if(pTexture != NULL) {
					pEffect = A3DEffect::CreateHLSLShader(m_pA3DDevice, pTexture->GetMapFile(), CHECKTANERROR_MTL_FILE);
					m_pA3DEngine->GetA3DTextureMan()->ReleaseTexture(pTexture);
					m_aTextures[pMesh->GetTextureIndex()] = pEffect;
				}
			}
		}
	}
	return bval;
}