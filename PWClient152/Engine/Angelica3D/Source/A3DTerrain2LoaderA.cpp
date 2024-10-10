/*
 * FILE: A3DTerrain2LoaderA.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/7
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

#include "A3DTerrain2LoaderA.h"
#include "A3DPI.h"
#include "A3DMacros.h"
#include "A3DTerrain2.h"
#include "A3DConfig.h"
#include "A3DTerrain2Blk.h"
#include "A3DTexture.h"
#include "A3DTerrain2Env.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

using namespace A3DTRN2LOADERA;

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
//	Implement A3DTerrain2LoaderA
//	
///////////////////////////////////////////////////////////////////////////

A3DTerrain2LoaderA::A3DTerrain2LoaderA(A3DTerrain2* pTerrain) : 
A3DTerrain2Loader(pTerrain, LOADER_A)
{
	m_pLMLoadBuf	= NULL;
	m_iMaskMapSize	= 0;
	m_bHasLightMap	= false;
	m_iLightMapSize	= 0;
	m_dwLMFileFlags = 0;
	m_iLMDataSize	= 0;

//	memset(&m_MaskLumps, 0, sizeof (m_MaskLumps));
	m_MaskLumps.aMaskMapOffs	= NULL;
	m_MaskLumps.aMaskSize		= NULL;
	m_MaskLumps.aProjAxises		= NULL;
	m_MaskLumps.aSpecuMaps		= NULL;
	m_MaskLumps.aTexIndices		= NULL;
	m_MaskLumps.aUVScales		= NULL;
	m_MaskLumps.aWeights		= NULL;

	memset(m_aMaskBuf, 0, sizeof (m_aMaskBuf));
}

A3DTerrain2LoaderA::~A3DTerrain2LoaderA()
{
}

/*	Initialize object

	Return true for success, otherwise return false.

	szTrn2File: terrain2 general information file name
*/
bool A3DTerrain2LoaderA::Init(const char* szTrn2File)
{
	AString strBaseName = szTrn2File;
	int iTemp = strBaseName.ReverseFind('.');
	if (iTemp > 0)
		strBaseName.CutRight(strBaseName.GetLength() - iTemp);

	//	Open block data file. For A3DTerrain2LoaderA block data file is
	//	just the terrain2 description data file
	if (!LoadBlockDataFile(szTrn2File))
		return false;

	//	Open mask data file
	AString strFile = strBaseName + ".t2mk";
	if (!LoadMaskDataFile(strFile))
		return false;

	//	Open lightmap data file
	m_bHasLightMap = false;
	strFile = strBaseName + ".t2lm";
	if (!LoadLightMapFile(strFile))
	{
		//	Create pure white lightmap
	}

	if (!CreateMaskBuffers())
		return false;

	return true;
}

//	Release object
void A3DTerrain2LoaderA::Release()
{
	//	Release mask lumps
	if (m_MaskLumps.aTexIndices)
	{
		a_free(m_MaskLumps.aTexIndices);
		m_MaskLumps.aTexIndices = NULL;
	}

	if (m_MaskLumps.aSpecuMaps)
	{
		a_free(m_MaskLumps.aSpecuMaps);
		m_MaskLumps.aSpecuMaps = NULL;
	}

	if (m_MaskLumps.aMaskSize)
	{
		a_free(m_MaskLumps.aMaskSize);
		m_MaskLumps.aMaskSize = NULL;
	}

	if (m_MaskLumps.aMaskMapOffs)
	{
		a_free(m_MaskLumps.aMaskMapOffs);
		m_MaskLumps.aMaskMapOffs = NULL;
	}

	if (m_MaskLumps.aProjAxises)
	{
		a_free(m_MaskLumps.aProjAxises);
		m_MaskLumps.aProjAxises = NULL;
	}

	if (m_MaskLumps.aUVScales)
	{
		a_free(m_MaskLumps.aUVScales);
		m_MaskLumps.aUVScales = NULL;
	}

	if (m_MaskLumps.aWeights)
	{
		a_free(m_MaskLumps.aWeights);
		m_MaskLumps.aWeights = NULL;
	}

	m_aBlockOffs.RemoveAll();
	m_aMaskOffs.RemoveAll();
	m_aLightMapOffs.RemoveAll();

	if (m_pLMLoadBuf)
	{
		delete [] m_pLMLoadBuf;
		m_pLMLoadBuf = NULL;
	}

	//	Release mask loading buffers
	for (int i=0; i < NUM_MASKBUF; i++)
	{
		if (m_aMaskBuf[i])
		{
			a_free(m_aMaskBuf[i]);
			m_aMaskBuf[i] = NULL;
		}
	}

	//	Close data files
	m_BlockFile.Close();
	m_MaskFile.Close();
	m_LightMapFile.Close();
}

//	Open terrain2 block data file
bool A3DTerrain2LoaderA::LoadBlockDataFile(const char* szFile)
{
	if (!m_BlockFile.Open(szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadBlockDataFile, Failed to open terrain block file %s", szFile);
		return false;
	}

	//	For A3DTerrain2LoaderA block data file is just the same file
	//	with terrain2 general information file.
	//	Skip TRN2FILEIDVER, TRN2FILEHEADER and sub-terrain data here
	int iSkipSize = sizeof (TRN2FILEIDVER) + sizeof (TRN2FILEHEADER) + 
			m_pt->GetSubTerrainNum() * sizeof (TRN2FILESUBTRN);
	m_BlockFile.Seek(iSkipSize, AFILE_SEEK_SET);

	DWORD dwRead;

	//	Read block data offsets
	m_aBlockOffs.SetSize(m_pt->m_iNumBlock, 100);
	if (!m_BlockFile.Read(m_aBlockOffs.GetData(), m_pt->m_iNumBlock * sizeof (DWORD), &dwRead))
	{
		m_aBlockOffs.RemoveAll();
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadBlockDataFile, Failed to load block offset data");
		return false;
	}

	return true;
}

//	Open terrain2 mask data file
bool A3DTerrain2LoaderA::LoadMaskDataFile(const char* szFile)
{
	if (!m_MaskFile.Open(szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskDataFile, Failed to open terrain mask file %s", szFile);
		return false;
	}

	DWORD dwRead;

	//	Read file version and identify
	TRN2FILEIDVER IdVer;
	if (!m_MaskFile.Read(&IdVer, sizeof (IdVer), &dwRead))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskDataFile, Failed to read mask file header");
		return false;
	}

	if (IdVer.dwIdentify != T2MKFILE_IDENTIFY)
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskDataFile, File format error");
		return false;
	}
	
	if (IdVer.dwVersion > T2MKFILE_VERSION)
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskDataFile, Version %d expected but file's version is %d", T2MKFILE_VERSION, IdVer.dwVersion);
		return false;
	}

	//	Read file header
	T2MKFILEHEADER Header;
	if (!m_MaskFile.Read(&Header, sizeof (Header), &dwRead))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskDataFile, Failed to read mask data file header");
		return false;
	}

	ASSERT(m_pt->m_iNumMaskArea == Header.iNumMaskArea);

	//	Load mask offsets data
	m_aMaskOffs.SetSize(m_pt->m_iNumMaskArea, 100);
	if (!m_MaskFile.Read(m_aMaskOffs.GetData(), m_pt->m_iNumMaskArea * sizeof (DWORD), &dwRead))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskDataFile, Failed to read mask offset data");
		return false;
	}

	//	Load lumps
	if (!LoadMaskLumpData(T2MKLUMP_TEXTURE, Header.aLumps[T2MKLUMP_TEXTURE]) ||
		!LoadMaskLumpData(T2MKLUMP_SPECUMAP, Header.aLumps[T2MKLUMP_SPECUMAP]) ||
		!LoadMaskLumpData(T2MKLUMP_MASKSIZE, Header.aLumps[T2MKLUMP_MASKSIZE]) ||
		!LoadMaskLumpData(T2MKLUMP_OFFSET, Header.aLumps[T2MKLUMP_OFFSET]) ||
		!LoadMaskLumpData(T2MKLUMP_PROJAXIS, Header.aLumps[T2MKLUMP_PROJAXIS]) ||
		!LoadUVScaleLump(Header.aLumps[T2MKLUMP_UVSCALE]) ||
		!LoadMaskLumpData(T2MKLUMP_WEIGHT, Header.aLumps[T2MKLUMP_WEIGHT]))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskDataFile, Failed to read lump data");
		return false;
	}

	return true;
}

//	Read lump data in mask file
bool A3DTerrain2LoaderA::LoadMaskLumpData(int iLump, const A3DTRN2LOADERA::T2MKFILELUMP& Lump)
{
	m_MaskFile.Seek((int)Lump.dwOff, AFILE_SEEK_SET);

	void* pData = a_malloc(Lump.dwSize);
	if (!pData)
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskLumpData, Not enough memory !");
		return false;
	}

	DWORD dwRead;
	if (!m_MaskFile.Read(pData, Lump.dwSize, &dwRead))
	{
		a_free(pData);
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskLumpData, Failed to read data !");
		return false;
	}

	switch (iLump)
	{
	case T2MKLUMP_TEXTURE:	m_MaskLumps.aTexIndices = (int*)pData;		break;
	case T2MKLUMP_SPECUMAP:	m_MaskLumps.aSpecuMaps = (int*)pData;		break;
	case T2MKLUMP_MASKSIZE:	m_MaskLumps.aMaskSize = (int*)pData;		break;
	case T2MKLUMP_OFFSET:	m_MaskLumps.aMaskMapOffs = (DWORD*)pData;	break;
	case T2MKLUMP_PROJAXIS:	m_MaskLumps.aProjAxises = (BYTE*)pData;		break;
	case T2MKLUMP_WEIGHT:	m_MaskLumps.aWeights = (BYTE*)pData;		break;
	default:
		ASSERT(0);
		return false;
	}

	if (iLump == T2MKLUMP_MASKSIZE)
	{
		//	Get the first non-zero mask map size
		int i, iNumMaskSize = Lump.dwSize / sizeof (int);
		for (i=0; i < iNumMaskSize; i++)
		{
			if (m_MaskLumps.aMaskSize[i])
			{
				m_iMaskMapSize = m_MaskLumps.aMaskSize[i];
				break;
			}
		}

		if (i >= iNumMaskSize)
		{
			//	Only in a case this thing will happen, the case is that
			//	the whole terrain only has one layer (the first layer hasn't mask).
			//	In the case, we assume a mask size (a small value is good)
			m_iMaskMapSize = 32;
		}
	}

	return true;
}

//	Read uv scale lump in mask file
bool A3DTerrain2LoaderA::LoadUVScaleLump(const A3DTRN2LOADERA::T2MKFILELUMP& Lump)
{
	m_MaskFile.Seek((int)Lump.dwOff, AFILE_SEEK_SET);

	ASSERT(!(Lump.dwSize % sizeof (T2MKFILEUVSCALE)));
	int iNumItem = (int)Lump.dwSize / sizeof (T2MKFILEUVSCALE);
	
	T2MKFILEUVSCALE* pSrcData = (T2MKFILEUVSCALE*)a_malloc(Lump.dwSize);
	if (!pSrcData)
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadUVScaleLump, Not enough memory !");
		return false;
	}

	if (!(m_MaskLumps.aUVScales = (float*)a_malloc(iNumItem * 2 * sizeof (float))))
	{
		a_free(pSrcData);
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadUVScaleLump, Not enough memory !");
		return false;
	}

	DWORD dwRead;
	if (!m_MaskFile.Read(pSrcData, Lump.dwSize, &dwRead))
	{
		a_free(pSrcData);
		a_free(m_MaskLumps.aUVScales);
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadUVScaleLump, Failed to read data !");
		return false;
	}

	for (int i=0; i < iNumItem; i++)
	{
		m_MaskLumps.aUVScales[(i<<1)+0]	= pSrcData[i].su;
		m_MaskLumps.aUVScales[(i<<1)+1]	= pSrcData[i].sv;
	}

	a_free(pSrcData);

	return true;
}

//	Open terrain2 light map data file
bool A3DTerrain2LoaderA::LoadLightMapFile(const char* szFile)
{
	if (m_pt->m_bNoRender)
		return true;

	if (!m_LightMapFile.Open(szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadLightMapFile, Failed to open lightmap file %s", szFile);
		return false;
	}
	
	DWORD dwRead;

	//	Read file version and identify
	TRN2FILEIDVER IdVer;
	if (!m_LightMapFile.Read(&IdVer, sizeof (IdVer), &dwRead))
	{
		m_LightMapFile.Close();
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadLightMapFile, Failed to read light map file header");
		return false;
	}

	if (IdVer.dwIdentify != T2LMFILE_IDENTIFY)
	{
		m_LightMapFile.Close();
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadLightMapFile, File format error");
		return false;
	}
	
	if (IdVer.dwVersion > T2LMFILE_VERSION)
	{
		m_LightMapFile.Close();
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadLightMapFile, Version %d expected but file's version is %d", T2LMFILE_VERSION, IdVer.dwVersion);
		return false;
	}

	int iNumMaskArea = 0;

	if (IdVer.dwVersion < 2)
	{
		//	Read lightmap file header
		T2LMFILEHEADER Header;
		if (!m_LightMapFile.Read(&Header, sizeof (Header), &dwRead))
		{
			m_LightMapFile.Close();
			g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadLightMapFile, Failed to read lightmap file header");
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
		if (!m_LightMapFile.Read(&Header, sizeof (Header), &dwRead))
		{
			m_LightMapFile.Close();
			g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadLightMapFile, Failed to read lightmap file header");
			return false;
		}

		iNumMaskArea	= Header.iNumMaskArea;
		m_iLightMapSize = Header.iMaskLMSize;
		m_dwLMFileFlags	= Header.dwFlags;
		m_iLMDataSize	= Header.iLMDataSize;
	}

	ASSERT(m_pt->m_iNumMaskArea == iNumMaskArea);

	//	Load light map data offsets
	m_aLightMapOffs.SetSize(m_pt->m_iNumMaskArea, 100);
	if (!m_LightMapFile.Read(m_aLightMapOffs.GetData(), m_pt->m_iNumMaskArea * sizeof (DWORD), &dwRead))
	{
		m_LightMapFile.Close();
		m_aLightMapOffs.RemoveAll();
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadLightMapFile, Failed to read light map offsets");
		return false;
	}

	//	Pre-allocate buffer used to load lightmap data from file
	m_pLMLoadBuf = new BYTE [m_iLMDataSize];
	if (!m_pLMLoadBuf)
	{
		m_LightMapFile.Close();
		m_aLightMapOffs.RemoveAll();
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadLightMapFile, Not enough memory");
		return false;
	}

	m_bHasLightMap = true;

	return true;
}

//	Create mask loading buffers
bool A3DTerrain2LoaderA::CreateMaskBuffers()
{
	//	Use pixel shader 1.4 to render 4 layers together, we must sure
	//	that mask map of each layer has same size.
	ASSERT(m_iMaskMapSize);

	for (int i=0; i < NUM_MASKBUF; i++)
	{
		if (!(m_aMaskBuf[i] = (BYTE*)a_malloc(m_iMaskMapSize * m_iMaskMapSize)))
		{
			g_A3DErrLog.Log("A3DTerrain2LoaderA::CreateMaskBuffers, Not enough memory !");
			return false;
		}
	}

	memset(m_aMaskBuf[MASK_ZERO], 0, m_iMaskMapSize * m_iMaskMapSize);
	memset(m_aMaskBuf[MASK_ONE], 0xff, m_iMaskMapSize * m_iMaskMapSize);

	return true;
}

//	Load block
A3DTerrain2Block* A3DTerrain2LoaderA::LoadBlock(int r, int c, int iBlock)
{
	if (iBlock < 0)
		iBlock = r * m_pt->m_iNumAllBlockCol + c;

	ASSERT(iBlock >= 0 && iBlock < m_pt->m_iNumBlock);

	//	Create a A3DTerrain2Block object
	A3DTerrain2Block* pBlock = new A3DTerrain2Block(m_pt);
	if (!pBlock)
		return NULL;

	if (!pBlock->Init())
		return NULL;

	//	Move file pointer
	m_BlockFile.Seek(m_aBlockOffs[iBlock], AFILE_SEEK_SET);

	float sx = m_pt->m_rcTerrain.left + c * m_pt->m_fBlockSize;
	float sz = m_pt->m_rcTerrain.top - r * m_pt->m_fBlockSize;

	if (!ReadBlockData(pBlock, sx, sz))
	{
		pBlock->Release();
		delete pBlock;
		return NULL;
	}

	return pBlock;
}

//	Unload a block
void A3DTerrain2LoaderA::UnloadBlock(A3DTerrain2Block* pBlock)
{
	ASSERT(pBlock);
	pBlock->Release();
	delete pBlock;
}

//	Read block data from file
bool A3DTerrain2LoaderA::ReadBlockData(A3DTerrain2Block* pBlock, float sx, float sz)
{
	DWORD dwRead;
	T2BKFILEBLOCK BlockInfo;

	//	Load block information
	if (!m_BlockFile.Read(&BlockInfo, sizeof (BlockInfo), &dwRead))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::ReadBlockData, Failed to read block info.");
		return false;
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

	//	Load block vertices data
	if (!m_BlockFile.Read(tbuf.aTempHei, pBlock->m_iNumVert * sizeof (float), &dwRead))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::ReadBlockData, Failed to read block height data.");
		return false;
	}

	//	Load block normal data
	ASSERT(tbuf.aTempNormal);
	if (!m_BlockFile.Read(tbuf.aTempNormal, pBlock->m_iNumVert * 3 * sizeof (float), &dwRead))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::ReadBlockData, Failed to read block normal data.");
		return false;
	}

	//	Fill stream vertex buffer
	if (!pBlock->FillVertexBuffer(sx, sz))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::ReadBlockData, Failed to fill vertex buffer.");
		return false;
	}

	//	Complete aabb
	pBlock->m_aabbBlock.CompleteCenterExts();

	pBlock->m_bDataLoaded = true;

	return true;
}

//	Load specified mask area data
A3DTerrain2Mask* A3DTerrain2LoaderA::LoadMask(int iMaskArea)
{
	if (m_pt->m_bNoRender)
		return NULL;

	ASSERT(iMaskArea >= 0 && iMaskArea < m_pt->m_iNumMaskArea);

	//	Create new mask
	A3DTerrain2Mask* pMask = new A3DTerrain2Mask(m_pt, iMaskArea);
	if (!pMask)
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMask, not enough memory !");
		return NULL;
	}

	//	Move file pointer
	m_MaskFile.Seek((int)m_aMaskOffs[iMaskArea], AFILE_SEEK_SET);

	DWORD dwRead;
	T2MKFILEMASKAREA MaskArea;

	//	Load mask information
	if (!m_MaskFile.Read(&MaskArea, sizeof (MaskArea), &dwRead))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMask, Failed to read mask area info.");
		return NULL;
	}

	ASSERT(MaskArea.iNumLayer > 0);
	pMask->Init(MaskArea.iNumLayer);

	int iFirstIdx = MaskArea.iFirstIndex;
	BYTE* aMaskBufs[3] = {NULL, NULL, NULL};

	//	Load textures and mask textures
	A3DTexture* pTexture;
	int i = 0;
	for (i=0; i < MaskArea.iNumLayer; i++)
	{
		int iIndex = iFirstIdx + i;

		//	Fill layer properties
		A3DTerrain2Mask::LAYER& Layer = pMask->GetLayer(i);
		Layer.iTextureIdx	= GetTextureIndex(iIndex);
		Layer.iSpecMapIdx	= GetSpecularMapIndex(iIndex);
		Layer.iMaskMapSize	= GetMaskMapSize(iIndex);
		Layer.byProjAxis	= GetProjAxis(iIndex);
		Layer.fUScale		= GetUScale(iIndex);
		Layer.fVScale		= GetVScale(iIndex);
		Layer.byWeight		= GetLayerWeight(iIndex);
		Layer.dwMapDataOff	= GetMaskMapOff(iIndex);

		//	Normal texture
		pTexture = m_pt->LoadTexture(Layer.iTextureIdx);
		pMask->m_aTextures.Add(pTexture);

		//	Specular texture
	//	pTexture = m_pt->LoadTexture(Layer.iSpecMapIdx);
	//	pMask->m_aSpecularMaps.Add(pTexture);

		//	Load mask data
		LoadMaskMapData(pMask, i, aMaskBufs);

		if ((i % 3) == 2)
		{
			pTexture = BuildMaskTexture(3, aMaskBufs);
			pMask->m_aMaskTextures.Add(pTexture);
		}
	}

	int iTailNum = i % 3;
	if (iTailNum)
	{
		pTexture = BuildMaskTexture(iTailNum, aMaskBufs);
		pMask->m_aMaskTextures.Add(pTexture);
	}

	//	Load mask light map
	if (m_bHasLightMap)
	{
		if (m_dwLMFileFlags & T2LMFILE_DDS)
			pTexture = LoadMaskLightMap_dds(iMaskArea);
		else
			pTexture = LoadMaskLightMap(iMaskArea);

		pMask->m_pLMTexture = pTexture;
		pMask->m_bOwnLM = true;
	}
	else
	{
		pMask->m_pLMTexture = A3D::g_pA3DTerrain2Env->GetWhiteTexture();
		pMask->m_bOwnLM = false;
	}

	return pMask;
}

//	Unload specified mask area data
void A3DTerrain2LoaderA::UnloadMask(A3DTerrain2Mask* pMaskArea)
{
	ASSERT(pMaskArea);
	pMaskArea->Release();
	delete pMaskArea;
}

//	Load mask map data from file
bool A3DTerrain2LoaderA::LoadMaskMapData(A3DTerrain2Mask* pMask, int iLayer, BYTE* aMaskBufs[3])
{
	int iMaskBufIdx = iLayer % 3;

	if (!iLayer)
	{
		//	First layer always have the
		aMaskBufs[iMaskBufIdx] = m_aMaskBuf[MASK_ONE];
		return true;
	}

	A3DTerrain2Mask::LAYER& Layer = pMask->GetLayer(iLayer);

	//	Non-first layer shouldn't have 0 size mask map
	int iMapSize = Layer.iMaskMapSize;
	ASSERT(iMapSize && iMapSize == m_iMaskMapSize);
	
	//	Seek to mask map data
	m_MaskFile.Seek((int)Layer.dwMapDataOff, AFILE_SEEK_SET);

	//	Load mask map data
	BYTE* pDataBuf = m_aMaskBuf[MASK_L0+iMaskBufIdx];
	ASSERT(pDataBuf);

	DWORD dwRead;
	if (!m_MaskFile.Read(pDataBuf, iMapSize * iMapSize, &dwRead))
	{
		g_A3DErrLog.Log("A3DTerrain2Mask::LoadMaskMapData, Failed to read mask data !");
		return false;
	}

	aMaskBufs[iMaskBufIdx] = pDataBuf;

	return true;
}

//	Build 32-bit mask map from 4 8-bit mask maps
A3DTexture* A3DTerrain2LoaderA::BuildMaskTexture(int iNumLayer, BYTE* aMaskBufs[3])
{
	A3DTexture* pTexture = new A3DTexture;
	if (!pTexture)
		return NULL;

	ASSERT(m_iMaskMapSize);

	//	Create 32-bit mask texture
	if (!pTexture->Create(m_pt->m_pA3DDevice, m_iMaskMapSize, m_iMaskMapSize, A3DFMT_A8R8G8B8, 1))
	{
		delete pTexture;
		g_A3DErrLog.Log("A3DTerrain2Mask::BuildMaskTexture, Failed to create mask texture.");
		return NULL;
	}

	int i, j;
	for (i=iNumLayer; i < 4; i++)
		aMaskBufs[i] = m_aMaskBuf[MASK_ZERO];
	
	//	Lock texture
	BYTE* pDstData;
	int iDstPitch;
	if (!pTexture->LockRect(NULL, (void**) &pDstData, &iDstPitch, 0))
	{
		delete pTexture;
		g_A3DErrLog.Log("A3DTerrain2Mask::BuildMaskTexture, Failed to lock texture");
		return NULL;
	}

	//	Fill texture data
	BYTE* pSrc0 = aMaskBufs[0];
	BYTE* pSrc1 = aMaskBufs[1];
	BYTE* pSrc2 = aMaskBufs[2];
//	BYTE* pSrc3 = aMaskBufs[3];
	BYTE* pDstLine = pDstData;

	for (i=0; i < m_iMaskMapSize; i++)
	{
		BYTE* pDst = pDstLine;
		pDstLine += iDstPitch;
		for (j=0; j < m_iMaskMapSize; j++)
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

//	Load light map of a mask area
A3DTexture* A3DTerrain2LoaderA::LoadMaskLightMap(int iMaskArea)
{
	ASSERT(m_bHasLightMap && m_pLMLoadBuf);

	int iMapSize = m_iLightMapSize;
	DWORD dwOff = GetLightMapOff(iMaskArea);
	m_LightMapFile.Seek((int)dwOff, AFILE_SEEK_SET);

	//	Load mask lightmap data
	DWORD dwRead;
	if (!m_LightMapFile.Read(m_pLMLoadBuf, m_iLMDataSize, &dwRead))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskLightMap, Failed to read light map data !");
		return NULL;
	}

	A3DTexture* pTexture = new A3DTexture;
	if (!pTexture)
		return NULL;

	//	Create light map of specified format
	A3DFORMAT fmt = A3D::g_pA3DTerrain2Env->GetLightMapFormat();
	if (!pTexture->Create(m_pt->m_pA3DDevice, iMapSize, iMapSize, fmt, 1))
	{
		delete pTexture;
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskLightMap, Failed to create light map texture.");
		return NULL;
	}

	//	Fill texture data
	if (!FillLightMap(iMapSize, m_pLMLoadBuf, fmt, pTexture))
	{
		A3DRELEASE(pTexture);
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskLightMap, Failed to fill light map texture");
		return NULL;
	}

	return pTexture;
}

//	Load light map of a mask area, dds version
A3DTexture* A3DTerrain2LoaderA::LoadMaskLightMap_dds(int iMaskArea)
{
	ASSERT(m_bHasLightMap && m_pLMLoadBuf);

	DWORD dwOff = m_aLightMapOffs[iMaskArea];
	m_LightMapFile.Seek((int)dwOff, AFILE_SEEK_SET);

	//	Load mask lightmap data
	DWORD dwRead;
	if (!m_LightMapFile.Read(m_pLMLoadBuf, m_iLMDataSize, &dwRead))
	{
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskLightMap_dds, Failed to read light map data !");
		return NULL;
	}

	A3DTexture* pTexture = new A3DTexture;
	if (!pTexture)
		return NULL;

	//	Create light map of specified format
	if (!pTexture->LoadFromMemory(m_pt->GetA3DDevice(), m_pLMLoadBuf, m_iLMDataSize, 0, 0, A3DFMT_UNKNOWN, 1))
	{
		delete pTexture;
		g_A3DErrLog.Log("A3DTerrain2LoaderA::LoadMaskLightMap_dds, Failed to create light map texture.");
		return NULL;
	}

	return pTexture;
}
