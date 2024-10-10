/*
 * FILE: A3DDeviceRSCache.h
 *
 * DESCRIPTION: The important class which represent the hardware in A3D Engine
 *
 * CREATED BY: liyi 2012, 6, 28
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DDEVICERSCACHE_H_
#define _A3DDEVICERSCACHE_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "hashmap.h"

///////////////////////////////////////////////////////////////////////////
//	
//	class A3DDeviceRSCache
//	
///////////////////////////////////////////////////////////////////////////

class A3DDeviceRSCache
{
public:		//	Types

	typedef abase::hash_map<D3DRENDERSTATETYPE, DWORD> RenderStateMap;
	typedef abase::hash_map<D3DTEXTURESTAGESTATETYPE, DWORD> TextureStateMap;
	typedef abase::hash_map<D3DSAMPLERSTATETYPE, DWORD> SamplerStateMap;

	enum
	{
		NUM_LIGHT		= 8,	//	Number of light
		NUM_WORLD_MAT	= 256,	//	Number world matrix
		NUM_STRMSOURCE	= 16,	//	Number of stream source
	};

	//	Light data
	struct LIGHTDATA
	{
		D3DLIGHT9	data;		//	Light data
		BOOL		bEnable;	//	Enable flag
	};

	//	Texture data
	struct TEXTUREDATA
	{
		A3DMATRIX4		matTrans;	//	Transform matrix
		TextureStateMap	TSMap;		//	Texture state map
	};

	//	Stream source
	struct STREAMSOURCE
	{
		IDirect3DVertexBuffer9*	pVB;
		UINT OffsetInBytes;
		UINT Stride;
	};

	friend class A3DDevice;

public:		//	Constructors and Destructors

	A3DDeviceRSCache();
	~A3DDeviceRSCache();

public:		//	Attributes

public:		//	Operations

	//	Reset state cache
	bool Reset(A3DDevice* pA3DDevice);

protected:	//	Attributes

	A3DDevice*				m_pA3DDevice;		//	A3D device object

	A3DMATRIX4				m_matView;						//	View matrix
	A3DMATRIX4				m_matProj;						//	Projection matrix
	A3DMATRIX4				m_aWorldMats[NUM_WORLD_MAT];	//	World matrices
	D3DMATERIAL9			m_Material;						//	Material
	LIGHTDATA				m_aLights[NUM_LIGHT];			//	Lights' data
	TEXTUREDATA				m_aTexStages[MAX_TEX_LAYERS];	//	Texture stages' data
	LPDIRECT3DBASETEXTURE9	m_aTextures[MAX_SAMPLE_LAYERS];	//	Texture objects
	LPDIRECT3DBASETEXTURE9	m_aVsTextures[VS_SAMPLE_LAYERS_NUM];
	FLOAT					m_fNPatchMode;					//	N-Patch mode
	DWORD					m_dwFVF;						//	FVF flag
	A3DCOLORVALUE			m_aClipPlanes[MAX_CLIP_PLANES];	//	Clip planes
					
	RenderStateMap			m_RSMap;						//	Render state map
	SamplerStateMap			m_aSSMaps[MAX_SAMPLE_LAYERS];	//	Sampler stage state maps
	SamplerStateMap			m_aVsSSMaps[VS_SAMPLE_LAYERS_NUM];
	STREAMSOURCE			m_aStrmSrcs[NUM_STRMSOURCE];	//	Stream sources
	IDirect3DIndexBuffer9*	m_pIndices;						//	Index stream

	LPDIRECT3DVERTEXSHADER9			m_pVS;			//	Vertex shader
	LPDIRECT3DPIXELSHADER9			m_pPS;			//	Pixel shader
	LPDIRECT3DVERTEXDECLARATION9	m_pVertexDecl;	//	Vertex declaration

protected:	//	Operations

	//	Apply all states to device
	void ApplyAllStates();
};

#endif		//	_A3DDEVICERSCACHE_H_