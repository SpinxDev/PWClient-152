/*
 * FILE: A3DSkinRender.h
 *
 * DESCRIPTION: A3D skin model render base class
 *
 * CREATED BY: liu chenglong, 20/2/2012
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKINRENDER_BASE_H_
#define _A3DSKINRENDER_BASE_H_

#include "AString.h"
#include "vector.h"

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

class A3DEngine;
//class A3DDevice;
class A3DSkin;
class A3DHLSL;
//class A3DMeshBase;
class A3DSkinModel;
class A3DViewport;
//class A3DTexture;
class A3DSkinModelRenderModifier;
class A3DReplaceHLSL;
//struct A3DSkinMeshRef;
//class A3DVertexShader;
//class A3DPixelShader;
//class A3DVertexDecl;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

class A3DReplaceHLSL
{
public:
	struct REP_PARAM
	{
		REP_PARAM(const char* szOriginalParam, const char* szReqParam)
			: strOriginalParam(szOriginalParam)
			, strReqParam(szReqParam)
		{

		}
		AString strOriginalParam;
		AString strReqParam;
	};

	typedef abase::vector<REP_PARAM> RepParams;

	//	本函数必须返回一个有效的指针，否则就没有必要使用A3DReplaceHLSL接口
	virtual A3DHLSL* GetReplaceHLSL() const = 0;
	virtual bool IsNeedReplaceTexture(const AString& strSrcParam, const AString& strSrcSemantic, AString* pReqParamName) const = 0;

	RepParams GetNeedRepTextureParams() const;
};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkinRender
//
///////////////////////////////////////////////////////////////////////////

class A3DSkinRenderBase : public A3DObject
{
public:
	// Render flags
	static const DWORD			RENDER_FLAG_FOR_SILHOUETTE		= 0x1;

public:		//	Constructors and Destructors
	A3DSkinRenderBase() : m_nRenderFlags(0) {}
	virtual ~A3DSkinRenderBase() {}

	//	Initialize object
	virtual bool Init(A3DEngine* pA3DEngine) = NULL;
	//	Release object
	virtual void Release() = NULL;

	//	Register skin which is ready to be rendered.
	virtual bool RegisterRenderSkin(A3DViewport* pViewport, A3DSkin* pSkin, A3DSkinModel* pSkinModel) = NULL;
	//	Register skin model modifier. They will take effect before&after forword rendering
	virtual bool RegisterRenderSkinModelModifier(A3DSkinModel* pSkinModel, A3DSkinModelRenderModifier* pModifier) = NULL;
	//	Render all skin models
	virtual bool Render(A3DViewport* pViewport, bool bRenderAlpha=true) = NULL;
	//	Render alpha skin models
	virtual bool RenderAlpha(A3DViewport* pViewport) = NULL;
	//	Render Raw
	virtual bool RenderRaw(A3DViewport* pViewport, A3DHLSL* pHLSLSkin, A3DHLSL* pHLSLRigid) = NULL;
	//	Render Bloom
	virtual bool RenderBloomMeshes(A3DViewport* pViewport, float fBrightScale) = NULL;
	virtual void ResetBloomMeshes() = NULL;
	//	Reset render information, this function should be called every frame
	virtual void ResetRenderInfo(bool bAlpha) = NULL;

	//	Render specified skin model at once
	virtual bool RenderSkinModelAtOnce(A3DViewport* pViewport, A3DSkinModel* pSkinModel, DWORD dwFlags, const A3DReplaceHLSL* pRepHLSL) = NULL;

	void SetRenderFlags(const DWORD nFlags) { m_nRenderFlags = nFlags; }
	DWORD GetRenderFlags() const { return m_nRenderFlags; }
	void AddRenderFlags(const DWORD nFlags) { m_nRenderFlags |= nFlags; }
	void RemoveRenderFlags(const DWORD nFlags) { m_nRenderFlags &= ~nFlags; }
	bool CheckRenderFlags(const DWORD nFlags) { return ((m_nRenderFlags & nFlags) != 0); }

protected:
	DWORD				m_nRenderFlags;
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DSKINRENDER_BASE_H_

