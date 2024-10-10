/*
 * FILE: ELTerrainOutline2.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Wangkuiwu, 2005/6/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */
#include <A3DDevice.h>

#include <A3DEngine.h>
#include <A3DTextureMan.h>

#include <A3DFont.h>

#include <A3DViewport.h>
#include <A3DTexture.h>
#include <A3DStream.h>
#include <A3DTerrain2.h>
#include <A3DTerrain2Blk.h>
#include <AFileImage.h>
#include <A3DCameraBase.h>
#include <A3DFuncs.h>

#include <io.h>

#include "ELTerrainOutline2.h"
#include "ELOutlineTexture.h"

#define OUTLINE_VIEW_DIST          2000
#define OUTLINE_VIEW_DIST_LEVEL0 800
#define OUTLINE_VIEW_DIST_LEVEL1 1200


#define OUTLINE_DETAIL_TEX_PATH   "textures\\maps\\detail.dds"


CELTerrainOutline2::CELTerrainOutline2()
{
	m_pA3DDevice		= NULL;
	m_pTerrain2         = NULL;

	m_nWorldWidth       = 0;
	m_nWorldHeight      = 0;
	m_x                 = 0.0f;
	m_z                 = 0.0f;


	m_nMaxReplaceTerrainFace = 0;
	m_nMaxReplaceTerrainVert = 0;
	
	m_pReplaceTerrainIndices = NULL;
	m_pReplaceTerrainVerts = NULL;
	
	m_pFakeTerrainStream = NULL;
	
	m_BlockVisibleRect.Clear();

	m_pOutlineTexCache	= NULL;
	m_dwFrame       = 0;
	
	m_pTerrain2BlkInfo = NULL;
	m_iMinLevel        = 0;
	m_bReplaceTerrain  = false;

	m_pDetailTexture = NULL;
	m_bNeedUpdate    = true;

	//m_fDNFactor = 0.0f;   // day as default 
	//m_fDNFactor = 1.0f;
	m_fDNFactor = 0.5f;

	m_iCurViewScheme = 4; // furthest as default
}



CELTerrainOutline2::~CELTerrainOutline2()
{
	Release();
}



bool CELTerrainOutline2::Init(A3DDevice * pA3DDevice,  const char * szTexDir, const char * szTileDir, A3DTerrain2* pTerrain2, unsigned int nWTileCount, unsigned int nHTileCount)
{
	//@note : release first. By Kuiwu[21/9/2005]
	Release();

	//@note : pre-condition. By Kuiwu[21/9/2005]
	//@todo : Ugly. Remove the hard-code. By Kuiwu[21/9/2005]
	assert(nWTileCount <= 8 && nHTileCount <= 11 && "invalid input");

	
	m_pA3DDevice = pA3DDevice;
	m_pTerrain2 = pTerrain2;
	m_szTexDir = szTexDir;
	//@todo : fixme, using the input param By Kuiwu[26/8/2005]
	//m_szTexDir = "E:\\完美世界\\element\\loddata\\512\\";

	m_szTileDir = szTileDir;

	//set the level resolution
	//@note : ugly but efficient :). By Kuiwu[21/9/2005]
	OUTLINE_TEX_LEVEL[0] = nWTileCount;   //level0
	OUTLINE_TEX_LEVEL[1] = nHTileCount;
	OUTLINE_TEX_LEVEL[2] = (int)ceil(OUTLINE_TEX_LEVEL[0] *0.5f); //level1
	OUTLINE_TEX_LEVEL[3] = (int)ceil(OUTLINE_TEX_LEVEL[1] *0.5f);
	OUTLINE_TEX_LEVEL[4] = (int)ceil(OUTLINE_TEX_LEVEL[2] *0.5f);//level2
	OUTLINE_TEX_LEVEL[5] = (int)ceil(OUTLINE_TEX_LEVEL[3] *0.5f);
	OUTLINE_TEX_LEVEL[6] = (int)ceil(OUTLINE_TEX_LEVEL[4] *0.5f);//level3
	OUTLINE_TEX_LEVEL[7] = (int)ceil(OUTLINE_TEX_LEVEL[5] *0.5f);
	OUTLINE_TEX_LEVEL[8] = (int)ceil(OUTLINE_TEX_LEVEL[6] *0.5f);//level4
	OUTLINE_TEX_LEVEL[9] = (int)ceil(OUTLINE_TEX_LEVEL[7] *0.5f);


	// now try to open one olm file
	char path[MAX_PATH];
	sprintf(path, "%s%d.olm", m_szTileDir, 1);
	AFileImage fileImage;
	if( !fileImage.Open(path, AFILE_OPENEXIST) )
		return false;
	DWORD dwReadLen;
	DWORD dwMagic;
	if (!fileImage.Read(&dwMagic, sizeof(DWORD), &dwReadLen)) {
		return false;
	}
	DWORD dwVersion;
	if (!fileImage.Read(&dwVersion, sizeof(DWORD), &dwReadLen)) {
		return false;
	}
	if (dwMagic != OUTLINE_TILE_MAGIC || dwVersion != OUTLINE_TILE_VERSION) {
		return false;
	}
	fileImage.Close();


	m_fLevelEndDist[0]  = OUTLINE_VIEW_DIST_LEVEL0;
	m_fLevelEndDist[1]  = OUTLINE_VIEW_DIST_LEVEL1;
	m_fLevelEndDist[2]  = OUTLINE_VIEW_DIST;
	m_fLevelEndDist[3]  = 5000.0F;
	m_fLevelEndDist[4]  = 10000.0F;

	m_nWorldWidth = OUTLINE_TILE_WIDTH * OUTLINE_TEX_LEVEL[0];
	m_nWorldHeight = OUTLINE_TILE_HEIGHT * OUTLINE_TEX_LEVEL[1];

	m_x  = (float)(-(m_nWorldWidth >>1));
	m_z  = (float)(m_nWorldHeight>>1);

	m_OutlineTexSize[0] = OUTLINE_TILE_WIDTH;
	m_OutlineTexSize[1] = OUTLINE_TILE_HEIGHT;
	for (int i = 1; i < OUTLINE_MAX_LEVEL; i++) {
		m_OutlineTexSize[i*2] = m_OutlineTexSize[(i-1)*2]*2;	
		m_OutlineTexSize[i*2+1] = m_OutlineTexSize[(i-1)*2+1]*2;
	} 
	

	if (!m_pOutlineTexCache) {
		m_pOutlineTexCache = new CELOutlineTextureCache;
		m_pOutlineTexCache->Init(this);
	}
	

	m_dwFrame  = 0;
	m_iMinLevel = 0;
	m_bReplaceTerrain = false;
	
	int iMaskBlock = m_pTerrain2->GetMaskGrid() / m_pTerrain2->GetBlockGrid();
	iMaskBlock *= iMaskBlock;
	m_nMaxReplaceTerrainVert  = (m_pTerrain2->GetBlockGrid() + 1) * (m_pTerrain2->GetBlockGrid() + 1) * iMaskBlock;
	m_nMaxReplaceTerrainFace  = m_pTerrain2->GetBlockGrid() * m_pTerrain2->GetBlockGrid() * 2 * iMaskBlock;


	//load detail texture
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(OUTLINE_DETAIL_TEX_PATH, &m_pDetailTexture, A3DTF_NODOWNSAMPLE) )
		return false;


	m_nExpectCacheSize[0] = 8;
	m_nExpectCacheSize[1] = 12;
	m_nExpectCacheSize[2] = 12;
	m_nExpectCacheSize[3] = 16;
	m_nExpectCacheSize[4] = 20;
	m_iCurViewScheme = 4;

	m_bNeedUpdate = true;
	

	
	return true;
}






void CELTerrainOutline2::Release()
{
	if( m_pDetailTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pDetailTexture);
		m_pDetailTexture = NULL;
	}



	if (m_pOutlineTexCache) {
		delete m_pOutlineTexCache;
		m_pOutlineTexCache = NULL;
	}


	
	if (m_pTerrain2BlkInfo) {
		delete[]  m_pTerrain2BlkInfo;
		m_pTerrain2BlkInfo = NULL;
	}

	_FreeReplaceTerrainBuf();
}




void CELTerrainOutline2::SetReplaceTerrain(bool bVal)
{
	m_bReplaceTerrain = bVal;
	if (m_bReplaceTerrain) {
		_AllocReplaceTerrainBuf();
	}
	else{
		_FreeReplaceTerrainBuf();
	}
}


void CELTerrainOutline2::_AllocReplaceTerrainBuf()
{
	if((m_pReplaceTerrainVerts != NULL) && (m_pReplaceTerrainIndices != NULL)) {
		return;
	}
	
	m_pReplaceTerrainVerts = new A3DVECTOR3[m_nMaxReplaceTerrainVert];
	assert(m_pReplaceTerrainVerts != NULL);
	m_pReplaceTerrainIndices  = new WORD[m_nMaxReplaceTerrainFace*3];
	assert(m_pReplaceTerrainIndices != NULL);

	m_pFakeTerrainStream = new A3DStream;
	if(!m_pFakeTerrainStream->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nMaxReplaceTerrainVert,
		m_nMaxReplaceTerrainFace*3, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR)){
			assert(0);
			return;
		}
	
	
}

void CELTerrainOutline2::_FreeReplaceTerrainBuf()
{
	if (m_pReplaceTerrainVerts) {
		delete[] m_pReplaceTerrainVerts;
		m_pReplaceTerrainVerts = NULL;
	}
	if (m_pReplaceTerrainIndices) {
		delete[] m_pReplaceTerrainIndices;
		m_pReplaceTerrainIndices = NULL;
	}

	if (m_pFakeTerrainStream) {
		m_pFakeTerrainStream->Release();
		delete m_pFakeTerrainStream;
		m_pFakeTerrainStream = NULL;
	}
}



void CELTerrainOutline2::Update(A3DCameraBase * pCamera, const A3DVECTOR3& vecCenter)
{
	A3DVECTOR3  dir = pCamera->GetDirH();
	A3DVECTOR3  pos = vecCenter;
	
	bool   bVisibleChange = false;
	ARectI rcVisible; 
	m_pTerrain2->CalcAreaInBlocks(pos, m_pTerrain2->GetViewRadius(), rcVisible );
	A3DTerrain2::ACTBLOCKS * pActBlocks = m_pTerrain2->GetActiveBlocks();
	
	

	if ((m_bNeedUpdate) || (rcVisible != m_BlockVisibleRect) ){
		if (rcVisible.Width() * rcVisible.Height() != 
			m_BlockVisibleRect.Width()* m_BlockVisibleRect.Height()) {
			
			if (m_pTerrain2BlkInfo) {
				delete[] m_pTerrain2BlkInfo;
			}
			m_pTerrain2BlkInfo = new TerrainBlockInfo[rcVisible.Width()*rcVisible.Height()];
		}
		m_BlockVisibleRect = rcVisible;
		
		//increase frame
		m_dwFrame++;
		//activate 
		m_pOutlineTexCache->ActivateTile(pos, m_fLevelEndDist[m_iMinLevel+2], m_dwFrame);

#ifdef _DEBUG

//		int nLevel0, nLevel1, nLevel2, nTotal;
//		nLevel0 = m_pOutlineTexCache->GetActiveTexCount(m_dwFrame, 0);
//		nLevel1 = m_pOutlineTexCache->GetActiveTexCount(m_dwFrame, 1);
//		nLevel2 = m_pOutlineTexCache->GetActiveTexCount(m_dwFrame, 2);
//		nTotal = nLevel0 + nLevel1 + nLevel2;
//		char msg[200];
//		sprintf(msg, "frame %d total %d level0 %d level1 %d level2 %d enddist %f %f %f\n",
//			m_dwFrame, nTotal, nLevel0, nLevel1, nLevel2, m_fLevelEndDist[0], m_fLevelEndDist[1], m_fLevelEndDist[2]);
//		OutputDebugStringA(msg);
//		
//		int nCache = m_pOutlineTexCache->GetCacheSize();
//		sprintf(msg, "scheme %d expectcache %d realcache %d\n", m_iCurViewScheme,
//			m_nExpectCacheSize[m_iCurViewScheme], nCache);
//		OutputDebugStringA(msg);
#endif
		
		//view
		m_pOutlineTexCache->CreateView(m_dwFrame, pos, m_fLevelEndDist[m_iMinLevel+2]);
		//m_pOutlineTexCache->CreateView(m_dwFrame, pos, m_fLevelEndDist[m_iMinLevel+1]);
		

		bVisibleChange = true;
	}

	bool bBlkChange;
	_CheckBlkInfo(pActBlocks, m_BlockVisibleRect, bBlkChange);
	if (m_bNeedUpdate || bVisibleChange || bBlkChange) {
		m_pOutlineTexCache->Stitch(m_dwFrame, pActBlocks);
	}
	m_bNeedUpdate = false;

}

void CELTerrainOutline2::_CheckBlkInfo(A3DTerrain2::ACTBLOCKS *pActBlocks, const ARectI& rcVisible, bool& bBlkChange)
{
	bBlkChange = false;

	int r, c;
	A3DTerrain2Block * pBlock;

#define   TV_INDEX(c, r)  ((r- rcVisible.top) * rcVisible.Width() + (c- rcVisible.left))

	for (r = rcVisible.top; r < rcVisible.bottom; r++) {
		for (c = rcVisible.left;  c < rcVisible.right; c++ ) {
			pBlock = pActBlocks->GetBlock(r, c, false);
			bool bReady = (pBlock != NULL);
			int  tvIndex = TV_INDEX(c, r);
			int iLod = -1;
			if (bReady) {
				iLod = pBlock->GetLODLevel();
			}
			if (!m_pTerrain2BlkInfo[tvIndex].Equal(r, c, bReady, iLod)) {
				m_pTerrain2BlkInfo[tvIndex].Set(r, c, bReady, iLod);
				bBlkChange =true;		
			}
		}
	}
#undef  TV_INDEX	


}


void CELTerrainOutline2::_RenderReplaceTerrain(A3DViewport* pViewport)
{
	if (!m_bReplaceTerrain) {
		return;
	}

	bool bNeedDetailPass = false;

	int iLayer;
	if (m_fDNFactor < 1.0f && m_fDNFactor > 0.0f) 
	{
		iLayer = 2;
		if( m_pA3DDevice->GetD3DCaps().MaxTextureBlendStages > 2 && m_pA3DDevice->GetD3DCaps().MaxSimultaneousTextures > 2 )
		{
			bNeedDetailPass = false;
		}
		else
		{
			bNeedDetailPass = true;
			iLayer = 0;
		}
	} 
	else 
	{//day or night
		iLayer = 1;
		bNeedDetailPass = false;
	}

	//	Update terrain block visibility info
//	m_pTerrain2->DoBlocksCullAndLOD(pViewport);
	m_pTerrain2->DoBlocksCull(pViewport);

	if( !bNeedDetailPass )
	{
		A3DTEXTUREOP  prevOp = m_pA3DDevice->GetTextureColorOP(iLayer);

		m_pDetailTexture->Appear(iLayer);
		m_pA3DDevice->SetTextureCoordIndex(iLayer, 0);
		m_pA3DDevice->SetTextureTransformFlags(iLayer, A3DTTFF_COUNT3);
		m_pA3DDevice->SetTextureMatrix(iLayer, Scaling(128.0f, 128.0f, 128.0f));
		m_pA3DDevice->SetTextureColorOP(iLayer, A3DTOP_MODULATE2X);
		m_pA3DDevice->SetTextureFilterType(iLayer, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureColorOP(iLayer+1, A3DTOP_DISABLE);
		m_pA3DDevice->SetTextureAlphaOP(iLayer+1, A3DTOP_DISABLE);

		//make sure buffer is ready
		assert((m_pReplaceTerrainIndices != NULL) &&(m_pReplaceTerrainVerts != NULL));
		A3DTerrain2Render * pTer2Render = m_pTerrain2->GetTerrainRender();
		int nMesh = pTer2Render->GetRenderMeshNum();

		for (int i = 0; i < nMesh; i++) {
			int nVert, nIndex;
			pTer2Render->GetRenderMesh(i, (BYTE*)m_pReplaceTerrainVerts, sizeof(A3DVECTOR3),
				m_pReplaceTerrainIndices, &nVert, &nIndex);
			if ((nVert <=0)  || (nIndex <= 0) ){
				continue;
			}
			m_pOutlineTexCache->RenderReplaceTerrain(m_dwFrame, m_iMinLevel, m_pReplaceTerrainVerts, nVert,
				m_pReplaceTerrainIndices, nIndex);
		}

		m_pDetailTexture->Disappear(iLayer);
		m_pA3DDevice->SetTextureColorOP(iLayer, prevOp);
		m_pA3DDevice->SetTextureCoordIndex(iLayer, iLayer);
		m_pA3DDevice->SetTextureTransformFlags(iLayer, A3DTTFF_DISABLE);
	}
	else
	{
		//make sure buffer is ready
		assert((m_pReplaceTerrainIndices != NULL) &&(m_pReplaceTerrainVerts != NULL));
		A3DTerrain2Render * pTer2Render = m_pTerrain2->GetTerrainRender();
		int nMesh = pTer2Render->GetRenderMeshNum();

		A3DCOLORVALUE clFogOld, clFogNew;
		clFogOld = clFogNew = m_pA3DDevice->GetFogColor();
		clFogNew.r = (float)sqrt(clFogNew.r * 0.5f);
		clFogNew.g = (float)sqrt(clFogNew.g * 0.5f);
		clFogNew.b = (float)sqrt(clFogNew.b * 0.5f);
		m_pA3DDevice->SetFogColor(clFogNew.ToRGBAColor());

		for (int i = 0; i < nMesh; i++) {
			int nVert, nIndex;
			pTer2Render->GetRenderMesh(i, (BYTE*)m_pReplaceTerrainVerts, sizeof(A3DVECTOR3),
				m_pReplaceTerrainIndices, &nVert, &nIndex);
			if ((nVert <=0)  || (nIndex <= 0) ){
				continue;
			}
			// we must enable texture transform process for replace terrain render, 
			// or z precision will be a problem because we use two pass rendering, the second pass
			// will have some z precision fighting agaist with 1st pass because it takes use of texture transform
			m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_COUNT3);
			m_pA3DDevice->SetTextureMatrix(0, IdentityMatrix());

			m_pOutlineTexCache->RenderReplaceTerrain(m_dwFrame, m_iMinLevel, m_pReplaceTerrainVerts, nVert,
				m_pReplaceTerrainIndices, nIndex);

			// render detail pass
			m_pDetailTexture->Appear(0);
			m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_COUNT3);
			m_pA3DDevice->SetTextureMatrix(0, Scaling(128.0f, 128.0f, 128.0f));
			m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
			m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
			m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
			m_pA3DDevice->SetAlphaBlendEnable(true);
			m_pA3DDevice->SetAlphaTestEnable(false);
			m_pA3DDevice->SetZWriteEnable(false);
			m_pA3DDevice->SetSourceAlpha(A3DBLEND_DESTCOLOR);
			m_pA3DDevice->SetDestAlpha(A3DBLEND_SRCCOLOR);
			m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);

			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nVert, 0, nIndex/3);

			m_pDetailTexture->Disappear(0);

			m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
			m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
			m_pA3DDevice->SetZWriteEnable(true);
			m_pA3DDevice->SetAlphaBlendEnable(false);
			m_pA3DDevice->SetAlphaTestEnable(true);
			m_pA3DDevice->SetTextureColorOP(0, A3DTOP_SELECTARG1);
			m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_DIFFUSE);  //diffuse as opaque white, ignore
			m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_DISABLE);
			m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
			m_pA3DDevice->SetTextureColorOP(1, A3DTOP_BLENDFACTORALPHA);
			m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		}

		m_pA3DDevice->SetFogColor(clFogOld.ToRGBAColor());
	}
}



void CELTerrainOutline2::Render2(A3DViewport * pViewport)
{
	if (!pViewport)
	{
		return;
	}
	m_pA3DDevice->ClearPixelShader();
	m_pA3DDevice->ClearVertexShader();

	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());

	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(2, A3DTTFF_DISABLE);


	if(m_fDNFactor < 1.0f && m_fDNFactor > 0.0f)
	{//morning or evening
 	 //@note : texture blend.
	 //        out = day * (1-fDN)  + night * fDN. By Kuiwu[27/8/2005] 
	 //stage 0
	 m_pA3DDevice->SetTextureColorOP(0, A3DTOP_SELECTARG1);
	 m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_DIFFUSE);  //diffuse as opaque white, ignore
	 m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_DISABLE);
	 m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	 
	 //stage 1
	 m_pA3DDevice->SetTextureCoordIndex(1, 0);
	 m_pA3DDevice->SetTextureColorOP(1, A3DTOP_BLENDFACTORALPHA);
	 m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	 m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
	 m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	
	 byte tAlpha = (byte)(m_fDNFactor * 255);
	 A3DCOLOR tFactor = A3DCOLORRGBA(0, 0, 0, tAlpha );
	 m_pA3DDevice->SetTextureFactor(tFactor);
	}


	m_pOutlineTexCache->Render(m_dwFrame, pViewport->GetCamera());
	if (m_bReplaceTerrain) {
		_RenderReplaceTerrain(pViewport);
	}
	
	//set back
	if(m_fDNFactor < 1.0f && m_fDNFactor > 0.0f)
	{//morning or evening
		//@todo :  make sure the default state. By Kuiwu[27/8/2005]
		//stage 0
		m_pA3DDevice->SetTextureColorOP(0,A3DTOP_MODULATE);
		m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
		m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		//stage 1
		m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
		m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
		m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureCoordIndex(1, 1);
	}

	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetLightingEnable(true);
}















