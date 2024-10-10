/*
 * FILE: EL_TerrainOutline.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2005/4/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EL_TERRAINOUTLINE_H_
#define _EL_TERRAINOUTLINE_H_

class A3DStream;
class A3DViewport;
class A3DTexture;

class CELTerrainOutline
{
private:
	A3DDevice *		m_pA3DDevice;			// device pointer

	float			m_vCellSize;			// size of each cell
	int				m_nWidth;				// cell width
	int				m_nHeight;				// cell height
	
	float			m_x;					// coordinates of left top corner in world space
	float			m_z;					// coordinates of left top corner in world space

	float *			m_pLowResHeights;		// the heights
	A3DStream *		m_pStreams[3];			// stream used when render
	int				m_nVertCount[3];		// vertex count of stream 1
	int				m_nFaceCount[3];		// face count of stream1
	
	A3DTexture *	m_pTexture;				// world texture
	float			m_vTOX;					// world texture u 0.0f pos
	float			m_vTOZ;					// world texture v 0.0f pos
	float			m_vTS;					// world texture u, v scale

	float			m_vStart;				// start distance of outline
	float			m_vEnd;					// end distance of outline
	
	int				m_nView;				// cell number of viewable range

protected:
	bool DrawPart(int sid, int x1, int x2, int y1, int y2, bool bUpdate);

public:
	CELTerrainOutline();
	~CELTerrainOutline();

	bool Init(A3DDevice * pA3DDevice, const char * szFile);
	bool Release();

	bool Render(A3DViewport * pViewport, bool bUpdate);

	bool SetStartAndEnd(float vStart, float vEnd);
	void SetStartOnly(float vStart) { m_vStart = vStart; }
};

#endif//_EL_TERRAINOUTLINE_H_
