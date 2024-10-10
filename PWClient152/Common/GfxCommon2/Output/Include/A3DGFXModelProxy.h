/*
* FILE: A3DGFXModelProxy.h
*
* DESCRIPTION: a render proxy for client's skin model
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/4/19
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#ifndef _A3DGFXModelProxy_H_
#define _A3DGFXModelProxy_H_

#include "EC_ModelBlur.h"

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

class ModelState;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DGFXModelProxy
//	This class represent a SkinModel created and held by the client
//	When this element comes out, the client's skin model may be hided or not (by setting)
//	When this element dies out, the client's skin model may be shown again or not(by setting)
//	
///////////////////////////////////////////////////////////////////////////

class A3DGFXModelProxy : public A3DGFXElement
{
public:		//	Types

	typedef A3DGFXElement base_class;

public:		//	Constructor and Destructor

	A3DGFXModelProxy(A3DGFXEx* pGfx);
	virtual ~A3DGFXModelProxy(void);

	A3DGFXModelProxy& operator = (const A3DGFXModelProxy& rhs);

public:		//	Attributes

public:		//	Operations

	void SetCurrentState(ModelState* pState);
	void SetClientModelVisibleById(bool bVisible);
	inline bool IsOriginModelInVisible() const { return m_bMakeOriginModelInVisible; }

	inline bool IsUseSkinModelStaticFrame() const { return m_bUseSkinModelTMs; }

	inline bool IsIncludeChildModels() const { return m_bIncludeChildModels; }

	bool ApplyClientModelPlaySpeed();
	void RestoreClientModelPlaySpeed();

private:	//	Attributes

	ModelState* m_pCurState;

	//	Runtime state
	float m_fLastFrameValue;
	BlendTMNode m_Root;				// skinmodel and child models' matrices that should be remembered

	//	使得原始模型的正常渲染流程不渲染该模型
	bool m_bMakeOriginModelInVisible;
	bool m_bUseSkinModelTMs;
	bool m_bIncludeChildModels;

	bool	m_bLastActive;		//客户端模型由不可见恢复成可见，这时element已结束，但仍需要渲染一帧，因为客户端模型在下一帧才会渲染，否则会出现闪烁

public:
	virtual A3DSkinModel* GetSkinModel();

protected:	//	Operations

	bool CreateModelTickSpeedProperty();
	bool GetOrSetupSkinModelStaticFrameTMs(A3DSkinModel* pSKinModel, BlendTMNode* pOutCurTmp);
	virtual bool Init(A3DDevice* pDevice);
	virtual bool Play();
	virtual bool Stop();
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual bool Render(A3DViewport*);
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;
	virtual void PrepareRenderSkinModel();
	virtual void RenderSkinModel(A3DViewport* pView, A3DSkinModel* pSkinModel, A3DRenderTarget* pFrameBuffer);
	virtual void SkinModelRenderAtOnce(A3DSkinModel* pSkinModel, A3DViewport* pView, DWORD dwFlags, A3DReplaceHLSL* pRepHLSL);
	virtual bool GetAnimatableProperty(const char* name, GFXEleAnimtableProperty* pProperty/*out*/);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DGFXModelProxy_H_

