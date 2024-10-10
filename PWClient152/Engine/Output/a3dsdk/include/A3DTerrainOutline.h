/*
 * FILE: A3DTerrainOutline.h
 *
 * DESCRIPTION: Class representing for terrain outline in A3D Engine;
 *
 * CREATED BY: Hedi, 2004/8/17
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTERRAINOUTLINE_H_
#define _A3DTERRAINOUTLINE_H_

#include "A3DTypes.h"
#include "A3DVertex.h"

class A3DDevice;
class A3DCamera;
class A3DStream;
class A3DTexture;
class A3DViewport;
class AFile;

typedef struct _OUTLINESAMPLE
{
	float			top;		// top of the outline in the world
	float			d;			// distance of the outline from the projection point

} OUTLINESAMPLE;

typedef struct _OUTLINESEGMENT
{
	int				start;
	int				num;
	OUTLINESAMPLE *	pSamples;

} OUTLINESEGMENT;

typedef struct _OUTLINECLUSTER
{
	int					numSegments;			// number of segments in this cluster
	OUTLINESEGMENT *	pSegments;				// segments

	int					nCX;					// x index of this cluster
	int					nCY;					// y index of this cluster

	bool				bToRelease;				// flag used by cache algorithm

} OUTLINECLUSTER;

class A3DTerrainOutline
{
private:
	A3DDevice *			m_pA3DDevice;

	AFile *				m_pFileOutline;			// outline file pointer

	A3DCamera *			m_pHostCamera;			// camera with which the outline moves
	A3DCOLOR			m_colorFog;				// color of the terrain outline
	A3DVECTOR3 *		m_pSampleDir;			// delta direction of each sample

	A3DLVERTEX *		m_pOutlines;			// the triangle strips of outline
	int					m_nNumLT;				// number of samples of LT stream after get rid of sharp jump
	int					m_nNumRT;				// number of samples of RT stream after get rid of sharp jump
	int					m_nNumLB;				// number of samples of LB stream after get rid of sharp jump
	int					m_nNumRB;				// number of samples of RB stream after get rid of sharp jump
	A3DStream *			m_pStreamLT;			// four corner seen outline streams
	A3DStream *			m_pStreamLB;			// four corner seen outline streams
	A3DStream *			m_pStreamRT;			// four corner seen outline streams
	A3DStream *			m_pStreamRB;			// four corner seen outline streams

	A3DTexture *		m_pOutlineTexture;		// texture used to make outline more real

	// data describes the outline files.
	A3DVECTOR3			m_vecOrigin;			// the origin position sampling start
	
	int					m_nNumSamples;			// number of samples on the terrain outline	of one cluster
	int					m_nMaxSamples;			// maximum of samples to put in the outlines
	float				m_vSampleRad;			// the covering degree of one sample

	// information about the terrain which outline is generated from
	int					m_nTerrainWidth;		// terrain grid number in x
	int					m_nTerrainHeight;		// terrain grid number in z
	float				m_vTerrainMaxHeight;	// terrain's max height 
	float				m_vTerrainGridSize;		// terrain's grid size

	float				m_vStepSize;			// the step size of the sample
	float				m_vProjStart;			// near projection distance, which is the projection plane's distance
	float				m_vProjEnd;				// far projection distance
	int					m_nNumClustersX;		// number of projection clusters in the world along x axis
	int					m_nNumClustersZ;		// number of projection clusters in the world along z axis
	float				m_vHeightLow;			// lowest sample height
	float				m_vHeightMiddle;		// middle sample height
	float				m_vHeightHigh;			// highest sample height
	DWORD *				m_pClusterOffsets;		// each cluster's offset in the outline file

	float				m_vViewRadius;			// current view radius
	
	OUTLINECLUSTER		m_ClustersLast[4];		// old clusters of last frame
	OUTLINECLUSTER 		m_Clusters[4];			// four corner cluster used to do bilinear interpolation

protected:
	bool CreateOutlines();
	bool ReleaseOutlines();

	// Load one cluster which center is (nCX, nCY)
	bool LoadCluster(OUTLINECLUSTER& cluster, int nCX, int nCY);
	bool ReleaseCluster(OUTLINECLUSTER& cluster);
	bool FindClusterFromCache(OUTLINECLUSTER& cluster, int nCX, int nCY);

	bool ConstructOutline(const OUTLINECLUSTER& cluster, const A3DVECTOR3& vecCenter, const A3DVECTOR3& vecClusterCenter, A3DLVERTEX * pOutlines, int& nNum);
	inline void AddToOutlines(A3DLVERTEX& vert, const A3DVECTOR3& vecTop)
	{
		vert.x = vecTop.x;
		vert.y = vecTop.y;
		vert.z = vecTop.z;
	}

public:
	A3DTerrainOutline();
	~A3DTerrainOutline();

	bool Init(A3DDevice * pA3DDevice, A3DCamera * pHostCamera, A3DCOLOR colorFog, const char * szOutlineTex=NULL);
	bool Release();

	bool Load(const char * szOutlineFile, const A3DVECTOR3& vecCenter);

	bool Update();
	bool Render(A3DViewport * pCurrentViewport);
	bool RenderForReflected(A3DViewport * pCurrentViewport);

public:
	inline void SetHostCamera(A3DCamera * pHostCamera) { m_pHostCamera = pHostCamera; }
	inline A3DCamera * GetHostCamera() { return m_pHostCamera; }
	inline void SetOrigin(const A3DVECTOR3& vecOrigin) { m_vecOrigin = vecOrigin; }
	inline const A3DVECTOR3& GetOrigin() { return m_vecOrigin; }
	inline void SetViewRadius(float vRadius) { m_vViewRadius = vRadius; }
};

#endif//_A3DTERRAINOUTLINE_H_