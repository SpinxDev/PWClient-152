/*
 * FILE: AutoTexture.cpp
 *
 * DESCRIPTION: Class for automaticlly generating texture mask
 *
 * CREATED BY: Jiang Dalong, 2005/02/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "AutoTexture.h"
#include "AutoScene.h"
#include "AutoHome.h"
#include "HomeTerrain.h"
#include "HomeTrnMask.h"
#include "AutoSceneConfig.h"
#include "AutoTerrain.h"
#include "PerlinNoise2D.h"
#include "AutoSceneFunc.h"
#include "Bitmap.h"
#include <A3DMacros.h>
#include <AFileImage.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoTexture::CAutoTexture(CAutoHome* pAutoHome)
{
	m_pAutoHome = pAutoHome;
	m_pAutoTerrain = pAutoHome->GetAutoTerrain();
	m_pHomeTerrain = pAutoHome->GetAutoScene()->GetHomeTerrain();

	m_nTerrainWidth = m_pAutoTerrain->GetHeightMap()->iWidth;
	m_nTerrainHeight = m_pAutoTerrain->GetHeightMap()->iHeight;

	m_pBridgeBack = NULL;
	m_pBridgePatchBack = NULL;

	m_bTextureReady = false;
}

CAutoTexture::~CAutoTexture()
{
	Release();
}

// Initialize
bool CAutoTexture::Init()
{
	//	Create backup mask maps
	if (!(m_pBridgeBack = new CELBitmap))
		return false;

	if (!m_pBridgeBack->CreateBitmap(MASK_SIZE, MASK_SIZE, 8, NULL))
		return false;
	
	if (!(m_pBridgePatchBack = new CELBitmap))
		return false;

	if (!m_pBridgePatchBack->CreateBitmap(MASK_SIZE, MASK_SIZE, 8, NULL))
		return false;
	
	//	Store texture ID
	int nNumLayer = m_pAutoHome->GetAutoScene()->GetAutoSceneConfig()->GetNumMaxLayer();
	for (int i=0; i<nNumLayer; i++)
		m_aTextureID[i] = m_pAutoHome->GetAutoScene()->GetAutoSceneConfig()->GetTextureID(i);
	
	return true;
}

//	Get mask layer object of this home area
CHomeTrnMaskLayer* CAutoTexture::GetTrnMaskLayer(int iLayer)
{
	if (!m_pAutoHome || !m_pHomeTerrain)
		return NULL;

	int r = m_pAutoHome->GetRowIndex();
	int c = m_pAutoHome->GetColIndex();
	CHomeTrnMask* pMask = m_pHomeTerrain->GetHomeAreaMask(r, c);
	if (!pMask)
		return NULL;

	return pMask->GetLayer(iLayer);
}

//	Get mask layer bitmap of this home area
CELBitmap* CAutoTexture::GetTrnMaskLayerBmp(int iLayer)
{
	CHomeTrnMaskLayer* pLayer = GetTrnMaskLayer(iLayer);
	if (pLayer)
		return pLayer->GetMaskBitmap();
	else
		return NULL;
}

// Release Mask
void CAutoTexture::Release()
{
	A3DRELEASE(m_pBridgeBack);
	A3DRELEASE(m_pBridgePatchBack);
}

// Update a point on a mask
bool CAutoTexture::UpdateMaskPoint(int nMask, int x, int y, BYTE btColor)
{
	ASSERT(x >= 0 && x < MASK_SIZE);
	ASSERT(y >= 0 && y < MASK_SIZE);

	CELBitmap* pBmp = GetTrnMaskLayerBmp(nMask);
	if (!pBmp)
		return false;

	BYTE* pData = pBmp->GetBitmapData();
	pData[y * pBmp->GetPitch() + x] = btColor;

	return true;
}

// Get a point's color on a mask
bool CAutoTexture::GetMaskPoint(int nMask, int x, int y, BYTE* btColor)
{
	ASSERT(x >= 0 && x < MASK_SIZE);
	ASSERT(y >= 0 && y < MASK_SIZE);

	CELBitmap* pBmp = GetTrnMaskLayerBmp(nMask);
	if (!pBmp)
		return false;

	BYTE* pData = pBmp->GetBitmapData();
	*btColor = pData[y * pBmp->GetPitch() + x];

	return true;
}

// Reset mask
bool CAutoTexture::ResetAllMasks()
{
	for (int i=0; i<TT_COUNT; i++)
	{
		if (!ResetMask(i))
			return false;
	}

	if (!ClearBridgeBack())
		return false;
	
	return true;
}

// Create terrain mask
bool CAutoTexture::CreateTerrainMask(float fTextureRatio, float fMainTexture, 
				float fPartTexture, float fPartDivision, DWORD dwSeed)
{
	int i,j;
	int nPosX, nPosY;

	// Create terrain noise
	CPerlinNoise2D pnTerrain;
	pnTerrain.Init(m_nTerrainWidth, m_nTerrainHeight, 1.0, fPartDivision, 1, 2, dwSeed, false);
	float fTerrainPatchHeight = (pnTerrain.GetMaxValue() + pnTerrain.GetMinValue()) * 0.5f
							+ (pnTerrain.GetMaxValue() - pnTerrain.GetMinValue()) * 0.5f * fTextureRatio;
	float fTransWeight;
	fTransWeight = (pnTerrain.GetMaxValue() - fTerrainPatchHeight) / fPartTexture;

	float fMaskTransWeight;
	fMaskTransWeight = (pnTerrain.GetMaxValue() - pnTerrain.GetMinValue());

	// Create texture masks
	float fDeltaX = (float)m_nTerrainWidth / MASK_SIZE;
	float fDeltaY = (float)m_nTerrainHeight / MASK_SIZE;
	float fPosX = 0.0f;
	float fPosY = 0.0f;
	for (i=0; i<MASK_SIZE; i++)
	{
		nPosY = (int)(fPosY + 0.5f);
		fPosY += fDeltaY;
		a_ClampRoof(nPosY, m_nTerrainHeight-1);
		nPosX = 0;
		fPosX = 0.0f;
		for (j=0; j<MASK_SIZE; j++)
		{
			nPosX = (int)(fPosX + 0.5f);
			fPosX += fDeltaX;
			a_ClampRoof(nPosX, m_nTerrainWidth-1);

			// mask and patch weight
			float fMaskPatchWeight;
			float fMaskWeight;

			fMaskWeight = 255 * ((pnTerrain.GetValue(nPosY, nPosX) - pnTerrain.GetMinValue()) / fMaskTransWeight + fMainTexture);
			a_ClampRoof(fMaskWeight, 255.0f);
			UpdateMaskPoint(TT_MAIN, j, i, 255);
			UpdateMaskPoint(TT_MAIN_PATCH, j, i, BYTE(255-fMaskWeight));
			if (pnTerrain.GetValue(nPosX, nPosY)>fTerrainPatchHeight)
			{
				fMaskPatchWeight = 255 * (pnTerrain.GetValue(nPosX, nPosY) - fTerrainPatchHeight) / fTransWeight;
				a_ClampRoof(fMaskPatchWeight, 255.0f);
				BYTE btColor;
				GetMaskPoint(TT_MAIN_PATCH, j, i, &btColor);
				if (fMaskPatchWeight>btColor)
					UpdateMaskPoint(TT_MAIN_PATCH, j, i, BYTE(fMaskPatchWeight));
			}
		}// for j
	}// for i

	// Blue mask
//	BlurMask(TT_MAIN);
	BlurMask(TT_MAIN_PATCH);

	SetTextureReady(true);

	return true;
}

// Save mask to file
bool CAutoTexture::SaveMaskToFile(int nMask, const char* szFile)
{
	CELBitmap* pBmp = GetTrnMaskLayerBmp(nMask);
	return pBmp ? pBmp->SaveToFile(szFile) : false;
}

// Build Gaussian 1D vector
int CAutoTexture::BuildGaussian(double *output, double size)
{
    double	sigma2;
    double	l;
    int		length, n, i;

    sigma2 = -(size + 1) * (size + 1) / log(1.0 / 255.0);
    l = size;

    n = int(ceil(l) * 2.0);
    if( (n % 2) == 0 ) n++;
	length = n / 2;

    for(i=0; i<=length; i++)
	    output[i] = exp(-(i * i) / sigma2);
    
    return length;
}

// Gaussian Blue
void CAutoTexture::GaussianBlur(BYTE * pImage, int nSize, double vBlurRange)
{
	BYTE * pLine = new BYTE[nSize];

	double gaussian[32]; // max blur radius is 32 pixel

	// first setup blur factors
	int		gwidth, kwidth;
	gwidth = BuildGaussian(gaussian, vBlurRange);
    kwidth = gwidth * 2 + 1;

	int i, j, k;
	double total = 0.0;
	// now normalize these factors
	for(i=0; i<kwidth; i++) 
	{
        int xpos = abs(i - gwidth);
        total += gaussian[xpos];
    }

	for(i=0; i<=gwidth; i++)
		gaussian[i] /= total;
	
	double	result;
	int		index;
	BYTE	value;

	int index1, index2;

	// first do horizontal blur and put into a temp line buffer first, then copy it to result
	for(i=0; i<nSize; i++) 
	{
		for(j=0; j<gwidth; j++)
		{
			result = 0.0;
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				if( j + k - gwidth < 0 )
					value = pImage[i * nSize];
				else
					value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = (BYTE)result;
		}
		for(j=gwidth; j<nSize-gwidth; j++)
		{
			result = 0.0;
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = (BYTE)result;
        }
		for(j=nSize-gwidth; j<nSize; j++)
		{
			result = 0.0;
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				if( j + k - gwidth >= nSize )
					value = pImage[i * nSize + nSize - 1];
				else
					value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = (BYTE)result;
		}
		memcpy(pImage + i * nSize, pLine, nSize);
    }

	// then transpose the image to make cache works
	for(i=0; i<nSize; i++)
	{
		index1 = i * nSize;
		index2 = i;
		for(j=0; j<i; j++)
		{
			BYTE temp = pImage[index1];
			pImage[index1] = pImage[index2];
			pImage[index2] = temp;
			index1 ++;
			index2 += nSize;
		}
	}

	// then do blur horizontal again.
	for(i=0; i<nSize; i++) 
	{
		for(j=0; j<gwidth; j++)
		{
			result = 0.0;
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				if( j + k - gwidth < 0 )
					value = pImage[i * nSize];
				else
					value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = (BYTE)result;
		}
		for(j=gwidth; j<nSize-gwidth; j++)
		{
			result = 0.0;
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = (BYTE)result;
        }
		for(j=nSize-gwidth; j<nSize; j++)
		{
			result = 0.0;
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				if( j + k - gwidth >= nSize )
					value = pImage[i * nSize + nSize - 1];
				else
					value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = (BYTE)result;
		}
		memcpy(pImage + i * nSize, pLine, nSize);
    }

	// then transpose the image back
	for(i=0; i<nSize; i++)
	{
		index1 = i * nSize;
		index2 = i;
		for(j=0; j<i; j++)
		{
			BYTE temp = pImage[index1];
			pImage[index1] = pImage[index2];
			pImage[index2] = temp;
			index1 ++;
			index2 += nSize;
		}
	}

	delete [] pLine;
}

// Reset a mask
bool CAutoTexture::ResetMask(int nIndex)
{
	CELBitmap* pBmp = GetTrnMaskLayerBmp(nIndex);
	if (pBmp)
	{
		pBmp->Clear();
		return true;
	}

	return false;
}

// Blur mask
void CAutoTexture::BlurMask(int mask)
{
	CELBitmap* pBmp = GetTrnMaskLayerBmp(mask);
	if (pBmp)
	{
		BYTE* pData = pBmp->GetBitmapData();
		GaussianBlur(pData, MASK_SIZE, MASK_SIZE * 2.0f / (m_nTerrainWidth-1));
	}
}

// Get mask data
BYTE* CAutoTexture::GetMaskData(int nMask)
{
	CELBitmap* pBmp = GetTrnMaskLayerBmp(nMask);
	return pBmp ? pBmp->GetBitmapData() : NULL;
}

// Render top texture
bool CAutoTexture::RenderTopTexture(ARectI rect, int nMainTexture, int nPatchTexture,
									float fMaskRatio, float fMainMask, float fPatchMask, float fPatchDivision,
									float fBridgeScope)
{
	a_Clamp(rect.top, 0, m_nTerrainHeight-1);	
	a_Clamp(rect.bottom, 0, m_nTerrainHeight-1);	
	a_Clamp(rect.left, 0, m_nTerrainWidth-1);	
	a_Clamp(rect.right, 0, m_nTerrainWidth-1);	

	CAutoTerrain::VERTEXINFO vMin, vMax;
	m_pAutoTerrain->GetExtremeVerticesInRect(vMin, vMax, rect);

	float fNoiseAmp = (vMax.fHeight - vMin.fHeight) / 15.0f;
	a_Clamp(fBridgeScope, 0.0f, 1.0f);
	float fHeightTop = (vMax.fHeight - vMin.fHeight) * (1 - fBridgeScope) + vMin.fHeight;

	CPerlinNoise2D pnHeight;
	pnHeight.Init(rect.Width(), rect.Height(), fNoiseAmp, 3, 0.5, 4, -1, false);

	float fHeight, fNoisedHeight;
	int i,j;
	ARectI rcInMask;
	rcInMask.left = int((float)rect.left * MASK_SIZE / m_nTerrainWidth + 0.5);
	rcInMask.right = int((float)rect.right * MASK_SIZE / m_nTerrainWidth + 0.5);
	rcInMask.top = int((float)rect.top * MASK_SIZE / m_nTerrainHeight + 0.5);
	rcInMask.bottom = int((float)rect.bottom * MASK_SIZE / m_nTerrainHeight + 0.5);

	float fMaskWeight = fMaskRatio;// Ratio of main mask and patch mask
	float fMainSelfWeight = fMainMask;
	float fPatchSelfWeight = fPatchMask;
	float fPatchNumber = fPatchDivision;

	CPerlinNoise2D pnPatch;
	pnPatch.Init(rect.Width(), rect.Height(), 1.0f, fPatchNumber, 0.8f, 2, -1, false);
	float fPatchHeight = (pnPatch.GetMaxValue() + pnPatch.GetMinValue()) * 0.5f
						+ (pnPatch.GetMaxValue() - pnPatch.GetMinValue()) * 0.5f * fMaskWeight;
	float fTransWeight;
	fTransWeight = (pnPatch.GetMaxValue() - fPatchHeight) / fPatchSelfWeight;

	float fMaskTransWeight;
	fMaskTransWeight = (pnPatch.GetMaxValue() - pnPatch.GetMinValue());


	int nPosX, nPosY;
	int nNoiseX, nNoiseY;
	float fMainValue, fPatchValue;
	float fRenderWeight;
	float fDisWeight;
	float fRectMiddleX = rcInMask.left + (rcInMask.Width()) / 2.0f;
	float fRectMiddleY = rcInMask.top + (rcInMask.Height()) / 2.0f;
	BYTE btColor;

	float fDeltaX = (float)m_nTerrainWidth / MASK_SIZE;
	float fDeltaY = (float)m_nTerrainHeight / MASK_SIZE;
	float fPosX;
	float fPosY = rcInMask.top * fDeltaY;
	for (i=rcInMask.top; i<rcInMask.bottom; i++)
	{
		nPosY = (int)(fPosY + 0.5f);
		fPosY += fDeltaY;
		if (nPosY<rect.top || nPosY>=rect.bottom)
			continue;
		fPosX = rcInMask.left * fDeltaX;
		nPosX = (int)(fPosX + 0.5f);
		for (j=rcInMask.left; j<rcInMask.right; j++)
		{
			nPosX = (int)(fPosX + 0.5f);
			fPosX += fDeltaX;
			if (nPosX<rect.left || nPosX>=rect.right)
				continue;
			
			nNoiseX = nPosX - rect.left;
			nNoiseY = nPosY - rect.top;
			// Get height
			fHeight = m_pAutoTerrain->GetHeightMap()->pAbsHeight[nPosY*m_nTerrainWidth+nPosX];
			// Get real height line
			fNoisedHeight = fHeightTop + pnHeight.GetValue(nNoiseX, nNoiseY);

			if (fHeight > fNoisedHeight)
			{
				float fDisX, fDisY;
				fDisX = (float)fabs((float)(j - fRectMiddleX) / rcInMask.Width() * 2.0f);
				fDisY = (float)fabs((float)(i - fRectMiddleY) / rcInMask.Height() * 2.0f);
				fDisWeight = m_pAutoTerrain->GetHeightWeight(fDisX, fDisY);
				fRenderWeight = (fHeight - fNoisedHeight) / (vMax.fHeight - fNoisedHeight) * fDisWeight;
				fMainValue = 255 * ((pnPatch.GetValue(nNoiseX, rect.Height()-nNoiseY-1) - pnPatch.GetMinValue()) / fMaskTransWeight + fMainSelfWeight);
				a_ClampRoof(fMainValue, 255.0f);
				GetMaskPoint(nMainTexture, j, i, &btColor);
				if (255*fRenderWeight > btColor)
					UpdateMaskPoint(nMainTexture, j, i, BYTE(255*fRenderWeight));
				GetMaskPoint(nPatchTexture, j, i, &btColor);
				if ((255-fMainValue)*fRenderWeight > btColor)
					UpdateMaskPoint(nPatchTexture, j, i, BYTE((255-fMainValue)*fRenderWeight));
				if (pnPatch.GetValue(nNoiseX, nNoiseY)>fPatchHeight)
				{
					fPatchValue = 255 * (pnPatch.GetValue(nNoiseX, nNoiseY) - fPatchHeight) / fTransWeight * fRenderWeight;
					a_ClampRoof(fPatchValue, 255.0f);
					GetMaskPoint(nPatchTexture, j, i, &btColor);
					if (fPatchValue>btColor)
						UpdateMaskPoint(nPatchTexture, j, i, BYTE(fPatchValue));
				}
			}
		}
	}

	return true;
}

//	Apply mask data of specified layers
bool CAutoTexture::ApplyLayerMaskData(int* aLayers, int iNumLayer)
{
	if (!m_pAutoHome || !m_pHomeTerrain)
		return false;

	int r = m_pAutoHome->GetRowIndex();
	int c = m_pAutoHome->GetColIndex();
	return m_pHomeTerrain->UpdateHomeAreaMask(r, c, aLayers, iNumLayer);
}

//	Apply mask data of all layers
bool CAutoTexture::ApplyLayerMaskData()
{
	if (!m_pAutoHome || !m_pHomeTerrain)
		return false;

	int r = m_pAutoHome->GetRowIndex();
	int c = m_pAutoHome->GetColIndex();
	return m_pHomeTerrain->UpdateHomeAreaMask(r, c);
}

//	Change texture of specified layer
bool CAutoTexture::ChangeTexture(int iLayer, const char* szFile)
{
	if (!m_pAutoHome || !m_pHomeTerrain)
		return false;

	int r = m_pAutoHome->GetRowIndex();
	int c = m_pAutoHome->GetColIndex();
	CHomeTrnMask* pMaskArea = m_pHomeTerrain->GetHomeAreaMask(r, c);
	return pMaskArea ? pMaskArea->ChangeLayerTexture(iLayer, szFile) : false;
}

// Clear bridge texture at down area
bool CAutoTexture::ClearBridgeAtDownArea()
{
	int i, j;
	int nPosX, nPosY;
	int nTerrainWidth = m_pAutoTerrain->GetHeightMap()->iWidth;
	int nTerrainHeight = m_pAutoTerrain->GetHeightMap()->iHeight;
	for (i=0; i<MASK_SIZE; i++)
	{
		for (j=0; j<MASK_SIZE; j++)
		{
			nPosX = j * nTerrainWidth / MASK_SIZE;
			nPosY = i * nTerrainHeight / MASK_SIZE;
			if (m_pAutoTerrain->GetHeightMap()->pAbsHeightDown[nPosY*nTerrainWidth+nPosX] < 0)
			{
				UpdateMaskPoint(TT_BRIDGE, j, i, 0);
				UpdateMaskPoint(TT_BRIDGE_PATCH, j, i, 0);
			}
		}
	}

	return true;
}

// Store bridge mask
bool CAutoTexture::StoreBridgeMask()
{
	CELBitmap* pBmp = GetTrnMaskLayerBmp(TT_BRIDGE);
	if (pBmp)
		memcpy(m_pBridgeBack->GetBitmapData(), pBmp->GetBitmapData(), m_pBridgeBack->GetPitch() * m_pBridgeBack->GetHeight());

	if ((pBmp = GetTrnMaskLayerBmp(TT_BRIDGE_PATCH)))
		memcpy(m_pBridgePatchBack->GetBitmapData(), pBmp->GetBitmapData(), m_pBridgePatchBack->GetPitch() * m_pBridgePatchBack->GetHeight());

	return true;
}

// Restore bridge mask
bool CAutoTexture::RestoreBridgeMask()
{
	CELBitmap* pBmp = GetTrnMaskLayerBmp(TT_BRIDGE);
	if (pBmp)
		memcpy(pBmp->GetBitmapData(), m_pBridgeBack->GetBitmapData(), m_pBridgeBack->GetPitch() * m_pBridgeBack->GetHeight());

	if ((pBmp = GetTrnMaskLayerBmp(TT_BRIDGE_PATCH)))
		memcpy(pBmp->GetBitmapData(), m_pBridgePatchBack->GetBitmapData(), m_pBridgePatchBack->GetPitch()*m_pBridgePatchBack->GetHeight());

	return true;
}

// Clear backuped bridge mask
bool CAutoTexture::ClearBridgeBack()
{
	if (m_pBridgeBack)
		m_pBridgeBack->Clear();

	if (m_pBridgePatchBack)
		m_pBridgePatchBack->Clear();

	return true;
}

//	Change texture by texture ID
bool CAutoTexture::ChangeTextureByID(int iLayer, DWORD dwID)
{
	CAutoSceneConfig* pSceneConfig = m_pAutoHome->GetAutoScene()->GetAutoSceneConfig();
	AUTOTEXTURETYPELIST type;
	int nTypeIndex = pSceneConfig->GetAutoTextureTypeListByID(dwID, &type);
	if (-1 == nTypeIndex)
		return false;

	m_aTextureID[iLayer] = dwID;

	return ChangeTexture(iLayer, type.strTexture);
}

// Reset hill texture
bool CAutoTexture::ResetHillTexture()
{
	ResetMask(TT_BRIDGE);
	ResetMask(TT_BRIDGE_PATCH);
	ClearBridgeBack();
	return true;
}

// Reset texture poly
bool CAutoTexture::ResetPartTexture()
{
	ResetMask(TT_PART);
	ResetMask(TT_PART_PATCH);
	return true;
}

// Reset road texture
bool CAutoTexture::ResetRoadTexture()
{
	ResetMask(TT_ROADBED);
	ResetMask(TT_ROAD);
	return true;
}

// Apply patch texture on main texture
bool CAutoTexture::ApplyPatchOnMain(int nMainTexture, int nPatchTexture)
{
	int i, j;
	BYTE btColorPatch, btColorMain;
	int nColor;
	for (i=0; i<MASK_SIZE; i++)
	{
		for (j=0; j<MASK_SIZE; j++)
		{
			GetMaskPoint(nPatchTexture, j, i, &btColorPatch);
			GetMaskPoint(nMainTexture, j, i, &btColorMain);
			nColor = btColorMain - btColorPatch;
			a_ClampFloor(nColor, 0);
			if (btColorMain > 0)
				UpdateMaskPoint(nMainTexture, j, i, BYTE(nColor));
		}
	}
	ResetMask(nPatchTexture);

	return true;
}
