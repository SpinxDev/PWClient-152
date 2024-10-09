/*
 * FILE: MaskModifier.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "MaskModifier.h"
#include "Terrain.h"
#include "TerrainLayer.h"
#include "TerrainRender.h"
#include "Render.h"
#include "Bitmap.h"
#include "A3D.h"
#include "A3DTerrain2Env.h"
#include "A3DTerrain2MaskForEditor.h"

//#define new A_DEBUG_NEW

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
//	Implement CMaskModifier::MODIFY
//	
///////////////////////////////////////////////////////////////////////////

CMaskModifier::MODIFY::MODIFY()
{
	aVerts		= NULL;
	aIndices	= NULL;
	iNumVert	= NULL;
	iNumIndex	= NULL;
}

void CMaskModifier::MODIFY::Clear()
{
	if (aVerts)
		delete [] aVerts;

	if (aIndices)
		delete [] aIndices;

	aVerts		= NULL;
	aIndices	= NULL;
	iNumVert	= NULL;
	iNumIndex	= NULL;

	rcGrid.Clear();
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CMaskModifier
//	
///////////////////////////////////////////////////////////////////////////

CMaskModifier::CMaskModifier(CTerrain* pTerrain) :
m_aAccumVerts(0, 1024),
m_aAccumIdx(0, 2048),
m_aAccumGrids(0, 1024),
m_bMoidfied(false)
{
	m_pTerrain		= pTerrain;
	m_pTexture		= NULL;
	m_pSpecularMap	= NULL;
	m_pMaskTexture	= NULL;
	m_iCurLayer		= -1;
	m_iMaskTexSize	= 0;
	m_p1LPS			= NULL;
	m_fmtMaskTex	= A3DFMT_A8R8G8B8;
	
}

CMaskModifier::~CMaskModifier()
{
}

//	Initalize object
bool CMaskModifier::Init()
{
	//	Load 1 layer pixel shader
	A3DShaderMan* pShaderMan = g_Render.GetA3DEngine()->GetA3DShaderMan();
	m_fmtMaskTex = A3D::g_pA3DTerrain2Env->GetMaskMapFormat();
	AString strFile;

	switch (m_fmtMaskTex)
	{
	case A3DFMT_A8:
	case A3DFMT_A8L8:
	case A3DFMT_A8P8:
	case A3DFMT_A4L4:
	case A3DFMT_A4R4G4B4:
	case A3DFMT_A8R8G8B8:	//	Use alpha version
	
		strFile = "shaders\\ps\\t2_1lay_alpha.txt";
		break;

	case A3DFMT_L8:
	case A3DFMT_R5G6B5:
	case A3DFMT_A1R5G5B5:
	case A3DFMT_X1R5G5B5:
	case A3DFMT_X4R4G4B4:
	case A3DFMT_R8G8B8:
	case A3DFMT_X8R8G8B8:	//	Use non-alpha version

		strFile = "shaders\\ps\\t2_1lay_nonalpha.txt";
		break;

	default:
		ASSERT(0);
		return false;
	}

	if (!(m_p1LPS = pShaderMan->LoadPixelShader(strFile, false)))
	{
		g_Log.Log("CMaskModifier::Init, Failed to load pixel shader %s.", strFile);
		return false;
	}

	return true;
}

//	Release object
void CMaskModifier::Release()
{
	if (m_p1LPS)
	{
		A3DShaderMan* pShaderMan = g_Render.GetA3DEngine()->GetA3DShaderMan();
		pShaderMan->ReleasePixelShader(&m_p1LPS);
		m_p1LPS = NULL;
	}

	//	Release resources
	ReleaseCurLayerRes();

	m_iCurLayer = -1;
}

//	Release current layer resources
void CMaskModifier::ReleaseCurLayerRes()
{
	//	Release modifies of layer
	ReleaseAllModifies();

	A3DTextureMan* pTexMan = g_Render.GetA3DEngine()->GetA3DTextureMan();

	//	Release layer texture
	if (m_pTexture)
	{
		pTexMan->ReleaseTexture(m_pTexture);
		m_pTexture = NULL;
	}

	if (m_pSpecularMap)
	{
		pTexMan->ReleaseTexture(m_pSpecularMap);
		m_pSpecularMap = NULL;
	}

	//	Release mask texture
	A3DRELEASE(m_pMaskTexture);
}

//	Change edit layer all modifies
bool CMaskModifier::ChangeEditLayer(int iLayer)
{
	A3DTextureMan* pTexMan = g_Render.GetA3DEngine()->GetA3DTextureMan();

	if (m_iCurLayer >= 0 && m_iCurLayer != iLayer)
	{
		//	Release resources
		ReleaseCurLayerRes();
	}

	m_iCurLayer = iLayer;

	//	Load texture
	CTerrainLayer* pLayer = m_pTerrain->GetLayer(iLayer);
	CString strFile = pLayer->GetTextureFile();
	if (strFile.GetLength())
		pTexMan->LoadTextureFromFile(strFile, &m_pTexture);
	else
		m_pTexture = NULL;

	//	Load specular map
	strFile = pLayer->GetSpecularMapFile();
	if (strFile.GetLength())
		pTexMan->LoadTextureFromFile(strFile, &m_pSpecularMap);
	else
		m_pSpecularMap = NULL;

	//	Choose mask map size
	//	Ensure max size is power of 2
/*	m_iMaskTexSize = a_Min(g_Render.GetA3DDevice()->GetMaxTextureWidth(), g_Render.GetA3DDevice()->GetMaxTextureHeight());
	for (int i=0; i < 30; i++)
	{
		int iSize1 = 1 << i;
		int iSize2 = 1 << (i + 1);
		if (m_iMaskTexSize == iSize1)
			break;
		else if (m_iMaskTexSize > iSize1 && m_iMaskTexSize < iSize2)
		{
			m_iMaskTexSize = iSize1;
			break;
		}
	}
*/
	m_iMaskTexSize = DEFMASK_SIZE;

	//	Create mask texture
	A3DRELEASE(m_pMaskTexture);
	if (!(m_pMaskTexture = new A3DTexture))
		return false;

	if (!m_pMaskTexture->Create(g_Render.GetA3DDevice(), m_iMaskTexSize, m_iMaskTexSize, m_fmtMaskTex))
	{
		g_Log.Log("CMaskModifier::ChangeEditLayer, Failed to create mask texture");
		return false;
	}

	//	Update mask texture content
	if (0)
//	if (pLayer->GetMaskBitmap())
	{
		int iSize = pLayer->GetMaskBitmap()->GetWidth();
		ARectI rcArea(0, 0, iSize, iSize);
		UpdateMaskTexture(rcArea);
	}
	else
	{
		//	Clear texture with black color
		void* pData;
		int iPitch;
		if (m_pMaskTexture->LockRect(NULL, &pData, &iPitch, 0))
		{
			memset(pData, 0, m_iMaskTexSize * iPitch);
			m_pMaskTexture->UnlockRect();
		}
	}

	return true;
}

//	Begin modifying layer mask
bool CMaskModifier::BeginModify()
{
	m_CurModify.Clear();
	return true;
}

//	End modifying layer mask
bool CMaskModifier::EndModify()
{
	if (!m_CurModify.aVerts)
		return true;

	//	Store current modify
	StoreCurrentModify();

	m_CurModify.Clear();

	return true;
}

/*	Modify routine

	rcArea: pixel area in original mask map
	pModifyData: modify data of original mask map
*/
bool CMaskModifier::Modify(const ARectI& rcArea, int* pModifyData)
{
	//	Modify original mask map
	CTerrainLayer* pLayer = m_pTerrain->GetLayer(m_iCurLayer);
	pLayer->EditMaskMap(rcArea, pModifyData);
	m_bMoidfied = true;
	UpdateMaskMapToRender( rcArea );
	


	//	Create modify mesh
// 	if (!CreateModifyMesh(rcArea))
// 	{
// 		g_Log.Log("CMaskModifier::Modify, Failed to call CreateModifyMesh.");
// 		return false;
// 	}
// 
// 	//	Update current mask texture content
// 	if (!UpdateMaskTexture(rcArea))
// 	{
// 		g_Log.Log("CMaskModifier::Modify, Failed to call UpdateMaskTexture.");
// 		return false;
// 	}

	return true;
}


bool IsPureBlack(CELBitmap::LOCKINFO* pBitInfo)
{
	if(pBitInfo->pData == NULL) return true;

	BYTE* pSrcData = pBitInfo->pData;
	for (int ti=0; ti < pBitInfo->iHeight; ti++)
	{
		//	Windows bitmap has the same color order as D3D
		int idx = 0;
		for (int tj=0; tj < pBitInfo->iWidth; tj++)
		{
			if(pSrcData[tj] != 0) return false;
		}
		pSrcData += pBitInfo->iPitch;
	}

	return true;
}


struct maskTextureInfo
{
	A3DTexture* pMaskTexture;
	BYTE* pDstData;
	BYTE* pDst;
	int iDstPitch;
};

void FillMaskArea(CELBitmap::LOCKINFO pSrcMask[], CTerrainLayer* ppLayers[], int iMaskNum, int iMaskWidth, A3DTerrain2Mask *pMaskArea, int iArea)
{
	((A3DTerrain2MaskForEditor*)pMaskArea)->ClearLayer();
	for( int i = 0; i < iMaskNum; ++i)
	{
		A3DTerrain2Mask::LAYER ly;
		ly.byProjAxis = (BYTE)ppLayers[i]->GetProjAxis();
		ly.byWeight = ppLayers[i]->GetLayerWeight();
		ly.dwDataSize = 0;
		ly.dwMapDataOff = 0;
		ly.fUScale = ppLayers[i]->GetScaleU();
		ly.fVScale = ppLayers[i]->GetScaleV();
		ly.iMaskMapSize = iMaskWidth;
		if(!((A3DTerrain2MaskForEditor*)pMaskArea)->AddLayer(ly,ppLayers[i]->GetTextureFile()))
			return;
	}


	if(iMaskNum < 2) 
	{
		if( 0 == ((A3DTerrain2MaskForEditor*)pMaskArea)->GetMaskTextureNum())
				((A3DTerrain2MaskForEditor*)pMaskArea)->AddMaskTexture(iMaskWidth);
		return;
	}


	AArray<maskTextureInfo> MaskTextruesInfos;

	//3张纹理对应一张纹理模板图 r , g, b， 不知道a 通道干什么用的 
	int needMaskNum = iMaskNum / 3;
	if( iMaskNum % 3 != 0 )
		needMaskNum++;

	MaskTextruesInfos.SetSize( needMaskNum, needMaskNum );
	
	if( 0 == ((A3DTerrain2MaskForEditor*)pMaskArea)->GetMaskTextureNum())
			((A3DTerrain2MaskForEditor*)pMaskArea)->AddMaskTexture(iMaskWidth);

	A3DTexture* pMaskTexture = pMaskArea->GetMaskTexture(0);
	int surfw;
	pMaskTexture->GetDimension(&surfw,&surfw);
	if(surfw != iMaskWidth)
	{
		((A3DTerrain2MaskForEditor*)pMaskArea)->ClearAllMaskTexture();
		((A3DTerrain2MaskForEditor*)pMaskArea)->AddMaskTexture(iMaskWidth);
	}

	
	for( int ti = 0; ti < MaskTextruesInfos.GetSize(); ++ti )
	{
		if( ti >= ((A3DTerrain2MaskForEditor*)pMaskArea)->GetMaskTextureNum())
			((A3DTerrain2MaskForEditor*)pMaskArea)->AddMaskTexture(iMaskWidth);

		MaskTextruesInfos[ti].pMaskTexture = pMaskArea->GetMaskTexture(ti);

		if (!MaskTextruesInfos[ti].pMaskTexture->LockRect(NULL, (void**) &MaskTextruesInfos[ti].pDstData, &MaskTextruesInfos[ti].iDstPitch, 0))
		{
			g_Log.Log("FillMaskArea(), failed to lock texture !");
			return;
		}

	}
	





	switch (MaskTextruesInfos[0].pMaskTexture->GetFormat())
	{
	case A3DFMT_R5G6B5:
	case A3DFMT_A1R5G5B5:
	case A3DFMT_X1R5G5B5:
	case A3DFMT_R8G8B8:
		break;
	case A3DFMT_A8R8G8B8:
	case A3DFMT_X8R8G8B8:
		{
			for (int i=0; i < iMaskWidth; i++)
			{
				for( int t = 0; t < MaskTextruesInfos.GetSize(); ++t )
				{
					MaskTextruesInfos[t].pDst = MaskTextruesInfos[t].pDstData;
					MaskTextruesInfos[t].pDstData += MaskTextruesInfos[t].iDstPitch;
				}

				for (int j=0; j < iMaskWidth; j++)
				{
					int iSrcIdx = pSrcMask[1].iPitch*i +j;
					for( int imn = 0; imn < iMaskNum; imn++ )
					{
						
						int iMaskgroupnum = imn / 3;
						int iLastgroupTexNum = imn % 3;
						//*(pDst1 + 0) = 255; // the mask of the default layer is always 255
						if( imn == 0)
						{
							*MaskTextruesInfos[iMaskgroupnum].pDst = 255;
						}
						else
						{
							*(MaskTextruesInfos[iMaskgroupnum].pDst + iLastgroupTexNum) = pSrcMask[imn].pData[iSrcIdx];
						}

					}

					for( int t = 0; t < MaskTextruesInfos.GetSize(); ++t )
					{
						MaskTextruesInfos[t].pDst += 4;
					}
// 					if(pDst1)
// 					{
// 						if(iMaskNum > 0) *(pDst1 + 0) = 255; // the mask of the default layer is always 255
// 						if(iMaskNum > 1) *(pDst1 + 1) = pSrcMask[1].pData[iSrcIdx];
// 						if(iMaskNum > 2) *(pDst1 + 2) = pSrcMask[2].pData[iSrcIdx];
// 						pDst1 +=4;
// 					}
// 
// 					if(pDst2)
// 					{
// 						if(iMaskNum > 3) *(pDst2 + 0) = pSrcMask[3].pData[iSrcIdx];
// 						if(iMaskNum > 4) *(pDst2 + 1) = pSrcMask[4].pData[iSrcIdx];
// 						if(iMaskNum > 5) *(pDst2 + 2) = pSrcMask[5].pData[iSrcIdx];
// 						pDst2 +=4;
// 					}
				}
			}

			break;
		}
	default:
		ASSERT(0);
		break;
	}

	
	for( int _i = 0; _i < MaskTextruesInfos.GetSize(); ++_i )
	{
		MaskTextruesInfos[_i].pMaskTexture->UnlockRect();
	}

}




void CMaskModifier::UpdateMaskMapToRender(const ARectI& rcUpdate)
{

	CTerrainRender* pTrnRender = m_pTerrain->GetRender();
	if(pTrnRender==NULL ) return;

	int num = pTrnRender->GetMaskAreaNum();
	if(num <= 0) return;
	int row = (int)sqrtf((float)num);
	int col = row;
	bool bSupportPS = A3D::g_pA3DTerrain2Env->GetSupportPSFlag();
	int iMaskSize = m_pTerrain->GetTerrainSize()/col;

	if(m_pTerrain->GetLayerNum() > 1 && m_pTerrain->GetLayer(1)->GetMaskBitmap()) 
		iMaskSize = m_pTerrain->GetLayer(1)->GetMaskBitmap()->GetWidth() / row; 

	for( int i = 0; i < row; ++i)
	{
		for( int j = 0;  j < col; ++j)
		{
			AArray<CELBitmap::LOCKINFO> LockInfo;
			AArray<CTerrainLayer*> ppLayers;


			LockInfo.SetSize(12,12);
			ppLayers.SetSize( 12,12 );


			ARectI rcArea;
			rcArea.left		= j * iMaskSize;
			rcArea.top		= i * iMaskSize;
			rcArea.right	= rcArea.left + iMaskSize;
			rcArea.bottom	= rcArea.top + iMaskSize;

			if(rcUpdate.right < rcArea.left || rcUpdate.left > rcArea.right) continue;
			if(rcUpdate.bottom < rcArea.top || rcUpdate.top > rcArea.bottom) continue;

			int iCount = 0;
			for( int l = 0; l < m_pTerrain->GetLayerNum(); ++l)
			{

				CTerrainLayer* pLayer = m_pTerrain->GetLayer(l);
				ppLayers[iCount] = pLayer;
				LockInfo[iCount].pData = NULL;
				if(l != 0)
				{
					// copy and sew the edge.
					if(pLayer->GetMaskBitmap())
					{
						pLayer->GetMaskBitmap()->LockRect(rcArea, &LockInfo[iCount]);
					}

					if(IsPureBlack(&LockInfo[iCount]))
					{
						//delete[] LockInfo[iCount].pData;
						//LockInfo[iCount].pData = NULL;
						continue;
					}
				}
				iCount++;
				if(iCount >= ppLayers.GetSize()) break;
			}

			int iAreaIdx = i*col + j;
			A3DTerrain2Mask *pMaskArea = m_pTerrain->GetRender()->GetMaskArea(iAreaIdx);
			if(pMaskArea) FillMaskArea(LockInfo.GetData(),ppLayers.GetData(),iCount,iMaskSize,pMaskArea,iAreaIdx);

			//for( int l = 0; l < m_pTerrain->GetLayerNum(); ++l)
			//{
			//	if(LockInfo[l].pData)
			//		delete[] LockInfo[l].pData;
			//}
		}
	}
}


/*	Create modify mesh

	rcArea: pixel area in original mask map
*//*
bool CMaskModifier::CreateModifyMesh(const ARectI& rcArea)
{
	CTerrainLayer* pLayer = m_pTerrain->GetLayer(m_iCurLayer);
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();

	//	Convert original mask map area to terrain grid area
//	int iMaskMapSize = pLayer->GetMaskBitmap() ? pLayer->GetMaskBitmap()->GetWidth() : DEFMASK_SIZE;
	float fScale = (pHeightMap->iWidth - 1.0f) / pLayer->GetMaskBitmap()->GetWidth();

	ARectI rcGrid;
	rcGrid.left		= (int)(rcArea.left * fScale);
	rcGrid.top		= (int)(rcArea.top * fScale);
	rcGrid.right	= (int)(rcArea.right * fScale);
	rcGrid.bottom	= (int)(rcArea.bottom * fScale);

	a_ClampFloor(rcGrid.left, 0);
	a_ClampFloor(rcGrid.top, 0);
	a_ClampRoof(rcGrid.right, pHeightMap->iWidth-1);
	a_ClampRoof(rcGrid.bottom, pHeightMap->iHeight-1);
	
	if (!m_CurModify.rcGrid.IsEmpty())
	{
		rcGrid |= m_CurModify.rcGrid;
	//	if ((rcGrid & m_CurModify.rcGrid) == m_CurModify.rcGrid)
	//		return true;
	}
	
	int iNumVert = (rcGrid.Width() + 1) * (rcGrid.Height() + 1);
	if (iNumVert > 65535)
		return false;

	int iNumIndex = rcGrid.Width() * rcGrid.Height() * 6;
	
	//	Release old primitive data
	if (m_CurModify.aVerts)
	{
		delete [] m_CurModify.aVerts;
		m_CurModify.aVerts = NULL;
	}

	if (m_CurModify.aIndices)
	{
		delete [] m_CurModify.aIndices;
		m_CurModify.aIndices = NULL;
	}

	//	Create new primitive data
	m_CurModify.rcGrid	= rcGrid;
	m_CurModify.iNumVert	= iNumVert;
	m_CurModify.iNumIndex	= iNumIndex;

	if (!(m_CurModify.aVerts = new A3DIBLVERTEX [iNumVert]))
	{
		g_Log.Log("CMaskModifier::CreateModifyMesh, Not enough memory !");
		return false;
	}

	if (!(m_CurModify.aIndices = new WORD [iNumIndex]))
	{
		delete [] m_CurModify.aVerts;
		m_CurModify.aVerts = NULL;
		g_Log.Log("CMaskModifier::CreateModifyMesh, Not enough memory !");
		return false;
	}

	int i, j, iCount = 0;

	//	Fill vertex buffer
	float* pSrcLine = pHeightMap->pHeightData + rcGrid.top * pHeightMap->iWidth + rcGrid.left;
	for (i=0; i <= rcGrid.Height(); i++)
	{
		float z = m_pTerrain->GetTerrainArea().top - m_pTerrain->GetTileSize() * (rcGrid.top + i);
		float x = m_pTerrain->GetTerrainArea().left + m_pTerrain->GetTileSize() * rcGrid.left;

		for (j=0; j <= rcGrid.Width(); j++)
		{
			A3DIBLVERTEX& v = m_CurModify.aVerts[iCount++];
			v.x = x;
			v.y = m_pTerrain->GetYOffset() + pSrcLine[j] * m_pTerrain->GetMaxHeight();
			v.z = z;
		//	v.tu1 = (float)rcGrid.left + j;
		//	v.tv1 = (float)rcGrid.top + i;
			v.tu1 = x;
			v.tv1 = z;
			v.tu2 = ((float)rcGrid.left + j) / (pHeightMap->iWidth - 1);
			v.tv2 = ((float)rcGrid.top + i) / (pHeightMap->iHeight - 1);
		
			v.diffuse	= 0xffffffff;
			v.specular	= 0xff000000;

			x += m_pTerrain->GetTileSize();
		}
		
		pSrcLine += pHeightMap->iWidth;
	}

	//	Fill indices buffer
	for (i=0, iCount=0; i < rcGrid.Height(); i++)
	{
		int v0 = i * rcGrid.Width();

		for (j=0; j < rcGrid.Width(); j++, v0++)
		{
			m_CurModify.aIndices[iCount++] = v0;
			m_CurModify.aIndices[iCount++] = v0 + 1;
			m_CurModify.aIndices[iCount++] = v0 + rcGrid.Width() + 1;

			m_CurModify.aIndices[iCount++] = v0;
			m_CurModify.aIndices[iCount++] = v0 + rcGrid.Width() + 1;
			m_CurModify.aIndices[iCount++] = v0 + rcGrid.Width();
		}
	}

	return true;
}
*/

/*	Create modify mesh

	rcArea: pixel area in original mask map
*/
bool CMaskModifier::CreateModifyMesh(const ARectI& rcArea)
{
	CTerrainLayer* pLayer = m_pTerrain->GetLayer(m_iCurLayer);
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();

	//	Convert original mask map area to terrain grid area
//	int iMaskMapSize = pLayer->GetMaskBitmap() ? pLayer->GetMaskBitmap()->GetWidth() : DEFMASK_SIZE;
	float fScale = (pHeightMap->iWidth - 1.0f) / pLayer->GetMaskBitmap()->GetWidth();

	ARectI rcGrid;
	rcGrid.left		= (int)(rcArea.left * fScale);
	rcGrid.top		= (int)(rcArea.top * fScale);
	rcGrid.right	= (int)(rcArea.right * fScale);
	rcGrid.bottom	= (int)(rcArea.bottom * fScale);

	a_ClampFloor(rcGrid.left, 0);
	a_ClampFloor(rcGrid.top, 0);
	a_ClampRoof(rcGrid.right, pHeightMap->iWidth-1);
	a_ClampRoof(rcGrid.bottom, pHeightMap->iHeight-1);
	
	if (!m_CurModify.rcGrid.IsEmpty())
	{
		ARectI rcOldArea = m_CurModify.rcGrid;
		rcGrid |= m_CurModify.rcGrid;
		if (rcGrid == rcOldArea)
			return true;
	}
	
	int iNumVert = (rcGrid.Width() + 1) * (rcGrid.Height() + 1);
	if (iNumVert > 60000)
		return false;

	int iNumIndex = rcGrid.Width() * rcGrid.Height() * 6;
	
	//	Release old primitive data
	if (m_CurModify.aVerts)
	{
		delete [] m_CurModify.aVerts;
		m_CurModify.aVerts = NULL;
	}

	if (m_CurModify.aIndices)
	{
		delete [] m_CurModify.aIndices;
		m_CurModify.aIndices = NULL;
	}

	//	Create new primitive data
	m_CurModify.rcGrid	= rcGrid;
	m_CurModify.iNumVert	= iNumVert;
	m_CurModify.iNumIndex	= iNumIndex;

	if (!(m_CurModify.aVerts = new A3DIBLVERTEX [iNumVert]))
	{
		g_Log.Log("CMaskModifier::CreateModifyMesh, Not enough memory !");
		return false;
	}

	if (!(m_CurModify.aIndices = new WORD [iNumIndex]))
	{
		delete [] m_CurModify.aVerts;
		m_CurModify.aVerts = NULL;
		g_Log.Log("CMaskModifier::CreateModifyMesh, Not enough memory !");
		return false;
	}

	A3DVECTOR3* aTempVerts = new A3DVECTOR3 [iNumVert];
	if (!aTempVerts)
	{
		delete [] m_CurModify.aVerts;
		m_CurModify.aVerts = NULL;
		delete [] m_CurModify.aIndices;
		m_CurModify.aIndices = NULL;
		return false;
	}

	if (!m_pTerrain->GetRender()->GetFacesOfArea(rcGrid, aTempVerts, m_CurModify.aIndices))
	{
		delete [] m_CurModify.aVerts;
		m_CurModify.aVerts = NULL;
		delete [] m_CurModify.aIndices;
		m_CurModify.aIndices = NULL;
		delete [] aTempVerts;
		g_Log.Log("CMaskModifier::CreateModifyMesh, Not enough memory !");
		return false;
	}

	//	Fill vertex buffer
	int i, j, iCount = 0;
	for (i=0; i <= rcGrid.Height(); i++)
	{
		for (j=0; j <= rcGrid.Width(); j++)
		{
			A3DIBLVERTEX& v = m_CurModify.aVerts[iCount];
			const A3DVECTOR3& vPos = aTempVerts[iCount];
			iCount++;

			v.x = vPos.x;
			v.y = vPos.y;
			v.z = vPos.z;
			v.tu1 = vPos.x;
			v.tv1 = vPos.z;
			v.tu2 = ((float)rcGrid.left + j) / (pHeightMap->iWidth - 1);
			v.tv2 = ((float)rcGrid.top + i) / (pHeightMap->iHeight - 1);
		
			v.diffuse	= 0xffffffff;
			v.specular	= 0xff000000;
		}
	}

	delete [] aTempVerts;

	return true;
}

//	Update mask texture
bool CMaskModifier::UpdateMaskTexture(const ARectI& rcArea)
{
	CTerrainLayer* pLayer = m_pTerrain->GetLayer(m_iCurLayer);
	CELBitmap* pLayerMask = pLayer->GetMaskBitmap();

	//	Convert original mask map area to current mask texture area
	float fScale = (float)m_iMaskTexSize / pLayerMask->GetWidth();

	ARectI rcCurMask;
	rcCurMask.left		= (int)(rcArea.left * fScale);
	rcCurMask.top		= (int)(rcArea.top * fScale);
	rcCurMask.right		= (int)(rcArea.right * fScale);
	rcCurMask.bottom	= (int)(rcArea.bottom * fScale);

	a_ClampFloor(rcCurMask.left, 0);
	a_ClampFloor(rcCurMask.top, 0);
	a_ClampRoof(rcCurMask.right, m_iMaskTexSize);
	a_ClampRoof(rcCurMask.bottom, m_iMaskTexSize);

	if (fScale == 1.0f)
	{
		CELBitmap::LOCKINFO LockInfo;
		pLayerMask->LockRect(rcCurMask, &LockInfo);
		UpdateMaskTexture(LockInfo.pData, rcCurMask, LockInfo.iPitch);
	}
	else if (fScale > 1.0f)
	{
		BYTE* pDstData = new BYTE [rcCurMask.Width() * rcCurMask.Height()];
		if (!pDstData)
			return false;

		fScale = 1.0f / fScale;
		int iCount = 0;

		for (int r=rcCurMask.top; r < rcCurMask.bottom; r++)
		{
			BYTE* pSrcLine = pLayerMask->GetBitmapData() + (int)(r * fScale) * pLayerMask->GetPitch();
			for (int c=rcCurMask.left; c < rcCurMask.right; c++)
				pDstData[iCount++] = pSrcLine[(int)(c * fScale)];
		}

		UpdateMaskTexture(pDstData, rcCurMask, rcCurMask.Width());

		delete [] pDstData;
	}
	else
	{
		BYTE* pDstData = new BYTE [rcCurMask.Width() * rcCurMask.Height()];
		if (!pDstData)
			return false;

		int iCount = 0;
		fScale = 1.0f / fScale;
		int iSrcStep = (int)(fScale + 0.1f);

		for (int r=rcCurMask.top; r < rcCurMask.bottom; r++)
		{
			BYTE* pSrcLine = pLayerMask->GetBitmapData() + r * iSrcStep * pLayerMask->GetPitch();
			for (int c=rcCurMask.left; c < rcCurMask.right; c++)
				pDstData[iCount++] = pSrcLine[c * iSrcStep];
		}

		UpdateMaskTexture(pDstData, rcCurMask, rcCurMask.Width());

		delete [] pDstData;
	}

	return true;
}

//	Update mask texture
bool CMaskModifier::UpdateMaskTexture(BYTE* pSrcData, const ARectI& rcArea, int iSrcPitch)
{
	RECT rcLock;
	::SetRect(&rcLock, rcArea.left, rcArea.top, rcArea.right, rcArea.bottom);

	BYTE* pDstData;
	int iDstPitch;
	if (!m_pMaskTexture->LockRect(&rcLock, (void**) &pDstData, &iDstPitch, 0))
		return false;
	
	BYTE* pDstLine = pDstData;
	BYTE* pSrcLine = pSrcData;

	switch (m_fmtMaskTex)
	{
	case A3DFMT_A8:
	case A3DFMT_L8:			//	8-bit
	{
		for (int i=0; i < rcArea.Height(); i++)
		{
			memcpy(pDstLine, pSrcLine, rcArea.Width());
			pDstLine += iDstPitch;
			pSrcLine += iSrcPitch;
		}

		break;
	}
	case A3DFMT_A4L4:		//	8-bit
	{
		float fScale = 15.0f / 255.0f;
		for (int i=0; i < rcArea.Height(); i++)
		{
			BYTE* pDst = pDstLine;
			BYTE* pSrc = pSrcLine;
			pDstLine += iDstPitch;
			pSrcLine += iSrcPitch;

			for (int j=0; j < rcArea.Width(); j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = (BYTE)(c * fScale);
			}
		}

		break;
	}
	case A3DFMT_A8L8:
	case A3DFMT_A8P8:
	case A3DFMT_A8R3G3B2:
	{
		for (int i=0; i < rcArea.Height(); i++)
		{
			WORD* pDst = (WORD*)pDstLine;
			BYTE* pSrc = pSrcLine;
			pDstLine += iDstPitch;
			pSrcLine += iSrcPitch;

			for (int j=0; j < rcArea.Width(); j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = (((WORD)c) << 8) | c;
			}
		}

		break;
	}
	case A3DFMT_R5G6B5:
	case A3DFMT_A1R5G5B5:
	case A3DFMT_X1R5G5B5:
	{
		float fScale = 31.0f / 255.0f;
		for (int i=0; i < rcArea.Height(); i++)
		{
			WORD* pDst = (WORD*)pDstLine;
			BYTE* pSrc = pSrcLine;
			pDstLine += iDstPitch;
			pSrcLine += iSrcPitch;

			for (int j=0; j < rcArea.Width(); j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = (WORD)(c * fScale);
			}
		}

		break;
	}
	case A3DFMT_A4R4G4B4:
	case A3DFMT_X4R4G4B4:	//	16-bit
	{
		float fScale = 15.0f / 255.0f;
		for (int i=0; i < rcArea.Height(); i++)
		{
			WORD* pDst = (WORD*)pDstLine;
			BYTE* pSrc = pSrcLine;
			pDstLine += iDstPitch;
			pSrcLine += iSrcPitch;

			for (int j=0; j < rcArea.Width(); j++)
			{
				WORD c = *pSrc++;
				c = (WORD)(c * fScale);
				*pDst++ = (c << 12) | (c << 8) | (c << 4) | c;
			}
		}

		break;
	}
	case A3DFMT_R8G8B8:		//	24-bit
	{
		for (int i=0; i < rcArea.Height(); i++)
		{
			BYTE* pDst = pDstLine;
			BYTE* pSrc = pSrcLine;
			pDstLine += iDstPitch;
			pSrcLine += iSrcPitch;

			for (int j=0; j < rcArea.Width(); j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = c;
				*pDst++ = c;
				*pDst++ = c;
			}
		}

		break;
	}
	case A3DFMT_A8R8G8B8:
	case A3DFMT_X8R8G8B8:	//	32-bit
	{
		for (int i=0; i < rcArea.Height(); i++)
		{
			DWORD* pDst = (DWORD*)pDstLine;
			BYTE* pSrc = pSrcLine;
			pDstLine += iDstPitch;
			pSrcLine += iSrcPitch;

			for (int j=0; j < rcArea.Width(); j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = A3DCOLORRGBA(c, c, c, c);
			}
		}

		break;
	}
	default:
		ASSERT(0);
		break;
	}

	m_pMaskTexture->UnlockRect();

	return true;
}

//	Release all modify objects
void CMaskModifier::ReleaseAllModifies()
{
	m_aAccumVerts.RemoveAll();
	m_aAccumIdx.RemoveAll();
	m_aAccumGrids.RemoveAll();

	//	Release current modify
	m_CurModify.Clear();

	if (m_pMaskTexture)
	{
		//	Clear mask texture with black color
		void* pData;
		int iPitch;
		if (m_pMaskTexture->LockRect(NULL, &pData, &iPitch, 0))
		{
			memset(pData, 0, m_iMaskTexSize * iPitch);
			m_pMaskTexture->UnlockRect();
		}
	}
}

//	Store currently modified mesh
bool CMaskModifier::StoreCurrentModify()
{
	if (!m_CurModify.aVerts)
		return true;

	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();

	int i, j;
	int iVertPitch = m_CurModify.rcGrid.Width() + 1;
	const ARectF& rcTerrain = m_pTerrain->GetTerrainArea();
	float fInvGridSize = 1.0f / m_pTerrain->GetTileSize();

	//	Temporary grid vertices and indices
	AArray<A3DIBLVERTEX, A3DIBLVERTEX&> aGridVerts;
	AArray<WORD, WORD> aGridIdx;

	//	We couldn't assume the grid order in m_CurModify.aVerts and
	//	m_CurModify.aIndices, but we know every 6 indices describe
	//	a grid. Then we can get all grid's 4 vertices and calculate
	//	grid's center, and use the center position we can get grid's
	//	index in whole terrain
	for (i=0; i < m_CurModify.iNumIndex; i+=6)
	{
		if (m_aAccumVerts.GetSize() >= 65000)
			break;	//	Too many vertices ...

		//	Clear temporary grid data
		aGridVerts.RemoveAll(false);
		aGridIdx.RemoveAll(false);

		int j;
		//	Fill grid data
		for (j=0; j < 6; j++)
		{
			int idx = m_CurModify.aIndices[i + j];
			if (aGridIdx.Find(idx) < 0)
			{
				aGridVerts.Add(m_CurModify.aVerts[idx]);
				aGridIdx.Add(idx);
			}
		}

		ASSERT(aGridVerts.GetSize() == 4);
		ASSERT(aGridIdx.GetSize() == 4);

		//	Calculate grid index in whole terrain
		A3DVECTOR3 vCenter(0.0f);
		for (j=0; j < 4; j++)
		{
			vCenter.x += aGridVerts[j].x;
			vCenter.z += aGridVerts[j].z;
		}

		vCenter *= 0.5f;

		int gx = (int)((vCenter.x - rcTerrain.left) * fInvGridSize);
		int gz = (int)((rcTerrain.top - vCenter.z) * fInvGridSize);
		int iGridIdx = gz * (pHeightMap->iWidth-1) + gx;

		//	Check whether this grid has been accumulated ?
		if (!TryToAccumGrid(iGridIdx))
			continue;

		//	Store vertex data
		int iBaseIdx = m_aAccumVerts.GetSize();
		m_aAccumVerts.Add(aGridVerts[0]);
		m_aAccumVerts.Add(aGridVerts[1]);
		m_aAccumVerts.Add(aGridVerts[2]);
		m_aAccumVerts.Add(aGridVerts[3]);

		//	Store index data
		for (j=0; j < 6; j++)
		{
			int idx = aGridIdx.Find(m_CurModify.aIndices[i + j]);
			m_aAccumIdx.Add(iBaseIdx + idx);
		}
	}

	return true;
}

//	Try to insert a grid into m_aAccumGrids
bool CMaskModifier::TryToAccumGrid(int iGrid)
{
	for (int i=0; i < m_aAccumGrids.GetSize(); i++)
	{
		int iCurGrid = m_aAccumGrids[i];
		if (iGrid == iCurGrid)
			return false;	//	Grid has been accumulated
		else if (iGrid < iCurGrid)
		{
			int pos = (i == 0) ? 0 : i-1;
			m_aAccumGrids.InsertAt(pos, iGrid);
			return true;
		}
	}

	m_aAccumGrids.Add(iGrid);
	return true;
}

//	Render
bool CMaskModifier::Render(A3DViewport* pA3DViewport)
{

	return true;

	if (m_iCurLayer < 0 || !m_pMaskTexture || (!m_CurModify.aVerts && !m_aAccumVerts.GetSize()))
		return true;

	//	Set render states
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();
	pA3DDevice->SetLightingEnable(false);
	pA3DDevice->SetZTestEnable(false);
	pA3DDevice->SetZWriteEnable(false);
	pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);

	pA3DDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DIBLVERTEX);


	if (!m_pTexture)
	{
		pA3DDevice->ClearTexture(0);
	}
	else
	{
		//	Scale texture coordinates
		CTerrainLayer* pLayer = m_pTerrain->GetLayer(m_iCurLayer);

		//	Open texture stage
		pA3DDevice->SetTextureColorOP(1, A3DTOP_MODULATE);
		pA3DDevice->SetTextureColorOP(2, A3DTOP_MODULATE);

		A3DMATRIX4 matScale;
		matScale.Identity();
		matScale._11 = pLayer->GetScaleU();
		matScale._22 = pLayer->GetScaleV();
		pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_COUNT2);
		pA3DDevice->SetTextureMatrix(0, matScale);

		pA3DDevice->SetTextureTransformFlags(2, A3DTTFF_COUNT2);
		pA3DDevice->SetTextureMatrix(2, matScale);

		matScale._11 = 1.0f;
		matScale._22 = 1.0f;
		pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_COUNT2);
		pA3DDevice->SetTextureMatrix(1, matScale);

		pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
		pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);

		m_pTexture->Appear();
		m_pMaskTexture->Appear(1);

		if (m_pSpecularMap)
			m_pSpecularMap->Appear(2);
		else
			m_pTexture->Appear(2);

		//	Apply pixel shader
		m_p1LPS->Appear();
	}

	//	Render accumulated modifiers
	if (m_aAccumVerts.GetSize())
	{
		pA3DDevice->SetZBias(1);
		pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, m_aAccumVerts.GetSize(), 
			m_aAccumIdx.GetSize() / 3, m_aAccumIdx.GetData(), A3DFMT_INDEX16, m_aAccumVerts.GetData(),
			sizeof (A3DIBLVERTEX));
	}

	//	Render current modifies
	if (m_CurModify.aVerts)
	{
		pA3DDevice->SetZBias(2);
		pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, m_CurModify.iNumVert, 
			m_CurModify.iNumIndex / 3, m_CurModify.aIndices, A3DFMT_INDEX16, m_CurModify.aVerts,
			sizeof (A3DIBLVERTEX));
	}

	pA3DDevice->SetZBias(0);
	pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	pA3DDevice->SetTextureTransformFlags(2, A3DTTFF_DISABLE);
	pA3DDevice->SetZTestEnable(true);
	pA3DDevice->SetZWriteEnable(true);
	pA3DDevice->ClearPixelShader();
	pA3DDevice->SetLightingEnable(true);

	return true;
}


UINT64  CMaskModifier::GetBlockFlags(int iBlk )
{
	if(m_aBlockFlags.size() <= iBlk) return 1;
	return m_aBlockFlags[iBlk];
}

UINT64  CMaskModifier::GetBlockFlags(int r, int c)
{
	if(m_pTerrain->GetRender() == NULL) return 1;
	
	int iNumBlk = m_pTerrain->GetRender()->GetBlockColNum();
	if(iNumBlk == 0) return 0;
	
	int idx = r * iNumBlk + c;
	if(m_aBlockFlags.size() <= idx) return 1;
	return m_aBlockFlags[idx];
}

void CMaskModifier::UpdateLayerFlags()
{
	if(m_pTerrain->GetLayerNum() < 2 || m_pTerrain->GetLayer(1)->GetMaskBitmap() == NULL) return;
	if(m_pTerrain->GetRender() == NULL) return;
	int iNumBlk = m_pTerrain->GetRender()->GetBlockColNum();
	if(iNumBlk == 0) return;
	
	m_aBlockFlags.clear();
	
	int iBlcMaskPixelWidth = m_pTerrain->GetLayer(1)->GetMaskBitmap()->GetWidth()/iNumBlk;
	
	for( int i = 0; i < iNumBlk; ++i)
	{
		for( int j = 0; j < iNumBlk; ++j)
		{
			//int idx = i * iNumBlk + j;
			UINT64  dwFlags = 0;
			UINT64  llBase = 1;
			CELBitmap::LOCKINFO LockInfo;
			ARectI rcArea;
			rcArea.left		= j * iBlcMaskPixelWidth;
			rcArea.top		= i * iBlcMaskPixelWidth;
			rcArea.right	= rcArea.left + iBlcMaskPixelWidth;
			rcArea.bottom	= rcArea.top + iBlcMaskPixelWidth;
			
			for( int l = 0; l < m_pTerrain->GetLayerNum(); ++l)
			{
				CTerrainLayer* pLayer = m_pTerrain->GetLayer(l);
				if(!pLayer->IsDefault())
				{
					if(pLayer->GetMaskBitmap() == NULL) continue;
					pLayer->GetMaskBitmap()->LockRect(rcArea,&LockInfo);
					if(IsPureBlack(&LockInfo)) continue;
				}
				dwFlags |= (llBase << l);
			}
			
			m_aBlockFlags.push_back(dwFlags);
		}
	}
}

