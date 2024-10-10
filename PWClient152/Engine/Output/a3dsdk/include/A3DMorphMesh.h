/*
 * FILE: A3DMorphMesh.h
 *
 * DESCRIPTION: A3D morph mesh class
 *
 * CREATED BY: duyuxin, 2003/11/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMORPHMESH_H_
#define _A3DMORPHMESH_H_

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DTrackData.h"
#include "AString.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DMeshBase;
class A3DSkinModel;
class A3DStream;
class A3DSkin;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DMorphMesh
//
///////////////////////////////////////////////////////////////////////////

class A3DMorphMesh
{
public:		//	Types

	//	Struct used to save and load skin mesh
	struct MORPHDATA
	{
		int		iNumChannel;	//	Number of morph channel
		int		iAnimStart;		//	Animation start frame
		int		iAnimEnd;		//	Animation end frame
		int		iAnimFPS;		//	Animation FPS
	};

	//	Morph channel
	struct CHANNEL
	{
		AString			strName;		//	Channel name
		A3DMCVERTEX*	aOriginVerts;	//	Original vertex data
		A3DStream*		pStream;		//	Channel stream
		A3DFloatTrack*	pWeightTrack;	//	Channel weight track

		//	Used to sort channel
		float			fCurWeight;		//	Current weight
	};

public:		//	Constructors and Destructors

	A3DMorphMesh();
	virtual ~A3DMorphMesh();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(A3DMeshBase* pMesh);
	//	Release object
	void Release();

	//	Load morph mesh data form file
	bool Load(AFile* pFile, A3DSkin* pSkin);
	//	Save morph mesh data from file
	bool Save(AFile* pFile);

	//	Create channel weight track
	A3DFloatTrack* CreateWeightTrack(const char* szName, int nNumKeys, int nFrameRate, int iNumSegment, A3DSkin* pSkin);
	//	Fill channel data into vertex shader
	bool ApplyVSChannelData(A3DSkinModel* pSkinModel);
	//	Clear VS channel data
	void ClearVSChannelData();
	//	Calculate current channels' weights
	BYTE* CalcCurrentChannelWeights(A3DSkinModel* pSkinModel);

	//	Get approximate mesh data size
	int GetDataSize() { return m_iDataSize; }
	//	Get bound bone
	int GetBoundBone() { return m_iBoundBone; }
	//	Get animation start frame
	int GetAnimStartFrame() { return m_iAnimStart; }
	//	Get animation end frame
	int GetAnimEndFrame() { return m_iAnimEnd; }
	//	Set animation frame range
	void SetAnimFrameRange(int iStart, int iEnd) { m_iAnimStart = iStart; m_iAnimEnd = iEnd; }
	//	Get animation FPS
	int GetAnimFPS() { return m_iAnimFPS; }
	//	Set animation FPS
	void SetAnimFPS(int iFPS) { m_iAnimFPS = iFPS; }
	//	Get channel number
	int GetChannelNum() { return m_aChannels.GetSize(); }
	//	Get channel by index
	CHANNEL* GetChannel(int n) { return (CHANNEL*)m_aChannels[n]; }
	//	Get maximum number of morph channel can be used to render mesh
	int GetMaxMorphChannelNum() { return m_iMaxMCNum; }
	//	Get maximum number of vertex constant registers hold by morph mesh
	int GetMaxVSConstNumUsedByMorph() { return (m_iMaxMCNum + 3) >> 2; }
	//	Get current number of vertex constant registers hold by morph mesh
	int GetCurVSConstNumUsedByMorph() { return (m_iActiveMCNum + 3) >> 2; }

	//	Set / Get number of currently active morph channel
	void SetActiveMorphChannelNum(int iNum);
	int GetActiveMorphChannelNum() { return m_iActiveMCNum; }

	//	Below functions are used by editor or exportor
	//	Add a channel
	int AddChannel(CHANNEL* pChannel) { return m_aChannels.Add(pChannel); }
	//	Set a channel sort array
	void SetChannelSorts(BYTE* pBuf) { m_aChannelSorts = pBuf; }

protected:	//	Attributes

	A3DMeshBase*	m_pA3DMesh;			//	Mesh object
	BYTE*			m_aChannelSorts;	//	Sorted channel indices for every frame
	float*			m_aCurWeights;		//	Channel sort result of this frame

	APtrArray<CHANNEL*>	m_aChannels;	//	Channels

	int		m_iMaxMCNum;	//	Maximum number of morph channel can be used to render mesh
	int		m_iActiveMCNum;	//	Number of currently active morph channel
	int		m_iAnimStart;	//	Animation start frame
	int		m_iAnimEnd;		//	Animation end frame
	int		m_iAnimFPS;		//	Animation FPS
	int		m_iBoundBone;	//	Index of bone this mesh is bound to
	int		m_iDataSize;	//	Approximate mesh data size

protected:	//	Operations

	//	Build morph mesh data
	bool BuildMorphData();
	//	Load a channel's data from file
	bool LoadChannel(AFile* pFile, A3DSkin* pSkin);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DMORPHMESH_H_

