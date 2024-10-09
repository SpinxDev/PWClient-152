 /*
 * FILE: NPCMoveMap.cpp
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

#include "NPCMoveMap.h"


// Version Info (DWORD)

// Current Version
#define NPCMOVEMAP_VER 0x00000001	

// Old Version


CNPCMoveMap::CNPCMoveMap() 
{ 
	m_ReachableMap = NULL; 
	m_DeltaHeightMap = NULL;
}

CNPCMoveMap::~CNPCMoveMap() 
{ 
	Release(); 
}



void CNPCMoveMap::Release()
{
	if(m_ReachableMap) delete [] m_ReachableMap;
	if(m_DeltaHeightMap) delete [] m_DeltaHeightMap;
	m_ReachableMap = NULL;
	m_DeltaHeightMap = NULL;
}

void CNPCMoveMap::Init(int iWidth, int iLength)
{
	m_iWidth = iWidth;
	m_iLength = iLength;
	int iSize = iWidth * iLength;
	m_ReachableMap = new CBitImage [iSize];
	m_DeltaHeightMap = new CBlockImage<FIX16> [iSize];
	
	m_iSubMapWidth = -1;
	m_iSubMapLength = -1;
	m_fPixelSize = 0.0f;
}

bool CNPCMoveMap::LoadSubReachableMap(int u, int v, FILE* pFileToLoad)
{
	if( u >= m_iWidth || u < 0 || v>= m_iLength || v < 0 )
		return false;
	
	int iSubMapID = v*m_iWidth + u;

	if(!m_ReachableMap[ iSubMapID ].Load(pFileToLoad))
		return false;

	if(m_iSubMapWidth == -1)
	{
		m_ReachableMap[ iSubMapID ].GetImageSize(m_iSubMapWidth, m_iSubMapLength);
		m_fPixelSize = m_ReachableMap[ iSubMapID ].GetPixelSize();
		m_bStandardSubmap = ( m_iSubMapWidth==STANDARD_SUBMAP_SIZE && m_iSubMapLength== STANDARD_SUBMAP_SIZE );
		return true;
	}
	else
	{
		int w, l;
		m_ReachableMap[ iSubMapID ].GetImageSize(w, l);
		if( w != m_iSubMapWidth || l != m_iSubMapLength || m_fPixelSize!=m_ReachableMap[ iSubMapID ].GetPixelSize() )
			return false;
		else
			return true;
	}

}

bool CNPCMoveMap::LoadSubDeltaHeightMap(int u, int v, FILE* pFileToLoad)
{
	if( u >= m_iWidth || u < 0 || v>= m_iLength || v < 0 )
		return false;
	
	int iSubMapID = v*m_iWidth + u;

	if(!m_DeltaHeightMap[iSubMapID ].Load(pFileToLoad))
		return false;

	if(m_iSubMapWidth == -1)
	{
		m_DeltaHeightMap[ iSubMapID ].GetImageSize(m_iSubMapWidth, m_iSubMapLength);
		m_fPixelSize = m_DeltaHeightMap[ iSubMapID ].GetPixelSize();
		m_bStandardSubmap = ( m_iSubMapWidth==STANDARD_SUBMAP_SIZE && m_iSubMapLength== STANDARD_SUBMAP_SIZE );
		return true;
	}
	else
	{
		int w, l;
		m_DeltaHeightMap[ iSubMapID ].GetImageSize(w, l);
		if( w != m_iSubMapWidth || l != m_iSubMapLength || m_fPixelSize!=m_ReachableMap[ iSubMapID ].GetPixelSize())
			return false;
		else
			return true;
	}
}

bool CNPCMoveMap::Save( FILE *pFileToSave )
{
	if(!pFileToSave) return false;
	
	DWORD dwWrite, dwWriteLen;
	
	// write the Version info
	dwWrite=NPCMOVEMAP_VER;
	dwWriteLen = fwrite(&dwWrite, 1, sizeof(DWORD), pFileToSave);
	if(dwWriteLen != sizeof(DWORD))
		return false;

	// write the buf size. To note is this buf size isn't include m_ReachableMap and m_DeltaHeightMap data
	DWORD BufSize= 4*sizeof(int)+4*sizeof(float) ;
	dwWrite= BufSize;
	dwWriteLen = fwrite(&dwWrite, 1, sizeof(DWORD), pFileToSave);
	if(dwWriteLen != sizeof(DWORD))
		return false;
	
	// write the following info in order
	// 1. m_iWidth
	// 2. m_iLength
	// 3. m_iSubMapWidth
	// 4. m_iSubMapLength
	// 5. m_fPixelSize
	// 6. m_vOrigin
	
	UCHAR *buf=new UCHAR[BufSize];
	int cur=0;
	* (int *) (buf+cur) = m_iWidth;
	cur+=sizeof(int);
	
	* (int *) (buf+cur) = m_iLength;
	cur+=sizeof(int);

	* (int *) (buf+cur) = m_iSubMapWidth;
	cur+=sizeof(int);

	* (int *) (buf+cur) = m_iSubMapLength;
	cur+=sizeof(int);

	* (float *) (buf+cur) = m_fPixelSize;
	cur+=sizeof(float);	

	* (float *) (buf+cur) = m_vOrigin.x;
	cur+=sizeof(float);	
	* (float *) (buf+cur) = m_vOrigin.y;
	cur+=sizeof(float);	
	* (float *) (buf+cur) = m_vOrigin.z;
	cur+=sizeof(float);	

	dwWriteLen = fwrite(buf, 1, BufSize, pFileToSave);
	if(dwWriteLen != BufSize)
	{
		delete [] buf;
		return false;
	}
	
	// write the map data : m_ReachableMap and m_DeltaHeightMap
	for(int i=0; i< m_iWidth * m_iLength; i++)
	{
		if(!m_ReachableMap[i].Save(pFileToSave) || ! m_DeltaHeightMap[i].Save(pFileToSave))
			return false;
	}
	
	return true;
	
}

bool CNPCMoveMap::Load( FILE *pFileToLoad )
{
	if(!pFileToLoad) return false;
	
	DWORD dwRead, dwReadLen;

	// Read the Version
	dwReadLen = fread(&dwRead, 1, sizeof(DWORD), pFileToLoad);
	if(dwReadLen != sizeof(DWORD))
		return false;

	if(dwRead==NPCMOVEMAP_VER)
	{
		// Current Version
		
		// Read the buf size
		dwReadLen = fread(&dwRead, 1, sizeof(DWORD), pFileToLoad);
		if(dwReadLen != sizeof(DWORD))
			return false;
		int BufSize=dwRead;
		UCHAR * buf = new UCHAR[BufSize];

		// Read the data
		dwReadLen = fread(buf, 1, BufSize, pFileToLoad);
		if( dwReadLen != BufSize )
		{
			delete [] buf;
			return false;
		}
			
		Release();			// Release the old data

		int cur=0;
		m_iWidth=* (int *) (buf+cur);
		cur+=sizeof(int);
		m_iLength=* (int *) (buf+cur);
		cur+=sizeof(int);
		
		m_iSubMapWidth=* (int *) (buf+cur);
		cur+=sizeof(int);
		m_iSubMapLength=* (int *) (buf+cur);
		cur+=sizeof(int);

		m_fPixelSize=* (float *) (buf+cur);
		cur+=sizeof(float);

		m_vOrigin.x=* (float *) (buf+cur);
		cur+=sizeof(float);
		m_vOrigin.y=* (float *) (buf+cur);
		cur+=sizeof(float);
		m_vOrigin.z=* (float *) (buf+cur);
		cur+=sizeof(float);
		
		delete [] buf;

		int iSize = m_iWidth * m_iLength;
		m_ReachableMap = new CBitImage[iSize];
		m_DeltaHeightMap = new CBlockImage<FIX16> [iSize];

		for(int i = 0; i < iSize; i++)
		{
			if( !m_ReachableMap[i].Load(pFileToLoad) || !m_DeltaHeightMap[i].Load(pFileToLoad))
				return false;
		}
	
		return true;
	}		// Version is current version!
	else
		return false;
	
}