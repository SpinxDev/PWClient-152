       /*
 * FILE: A3DTerrain2LoaderB.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/21
 *
 * HISTORY: 
 *
 *	2005.2: 使用 pixel shader 1.4 来渲染。每个 pass 可以渲染最多 4 层个地形 layers。
 *			这要求加载 mask 时，将 4 层 mask 合并到 1 个 32-bit 位图中，加上 4 张
 *			layer texture 和 1 张 lightmap. 每个渲染 pass 一共使用 6 张 textures.
 *			使用这种方法渲染，要求每一个 layer mask 的大小一致，这一点在做地形的时候
 *			要多加注意!!
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "A3DTerrain2LoaderB.h"
#include "A3DPI.h"
#include "A3DMacros.h"
#include "A3DTerrain2.h"
#include "A3DConfig.h"
#include "A3DTerrain2Blk.h"
#include "A3DTexture.h"
#include "A3DTerrain2Env.h"

#include "AFI.h"
#include "AFilePackage.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

using namespace A3DTRN2LOADERB;

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

static bool Fill32BitTexture(int iMapSize, BYTE* pSrcData, A3DTexture* pTexture)
{
	BYTE* pDstData;
	int iDstPitch;
	if (!pTexture->LockRect(NULL, (void**) &pDstData, &iDstPitch, 0))
	{
		g_A3DErrLog.Log("Fill32BitTexture, Failed to lock texture");
		return false;
	}

	BYTE* pSrc = pSrcData;
	BYTE* pDstLine = pDstData;

	for (int i=0; i < iMapSize; i++)
	{
		BYTE* pDst = pDstLine;
		pDstLine += iDstPitch;
		for (int j=0; j < iMapSize; j++)
		{
			*pDst++ = *pSrc++;
			*pDst++ = *pSrc++;
			*pDst++ = *pSrc++;
			*pDst++ = *pSrc++;
		}
	}

	pTexture->UnlockRect();

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DTrn2SubLoaderB
//	
///////////////////////////////////////////////////////////////////////////

A3DTrn2SubLoaderB::A3DTrn2SubLoaderB(A3DTerrain2LoaderB* pLoaderB)
{
	m_pl	= pLoaderB;
	m_pt	= pLoaderB->GetTerrain();

	m_bHasLightMap	= false;
	m_bHasLightMap1	= false;
	m_bHasColorMap	= false;
	m_iLightMapSize	= 0;
	m_iBlkLoadCnt	= 0;
	m_bActive		= false;
	m_dwLMFileFlags = 0;
	m_iLMDataSize	= 0;
	m_byBlockComp	= 0;
	m_bMaskComp		= false;
	m_dwBlkFileVer	= T2BKFILE_VERSION;
	m_dwMaskFileVer	= T2MKFILE_VERSION;
}

A3DTrn2SubLoaderB::~A3DTrn2SubLoaderB()
{
}

//	Initialize object
bool A3DTrn2SubLoaderB::Init(const char* szBaseName)
{
	m_strBaseName = szBaseName;

	//	Open block data file
	AString strFile = m_strBaseName + ".t2bk";
	if (!LoadBlockDataFile(strFile))
		return false;

	//	Open mask map flie
	strFile = m_strBaseName + ".t2mk";
	if (!LoadMaskDataFile(strFile))
		return false;

	//	Try to open lightmap file for day
	m_bHasLightMap = false;
	strFile = m_strBaseName + ".t2lm";
	if (LoadLightMapFile(strFile, LM_DAY))
		m_bHasLightMap = true;

	//	Try to open lightmap file for night
	m_bHasLightMap1 = false;
	strFile += "1";
	if (LoadLightMapFile(strFile, LM_NIGHT))
		m_bHasLightMap1 = true;

	m_bHasColorMap = false;
	strFile = m_strBaseName + ".t2dm";
	if (LoadLightMapFile(strFile, LM_MANUAL))
		m_bHasColorMap = true;

	m_bActive = true;

	return true;
}

//	Release object
void A3DTrn2SubLoaderB::Release()
{
	m_aBlockOffs.RemoveAll();
	m_aMaskOffs.RemoveAll();
	m_aLightMapOffs.RemoveAll();
	m_aColorMapOffs.RemoveAll();
	m_aLayers.RemoveAll();

	m_BlockFile.Close();
	m_MaskFile.Close();
	m_LightMapFile.Close();
	m_LightMapFile1.Close();
}

//	Activate / Disactivate this sub-terrain
bool A3DTrn2SubLoaderB::Activate(bool bActivate)
{
	if (bActivate)
	{
		AString strFile = m_strBaseName + ".t2bk";
		if (!m_BlockFile.Open("", strFile, AFILE_OPENEXIST | AFILE_BINARY))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::Activate, Failed to open block file %s", strFile);
			return false;
		}

		//	Open mask map flie
		strFile = m_strBaseName + ".t2mk";
		if (!m_MaskFile.Open("", strFile, AFILE_OPENEXIST | AFILE_BINARY))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::Activate, Failed to open mask map file %s", strFile);
			return false;
		}

		//	Try to open lightmap file
		strFile = m_strBaseName + ".t2lm";
		if (m_bHasLightMap)
		{
			//	Light map for day
			if (!m_LightMapFile.Open("", strFile, AFILE_OPENEXIST | AFILE_BINARY))
			{
				g_A3DErrLog.Log("A3DTrn2SubLoaderB::Activate, Failed to open lightmap file %s", strFile);
				return false;
			}
		}

		if (m_bHasLightMap1)
		{
			//	Light map file night
			strFile += "1";
			if (!m_LightMapFile1.Open("", strFile, AFILE_OPENEXIST | AFILE_BINARY))
			{
				g_A3DErrLog.Log("A3DTrn2SubLoaderB::Activate, Failed to open lightmap file %s", strFile);
				return false;
			}
		}
	}
	else
	{
		m_BlockFile.Close();
		m_MaskFile.Close();
		m_LightMapFile.Close();
		m_LightMapFile1.Close();

		m_iBlkLoadCnt = 0;
	}

	m_bActive		= bActivate;
	m_dwWaitTime	= 0;

	return true;
}

//	Open terrain2 block data file
bool A3DTrn2SubLoaderB::LoadBlockDataFile(const char* szFile)
{
	//	Open file
	if (!m_BlockFile.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadBlockDataFile, Failed to open terrain block file %s", szFile);
		return false;
	}

	DWORD dwRead;

	//	Read file version and identify
	TRN2FILEIDVER IdVer;
	if (!m_BlockFile.Read(&IdVer, sizeof (IdVer), &dwRead))
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadBlockDataFile, Failed to read block file header");
		return false;
	}

	if (IdVer.dwIdentify != T2BKFILE_IDENTIFY)
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadBlockDataFile, File format error");
		return false;
	}
	
	if (IdVer.dwVersion > T2BKFILE_VERSION)
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadBlockDataFile, Version %d expected but file's version is %d", T2BKFILE_VERSION, IdVer.dwVersion);
		return false;
	}

	m_dwBlkFileVer = IdVer.dwVersion;

	int iNumBlock = 0;

	if (IdVer.dwVersion < 2)
	{
		//	Read file header
		T2BKFILEHEADER Header;
		if (!m_BlockFile.Read(&Header, sizeof (Header), &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadBlockDataFile, Failed to read block data file header");
			return false;
		}

		iNumBlock		= Header.iNumBlock;
		m_byBlockComp	= 0;
	}
	else if (IdVer.dwVersion < 5)	//	IdVer.dwVersion >= 2 && < 5
	{
		//	Read file header
		T2BKFILEHEADER2 Header;
		if (!m_BlockFile.Read(&Header, sizeof (Header), &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadBlockDataFile, Failed to read block data file header");
			return false;
		}

		iNumBlock		= Header.iNumBlock;
		m_byBlockComp	= Header.bCompressed ? (T2BKCOMP_COL_ZIP | T2BKCOMP_HEI_ZIP | T2BKCOMP_NOR_ZIP) : 0;
	}
	else	//	IdVer.dwVersion >= 5
	{
		//	Read file header
		T2BKFILEHEADER5 Header;
		if (!m_BlockFile.Read(&Header, sizeof (Header), &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadBlockDataFile, Failed to read block data file header");
			return false;
		}

		iNumBlock		= Header.iNumBlock;
		m_byBlockComp	= Header.byCompressed;
	}

	//	Check block number
	int iBlkNum = m_pt->GetSubTerrainGrid() / m_pt->GetBlockGrid();
	ASSERT(iBlkNum * iBlkNum == iNumBlock);

	//	Read block data offsets
	m_aBlockOffs.SetSize(iNumBlock, 100);
	if (!m_BlockFile.Read(m_aBlockOffs.GetData(), iNumBlock * sizeof (DWORD), &dwRead))
	{
		m_aBlockOffs.RemoveAll();
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadBlockDataFile, Failed to load block offset data");
		return false;
	}

	return true;
}

//	Open terrain2 mask data file
bool A3DTrn2SubLoaderB::LoadMaskDataFile(const char* szFile)
{
	//	Open file
	if (!m_MaskFile.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskDataFile, Failed to open terrain mask file %s", szFile);
		return false;
	}

	DWORD dwRead;

	//	Read file version and identify
	TRN2FILEIDVER IdVer;
	if (!m_MaskFile.Read(&IdVer, sizeof (IdVer), &dwRead))
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskDataFile, Failed to read mask file header");
		return false;
	}

	if (IdVer.dwIdentify != T2MKFILE_IDENTIFY)
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskDataFile, File format error");
		return false;
	}
	
	if (IdVer.dwVersion > T2MKFILE_VERSION)
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskDataFile, Version %d expected but file's version is %d", T2MKFILE_VERSION, IdVer.dwVersion);
		return false;
	}

	m_dwMaskFileVer = IdVer.dwVersion;

	int iNumMaskArea, iNumLayer;

	if (IdVer.dwVersion < 2)
	{
		//	Read file header
		T2MKFILEHEADER Header;
		if (!m_MaskFile.Read(&Header, sizeof (Header), &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskDataFile, Failed to read mask data file header");
			return false;
		}

		iNumMaskArea	= Header.iNumMaskArea;
		iNumLayer		= Header.iNumLayer;
		m_bMaskComp		= false;
	}
	else	//	IdVer.dwVersion >= 2
	{
		//	Read file header
		T2MKFILEHEADER2 Header;
		if (!m_MaskFile.Read(&Header, sizeof (Header), &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskDataFile, Failed to read mask data file header");
			return false;
		}

		iNumMaskArea	= Header.iNumMaskArea;
		iNumLayer		= Header.iNumLayer;
		m_bMaskComp		= Header.bCompressed;
	}

	//	Check mask area number
	int iMaskNum = m_pt->GetSubTerrainGrid() / m_pt->GetMaskGrid();
	ASSERT(iMaskNum * iMaskNum == iNumMaskArea);

	//	Load mask offsets data
	m_aMaskOffs.SetSize(iNumMaskArea, 100);
	if (!m_MaskFile.Read(m_aMaskOffs.GetData(), iNumMaskArea * sizeof (DWORD), &dwRead))
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskDataFile, Failed to read mask offset data");
		return false;
	}

	//	Load layer information
	for (int i=0; i < iNumLayer; i++)
	{
		T2MKFILELAYER src;
		m_MaskFile.Read(&src, sizeof (src), &dwRead);

		LAYER dst;
		dst.iTextureIdx		= src.iTextureIdx;
		dst.iSpecMapIdx		= src.iSpecMapIdx;
		dst.iMaskMapSize	= src.iMaskMapSize;
		dst.byProjAxis		= src.byProjAxis;
		dst.fUScale			= src.fUScale;
		dst.fVScale			= src.fVScale;
		dst.byWeight		= src.byWeight;

		m_aLayers.Add(dst);
	}

	return true;
}

//	Open terrain2 light map data file
bool A3DTrn2SubLoaderB::LoadLightMapFile(const char* szFile, int iLMType)
{
	if (m_pt->GetNoRenderFlag())
		return true;

	AFile* pLMFile;
	AArray<DWORD, DWORD>* aOffsets;

	switch (iLMType)
	{
	case LM_NIGHT:
		pLMFile = &m_LightMapFile1;
		aOffsets = &m_aLightMapOffs1;
		break;
	case LM_DAY:
		pLMFile = &m_LightMapFile;
		aOffsets = &m_aLightMapOffs;
		break;
	case LM_MANUAL:
		pLMFile = &m_ColorMapFile;
		aOffsets = &m_aColorMapOffs;
		break;
	default:
		return false;
	}

	//	Open file
	if (!pLMFile->Open(szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadLightMapFile, Failed to open lightmap file %s", szFile);
		return false;
	}
	
	DWORD dwRead;

	//	Read file version and identify
	TRN2FILEIDVER IdVer;
	if (!pLMFile->Read(&IdVer, sizeof (IdVer), &dwRead))
	{
		pLMFile->Close();
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadLightMapFile, Failed to read light map file header");
		return false;
	}

	if (IdVer.dwIdentify != T2LMFILE_IDENTIFY)
	{
		pLMFile->Close();
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadLightMapFile, File format error");
		return false;
	}
	
	if (IdVer.dwVersion > T2LMFILE_VERSION)
	{
		pLMFile->Close();
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadLightMapFile, Version %d expected but file's version is %d", T2LMFILE_VERSION, IdVer.dwVersion);
		return false;
	}

	int iNumMaskArea = 0;

	if (IdVer.dwVersion < 2)
	{
		//	Read lightmap file header
		T2LMFILEHEADER Header;
		if (!pLMFile->Read(&Header, sizeof (Header), &dwRead))
		{
			pLMFile->Close();
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadLightMapFile, Failed to read lightmap file header");
			return false;
		}

		iNumMaskArea	= Header.iNumMaskArea;
		m_iLightMapSize = Header.iMaskLMSize;
		m_dwLMFileFlags	= 0;
		m_iLMDataSize	= m_iLightMapSize * m_iLightMapSize * 3;
	}
	else
	{
		//	Read lightmap file header
		T2LMFILEHEADER2 Header;
		if (!pLMFile->Read(&Header, sizeof (Header), &dwRead))
		{
			pLMFile->Close();
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadLightMapFile, Failed to read lightmap file header");
			return false;
		}

		iNumMaskArea	= Header.iNumMaskArea;
		m_iLightMapSize = Header.iMaskLMSize;
		m_dwLMFileFlags	= Header.dwFlags;
		m_iLMDataSize	= Header.iLMDataSize;
	}

	//	Check mask area number
	int iMaskNum = m_pt->GetSubTerrainGrid() / m_pt->GetMaskGrid();
	ASSERT(iMaskNum * iMaskNum == iNumMaskArea);

	//	Load light map data offsets
	aOffsets->SetSize(iNumMaskArea, 100);
	if (!pLMFile->Read(aOffsets->GetData(), iNumMaskArea * sizeof (DWORD), &dwRead))
	{
		pLMFile->Close();
		aOffsets->RemoveAll();
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadLightMapFile, Failed to read lightmap offsets");
		return false;
	}

	return true;
}

//	Load block
//	iBlock: block index in this sub-terrain
A3DTerrain2Block* A3DTrn2SubLoaderB::LoadBlock(int iBlock, float sx, float sz)
{
	if (!m_bActive)
	{
		//	Activate loader
		if (!Activate(true))
			return NULL;
	}

	//	Create a A3DTerrain2Block object
	A3DTerrain2Block* pBlock = new A3DTerrain2Block(m_pt);
	if (!pBlock)
		return NULL;

	if (!pBlock->Init())
		return NULL;
	
	//	Move file pointer
	m_BlockFile.Seek(m_aBlockOffs[iBlock], AFILE_SEEK_SET);

	if (!ReadBlockData(pBlock, sx, sz))
	{
		pBlock->Release();
		delete pBlock;
		return NULL;
	}

	m_iBlkLoadCnt++;

	return pBlock;
}

//	Unload a block
void A3DTrn2SubLoaderB::UnloadBlock(A3DTerrain2Block* pBlock)
{
	ASSERT(pBlock);
	pBlock->Release();
	delete pBlock;

	m_iBlkLoadCnt--;

	if (!m_iBlkLoadCnt)
		m_dwWaitTime = 0;	//	Reset time counter
}

//	Read block data from file
bool A3DTrn2SubLoaderB::ReadBlockData(A3DTerrain2Block* pBlock, float sx, float sz)
{
	DWORD dwRead;
	T2BKFILEBLOCK6 BlockInfo;

	if (m_dwBlkFileVer < 2)
	{
		T2BKFILEBLOCK Info1;

		//	Load block information
		if (!m_BlockFile.Read(&Info1, sizeof (Info1), &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to read block info.");
			return false;
		}

		memcpy(&BlockInfo, &Info1, sizeof (Info1));

		BlockInfo.dwHeiSize		= pBlock->m_iNumVert * sizeof (float);
		BlockInfo.dwNormalSize	= pBlock->m_iNumVert * 3 * sizeof (float);
		BlockInfo.dwDiffSize	= 0;
		BlockInfo.dwBlkFlags	= T2BKFLAG_DEFAULT;
	}
	else if (m_dwBlkFileVer < 3)
	{
		T2BKFILEBLOCK2 Info2;

		//	Load block information
		if (!m_BlockFile.Read(&Info2, sizeof (Info2), &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to read block info.");
			return false;
		}

		memcpy(&BlockInfo, &Info2, sizeof (Info2));

		BlockInfo.dwDiffSize	= 0;
		BlockInfo.dwBlkFlags	= T2BKFLAG_DEFAULT;
	}
	else if (m_dwBlkFileVer < 4)
	{
		T2BKFILEBLOCK3 Info3;

		//	Load block information
		if (!m_BlockFile.Read(&Info3, sizeof (Info3), &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to read block info.");
			return false;
		}

		memcpy(&BlockInfo, &Info3, sizeof (Info3));

		BlockInfo.dwDiffSize1	= 0;
		BlockInfo.dwBlkFlags	= T2BKFLAG_DEFAULT;
	}
	else if (m_dwBlkFileVer < 6)	//	m_dwBlkFileVer >= 4 && < 6
	{
		T2BKFILEBLOCK4 Info4;
		//	Load block information
		if (!m_BlockFile.Read(&Info4, sizeof (Info4), &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to read block info.");
			return false;
		}

		memcpy(&BlockInfo, &Info4, sizeof(Info4));

		BlockInfo.dwBlkFlags	= T2BKFLAG_DEFAULT;
	}
	else	//	m_dwBlkFileVer >= 6
	{
		if (!m_BlockFile.Read(&BlockInfo, sizeof(BlockInfo), &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to read block info.");
			return false;
		}
	}

	pBlock->m_iMaskArea		= BlockInfo.iMaskArea;
	pBlock->m_iRowInTrn		= BlockInfo.iRowInTrn;
	pBlock->m_iColInTrn		= BlockInfo.iColInTrn;
//	pBlock->m_fLODScale		= BlockInfo.fLODScale;
	pBlock->m_fLODScale		= 1.0f;
	pBlock->m_dwLayerFlags	= BlockInfo.dwLayerFlags;
	pBlock->m_iLODLevel		= 3;
	pBlock->m_dwLODFlag		= 0;
	pBlock->m_iFirstLayer	= 0;
	pBlock->m_dwBlockFlags	= BlockInfo.dwBlkFlags;
	
	//	Search the first layer which effect this block
	for (int i=0; i < 32; i++)
	{
		if (pBlock->m_dwLayerFlags & (1 << i))
		{
			pBlock->m_iFirstLayer = i;
			break;
		}
	}

	//	Do some calculation
	int iNumBlock = m_pt->GetMaskGrid() / m_pt->GetBlockGrid();
	int iRow = pBlock->m_iRowInTrn / iNumBlock;
	int iCol = pBlock->m_iColInTrn / iNumBlock;
	pBlock->m_iRowInMask = pBlock->m_iRowInTrn % iNumBlock;
	pBlock->m_iColInMask = pBlock->m_iColInTrn % iNumBlock;

	iNumBlock = m_pt->GetSubTerrainGrid() / m_pt->GetBlockGrid();
	iRow = pBlock->m_iRowInTrn / iNumBlock;
	iCol = pBlock->m_iColInTrn / iNumBlock;
	pBlock->m_iSubTerrain = iRow * (m_pt->GetBlockColNum() / iNumBlock) + iCol;

	A3DTerrain2::TEMPBUFS& tbuf = m_pt->GetTempBuffers();
	ASSERT(tbuf.iNumVert == pBlock->m_iNumVert);
	ASSERT(tbuf.aTempHei);
	ASSERT(tbuf.aTempNormal);
	ASSERT(tbuf.aTempDiff);
	ASSERT(tbuf.aTempDiff1);

	//	Load block height and normal data
	if (1)
	{
		BYTE *pHeiData=NULL, *pNormalData=NULL, *pHeiData2=NULL, *pNormalData2=NULL;
		if (m_byBlockComp)
		{
			if (!m_pl->GetBlockTempBuffers(&pHeiData, &pNormalData, NULL, &pHeiData2, &pNormalData2))
			{
				g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to get buffers.");
				return false;
			}

			ASSERT(pHeiData && pNormalData);
		}

		//	Load block height data ...
		if ((m_byBlockComp & T2BKCOMP_HEI_ZIP) || (m_byBlockComp & T2BKCOMP_HEI_INC))
		{
			//	Load block height data
			if (!m_BlockFile.Read(pHeiData, BlockInfo.dwHeiSize, &dwRead))
			{
				g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to read block height data.");
				return false;
			}

			if (!(m_byBlockComp & T2BKCOMP_HEI_INC))
			{
				DWORD dwSize = tbuf.iNumVert * sizeof (float);
				AFilePackage::Uncompress(pHeiData, BlockInfo.dwHeiSize, (BYTE*)tbuf.aTempHei, &dwSize);
				ASSERT(dwSize == pBlock->m_iNumVert * sizeof (float));
			}
			else if (!(m_byBlockComp & T2BKCOMP_HEI_ZIP))
			{
				DWORD dwSize = A3DTerrain2::DecompressHeightMap(pHeiData, tbuf.aTempHei);
				ASSERT(dwSize == pBlock->m_iNumVert * sizeof (float));
			}
			else	//	(m_byBlockComp & T2BKCOMP_HEI_ZIP) && (m_byBlockComp & T2BKCOMP_HEI_INC)
			{
				DWORD dwSize = pBlock->m_iNumVert * sizeof (float);
				AFilePackage::Uncompress(pHeiData, BlockInfo.dwHeiSize, (BYTE*)pHeiData2, &dwSize);
				dwSize = m_pt->DecompressHeightMap(pHeiData2, tbuf.aTempHei);
				ASSERT(dwSize == pBlock->m_iNumVert * sizeof (float));
			}
		}
		else	//	No compression
		{
			//	Load block height data
			if (!m_BlockFile.Read(tbuf.aTempHei, BlockInfo.dwHeiSize, &dwRead))
			{
				g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to read block height data.");
				return false;
			}
		}

		//	Load block normal data ...
		if ((m_byBlockComp & T2BKCOMP_NOR_ZIP) || (m_byBlockComp & T2BKCOMP_NOR_DEG))
		{
			//	Load block normal data
			if (!m_BlockFile.Read(pNormalData, BlockInfo.dwNormalSize, &dwRead))
			{
				g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to read block normal data.");
				return false;
			}

			if (!(m_byBlockComp & T2BKCOMP_NOR_DEG))
			{
				DWORD dwSize = tbuf.iNumVert * 3 * sizeof (float);
				AFilePackage::Uncompress(pNormalData, BlockInfo.dwNormalSize, (BYTE*)tbuf.aTempNormal, &dwSize);
				ASSERT(dwSize == pBlock->m_iNumVert * 3 * sizeof (float));
			}
			else if (!(m_byBlockComp & T2BKCOMP_NOR_ZIP))
			{
				DWORD dwSize = A3DTerrain2::DecompressNormalMap(pNormalData, tbuf.aTempNormal);
				ASSERT(dwSize == pBlock->m_iNumVert * 3 * sizeof (float));
			}
			else	//	(m_byBlockComp & T2BKCOMP_NOR_ZIP) || (m_byBlockComp & T2BKCOMP_NOR_DEG)
			{
				DWORD dwSize = tbuf.iNumVert * 3 * sizeof (float);
				AFilePackage::Uncompress(pNormalData, BlockInfo.dwNormalSize, (BYTE*)pNormalData2, &dwSize);
				dwSize = A3DTerrain2::DecompressNormalMap(pNormalData2, tbuf.aTempNormal);
				ASSERT(dwSize == pBlock->m_iNumVert * 3 * sizeof (float));
			}
		}
		else	//	No compression
		{
			if (!m_BlockFile.Read(tbuf.aTempNormal, BlockInfo.dwNormalSize, &dwRead))
			{
				g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to read block normal data.");
				return false;
			}
		}
	}

	//	Read vertex diffuse color data
	if (!m_pt->UseLightmapTech())
	{
		if (!(m_byBlockComp & T2BKCOMP_COL_ZIP))
		{
			if (m_dwBlkFileVer >= 3 && BlockInfo.dwDiffSize)
			{
				if (!m_BlockFile.Read(tbuf.aTempDiff, BlockInfo.dwDiffSize, &dwRead))
				{
					g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to read block diffuse data.");
					return false;
				}
			}
			else
			{
				memset(tbuf.aTempDiff, 0xff, tbuf.iNumVert * sizeof (DWORD));
			}

			if (m_dwBlkFileVer >= 4 && BlockInfo.dwDiffSize1)
			{
				if (!m_BlockFile.Read(tbuf.aTempDiff1, BlockInfo.dwDiffSize1, &dwRead))
				{
					g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to read block diffuse data.");
					return false;
				}
			}
			else
			{
				memset(tbuf.aTempDiff1, 0xff, tbuf.iNumVert * sizeof (DWORD));
			}
		}
		else
		{
			BYTE* pDiffData=NULL;
			if (BlockInfo.dwDiffSize || BlockInfo.dwDiffSize1)
			{
				if (!m_pl->GetBlockTempBuffers(NULL, NULL, &pDiffData, NULL, NULL))
				{
					g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to get buffers.");
					return false;
				}
			}

			//	Diffuse color data for day
			if (m_dwBlkFileVer >= 3 && BlockInfo.dwDiffSize)
			{
				if (!m_BlockFile.Read(pDiffData, BlockInfo.dwDiffSize, &dwRead))
				{
					g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to read block diffuse data.");
					return false;
				}

				//	Decompress data
				DWORD dwSize = tbuf.iNumVert * sizeof (DWORD);
				AFilePackage::Uncompress(pDiffData, BlockInfo.dwDiffSize, (BYTE*)tbuf.aTempDiff, &dwSize);
				ASSERT(dwSize == pBlock->m_iNumVert * sizeof (DWORD));
			}

			//	Diffuse color data for night
			if (m_dwBlkFileVer >= 4 && BlockInfo.dwDiffSize1)
			{
				if (!m_BlockFile.Read(pDiffData, BlockInfo.dwDiffSize1, &dwRead))
				{
					g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to read block diffuse data.");
					return false;
				}

				//	Decompress data
				DWORD dwSize = tbuf.iNumVert * sizeof (DWORD);
				AFilePackage::Uncompress(pDiffData, BlockInfo.dwDiffSize1, (BYTE*)tbuf.aTempDiff1, &dwSize);
				ASSERT(dwSize == pBlock->m_iNumVert * sizeof (DWORD));
			}
		}
	}

	//	Fill stream vertex buffer
	if (!pBlock->FillVertexBuffer(sx, sz))
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::ReadBlockData, Failed to fill vertex buffer.");
		return false;
	}

	//	Complete aabb
	pBlock->m_aabbBlock.CompleteCenterExts();

	pBlock->m_bDataLoaded = true;

	return true;
}

A3DTerrain2Mask* A3DTrn2SubLoaderB::CreateTerrain2Mask(A3DTerrain2* pTrn, int iMaskArea) const
{
	//	Create new mask
	return new A3DTerrain2Mask(m_pt, iMaskArea);
}

//	Load a mask area
A3DTerrain2Mask* A3DTrn2SubLoaderB::LoadMask(int iMaskArea, int iIdxInSub)
{
	A3DTerrain2Mask* pMask = CreateTerrain2Mask(m_pt, iMaskArea);
	if (!pMask)
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMask, not enough memory !");
		return NULL;
	}

	//	Move file pointer
	m_MaskFile.Seek((int)m_aMaskOffs[iIdxInSub], AFILE_SEEK_SET);

	DWORD dwRead;
	T2MKFILEMASKAREA MaskArea;

	//	Load mask information
	if (!m_MaskFile.Read(&MaskArea, sizeof (MaskArea), &dwRead))
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMask, Failed to read mask area info.");
		return NULL;
	}

	ASSERT(MaskArea.iNumLayer > 0);
	pMask->Init(MaskArea.iNumLayer);

	if (m_dwMaskFileVer < 2)
	{
		//	Read mask layer information
		T2MKFILEMASKLAYER* aMaskLayers = new T2MKFILEMASKLAYER [MaskArea.iNumLayer];
		if (!aMaskLayers)
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMask, Not enough memory!");
			return NULL;
		}

		m_MaskFile.Read(aMaskLayers, MaskArea.iNumLayer * sizeof (T2MKFILEMASKLAYER), &dwRead);

		BYTE* aMaskBufs[3] = {NULL, NULL, NULL};

		//	Load textures and mask textures
		A3DTexture* pTexture;
		int i = 0;
		for (i=0; i < MaskArea.iNumLayer; i++)
		{
			const T2MKFILEMASKLAYER& Info = aMaskLayers[i];
			const LAYER& src = m_aLayers[Info.iLayer];

			//	Fill layer properties
			A3DTerrain2Mask::LAYER& dst = pMask->GetLayer(i);
			dst.iTextureIdx		= src.iTextureIdx;
			dst.iSpecMapIdx		= src.iSpecMapIdx;
			dst.iMaskMapSize	= src.iMaskMapSize;
			dst.byProjAxis		= src.byProjAxis;
			dst.fUScale			= src.fUScale;
			dst.fVScale			= src.fVScale;
			dst.byWeight		= src.byWeight;
			dst.dwMapDataOff	= Info.dwMapDataOff;
			dst.dwDataSize		= 0;

			//	Normal texture
			pTexture = m_pt->LoadTexture(dst.iTextureIdx);
			pMask->m_aTextures.Add(pTexture);

			//	Specular texture
		//	pTexture = m_pt->LoadTexture(dst.iSpecMapIdx);
		//	pMask->m_aSpecularMaps.Add(pTexture);

			if (A3D::g_pA3DTerrain2Env->GetSupportPSFlag())	//	Using PS
			{
				//	Load mask data
				LoadMaskMapData(pMask, i, aMaskBufs);

				if ((i % 3) == 2)
				{
					pTexture = BuildMaskTexture(3, aMaskBufs);
					pMask->m_aMaskTextures.Add(pTexture);
				}
			}
			else	//	Not use PS
			{
				//	Load mask map
				if (!i)
					pMask->m_aMaskTextures.Add(NULL);
				else
				{
					pTexture = LoadMaskMap(pMask, i);
					pMask->m_aMaskTextures.Add(pTexture);
				}
			}
		}

		delete [] aMaskLayers;

		if (A3D::g_pA3DTerrain2Env->GetSupportPSFlag())	//	Using PS
		{
			int iTailNum = i % 3;
			if (iTailNum)
			{
				pTexture = BuildMaskTexture(iTailNum, aMaskBufs);
				pMask->m_aMaskTextures.Add(pTexture);
			}
		}
	}
	else 	//	m_dwMaskFileVer >= 2
	{
		//	Read mask layer information
		T2MKFILEMASKLAYER2* aMaskLayers = new T2MKFILEMASKLAYER2 [MaskArea.iNumLayer];
		if (!aMaskLayers)
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMask, Not enough memory!");
			return NULL;
		}

		m_MaskFile.Read(aMaskLayers, MaskArea.iNumLayer * sizeof (T2MKFILEMASKLAYER2), &dwRead);

		BYTE* aMaskBufs[3] = {NULL, NULL, NULL};

		//	Load textures and mask textures
		A3DTexture* pTexture;
		int i = 0;
		for (i=0; i < MaskArea.iNumLayer; i++)
		{
			const T2MKFILEMASKLAYER2& Info = aMaskLayers[i];
			const LAYER& src = m_aLayers[Info.iLayer];

			//	Fill layer properties
			A3DTerrain2Mask::LAYER& dst = pMask->GetLayer(i);
			dst.iTextureIdx		= src.iTextureIdx;
			dst.iSpecMapIdx		= src.iSpecMapIdx;
			dst.iMaskMapSize	= src.iMaskMapSize;
			dst.byProjAxis		= src.byProjAxis;
			dst.fUScale			= src.fUScale;
			dst.fVScale			= src.fVScale;
			dst.byWeight		= src.byWeight;
			dst.dwMapDataOff	= Info.dwMapDataOff;
			dst.dwDataSize		= Info.dwDataSize;

			//	Normal texture
			pTexture = m_pt->LoadTexture(dst.iTextureIdx);
			pMask->m_aTextures.Add(pTexture);

			//	Specular texture
		//	pTexture = m_pt->LoadTexture(dst.iSpecMapIdx);
		//	pMask->m_aSpecularMaps.Add(pTexture);

			if (m_dwMaskFileVer < 3)
			{
				if (A3D::g_pA3DTerrain2Env->GetSupportPSFlag())	//	Using PS
				{
					//	Load mask data
					LoadMaskMapData(pMask, i, aMaskBufs);

					if ((i % 3) == 2)
					{
						pTexture = BuildMaskTexture(3, aMaskBufs);
						pMask->m_aMaskTextures.Add(pTexture);
					}
				}
				else	//	Not use PS
				{
					//	Load mask map
					if (!i)
						pMask->m_aMaskTextures.Add(NULL);
					else
					{
						pTexture = LoadMaskMap(pMask, i);
						pMask->m_aMaskTextures.Add(pTexture);
					}
				}
			}
			else
			{
				//////////////////////////////////////////////////////////////////////////
				//	mask file version >= 3
				//////////////////////////////////////////////////////////////////////////
			}
		}

		delete [] aMaskLayers;

		if (m_dwMaskFileVer < 3)
		{
			if (A3D::g_pA3DTerrain2Env->GetSupportPSFlag())	//	Using PS
			{
				int iTailNum = i % 3;
				if (iTailNum)
				{
					pTexture = BuildMaskTexture(iTailNum, aMaskBufs);
					pMask->m_aMaskTextures.Add(pTexture);
				}
			}
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			//	mask file version >= 3
			//////////////////////////////////////////////////////////////////////////

			//	Load mask texture directly from file
			int iNumMaskTex = (MaskArea.iNumLayer + 2) / 3;
			LoadMaskTextures(&m_MaskFile, iNumMaskTex, pMask);
		}
	}

	//	Load mask lightmap for day
	if (m_bHasLightMap)
	{
		A3DTexture* pTexture;
		if (m_dwLMFileFlags & T2LMFILE_DDS)
			pTexture = LoadMaskLightMap_dds(iIdxInSub, LM_DAY);
		else
			pTexture = LoadMaskLightMap(iIdxInSub, LM_DAY);

		pMask->m_pLMTexture = pTexture;
		pMask->m_bOwnLM = true;
	}
	else
	{
		pMask->m_pLMTexture	= A3D::g_pA3DTerrain2Env->GetWhiteTexture();
		pMask->m_bOwnLM = false;
	}

	//	Load mask lightmap for night
	if (m_bHasLightMap1)
	{
		A3DTexture* pTexture;
		if (m_dwLMFileFlags & T2LMFILE_DDS)
			pTexture = LoadMaskLightMap_dds(iIdxInSub, LM_NIGHT);
		else
			pTexture = LoadMaskLightMap(iIdxInSub, LM_NIGHT);

		pMask->m_pLMTexture1 = pTexture;
		pMask->m_bOwnLM1 = true;
	}
	else
	{
		pMask->m_pLMTexture1 = A3D::g_pA3DTerrain2Env->GetWhiteTexture();
		pMask->m_bOwnLM1 = false;
	}

	//	Merge texture if necessary
	if (m_pt->IsTexMergeUsed())
	{
		A3DRenderTarget* pRenderTarget = m_pt->GetTextureMerger()->MergeMaskTextures(pMask);
		pMask->m_pMergedTex = pRenderTarget;
	}

	if (m_bHasColorMap)
	{
		A3DTexture* pTexture;
		if (m_dwLMFileFlags & T2LMFILE_DDS)
			pTexture = LoadMaskLightMap_dds(iIdxInSub, LM_MANUAL);
		else
			pTexture = LoadMaskLightMap(iIdxInSub, LM_MANUAL);

		pMask->m_pColTexture = pTexture;
	}

	return pMask;
}

//	Unload a mask area
void A3DTrn2SubLoaderB::UnloadMask(A3DTerrain2Mask* pMaskArea)
{
	ASSERT(pMaskArea);
	pMaskArea->Release();
	delete pMaskArea;
}

//	Load mask map data from file
bool A3DTrn2SubLoaderB::LoadMaskMapData(A3DTerrain2Mask* pMask, int iLayer, BYTE* aMaskBufs[3])
{
	int iMaskBufIdx = iLayer % 3;

	if (!iLayer)
	{
		//	First layer always have pure white mask
		aMaskBufs[iMaskBufIdx] = m_pl->m_aMaskBuf[A3DTerrain2LoaderB::MASK_ONE];
		return true;
	}

	A3DTerrain2Mask::LAYER& Layer = pMask->GetLayer(iLayer);

	//	Non-first layer shouldn't have 0 size mask map
	int iMapSize = Layer.iMaskMapSize;
	ASSERT(iMapSize && iMapSize == m_pl->m_iMaskMapSize);
	
	//	Seek to mask map data
	m_MaskFile.Seek((int)Layer.dwMapDataOff, AFILE_SEEK_SET);

	//	Load mask map data
	BYTE* pDataBuf = m_pl->m_aMaskBuf[A3DTerrain2LoaderB::MASK_L0+iMaskBufIdx];
	ASSERT(pDataBuf);

	if (m_bMaskComp)
	{
		DWORD dwRead, dwSize = iMapSize * iMapSize;
		BYTE* pTempBuf = m_pl->GetMaskMapTempBuffer();
		if (!pTempBuf)
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskMapData, Failed to get temp buffer !");
			return false;
		}

		if (!m_MaskFile.Read(pTempBuf, Layer.dwDataSize, &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskMapData, Failed to read mask data !");
			return false;
		}

		AFilePackage::Uncompress(pTempBuf, Layer.dwDataSize, pDataBuf, &dwSize);
		ASSERT(dwSize == iMapSize * iMapSize);
	}
	else
	{
		DWORD dwRead;
		if (!m_MaskFile.Read(pDataBuf, iMapSize * iMapSize, &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskMapData, Failed to read mask data !");
			return false;
		}
	}

	aMaskBufs[iMaskBufIdx] = pDataBuf;

	return true;
}

//	Build 32-bit mask map from 4 8-bit mask maps
A3DTexture* A3DTrn2SubLoaderB::BuildMaskTexture(int iNumLayer, BYTE* aMaskBufs[3])
{
	A3DTexture* pTexture = new A3DTexture;
	if (!pTexture)
		return NULL;

	ASSERT(m_pl->m_iMaskMapSize);

	//	Create 32-bit mask texture
	if (!pTexture->Create(m_pt->GetA3DDevice(), m_pl->m_iMaskMapSize, m_pl->m_iMaskMapSize, A3DFMT_A8R8G8B8, 1))
	{
		delete pTexture;
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::BuildMaskTexture, Failed to create mask texture.");
		return NULL;
	}

	int i, j;
	for (i=iNumLayer; i < 3; i++)
		aMaskBufs[i] = m_pl->m_aMaskBuf[A3DTerrain2LoaderB::MASK_ZERO];
	
	//	Lock texture
	BYTE* pDstData;
	int iDstPitch;
	if (!pTexture->LockRect(NULL, (void**) &pDstData, &iDstPitch, 0))
	{
		delete pTexture;
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::BuildMaskTexture, Failed to lock texture");
		return NULL;
	}

	//	Fill texture data
	BYTE* pSrc0 = aMaskBufs[0];
	BYTE* pSrc1 = aMaskBufs[1];
	BYTE* pSrc2 = aMaskBufs[2];
//	BYTE* pSrc3 = aMaskBufs[3];
	BYTE* pDstLine = pDstData;

	for (i=0; i < m_pl->m_iMaskMapSize; i++)
	{
		BYTE* pDst = pDstLine;
		pDstLine += iDstPitch;
		for (j=0; j < m_pl->m_iMaskMapSize; j++)
		{
			*pDst++ = *pSrc0++;
			*pDst++ = *pSrc1++;
			*pDst++ = *pSrc2++;
		//	*pDst++ = *pSrc3++;
			*pDst++ = 0xff;
		}
	}

	pTexture->UnlockRect();

	return pTexture;
}

//	Load mask textures from mask file
bool A3DTrn2SubLoaderB::LoadMaskTextures(AFile* pFile, int iNumMaskTex, A3DTerrain2Mask* pMask)
{
	int i;
	DWORD dwRead;

	for (i=0; i < iNumMaskTex; i++)
	{
		//	Get data size
		DWORD dwDataSize = 0;
		if (!pFile->Read(&dwDataSize, sizeof (DWORD), &dwRead))
			return false;

		BYTE* pData = m_pl->GetMaskMapTempBuffer(dwDataSize);
		if (!pData)
		{
			g_A3DErrLog.Log("%s, failed to create buffer for mask texture data !", __FUNCTION__);
			return false;
		}

		//	Read texture data
		DWORD dwOffInFile = pFile->GetPos();
		if (!pFile->Read(pData, dwDataSize, &dwRead) || dwRead != dwDataSize)
		{
			g_A3DErrLog.Log("%s, failed to load mask texture data !", __FUNCTION__);
			return false;
		}

		//	Create texture
		A3DTexture* pTexture = new A3DTexture;
		if (!pTexture || !pTexture->LoadFromMemory(m_pt->GetA3DDevice(), pData, (int)dwDataSize, 0, 0, A3DFMT_UNKNOWN, 0))
		{
			A3DRELEASE(pTexture);
			g_A3DErrLog.Log("%s, failed to create mask texture !", __FUNCTION__);
			return false;
		}

		//	Add texture to reload table so that it can be controlled by texture quality
		//	Note: Decrease mask texture size may cause boundary line appear between mask areas !
		//	pTexture->SetMapFile(pFile->GetFileName());
		//	pTexture->SetReloadInfo(dwOffInFile, dwDataSize);
		//	m_pt->GetA3DEngine()->GetA3DTextureMan()->AddToReloadTable(pTexture);

		pMask->m_aMaskTextures.Add(pTexture);
	}

	return true;
}

//	Load mask map data and create mask map texture, non-ps version
A3DTexture* A3DTrn2SubLoaderB::LoadMaskMap(A3DTerrain2Mask* pMask, int iLayer)
{
	if (!iLayer)
	{
		//	First layer needn't mask map
		return NULL;
	}

	A3DTerrain2Mask::LAYER& Layer = pMask->GetLayer(iLayer);

	//	Non-first layer shouldn't have 0 size mask map
	int iMapSize = Layer.iMaskMapSize;
	ASSERT(iMapSize && iMapSize == m_pl->m_iMaskMapSize);
	
	//	Seek to mask map data
	m_MaskFile.Seek((int)Layer.dwMapDataOff, AFILE_SEEK_SET);

	//	Load mask map data
	BYTE* pDataBuf = m_pl->m_aMaskBuf[A3DTerrain2LoaderB::MASK_L0];
	ASSERT(pDataBuf);

	if (m_bMaskComp)
	{
		DWORD dwRead, dwSize = iMapSize * iMapSize;
		BYTE* pTempBuf = m_pl->GetMaskMapTempBuffer();
		if (!pTempBuf)
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskMap, Failed to get temp buffer !");
			return NULL;
		}

		if (!m_MaskFile.Read(pTempBuf, Layer.dwDataSize, &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskMap, Failed to read mask data !");
			return NULL;
		}

		AFilePackage::Uncompress(pTempBuf, Layer.dwDataSize, pDataBuf, &dwSize);
		ASSERT(dwSize == iMapSize * iMapSize);
	}
	else
	{
		DWORD dwRead;
		if (!m_MaskFile.Read(pDataBuf, iMapSize * iMapSize, &dwRead))
		{
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskMap, Failed to read mask data !");
			return NULL;
		}
	}

	A3DTexture* pTexture = new A3DTexture;
	if (!pTexture)
		return NULL;

	//	Create mask texture of specified format
	A3DFORMAT fmt = A3D::g_pA3DTerrain2Env->GetMaskMapFormat();
	if (!pTexture->Create(m_pt->GetA3DDevice(), iMapSize, iMapSize, fmt, 1))
	{
		delete pTexture;
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskMap, Failed to create mask texture.");
		return NULL;
	}

	//	Fill texture data
	if (!m_pl->FillMaskTexture(iMapSize, pDataBuf, fmt, pTexture))
	{
		A3DRELEASE(pTexture);
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskMap, Failed to fill mask texture");
		return NULL;
	}

	return pTexture;
}

//	Load light map of a mask area
//	iMaskArea: mask area index in sub-terrain
A3DTexture* A3DTrn2SubLoaderB::LoadMaskLightMap(int iMaskArea, int iLMType)
{
	if ((iLMType == LM_DAY && !m_bHasLightMap) || (iLMType == LM_NIGHT && !m_bHasLightMap1) || (iLMType == LM_MANUAL && !m_bHasColorMap))
		return NULL;

	BYTE* pLMLoadBuf = m_pl->GetLMLoadBuffer(m_iLightMapSize, m_iLMDataSize);
	if (!pLMLoadBuf)
		return NULL;

	AFile* pLMFile;
	DWORD dwOff;
	switch (iLMType)
	{
	case LM_DAY:
		pLMFile = &m_LightMapFile;
		dwOff = m_aLightMapOffs[iMaskArea];
		break;
	case LM_NIGHT:
		pLMFile = &m_LightMapFile1;
		dwOff = m_aLightMapOffs1[iMaskArea];
		break;
	case LM_MANUAL:
		pLMFile = &m_ColorMapFile;
		dwOff = m_aColorMapOffs[iMaskArea];
		break;
	default:
		return NULL;
	}
	pLMFile->Seek((int)dwOff, AFILE_SEEK_SET);

	//	Load mask lightmap data
	DWORD dwRead;
	if (!pLMFile->Read(pLMLoadBuf, m_iLMDataSize, &dwRead))
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskLightMap, Failed to read light map data !");
		return NULL;
	}

	A3DTexture* pTexture = new A3DTexture;
	if (!pTexture)
		return NULL;

	//	Create light map of specified format
	A3DFORMAT fmt = A3D::g_pA3DTerrain2Env->GetLightMapFormat();
	if (!pTexture->Create(m_pt->GetA3DDevice(), m_iLightMapSize, m_iLightMapSize, fmt, 1))
	{
		delete pTexture;
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskLightMap, Failed to create light map texture.");
		return NULL;
	}

	int iSrcBitsPerPixel = m_iLightMapSize ? m_iLMDataSize * 8 / (m_iLightMapSize * m_iLightMapSize) : 0;

	if (iLMType == LM_MANUAL)
	{
		//	color map (non-dds version) has 32 bit per color pixel in src data
		//	as Light map use X8R8G8B8 format, we need to copy each byte of the color pixel completely
		//	fill texture data
		if (!Fill32BitTexture(m_iLightMapSize, pLMLoadBuf, pTexture))
		{
			A3DRELEASE(pTexture);
			g_A3DErrLog.Log("%s, Failed to fill color map texture", __FUNCTION__);
			return NULL;
		}
	}
	else 
	{
		//	Fill texture data
		if (!m_pl->FillLightMap(m_iLightMapSize, pLMLoadBuf, fmt, pTexture, iSrcBitsPerPixel))
		{
			A3DRELEASE(pTexture);
			g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskLightMap, Failed to fill light map texture");
			return NULL;
		}
	}


	return pTexture;
}

//	Load light map of a mask area, dds version
A3DTexture* A3DTrn2SubLoaderB::LoadMaskLightMap_dds(int iMaskArea, int iLMType)
{
	if ((iLMType == LM_DAY && !m_bHasLightMap) || (iLMType == LM_NIGHT && !m_bHasLightMap1) || (iLMType == LM_MANUAL && !m_bHasColorMap))
		return NULL;

	BYTE* pLMLoadBuf = m_pl->GetLMLoadBuffer(m_iLightMapSize, m_iLMDataSize);
	if (!pLMLoadBuf)
		return NULL;

	AFile* pLMFile;
	DWORD dwOff;
	switch (iLMType)
	{
	case LM_DAY:
		pLMFile = &m_LightMapFile;
		dwOff = m_aLightMapOffs[iMaskArea];
		break;
	case LM_NIGHT:
		pLMFile = &m_LightMapFile1;
		dwOff = m_aLightMapOffs1[iMaskArea];
		break;
	case LM_MANUAL:
		pLMFile = &m_ColorMapFile;
		dwOff = m_aColorMapOffs[iMaskArea];
		break;
	default:
		return NULL;
	}

	pLMFile->Seek((int)dwOff, AFILE_SEEK_SET);

	//	Load mask lightmap data
	DWORD dwRead;
	if (!pLMFile->Read(pLMLoadBuf, m_iLMDataSize, &dwRead))
	{
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskLightMap_dds, Failed to read light map data !");
		return NULL;
	}

	A3DTexture* pTexture = new A3DTexture;
	if (!pTexture)
		return NULL;

	//	Create light map of specified format
	if (!pTexture->LoadFromMemory(m_pt->GetA3DDevice(), pLMLoadBuf, m_iLMDataSize, 0, 0, A3DFMT_UNKNOWN, 1))
	{
		delete pTexture;
		g_A3DErrLog.Log("A3DTrn2SubLoaderB::LoadMaskLightMap_dds, Failed to create light map texture.");
		return NULL;
	}

	return pTexture;
}

//	Tick routine
bool A3DTrn2SubLoaderB::Tick(DWORD dwDeltaTime)
{
	//	If no block was loaded during some time, disactivate this loader
	if (m_bActive && !m_iBlkLoadCnt)
		m_dwWaitTime += dwDeltaTime;

	return true;
}

//	Check activate state
void A3DTrn2SubLoaderB::CheckActivate()
{
	if (m_bActive && !m_iBlkLoadCnt && m_dwWaitTime >= 10000)
		Activate(false);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DTerrain2LoaderB
//	
///////////////////////////////////////////////////////////////////////////

A3DTerrain2LoaderB::A3DTerrain2LoaderB(A3DTerrain2* pTerrain) : 
A3DTerrain2Loader(pTerrain, LOADER_B)
{
	m_pLMLoadBuf	= NULL;
	m_iMaskMapSize	= 0;
	m_iLightMapSize	= 0;
	m_iMaskMapBufSize = 0;
	m_pHeightData	= NULL;
	m_pNormalData	= NULL;
	m_pHeightData2	= NULL;
	m_pNormalData2	= NULL;
	m_pDiffData		= NULL;
	m_pMaskMapData	= NULL;
	m_pColMapData	= NULL;

	memset(m_aMaskBuf, 0, sizeof (m_aMaskBuf));
}

A3DTerrain2LoaderB::~A3DTerrain2LoaderB()
{
}

/*	Initialize object

	Return true for success, otherwise return false.

	szTrn2File: terrain2 general information file name
*/
bool A3DTerrain2LoaderB::Init(const char* szTrn2File)
{
	af_GetFilePath(szTrn2File, m_strPathName);
	m_strPathName += "\\";

	//	Open description file
	if (!LoadDescriptionFile(szTrn2File))
		return false;

	//	Create mask buffers
	if (!CreateMaskBuffers())
		return false;

	//	Allocate sub-terrain loader array
	int iNumSubTrn = m_pt->GetSubTerrainNum();
	m_aSubTrnLoaders.SetSize(iNumSubTrn, 10);
	for (int i=0; i < iNumSubTrn; i++)
		m_aSubTrnLoaders[i] = NULL;

	return true;
}

//	Release object
void A3DTerrain2LoaderB::Release()
{
	int i;

	//	Release sub terrain loaders
	for (i=0; i < m_aSubTrnLoaders.GetSize(); i++)
	{
		A3DRELEASE(m_aSubTrnLoaders[i]);
	}

	m_aSubTrnLoaders.RemoveAll();

	//	Release lightmap loading buffer
	if (m_pLMLoadBuf)
	{
		delete [] m_pLMLoadBuf;
		m_pLMLoadBuf = NULL;
	}

	if (m_pColMapData)
	{
		delete [] m_pColMapData;
		m_pColMapData = NULL;
	}

	//	Release mask loading buffers
	for (i=0; i < NUM_MASKBUF; i++)
	{
		if (m_aMaskBuf[i])
		{
			a_free(m_aMaskBuf[i]);
			m_aMaskBuf[i] = NULL;
		}
	}

	//	Release all sub-terrain names
	for (i=0; i < m_aSubTrnNames.GetSize(); i++)
		delete m_aSubTrnNames[i];

	m_aSubTrnNames.RemoveAll();

	//	Release temporary buffers
	if (m_pHeightData)
	{
		a_free(m_pHeightData);
		m_pHeightData = NULL;
	}

	if (m_pNormalData)
	{
		a_free(m_pNormalData);
		m_pNormalData = NULL;
	}

	if (m_pHeightData2)
	{
		a_free(m_pHeightData2);
		m_pHeightData2 = NULL;
	}

	if (m_pNormalData2)
	{
		a_free(m_pNormalData2);
		m_pNormalData2 = NULL;
	}

	if (m_pDiffData)
	{
		a_free(m_pDiffData);
		m_pDiffData = NULL;
	}
	
	if (m_pMaskMapData)
	{
		a_free(m_pMaskMapData);
		m_pMaskMapData = NULL;
	}

}

//	Load terrain2 description file
bool A3DTerrain2LoaderB::LoadDescriptionFile(const char* szFile)
{
	AFileImage DescFile;
	if (!DescFile.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderB::LoadDescriptionFile, Failed to open file %s", szFile);
		return false;
	}

	//	Skip version and file flag
	DescFile.Seek(sizeof (TRN2FILEIDVER), AFILE_SEEK_SET);
	
	DWORD dwRead;
	TRN2FILEHEADER Header;
	DescFile.Read(&Header, sizeof (Header), &dwRead);

	//	Seek to loader section
	DescFile.Seek(Header.dwLoaderOff, AFILE_SEEK_SET);

	//	Read loader information
	TRN2FILELOADERINFO Info;
	if (!DescFile.Read(&Info, sizeof (Info), &dwRead))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderB::LoadDescriptionFile, Failed to read loader section");
		return false;
	}

	m_iMaskMapSize = Info.iMaskMapSize;
	if (!m_iMaskMapSize)
	{
		//	Only in a case this thing will happen, the case is that
		//	the whole terrain only has one layer (the first layer hasn't mask).
		//	In the case, we assume a mask size (a small value is good)
		m_iMaskMapSize = 32;
	}

	//	Read sub-terrain names
	for (int i=0; i < Header.iNumSubTrn; i++)
	{
		AString* pstr = new AString;
		DescFile.ReadString(*pstr);
		m_aSubTrnNames.Add(pstr);
	}

	DescFile.Close();

	return true;
}

//	Create mask loading buffers
bool A3DTerrain2LoaderB::CreateMaskBuffers()
{
	//	Use pixel shader 1.4 to render 4 layers together, we must sure
	//	that mask map of each layer has same size.
	ASSERT(m_iMaskMapSize);

	for (int i=0; i < NUM_MASKBUF; i++)
	{
		if (!(m_aMaskBuf[i] = (BYTE*)a_malloc(m_iMaskMapSize * m_iMaskMapSize)))
		{
			g_A3DErrLog.Log("A3DTerrain2LoaderB::CreateMaskBuffers, Not enough memory !");
			return false;
		}
	}

	memset(m_aMaskBuf[MASK_ZERO], 0, m_iMaskMapSize * m_iMaskMapSize);
	memset(m_aMaskBuf[MASK_ONE], 0xff, m_iMaskMapSize * m_iMaskMapSize);

	return true;
}

//	Get temporary buffers used to load block data
bool A3DTerrain2LoaderB::GetBlockTempBuffers(BYTE** ppHeiBuf, BYTE** ppNormalBuf, 
						BYTE** ppDiffBuf, BYTE** ppHeiBuf2, BYTE** ppNormalBuf2)
{
	ASSERT(m_pt);

	int iNumVert = (m_pt->m_iBlockGrid + 1) * (m_pt->m_iBlockGrid + 1);

	if (ppHeiBuf)
	{
		if (!m_pHeightData)
		{
			//	* 2 to ensure buffer is large enough
			if (!(m_pHeightData = (BYTE*)a_malloc(iNumVert * sizeof (float) * 2)))
			{
				g_A3DErrLog.Log("A3DTerrain2LoaderB::GetBlockTempBuffers, Not enough memory !");
				return false;
			}
		}

		*ppHeiBuf = m_pHeightData;
	}

	if (ppNormalBuf)
	{
		if (!m_pNormalData)
		{
			//	* 2 to ensure buffer is large enough
			if (!(m_pNormalData = (BYTE*)a_malloc(iNumVert * 3 * sizeof (float) * 2)))
			{
				g_A3DErrLog.Log("A3DTerrain2LoaderB::GetBlockTempBuffers, Not enough memory !");
				return false;
			}
		}

		*ppNormalBuf = m_pNormalData;
	}

	if (ppDiffBuf)
	{
		if (!m_pDiffData)
		{
			//	* 2 to ensure buffer is large enough
			if (!(m_pDiffData = (BYTE*)a_malloc(iNumVert * sizeof (DWORD) * 2)))
			{
				g_A3DErrLog.Log("A3DTerrain2LoaderB::GetBlockTempBuffers, Not enough memory !");
				return false;
			}
		}

		*ppDiffBuf = m_pDiffData;
	}

	if (ppHeiBuf2)
	{
		if (!m_pHeightData2)
		{
			if (!(m_pHeightData2 = (BYTE*)a_malloc(iNumVert * sizeof (float))))
			{
				g_A3DErrLog.Log("A3DTerrain2LoaderB::GetBlockTempBuffers, Not enough memory !");
				return false;
			}
		}

		*ppHeiBuf2 = m_pHeightData2;
	}

	if (ppNormalBuf2)
	{
		if (!m_pNormalData2)
		{
			//	* 2 to ensure buffer is large enough
			if (!(m_pNormalData2 = (BYTE*)a_malloc(iNumVert * 3 * sizeof (float))))
			{
				g_A3DErrLog.Log("A3DTerrain2LoaderB::GetBlockTempBuffers, Not enough memory !");
				return false;
			}
		}

		*ppNormalBuf2 = m_pNormalData2;
	}

	return true;
}

//	Get temporary buffer used to load mask map data
BYTE* A3DTerrain2LoaderB::GetMaskMapTempBuffer(int iSize)
{
	if (m_iMaskMapBufSize < iSize)
	{
		if (m_pMaskMapData)
		{
			a_free(m_pMaskMapData);
			m_pMaskMapData = NULL;
		}

		if (!(m_pMaskMapData = (BYTE*)a_malloc(iSize)))
		{
			g_A3DErrLog.Log("%s, Not enough memory !", __FUNCTION__);
			return NULL;
		}

		memset(m_pMaskMapData, 0, sizeof(BYTE) * iSize);

		m_iMaskMapBufSize = iSize;
	}

	return m_pMaskMapData;
}

//	Load block
A3DTerrain2Block* A3DTerrain2LoaderB::LoadBlock(int r, int c, int iBlock)
{
	//	Block row and column in sub-terrain
	int iSubTrnBlock = m_pt->m_iSubTrnGrid / m_pt->m_iBlockGrid;
	int iSubRow = r % iSubTrnBlock;
	int iSubCol = c % iSubTrnBlock;

	//	Which sub-terrain this block is in ?
	int iRow = r / iSubTrnBlock;
	int iCol = c / iSubTrnBlock;
	int iSubTrn = iRow * (m_pt->m_iNumAllBlockCol / iSubTrnBlock) + iCol;

	//	If sub-terrain doesn't exist, create it
	A3DTrn2SubLoaderB* pSubLoader = m_aSubTrnLoaders[iSubTrn];
	if (!pSubLoader)
	{
		pSubLoader = CreateSubLoader();
		AString strBaseName = m_strPathName + *m_aSubTrnNames[iSubTrn];
		if (!pSubLoader->Init(strBaseName))
		{
			delete pSubLoader;
			return NULL;
		}

		m_aSubTrnLoaders[iSubTrn] = pSubLoader;
	}

	//	Calculate block's offset
	float sx = m_pt->m_rcTerrain.left + c * m_pt->m_fBlockSize;
	float sz = m_pt->m_rcTerrain.top - r * m_pt->m_fBlockSize;
	int iBlockInSub = iSubRow * iSubTrnBlock + iSubCol;

	return pSubLoader->LoadBlock(iBlockInSub, sx, sz);
}

//	Create sub loader
A3DTrn2SubLoaderB* A3DTerrain2LoaderB::CreateSubLoader()
{
	return new A3DTrn2SubLoaderB(this);
}

//	Unload a block
void A3DTerrain2LoaderB::UnloadBlock(A3DTerrain2Block* pBlock)
{
	ASSERT(pBlock);
	A3DTrn2SubLoaderB* pSubLoader = m_aSubTrnLoaders[pBlock->GetSubTerrainIdx()];
	ASSERT(pSubLoader);
	pSubLoader->UnloadBlock(pBlock);
}

//	Load specified mask area data
A3DTerrain2Mask* A3DTerrain2LoaderB::LoadMask(int iMaskArea)
{
	if (m_pt->m_bNoRender)
		return NULL;

	int iSubTrn, iIndexInSub;
	ConvertMaskAreaIdx(iMaskArea, iSubTrn, iIndexInSub);

	//	sub-loader should has been created when block loaded
	A3DTrn2SubLoaderB* pSubLoader = m_aSubTrnLoaders[iSubTrn];
	ASSERT(pSubLoader);

	return pSubLoader->LoadMask(iMaskArea, iIndexInSub);
}

//	Unload specified mask area data
void A3DTerrain2LoaderB::UnloadMask(A3DTerrain2Mask* pMaskArea)
{
	ASSERT(pMaskArea);

	int iSubTrn, iIndexInSub;
	ConvertMaskAreaIdx(pMaskArea->GetMaskAreaIndex(), iSubTrn, iIndexInSub);
	
	//	sub-loader should has been created when block loaded
	A3DTrn2SubLoaderB* pSubLoader = m_aSubTrnLoaders[iSubTrn];
	ASSERT(pSubLoader);

	pSubLoader->UnloadMask(pMaskArea);
}

//	Convert a global mask area index to sub-terrain index
void A3DTerrain2LoaderB::ConvertMaskAreaIdx(int iMaskArea, int& iSubTrn, int& iIdxInSubTrn)
{
	ASSERT(iMaskArea >= 0 && iMaskArea < m_pt->m_iNumMaskArea);

	//	Mask area row and column in whole terrain
	int iMaskPitch = m_pt->m_iNumAllBlockCol * m_pt->m_iBlockGrid / m_pt->m_iMaskGrid;
	int r = iMaskArea / iMaskPitch;
	int c = iMaskArea % iMaskPitch;

	//	Mask area row and column in sub-terrain
	int iSubTrnMask = m_pt->m_iSubTrnGrid / m_pt->m_iMaskGrid;
	int iSubRow = r % iSubTrnMask;
	int iSubCol = c % iSubTrnMask;

	//	Which sub-terrain this block is in ?
	int iRow = r / iSubTrnMask;
	int iCol = c / iSubTrnMask;
	iSubTrn = iRow * (m_pt->m_iNumAllBlockCol * m_pt->m_iBlockGrid / m_pt->m_iSubTrnGrid) + iCol;

	iIdxInSubTrn = iSubRow * iSubTrnMask + iSubCol;
}

//	Get lightmap load buffer
BYTE* A3DTerrain2LoaderB::GetLMLoadBuffer(int iLightMapSize, int iBufLen)
{
	if (!m_pLMLoadBuf)
	{
		m_pLMLoadBuf = new BYTE [iBufLen];
		if (!m_pLMLoadBuf)
		{
			g_A3DErrLog.Log("A3DTerrain2LoaderB::GetLMLoadBuffer, Not enough memory");
			return NULL;
		}

		m_iLightMapSize = iLightMapSize;
	}
	else
	{
		ASSERT(m_iLightMapSize == iLightMapSize);
	}

	return m_pLMLoadBuf;
}

BYTE* A3DTerrain2LoaderB::GetColMapLoadBuffer(int iBufLen)
{
	if (!m_pColMapData || m_iCMDataSize < iBufLen)
	{
		m_pColMapData = new BYTE [iBufLen];
		if (!m_pColMapData)
		{
			m_iCMDataSize = 0;
			g_A3DErrLog.Log("%s, Not enough memory", __FUNCTION__);
			return NULL;
		}

		m_iCMDataSize = iBufLen;
	}

	return m_pColMapData;
}

//	Tick routine
bool A3DTerrain2LoaderB::Tick(DWORD dwDeltaTime)
{
	for (int i=0; i < m_aSubTrnLoaders.GetSize(); i++)
	{
		if (m_aSubTrnLoaders[i])
			m_aSubTrnLoaders[i]->Tick(dwDeltaTime);
	}

	return true;
}

//	Deactivate idle sub-terrains
void A3DTerrain2LoaderB::DeactivateIdleSubTerrains()
{
	for (int i=0; i < m_aSubTrnLoaders.GetSize(); i++)
	{
		if (m_aSubTrnLoaders[i])
			m_aSubTrnLoaders[i]->CheckActivate();
	}
}
