/*
 * FILE: ELTerrainOutline2.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Wangkuiwu, 2005/6/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef   _EL_TERRAINOULINE2_H_
#define   _EL_TERRAINOULINE2_H_

#include <A3DTypes.h>
#include <A3DTerrain2Env.h>
#include <ARect.h>

class A3DDevice;
class A3DCameraBase;
class A3DTerrain2;
class A3DStream;
class A3DViewport;
class TerrainBlockInfo;

#define OUTLINE_MAX_LEVEL   5


class CELTerrainOutline2  
{
	friend class CELOutlineTexture;
	friend class CELOutlineTextureCache;
private:



	A3DDevice *		m_pA3DDevice;			// device pointer
	A3DTerrain2 *   m_pTerrain2;            // the terrain

	int             m_nWorldWidth;          // width in logic unit (meters)
	int             m_nWorldHeight;         // height in logic unit (meters)
	float			m_x;					// coordinates of left top corner in world space
	float			m_z;					// coordinates of left top corner in world space


	int             m_nMaxReplaceTerrainVert;
	int             m_nMaxReplaceTerrainFace;
	A3DVECTOR3     * m_pReplaceTerrainVerts;    //temp buffer used for replace terrain
	WORD           * m_pReplaceTerrainIndices;
	



	ARectI          m_BlockVisibleRect;   //active block visible rect;


	float               m_OutlineTexSize[OUTLINE_MAX_LEVEL*2];  //texture occupied size in world space, w and h
	
	CELOutlineTextureCache * m_pOutlineTexCache;

	DWORD                    m_dwFrame;
	A3DVECTOR3               m_vCameraDir;
	AString                  m_szTexDir;
	
	TerrainBlockInfo       * m_pTerrain2BlkInfo;
	

	int                      m_iMinLevel;
	
	bool                     m_bReplaceTerrain;
	
	A3DTexture            *  m_pDetailTexture;
	
	float						 m_fLevelEndDist[OUTLINE_MAX_LEVEL];

	bool                     m_bNeedUpdate;
	
	AString                  m_szTileDir;

	A3DStream             * m_pFakeTerrainStream;
	
	float                   m_fDNFactor;   //day & night

	int                     m_nExpectCacheSize[5];  
	int                     m_iCurViewScheme;

private:


	
	inline int  _GetSign(float a){
		return (a >0)? (+1):(-1);
	}

	void _CheckBlkInfo(A3DTerrain2::ACTBLOCKS *pActBlocks, const ARectI& rcVisible, bool& bBlkChange);
	
	void _RenderReplaceTerrain(A3DViewport* pViewport);

	void _AllocReplaceTerrainBuf();
	void _FreeReplaceTerrainBuf();
	
	
public:
	CELTerrainOutline2();
	virtual ~CELTerrainOutline2();
	/*
	 *	init the outline
	 *  @param  pA3DDevice	the a3d device
	 *  @param  szTexDir	the texture dir
	 *  @param  szMap		the height map file path
	 *  @param  pTerrain2	the terrain to be stitched
	 *  @return             true if init succeed
	 *  @author kuiwu
	 */
	bool Init(A3DDevice * pA3DDevice, const char * szTexDir, const char * szTileDir, A3DTerrain2 * pTerrain2, unsigned int nWTileCount = 8, unsigned int nHTileCount = 11);
	void Release();
	void Render2(A3DViewport * pViewport);
	void SetReplaceTerrain(bool bVal);
	bool GetReplaceTerrain() const {
		return m_bReplaceTerrain;
	}

	/*
	 *
	 * @desc : set the view distance scheme
	 * @param iScheme : 0 ~ 4 , near ~ far     
	 * @return :
	 * @note:
	 * @todo:   
	 * @author: kuiwu [22/9/2005]
	 * @ref:
	 */
	void SetViewScheme(int iScheme)
	{
		 a_Clamp(iScheme, 0, 4);
		 m_iCurViewScheme = iScheme;
	}
	void SetEndDist(int iLevel, float fDist){
		if ((iLevel < 0) || (iLevel > OUTLINE_MAX_LEVEL-1)) {
			return;
		}
		m_bNeedUpdate = m_bNeedUpdate || (m_fLevelEndDist[iLevel] - fDist > 1.0f) 
						|| (m_fLevelEndDist[iLevel] - fDist < -1.0f);
		if (!m_bNeedUpdate) {
			return;
		}
		m_fLevelEndDist[iLevel] = fDist;
	}
	/*
	 * set the day night factor
	 * @param :     
	 * @return :
	 * @note:
	 * @todo:   
	 * @author: kuiwu [26/8/2005]
	 * @ref:
	 */
	void SetDNFactor(float fDNFactor )
	{
		assert(fDNFactor >= 0.0f && fDNFactor <= 1.0f);
		m_fDNFactor = fDNFactor;
	}
	
	/*
	 *	update the outline
	 *  @param pCamera  the 3d camera
	 *  @param vecCenter the  center pos of the terrain
	 *  @param bReplaceTerrain   indicates if the outline replaces the terrain
	 *  @author kuiwu
	 */
	void Update(A3DCameraBase * pCamera, const A3DVECTOR3& vecCenter);
};

#endif 
