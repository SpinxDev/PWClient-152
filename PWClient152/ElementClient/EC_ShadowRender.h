/*
 * FILE: EC_ShadowRender.h
 *
 * DESCRIPTION: a class for shadow rendering
 *
 * CREATED BY: Hedi, 2004/9/21
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _EC_SHADOWRENDER_H_
#define _EC_SHADOWRENDER_H_

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DGeometry.h"
#include "AArray.h"

class A3DDevice;
class A3DCamera;
class A3DOrthoCamera;
class A3DStream;
class A3DTexture;
class A3DViewport;
class A3DTerrain2;
class A3DRenderTarget;
class A3DPixelShader;
class A3DVertexShader;
class A3DLitModel;

// Vertex for shadow area
#define A3DSHADOWVERT_FVF		D3DFVF_XYZ | D3DFVF_DIFFUSE
struct A3DSHADOWVERTEX
{
	A3DSHADOWVERTEX() {}
	A3DSHADOWVERTEX(const A3DVECTOR3& _pos, A3DCOLOR _diffuse)
	{
		vPos = _pos;
		diffuse = _diffuse;
	}
	 
	A3DVECTOR3	vPos;			//	Position
	A3DCOLOR	diffuse;		//	Color
};

// shadow object rendering callback
typedef bool (*LPRENDERFORSHADOW)(A3DViewport *, void * pArg);

// what receive the shadows
enum SHADOW_RECEIVER
{
	SHADOW_RECEIVER_TERRAIN = 0x1,
	SHADOW_RECEIVER_LITMODEL = 0x2,
	SHADOW_RECEIVER_ALL	= 0xffffffff,
};

// class represent shadow renderer
class CECShadowRender
{
private:
	struct SHADOWER
	{
		A3DVECTOR3			vecCenter;
		A3DAABB				aabb;
		SHADOW_RECEIVER		shadowReceiver;
		LPRENDERFORSHADOW	pfnRenderForShadow;
		LPVOID				pArg;
	};

	bool				m_bCanDoShadow;				// flag indicates whether can cast shadow on this hardware

	bool				m_bCastShadow;				// flag indicates if cast shadow now.

	A3DTerrain2 *		m_pA3DTerrain;				// terrain shadow receiver

	int					m_nShadowMapSize;			// shadow map's size for the light
	A3DViewport *		m_pShadowMapViewport;		// viewport to render light's shadow map
	A3DOrthoCamera *	m_pShadowMapCamera;			// camera to do rendering of light's shadow map
	A3DRenderTarget *	m_pShadowMapTarget;			// render target to render light's shadow map

	int					m_nMaxVerts;
	int					m_nMaxFaces;				// max vertex and face used to show the shadow
	int					m_nVertCount;				// shadow stream vertex count used to show the shadow
	int					m_nFaceCount;				// shadow stream face count used to show the shadow
	int					m_nOccludeVertCount;		// verts that can create occlude things.
	int					m_nOccludeFaceCount;		// faces that can create occlude things.
	A3DVECTOR3 *		m_pVertsArea;				// verts position buffer to get verts out of terrain
	A3DStream *			m_pShadowStream;			// shadow stream

	A3DVertexShader *	m_pVertexShader;			// shadow vertex shader
	A3DPixelShader *	m_pShadowShader;			// shadow rendering shader
	
	AArray<SHADOWER, SHADOWER&> m_arrayShadowers;	// shadower array

protected:
	bool AllocResource();
	bool ReleaseResource();

	bool CreateStream(int nMaxVerts, int nMaxFaces);
	bool ReleaseStream();

	bool SetupStreamForShadower(A3DViewport * pViewport, const SHADOWER& shadower, const A3DVECTOR3& vecLightDir);

public:
	CECShadowRender();
	~CECShadowRender();

	bool Init(int nShadowMapSize);
	bool Release();

	bool AddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, SHADOW_RECEIVER shadowReceiver, LPRENDERFORSHADOW pfnRenderForShadow, LPVOID pArg);
	bool Render(A3DViewport * pViewport, const A3DVECTOR3& vecLightDir);
	bool ClearShadowers();

	bool SetCastShadowFlag(bool bFlag);
	inline bool GetCastShadowFlag() { return m_bCastShadow; }
	
	inline int GetVertCount() { return m_nVertCount; }
	inline int GetFaceCount() { return m_nFaceCount; }

	inline void SetA3DTerrain(A3DTerrain2 * pTerrain)	{ m_pA3DTerrain = pTerrain; }

	inline bool CanDoShadow() { return m_bCanDoShadow; }
};

#endif//_EC_SHADOWRENDER_H_