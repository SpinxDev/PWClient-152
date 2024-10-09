 /*
 * FILE: NPCMoveMap.h
 *
 * DESCRIPTION:   A  class to realize a map for NPCs' movement, 
 *							which mainly includes two parts: 
 *							1. Reachable map (to tell whether a tile can enter )
 *							2. Delta Height map ( if the tile is reachable, refer to 
 *								this map for the delta height from the terrain surface)
 *
 * CREATED BY: He wenfeng, 2005/4/4
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _NPCMOVEMAP_H_
#define _NPCMOVEMAP_H_

#define STANDARD_SUBMAP_SIZE 1024

#include "BlockImage.h"
#include "BitImage.h"
#include <A3DVECTOR.h>

// Data structure for Path Finding
struct POS2D
{
	int u;
	int v;
};

/*
// seems no use

struct  MOVEMAP_POS
{
	int u_submap;			// submap's u pos
	int v_submap;			// submap's v pos
	int u;						//pixel's u pos in submap
	int v;						//pixel's v pos in submap
};
  */

class CNPCMoveMap
{
public:
	CNPCMoveMap() ;
	
	virtual ~CNPCMoveMap();

	void Release();

	void Init(int iWidth, int iLength);

	void SetWorldOrigin(A3DVECTOR3 vOrigin)
	{
		m_vOrigin = vOrigin;
	}

	bool SetMapCenterAsOrigin()
	{
		if( m_iSubMapWidth == -1)
			return false;						// no submaps loaded
		else
		{
			m_vOrigin.Clear();
			m_vOrigin.x = m_iSubMapWidth * m_iWidth * m_fPixelSize /2.0f;
			m_vOrigin.z = m_iSubMapLength * m_iLength * m_fPixelSize /2.0f;
			return true;
		}		
	}
	
	/*
	// Transform an absolute 3D position to the map position
	inline void GetMoveMapPos(const A3DVECTOR3& vPos, MOVEMAP_POS& mPos)
	{
		int u, v;
		float fPSizeRecip=(m_fPixelSize==1.0f)?1.0f:(1.0f/m_fPixelSize);
		u = (int) ((vPos.x + m_vOrigin.x) * fPSizeRecip );
		v = (int) ((vPos.z + m_vOrigin.z) * fPSizeRecip );
		mPos.u_submap = u /m_iSubMapWidth;
		mPos.v_submap = v / m_iSubMapLength;
		mPos.u = u % m_iSubMapWidth;
		mPos.v = v % m_iSubMapLength;
	}
	*/

	// Transform an absolute 3D position to the map position
	inline POS2D GetMapPos(const A3DVECTOR3& vPos)
	{
		POS2D mPos;
		float fPSizeRecip=(m_fPixelSize==1.0f)?1.0f:(1.0f/m_fPixelSize);
		mPos.u = (int) ((vPos.x + m_vOrigin.x) * fPSizeRecip );
		mPos.v = (int) ((vPos.z + m_vOrigin.z) * fPSizeRecip );
		return mPos;
	}

	inline bool IsPosReachable(const POS2D& pos)
	{
		int su,sv,u,v;
		if(m_bStandardSubmap)
		{
			su = pos.u >> 10;
			sv = pos.v >> 10;
			u = pos.u & 0x3ff;
			v = pos.v & 0x3ff;
		}
		else
		{
			su = pos.u / m_iSubMapWidth;
			sv = pos.v / m_iSubMapLength;
			u = pos.u % m_iSubMapWidth;
			v = pos.v % m_iSubMapLength;
		}
		
		return m_ReachableMap[ sv * m_iWidth + su].GetPixel(u,v);
	}
	
	inline float GetPosDeltaHeight(const POS2D& pos)
	{

		int su,sv,u,v;
		if(m_bStandardSubmap)
		{
			su = pos.u >> 10;
			sv = pos.v >> 10;
			u = pos.u & 0x3ff;
			v = pos.v & 0x3ff;
		}
		else
		{
			su = pos.u / m_iSubMapWidth;
			sv = pos.v / m_iSubMapLength;
			u = pos.u % m_iSubMapWidth;
			v = pos.v % m_iSubMapLength;
		}
		FIX16 f16DeltaHeight=m_DeltaHeightMap[ sv * m_iWidth + su].GetPixel(u, v);
		return FIX16TOFLOAT( f16DeltaHeight );		
	}

	/*
	// Move a pos in the map.
	//		In:		mPosFrom, du, dv
	//		Out:  mPosTo
	//  if the mPosTo is an invalid pos, we return false
	inline bool MovePos(const MOVEMAP_POS& mPosFrom, int du, int dv, MOVEMAP_POS& mPosTo)
	{
		mPosTo = mPosFrom;
		mPosTo.u+=du;
		mPosTo.v+=dv;

		if( mPosTo.u >= m_iSubMapWidth )
		{
			mPosTo.u -=m_iSubMapWidth;
			mPosTo.u_submap ++;
		}
		else if(mPosTo.u < 0 )
		{
			mPosTo.u +=m_iSubMapWidth;
			mPosTo.u_submap --;
		}

		if( mPosTo.v >= m_iSubMapLength )
		{
			mPosTo.v -=m_iSubMapLength;
			mPosTo.v_submap ++;
		}
		else if(mPosTo.v < 0 )
		{
			mPosTo.v +=m_iSubMapLength;
			mPosTo.v_submap --;
		}

		if( mPosTo.u >= m_iWidth || mPosTo.u < 0 || mPosTo.v>= m_iLength || mPosTo.v < 0 )
			return false;
		else
			return true;
	}

	// to tell if the mPos can be reached, if so,
	// the fDeltaHeight is plus the terrain height to compute the real Y 
	inline bool IsPosReachable(const MOVEMAP_POS& mPos, float& fDeltaHeight)
	{
		int iSubMapID = mPos.v_submap * m_iWidth + mPos.u_submap;
		bool bReachable = m_ReachableMap [ iSubMapID ].GetPixel(mPos.u, mPos.v);
		if(!bReachable) return false;			// and the parameter fDeltaHeight is nonsense
		
		FIX16 f16DeltaHeight=m_DeltaHeightMap[iSubMapID].GetPixel(mPos.u, mPos.v);
		fDeltaHeight = FIX16TOFLOAT( f16DeltaHeight );
		return true;
	}
	*/
	
	// Load and Save, using FILE
	bool Save( FILE *pFileToSave );
	bool Load( FILE *pFileToLoad );

	bool LoadSubReachableMap(int u, int v, FILE* pFileToLoad);
	bool LoadSubDeltaHeightMap(int u, int v, FILE* pFileToLoad);
	
protected:
	CBitImage * m_ReachableMap;								// set of sub- reachable maps
	CBlockImage<FIX16> * m_DeltaHeightMap;			// set of sub- deltaheight maps
	
	int m_iWidth;					// sub-maps in width
	int m_iLength;					// sub-maps in length
	
	int m_iSubMapWidth;			// sub-map's width
	int m_iSubMapLength;		// sub_map's length
	
	bool m_bStandardSubmap;	// whether the submap is 1024*1024

	float m_fPixelSize;					// the pixel's size of the map, pixel is the elementary unit of the map!

	A3DVECTOR3 m_vOrigin;	// origin of the world in the map coordinate system

};



#endif