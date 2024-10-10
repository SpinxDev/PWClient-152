/*
 * FILE: A3DTerrain2Render.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/7
 *
 * HISTORY: 
 * 
 *	2005.2: 使用 pixel shader 1.4 来渲染。每个 pass 可以渲染最多 4 层个地形 layers。
 *			这要求加载 mask 时，将 4 层 mask 合并到 1 个 32-bit 位图中，加上 4 张
 *			layer texture 和 1 张 lightmap. 每个渲染 pass 一共使用 6 张 textures.
 *			使用这种方法渲染，要求每一个 layer mask 的大小一致，这一点在做地形的时候
 *			要多加注意!!
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "A3DTerrain2Render.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DMacros.h"
#include "A3DDevice.h"
#include "A3DTerrain2.h"
#include "A3DTerrain2Blk.h"
#include "A3DTerrain2Env.h"
#include "A3DEngine.h"
#include "A3DShaderMan.h"
#include "A3DTexture.h"
#include "A3DPixelShader.h"
#include "A3DVertexShader.h"
#include "A3DStream.h"
#include "A3DVSDef.h"
#include "A3DViewport.h"
#include "A3DRenderTarget.h"
#include "A3DOrthoCamera.h"
#include "A3DHLSL.h"
#include "A3DShadowMap.h"
#include "AGPAPerfIntegration.h"

#define new A_DEBUG_NEW

using namespace A3D;

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define FVF_MERGEVERTEX	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2)

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

static int l_iTotalVertCnt	= 0;
static int l_iTotalIdxCnt	= 0;	

struct MERGEVERTEX
{
    float x, y, z;
	float nx, ny, nz;
	float tu1, tv1;
	float tu2, tv2;

	MERGEVERTEX() {}
	MERGEVERTEX(const A3DVECTOR3& vPos, const A3DVECTOR3 vNormal, float _tu1, float _tv1, float _tu2, float _tv2)
	{
		x = vPos.x;	y = vPos.y;	z = vPos.z;	nx = vNormal.x; ny = vNormal.y; nz = vNormal.z; tu1 = _tu1; tv1 = _tv1;	tu2 = _tu2; tv2 = _tv2;
	}
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

//	Sort mask area by distance
static int _MaskAreaCompare1(const void *arg1, const void *arg2)
{
	A3DTrn2RenderSlot* pSlot1 = *(A3DTrn2RenderSlot**)arg1;
	A3DTrn2RenderSlot* pSlot2 = *(A3DTrn2RenderSlot**)arg2;
	return pSlot1->GetSortDist() - pSlot2->GetSortDist();
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DTerrain2Render
//	
///////////////////////////////////////////////////////////////////////////

A3DTerrain2Render::A3DTerrain2Render()
{
	m_pTerrain			= NULL;
	m_pA3DDevice		= NULL;
	m_iVertFillCnt		= 0;
	m_iIdxFillCnt		= 0;
	m_bRebuildStream	= false;
	m_dwFogColor		= 0xff000000;
	m_pStreamMan		= NULL;
	m_pBlackTex			= NULL;

	m_matScale.Identity();
}

A3DTerrain2Render::~A3DTerrain2Render()
{
}

//	Initlaize object
bool A3DTerrain2Render::Init(A3DTerrain2* pTerrain)
{
	m_pTerrain		= pTerrain;
	m_pA3DDevice	= pTerrain->GetA3DDevice();
	m_pBlackTex		= A3DTexture::CreateColorTexture(m_pA3DDevice, 2, 2, A3DCOLORRGB(0, 0, 0));

	return true;
}

//	Release object
void A3DTerrain2Render::Release()
{
	int i;

	//	Release all render slots in m_aCurSlots
	for (i=0; i < m_aCurSlots.GetSize(); i++)
	{
		A3DTrn2RenderSlot* pSlot = m_aCurSlots[i];
		pSlot->ReleaseStreams();
		delete pSlot;
	}

	m_aCurSlots.RemoveAll();

	//	Release all render slots in m_aLastSlots
	for (i=0; i < m_aLastSlots.GetSize(); i++)
	{
		A3DTrn2RenderSlot* pSlot = m_aLastSlots[i];
		pSlot->ReleaseStreams();
		delete pSlot;
	}

	m_aLastSlots.RemoveAll();

	//	Release all render slots in m_aFreeSlots
	for (i=0; i < m_aFreeSlots.GetSize(); i++)
	{
		A3DTrn2RenderSlot* pSlot = m_aFreeSlots[i];
		delete pSlot;
	}

	m_aFreeSlots.RemoveAll();

	A3DRELEASE(m_pStreamMan);
	A3DRELEASE(m_pBlackTex);
}

//	After terrain data has been loaded, this function is called
bool A3DTerrain2Render::OnTerrainLoaded()
{
	//	Create stream manger
	m_pStreamMan = new A3DTrn2StreamMan;
	if (!m_pStreamMan || !m_pStreamMan->Init(this))
		return false;

	//	Number of block in a mask area
    int nBlockGrid = m_pTerrain->GetBlockGrid();

    // in case divide by zero
    if (nBlockGrid == 0)
        nBlockGrid = 1;
	int iMaskBlock = m_pTerrain->GetMaskGrid() / nBlockGrid;
	iMaskBlock *= iMaskBlock;
	l_iTotalVertCnt	= (m_pTerrain->GetBlockGrid() + 1) * (m_pTerrain->GetBlockGrid() + 1) * iMaskBlock;
	l_iTotalIdxCnt	= m_pTerrain->GetBlockGrid() * m_pTerrain->GetBlockGrid() * 6 * iMaskBlock;

	return true;
}

//	Get render mesh data
bool A3DTerrain2Render::GetRenderMesh(int iMesh, BYTE* pVertData, int iVertStride, 
							WORD* pIdxData, int* piVertNum, int* piIdxNum, DWORD dwBlockFlag)
{
	int i, j, iVertCnt=0, iIndexCnt=0;
	A3DTrn2RenderSlot* pSlot = m_aCurSlots[iMesh];
	APtrArray<A3DTerrain2Block::RENDERDATA*> aRenderData;

	for (i=0; i < A3DTrn2RenderSlot::MAX_BLOCK_NUM; i++)
	{
		A3DTrn2RenderSlot::BLOCK_ITEM& block_item = pSlot->m_aBlocks[i];
		if (!block_item.pBlock)
			continue;

		A3DTerrain2Block* pBlock = block_item.pBlock;

		if ((dwBlockFlag & pBlock->GetBlockFlags()) != 0)
			continue;

		pBlock->BuildRenderData();

		const A3DTerrain2Block::RENDERDATA* pRenderData = pBlock->GetRenderData();

		ASSERT(iVertCnt + pRenderData->iNumVert <= l_iTotalVertCnt);
		ASSERT(iIndexCnt + pRenderData->iNumIndex <= l_iTotalIdxCnt);

		iVertCnt	+= pRenderData->iNumVert;
		iIndexCnt	+= pRenderData->iNumIndex;

		aRenderData.Add((A3DTerrain2Block::RENDERDATA*)pRenderData);
	}

	iVertCnt	= 0;
	iIndexCnt	= 0;

	BYTE* pDstVert = pVertData;

	//	Fill vertex and index buffer
	for (i=0; i < aRenderData.GetSize(); i++)
	{
		const A3DTerrain2Block::RENDERDATA* pRenderData = aRenderData[i];

		if (m_pTerrain->UseLightmapTech())
		{
			A3DTRN2VERTEX1* pSrcVert = (A3DTRN2VERTEX1*)pRenderData->aVertices;

			for (j=0; j < pRenderData->iNumVert; j++)
			{
				A3DVECTOR3* pv = (A3DVECTOR3*)pDstVert;
				pv->x = pSrcVert->vPos[0];
				pv->y = pSrcVert->vPos[1];
				pv->z = pSrcVert->vPos[2];

				pDstVert += iVertStride;
				pSrcVert++;
			}
		}
		else
		{
			A3DTRN2VERTEX2* pSrcVert = (A3DTRN2VERTEX2*)pRenderData->aVertices;

			for (j=0; j < pRenderData->iNumVert; j++)
			{
				A3DVECTOR3* pv = (A3DVECTOR3*)pDstVert;
				pv->x = pSrcVert->vPos[0];
				pv->y = pSrcVert->vPos[1];
				pv->z = pSrcVert->vPos[2];

				pDstVert += iVertStride;
				pSrcVert++;
			}
		}
	
		for (j=0; j < 5; j++)
		{
			WORD* aSrcIndices = pRenderData->aIndices[j];
			for (int n=0; n < pRenderData->aIdxNum[j]; n++)
				pIdxData[iIndexCnt++] = aSrcIndices[n] + iVertCnt;
		}

		iVertCnt += pRenderData->iNumVert;
	}

	*piVertNum	= iVertCnt;
	*piIdxNum	= iIndexCnt;

	return true;
}

//	Register a terrain block which will be rendered.
bool A3DTerrain2Render::RegisterBlock(A3DTerrain2Block* pBlock)
{
	//	Arrange blocks base on mask area they belong to
	int iMaskArea = pBlock->GetMaskAreaIdx();

	for (int i=0; i < m_aCurSlots.GetSize(); i++)
	{
		A3DTrn2RenderSlot* pSlot = m_aCurSlots[i];
		if (iMaskArea == pSlot->m_iMaskIdx)
		{
			pSlot->RegisterBlock(pBlock, 0);
			return true;
		}
	}

	//	Allocate a new render slot
	A3DTrn2RenderSlot* pSlot = AllocRenderSlot(iMaskArea);
	if (pSlot)
	{
		pSlot->RegisterBlock(pBlock, 0);
		m_aCurSlots.Add(pSlot);
		return true;
	}

	return true;
}

//	Allocate a render slot
A3DTrn2RenderSlot* A3DTerrain2Render::AllocRenderSlot(int iMaskArea)
{
	A3DTrn2RenderSlot* pSlot = NULL;

	if (m_aFreeSlots.GetSize())
	{
		int iIndex = m_aFreeSlots.GetSize() - 1;
		pSlot = m_aFreeSlots[iIndex];
		m_aFreeSlots.RemoveAtQuickly(iIndex);
	}
	else
	{
		pSlot = new A3DTrn2RenderSlot(this);
	}

	pSlot->Reset(iMaskArea);
	return pSlot;
}

//	Free a render slot
void A3DTerrain2Render::FreeRenderSlot(A3DTrn2RenderSlot* pSlot)
{
	m_aFreeSlots.Add(pSlot);
}

//	Reset render information, this function is called every frame
void A3DTerrain2Render::ResetRenderInfo()
{
	if (!m_aCurSlots.GetSize())
		return;		//	Nothing needs resetting

	if (m_aLastSlots.GetSize())
	{
		//	m_aLastSlots.GetSize != 0 means terrain blocks were registed,
		//	but no render methods were called. In this case, just keep 
		//	content in m_aLastSlots
		for (int i=0; i < m_aCurSlots.GetSize(); i++)
			FreeRenderSlot(m_aCurSlots[i]);
	}
	else
	{
		//	Store current slots for later using
		for (int i=0; i < m_aCurSlots.GetSize(); i++)
		{
			A3DTrn2RenderSlot* pSlot = m_aCurSlots[i];
			pSlot->ClearBlockPtrs();
			m_aLastSlots.Add(pSlot);
		}
	}

	m_aCurSlots.RemoveAll(false);
}

//	Sync last render slots to current slots
void A3DTerrain2Render::SyncLastRenderSlots()
{
	int i, j;

	//	Before sync render slots, we must prepare blocks render info
	for (i=0; i < m_aCurSlots.GetSize(); i++)
	{
		A3DTrn2RenderSlot* pSlot = m_aCurSlots[i];
		pSlot->PrepareRenderInfo();
	}

	//	Force to rebuild all render resources ?
	if (m_bRebuildStream)
	{
		//	Release all streams
		for (i=0; i < m_aCurSlots.GetSize(); i++)
		{
			A3DTrn2RenderSlot* pSlot = m_aCurSlots[i];
			pSlot->ReleaseStreams();
		}

		for (i=0; i < m_aLastSlots.GetSize(); i++)
		{
			A3DTrn2RenderSlot* pSlot = m_aLastSlots[i];
			pSlot->ReleaseStreams();
			FreeRenderSlot(pSlot);
		}

		m_aLastSlots.RemoveAll(false);
	}
	else
	{
		if (m_aLastSlots.GetSize())
		{
			for (i=0; i < m_aLastSlots.GetSize(); i++)
			{
				A3DTrn2RenderSlot* pLastSlot = m_aLastSlots[i];

				for (j=0; j < m_aCurSlots.GetSize(); j++)
				{
					A3DTrn2RenderSlot* pCurSlot = m_aCurSlots[j];
					if (pLastSlot->m_iMaskIdx != pCurSlot->m_iMaskIdx)
						continue;

					//	Transfer streams
					pLastSlot->TransferStreams(pCurSlot);
					break;
				}

				//	Release all un-transferred streams
				pLastSlot->ReleaseStreams();
				FreeRenderSlot(pLastSlot);
			}

			m_aLastSlots.RemoveAll(false);
		}
	}

	//	Create streams for blocks that still don't have
	for (j=0; j < m_aCurSlots.GetSize(); j++)
	{
		A3DTrn2RenderSlot* pCurSlot = m_aCurSlots[j];
		pCurSlot->CreateStreams();
	}

	m_bRebuildStream = false;
}

//	Sort render slots by distance
void A3DTerrain2Render::SortRenderSlotsByDist(const A3DVECTOR3& vEyePos)
{
	int i, iNumSlot = m_aCurSlots.GetSize();

	//	Calculate distance at first
	for (i=0; i < iNumSlot; i++)
	{
		A3DTrn2RenderSlot* pSlot = m_aCurSlots[i];
		pSlot->CalcSortDist(vEyePos);
	}

	//	Sort slots
	qsort(m_aCurSlots.GetData(), m_aCurSlots.GetSize(), sizeof (A3DTrn2RenderSlot*), _MaskAreaCompare1);
}

bool A3DTerrain2Render::RenderWithShadowOnePass(A3DViewport* pViewport)
{
	using namespace A3D;

	if (!m_aCurSlots.GetSize())
		return true;	//	Nothing needs rendering

	//	Sync slots to accelerate stream constructing speed
	SyncLastRenderSlots();

	//	Set render states
	m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_LINEAR);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	//	Set world transform matrix
	m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

	//	Apply material
	m_pTerrain->GetTerrainMaterial().Appear();

	for (int i=0; i < m_aCurSlots.GetSize(); i++)
		RenderSlot_ShadowMapL8(pViewport, m_aCurSlots[i]);

	//	Restore render states
	m_pA3DDevice->ClearPixelShader();
	m_pA3DDevice->ClearVertexShader();
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(2, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(3, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(4, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(5, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(2, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(3, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(4, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(5, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureCoordIndex(0, 0);
	m_pA3DDevice->SetTextureCoordIndex(1, 1);
	m_pA3DDevice->SetTextureCoordIndex(2, 2);
	m_pA3DDevice->SetTextureCoordIndex(3, 3);
	m_pA3DDevice->SetTextureCoordIndex(4, 4);
	m_pA3DDevice->SetTextureCoordIndex(5, 5);
	m_pA3DDevice->ClearTexture(1);
	m_pA3DDevice->ClearTexture(2);
	m_pA3DDevice->ClearTexture(3);
	m_pA3DDevice->ClearTexture(4);
	m_pA3DDevice->ClearTexture(5);

	return true;
}

//	Render routine
bool A3DTerrain2Render::Render(A3DViewport* pViewport, bool bShadow, A3DRenderTarget* pTerrainColorRT, 
							   float fZBias, float fShadowLum, bool bHDRLight, bool bOnePassHDRLM, bool bShadowMap)
{
	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__));
	ASSERT(m_pA3DDevice);

	if (A3D::g_pA3DTerrain2Env->GetSupportPSFlag())
	{
		if (m_pTerrain->UseLightmapTech())
		{
			if( bOnePassHDRLM)
			{
				Render_PS14_LM(pViewport, false, false, true);
			}
			else if( bHDRLight)
			{
				if(pTerrainColorRT != NULL)
				{	
					//render terrain to color rt
					pTerrainColorRT->ApplyToDevice();
					//set viewport
					pViewport->Active();					
					m_pA3DDevice->Clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, A3DCOLORRGBA(255,255,255,255), 1.0f, 0);
					Render_PS14_LM(pViewport, false, true);
					pTerrainColorRT->WithdrawFromDevice();

					//set render state
					//m_pA3DDevice->SetZFunc(A3DCMP_EQUAL);
					//m_pA3DDevice->SetZWriteEnable(false);

					RenderWithLightMapHDR(pViewport, pTerrainColorRT);
					//m_pA3DDevice->SetZFunc(A3DCMP_LESSEQUAL);
					//m_pA3DDevice->SetZWriteEnable(true);
				}
			}
			else if (m_pTerrain->IsTexMergeUsed())
			{
				Render_TM_LM(pViewport);
			}	
			else if( bShadow)
			{
				if(pTerrainColorRT != NULL)
				{	
					//render terrain to color rt
					pTerrainColorRT->ApplyToDevice();
					//set viewport
					pViewport->Active();					
					m_pA3DDevice->Clear(D3DCLEAR_TARGET, A3DCOLORRGBA(255,255,255,255), 1.0f, 0);
					Render_PS14_LM(pViewport, true);
					pTerrainColorRT->WithdrawFromDevice();

					RenderWithShadow(pViewport, pTerrainColorRT, fZBias, fShadowLum);
				}
			}
			else
			{
				if (m_pA3DDevice->TestPixelShaderVersion(2, 0) && !m_pTerrain->IsRenderForRefract() /*&& !(GetAsyncKeyState(VK_F3) & 0x8000)*/)
					Render_PS20_LM(pViewport, bShadowMap);
				else
					Render_PS14_LM(pViewport);
			}
		}
		else
			Render_PS14_VL(pViewport);
	}
	else
		Render_NoPS_VL(pViewport);

	return true;
}

bool A3DTerrain2Render::ZPrePass(A3DViewport* pViewport)
{
	ASSERT(m_pA3DDevice);

	//if (A3D::g_pA3DTerrain2Env->GetSupportPSFlag())
	//{
	//	if (m_pTerrain->UseLightmapTech())
	//	{
	//		if( bHDRLight)
	//		{
	//			if(pTerrainColorRT != NULL)
	//			{	
	//				//render terrain to color rt
	//				m_pA3DDevice->SetRenderTarget( pTerrainColorRT);
	//				//set viewport
	//				pViewport->Active();					
	//				m_pA3DDevice->Clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
	//				Render_PS14_LM(pViewport, false, true);
	//				m_pA3DDevice->RestoreRenderTarget();

	//				//set render state
	//				//m_pA3DDevice->SetZFunc(A3DCMP_EQUAL);
	//				//m_pA3DDevice->SetZWriteEnable(false);

	//				RenderWithLightMapHDR(pViewport, pTerrainColorRT);
	//				//m_pA3DDevice->SetZFunc(A3DCMP_LESSEQUAL);
	//				//m_pA3DDevice->SetZWriteEnable(true);
	//			}

	//		}
	//		else if (m_pTerrain->IsTexMergeUsed())
	//		{
	//			Render_TM_LM(pViewport);
	//		}	
	//		else if( bShadow)
	//		{
	//			if(pTerrainColorRT != NULL)
	//			{	
	//				//render terrain to color rt
	//				m_pA3DDevice->SetRenderTarget( pTerrainColorRT);
	//				//set viewport
	//				pViewport->Active();					
	//				m_pA3DDevice->Clear(D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0);
	//				Render_PS14_LM(pViewport, true);
	//				m_pA3DDevice->RestoreRenderTarget();

	//				RenderWithShadow(pViewport, pTerrainColorRT, fZBias, fShadowLum);
	//			}
	//		}
	//		else
	//		{
	//			Render_PS14_LM(pViewport);
	//		}
	//	}
	//	else
	//		Render_PS14_VL(pViewport);
	//}
	//else
	//	Render_NoPS_VL(pViewport);
	ZPrePass_NoPS_VL(pViewport);

	return true;
}

//	Render routine using ps 1.4 and lightmap
bool A3DTerrain2Render::Render_PS14_LM(A3DViewport* pViewport, bool bShadow, bool bHDR,  bool bOnePassHDRLM )
{
	using namespace A3D;

	if (!m_aCurSlots.GetSize())
		return true;	//	Nothing needs rendering

	//	Sync slots to accelerate stream constructing speed
	SyncLastRenderSlots();

	//	Set render states
	m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_LINEAR);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	//	Set world transform matrix
	m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

	//	Apply material
	m_pTerrain->GetTerrainMaterial().Appear();

	for (int i=0; i < m_aCurSlots.GetSize(); i++)
		RenderSlot_PS14_LM(pViewport, m_aCurSlots[i], bShadow, bHDR, bOnePassHDRLM);

	//	Restore render states
	m_pA3DDevice->ClearPixelShader();
	m_pA3DDevice->ClearVertexShader();
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(2, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(3, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(4, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(5, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(2, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(3, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(4, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(5, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureCoordIndex(0, 0);
	m_pA3DDevice->SetTextureCoordIndex(1, 1);
	m_pA3DDevice->SetTextureCoordIndex(2, 2);
	m_pA3DDevice->SetTextureCoordIndex(3, 3);
	m_pA3DDevice->SetTextureCoordIndex(4, 4);
	m_pA3DDevice->SetTextureCoordIndex(5, 5);
	m_pA3DDevice->ClearTexture(1);
	m_pA3DDevice->ClearTexture(2);
	m_pA3DDevice->ClearTexture(3);
	m_pA3DDevice->ClearTexture(4);
	m_pA3DDevice->ClearTexture(5);

	return true;
}

bool A3DTerrain2Render::RenderSlot_ShadowMapL8(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot)
{
	using namespace A3D;

	A3DTerrain2Mask* pMask = m_pTerrain->GetMaskArea(pSlot->m_iMaskIdx);

	if (!pMask)
		return true;

	//	Apply pixel shader
	g_pA3DTerrain2Env->GetShadowL8PS()->Appear();

	D3DXPLANE cp;

	if (m_pTerrain->IsRenderForRefract())
	{
		if (g_pA3DTerrain2Env->GetRefractVertexShader())
			g_pA3DTerrain2Env->GetRefractVertexShader()->Appear();
	}
	else
	{
		g_pA3DTerrain2Env->GetShadowL8VS()->Appear();
	}

	//	Close fog
	A3DCOLOR dwFogCol = m_pA3DDevice->GetFogColor();
	m_dwFogColor = dwFogCol;
	m_pA3DDevice->SetFogColor(0xff000000);

	int iStartLayer = 0;
	int iLayerCnt	= 0;
	int iMaxWeight	= m_pTerrain->GetMaxLayerWeight();

	for (int iLayer=0; iLayer < pMask->GetLayerNum(); iLayer++)
	{
		//	Check layer weight. 
		if (iLayer && (int)pMask->GetLayerWeight(iLayer) > iMaxWeight)
		{
			//	In current version, we assume layer weight is in increasing
			//	order, so we can break now.
			break;
		}

		if (++iLayerCnt >= 3)
		{
			if (iLayer == pMask->GetLayerNum()-1 ||	(int)pMask->GetLayerWeight(iLayer+1) > iMaxWeight)
			{
				//	Open fog if this is the last pass
				m_pA3DDevice->SetFogColor(dwFogCol);
			}

			//	Render primitives
			PrepareRenderShadowL8PS(pMask, iStartLayer, iLayerCnt);
			PrepareVertexConstantsL8(pViewport);

			pSlot->DrawAllBlocks(false);

			iStartLayer += iLayerCnt;
			iLayerCnt = 0;
		}
	}

	if (iLayerCnt)
	{
		//	Open fog because this is the last pass
		m_pA3DDevice->SetFogColor(dwFogCol);
		
		//	Render primitives
		PrepareRenderShadowL8PS(pMask, iStartLayer, iLayerCnt);
		PrepareVertexConstantsL8(pViewport);

		pSlot->DrawAllBlocks(false);
	}

	if (m_pTerrain->IsRenderForRefract())
	{
		if (g_pA3DTerrain2Env->GetRefractVertexShader())
			g_pA3DTerrain2Env->GetRefractVertexShader()->Disappear();
	}
	else
	{
		g_pA3DTerrain2Env->GetShadowL8VS()->Disappear();
	}

	return true;
}

//	Render slot using depth first method. pixel shader and lightmap version
bool A3DTerrain2Render::RenderSlot_PS14_LM(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot, bool bShadow, bool bHDR,  bool bOnePassHDRLM)
{
	using namespace A3D;

	A3DTerrain2Mask* pMask = m_pTerrain->GetMaskArea(pSlot->m_iMaskIdx);
	if (!pMask)
		return true;

	//	Apply pixel shader
	A3DPixelShader* p4LPS = NULL;

	if( bShadow) //dynamic shadow
	{
		p4LPS = g_pA3DTerrain2Env->GetPS3LayersLM();
	}
	else if( bOnePassHDRLM)
	{
		p4LPS = g_pA3DTerrain2Env->GetGetPS3LayersOnePassHDRLM();
	}
	else if( bHDR)
	{
		p4LPS = g_pA3DTerrain2Env->GetPS3LayersNoLM();
	}
	else // light map
	{
		if( g_pA3DTerrain2Env->IsLightMap32Bit()) // 32bit light map, alpha channel is shadow
		{
			p4LPS = g_pA3DTerrain2Env->GetPS3LayersLMAlphaShadow();
		}
		else // 24bit light map
		{
			p4LPS = g_pA3DTerrain2Env->GetPS3LayersLM();
		}

	}

	if(!p4LPS)
		return false;

	p4LPS->Appear();

	D3DXPLANE cp;
	if (m_pTerrain->IsRenderForRefract())
	{
		if (g_pA3DTerrain2Env->GetRefractVertexShader())
			g_pA3DTerrain2Env->GetRefractVertexShader()->Appear();
	}
	else
	{
		if ((!m_pA3DDevice->IsHardwareTL() || bShadow||bHDR) && g_pA3DTerrain2Env->GetVertexShader())
		{
			g_pA3DTerrain2Env->GetVertexShader()->Appear();
		}
	}

	//	Close fog
	bool bFog = m_pA3DDevice->GetFogEnable();

	A3DCOLOR dwFogCol = m_pA3DDevice->GetFogColor();
	m_dwFogColor = dwFogCol;
	if( bShadow || bHDR)
	{
		m_pA3DDevice->SetFogEnable(false);
	}
	else
	{
		m_pA3DDevice->SetFogColor(0xff000000);	
	}

	int iStartLayer = 0;
	int iLayerCnt	= 0;
	int iMaxWeight	= m_pTerrain->GetMaxLayerWeight();
	bool bApplyFVF = (bShadow||bHDR || m_pTerrain->IsRenderForRefract()) ? false : true;

	for (int iLayer=0; iLayer < pMask->GetLayerNum(); iLayer++)
	{
		//	Check layer weight. 
		if (iLayer && (int)pMask->GetLayerWeight(iLayer) > iMaxWeight)
		{
			//	In current version, we assume layer weight is in increasing
			//	order, so we can break now.
			break;
		}

		if (++iLayerCnt >= 3)
		{
			if (iLayer == pMask->GetLayerNum()-1 ||	(int)pMask->GetLayerWeight(iLayer+1) > iMaxWeight)
			{
				//	Open fog if this is the last pass
				m_pA3DDevice->SetFogColor(dwFogCol);
				
			}

			//	Render primitives
			PrepareRender3Layers_PS14_LM(pMask, iStartLayer, iLayerCnt);
			PrepareVertexConstants(pViewport);

			//	Render
			pSlot->DrawAllBlocks(bApplyFVF);

			iStartLayer += iLayerCnt;
			iLayerCnt = 0;
		}
	}

	if (iLayerCnt)
	{
		//	Open fog because this is the last pass
		m_pA3DDevice->SetFogColor(dwFogCol);
		
		//	Render primitives
		PrepareRender3Layers_PS14_LM(pMask, iStartLayer, iLayerCnt);
		PrepareVertexConstants(pViewport);

		//	Render
		pSlot->DrawAllBlocks(bApplyFVF);
	}

	if (m_pTerrain->IsRenderForRefract())
	{
		if (g_pA3DTerrain2Env->GetRefractVertexShader())
			g_pA3DTerrain2Env->GetRefractVertexShader()->Disappear();
	}
	else
	{
		if ((!m_pA3DDevice->IsHardwareTL() || bShadow) && g_pA3DTerrain2Env->GetVertexShader())
		{
			g_pA3DTerrain2Env->GetVertexShader()->Disappear();
		}
	}

	if( bShadow || bHDR)
		m_pA3DDevice->SetFogEnable(bFog);

	return true;
}

bool A3DTerrain2Render::RenderTerrainShadowMapL8(const A3DVECTOR3& vShadowCenter, float fShadowRealRadius, const A3DVECTOR3& vSunLightDir, float fZBias, float fShadowLum, bool bUpdateTotal)
{
	return true;
}

bool A3DTerrain2Render::RenderShadowMap(A3DViewport* pViewport, const A3DVECTOR3* pShadowCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRadius, const float fShadowRealRadius)
{	

	A3DShadowMap* pShadowMap = m_pA3DDevice->GetA3DEngine()->GetA3DShadowMap();
	if(!pShadowMap)
		return false;

	A3DVertexShader* pShadowMapVS = g_pA3DTerrain2Env->GetShadowMapVS();
	A3DPixelShader* pShadowMapPS = g_pA3DTerrain2Env->GetShadowMapPS();
	A3DTexture* pLookupMap = g_pA3DTerrain2Env->GetLookupMap();
	if( pShadowMapVS== NULL || pShadowMapPS == NULL || pLookupMap == NULL)
		return false;

	if (!m_aCurSlots.GetSize())
		return true;	//	Nothing needs rendering

	//	Sync slots to accelerate stream constructing speed
	SyncLastRenderSlots();

	//Set render target
	pShadowMap->Set(pShadowCenter, pSunLightDir, fShadowRadius,fShadowRealRadius);

	pShadowMapVS->Appear();
	pShadowMapPS->Appear();

	//set render state
	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
	bool bAlphaTest = m_pA3DDevice->GetAlphaTestEnable();
	m_pA3DDevice->SetAlphaTestEnable(false);
	m_pA3DDevice->SetLightingEnable(false);
	bool bFog = m_pA3DDevice->GetFogEnable();
	m_pA3DDevice->SetFogEnable(false);

	//set tex
	pLookupMap->Appear(0);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);

	//set vertex const
	A3DMATRIX4 matShadowMVP = pShadowMap->GetShadowCamera()->GetVPTM();
	matShadowMVP.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(0, (void*)&matShadowMVP, 4);
	
	//set pixel const
	A3DVECTOR4 c0(0.0f);
	m_pA3DDevice->SetPixelShaderConstants(0, (void*)&c0, 1);

	//draw 
	for (int i=0; i < m_aCurSlots.GetSize(); i++)
		RenderShadowSlot(pViewport, m_aCurSlots[i]);

	//Restore render target
	pShadowMap->Restore();

	//Restore vs and ps
	pShadowMapVS->Disappear();
	pShadowMapPS->Disappear();

	//restore render state
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	m_pA3DDevice->SetAlphaTestEnable(bAlphaTest);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetFogEnable(bFog);

	//restore tex
	pLookupMap->Disappear(0);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);

	return true;
}

bool A3DTerrain2Render::RenderWithShadow(A3DViewport* pViewport, A3DRenderTarget* pTerrainColorRT,float fZBias, float fShadowLum)
{
	using namespace A3D;

	if( pTerrainColorRT == NULL)
		return false;

	if (!m_aCurSlots.GetSize())
		return true;	

	return true;

}
bool A3DTerrain2Render::RenderShadowSlot(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot)
{
	A3DTerrain2Mask* pMask = m_pTerrain->GetMaskArea(pSlot->m_iMaskIdx);
	if (!pMask)
		return true;
	
	pSlot->DrawAllBlocks(false);

	return true;
}

void A3DTerrain2Render::SetShadowVSConst(A3DMATRIX4* pMatMVP, A3DMATRIX4* pMatShadowMVP,int nShadowMapSize, float fZBias)
{
	A3DDEVFMT fmt = m_pA3DDevice->GetDevFormat();
	
	A3DMATRIX4 matScale(A3DMATRIX4::IDENTITY);
	matScale.m[0][0] = 0.5f;
	matScale.m[1][1] = -0.5f;
	matScale.m[3][0] = 0.5f + 0.5f/fmt.nWidth;
	matScale.m[3][1] = 0.5f + 0.5f/fmt.nHeight;
	
	A3DMATRIX4 matRT = (*pMatMVP) * matScale;
	matRT.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(24, (void*)&matRT, 4);

	pMatMVP->Transpose();
	m_pA3DDevice->SetVertexShaderConstants(0, (void*)pMatMVP, 4);
		
	// Texture adjustment matrix
	A3DMATRIX4 matTexAdj(A3DMATRIX4::IDENTITY);
	matTexAdj._11 = 0.5f;
	matTexAdj._22 = -0.5f;
	matTexAdj._33 = 0.0f;
	matTexAdj._43 = 1.0f;
	matTexAdj._44 = 1.0f;
	
	// Jitter 4 samples
	
	//tex1
	FLOAT fC1 = 0.1f / nShadowMapSize;
	FLOAT fC2 = 0.9f / nShadowMapSize;
	matTexAdj._41 = 0.5f + fC1;
	matTexAdj._42 = 0.5f + fC1;
	
	A3DMATRIX4 matShadowTex1 = (*pMatShadowMVP)* matTexAdj;
	matShadowTex1.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(8, (void*)&matShadowTex1, 4);
	
	//tex2
	matTexAdj._41 = 0.5f + fC1;
	matTexAdj._42 = 0.5f + fC2;
	A3DMATRIX4 matShadowTex2 = (*pMatShadowMVP) * matTexAdj;
	matShadowTex2.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(12, (void*)&matShadowTex2, 4);
	
	//tex3
	matTexAdj._41 = 0.5f + fC2;
	matTexAdj._42 = 0.5f + fC1;
	A3DMATRIX4 matShadowTex3 = (*pMatShadowMVP) * matTexAdj;
	matShadowTex3.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(16, (void*)&matShadowTex3, 4);
	
	//tex4
	matTexAdj._41 = 0.5f + fC2;
	matTexAdj._42 = 0.5f + fC2;
	A3DMATRIX4 matShadowTex4 = (*pMatShadowMVP) * matTexAdj;
	matShadowTex4.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(20, (void*)&matShadowTex4, 4);

	//terrain color tex

	
	//shadow map mvp
	pMatShadowMVP->Transpose();
	m_pA3DDevice->SetVertexShaderConstants(4, (void*)pMatShadowMVP, 4);
	
	//z-bais
	A3DVECTOR4 vZBais(fZBias, 0.0f, 0.0f, 0.0f);
	m_pA3DDevice->SetVertexShaderConstants(32, (void*)&vZBais, 1);


}

void A3DTerrain2Render::SetShadowMapL8VSConst(A3DMATRIX4* pMatMVP, A3DMATRIX4* pMatShadowMVP, int nShadowMapSize, float fZBias)
{
	pMatMVP->Transpose();
	m_pA3DDevice->SetVertexShaderConstants(0, (void*)pMatMVP, 4);

	// Texture adjustment matrix
	A3DMATRIX4 matTexAdj(A3DMATRIX4::IDENTITY);
	matTexAdj._11 = 0.5f;
	matTexAdj._22 = -0.5f;
	matTexAdj._33 = 0.0f;
	matTexAdj._43 = 1.0f;
	matTexAdj._44 = 1.0f;

	// Jitter 4 samples
	
	//tex1
	FLOAT fC1 = 0.1f / nShadowMapSize;
	FLOAT fC2 = 0.9f / nShadowMapSize;
	matTexAdj._41 = 0.5f + fC1;
	matTexAdj._42 = 0.5f + fC1;
	
	A3DMATRIX4 matShadowTex1 = (*pMatShadowMVP)* matTexAdj;
	matShadowTex1.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(8, (void*)&matShadowTex1, 4);
	
	//tex2
	matTexAdj._41 = 0.5f + fC1;
	matTexAdj._42 = 0.5f + fC2;
	A3DMATRIX4 matShadowTex2 = (*pMatShadowMVP) * matTexAdj;
	matShadowTex2.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(12, (void*)&matShadowTex2, 4);
	
	//tex3
	matTexAdj._41 = 0.5f + fC2;
	matTexAdj._42 = 0.5f + fC1;
	A3DMATRIX4 matShadowTex3 = (*pMatShadowMVP) * matTexAdj;
	matShadowTex3.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(16, (void*)&matShadowTex3, 4);
	
	//tex4
	matTexAdj._41 = 0.5f + fC2;
	matTexAdj._42 = 0.5f + fC2;
	A3DMATRIX4 matShadowTex4 = (*pMatShadowMVP) * matTexAdj;
	matShadowTex4.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(20, (void*)&matShadowTex4, 4);

	//shadow map mvp
	pMatShadowMVP->Transpose();
	m_pA3DDevice->SetVertexShaderConstants(4, (void*)pMatShadowMVP, 4);
	
	//z-bais
	A3DVECTOR4 vZBais(fZBias, 0.0f, 0.0f, 0.0f);
	m_pA3DDevice->SetVertexShaderConstants(32, (void*)&vZBais, 1);
}

//	Render routine using ps 1.4 and vertex-light
bool A3DTerrain2Render::Render_PS14_VL(A3DViewport* pViewport)
{
	if (!m_aCurSlots.GetSize())
		return true;	//	Nothing needs rendering

	//	Sync slots to accelerate stream constructing speed
	SyncLastRenderSlots();

	//	Set render states
	m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_LINEAR);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSpecularEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	//	Set world transform matrix
	m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

	//	Apply material
	m_pTerrain->GetTerrainMaterial().Appear();

	for (int i=0; i < m_aCurSlots.GetSize(); i++)
		RenderSlot_PS14_VL(pViewport, m_aCurSlots[i]);

	//	Restore render states
	m_pA3DDevice->ClearPixelShader();
	m_pA3DDevice->ClearVertexShader();
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(2, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(3, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(4, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(5, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(2, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(3, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(4, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(5, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureCoordIndex(0, 0);
	m_pA3DDevice->SetTextureCoordIndex(1, 1);
	m_pA3DDevice->SetTextureCoordIndex(2, 2);
	m_pA3DDevice->SetTextureCoordIndex(3, 3);
	m_pA3DDevice->SetTextureCoordIndex(4, 4);
	m_pA3DDevice->SetTextureCoordIndex(5, 5);
	m_pA3DDevice->ClearTexture(1);
	m_pA3DDevice->ClearTexture(2);
	m_pA3DDevice->ClearTexture(3);
	m_pA3DDevice->ClearTexture(4);
	m_pA3DDevice->ClearTexture(5);

	return true;
}

//	Render slot using pixel shader and vertex-light
bool A3DTerrain2Render::RenderSlot_PS14_VL(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot)
{
	using namespace A3D;

	A3DTerrain2Mask* pMask = m_pTerrain->GetMaskArea(pSlot->m_iMaskIdx);
	if (!pMask)
		return true;

	//	Apply pixel shader
	A3DPixelShader* p4LPS = g_pA3DTerrain2Env->GetPS3LayersVL();
	p4LPS->Appear();

	//	Close fog
	A3DCOLOR dwFogCol = m_pA3DDevice->GetFogColor();
	m_pA3DDevice->SetFogColor(0xff000000);

	int iStartLayer = 0;
	int iLayerCnt	= 0;
	int iMaxWeight	= m_pTerrain->GetMaxLayerWeight();

	for (int iLayer=0; iLayer < pMask->GetLayerNum(); iLayer++)
	{
		//	Check layer weight. 
		if (iLayer && (int)pMask->GetLayerWeight(iLayer) > iMaxWeight)
		{
			//	In current version, we assume layer weight is in increasing
			//	order, so we can break now.
			break;
		}

		if (++iLayerCnt >= 3)
		{
			if (iLayer == pMask->GetLayerNum()-1 ||	(int)pMask->GetLayerWeight(iLayer+1) > iMaxWeight)
			{
				//	Open fog if this is the last pass
				m_pA3DDevice->SetFogColor(dwFogCol);
			}

			//	Render primitives
			PrepareRender3Layers_PS14_VL(pMask, iStartLayer, iLayerCnt);
			PrepareVertexConstants(pViewport);

			pSlot->DrawAllBlocks(true);

			iStartLayer += iLayerCnt;
			iLayerCnt = 0;
		}
	}

	if (iLayerCnt)
	{
		//	Open fog because this is the last pass
		m_pA3DDevice->SetFogColor(dwFogCol);

		//	Render primitives
		PrepareRender3Layers_PS14_VL(pMask, iStartLayer, iLayerCnt);
		PrepareVertexConstants(pViewport);

		pSlot->DrawAllBlocks(true);
	}

	return true;
}

//	Render routine without ps and lightmap
bool A3DTerrain2Render::Render_NoPS_VL(A3DViewport* pViewport)
{
	using namespace A3D;

	if (!m_aCurSlots.GetSize())
		return true;	//	Nothing needs rendering

	//	Sync slots to accelerate stream constructing speed
	SyncLastRenderSlots();

	//	Set render states
	m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSpecularEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X); // v0 * texture
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_SELECTARG2); // select v0.a which is 1.0 here
	m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_DIFFUSE);
	m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_DIFFUSE);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_SELECTARG2); // select color from previous stage's result
	m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_SELECTARG1); // select mask from mask texture's alpha channel
	m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);

	//	Set world transform matrix
	m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

	//	Apply material
	m_pTerrain->GetTerrainMaterial().Appear();

	for (int i=0; i < m_aCurSlots.GetSize(); i++)
		RenderSlot_NoPS_VL(pViewport, m_aCurSlots[i]);

	//	Restore render states
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureCoordIndex(0, 0);
	m_pA3DDevice->SetTextureCoordIndex(1, 1);
	m_pA3DDevice->ClearTexture(1);

	return true;
}
bool A3DTerrain2Render::ZPrePass_NoPS_VL(A3DViewport* pViewport)
{
	using namespace A3D;

	if (!m_aCurSlots.GetSize())
		return true;	//	Nothing needs rendering

	//	Sync slots to accelerate stream constructing speed
	SyncLastRenderSlots();

	////	Set render states
	//m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	//m_pA3DDevice->SetLightingEnable(false);
	//m_pA3DDevice->SetSpecularEnable(false);
	//m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	//m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	//m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X); // v0 * texture
	//m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_SELECTARG2); // select v0.a which is 1.0 here
	//m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_DIFFUSE);
	//m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_DIFFUSE);
	//m_pA3DDevice->SetTextureColorOP(1, A3DTOP_SELECTARG2); // select color from previous stage's result
	//m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_SELECTARG1); // select mask from mask texture's alpha channel
	//m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	//m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);

	//	Set world transform matrix
	m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

	//	Apply material
	//m_pTerrain->GetTerrainMaterial().Appear();

	for (int i=0; i < m_aCurSlots.GetSize(); i++)
		ZPrePassSlot_NoPS_VL(pViewport, m_aCurSlots[i]);

	////	Restore render states
	//m_pA3DDevice->SetZWriteEnable(true);
	//m_pA3DDevice->SetLightingEnable(true);
	//m_pA3DDevice->SetSpecularEnable(true);
	//m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	//m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	//m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	//m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	//m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	//m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	//m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	//m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
	//m_pA3DDevice->SetTextureCoordIndex(0, 0);
	//m_pA3DDevice->SetTextureCoordIndex(1, 1);
	//m_pA3DDevice->ClearTexture(1);

	return true;
}
//	Render slot without ps and lightmap
bool A3DTerrain2Render::RenderSlot_NoPS_VL(A3DViewport * pViewport, A3DTrn2RenderSlot* pSlot)
{
	using namespace A3D;

	A3DTerrain2Mask* pMask = m_pTerrain->GetMaskArea(pSlot->m_iMaskIdx);
	if (!pMask)
		return true;

	int iMaxWeight = m_pTerrain->GetMaxLayerWeight();

	for (int iLayer=0; iLayer < pMask->GetLayerNum(); iLayer++)
	{
		if (!iLayer)
		{
			m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
			m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);

			ApplyLayerTexture(0, pMask, 0);

			pSlot->DrawAllBlocks(true);

			m_pA3DDevice->SetTextureColorOP(1, A3DTOP_SELECTARG2);
			m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_SELECTARG1);
		}
		else	//	Not the first layer
		{
			//	Check layer weight. 
			if ((int)pMask->GetLayerWeight(iLayer) > iMaxWeight)
			{
				//	In current version, we assume layer weight is in increasing
				//	order, so we can break now.
				break;
			}

			ApplyLayerTexture(0, pMask, iLayer);
			ApplyLayerMaskTexture(1, pMask, iLayer);

			pSlot->DrawAllBlocks(true);
		}
	}

	return true;
}

bool A3DTerrain2Render::ZPrePassSlot_NoPS_VL(A3DViewport * pViewport, A3DTrn2RenderSlot* pSlot)
{
	using namespace A3D;

	A3DTerrain2Mask* pMask = m_pTerrain->GetMaskArea(pSlot->m_iMaskIdx);
	if (!pMask)
		return true;

	int iMaxWeight = m_pTerrain->GetMaxLayerWeight();

	for (int iLayer=0; iLayer < pMask->GetLayerNum(); iLayer++)
	{
		if (!iLayer)
		{
			//m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
			//m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);

			//ApplyLayerTexture(0, pMask, 0);

			pSlot->DrawAllBlocks(true);

			//m_pA3DDevice->SetTextureColorOP(1, A3DTOP_SELECTARG2);
			//m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_SELECTARG1);
		}
		else	//	Not the first layer
		{
			//	Check layer weight. 
			if ((int)pMask->GetLayerWeight(iLayer) > iMaxWeight)
			{
				//	In current version, we assume layer weight is in increasing
				//	order, so we can break now.
				break;
			}

			//ApplyLayerTexture(0, pMask, iLayer);
			//ApplyLayerMaskTexture(1, pMask, iLayer);

			pSlot->DrawAllBlocks(true);
		}
	}

	return true;
}

//	Render routine using Texture Merge technique, lightmap version
bool A3DTerrain2Render::Render_TM_LM(A3DViewport* pViewport)
{
	//	Not supportted any more
	ASSERT(0);
	return true;
}

//	Render routine using Texture Merge technique, vertex light version
bool A3DTerrain2Render::Render_TM_VL(A3DViewport* pViewport)
{
	return true;
}

//	Render 1-4 layers using ps 1.4
bool A3DTerrain2Render::PrepareRender4Layers_PS14_LM(A3DTerrain2Mask* pMask, 
							int iStartLayer, int iNumLayers)
{
	ASSERT(!(iStartLayer & 3));

	//	Layer mask enable flags
	A3DCOLORVALUE EnableFlags;
	float* aFlags[4] = {&EnableFlags.b, &EnableFlags.g, &EnableFlags.r, &EnableFlags.a};

	for (int i=0; i < 4; i++)
	{
		if (i >= iNumLayers)
		{
			*aFlags[i] = 0.0f;
			ApplyLayerTexture(i, pMask, 0);
		}
		else
		{
			*aFlags[i] = 1.0f;
			ApplyLayerTexture(i, pMask, iStartLayer+i);
		}
	}

	m_pA3DDevice->SetPixelShaderConstants(2, &EnableFlags, 1);

	ApplyLayerMaskTexture(4, pMask, iStartLayer);
	ApplyLightMap(5, pMask, true);

	return true;
}

bool A3DTerrain2Render::PrepareRender3Layers_PS14_LM(A3DTerrain2Mask* pMask, 
							int iStartLayer, int iNumLayers)
{
	ASSERT(!(iStartLayer % 3));

	//	Layer mask enable flags
	A3DCOLORVALUE EnableFlags;
	float* aFlags[3] = {&EnableFlags.b, &EnableFlags.g, &EnableFlags.r};

	for (int i=0; i < 3; i++)
	{
		if (i >= iNumLayers)
		{
			*aFlags[i] = 0.0f;
			ApplyLayerTexture(i, pMask, 0);
		}
		else
		{
			*aFlags[i] = 1.0f;
			ApplyLayerTexture(i, pMask, iStartLayer+i);
		}
	}

	m_pA3DDevice->SetPixelShaderConstants(2, &EnableFlags, 1);
	m_pA3DDevice->SetPixelShaderConstants(3, &A3DCOLORVALUE(m_pTerrain->GetDNFactor()), 1);

	ApplyLayerMaskTexture(3, pMask, iStartLayer);
	ApplyLightMap(4, pMask, true);
	ApplyLightMap(5, pMask, false);

	return true;
}

bool A3DTerrain2Render::PrepareRenderShadowL8PS(A3DTerrain2Mask* pMask, int iStartLayer, int iNumLayers)
{
	using namespace A3D;

	ASSERT(!(iStartLayer % 3));

	if(!m_pA3DDevice->GetA3DEngine()->GetA3DShadowMap())
		return false;

	//	Layer mask enable flags
	A3DCOLORVALUE EnableFlags;
	float* aFlags[3] = {&EnableFlags.b, &EnableFlags.g, &EnableFlags.r};

	for (int i=0; i < 3; i++)
	{
		if (i >= iNumLayers)
		{
			*aFlags[i] = 0.0f;
			ApplyLayerTexture(i, pMask, 0);
		}
		else
		{
			*aFlags[i] = 1.0f;
			ApplyLayerTexture(i, pMask, iStartLayer+i);
		}
	}

	m_pA3DDevice->SetPixelShaderConstants(2, &EnableFlags, 1);
	// m_pA3DDevice->SetPixelShaderConstants(3, &A3DCOLORVALUE(m_pTerrain->GetDNFactor()), 1);

	ApplyLayerMaskTexture(3, pMask, iStartLayer);
	ApplyLightMap(4, pMask, true);

	m_pA3DDevice->GetA3DEngine()->GetA3DShadowMap()->GetTerrainShadowMap()->AppearAsTexture(5);
	m_pA3DDevice->SetTextureAddress(5, A3DTADDR_BORDER, A3DTADDR_BORDER);
	m_pA3DDevice->SetSamplerState(5, D3DSAMP_BORDERCOLOR, 0xffffffff);
	m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_POINT);

	return true;
}

bool A3DTerrain2Render::PrepareRender3Layers_PS14_VL(A3DTerrain2Mask* pMask, 
							int iStartLayer, int iNumLayers)
{
	ASSERT(!(iStartLayer % 3));

	//	Layer mask enable flags
	A3DCOLORVALUE EnableFlags;
	float* aFlags[3] = {&EnableFlags.b, &EnableFlags.g, &EnableFlags.r};

	for (int i=0; i < 3; i++)
	{
		if (i >= iNumLayers)
		{
			*aFlags[i] = 0.0f;
			ApplyLayerTexture(i, pMask, 0);
		}
		else
		{
			*aFlags[i] = 1.0f;
			ApplyLayerTexture(i, pMask, iStartLayer+i);
		}
	}

	m_pA3DDevice->SetPixelShaderConstants(2, &EnableFlags, 1);
	m_pA3DDevice->SetPixelShaderConstants(3, &A3DCOLORVALUE(m_pTerrain->GetDNFactor()), 1);

	ApplyLayerMaskTexture(3, pMask, iStartLayer);

	return true;
}

bool A3DTerrain2Render::PrepareRender_TM_LM(A3DTerrain2Mask* pMask)
{
	A3DRenderTarget* pTexture = pMask->GetMergedTexture();
	if (!pTexture)
		return false;

	//	Apply render target as texture
	pTexture->AppearAsTexture(0);

	//	Use lightmap texture coordinates
	m_pA3DDevice->SetTextureCoordIndex(0, 3);

	//	Scale texture coordinates
	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_COUNT2);
	m_matScale._11 = 1.0f;
	m_matScale._22 = 1.0f;
	m_pA3DDevice->SetTextureMatrix(0, m_matScale);

	//	Day night factor
	m_pA3DDevice->SetPixelShaderConstants(3, &A3DCOLORVALUE(m_pTerrain->GetDNFactor()), 1);

	//	Apply lightmap
	ApplyLightMap(1, pMask, true);
	ApplyLightMap(2, pMask, false);

	return true;
}

/*	Apply specified layer's texture in a mask area

	iStage: texture stage
	pMask: mask area object.
	iLayer: layer in mask area
*/
void A3DTerrain2Render::ApplyLayerTexture(int iStage, A3DTerrain2Mask* pMask, int iLayer)
{
	A3DTexture* pTexture = pMask->GetTexture(iLayer);
	if (!pTexture)
		return;

	pTexture->Appear(iStage);

	A3DTerrain2Mask::LAYER& Layer = pMask->GetLayer(iLayer);

	//	Select texture coordinates base on project axis
	switch (Layer.byProjAxis)
	{
	case 0:	m_pA3DDevice->SetTextureCoordIndex(iStage, 2);	break;
	case 1:	m_pA3DDevice->SetTextureCoordIndex(iStage, 0);	break;
	case 2:	m_pA3DDevice->SetTextureCoordIndex(iStage, 1);	break;
	}

	m_pA3DDevice->SetTextureAddress(iStage, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureTransformFlags(iStage, A3DTTFF_COUNT2);

	//	Scale texture coordinates
	m_matScale._11 = Layer.fUScale;
	m_matScale._22 = Layer.fVScale;
	m_pA3DDevice->SetTextureMatrix(iStage, m_matScale);
}

//	Apply specified layer's caustic map in a mask area
void A3DTerrain2Render::ApplyLayerCausticMap(int iStage, A3DTerrain2Mask* pMask, int iLayer)
{
	A3DTexture* pTexture = m_pTerrain->GetCausticTexture();
	if (!pTexture) // not set a caustic texture, just ignore it
		return;
	
	pTexture->Appear(iStage);

	A3DCOLORVALUE caustBrightness(0.0f, 0.0f, 0.0f, 0.1f);
	m_pA3DDevice->SetPixelShaderConstants(1, &caustBrightness, 1);
	m_pA3DDevice->SetTextureCoordIndex(iStage, D3DTSS_TCI_CAMERASPACEPOSITION);
	m_pA3DDevice->SetTextureAddress(iStage, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureTransformFlags(iStage, A3DTTFF_COUNT2);

	//	Scale texture coordinates
	A3DMATRIX4 matScale = IdentityMatrix();
	matScale._22 = 0.0f;	// switch y and z
	matScale._23 = 1.0f;
	matScale._33 = 0.0f;
	matScale._32 = -1.0f;
	matScale = InverseTM(m_pA3DDevice->GetA3DEngine()->GetActiveCamera()->GetViewTM()) * matScale * Scaling(0.04f, 0.04f, 0.04f) 
		* Translate(m_pTerrain->GetCaustDU() * 8.0f, m_pTerrain->GetCaustDV() * 8.0f, 0.0f);

	m_pA3DDevice->SetTextureMatrix(iStage, matScale);
}

//	Apply specified layer's mask texture in a mask area
void A3DTerrain2Render::ApplyLayerMaskTexture(int iStage, A3DTerrain2Mask* pMask, int iLayer)
{
	int iIndex = A3D::g_pA3DTerrain2Env->GetSupportPSFlag() ? (iLayer / 3) : iLayer;
	A3DTexture* pTexture = pMask->GetMaskTexture(iIndex);
	if (!pTexture)
		return;

	pTexture->Appear(iStage);

	m_pA3DDevice->SetTextureCoordIndex(iStage, 3);
	m_pA3DDevice->SetTextureAddress(iStage, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	//	For the same reason as in ApplyLightMap, we have to set texture
	//	transform matrix to identity instead of disabling it.
	m_pA3DDevice->SetTextureTransformFlags(iStage, A3DTTFF_COUNT2);
	m_matScale._11 = 1.0f;
	m_matScale._22 = 1.0f;
	m_pA3DDevice->SetTextureMatrix(iStage, m_matScale);
}

//	Apply light map
void A3DTerrain2Render::ApplyLightMap(int iStage, A3DTerrain2Mask* pMask, bool bDay)
{
	A3DTexture* pTexture = NULL;
	if (m_pTerrain->GetLightEnableFlag())
	{
		pTexture = pMask->GetLightMap(bDay);
		if (!pTexture)
		{
			if (!bDay)
				pTexture = pMask->GetLightMap(true);

			if (!pTexture)
				return;
		}
	}
	else
	{
		//	Use pure white texture as lightmap
		pTexture = A3D::g_pA3DTerrain2Env->GetWhiteTexture();
	}

	pTexture->Appear(iStage);

	m_pA3DDevice->SetTextureCoordIndex(iStage, 3);
	m_pA3DDevice->SetTextureAddress(iStage, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	//	Note: Because hardware's float precision problem, once we use
	//	SetTextureTransformFlags, we should always use it in following rendering,
	//	otherwise there will be flash problem caused by Z testing failure.
	m_pA3DDevice->SetTextureTransformFlags(iStage, A3DTTFF_COUNT2);
	m_matScale._11 = 1.0f;
	m_matScale._22 = 1.0f;
	m_pA3DDevice->SetTextureMatrix(iStage, m_matScale);
}

bool A3DTerrain2Render::PrepareVertexConstants(A3DViewport * pViewport)
{
	using namespace A3D;

	if( m_pTerrain->IsRenderForRefract() )
	{
		// prepare vertex shader for refract rendering
		static float s_fogDensity = 0.075f;
		static float s_fogOffset = 0.0f;
		if( GetKeyState('1') & 0x8000 )
		{
			if( GetKeyState(VK_UP) & 0x8000 )
				s_fogDensity += 0.001f;
			if( GetKeyState(VK_DOWN) & 0x8000 )
				s_fogDensity -= 0.001f;
		}
		if( GetKeyState('2') & 0x8000 )
		{
			if( GetKeyState(VK_UP) & 0x8000 )
				s_fogOffset += 0.001f;
			if( GetKeyState(VK_DOWN) & 0x8000 )
				s_fogOffset -= 0.001f;
		}

		s_fogDensity = min2(max2(s_fogDensity, 0.0f), 1.0f);
		s_fogOffset = min2(max2(s_fogOffset, 0.0f), 1.0f);
		A3DVECTOR4 c0 = A3DVECTOR4(1.0f, m_pTerrain->GetRefractSurfaceHeight(), s_fogDensity, s_fogOffset);
		m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);
	}
	else
	{
		A3DVECTOR4 c0 = A3DVECTOR4(1.0f, 0.0f, 0.0f, 0.0f);
		m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);
	}

	A3DMATRIX4 matVP = m_pA3DDevice->GetViewMatrix() * m_pA3DDevice->GetProjectionMatrix();
	matVP.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(1, &matVP, 4);

	A3DVECTOR4 c5 = A3DVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	c5.m[m_pA3DDevice->GetTextureCoordIndex(0)]  = 1.0f;
	m_pA3DDevice->SetVertexShaderConstants(5, &c5, 1);

	A3DVECTOR4 c6 = A3DVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	c6.m[m_pA3DDevice->GetTextureCoordIndex(1)]  = 1.0f;
	m_pA3DDevice->SetVertexShaderConstants(6, &c6, 1);

	A3DVECTOR4 c7 = A3DVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	c7.m[m_pA3DDevice->GetTextureCoordIndex(2)]  = 1.0f;
	m_pA3DDevice->SetVertexShaderConstants(7, &c7, 1);

	A3DMATRIX4 matTex0 = m_pA3DDevice->GetTextureMatrix(0);
	matTex0.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(8, &matTex0, 4);

	A3DMATRIX4 matTex1 = m_pA3DDevice->GetTextureMatrix(1);
	matTex1.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(12, &matTex1, 4);

	A3DMATRIX4 matTex2 = m_pA3DDevice->GetTextureMatrix(2);
	matTex2.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(16, &matTex2, 4);

	A3DVECTOR4 c20 = A3DVECTOR4(pViewport->GetCamera()->GetPos());
	m_pA3DDevice->SetVertexShaderConstants(20, &c20, 1);

	A3DVECTOR4 c21 = A3DVECTOR4(1.0f, 0.0f, 0.0f, 0.0f);
	m_pA3DDevice->SetVertexShaderConstants(21, &c21, 1);
	
	return true;
}

bool A3DTerrain2Render::PrepareVertexConstantsL8(A3DViewport* pViewport)
{
	A3DShadowMap* pShadowMap = m_pA3DDevice->GetA3DEngine()->GetA3DShadowMap();
	if(!pShadowMap)
		return false;

	A3DVECTOR4 c0 = A3DVECTOR4(1.0f, 0.0f, 0.0f, 0.0f);
	m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);

	A3DMATRIX4 matVP = m_pA3DDevice->GetViewMatrix() * m_pA3DDevice->GetProjectionMatrix();
	matVP.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(1, &matVP, 4);

	A3DVECTOR4 c5 = A3DVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	c5.m[m_pA3DDevice->GetTextureCoordIndex(0)]  = 1.0f;
	m_pA3DDevice->SetVertexShaderConstants(5, &c5, 1);

	A3DVECTOR4 c6 = A3DVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	c6.m[m_pA3DDevice->GetTextureCoordIndex(1)]  = 1.0f;
	m_pA3DDevice->SetVertexShaderConstants(6, &c6, 1);

	A3DVECTOR4 c7 = A3DVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	c7.m[m_pA3DDevice->GetTextureCoordIndex(2)]  = 1.0f;
	m_pA3DDevice->SetVertexShaderConstants(7, &c7, 1);

	A3DMATRIX4 matTex0 = m_pA3DDevice->GetTextureMatrix(0);
	matTex0.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(8, &matTex0, 4);

	A3DMATRIX4 matTex1 = m_pA3DDevice->GetTextureMatrix(1);
	matTex1.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(12, &matTex1, 4);

	A3DMATRIX4 matTex2 = m_pA3DDevice->GetTextureMatrix(2);
	matTex2.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(16, &matTex2, 4);

	A3DVECTOR4 c20 = A3DVECTOR4(pViewport->GetCamera()->GetPos());
	m_pA3DDevice->SetVertexShaderConstants(20, &c20, 1);

	A3DVECTOR4 c21 = A3DVECTOR4(1.0f, 0.0f, 0.0f, 0.0f);
	m_pA3DDevice->SetVertexShaderConstants(21, &c21, 1);

	int nShadowMapSize = pShadowMap->GetShadowMapSize();
	A3DMATRIX4 matScale(A3DMATRIX4::IDENTITY);
	matScale.m[0][0] = 0.5f;
	matScale.m[1][1] = -0.5f;
	matScale.m[3][0] = 0.5f + 0.5f / nShadowMapSize;
	matScale.m[3][1] = 0.5f + 0.5f / nShadowMapSize;
	A3DMATRIX4 matRT = pShadowMap->GetTerrainShadowCamera()->GetVPTM() * matScale;
	matRT.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(22, (void*)&matRT, 4);
	
	return true;
}

bool A3DTerrain2Render::RenderWithLightMapHDR(A3DViewport* pViewport, A3DRenderTarget* pTerrainColorRT)
{
	using namespace A3D;

	if( pTerrainColorRT == NULL)
		return false;

	//get hdr resouce
	A3DPixelShader* psHDR = g_pA3DTerrain2Env->GetPSLightMapHDR();
	A3DVertexShader* vsHDR = g_pA3DTerrain2Env->GetVSLightMapHDR();

	A3DTexture* pHDRLookup = g_pA3DTerrain2Env->GetHDRLookup();
	if( psHDR == NULL || pHDRLookup == NULL || vsHDR == NULL)
		return false;

	if (!m_aCurSlots.GetSize())
		return true;	//	Nothing needs rendering

	//	Sync slots to accelerate stream constructing speed
	SyncLastRenderSlots();

	pViewport->Active();

	//set render state
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSpecularEnable(false);
	//m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetAlphaBlendEnable(false);
	
	//Set color texture
	pTerrainColorRT->AppearAsTexture(0);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
		
	pHDRLookup->Appear(3);
	m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(3,A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	pHDRLookup->Appear(4);
	m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(4,A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	pHDRLookup->Appear(5);
	m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(5,A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	
	//Set shader
	psHDR->Appear();
	vsHDR->Appear();

	A3DVECTOR4 c1(0.0f, 0.0f, 0.0f, m_pTerrain->GetDNFactor());
	m_pA3DDevice->SetPixelShaderConstants(1, &c1, 1);
	
	//set vs const
	A3DMATRIX4 matVP= pViewport->GetCamera()->GetVPTM();
	A3DMATRIX4 matVPTranpose = matVP.GetTranspose();
	A3DMATRIX4 matAdjust = IdentityMatrix();

	int nWidth = pViewport->GetParam()->Width;
	int nHeight = pViewport->GetParam()->Height;
	A3DDEVFMT devFmt = m_pA3DDevice->GetDevFormat();
	matAdjust._11 = 0.5f * nWidth / devFmt.nWidth;
	matAdjust._22 = -0.5f * nHeight / devFmt.nHeight;

	matAdjust._41 = 0.5f + 0.5f / devFmt.nWidth;
	matAdjust._42 = 0.5f + 0.5f / devFmt.nHeight;
	
	A3DMATRIX4 matTex = matVP * matAdjust;
	matTex.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(0, &matVPTranpose, 4);
	m_pA3DDevice->SetVertexShaderConstants(4, &matTex, 4);
	
	//Draw
	for (int i=0; i < m_aCurSlots.GetSize(); i++)
		RenderSlot_HDRPass(pViewport, m_aCurSlots[i]);
	
	//restore textures
	pTerrainColorRT->DisappearAsTexture(0);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	
	pHDRLookup->Disappear(3);
	pHDRLookup->Disappear(4);
	pHDRLookup->Disappear(5);
	m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(3,A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(4,A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(5,A3DTADDR_WRAP, A3DTADDR_WRAP);

		
	//Restore shaders
	psHDR->Disappear();
	vsHDR->Disappear();

	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(2, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(3, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(4, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(5, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(2, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(3, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(4, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(5, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureCoordIndex(0, 0);
	m_pA3DDevice->SetTextureCoordIndex(1, 1);
	m_pA3DDevice->SetTextureCoordIndex(2, 2);
	m_pA3DDevice->SetTextureCoordIndex(3, 3);
	m_pA3DDevice->SetTextureCoordIndex(4, 4);
	m_pA3DDevice->SetTextureCoordIndex(5, 5);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(0,A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(2,A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(1,A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(2,A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(3,A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(4,A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(5,A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->ClearTexture(0);
	m_pA3DDevice->ClearTexture(1);
	m_pA3DDevice->ClearTexture(2);
	m_pA3DDevice->ClearTexture(3);
	m_pA3DDevice->ClearTexture(4);
	m_pA3DDevice->ClearTexture(5);


	//Restore render state
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	//m_pA3DDevice->SetFogEnable(true);

	return true;

}

//	Render slot using depth first method. pixel shader and lightmap version
bool A3DTerrain2Render::RenderSlot_HDRPass(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot)
{
	A3DTerrain2Mask* pMask = m_pTerrain->GetMaskArea(pSlot->m_iMaskIdx);
	if (!pMask)
		return true;
	
	//	Apply lightmap
	ApplyLightMap(1, pMask, true);
	ApplyLightMap(2, pMask, false);
	
	//Draw terrain
	pSlot->DrawAllBlocks(false);

	return true;
}

//	Render raw mesh routine
bool A3DTerrain2Render::RenderRawMesh(A3DViewport* pViewport, bool bApplyFVF)
{
	using namespace A3D;

	if (!m_aCurSlots.GetSize())
		return true;	//	Nothing needs rendering

	//	Sync slots to accelerate stream constructing speed
	SyncLastRenderSlots();

	//	Set world transform matrix
	m_pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

	for (int i=0; i < m_aCurSlots.GetSize(); i++)
	{
		A3DTrn2RenderSlot* pSlot = m_aCurSlots[i];
		pSlot->DrawAllBlocks(bApplyFVF);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DTerrain2TexMerger
//	
///////////////////////////////////////////////////////////////////////////

A3DTerrain2TexMerger::A3DTerrain2TexMerger()
{
	m_pTerrain		= NULL;
	m_pA3DDevice	= NULL;
	m_pViewport		= NULL;
	m_pCamera		= NULL;
	m_pStream		= NULL;

	m_iTexWidth		= 512;
	m_iTexHeight	= 512;

	m_matScale.Identity();
}

A3DTerrain2TexMerger::~A3DTerrain2TexMerger()
{
}

//	Initlaize object
bool A3DTerrain2TexMerger::Init(A3DTerrain2* pTerrain)
{
	ASSERT(pTerrain);

	m_pTerrain = pTerrain;
	m_pA3DDevice = pTerrain->GetA3DDevice();
	if (!m_pA3DDevice)
		return false;

	float fLen = 10.0f;

	//	Create camera
	m_pCamera = new A3DOrthoCamera;
	if (!m_pCamera || !m_pCamera->Init(m_pA3DDevice, -fLen, fLen, -fLen, fLen, -100.0f, 100.0f))
	{
		g_A3DErrLog.Log("A3DTerrain2TexMerger::Init, Failed to create camera !");
		return false;
	}
	
	m_pCamera->SetPos(A3D::g_vOrigin);
	m_pCamera->SetDirAndUp(A3D::g_vAxisZ, A3D::g_vAxisY);

	//	Create viewport
	if (!m_pA3DDevice->CreateViewport(&m_pViewport, 0, 0, m_iTexWidth, m_iTexHeight,
									0.0f, 1.0f, true, true, 0xff000000))
	{
		A3DRELEASE(m_pCamera);
		g_A3DErrLog.Log("A3DTerrain2TexMerger::Init, Failed to create viewport");
		return false;
	}
	
	m_pViewport->SetCamera(m_pCamera);

	//	Create stream
	m_pStream = new A3DStream;
	if (!m_pStream || !m_pStream->Init(m_pA3DDevice, sizeof (MERGEVERTEX), FVF_MERGEVERTEX, 4, 6, A3DSTRM_STATIC, A3DSTRM_STATIC))
	{
		A3DRELEASE(m_pCamera);
		A3DRELEASE(m_pViewport);
		g_A3DErrLog.Log("A3DTerrain2TexMerger::Init, failed to create stream !");
		return false;
	}

	WORD aIndices[6] = {0, 1, 2, 2, 1, 3};

	MERGEVERTEX aVerts[4];
	aVerts[0] = MERGEVERTEX(A3DVECTOR3(-fLen, fLen, 0.0f), -A3D::g_vAxisZ, 0.0f, 0.0f, 0.0f, 0.0f);
	aVerts[1] = MERGEVERTEX(A3DVECTOR3(fLen, fLen, 0.0f), -A3D::g_vAxisZ, 1.0f, 0.0f, 1.0f, 0.0f);
	aVerts[2] = MERGEVERTEX(A3DVECTOR3(-fLen, -fLen, 0.0f), -A3D::g_vAxisZ, 0.0f, 1.0f, 0.0f, 1.0f);
	aVerts[3] = MERGEVERTEX(A3DVECTOR3(fLen, -fLen, 0.0f), -A3D::g_vAxisZ, 1.0f, 1.0f, 1.0f, 1.0f);

	m_pStream->SetVerts((BYTE*)aVerts, 4);
	m_pStream->SetIndices((BYTE*)aIndices, 6);

	return true;
}

//	Release object
void A3DTerrain2TexMerger::Release()
{
	A3DRELEASE(m_pStream);
	A3DRELEASE(m_pViewport);
	A3DRELEASE(m_pCamera);

	m_pA3DDevice = NULL;
}

//	Merge mask textures
A3DRenderTarget* A3DTerrain2TexMerger::MergeMaskTextures(A3DTerrain2Mask* pTrnMask)
{
	if (!m_pA3DDevice)
	{
		ASSERT(m_pA3DDevice);
		return NULL;
	}

	//	Calculate vertex coordinates
	if (!CalcTextureCoords(pTrnMask))
		return NULL;

	//	Create target object
	A3DRenderTarget::RTFMT rtFmt;
	
	rtFmt.iWidth		= m_iTexWidth;
	rtFmt.iHeight		= m_iTexHeight;
	rtFmt.fmtTarget	= A3DFMT_X8R8G8B8;
	rtFmt.fmtDepth		= A3DFMT_D24X8;

	A3DRenderTarget* pTarget = new A3DRenderTarget;
	if (!pTarget || !pTarget->Init(m_pA3DDevice, rtFmt, true, true))
		return NULL;

	m_pA3DDevice->BeginRender();

	//	Apply viewport
	pTarget->ApplyToDevice();
	m_pViewport->Active();
	m_pViewport->ClearDevice();

	//	Set render states
	m_pA3DDevice->SetLightingEnable(true);
	bool bFog = m_pA3DDevice->GetFogEnable();
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	m_pStream->Appear();

	//	Set pixel shader
	A3D::g_pA3DTerrain2Env->GetPSTexMerge()->Appear();

	A3DCOLORVALUE aMasks[3];
	aMasks[0].Set(0.0f, 0.0f, 1.0f, 0.0f);
	aMasks[1].Set(0.0f, 1.0f, 0.0f, 0.0f);
	aMasks[2].Set(1.0f, 0.0f, 0.0f, 0.0f);

	for (int iLayer=0; iLayer < pTrnMask->GetLayerNum(); iLayer++)
	{
		//	Check layer weight. 
	//	if (iLayer && (int)pMask->GetLayerWeight(iLayer) > iMaxWeight)
	//	{
	//		//	In current version, we assume layer weight is in increasing
	//		//	order, so we can break now.
	//		break;
	//	}

	//	if (iLayer > 0)
	//		break;

		//	Apply layer textures
		ApplyLayerTexture(pTrnMask, iLayer);

		//	Set ps constants
		int t = iLayer % 3;
		m_pA3DDevice->SetPixelShaderConstants(2, &aMasks[t], 1);

		//	Render primitives
		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2);
	}

	//	Restore render states
	m_pA3DDevice->ClearPixelShader();
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetFogEnable(bFog);

	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureCoordIndex(1, 1);
	m_pA3DDevice->ClearTexture(1);

	pTarget->WithdrawFromDevice();
	m_pA3DDevice->EndRender();

/*	static int count = 0;
	if (count < 25)
	{
		char szFile[MAX_PATH];
		sprintf(szFile, "G:\\Develop\\赤壁\\Temp\\merge_tex_%d.bmp", count++);
		pTarget->ExportColorToFile(szFile);
	}
*/
	return pTarget;
}

//	Calculate vertex's texture coordinates
bool A3DTerrain2TexMerger::CalcTextureCoords(A3DTerrain2Mask* pTrnMask)
{
	int iMaskArea = pTrnMask->GetMaskAreaIndex();
	ASSERT(iMaskArea >= 0 && iMaskArea < m_pTerrain->GetMaskAreaNum());

	//	Mask area row and column in whole terrain
	int iMaskPitch = m_pTerrain->GetBlockColNum() * m_pTerrain->GetBlockGrid() / m_pTerrain->GetMaskGrid();
	int r = iMaskArea / iMaskPitch;
	int c = iMaskArea % iMaskPitch;

	float ou = m_pTerrain->GetTexOffsetU();
	float ov = m_pTerrain->GetTexOffsetV();

	//	Set texture coordinates for vertices
	MERGEVERTEX* aVerts;
	if (!m_pStream->LockVertexBuffer(0, 0, (BYTE**)&aVerts, 0))
		return false;

	float fMaskAreaSize = m_pTerrain->GetMaskGrid() * m_pTerrain->GetGridSize();
	float x = m_pTerrain->GetTerrainArea().left + c * fMaskAreaSize;
	float z = m_pTerrain->GetTerrainArea().top - r * fMaskAreaSize;

	aVerts[0].tu1 = x + ou;
	aVerts[0].tv1 = z + ov;
	aVerts[1].tu1 = x + fMaskAreaSize + ou;
	aVerts[1].tv1 = z + ov;
	aVerts[2].tu1 = x + ou;
	aVerts[2].tv1 = z - fMaskAreaSize + ov;
	aVerts[3].tu1 = x + fMaskAreaSize + ou;
	aVerts[3].tv1 = z - fMaskAreaSize + ov;

	m_pStream->UnlockVertexBuffer();

	return true;
}

//	Apply specified layer's texture
bool A3DTerrain2TexMerger::ApplyLayerTexture(A3DTerrain2Mask* pTrnMask, int iLayer)
{
	int iIndex = A3D::g_pA3DTerrain2Env->GetSupportPSFlag() ? (iLayer / 3) : iLayer;

	A3DTexture* pTexture = pTrnMask->GetTexture(iLayer);
	A3DTexture* pMaskTexture = pTrnMask->GetMaskTexture(iIndex);
	if (!pTexture || !pMaskTexture)
		return false;

	pTexture->Appear(0);
	pMaskTexture->Appear(1);

	A3DTerrain2Mask::LAYER& Layer = pTrnMask->GetLayer(iLayer);

	//	Set texture stage states
	m_pA3DDevice->SetTextureCoordIndex(0, 0);
	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_COUNT2);
	m_matScale._11 = Layer.fUScale;
	m_matScale._22 = Layer.fVScale;
	m_pA3DDevice->SetTextureMatrix(0, m_matScale);

	//	Set mask stage states
	m_pA3DDevice->SetTextureCoordIndex(1, 1);
	m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_COUNT2);
	m_matScale._11 = 1.0f;
	m_matScale._22 = 1.0f;
	m_pA3DDevice->SetTextureMatrix(1, m_matScale);

	return true;
}

//	Release merged texture of mask
void A3DTerrain2TexMerger::ReleaseMergedTexture(A3DRenderTarget* pTarget)
{
	A3DRELEASE(pTarget);
}
