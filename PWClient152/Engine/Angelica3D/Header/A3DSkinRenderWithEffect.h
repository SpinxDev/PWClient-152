/*
 * FILE: A3DSkinRender.h
 *
 * DESCRIPTION: A3D skin model render class
 *
 * CREATED BY: duyuxin, 2005/3/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKINRENDER_WITHEFFECT_H_
#define _A3DSKINRENDER_WITHEFFECT_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "AList2.h"
#include "AArray.h"
#include "A3DSkinRenderBase.h"

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
class A3DDevice;
class A3DSkin;
class A3DMeshBase;
class A3DSkinModel;
class A3DSkinModelRenderModifier;
class A3DViewport;
class A3DTexture;
struct A3DSkinMeshRef;
class A3DVertexShader;
class A3DPixelShader;
class A3DVertexDecl;
class A3DHLSL;
class A3DHLSLCore;
class A3DEffect;
class A3DSkinRender;
///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkinRender
//
///////////////////////////////////////////////////////////////////////////

class A3DSkinRenderWithEffect : public A3DSkinRenderBase
{
public:		//	Types
	//	Render mesh node
	struct MESHNODE
	{
		A3DSkinModel*	pSkinModel;		//	Skin model object
		A3DSkin*		pSkin;			//	Skin object
		float			fDist;			//	Distance between camera and mesh (used by alpha mesh)
		A3DSkinMeshRef*	pMeshRef;		//	Mesh reference
	};
	typedef APtrList<MESHNODE*> MeshNodeList;

	//	Render slot
	struct RENDERSLOT
	{
		DWORD			dwTexture;			//	Texture ID
		A3DHLSLCore*	pHLSLCore;			//	A3DEffect's HLSLCore

		MeshNodeList	SkinMeshList;		//	Skin mesh list
		MeshNodeList	SkinMeshDLList;		//	Skin mesh with dynamic light list
		MeshNodeList	RigidMeshList;		//	Rigid mesh list
		MeshNodeList	RigidMeshDLList;	//	Rigid mesh with dynamic light list
		MeshNodeList	SuppleMeshList;		//	Supple mesh list
		MeshNodeList	MorphMeshList;		//	Morph mesh list
		MeshNodeList	MuscleMeshList;		//	Muscle mesh list
		MeshNodeList	ClothMeshList;		//	Cloth mesh list
	};

	friend class A3DSkinMan;

public:		//	Constructors and Destructors

	A3DSkinRenderWithEffect();
	virtual ~A3DSkinRenderWithEffect();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DEngine* pA3DEngine);
	//	Release object
	virtual void Release();

	//	Register skin which is ready to be rendered.
	virtual bool RegisterRenderSkin(A3DViewport* pViewport, A3DSkin* pSkin, A3DSkinModel* pSkinModel);
	
	//	Register skin model modifier. They will take effect before&after forword rendering
	virtual bool RegisterRenderSkinModelModifier(A3DSkinModel* pSkinModel, A3DSkinModelRenderModifier* pModifier);

	//	Render all skin models
	virtual bool Render(A3DViewport* pViewport, bool bRenderAlpha=true);
	//	Render alpha skin models
	virtual bool RenderAlpha(A3DViewport* pViewport);
	//	Render Raw
	virtual bool RenderRaw(A3DViewport* pViewport, A3DHLSL* pHLSLSkin, A3DHLSL* pHLSLRigid);

	//	Reset render information, this function should be called every frame
	virtual void ResetRenderInfo(bool bAlpha);
	
	//	Render specified skin model at once
	virtual bool RenderSkinModelAtOnce(A3DViewport* pViewport, A3DSkinModel* pSkinModel, DWORD dwFlags, const A3DReplaceHLSL* pRepHLSL);

	//	Set / Get color operation
	void SetColorOP(A3DTEXTUREOP op) { m_ColorOP = op; }
	A3DTEXTUREOP GetColorOP() { return m_ColorOP; }

	//	Get A3D engine object
	A3DEngine* GetA3DEngine() { return m_pA3DEngine; }
	//	Get A3D device object
	A3DDevice* GetA3DDevice() { return m_pA3DDevice; }

	virtual bool RenderBloomMeshes(A3DViewport* pViewport, float fBrightScale);
	virtual void ResetBloomMeshes();

protected:	//	Attributes
	
	A3DEngine*		m_pA3DEngine;		//	A3D engine object
	A3DDevice*		m_pA3DDevice;		//	A3D device object
	A3DSkinMan*		m_psm;				//	A3D skin model manager

	APtrArray<RENDERSLOT*>	m_aRenderSlots;		//	Render slot
	APtrArray<MESHNODE*>	m_aFreeMeshNodes;	//	Free mesh node pool
	APtrArray<MESHNODE*>	m_aFreeAlphaNodes;	//	Free alpha mesh node pool
	MeshNodeList			m_AlphaMeshList;	//	Alpha mesh list
    APtrArray<MESHNODE*>	m_BorderMeshList;	//	Border mesh list
	APtrArray<MESHNODE*>	m_BloomMeshes;		// bloom Meshes
	

	int				m_iRenderSlotCnt;	//	Render slot counter
	int				m_iFreeMNCnt;		//	Free mesh node counter
	int				m_iFreeAMNCnt;		//	Free alpha mesh node counter
	A3DTEXTUREOP	m_ColorOP;			//	Color operation
	A3DVECTOR4		m_vVSConst0;		//	Vertex shader const 0
	A3DCULLTYPE		m_CurCull;			//	Current cull mode
	DWORD			m_dwAmbient;		//	Current ambient
	A3DLIGHTPARAM	m_DirLightParams;	//	Dir light params

	A3DHLSL*			m_pBloomMeshPS;
	A3DVertexDecl*		m_pRigidMeshDecl;
	A3DVertexDecl*		m_pRigidMeshTanDecl;
	A3DVertexDecl*		m_pSkinMeshDecl;	 // 普通模型的顶点声明
	A3DVertexDecl*		m_pSkinMeshTanDecl;	 // 带有Tangent数据的新材质模型的顶点声明

	struct MODIFIERITEM
	{
		A3DSkinModel* pSkinModel;
		A3DSkinModelRenderModifier* pModifier;
	};
	AArray<MODIFIERITEM> m_aModiferList;
    A3DHLSL*            m_pPSBorder;

	A3DSkinRender*		m_pOldSkinRender;
	bool				m_bUsingOldSkinRender;
protected:	//	Operations

	//	
	bool ApplyTextureOrEffect(A3DSkinModel* pModel, 
		const MESHNODE* pMeshNode, 
		A3DTexture* pTexture, 
		const A3DCOMMCONSTTABLE* pEnvCommConstTab, 
		const A3DReplaceHLSL* pRepHLSL, 
		DWORD dwTexture, 
		DWORD dwFlags, 
		A3DTexture** ppLastTexture);
	//	Get alpha mesh insert place
	virtual ALISTPOSITION GetAlphaMeshInsertPlace(MeshNodeList& meshList, MESHNODE* pNewNode);
	//	Test is mesh is alpha mesh
	virtual bool IsModelAlphaMesh(A3DSkinModel* pSkinModel, A3DSkin* pSkin, A3DSkinMeshRef* pMeshRef) const;

	void SaveLightInformation();
	void RestoreLightInformation();
	void SetupVPTMConstant(A3DViewport* pViewport);

	//	Release all resources
	void ReleaseAllResources();
	//	Allocate a new mesh node
	MESHNODE* AllocMeshNode(bool bAlpha);
	//	Select a proper render slot for specified mesh
	RENDERSLOT* SelectRenderSlot(A3DSkin* pSkin, A3DMeshBase* pMesh);

	////	Apply mesh material (traditional version)
	//void ApplyMaterial(const MESHNODE* pNode);
	////	Apply mesh material (Vertex Shader version)
	//void ApplyVSMaterial(const MESHNODE* pNode);
	////	Apply default mesh material (traditional version)
	//void ApplyDefMaterial(float fTransparent);
	////	Apply default mesh material (Vertex Shader version)
	//void ApplyDefVSMaterial(float fTransparent);

	//	Register mesh which is ready to be rendered.
	bool RegisterRenderMesh(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin, A3DSkinMeshRef* pMeshRef);

	// render Skin meshes and Rigid meshes
	bool RenderMeshes(A3DViewport* pViewport, MeshNodeList& mnl, bool bDynLight, int eIdx);

	////	Render skin meshes of specified slot using vertex shader
	//bool RenderSkinMeshes_VS(A3DViewport* pViewport, RENDERSLOT* pSlot);
	////	Render skin meshes of specified slot using indexed vertex matrix
	//bool RenderSkinMeshes_IVM(A3DViewport* pViewport, RENDERSLOT* pSlot);
	////	Render skin meshes of specified slot using software
	//bool RenderSkinMeshes_SW(A3DViewport* pViewport, RENDERSLOT* pSlot);
	//	Render rigid meshes of specified slot
	//bool RenderRigidMeshes(A3DViewport* pViewport, MeshNodeList& mnl, bool bDynLight);
	////	Render supple meshes of specified slot
	//bool RenderSuppleMeshes(A3DViewport* pViewport, RENDERSLOT* pSlot);
	////	Render morph meshes of specified slot using vertex shader
	//bool RenderMorphMeshes_VS(A3DViewport* pViewport, RENDERSLOT* pSlot);
	////	Render morph meshes of specified slot using software
	//bool RenderMorphMeshes_SW(A3DViewport* pViewport, RENDERSLOT* pSlot);
	////	Render muscle meshes of specified slot using vertex shader
	//bool RenderMuscleMeshes_VS(A3DViewport* pViewport, RENDERSLOT* pSlot);
	////	Render muscle meshes of specified slot using software
	//bool RenderMuscleMeshes_SW(A3DViewport* pViewport, RENDERSLOT* pSlot);
	//	Render cloth meshes
	bool RenderClothMeshes(A3DViewport* pViewport, RENDERSLOT* pSlot);
	//	Render alpha meshes
	bool RenderAlphaMeshes(A3DViewport* pViewport);
    bool RenderBorderMeshes(A3DViewport* pViewport);

	//	Render skin item in specified skin model
	bool RenderSkinAtOnce(A3DViewport* pViewport, A3DSkinModel* pModel, int iSkinItem, DWORD dwFlags, const A3DReplaceHLSL* pRepHLSL);
	//	Apply mesh texture
	void ApplyMeshTexture(A3DTexture** ppLastTex, DWORD& dwTexID, const MESHNODE* pNode, const A3DCOMMCONSTTABLE* pEnvCommConstTab);

	void ReplaceHLSLParam(const A3DReplaceHLSL* pRepHLSL, A3DSkinModel* pSkinModel, A3DEffect* pSrcEffect);
	//	Apply mesh material
	//void ApplyMeshMaterial(MESHNODE* pNode, DWORD dwFlags, bool bVS);
	void ApplyMaterial(const MESHNODE* pNode);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DSKINRENDER_WITHEFFECT_H_

