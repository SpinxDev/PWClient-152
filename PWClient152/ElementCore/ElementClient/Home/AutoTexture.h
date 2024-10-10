/*
 * FILE: AutoTexture.h
 *
 * DESCRIPTION: Class for automaticlly generating texture mask
 *
 * CREATED BY: Jiang Dalong, 2005/02/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOTEXTURE_H_
#define _AUTOTEXTURE_H_

#include <AString.h>
#include "AutoSceneConfig.h"

class CAutoHome;
class CAutoTerrain;
class CHomeTerrain;
class CHomeTrnMaskLayer;
class CELBitmap;

class CAutoTexture  
{
public:
	CAutoTexture(CAutoHome* pAutoHome);
	virtual ~CAutoTexture();

public:
	// Initialize
	bool Init();
	// Release Mask
	void Release();

	//	Apply mask data of specified layers
	bool ApplyLayerMaskData(int* aLayers, int iNumLayer);
	//	Apply mask data of all layers
	bool ApplyLayerMaskData();
	//	Change texture of specified layer
	bool ChangeTexture(int iLayer, const char* szFile);
	//	Change texture by texture ID
	bool ChangeTextureByID(int iLayer, DWORD dwID);

	// Update a point on a mask
	bool UpdateMaskPoint(int nMask, int x, int y, BYTE btColor);
	// Get a point's color on a mask
	bool GetMaskPoint(int nMask, int x, int y, BYTE* btColor);
	// Reset all masks
	bool ResetAllMasks();
	// Reset a mask
	bool ResetMask(int nIndex);
	// Calculate masks
	bool CreateTerrainMask(float fTextureRatio, float fMainTexture, 
					float fPartTexture, float fPartDivision, DWORD dwSeed);
	// Save mask to file
	bool SaveMaskToFile(int nMask, const char* szFile);
	// Gaussian Blue
	void GaussianBlur(BYTE * pImage, int nSize, double vBlurRange);
	// Blur mask
	void BlurMask(int mask);
	// Get mask data
	BYTE* GetMaskData(int nMask);
	// Render top texture
	bool RenderTopTexture(ARectI rect, int nMainTexture, int nPatchTexture,
		float fMaskRatio, float fMainMask, float fPatchMask, float fPatchDivision, float fBridgeScope);
	// Clear bridge texture at down area
	bool ClearBridgeAtDownArea();
	// Store bridge mask
	bool StoreBridgeMask();
	// Restore bridge mask
	bool RestoreBridgeMask();
	// Clear backuped bridge mask
	bool ClearBridgeBack();
	// Reset hill texture
	bool ResetHillTexture();
	// Reset texture poly
	bool ResetPartTexture();
	// Reset road texture
	bool ResetRoadTexture();

	// Apply patch texture on main texture
	bool ApplyPatchOnMain(int nMainTexture, int nPatchTexture);
	
	inline void SetTextureReady(bool bReady) { m_bTextureReady = bReady; }
	inline bool GetTextureReady() { return m_bTextureReady; }
	inline DWORD GetTextureID(int nIndex) { return m_aTextureID[nIndex]; }

protected:

	CHomeTerrain*	m_pHomeTerrain;
	CAutoHome*		m_pAutoHome;
	CAutoTerrain*	m_pAutoTerrain;			//	Terrain point

	int				m_nTerrainWidth;		//	Terrain width
	int				m_nTerrainHeight;		//	Terrain height

	bool			m_bTextureReady;		//	Texture ready?

	CELBitmap*		m_pBridgeBack;			//	Mask bridge backup
	CELBitmap*		m_pBridgePatchBack;		//	Mask bridge patch backup

	DWORD			m_aTextureID[TT_COUNT];	//	Texture IDs
	
protected:

	// Build Gaussian vector
	int BuildGaussian(double *output, double size);
	//	Get mask layer object of this home area
	CHomeTrnMaskLayer* GetTrnMaskLayer(int iLayer);
	//	Get mask layer bitmap of this home area
	CELBitmap* GetTrnMaskLayerBmp(int iLayer);
};

#endif // #ifndef _AUTOTEXTURE_H_
