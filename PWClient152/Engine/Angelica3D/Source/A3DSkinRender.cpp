 /*
 * FILE: A3DSkinRender.cpp
 *
 * DESCRIPTION: A3D skin model render class
 *
 * CREATED BY: duyuxin, 2005/3/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */
#include "A3DObject.h"
#include "A3DSkinRender.h"
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
#include "A3DClothMesh.h"
#include "A3DTexture.h"
#include "A3DMaterial.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DVertexShader.h"
#include "A3DCameraBase.h"
#include "A3DLight.h"
#include "A3DViewport.h"
#include "A3DShaderMan.h"
#include "A3DShader.h"
#include "A3DSkeleton.h"
#include "A3DSkinMan.h"
#include "AFileImage.h"
#include "AIniFile.h"
#include "AFI.h"
#include "AAssist.h"
#include "AMemory.h"
#include "A3DPixelShader.h"
#include "A3DHLSL.h"
#include "A3DEffect.h"
#include "AGPAPerfIntegration.h"
#include "A3DEnvironment.h"


///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

#define PVS_APPEAR_PARAM(_PVS)						\
if((_PVS)->GetClassID() == A3D_CID_VERTEXSHADER){	\
	((A3DVertexShader*)_PVS)->Appear();				\
}													\
else if((_PVS)->GetClassID() == A3D_CID_HLSL){		\
	((A3DHLSL*)_PVS)->Appear(APPEAR_SETSHADERONLY);	\
	((A3DHLSL*)_PVS)->SubmitCommTable(pEnvironment->GetCommonConstTable());	\
	m_pSkinMeshDecl->Appear();					\
}
#define PVS_APPEAR			PVS_APPEAR_PARAM(pvs)


///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

float GetNearestDistToAABB(const A3DAABB& aabb, const A3DMATRIX4& mat, const A3DVECTOR3& pos);
///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////
//A3DCOMMONCONSTDESC s_aSkinRenderCommConst[] =
//{
//	{"g_colAmbient",   MEMBER_OFFSET(A3DSkinModel::LIGHTINFO, colAmbient),  A3DCOMMONCONSTDESC::CCD_FLOAT4, 0},	// 0
//	{"g_colDiffuse",   MEMBER_OFFSET(A3DSkinModel::LIGHTINFO, colDirDiff),  A3DCOMMONCONSTDESC::CCD_FLOAT4, 0},	// 1
//	{"g_colSpecular",  MEMBER_OFFSET(A3DSkinModel::LIGHTINFO, colDirSpec),  A3DCOMMONCONSTDESC::CCD_FLOAT4, 0},	// 2
//	{"g_vPtLightPos",  MEMBER_OFFSET(A3DSkinModel::LIGHTINFO, vPtLightPos), A3DCOMMONCONSTDESC::CCD_FLOAT3, 0},	// 3
//	{"g_colPtAmbient", MEMBER_OFFSET(A3DSkinModel::LIGHTINFO, colPtAmb),    A3DCOMMONCONSTDESC::CCD_FLOAT4, 0},	// 4
//	{"g_colPtDiffuse", MEMBER_OFFSET(A3DSkinModel::LIGHTINFO, colPtDiff),   A3DCOMMONCONSTDESC::CCD_FLOAT4, 0},	// 5
//	{"g_vPtAtten",     MEMBER_OFFSET(A3DSkinModel::LIGHTINFO, vPtAtten),    A3DCOMMONCONSTDESC::CCD_FLOAT3, 0},	// 6
//	{"g_fPtRange",     MEMBER_OFFSET(A3DSkinModel::LIGHTINFO, fPtRange),    A3DCOMMONCONSTDESC::CCD_FLOAT,  0},	// 7
//	{0}
//};
//
//size_t g_cbSizeOfSkinRenderCommConst = sizeof(A3DSkinModel::LIGHTINFO);

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement of A3DSkinRender
//
///////////////////////////////////////////////////////////////////////////

A3DSkinRender::A3DSkinRender()
: m_aFreeMeshNodes		(0, 200)
, m_aFreeAlphaNodes		(0, 200)
, m_BloomMeshes			(0, 200)
, m_vVSConst0			(1.0f, 0.0f, 0.0f, 765.01f)
, m_pSkinMeshDecl		(NULL)
, m_pSkinMeshTanDecl	(NULL)
, m_pRigidMeshDecl		(NULL)
, m_pRigidMeshTanDecl	(NULL)
, m_dwAmbient			(0)
{
	m_pA3DEngine		= NULL;
	m_pA3DDevice		= NULL;
	m_psm				= NULL;
	m_iRenderSlotCnt	= 0;
	m_iFreeMNCnt		= 0;
	m_iFreeAMNCnt		= 0;
	m_ColorOP			= A3DTOP_MODULATE;
	m_pBloomMeshPS		= NULL;
	m_dwClassID			= A3D_CID_SKINRENDERWITHEFFECT;

	memset(&m_DirLightParams, 0, sizeof(m_DirLightParams));
}

A3DSkinRender::~A3DSkinRender()
{
}

//	Initialize object
bool A3DSkinRender::Init(A3DEngine* pA3DEngine)
{
	ASSERT(pA3DEngine);
	m_pA3DEngine	= pA3DEngine;
	m_pA3DDevice	= pA3DEngine->GetA3DDevice();
	m_psm	= pA3DEngine->GetA3DSkinMan();

	A3DHLSLMan* pHLSLMan = m_pA3DEngine->GetA3DHLSLMan();

	const char* szPSBloom = "shaders\\2.2\\HLSL\\Common\\bloom_mask.hlsl";
	if(af_IsFileExist(szPSBloom))
	{
		m_pBloomMeshPS = pHLSLMan->LoadShader(NULL, NULL, szPSBloom, "ps_main", "", 0);
		if( m_pBloomMeshPS == NULL)
		{
			g_A3DErrLog.Log("A3DSkinRender::Init(), Failed to create bloom mesh pixel shader!");
			return false;
		}
	}
		
	D3DVERTEXELEMENT9 aDecl[MAX_FVF_DECL_SIZE];

	a3d_MakeVSDeclarator(A3DVT_BVERTEX3, 0, aDecl);
	m_pSkinMeshDecl = new A3DVertexDecl();
	m_pSkinMeshDecl->Init(m_pA3DDevice, aDecl);

	a3d_MakeVSDeclarator(A3DVT_BVERTEX3TAN, 0, aDecl);
	m_pSkinMeshTanDecl = new A3DVertexDecl();
	m_pSkinMeshTanDecl->Init(m_pA3DDevice, aDecl);

	a3d_MakeVSDeclarator(A3DVT_VERTEX, 0, aDecl);
	m_pRigidMeshDecl = new A3DVertexDecl();
	m_pRigidMeshDecl->Init(m_pA3DDevice, aDecl);

	a3d_MakeVSDeclarator(A3DVT_VERTEXTAN, 0, aDecl);
	m_pRigidMeshTanDecl = new A3DVertexDecl();
	m_pRigidMeshTanDecl->Init(m_pA3DDevice, aDecl);

	return true;
}

//	Release object
void A3DSkinRender::Release()
{
	int i;

	A3DRELEASE(m_pRigidMeshDecl);
	A3DRELEASE(m_pRigidMeshTanDecl);
	A3DRELEASE(m_pSkinMeshDecl);
	A3DRELEASE(m_pSkinMeshTanDecl);

	//	Release render slots
	for (i=0; i < m_aRenderSlots.GetSize(); i++)
		delete m_aRenderSlots[i];

	m_aRenderSlots.RemoveAll();

	//	Release free mesh node pool
	for (i=0; i < m_aFreeMeshNodes.GetSize(); i++)
		delete m_aFreeMeshNodes[i];

	m_aFreeMeshNodes.RemoveAll();

	//	Release free alpha node pool
	for (i=0; i < m_aFreeAlphaNodes.GetSize(); i++)
		delete m_aFreeAlphaNodes[i];

	m_aFreeAlphaNodes.RemoveAll();

	m_iRenderSlotCnt	= 0;
	m_iFreeMNCnt		= 0;
	m_iFreeAMNCnt		= 0;

	A3DHLSLMan* pHLSLMan = m_pA3DEngine->GetA3DHLSLMan();
	if( NULL != pHLSLMan && m_pBloomMeshPS != NULL)
	{
		pHLSLMan->ReleaseShader(m_pBloomMeshPS);
		m_pBloomMeshPS = NULL;
	}

	ResetBloomMeshes();

	m_psm			= NULL;
	m_pA3DEngine	= NULL;
	m_pA3DDevice	= NULL;
}

//	Allocate a new mesh node
A3DSkinRender::MESHNODE* A3DSkinRender::AllocMeshNode(bool bAlpha)
{
	MESHNODE* pMeshNode;

	if (bAlpha)
	{
		if (m_iFreeAMNCnt >= m_aFreeAlphaNodes.GetSize())
		{
			//	Allocate a new mesh node
			if (!(pMeshNode = new MESHNODE))
			{
				g_A3DErrLog.Log("A3DSkinRender::AllocMeshNode, Not enough memory !");
				return NULL;
			}

			m_aFreeAlphaNodes.Add(pMeshNode);
		}
		else
			pMeshNode = m_aFreeAlphaNodes[m_iFreeAMNCnt];

		m_iFreeAMNCnt++;
	}
	else
	{
		if (m_iFreeMNCnt >= m_aFreeMeshNodes.GetSize())
		{
			//	Allocate a new mesh node
			if (!(pMeshNode = new MESHNODE))
			{
				g_A3DErrLog.Log("A3DSkinRender::AllocMeshNode, Not enough memory !");
				return NULL;
			}

			m_aFreeMeshNodes.Add(pMeshNode);
		}
		else
			pMeshNode = m_aFreeMeshNodes[m_iFreeMNCnt];

		m_iFreeMNCnt++;
	}

	return pMeshNode;
}

//	Reset render information, this function should be called every frame
void A3DSkinRender::ResetRenderInfo(bool bAlpha)
{
	if (bAlpha)
	{
		//	Reset alpha mesh list
		m_AlphaMeshList.RemoveAll();
		m_iFreeAMNCnt = 0;
	}
	else
	{
		for (int i=0; i < m_iRenderSlotCnt; i++)
		{
			RENDERSLOT* pSlot = m_aRenderSlots[i];

			pSlot->MorphMeshList.RemoveAll();
			pSlot->RigidMeshList.RemoveAll();
			pSlot->RigidMeshDLList.RemoveAll();
			pSlot->SuppleMeshList.RemoveAll();
			pSlot->SkinMeshList.RemoveAll();
			pSlot->SkinMeshDLList.RemoveAll();
			pSlot->MuscleMeshList.RemoveAll();
			pSlot->ClothMeshList.RemoveAll();
		}

		m_iRenderSlotCnt = 0;
		m_iFreeMNCnt	 = 0;
	}

	m_aModiferList.RemoveAll(false);
}

void A3DSkinRender::ResetBloomMeshes()
{
	for( int i = 0; i < m_BloomMeshes.GetSize(); i++)
	{
		delete m_BloomMeshes[i];
	}
	m_BloomMeshes.RemoveAll(false);
}
//
float GetNearestDistToAABB(const A3DAABB& aabb, const A3DMATRIX4& mat, const A3DVECTOR3& pos)
{
	float vMinDist = 999999.9f;
	float vDist = 0.0f;
	A3DVECTOR3 v;

	v = A3DVECTOR3(aabb.Mins.x, aabb.Mins.y, aabb.Mins.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Mins.x, aabb.Mins.y, aabb.Maxs.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Mins.x, aabb.Maxs.y, aabb.Mins.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Mins.x, aabb.Maxs.y, aabb.Maxs.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Maxs.x, aabb.Mins.y, aabb.Mins.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Maxs.x, aabb.Mins.y, aabb.Maxs.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Maxs.x, aabb.Maxs.y, aabb.Mins.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	v = A3DVECTOR3(aabb.Maxs.x, aabb.Maxs.y, aabb.Maxs.z);
	vDist = Magnitude(v * mat - pos);
	vMinDist = min2(vMinDist, vDist);

	return vMinDist;
}

bool A3DSkinRender::IsModelAlphaMesh(A3DSkinModel* pSkinModel, A3DSkin* pSkin, A3DSkinMeshRef* pMeshRef) const
{
	bool bAlphaShader = false;
	A3DTexture * pTexture = pMeshRef->pMesh->GetTexturePtr(pSkin);
	if( pTexture && /*pTexture->IsShaderTexture() && */pTexture->IsAlphaTexture() )
		bAlphaShader = true;

	if (pMeshRef->fTrans >= 0.0f || pSkinModel->GetSrcBlend() != A3DBLEND_SRCALPHA ||
		pSkinModel->GetDstBlend() != A3DBLEND_INVSRCALPHA || bAlphaShader)
		return true;

	return false;
}

ALISTPOSITION A3DSkinRender::GetAlphaMeshInsertPlace(MeshNodeList& meshList, MESHNODE* pNewNode)
{
	ALISTPOSITION pos = m_AlphaMeshList.GetHeadPosition();
	while (pos)
	{
		MESHNODE* pTempNode = m_AlphaMeshList.GetAt(pos);

		// first see if the skin are belonging to the same skin model
		if( pTempNode->pSkinModel == pNewNode->pSkinModel )
		{
			int iWeight1 = (pTempNode->pMeshRef->iAlphaWeight == 0) ? pTempNode->pSkin->GetAlphaSortWeight() : pTempNode->pMeshRef->iAlphaWeight;
			int iWeight2 = (pNewNode->pMeshRef->iAlphaWeight == 0) ? pNewNode->pSkin->GetAlphaSortWeight() : pNewNode->pMeshRef->iAlphaWeight;

			//	Compare alpha sort weight first
			if(iWeight1 != iWeight2)
			{
				if (iWeight2 < iWeight1)
				{
					return pos;
				}
			}
			else
			{
				// now the situation is simple, we just need to compare the aabb along main axis, which is z axis in fact
				const A3DAABB& aabb1 = pTempNode->pMeshRef->pMesh->GetInitMeshAABB();
				const A3DAABB& aabb2 = pNewNode->pMeshRef->pMesh->GetInitMeshAABB();

				if( aabb1.Maxs.y > aabb2.Maxs.y || aabb1.Maxs.z > aabb2.Maxs.z || aabb1.Mins.z < aabb2.Mins.z )
				{
					return pos;
				}
			}
		}
		else if (pTempNode->pSkinModel->GetAlphaSortID() != 0 &&
			pTempNode->pSkinModel->GetAlphaSortID() == pNewNode->pSkinModel->GetAlphaSortID())
		{
			if (pNewNode->pSkinModel->GetAlphaSortWeight() < pTempNode->pSkinModel->GetAlphaSortWeight())
			{
				return pos;
			}
		}
		else // coming from different models, so we need to compare the distance
		{
			if (pTempNode->fDist <= pNewNode->fDist)
			{
				return pos;
			}
		}

		m_AlphaMeshList.GetNext(pos);
	}

	return pos;
}

//	Register mesh which is ready to be rendered.
bool A3DSkinRender::RegisterRenderMesh(A3DViewport* pViewport, A3DSkinModel* pSkinModel,
						A3DSkin* pSkin, A3DSkinMeshRef* pMeshRef)
{
	// see if totally transparent or invisible flag is set
	if (pMeshRef->fTrans >= 1.0f || pMeshRef->bInvisible)
		return true;

	//Add Bloom Mesh
	A3DEffect* pEffect = (A3DEffect*)pMeshRef->pMesh->GetTexturePtr(pSkin);
	if (pEffect && pSkin->IsBloomEnabled())
	{
		MESHNODE* pNewNode = new MESHNODE;
		pNewNode->pSkinModel = pSkinModel;
		pNewNode->pSkin = pSkin;
		pNewNode->pMeshRef = pMeshRef;
		pNewNode->fDist = 0.0f;

		m_BloomMeshes.Add(pNewNode);
	}

	if (IsModelAlphaMesh(pSkinModel, pSkin, pMeshRef))
	{
		//	Allocate a new mesh node
		MESHNODE* pNewNode = AllocMeshNode(true);
		if (!pNewNode)
			return false;

		pNewNode->pSkinModel	= pSkinModel;
		pNewNode->pSkin			= pSkin;
		pNewNode->pMeshRef		= pMeshRef;

		//	Register alpha mesh
		A3DMATRIX4 mat = pSkinModel->GetAbsoluteTM();
		pNewNode->fDist = GetNearestDistToAABB(pMeshRef->pMesh->GetInitMeshAABB(), mat, pViewport->GetCamera()->GetPos());

		ALISTPOSITION pos = GetAlphaMeshInsertPlace(m_AlphaMeshList, pNewNode);

		if (!pos)
			m_AlphaMeshList.AddTail(pNewNode);
		else
			m_AlphaMeshList.InsertBefore(pos, pNewNode);

		return true;
	}

	//	Allocate a new mesh node
	MESHNODE* pNewNode = AllocMeshNode(false);
	if (!pNewNode)
		return false;

	pNewNode->pSkinModel	= pSkinModel;
	pNewNode->pSkin			= pSkin;
	pNewNode->pMeshRef		= pMeshRef;

	RENDERSLOT* pSlot = SelectRenderSlot(pSkin, pMeshRef->pMesh);

	switch (pMeshRef->pMesh->GetClassID())
	{
	case A3D_CID_SKINMESH:
		if(pSkinModel->HasDynamicLight()) {
			pSlot->SkinMeshDLList.AddTail(pNewNode);
		}
		else {
			pSlot->SkinMeshList.AddTail(pNewNode);
		}
		break;

	case A3D_CID_RIGIDMESH:
		if(pSkinModel->HasDynamicLight()) {
			pSlot->RigidMeshDLList.AddTail(pNewNode);
		}
		else {
			pSlot->RigidMeshList.AddTail(pNewNode);
		}
		break;

	case A3D_CID_SUPPLEMESHIMP:

		pSlot->SuppleMeshList.AddTail(pNewNode);
		break;

	case A3D_CID_MORPHSKINMESH:
	case A3D_CID_MORPHRIGIDMESH:

		pSlot->MorphMeshList.AddTail(pNewNode);
		break;

	case A3D_CID_MUSCLEMESHIMP:

		pSlot->MuscleMeshList.AddTail(pNewNode);
		break;

	case A3D_CID_CLOTHMESHIMP:

		pSlot->ClothMeshList.AddTail(pNewNode);
		break;

	default:
		ASSERT(0);
		return false;
	}

	return true;
}

//	Register skin which is ready to be rendered. This function don't actually render
//	meshes, it just register them and sort them base on texture, material and skin.
bool A3DSkinRender::RegisterRenderSkin(A3DViewport* pViewport, A3DSkin* pSkin, A3DSkinModel* pSkinModel)
{
	int i, iNumMesh;

	//	Register cloth meshes
	if (pSkin->GetClothMeshNum() && pSkin->IsClothesEnable())
	{
		iNumMesh = pSkin->GetClothMeshNum();
		for (i=0; i < iNumMesh; i++)
		{
			A3DSkinMeshRef& Ref = pSkin->GetClothMeshRef(i);
			RegisterRenderMesh(pViewport, pSkinModel, pSkin, &Ref);
		}
	}

	//	Register all skin meshes
	iNumMesh = pSkin->GetSkinMeshNum();
	for (i=0; i < iNumMesh; i++)
	{
		A3DSkinMeshRef& Ref = pSkin->GetSkinMeshRef(i);
		if (!Ref.pClothImp || !pSkin->IsClothesEnable())
			RegisterRenderMesh(pViewport, pSkinModel, pSkin, &Ref);
	}

	//	Register all rigid meshes
	iNumMesh = pSkin->GetRigidMeshNum();
	for (i=0; i < iNumMesh; i++)
	{
		A3DSkinMeshRef& Ref = pSkin->GetRigidMeshRef(i);
		if (!Ref.pClothImp || !pSkin->IsClothesEnable())
			RegisterRenderMesh(pViewport, pSkinModel, pSkin, &Ref);
	}

	//	Register all morph skin meshes
	iNumMesh = pSkin->GetMorphSkinMeshNum();
	for (i=0; i < iNumMesh; i++)
	{
		A3DSkinMeshRef& Ref = pSkin->GetMorphSkinMeshRef(i);
		RegisterRenderMesh(pViewport, pSkinModel, pSkin, &Ref);
	}

	//	Register all morph rigid meshes
	iNumMesh = pSkin->GetMorphRigidMeshNum();
	for (i=0; i < iNumMesh; i++)
	{
		A3DSkinMeshRef& Ref = pSkin->GetMorphRigidMeshRef(i);
		RegisterRenderMesh(pViewport, pSkinModel, pSkin, &Ref);
	}

	return true;
}

//	Select a proper render slot for specified mesh
A3DSkinRender::RENDERSLOT* A3DSkinRender::SelectRenderSlot(A3DSkin* pSkin, A3DMeshBase* pMesh)
{
	DWORD dwTexture = 0;

	if (pMesh->GetTextureIndex() >= 0)
	{
		A3DTexture* pTexture = pSkin->GetTexture(pMesh->GetTextureIndex());
		if (pTexture)
			dwTexture = pTexture->GetTextureID();
	}

	//	Search proper render slot
	RENDERSLOT* pSlot = NULL;
	int i;

	for (i=0; i < m_iRenderSlotCnt; i++)
	{
		pSlot = m_aRenderSlots[i];
		if (pSlot->dwTexture == dwTexture)
			break;
	}

	//	Cann't find a slot, create a new one
	if (i == m_iRenderSlotCnt)
	{
		if (m_iRenderSlotCnt >= m_aRenderSlots.GetSize())
		{
			//	Allocate a new slot
			if (!(pSlot = new RENDERSLOT))
			{
				g_A3DErrLog.Log("A3DSkinRender::SelectRenderSlot, Not enough memory !");
				return false;
			}
		}
		else
			pSlot = m_aRenderSlots[m_iRenderSlotCnt];

		pSlot->dwTexture = dwTexture;

		if (m_iRenderSlotCnt >= m_aRenderSlots.GetSize())
			m_aRenderSlots.Add(pSlot);

		m_iRenderSlotCnt++;
	}

	return pSlot;
}
//A3DSkinRender::RENDERSLOT* A3DSkinRender::SelectRenderSlot(A3DSkin* pSkin, A3DMeshBase* pMesh)
//{
//	DWORD dwTexture = 0;
//	A3DHLSLCore* pHLSLCore = NULL;
//
//	if (pMesh->GetTextureIndex() >= 0)
//	{
//		A3DTexture* pTexture = pSkin->GetTexture(pMesh->GetTextureIndex());
//		if (pTexture)
//		{
//			if(pTexture->GetClassID() == A3D_CID_HLSLSHADER) {
//				A3DEffect* pEffect = (A3DEffect*)pTexture;
//				pHLSLCore = pEffect->GetHLSLCore();
//				dwTexture = pEffect->GetCoreID();	// A3DEffect Ҳͬ��ʹ�����ID
//			}
//			else
//			{
//				return NULL;
//			}
//		}
//	}
//	
//	//	Search proper render slot
//	RENDERSLOT* pSlot = NULL;
//	int i;
//
//	for (i=0; i < m_iRenderSlotCnt; i++)
//	{
//		pSlot = m_aRenderSlots[i];
//		if (pSlot->dwTexture == dwTexture)
//			break;
//	}
//
//	//	Cann't find a slot, create a new one
//	if (i == m_iRenderSlotCnt)
//	{
//		if (m_iRenderSlotCnt >= m_aRenderSlots.GetSize())
//		{
//			//	Allocate a new slot
//			if (!(pSlot = new RENDERSLOT))
//			{
//				g_A3DErrLog.Log("A3DSkinRender::SelectRenderSlot, Not enough memory !");
//				return false;
//			}
//		}
//		else
//			pSlot = m_aRenderSlots[m_iRenderSlotCnt];
//
//		pSlot->dwTexture = dwTexture;
//		pSlot->pHLSLCore = pHLSLCore;
//
//		if (m_iRenderSlotCnt >= m_aRenderSlots.GetSize())
//			m_aRenderSlots.Add(pSlot);
//
//		m_iRenderSlotCnt++;
//	}
//
//	return pSlot;
//}

//	Apply mesh material (traditional version)
//void A3DSkinRender::ApplyMaterial(const MESHNODE* pNode)
//{
//	if (!m_pA3DDevice)
//		return;
//
//	int iMethod = pNode->pSkinModel->GetMaterialMethod();
//	float fTransparent = pNode->pMeshRef->fTrans;
//	float fAlpha = 1.0f - fTransparent;
//
//	if (iMethod == A3DSkinModel::MTL_SCALED)
//	{
//		A3DCOLORVALUE vScale = pNode->pSkinModel->GetMaterialScale();
//		if (fTransparent > 0.0f)
//			vScale.a = fAlpha;
//
//		A3DMaterial mtl = *pNode->pMeshRef->pMesh->GetMaterialPtr(pNode->pSkin);
//		mtl.ScaleValues(vScale, true);
//		mtl.SetEmissive(mtl.GetEmissive() + pNode->pSkinModel->GetExtraEmissive());
//		mtl.Appear();
//	}
//	else
//	{
//		A3DMaterial* pSrcMtl;
//		if (iMethod == A3DSkinModel::MTL_ORIGINAL)
//			pSrcMtl = (A3DMaterial*)pNode->pMeshRef->pMesh->GetMaterialPtr(pNode->pSkin);
//		else
//			pSrcMtl = &pNode->pSkinModel->GetUniformMaterial();
//
//		A3DMaterial mtl = *pSrcMtl;
//		if (fTransparent > 0.0f)
//			mtl.ScaleValues(A3DCOLORVALUE(1.0f, 1.0f, 1.0f, fAlpha), true);
//		
//		mtl.SetEmissive(mtl.GetEmissive() + pNode->pSkinModel->GetExtraEmissive());
//		mtl.Appear();
//	}
//
//	if (pNode->pMeshRef->pMesh->GetMaterialPtr(pNode->pSkin)->Is2Sided())
//		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
//	else
//		m_pA3DDevice->SetFaceCull(m_CurCull);
//}

//	Apply mesh material (Vertex Shader version)
//void A3DSkinRender::ApplyVSMaterial(const MESHNODE* pNode)
//{
//	if (!m_pA3DDevice)
//		return;
//
//	A3DMaterial* pMtl, MtlTemp;
//	A3DMaterial* pOriginMtl = (A3DMaterial*)pNode->pMeshRef->pMesh->GetMaterialPtr(pNode->pSkin);
//
//	int iMethod = pNode->pSkinModel->GetMaterialMethod();
//	float fTransparent = pNode->pMeshRef->fTrans;
//	float fAlpha = 1.0f - fTransparent;
//
//	if (iMethod == A3DSkinModel::MTL_SCALED)
//	{
//		MtlTemp = *pOriginMtl;
//		MtlTemp.ScaleValues(pNode->pSkinModel->GetMaterialScale(), true);
//		pMtl = &MtlTemp;
//	}
//	else
//	{
//		if (iMethod == A3DSkinModel::MTL_ORIGINAL)
//			pMtl = pOriginMtl;
//		else if (iMethod == A3DSkinModel::MTL_UNIFORM)
//			pMtl = &pNode->pSkinModel->GetUniformMaterial();
//	}
//
//	//	Get material parameters
//	const A3DMATERIALPARAM& mp = pMtl->GetMaterialParam();
//	const A3DSkinModel::LIGHTINFO& LightInfo = pNode->pSkinModel->GetLightInfo();
//
//	//	Set specular power value
//	m_vVSConst0.y = mp.Power;
//	m_pA3DDevice->SetVertexShaderConstants(SMVSC_CONSTANT, &m_vVSConst0, 1);
//
//	//	Set ambient value
//	A3DCOLORVALUE col = mp.Ambient * LightInfo.colAmbient + mp.Emissive + pNode->pSkinModel->GetExtraEmissive();
//	m_pA3DDevice->SetVertexShaderConstants(SMVSC_AMBIENT, &col, 1);
//
//	//	Set diffuse value
//	col = mp.Diffuse * LightInfo.colDirDiff;
//	col.a = fTransparent > 0.0f ? fAlpha : col.a;
//	m_pA3DDevice->SetVertexShaderConstants(SMVSC_DIRDIFFUSE, &col, 1);
//
//	//	Set specular value
//	col = mp.Specular * LightInfo.colDirSpec;
//	m_pA3DDevice->SetVertexShaderConstants(SMVSC_DIRSPECULAR, &col, 1);
//
//	if (LightInfo.bPtLight)
//	{
//		//	Set dynamic lgiht's ambient component
//		col = mp.Ambient * LightInfo.colPtAmb;
//		m_pA3DDevice->SetVertexShaderConstants(SMVSC_PTAMBIENT, &col, 1);
//
//		//	Set dynamic light's diffuse component
//		col = mp.Diffuse * LightInfo.colPtDiff;
//		col.a = 0.0f;	//	This will effect model's transparence
//		m_pA3DDevice->SetVertexShaderConstants(SMVSC_PTDIFFUSE, &col, 1);
//	}
//
//	//	2 sides property always get from original material
////	if (pMtl->Is2Sided())
//	if (pOriginMtl->Is2Sided())
//		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
//	else
//		m_pA3DDevice->SetFaceCull(m_CurCull);
//
//	if (iMethod == A3DSkinModel::MTL_SCALED)
//		MtlTemp.Release();
//}

//	Apply default mesh material (traditional version)
//void A3DSkinRender::ApplyDefMaterial(float fTransparent)
//{
//	if (!m_pA3DDevice)
//		return;
//
//	static A3DMaterial mtlDef;
//	static bool bInit = false;
//
//	if (!bInit)
//	{
//		mtlDef.Init(m_pA3DDevice);
//		bInit = true;
//	}
//
//	if (fTransparent > 0.0f)
//	{
//		A3DMaterial mtl = mtlDef;
//		A3DCOLORVALUE c = mtl.GetDiffuse();
//		c.a = 1.0f - fTransparent;
//		mtl.SetDiffuse(c);
//		mtl.Appear();
//	}
//	else
//		mtlDef.Appear();
//}

//	Apply default mesh material (Vertex Shader version)
//void A3DSkinRender::ApplyDefVSMaterial(float fTransparent)
//{
//	if (!m_pA3DDevice)
//		return;
//
//	static A3DMaterial mtlDef;
//	static bool bInit = false;
//
//	if (!bInit)
//	{
//		mtlDef.Init(m_pA3DDevice);
//		bInit = true;
//	}
//
//	//	Get material parameters
//	const A3DMATERIALPARAM& mp = mtlDef.GetMaterialParam();
//
//	//	Set specular power value
//	m_vVSConst0.y = mp.Power;
//	m_pA3DDevice->SetVertexShaderConstants(SMVSC_CONSTANT, &m_vVSConst0, 1);
//	//	Set ambient value
//	m_pA3DDevice->SetVertexShaderConstants(SMVSC_AMBIENT, &mp.Ambient, 1);
//	//	Set specular value
//	m_pA3DDevice->SetVertexShaderConstants(SMVSC_DIRSPECULAR, &mp.Specular, 1);
//	//	Set dynamic lgiht's ambient component
//	m_pA3DDevice->SetVertexShaderConstants(SMVSC_PTAMBIENT, &mp.Ambient, 1);
//	//	Set dynamic light's diffuse component
//	m_pA3DDevice->SetVertexShaderConstants(SMVSC_PTDIFFUSE, &mp.Diffuse, 1);
//	//	Set cull mode
//	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
//
//	//	Set diffuse value
//	if (fTransparent > 0.0f)
//	{
//		A3DCOLORVALUE c = mp.Diffuse;
//		c.a = 1.0f - fTransparent;
//		m_pA3DDevice->SetVertexShaderConstants(SMVSC_DIRDIFFUSE, &c, 1);
//	}
//	else
//		m_pA3DDevice->SetVertexShaderConstants(SMVSC_DIRDIFFUSE, &mp.Diffuse, 1);
//}

//	Render skins
bool A3DSkinRender::Render(A3DViewport* pViewport, bool bRenderAlpha/* true */)
{
	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__));
	if (!m_pA3DDevice)
		return true;

	A3DEnvironment* pEnvironment = m_pA3DEngine->GetA3DEnvironment();
	pEnvironment->SetViewport(pViewport);
	
	//Ӧ��ģ������
	for(int i = 0; i < m_aModiferList.GetSize(); i++)
	{
		MODIFIERITEM& item = m_aModiferList[i];
		item.pModifier->BeforeForwardRender(item.pSkinModel);
	}

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	//	Save light information
	SaveLightInformation();

	//	Set some vertex shader constants
	SetupVPTMConstant(pViewport);

	//	Save current cull mode
	m_CurCull = m_pA3DDevice->GetFaceCull();

	int i;

	//	Render all slots
	for (i=0; i < m_iRenderSlotCnt; i++)
	{
		RENDERSLOT* pSlot = m_aRenderSlots[i];

		const int smnum = pSlot->SkinMeshList.GetCount();
		const int smdlnum = pSlot->SkinMeshDLList.GetCount();
		const int rmnum = pSlot->RigidMeshList.GetCount();
		const int rmdlnum = pSlot->RigidMeshDLList.GetCount();

		if(smnum + smdlnum + rmnum + rmdlnum > 0)
		{
			// �ύ��������
			m_pSkinMeshTanDecl->Appear();

			// �ύps��HLSLCore
			//if(pSlot->pHLSLCore != NULL) {
			//	pSlot->pHLSLCore->Appear();
			//}

			// ��Ⱦ��ͨģ�ͺ����ն�̬��ģ��
			if(smnum > 0) {
				RenderMeshes(pViewport, pSlot->SkinMeshList, false, A3DSkinMan::VS_SKINMESH4TAN);
			}
			if(smdlnum > 0) {
				RenderMeshes(pViewport, pSlot->SkinMeshDLList, true, A3DSkinMan::VS_SKINMESH4TAN);
			}
			
			////////////////////////////
			//
			//	Render rigid meshes
			//

			// �ύ��������
			m_pRigidMeshDecl->Appear();

			// ��Ⱦ��ͨģ�ͺ����ն�̬��ģ��
			if(rmnum > 0) {
				RenderMeshes(pViewport, pSlot->RigidMeshList, false, A3DSkinMan::VS_RIGIDMESHTAN);
			}
			if(rmdlnum > 0) {
				RenderMeshes(pViewport, pSlot->RigidMeshDLList, true, A3DSkinMan::VS_RIGIDMESHTAN);
			}


		}

		//if (pSlot->SkinMeshList.GetCount())
		//{
		//	//	Render skin meshes
		//	switch (iRenderMethod)
		//	{
		//	case A3DDevice::SMRD_VERTEXSHADER:
		//		
		//		RenderSkinMeshes_VS(pViewport, pSlot);
		//		break;

		//	case A3DDevice::SMRD_INDEXEDVERTMATRIX:
		//		
		//		RenderSkinMeshes_IVM(pViewport, pSlot);
		//		break;

		//	case A3DDevice::SMRD_SOFTWARE:	
		//		
		//		RenderSkinMeshes_SW(pViewport, pSlot);
		//		break;
		//	}
		//}


	
		////	Render rigid meshes
		//if (pSlot->RigidMeshList.GetCount())
		//{
		//	RenderRigidMeshes(pViewport, pSlot);
		//}

		////	Render supple meshes
		//if (pSlot->SuppleMeshList.GetCount())
		//	RenderSuppleMeshes(pViewport, pSlot);

		//if (pSlot->MorphMeshList.GetCount())
		//{
		//	//	Render morph meshes
		//	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
		//	{
		//		RenderMorphMeshes_VS(pViewport, pSlot);
		//	}
		//	else
		//		RenderMorphMeshes_SW(pViewport, pSlot);
		//}

		//if (pSlot->MuscleMeshList.GetCount())
		//{
		//	//	Render muscle meshes
		////	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
		//	if (0)
		//		RenderMuscleMeshes_VS(pViewport, pSlot);
		//	else
		//		RenderMuscleMeshes_SW(pViewport, pSlot);
		//}

		//	Render cloth meshes
		if (pSlot->ClothMeshList.GetCount())
			RenderClothMeshes(pViewport, pSlot);
	}


	if (bRenderAlpha && m_AlphaMeshList.GetCount())
		RenderAlphaMeshes(pViewport);

	//	Clear all vertex streams
	for (i=0; i <= MAXNUM_HWMORPHCHANNEL; i++)
		m_pA3DDevice->ClearStreamSource(i);

	m_pA3DDevice->ClearVertexShader();
	m_pA3DDevice->ClearPixelShader();
	for(i = 0; i < MAX_SAMPLERCOUNT; i++)
	{
		m_pA3DDevice->ClearTexture(i);
	}

	//	Restore light information
	RestoreLightInformation();

	//	Restore cull mode
	m_pA3DDevice->SetFaceCull(m_CurCull);

	//Ӧ��ģ������
	for(int i = 0; i < m_aModiferList.GetSize(); i++)
	{
		MODIFIERITEM& item = m_aModiferList[i];
		item.pModifier->AfterForwardRender(item.pSkinModel);
	}
	for(int i = 0; i < m_aModiferList.GetSize(); i++)
	{
		A3DSkinModelRenderModifier* pModifier = m_aModiferList[i].pModifier;
		pModifier->ClearAfterAllRender();
	}
	m_aModiferList.RemoveAll(false);

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	return true;
}

//	Render alpha skin models
bool A3DSkinRender::RenderAlpha(A3DViewport* pViewport)
{
	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__));
	if (!m_pA3DDevice || !m_AlphaMeshList.GetCount())
		return true;

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	//	Save light information
	SaveLightInformation();

	//	Set some vertex shader constants
	SetupVPTMConstant(pViewport);

	//	Save current cull mode
	m_CurCull = m_pA3DDevice->GetFaceCull();

	//	Render alpha meshes
	RenderAlphaMeshes(pViewport);

	//	Clear all vertex streams
	for (int i=0; i <= MAXNUM_HWMORPHCHANNEL; i++)
		m_pA3DDevice->ClearStreamSource(i);

	//	Restore light information
	RestoreLightInformation();

	//	Restore cull mode
	m_pA3DDevice->SetFaceCull(m_CurCull);

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	return true;
}

void A3DSkinRender::SaveLightInformation()
{
	//	Save light information
	m_dwAmbient = m_pA3DDevice->GetAmbientColor();

	if (m_psm->m_pDirLight)
		m_DirLightParams = m_psm->m_pDirLight->GetLightparam();

	if (m_psm->m_pDPointLight)
		m_psm->m_pDPointLight->TurnOn();

	if (m_ColorOP != A3DTOP_MODULATE)
		m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
}

void A3DSkinRender::RestoreLightInformation()
{
	//	Restore light information
	m_pA3DDevice->SetAmbient(m_dwAmbient);
	if (m_psm->m_pDirLight)
		m_psm->m_pDirLight->SetLightParam(m_DirLightParams);

	if (m_psm->m_pDPointLight)
		m_psm->m_pDPointLight->TurnOff();

	if (m_ColorOP != A3DTOP_MODULATE)
		m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
}

void A3DSkinRender::SetupVPTMConstant(A3DViewport* pViewport)
{
	//	Set some vertex shader constants
	int iRenderMethod = m_pA3DDevice->GetSkinModelRenderMethod();
	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
	{

		A3DCameraBase* pCamera = pViewport->GetCamera();

		//	Set project matrix
		A3DMATRIX4 mat = pCamera->GetProjectionTM() * pCamera->GetPostProjectTM();
		mat.Transpose();

		m_pA3DDevice->SetVertexShaderConstants(SMVSC_PROJECT, &mat, 4);

		//	Set eye's direction in camera space, it is the z axis in fact
		A3DVECTOR4 vEyeDir(0.0f, 0.0f, 1.0f, 0.0f);
		m_pA3DDevice->SetVertexShaderConstants(SMVSC_EYEDIR, &vEyeDir, 1);
	}
}


//	Render skin meshes of specified slot using indexed vertex matrix
//bool A3DSkinRender::RenderSkinMeshes_IVM(A3DViewport* pViewport, RENDERSLOT* pSlot)
//{
//	//	Set render state
//	m_pA3DDevice->SetIndexedVertexBlendEnable(true);
//	m_pA3DDevice->SetVertexBlendFlag(A3DVBF_3WEIGHTS);
//
//	//	Change texture
//	ALISTPOSITION pos = pSlot->SkinMeshList.GetHeadPosition();
//	MESHNODE* pMeshNode = pSlot->SkinMeshList.GetAt(pos);
//
//	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
//	if (!pTexture)
//		m_pA3DDevice->ClearTexture(0);
//	else
//		pTexture->Appear(0);
//	// no normal map in indexed mode
//
//	while (pos)
//	{
//		pMeshNode = pSlot->SkinMeshList.GetNext(pos);
//
//		A3DSkinMesh* pMesh = (A3DSkinMesh*)pMeshNode->pMeshRef->pMesh;
//		A3DSkin* pSkin = pMeshNode->pSkin;
//		A3DSkinModel* pModel = pMeshNode->pSkinModel;
//
//		//	Apply model's light
//		m_psm->ApplyModelLightTo(pModel);
//		//	Set material
//		ApplyMaterial(pMeshNode);
//		//	Render mesh
//		pMesh->Render(pViewport, pModel, pSkin);
//	}
//
//	//	Restore render state
//	if (pTexture)
//	{
//		//	Texture may be a A3DShader, so we must call Disappear
//		pTexture->Disappear();
//		if( pTexture->IsShaderTexture() )
//		{
//			if (m_ColorOP != A3DTOP_MODULATE)
//				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
//		}
//	}
//
//	m_pA3DDevice->SetIndexedVertexBlendEnable(false);
//	m_pA3DDevice->SetVertexBlendFlag(A3DVBF_DISABLE);
//
//	return true;
//}

//	Render skin meshes of specified slot using vertex shader
bool A3DSkinRender::RenderMeshes(A3DViewport* pViewport, MeshNodeList& mnl, bool bDynLight, int eIdx)
{
	// ��������
	A3DEnvironment* pEnvironment = m_pA3DEngine->GetA3DEnvironment();
	const A3DCOMMCONSTTABLE* pEnvCommConstTab = pEnvironment->GetCommonConstTable();

	// ����vs
	A3DHLSL* pvs = (A3DHLSL*)m_psm->GetMeshVS(bDynLight ? A3DSkinMan::VST_NONSPEC_DLT : A3DSkinMan::VST_NONSPEC, (A3DSkinMan::VertexShaderIdx)eIdx);
	if(pvs != NULL && pvs->GetClassID() == A3D_CID_HLSL)
	{
		pvs->Appear(APPEAR_SETSHADERONLY);
		if( ! bDynLight) {
			pvs->SubmitCommTable(pEnvCommConstTab);
		}
	}
	else {
		return false;
	}

	//	Change texture
	//bool bEffect = false;
	ALISTPOSITION pos = mnl.GetHeadPosition();
	MESHNODE*     pMeshNode = mnl.GetAt(pos);
	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);

	if (!pTexture)
		m_pA3DDevice->ClearTexture(0);
	else
	{
	//	bEffect = (pTexture->GetClassID() == A3D_CID_HLSLSHADER);

	//	if(bEffect)
	//	{
	//		((A3DEffect*)pTexture)->Appear(APPEAR_SETSHADERONLY);
	//	}
	//	else
	//	{
			pTexture->Appear();
	//	}
	}

	while (pos)
	{
		pMeshNode = mnl.GetNext(pos);

		A3DMeshBase*  pMesh = pMeshNode->pMeshRef->pMesh;
		A3DSkin*      pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;

		A3DTexture* pTexture = pMesh->GetTexturePtr(pSkin);
		if(pTexture->GetClassID() == A3D_CID_HLSLSHADER)
		{
			((A3DEffect*)pTexture)->Commit(pEnvCommConstTab);
		}
		


		if(bDynLight)
		{
			const A3DSkinModel::LIGHTINFO& LightInfo = pMeshNode->pSkinModel->GetLightInfo();
			pEnvironment->SetLightInfo(LightInfo);
			pvs->SubmitCommTable(pEnvCommConstTab);
		}

		if(eIdx == A3DSkinMan::VS_RIGIDMESH || eIdx == A3DSkinMan::VS_RIGIDMESHTAN)
		{
			pModel->ApplyVSRigidMeshMat(pViewport, ((A3DRigidMesh*)pMesh)->GetBoneIndex(), SMVSC_BLENDMAT0_VS2);
		}

		//	Render mesh
		pMesh->Render(pViewport, pModel, pSkin);

		

		//if (pMeshNode->pMeshRef->bSWRender)
		//{
		//	//	Apply model's light
		//	m_psm->ApplyModelLightTo(pModel);
		//	//	Set material
		//	ApplyMaterial(pMeshNode);
		//	//	Render mesh
		//	pMesh->RenderSoftware(pViewport, pModel, pSkin);
		//}
		//else
		//{

		//	//	Get vertex shader
		//	//A3DObject* pvs = pMeshNode->pMeshRef->pvs;
		//	//if (!pvs)
		//	//{
		//	//	if (!(pvs = m_psm->GetSkinMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight(), pMesh->IsEnableExtMaterial() && bEffect)))
		//	//		continue;
		//	//}

		//	if(bDynLight)
		//	{
		//		const A3DSkinModel::LIGHTINFO& LightInfo = pMeshNode->pSkinModel->GetLightInfo();
		//		pEnvironment->SetLightInfo(LightInfo);
		//		pvs->SubmitCommTable(pEnvCommConstTab);
		//	}

		//	////PVS_APPEAR;
		//	//if(pvs->GetClassID() == A3D_CID_VERTEXSHADER)
		//	//{
		//	//	((A3DVertexShader*)pvs)->Appear();
		//	//	//	Apply light parameters to vertex shader
		//	//	pModel->ApplyVSLightParams(pViewport);
		//	//	//	Set material
		//	//	ApplyVSMaterial(pMeshNode);
		//	//}
		//	//else if(pvs->GetClassID() == A3D_CID_HLSL)
		//	//{
		//	//	((A3DHLSL*)pvs)->Appear(APPEAR_SETSHADERONLY);
		//	//	((A3DHLSL*)pvs)->SubmitCommTable(pEnvironment->GetCommonConstTable());
		//	//	m_pSkinMeshTanDecl->Appear();
		//	//}


		//	//	Render mesh
		//	pMesh->Render(pViewport, pModel, pSkin);
		//}
	}

	//	Restore render state

	//if (pTexture)
	//{
	//	//	Texture may be a A3DShader, so we must call Disappear
	//	pTexture->Disappear();
	//	if( pTexture->IsShaderTexture() )
	//	{
	//		if (m_ColorOP != A3DTOP_MODULATE)
	//			m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
	//	}
	//}

	m_pA3DDevice->ClearVertexShader();
	return true;
}

//	Render skin meshes of specified slot using software
//bool A3DSkinRender::RenderSkinMeshes_SW(A3DViewport* pViewport, RENDERSLOT* pSlot)
//{
//	//	Set render state
//	m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);
//
//	//	Change texture
//	ALISTPOSITION pos = pSlot->SkinMeshList.GetHeadPosition();
//	MESHNODE* pMeshNode = pSlot->SkinMeshList.GetAt(pos);
//
//	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
//	if (!pTexture)
//		m_pA3DDevice->ClearTexture(0);
//	else
//		pTexture->Appear();
//	// no normal map in sw
//
//	while (pos)
//	{
//		pMeshNode = pSlot->SkinMeshList.GetNext(pos);
//
//		A3DSkinMesh* pMesh = (A3DSkinMesh*)pMeshNode->pMeshRef->pMesh;
//		A3DSkin* pSkin = pMeshNode->pSkin;
//		A3DSkinModel* pModel = pMeshNode->pSkinModel;
//
//		//	Apply model's light
//		m_psm->ApplyModelLightTo(pModel);
//		//	Set material
//		ApplyMaterial(pMeshNode);
//		//	Render mesh
//		pMesh->Render(pViewport, pModel, pSkin);
//	}
//
//	if (pTexture)
//	{
//		//	Texture may be a A3DShader, so we must call Disappear
//		pTexture->Disappear();
//		if( pTexture->IsShaderTexture() )
//		{
//			if (m_ColorOP != A3DTOP_MODULATE)
//				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
//		}
//	}
//
//	return true;
//}

//	Render rigid meshes of specified slot
//bool A3DSkinRender::RenderRigidMeshes(A3DViewport* pViewport, MeshNodeList& mnl, bool bDynLight)
//{
//	//	Change texture
//	ALISTPOSITION pos = mnl.GetHeadPosition();
//	MESHNODE* pMeshNode = mnl.GetAt(pos);
//	A3DEnvironment*	         pEnvironment = m_pA3DEngine->GetA3DEnvironment();
//	const A3DCOMMCONSTTABLE* pEnvCommConstTab = pEnvironment->GetCommonConstTable();
//
//	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
//	if (!pTexture)
//		m_pA3DDevice->ClearTexture(0);
//	else
//	{
//		if(pTexture->GetClassID() == A3D_CID_HLSLSHADER)
//		{
//			((A3DEffect*)pTexture)->Appear(pEnvCommConstTab);
//		}
//		else {
//			pTexture->Appear();
//		}
//	}
//
//	if (m_pA3DDevice->GetSkinModelRenderMethod() == A3DDevice::SMRD_VERTEXSHADER)
//	{
//		while (pos)
//		{
//			pMeshNode = mnl.GetNext(pos);
//
//			A3DRigidMesh* pMesh = (A3DRigidMesh*)pMeshNode->pMeshRef->pMesh;
//			A3DSkin* pSkin = pMeshNode->pSkin;
//			A3DSkinModel* pModel = pMeshNode->pSkinModel;
//
//			//	Get vertex shader
//			A3DObject* pvs = pMeshNode->pMeshRef->pvs;
//			if (!pvs)
//			{
//				if (!(pvs = m_psm->GetRigidMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight())))
//					continue;
//			}
//
//			const A3DSkinModel::LIGHTINFO& LightInfo = pMeshNode->pSkinModel->GetLightInfo();
//			pEnvironment->SetLightInfo(LightInfo);
//			
//			if(pvs->GetClassID() == A3D_CID_HLSL) {
//				((A3DHLSL*)pvs)->Appear(APPEAR_SETSHADERONLY);
//				((A3DHLSL*)pvs)->SubmitCommTable(pEnvironment->GetCommonConstTable());
//				//m_pSkinMeshTanDecl->Appear();
//			}
//
//
//			////	Apply light parameters to vertex shader
//			//pModel->ApplyVSLightParams(pViewport);
//			////	Set material
//			//ApplyVSMaterial(pMeshNode);
//			m_pRigidMeshDecl->Appear();
//			//	Set blending matrices as vertex constants
//			pModel->ApplyVSRigidMeshMat(pViewport, pMesh->GetBoneIndex(), SMVSC_BLENDMAT0_VS2);
//
//
//			//	Render mesh
//			pMesh->Render(pViewport, pModel, pSkin);
//
//		}
//
//	}
//	else
//	{
//		while (pos)
//		{
//			pMeshNode = mnl.GetNext(pos);
//
//			A3DRigidMesh* pMesh = (A3DRigidMesh*)pMeshNode->pMeshRef->pMesh;
//			A3DSkin* pSkin = pMeshNode->pSkin;
//			A3DSkinModel* pModel = pMeshNode->pSkinModel;
//
//			//	Apply model's light
//			m_psm->ApplyModelLightTo(pModel);
//			//	Set material
//			ApplyMaterial(pMeshNode);
//			//	Render mesh
//			pMesh->Render(pViewport, pModel, pSkin);
//		}
//	}
//
//	if (pTexture)
//	{
//		//	Texture may be a A3DShader, so we must call Disappear
//		pTexture->Disappear();
//		if( pTexture->IsShaderTexture() )
//		{
//			if (m_ColorOP != A3DTOP_MODULATE)
//				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
//		}
//	}
//
//	return true;
//}

//	Render supple meshes of specified slot
//bool A3DSkinRender::RenderSuppleMeshes(A3DViewport* pViewport, RENDERSLOT* pSlot)
//{
//	//	Change texture
//	ALISTPOSITION pos = pSlot->SuppleMeshList.GetHeadPosition();
//	MESHNODE* pMeshNode = pSlot->SuppleMeshList.GetAt(pos);
//
//	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
//	if (!pTexture)
//		m_pA3DDevice->ClearTexture(0);
//	else
//		pTexture->Appear();
//	// no normal map supported for supple meshes
//
//	while (pos)
//	{
//		pMeshNode = pSlot->SuppleMeshList.GetNext(pos);
//
//		A3DRigidMesh* pMesh = (A3DRigidMesh*)pMeshNode->pMeshRef->pMesh;
//		A3DSkin* pSkin = pMeshNode->pSkin;
//		A3DSkinModel* pModel = pMeshNode->pSkinModel;
//
//		//	Apply model's light
//		m_psm->ApplyModelLightTo(pModel);
//		//	Set material
//		ApplyMaterial(pMeshNode);
//		//	Render mesh
//		pMesh->Render(pViewport, pModel, pSkin);
//	}
//
//	if (pTexture)
//	{
//		//	Texture may be a A3DShader, so we must call Disappear
//		pTexture->Disappear();
//		if( pTexture->IsShaderTexture() )
//		{
//			if (m_ColorOP != A3DTOP_MODULATE)
//				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
//		}
//	}
//
//	return true;
//}

//	Render cloth meshes
bool A3DSkinRender::RenderClothMeshes(A3DViewport* pViewport, RENDERSLOT* pSlot)
{
	//	Change texture
	ALISTPOSITION pos = pSlot->ClothMeshList.GetHeadPosition();
	MESHNODE* pMeshNode = pSlot->ClothMeshList.GetAt(pos);

	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
	if (!pTexture)
		m_pA3DDevice->ClearTexture(0);
	else
		pTexture->Appear();
	
	while (pos)
	{
		pMeshNode = pSlot->ClothMeshList.GetNext(pos);

		A3DClothMeshImp* pMesh = (A3DClothMeshImp*)pMeshNode->pMeshRef->pMesh;
		A3DSkin* pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;

		//	Apply model's light
		m_psm->ApplyModelLightTo(pModel);
		//	Set material
		//ApplyMaterial(pMeshNode);
		//	Render mesh
		pMesh->Render(pViewport, pModel, pSkin);
	}

	if (pTexture)
	{
		//	Texture may be a A3DShader, so we must call Disappear
		pTexture->Disappear();
		if (pTexture->IsShaderTexture())
		{
			if (m_ColorOP != A3DTOP_MODULATE)
				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
		}
	}

	return true;
}

//	Render morph meshes of specified slot using vertex shader
//bool A3DSkinRender::RenderMorphMeshes_VS(A3DViewport* pViewport, RENDERSLOT* pSlot)
//{
//	//	Change texture
//	ALISTPOSITION pos = pSlot->MorphMeshList.GetHeadPosition();
//	MESHNODE* pMeshNode = pSlot->MorphMeshList.GetAt(pos);
//	A3DEnvironment* pEnvironment = m_pA3DEngine->GetA3DEnvironment();
//
//	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
//	if (!pTexture)
//		m_pA3DDevice->ClearTexture(0);
//	else
//		pTexture->Appear();
//	// no normal map supported for morph meshes
//
//	while (pos)
//	{
//		pMeshNode = pSlot->MorphMeshList.GetNext(pos);
//
//		A3DSkin* pSkin = pMeshNode->pSkin;
//		A3DSkinModel* pModel = pMeshNode->pSkinModel;
//
//		//	Select proper vertex shader
//		int iVSType, iVSIndex;
//
//		iVSType = pModel->IsSpecularEnable() ? 0 : 1;
//		if (pModel->HasDynamicLight())
//			iVSType += 2;
//
//		if (pMeshNode->pMeshRef->pMesh->GetClassID() == A3D_CID_MORPHSKINMESH)
//		{
//			A3DMorphSkinMesh* pMesh = (A3DMorphSkinMesh*)pMeshNode->pMeshRef->pMesh;
//			iVSIndex = A3DSkinMan::VS_MORPHSKINMESH0 + pMesh->m_MorphData.GetActiveMorphChannelNum();
//		}
//		else	//	A3D_CID_MORPHRIGIDMESH
//		{
//			A3DMorphRigidMesh* pMesh = (A3DMorphRigidMesh*)pMeshNode->pMeshRef->pMesh;
//			iVSIndex = A3DSkinMan::VS_MORPHRIGIDMESH0 + pMesh->m_MorphData.GetActiveMorphChannelNum();
//		}
//
//		if (!m_psm->m_aVS[iVSType][iVSIndex])
//			continue;
//
//		const A3DSkinModel::LIGHTINFO& LightInfo = pMeshNode->pSkinModel->GetLightInfo();
//		pEnvironment->SetLightInfo(LightInfo);
//		PVS_APPEAR_PARAM(m_psm->m_aVS[iVSType][iVSIndex]);
//
//		//	Apply light parameters to vertex shader
//		pModel->ApplyVSLightParams(pViewport);
//		//	Set material
//		ApplyVSMaterial(pMeshNode);
//		//	Render mesh
//		pMeshNode->pMeshRef->pMesh->Render(pViewport, pModel, pSkin);
//	}
//
//	if (pTexture)
//	{
//		//	Texture may be a A3DShader, so we must call Disappear
//		pTexture->Disappear();
//		if( pTexture->IsShaderTexture() )
//		{
//			if (m_ColorOP != A3DTOP_MODULATE)
//				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
//		}
//	}
//
//	m_pA3DDevice->ClearVertexShader();
//
//	return true;
//}
//
////	Render morph meshes of specified slot using software
//bool A3DSkinRender::RenderMorphMeshes_SW(A3DViewport* pViewport, RENDERSLOT* pSlot)
//{
//	//	Change texture
//	ALISTPOSITION pos = pSlot->MorphMeshList.GetHeadPosition();
//	MESHNODE* pMeshNode = pSlot->MorphMeshList.GetAt(pos);
//
//	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
//	if (!pTexture)
//		m_pA3DDevice->ClearTexture(0);
//	else
//		pTexture->Appear();
//	// no normal map supported for morph meshes
//
//	while (pos)
//	{
//		pMeshNode = pSlot->MorphMeshList.GetNext(pos);
//
//		A3DMeshBase* pMesh = pMeshNode->pMeshRef->pMesh;
//		A3DSkin* pSkin = pMeshNode->pSkin;
//		A3DSkinModel* pModel = pMeshNode->pSkinModel;
//
//		//	Apply model's light
//		m_psm->ApplyModelLightTo(pModel);
//		//	Set material
//		ApplyMaterial(pMeshNode);
//		//	Render mesh
//		pMesh->Render(pViewport, pModel, pSkin);
//	}
//
//	if (pTexture)
//	{
//		//	Texture may be a A3DShader, so we must call Disappear
//		pTexture->Disappear();
//		if( pTexture->IsShaderTexture() )
//		{
//			if (m_ColorOP != A3DTOP_MODULATE)
//				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
//		}
//	}
//
//	return true;
//}
//
////	Render muscle meshes of specified slot using vertex shader
//bool A3DSkinRender::RenderMuscleMeshes_VS(A3DViewport* pViewport, RENDERSLOT* pSlot)
//{
//	return true;
//}
//
////	Render muscle meshes of specified slot using software
//bool A3DSkinRender::RenderMuscleMeshes_SW(A3DViewport* pViewport, RENDERSLOT* pSlot)
//{
//	//	Set render state
//	m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);
//
//	//	Change texture
//	ALISTPOSITION pos = pSlot->MuscleMeshList.GetHeadPosition();
//	MESHNODE* pMeshNode = pSlot->MuscleMeshList.GetAt(pos);
//
//	A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
//	if (!pTexture)
//		m_pA3DDevice->ClearTexture(0);
//	else
//		pTexture->Appear();
//	// no normal map supported for muslce meshes
//
//	while (pos)
//	{
//		pMeshNode = pSlot->MuscleMeshList.GetNext(pos);
//
//		A3DMuscleMeshImp* pMesh = (A3DMuscleMeshImp*)pMeshNode->pMeshRef->pMesh;
//		A3DSkin* pSkin = pMeshNode->pSkin;
//		A3DSkinModel* pModel = pMeshNode->pSkinModel;
//
//		//	Apply model's light
//		m_psm->ApplyModelLightTo(pModel);
//		//	Set material
//		ApplyMaterial(pMeshNode);
//		//	Render mesh
//		pMesh->Render(pViewport, pModel, pSkin);
//	}
//
//	if (pTexture)
//	{
//		//	Texture may be a A3DShader, so we must call Disappear
//		pTexture->Disappear();
//		if( pTexture->IsShaderTexture() )
//		{
//			if (m_ColorOP != A3DTOP_MODULATE)
//				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
//		}
//	}
//
//	return true;
//}

bool A3DSkinRender::RenderBloomMeshes(A3DViewport* pViewport, float fBrightScale)
{
	
	A3DEnvironment* pEnvironment = m_pA3DEngine->GetA3DEnvironment();
	
	for( int i = 0; i < m_BloomMeshes.GetSize(); i++)
	{
		MESHNODE* pMeshNode = m_BloomMeshes[i];
		A3DSkin* pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;
		A3DSkinMesh* pMesh = (A3DSkinMesh*)pMeshNode->pMeshRef->pMesh;
		if (NULL == pMesh)
			continue;
	
		A3DTexture* pTex = NULL;
		for(int j =0; j < pSkin->GetTextureNum(); j++)
		{
			 pTex = pSkin->GetTexture(j);

			if (pTex->IsShaderTexture())
			{
				pTex->Appear(0);
				break;
			}
		}

		A3DObject* pvs = pMeshNode->pMeshRef->pvs;
		const A3DSkinModel::LIGHTINFO& LightInfo = pMeshNode->pSkinModel->GetLightInfo();
		pEnvironment->SetLightInfo(LightInfo);
		switch (pMesh->GetClassID())
		{
			case A3D_CID_SKINMESH:
			{
				
				if (!pvs)
				{
					if (!(pvs = m_psm->GetSkinMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight(), pMesh->IsEnableExtMaterial())))
						return true;
				}

				PVS_APPEAR;
				break;
					
			}
			case A3D_CID_RIGIDMESH:
				{					
					int iRenderMethod = m_pA3DDevice->GetSkinModelRenderMethod();
					if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
					{
						if (!pvs)
						{
							bool bDynamic = pModel->HasDynamicLight();
							if (!(pvs = m_psm->GetMeshVS(bDynamic ? 
								A3DSkinMan::VST_NONSPEC_DLT : A3DSkinMan::VST_NONSPEC, 
								A3DSkinMan::VS_RIGIDMESHTAN)))
								return true;
						}

						if (pvs->GetClassID() == A3D_CID_HLSL)
						{
							((A3DHLSL*)pvs)->Appear(APPEAR_SETSHADERONLY);
							((A3DHLSL*)pvs)->SubmitCommTable(pEnvironment->GetCommonConstTable());
							m_pRigidMeshTanDecl->Appear();
							pModel->ApplyVSRigidMeshMat(pViewport, ((A3DRigidMesh*)pMesh)->GetBoneIndex(), SMVSC_BLENDMAT0_VS2);
						}
					}
					break;
				}
			default:
				break;
		}


		A3DCameraBase* pCamera = pViewport->GetCamera();
		
		//	Set project matrix
		A3DMATRIX4 mat = pCamera->GetProjectionTM() * pCamera->GetPostProjectTM();
		mat.Transpose();
		
		m_pA3DDevice->SetVertexShaderConstants(SMVSC_PROJECT, &mat, 4);
		
		//	Set eye's direction in camera space, it is the z axis in fact
		A3DVECTOR4 vEyeDir(0.0f, 0.0f, 1.0f, 0.0f);
		m_pA3DDevice->SetVertexShaderConstants(SMVSC_EYEDIR, &vEyeDir, 1);
		m_pBloomMeshPS->SetValue4f("vBloomCol", &(pSkin->GetBloomColor()));

		//	Render mesh
		m_pBloomMeshPS->Appear();
		pMesh->Render(pViewport, pModel, pSkin);

		if(pvs->GetClassID() == A3D_CID_VERTEXSHADER)
			((A3DVertexShader*)pvs)->Disappear();

		if( pTex)
			pTex->Disappear();
		m_pBloomMeshPS->Disappear();
	}

	ResetBloomMeshes();
	return true;
}

//	Render alpha meshes
bool A3DSkinRender::RenderAlphaMeshes(A3DViewport* pViewport)
{
	A3DTexture* pLastTexture = NULL;
	A3DHLSL*    pLastMeshVS = NULL;
	DWORD dwTexture = (DWORD)(-1);

	A3DEnvironment*          pEnvironment     = m_pA3DEngine->GetA3DEnvironment();
	const A3DCOMMCONSTTABLE* pEnvCommConstTab = pEnvironment->GetCommonConstTable();
	
	int iRenderMethod = m_pA3DDevice->GetSkinModelRenderMethod();
	bool bAlphaComp = m_pA3DDevice->GetAlphaTestEnable();

	ALISTPOSITION pos = m_AlphaMeshList.GetHeadPosition();

	while (pos)
	{
		MESHNODE* pMeshNode = m_AlphaMeshList.GetNext(pos);
		A3DSkin* pSkin = pMeshNode->pSkin;
		A3DSkinModel* pModel = pMeshNode->pSkinModel;
		const bool bDynamicLight = pModel->HasDynamicLight();

		//	Apply mesh texture
		ApplyMeshTexture(&pLastTexture, dwTexture, pMeshNode, pEnvCommConstTab);

		//	Apply mesh render state
		m_pA3DDevice->SetSourceAlpha(pModel->GetSrcBlend());
		m_pA3DDevice->SetDestAlpha(pModel->GetDstBlend());
		if( pModel->GetAlphaCompFlag() )
			m_pA3DDevice->SetAlphaTestEnable(true);

		switch (pMeshNode->pMeshRef->pMesh->GetClassID())
		{
		case A3D_CID_SKINMESH:
		{
			A3DSkinMesh* pMesh = (A3DSkinMesh*)pMeshNode->pMeshRef->pMesh;

			switch (iRenderMethod)
			{
			case A3DDevice::SMRD_VERTEXSHADER:
			{
				//if (pMeshNode->pMeshRef->bSWRender)
				//{
				//	//	Apply model's light
				//	m_psm->ApplyModelLightTo(pModel);
				//	//	Set material
				//	ApplyMaterial(pMeshNode);
				//	//	Render mesh
				//	pMesh->RenderSoftware(pViewport, pModel, pSkin);
				//}
				//else
				//{
					A3DObject* pvs = pMeshNode->pMeshRef->pvs;
					if ( ! pvs)
					{
						if ( ! (pvs = m_psm->GetMeshVS(bDynamicLight 
							? A3DSkinMan::VST_NONSPEC_DLT 
							: A3DSkinMan::VST_NONSPEC, A3DSkinMan::VS_SKINMESH4TAN ))) {
							goto NextMesh;
						}
					}

					if(pvs->GetClassID() == A3D_CID_HLSL)
					{
				

					if(pLastMeshVS != pvs)
						{
							pLastMeshVS = (A3DHLSL*)pvs;
							pLastMeshVS->Appear(APPEAR_SETSHADERONLY);
							m_pSkinMeshDecl->Appear();

							// ��̬����������³���
							if( ! bDynamicLight) {
								pLastMeshVS->SubmitCommTable(pEnvCommConstTab);
							}
						}

						if(bDynamicLight)
						{
							const A3DSkinModel::LIGHTINFO& LightInfo = pMeshNode->pSkinModel->GetLightInfo();
							pEnvironment->SetLightInfo(LightInfo);
							pLastMeshVS->SubmitCommTable(pEnvCommConstTab);
						}
						
						////	Apply light parameters to vertex shader
						//pModel->ApplyVSLightParams(pViewport);
						////	Set material
						//ApplyVSMaterial(pMeshNode);
						//	Render mesh
						pMesh->Render(pViewport, pModel, pSkin);

					//}
				}
				break;
			}
			//case A3DDevice::SMRD_INDEXEDVERTMATRIX:
			//{
			//	//	Set render state
			//	m_pA3DDevice->SetIndexedVertexBlendEnable(true);
			//	m_pA3DDevice->SetVertexBlendFlag(A3DVBF_3WEIGHTS);

			//	//	Apply model's light
			//	m_psm->ApplyModelLightTo(pModel);
			//	//	Set material
			//	ApplyMaterial(pMeshNode);
			//	//	Render mesh
			//	pMesh->Render(pViewport, pModel, pSkin);

			//	//	Restore render state
			//	m_pA3DDevice->SetIndexedVertexBlendEnable(false);
			//	m_pA3DDevice->SetVertexBlendFlag(A3DVBF_DISABLE);

			//	break;
			//}
			//case A3DDevice::SMRD_SOFTWARE:
			//{	
			//	//	Set render state
			//	m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

			//	//	Apply model's light
			//	m_psm->ApplyModelLightTo(pModel);
			//	//	Set material
			//	ApplyMaterial(pMeshNode);
			//	//	Render mesh
			//	pMesh->Render(pViewport, pModel, pSkin);

			//	break;
			//}
			}

			break;
		}
		case A3D_CID_RIGIDMESH:
		{
			A3DRigidMesh* pMesh = (A3DRigidMesh*)pMeshNode->pMeshRef->pMesh;

			if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
			{
				A3DObject* pvs = pMeshNode->pMeshRef->pvs;
				if ( ! pvs)
				{
					if ( ! (pvs = m_psm->GetMeshVS(bDynamicLight 
						? A3DSkinMan::VST_NONSPEC_DLT 
						: A3DSkinMan::VST_NONSPEC, A3DSkinMan::VS_RIGIDMESHTAN ))) {
						goto NextMesh;
					}
				}

				if(pvs->GetClassID() == A3D_CID_HLSL)
				{
					if(pLastMeshVS != pvs)
					{
						pLastMeshVS = (A3DHLSL*)pvs;
						pLastMeshVS->Appear(APPEAR_SETSHADERONLY);
						m_pRigidMeshDecl->Appear();

						// ��̬����������³���
						if( ! bDynamicLight) {
							pLastMeshVS->SubmitCommTable(pEnvCommConstTab);
						}
					}

					if(bDynamicLight)
					{
						const A3DSkinModel::LIGHTINFO& LightInfo = pMeshNode->pSkinModel->GetLightInfo();
						pEnvironment->SetLightInfo(LightInfo);
						pLastMeshVS->SubmitCommTable(pEnvCommConstTab);
					}

					////	Apply light parameters to vertex shader
					//pModel->ApplyVSLightParams(pViewport);
					////	Set material
					//ApplyVSMaterial(pMeshNode);
					//	Render mesh
					pModel->ApplyVSRigidMeshMat(pViewport, pMesh->GetBoneIndex(), SMVSC_BLENDMAT0_VS2);
					pMesh->Render(pViewport, pModel, pSkin);

				}


				//pLastSkinMeshVS = NULL;
				//A3DObject* pvs = pMeshNode->pMeshRef->pvs;
				//if (!pvs)
				//{
				//	if (!(pvs = m_psm->GetRigidMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight())))
				//		continue;
				//}

				//const A3DSkinModel::LIGHTINFO& LightInfo = pMeshNode->pSkinModel->GetLightInfo();
				//pEnvironment->SetLightInfo(LightInfo);
				//PVS_APPEAR;

				////	Apply light parameters to vertex shader
				//pModel->ApplyVSLightParams(pViewport);
				////	Set material
				//ApplyVSMaterial(pMeshNode);
				////	Set blending matrices as vertex constants
				//pModel->ApplyVSRigidMeshMat(pViewport, pMesh->GetBoneIndex(), SMVSC_BLENDMAT0_VS2);

				////	Render mesh
				//pMesh->Render(pViewport, pModel, pSkin);

			
			}
			//else
			//{
			//	//	Apply model's light
			//	m_psm->ApplyModelLightTo(pModel);
			//	//	Set material
			//	ApplyMaterial(pMeshNode);
			//	//	Render mesh
			//	pMesh->Render(pViewport, pModel, pSkin);
			//}

			break;
		}
		//case A3D_CID_SUPPLEMESHIMP:
		//case A3D_CID_MUSCLEMESHIMP:
		case A3D_CID_CLOTHMESHIMP:
		{
			A3DMeshBase* pMesh = (A3DMeshBase*)pMeshNode->pMeshRef->pMesh;

			//	Apply model's light
			m_psm->ApplyModelLightTo(pModel);
			//	Set material
			//ApplyMaterial(pMeshNode);
			//	Render mesh
			pMesh->Render(pViewport, pModel, pSkin);

			break;
		}
		//case A3D_CID_MORPHSKINMESH:
		//case A3D_CID_MORPHRIGIDMESH:
		//{
		//	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
		//	{
		//		//	Apply light parameters to vertex shader
		//		pModel->ApplyVSLightParams(pViewport);

		//		//	Select proper vertex shader
		//		int iVSType, iVSIndex;

		//		iVSType = pModel->IsSpecularEnable() ? 0 : 1;
		//		if (pModel->HasDynamicLight())
		//			iVSType += 2;

		//		if (pMeshNode->pMeshRef->pMesh->GetClassID() == A3D_CID_MORPHSKINMESH)
		//		{
		//			A3DMorphSkinMesh* pMesh = (A3DMorphSkinMesh*)pMeshNode->pMeshRef->pMesh;
		//			iVSIndex = A3DSkinMan::VS_MORPHSKINMESH0 + pMesh->m_MorphData.GetActiveMorphChannelNum();
		//		}
		//		else	//	A3D_CID_MORPHRIGIDMESH
		//		{
		//			A3DMorphRigidMesh* pMesh = (A3DMorphRigidMesh*)pMeshNode->pMeshRef->pMesh;
		//			iVSIndex = A3DSkinMan::VS_MORPHRIGIDMESH0 + pMesh->m_MorphData.GetActiveMorphChannelNum();
		//		}

		//		if (!m_psm->m_aVS[iVSType][iVSIndex])
		//			continue;
		//		pLastMeshVS = NULL;

		//		const A3DSkinModel::LIGHTINFO& LightInfo = pMeshNode->pSkinModel->GetLightInfo();
		//		pEnvironment->SetLightInfo(LightInfo);
		//		PVS_APPEAR_PARAM(m_psm->m_aVS[iVSType][iVSIndex]);

		//		//	Set material
		//		ApplyVSMaterial(pMeshNode);
		//		//	Render mesh
		//		pMeshNode->pMeshRef->pMesh->Render(pViewport, pModel, pSkin);

		//	//	m_pA3DDevice->ClearVertexShader();
		//	}
		//	else
		//	{
		//		A3DMeshBase* pMesh = pMeshNode->pMeshRef->pMesh;

		//		//	Apply model's light
		//		m_psm->ApplyModelLightTo(pModel);
		//		//	Set material
		//		ApplyMaterial(pMeshNode);
		//		//	Render mesh
		//		pMesh->Render(pViewport, pModel, pSkin);
		//	}

		//	break;
		//}
		default:
			ASSERT(0);
			break;
		}

	NextMesh:;

	}

	if (pLastTexture)
	{
		//	Texture may be a A3DShader, so we must call Disappear
		pLastTexture->Disappear();
		if( pLastTexture->IsShaderTexture() )
		{
			if (m_ColorOP != A3DTOP_MODULATE)
				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
		}
	}

	m_pA3DDevice->ClearVertexShader();

	//	Restore mesh render state
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetAlphaTestEnable(bAlphaComp);

	return true;
}
//	Render specified skin model at once
bool A3DSkinRender::RenderSkinModelAtOnce(A3DViewport* pViewport, A3DSkinModel* pSkinModel, 
									   DWORD dwFlags, const A3DReplaceHLSL* pRepHLSL)
{
	return true;
}

////	Render specified skin model at once
//bool A3DSkinRender::RenderSkinModelAtOnce(A3DViewport* pViewport, A3DSkinModel* pSkinModel, 
//									   DWORD dwFlags)
//{
//	if (!m_pA3DDevice)
//		return true;
//
//	//	Save light information
//	DWORD dwAmbient = m_pA3DDevice->GetAmbientColor();
//	A3DLIGHTPARAM DirLightParams;
//	if (m_psm->m_pDirLight)
//		DirLightParams = m_psm->m_pDirLight->GetLightparam();
//
//	if (m_psm->m_pDPointLight)
//		m_psm->m_pDPointLight->TurnOn();
//
//	if (m_ColorOP != A3DTOP_MODULATE)
//		m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
//
//	//	Set some vertex shader constants
//	int iRenderMethod = m_pA3DDevice->GetSkinModelRenderMethod();
//	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
//	{
//		A3DCameraBase* pCamera = pViewport->GetCamera();
//
//		//	Set project matrix
//		A3DMATRIX4 mat = pCamera->GetProjectionTM() * pCamera->GetPostProjectTM();
//		mat.Transpose();
//
//		m_pA3DDevice->SetVertexShaderConstants(SMVSC_PROJECT, &mat, 4);
//
//		//	Set eye's direction in camera space, it is the z axis in fact
//		A3DVECTOR4 vEyeDir(0.0f, 0.0f, 1.0f, 0.0f);
//		m_pA3DDevice->SetVertexShaderConstants(SMVSC_EYEDIR, &vEyeDir, 1);
//
//		//	Apply light parameters to vertex shader
//		pSkinModel->ApplyVSLightParams(pViewport);
//	}
//
//	bool bAlphaModel = false;
//	if (pSkinModel->GetSrcBlend() != A3DBLEND_SRCALPHA ||
//		pSkinModel->GetDstBlend() != A3DBLEND_INVSRCALPHA)
//		bAlphaModel = true;
//
//	if (bAlphaModel)
//	{
//		m_pA3DDevice->SetSourceAlpha(pSkinModel->GetSrcBlend());
//		m_pA3DDevice->SetDestAlpha(pSkinModel->GetDstBlend());
//		m_pA3DDevice->SetAlphaBlendEnable(true);
//	}
//
//	//	Save current cull mode
//	m_CurCull = m_pA3DDevice->GetFaceCull();
//
//	int i;
//
//	//	Prepare a transformed clip plane
//	m_bUserClip = false;
//	DWORD dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_CLIPPLANEENABLE);
//	if (dwState == D3DCLIPPLANE0) // only one clip plane supported now
//	{
//		m_bUserClip = true;
//		m_pA3DDevice->GetClipPlane(0, (float *)&m_cp);
//		A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
//		matVP.InverseTM();
//		matVP.Transpose();
//		D3DXPlaneTransform(&m_hcp, &m_cp, (D3DXMATRIX *) &matVP);
//	}
//
//	//	Apply model's light
//	pSkinModel->ApplyModelLight(m_psm->m_pDirLight, m_psm->m_pDPointLight);
//
//	if (dwFlags & A3DSkinModel::RAO_NOTEXTURE)
//		m_pA3DDevice->ClearTexture(0);
//
//	//	Render all skins
//	for (i=0; i < pSkinModel->GetSkinNum(); i++)
//		RenderSkinAtOnce(pViewport, pSkinModel, i, dwFlags);
//
//	//	Clear all vertex streams
//	for (i=0; i <= MAXNUM_HWMORPHCHANNEL; i++)
//		m_pA3DDevice->ClearStreamSource(i);
//
//	//	Restore light information
//	m_pA3DDevice->SetAmbient(dwAmbient);
//	if (m_psm->m_pDirLight)
//		m_psm->m_pDirLight->SetLightParam(DirLightParams);
//
//	if (m_psm->m_pDPointLight)
//		m_psm->m_pDPointLight->TurnOff();
//
//	if (m_ColorOP != A3DTOP_MODULATE)
//		m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
//
//	//	Restore cull mode
//	m_pA3DDevice->SetFaceCull(m_CurCull);
//
//	if (bAlphaModel)
//	{
//		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
//		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
//		m_pA3DDevice->SetAlphaBlendEnable(false);
//	}
//
//	return true;
//}

//	Render skin item in specified skin model
//bool A3DSkinRender::RenderSkinAtOnce(A3DViewport* pViewport, A3DSkinModel* pModel, 
//							int iSkinItem, DWORD dwFlags)
//{
//	A3DEnvironment* pEnvironment = m_pA3DEngine->GetA3DEnvironment();
//	A3DSkinModel::SKIN* pSkinItem = pModel->GetSkinItem(iSkinItem);
//	ASSERT(pSkinItem);
//	if (!pSkinItem->bRender || !pSkinItem->pA3DSkin)
//		return true;
//
//	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);
//
//	bool bAlphaBlend = m_pA3DDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE) ? true : false;
//
//	int i, iNumMesh;
//	int iRenderMethod = m_pA3DDevice->GetSkinModelRenderMethod();
//	int iNumClothMesh = 0;
//	A3DClothMeshImp* aClothMeshes[128];
//
//	A3DSkin* pSkin = pSkinItem->pA3DSkin;
//	A3DTexture* pLastTexture = NULL;
//	DWORD dwTexture = (DWORD)(-1);
//	bool bEffect = false;
//
//	A3DSkinMeshRef Ref;
//	MESHNODE MeshNode;
//	MeshNode.pSkin		= pSkin;
//	MeshNode.pSkinModel	= pModel;
//	MeshNode.pMeshRef	= &Ref;
//
//	//	Render skin meshes
//	if (!(iNumMesh = pSkin->GetSkinMeshNum()))
//		goto RenderRigidMesh;
//
//	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
//	{
//		for (i=0; i < iNumMesh; i++)
//		{
//			Ref = pSkin->GetSkinMeshRef(i);
//
//			if (Ref.bInvisible)
//				continue;
//
//			//	Is a cloth mesh ?
//			if (Ref.pClothImp && pSkin->IsClothesEnable())
//			{
//				aClothMeshes[iNumClothMesh++] = Ref.pClothImp;
//				continue;
//			}
//
//			A3DSkinMesh* pMesh = (A3DSkinMesh*)Ref.pMesh;
//			A3DTexture * pTexture = pMesh->GetTexturePtr(pSkin);
//			if (pTexture)
//			{
//				if (pTexture->IsAlphaTexture() && (dwFlags & A3DSkinModel::RAO_NOALPHA))
//					continue;
//
//				//	Apply mesh texture if RAO_NOTEXTURE flag isn't set
//				if (!(dwFlags & A3DSkinModel::RAO_NOTEXTURE))
//				{
//					bEffect = pTexture->GetClassID() == A3D_CID_HLSLSHADER;
//					if(bEffect)
//					{
//						((A3DEffect*)pTexture)->Appear(pEnvironment->GetCommonConstTable());
//					}
//					else
//					{
//						if ((dwFlags & A3DSkinModel::RAO_NOPIXELSHADER) && pTexture->IsShaderTexture())
//						{
//							A3DTexture* pBaseTex = ((A3DShader*)pTexture)->GetBaseTexture();
//
//							if (pBaseTex)
//							{
//								pBaseTex->Appear(0);
//								pLastTexture = pBaseTex;
//							}
//						}
//						else
//							ApplyMeshTexture(&pLastTexture, dwTexture, &MeshNode, pEnvironment->GetCommonConstTable());
//					}
//				}
//			}
//
//			if (Ref.bSWRender)
//			{
//				//	Set material and transparent
//				//ApplyMeshMaterial(&MeshNode, dwFlags, false);
//				//	Render mesh
//				pMesh->RenderSoftware(pViewport, pModel, pSkin);
//			}
//			else
//			{
//				//	Set material and transparent
//				//ApplyMeshMaterial(&MeshNode, dwFlags, true);
//
//				A3DObject* pvs = MeshNode.pMeshRef->pvs;
//				if (!pvs)
//				{
//					if (!(pvs = m_psm->GetSkinMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight(), pMesh->IsEnableExtMaterial() && bEffect)))
//						continue;
//				}
//
//				const A3DSkinModel::LIGHTINFO& LightInfo = MeshNode.pSkinModel->GetLightInfo();
//				pEnvironment->SetLightInfo(LightInfo);
//				PVS_APPEAR;
//
//				//	Render mesh
//				pMesh->Render(pViewport, pModel, pSkin);
//			}
//		}
//	}
//	//else if (iRenderMethod == A3DDevice::SMRD_INDEXEDVERTMATRIX)
//	//{
//	//	//	Set render state
//	//	m_pA3DDevice->SetIndexedVertexBlendEnable(true);
//	//	m_pA3DDevice->SetVertexBlendFlag(A3DVBF_3WEIGHTS);
//
//	//	for (i=0; i < pSkin->GetSkinMeshNum(); i++)
//	//	{
//	//		Ref = pSkin->GetSkinMeshRef(i);
//
//	//		if (Ref.bInvisible)
//	//			continue;
//
//	//		//	Is a cloth mesh ?
//	//		if (Ref.pClothImp && pSkin->IsClothesEnable())
//	//		{
//	//			aClothMeshes[iNumClothMesh++] = Ref.pClothImp;
//	//			continue;
//	//		}
//
//	//		A3DTexture * pTexture = Ref.pMesh->GetTexturePtr(pSkin);
//	//		if(pTexture && pTexture->IsAlphaTexture() && (dwFlags & A3DSkinModel::RAO_NOALPHA))
//	//			continue;
//
//	//		//	Set material and transparent
//	//		//ApplyMeshMaterial(&MeshNode, dwFlags, false);
//
//	//		//	Apply mesh texture if RAO_NOTEXTURE flag isn't set
//	//		if (!(dwFlags & A3DSkinModel::RAO_NOTEXTURE))
//	//			ApplyMeshTexture(&pLastTexture, dwTexture, &MeshNode, pEnvironment->GetCommonConstTable());
//
//	//		//	Render mesh
//	//		Ref.pMesh->Render(pViewport, pModel, pSkin);
//	//	}
//
//	//	m_pA3DDevice->SetIndexedVertexBlendEnable(false);
//	//	m_pA3DDevice->SetVertexBlendFlag(A3DVBF_DISABLE);
//	//}
//	//else if (iRenderMethod == A3DDevice::SMRD_SOFTWARE)
//	//{
//	//	//	Set render state
//	//	m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);
//
//	//	for (i=0; i < pSkin->GetSkinMeshNum(); i++)
//	//	{
//	//		Ref = pSkin->GetSkinMeshRef(i);
//
//	//		if (Ref.bInvisible)
//	//			continue;
//
//	//		//	Is a cloth mesh ?
//	//		if (Ref.pClothImp && pSkin->IsClothesEnable())
//	//		{
//	//			aClothMeshes[iNumClothMesh++] = Ref.pClothImp;
//	//			continue;
//	//		}
//
//	//		A3DTexture * pTexture = Ref.pMesh->GetTexturePtr(pSkin);
//	//		if(pTexture && pTexture->IsAlphaTexture() && (dwFlags & A3DSkinModel::RAO_NOALPHA))
//	//			continue;
//
//	//		//	Set material and transparent
//	//		//ApplyMeshMaterial(&MeshNode, dwFlags, false);
//
//	//		//	Apply mesh texture if RAO_NOTEXTURE flag isn't set
//	//		if (!(dwFlags & A3DSkinModel::RAO_NOTEXTURE))
//	//			ApplyMeshTexture(&pLastTexture, dwTexture, &MeshNode, pEnvironment->GetCommonConstTable());
//
//	//		//	Render mesh
//	//		Ref.pMesh->Render(pViewport, pModel, pSkin);
//	//	}
//	//}
//
//RenderRigidMesh:
//
//	//	Render rigid meshes
//	if (!(iNumMesh = pSkin->GetRigidMeshNum()))
//		goto RenderMorphMesh;
//
//	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
//	{
//		for (i=0; i < iNumMesh; i++)
//		{
//			Ref = pSkin->GetRigidMeshRef(i);
//
//			if (Ref.bInvisible)
//				continue;
//
//			//	Is a cloth mesh ?
//			if (Ref.pClothImp && pSkin->IsClothesEnable())
//			{
//				aClothMeshes[iNumClothMesh++] = Ref.pClothImp;
//				continue;
//			}
//
//			A3DRigidMesh* pMesh = (A3DRigidMesh*)Ref.pMesh;
//			A3DTexture * pTexture = pMesh->GetTexturePtr(pSkin);
//			if (pTexture)
//			{
//				if (pTexture->IsAlphaTexture() && (dwFlags & A3DSkinModel::RAO_NOALPHA))
//					continue;
//
//				//	Set material and transparent
//				//ApplyMeshMaterial(&MeshNode, dwFlags, true);
//
//				//	Apply mesh texture if RAO_NOTEXTURE flag isn't set
//				if (!(dwFlags & A3DSkinModel::RAO_NOTEXTURE))
//				{
//					if ((dwFlags & A3DSkinModel::RAO_NOPIXELSHADER) && pTexture->IsShaderTexture())
//					{
//						A3DTexture* pBaseTex = ((A3DShader*)pTexture)->GetBaseTexture();
//
//						if (pBaseTex)
//						{
//							pBaseTex->Appear(0);
//							pLastTexture = pBaseTex;
//						}
//					}
//					else
//						ApplyMeshTexture(&pLastTexture, dwTexture, &MeshNode, pEnvironment->GetCommonConstTable());
//				}
//			}
//
//			A3DObject* pvs = MeshNode.pMeshRef->pvs;
//			if (!pvs)
//			{
//				if (!(pvs = m_psm->GetRigidMeshVS(pModel->IsSpecularEnable(), pModel->HasDynamicLight())))
//					continue;
//			}
//
//			const A3DSkinModel::LIGHTINFO& LightInfo = MeshNode.pSkinModel->GetLightInfo();
//			pEnvironment->SetLightInfo(LightInfo);
//			PVS_APPEAR;
//
//			//	Set blending matrices as vertex constants
//			pModel->ApplyVSRigidMeshMat(pViewport, pMesh->GetBoneIndex(), SMVSC_BLENDMAT0_VS2);
//
//			//	Render mesh
//			pMesh->Render(pViewport, pModel, pSkin);
//
//		}
//	}
//	//else
//	//{
//	//	for (i=0; i < iNumMesh; i++)
//	//	{
//	//		Ref = pSkin->GetRigidMeshRef(i);
//
//	//		if (Ref.bInvisible)
//	//			continue;
//
//	//		//	Is a cloth mesh ?
//	//		if (Ref.pClothImp && pSkin->IsClothesEnable())
//	//		{
//	//			aClothMeshes[iNumClothMesh++] = Ref.pClothImp;
//	//			continue;
//	//		}
//
//	//		A3DTexture * pTexture = Ref.pMesh->GetTexturePtr(pSkin);
//	//		if(pTexture && pTexture->IsAlphaTexture() && (dwFlags & A3DSkinModel::RAO_NOALPHA))
//	//			continue;
//
//	//		//	Set material and transparent
//	//		//ApplyMeshMaterial(&MeshNode, dwFlags, false);
//
//	//		//	Apply mesh texture if RAO_NOTEXTURE flag isn't set
//	//		if (!(dwFlags & A3DSkinModel::RAO_NOTEXTURE))
//	//			ApplyMeshTexture(&pLastTexture, dwTexture, &MeshNode, pEnvironment->GetCommonConstTable());
//	//		
//	//		//	Render mesh
//	//		Ref.pMesh->Render(pViewport, pModel, pSkin);
//	//	}
//	//}
//
//RenderMorphMesh:
//
//	//	TODO: morph mesh isn't used now, so we skip it. Maybe we will
//	//		handle it later
//
////	RenderSuppleMesh:
//	
//	//	TODO: supple mesh isn't used now, so we skip it. Maybe we will
//	//		handle it later
//
////	RenderMuscleMesh:
//	
//	//	TODO: muscle mesh isn't used now, so we skip it. Maybe we will
//	//		handle it later
//
//	//	RenderClothMeshes:
//	for (i=0; i < iNumClothMesh; i++)
//	{
//		A3DClothMeshImp* pMesh = aClothMeshes[i];
//
//		//	Apply model's light
//		m_psm->ApplyModelLightTo(pModel);
//		//	Set material
//		//ApplyMaterial(&MeshNode);
//
//		//	Apply mesh texture if RAO_NOTEXTURE flag isn't set
//		if (!(dwFlags & A3DSkinModel::RAO_NOTEXTURE))
//			ApplyMeshTexture(&pLastTexture, dwTexture, &MeshNode, pEnvironment->GetCommonConstTable());
//
//		//	Render mesh
//		pMesh->Render(pViewport, pModel, pSkin);
//	}
//
//	//	Last texture may be a shader, so disappear is necessary
//	if (pLastTexture)
//	{
//		pLastTexture->Disappear();
//		if( pLastTexture->IsShaderTexture() )
//		{
//			if (m_ColorOP != A3DTOP_MODULATE)
//				m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);
//		}
//	}
//
//	m_pA3DDevice->SetAlphaBlendEnable(bAlphaBlend);
//
//	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);
//
//	return true;
//}

//	Apply mesh texture
void A3DSkinRender::ApplyMeshTexture(A3DTexture** ppLastTex, DWORD& dwTexID, const MESHNODE* pNode, const A3DCOMMCONSTTABLE* pEnvCommConstTab)
{
	A3DTexture* pLastTex = *ppLastTex;
	A3DTexture* pNewTex = pNode->pMeshRef->pMesh->GetTexturePtr(pNode->pSkin);
	if (!pNewTex)
	{
		//	Last texture may be a shader, so disappear is necessary
		if (pLastTex)
		{
			pLastTex->Disappear();
			//if( pLastTex->IsShaderTexture() )
			//{
			//	if (m_ColorOP != A3DTOP_MODULATE)
			//		m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);	
			//}
		}

		m_pA3DDevice->ClearTexture(0);

		*ppLastTex = NULL;
		dwTexID = (DWORD)(-1);
	}
	else if (pNewTex->GetClassID() == A3D_CID_HLSLSHADER)
	{
		A3DEffect* pEffect = (A3DEffect*)pNewTex;
		if (dwTexID != pEffect->GetCoreID())	// ��ͬ����,����shader�ͳ���
		{
			pEffect->Appear(pEnvCommConstTab);
			*ppLastTex = pNewTex;
			dwTexID = pEffect->GetCoreID();
		}
		else	// ��ͬshaderֻ���³���
		{
			pEffect->Commit(pEnvCommConstTab);
		}
	}
	else if (dwTexID != pNewTex->GetTextureID())	// ���ڷ� A3DEffect ����������
	{
		//	Last texture may be a shader, so disappear is necessary
		if (pLastTex)
		{
			pLastTex->Disappear();
			//if( pLastTex->IsShaderTexture() )
			//{
			//	if (m_ColorOP != A3DTOP_MODULATE)
			//		m_pA3DDevice->SetTextureColorOP(0, m_ColorOP);	
			//}
		}
		pNewTex->Appear();
		////	Change texture
		//if(pNewTex->GetClassID() == A3D_CID_HLSLSHADER) {
		//	((A3DEffect*)pNewTex)->Appear(pEnvCommConstTab);
		//}

		*ppLastTex = pNewTex;
		dwTexID = pNewTex->GetTextureID();
	}
}

//	Apply mesh material
//void A3DSkinRender::ApplyMeshMaterial(MESHNODE* pNode, DWORD dwFlags, bool bVS)
//{
//	//	RAO_NOALPHA flag should be checked before material is set
//	if (dwFlags & A3DSkinModel::RAO_NOALPHA)
//		pNode->pMeshRef->fTrans = -1.0f;
//
//	//	Set material
//	if (dwFlags & A3DSkinModel::RAO_NOMATERIAL)
//	{
//		if (bVS)
//			ApplyDefVSMaterial(pNode->pMeshRef->fTrans);
//		else
//			ApplyDefMaterial(pNode->pMeshRef->fTrans);
//	}
//	else
//	{
//		if (bVS)
//			ApplyVSMaterial(pNode);
//		else
//			ApplyMaterial(pNode);
//	}
//}

bool A3DSkinRender::RenderRaw( A3DViewport* pViewport, A3DHLSL* pHLSLSkin, A3DHLSL* pHLSLRigid)
{
	if (!m_pA3DDevice)
		return true;

	int iRenderMethod = m_pA3DDevice->GetSkinModelRenderMethod();
	if(iRenderMethod != A3DDevice::SMRD_VERTEXSHADER)
		return false;

	//	Save current cull mode
	m_CurCull = m_pA3DDevice->GetFaceCull();

	m_pA3DDevice->SetFaceCull(A3DCULL_CCW);


	A3DCameraBase* pCamera = pViewport->GetCamera();
	A3DMATRIX4 mat = pCamera->GetProjectionTM() * pCamera->GetPostProjectTM();
	mat.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(SMVSC_PROJECT, &mat, 4);

	int i;


	//	Render all slots
	for (i=0; i < m_iRenderSlotCnt; i++)
	{
		RENDERSLOT* pSlot = m_aRenderSlots[i];
		
		const int c_nListCount = 4;
		MeshNodeList* aList[c_nListCount];
		aList[0] = &pSlot->SkinMeshList;
		aList[1] = &pSlot->RigidMeshList;
		aList[2] = &pSlot->SkinMeshDLList;
		aList[3] = &pSlot->RigidMeshDLList;
		//ʣ�µ�Ӧ�ö�û���õ�.
// 		aList[] = &pSlot->SuppleMeshList;
// 		aList[] = &pSlot->MorphMeshList;
// 		aList[] = &pSlot->MuscleMeshList;
// 		aList[] = &pSlot->ClothMeshList;
		for(int j = 0; j < c_nListCount; j++)
		{
			if (aList[j]->GetCount() == 0)
				continue;
			
			//����mesh���͵Ĳ�ͬ,���ò�ͬ��hlsl
 			if(j == 0 || j == 2)
 				pHLSLSkin->Appear(APPEAR_SETSHADERONLY);
 			else if(j == 1 || j == 3)
 				pHLSLRigid->Appear(APPEAR_SETSHADERONLY);
 			else
 				ASSERT(0);

			ALISTPOSITION pos = aList[j]->GetHeadPosition();
			MESHNODE* pMeshNode = aList[j]->GetAt(pos);
			A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);
			
			if(pMeshNode == NULL || pTexture == NULL)
				continue;

			A3DShader* pA3DShader = dynamic_cast<A3DShader*>(pTexture);
			A3DEffect* pA3DEffect = dynamic_cast<A3DEffect*>(pTexture);
			
			//��������  FIXME!! ע��, ����SkinMesh����Ⱦ�㷺ʹ�����Զ���Shader, ����GetBaseTextureȡ���������е�Alphaͨ����ֵ����һ������������AlphaTest.
			//					����ͷ��ȡ���ĸ�����alphaͨ����ȫ1, ����ȡ���ĸ������ʾ�˸߹�.					
			//					��ǰΪ����ȷ����ElementRender����ʱû��û�п���skinMesh��AlphaTest
			A3DVECTOR4 mask[5];
			mask[0] = A3DVECTOR4(1,0,0,0);
			mask[1] = A3DVECTOR4(0,1,0,0);
			mask[2] = A3DVECTOR4(0,0,1,0);
			mask[3] = A3DVECTOR4(0,0,0,1);
			mask[4] = A3DVECTOR4(1,1,1,1);
			if(pA3DShader)
			{
// 				if(pA3DShader->GetBaseTexture())
// 					pA3DShader->GetBaseTexture()->Appear(0);
// 				else
// 					m_pA3DDevice->GetD3DDevice()->SetTexture(0, NULL);
 				if(pA3DShader->GetTransparentMaskTexture())
 				{
 					pA3DShader->GetTransparentMaskTexture()->Appear(0);
 					int chan = pA3DShader->GetTransparentChannel();
 					m_pA3DDevice->SetPixelShaderConstants(0, &mask[chan].x, 1);
 				}
 				else
 				{
 					m_pA3DDevice->SetPixelShaderConstants(0, &mask[4].x, 1);
					m_pA3DDevice->ClearTexture(0);
 					//m_pA3DDevice->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, A3DCOLORRGB(0, 0, 0), 1.0f, 0);
 				}
			}
			else if(pA3DEffect)
			{
				//if(!pA3DEffect->GetD3DBaseTexture())
				//	continue;
				pA3DEffect->AppearTextureOnly(0);
				//m_pA3DDevice->GetD3DDevice()->SetTexture(0, pA3DEffect->GetD3DBaseTexture());
				m_pA3DDevice->SetPixelShaderConstants(0, &mask[3].x, 1);
			}
			else//��ʱӦ���Ѿ�����ͨ��������.
			{
				pTexture->Appear();
				m_pA3DDevice->SetPixelShaderConstants(0, &mask[3].x, 1);
			}


			while (pos)
			{
				pMeshNode = aList[j]->GetNext(pos);

				A3DMeshBase* pMesh = pMeshNode->pMeshRef->pMesh;
				A3DSkin* pSkin = pMeshNode->pSkin;
				A3DSkinModel* pModel = pMeshNode->pSkinModel;
				
				A3DSkinMesh* pSkinMesh = dynamic_cast<A3DSkinMesh*>(pMesh);
				A3DRigidMesh* pRigidMesh = dynamic_cast<A3DRigidMesh*>(pMesh);
				
				//�����SkinMesh
				if(pSkinMesh)
				{
					if(pSkinMesh->IsEnableExtMaterial())
						//m_pA3DDevice->SetDeviceVertexShader(SKIN_FVF_VERTEX_TANGENT);
						m_pSkinMeshDecl->Appear();
					else
						//m_pA3DDevice->SetDeviceVertexShader(SKIN_FVF_VERTEX);
						m_pSkinMeshDecl->Appear();
				}
				//�����RigidMesh
				else if(pRigidMesh)
				{
					m_pRigidMeshDecl->Appear();
					//m_pA3DDevice->SetFVF(A3DFVF_A3DVERTEX); //FIXME!!�޸�Ϊ����Declaration
					pModel->ApplyVSRigidMeshMat(pViewport, pRigidMesh->GetBoneIndex(), SMVSC_BLENDMAT0_VS2);
				}

				pMesh->Render(pViewport, pModel, pSkin);

			}
			
		}
		
	}
/*


		if (pSlot->RigidMeshList.GetCount())
			RenderRigidMeshes(pViewport, pSlot);

		if (pSlot->SuppleMeshList.GetCount())
			RenderSuppleMeshes(pViewport, pSlot);

		if (pSlot->MorphMeshList.GetCount())
		{
			RenderMorphMeshes_VS(pViewport, pSlot);

		}

		if (pSlot->MuscleMeshList.GetCount())
		{
			RenderMuscleMeshes_SW(pViewport, pSlot);
		}

		if (pSlot->ClothMeshList.GetCount())
			RenderClothMeshes(pViewport, pSlot);

	bool bRenderAlpha = false;
	if (bRenderAlpha && m_AlphaMeshList.GetCount())
		RenderAlphaMeshes(pViewport);
*/
	/*
	//��ȾAlpha
	{
		if (m_AlphaMeshList.GetCount() != 0)
		{
			ALISTPOSITION pos = m_AlphaMeshList.GetHeadPosition();
		
			while (pos)
			{
				MESHNODE* pMeshNode = m_AlphaMeshList.GetNext(pos);

				A3DSkinMesh* pMesh = (A3DSkinMesh*)pMeshNode->pMeshRef->pMesh;
				A3DSkin* pSkin = pMeshNode->pSkin;
				A3DSkinModel* pModel = pMeshNode->pSkinModel;

				A3DTexture* pTexture = pMeshNode->pMeshRef->pMesh->GetTexturePtr(pMeshNode->pSkin);

				A3DShader* pA3DShader = dynamic_cast<A3DShader*>(pTexture);
				A3DEffect* pA3DEffect = dynamic_cast<A3DEffect*>(pTexture);

				if(pA3DShader)
				{
					if(pA3DShader->GetD3DBaseTexture())
						pA3DShader->GetBaseTexture()->Appear(0);
					else
						continue;//m_pA3DDevice->GetD3DDevice()->SetTexture(0, NULL);
				}
				else if(pA3DEffect)
				{
					if(!pA3DEffect->GetD3DBaseTexture())
						continue;
					m_pA3DDevice->GetD3DDevice()->SetTexture(0, pA3DEffect->GetD3DBaseTexture());
				}
				else//��ʱӦ���Ѿ�����ͨ��������.
					pTexture->Appear();

				A3DObject* pvs = pMeshNode->pMeshRef->pvs;
				if(pvs && pvs->GetClassID() == A3D_CID_HLSL)
					m_pSkinMeshTanDecl->Appear();
				else
					m_pSkinMeshDecl->Appear();

				pMesh->Render(pViewport, pModel, pSkin);

			}
		}
	}
*/
	for (i=0; i <= MAXNUM_HWMORPHCHANNEL; i++)
		m_pA3DDevice->ClearStreamSource(i);

	m_pA3DDevice->SetFaceCull(m_CurCull);


	return true;
}


bool A3DSkinRender::RegisterRenderSkinModelModifier( A3DSkinModel* pSkinModel, A3DSkinModelRenderModifier* pModifier )
{
	if(!pSkinModel || !pModifier)
		return false;
	MODIFIERITEM item;
	item.pSkinModel = pSkinModel;
	item.pModifier = pModifier;
	m_aModiferList.Add(item);
	return true;
}

