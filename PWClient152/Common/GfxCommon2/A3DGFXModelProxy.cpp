/*
* FILE: A3DGFXModelProxy.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/4/19
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "A3DGFXModelProxy.h"
#include "A3DGFXInterface.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

static const char* _format_hideclientmodel	= "HideModel: %d";
static const char* _format_delay_for_fb		= "DelayRender: %d";
static const char* _format_usestaticframe	= "UseStatic: %d";
static const char* _format_includechild = "IncludeChild: %d";

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

class ModelState
{
	ModelState(const ModelState&);
	ModelState& operator = (const ModelState&);
public:
	explicit ModelState(A3DGFXModelProxy* pProxy)
		: m_pProxy(pProxy)
	{

	}
	virtual ~ModelState() = 0 {}

	virtual void OnTick(DWORD dwTickTime) {}
	virtual void OnPlay();
	virtual void OnStop() {}
	virtual void OnEnterState() {}
	virtual void OnLeaveState() {}
protected:
	A3DGFXModelProxy* GetParentProxy() const { return m_pProxy; }
private:
	A3DGFXModelProxy* m_pProxy;
};

class UnActivedState : public ModelState
{
public:
	UnActivedState(A3DGFXModelProxy* pProxy)
		: ModelState(pProxy)
	{

	}
	~UnActivedState() 
	{

	}
	
	virtual void OnTick(DWORD dwTickTime);
	virtual void OnStop();
	virtual void OnPlay() {}
};

class ActivedState : public ModelState
{
	bool m_bNeedResetSkinModelVisibleState;
	void ResetSkinModelVisibleState()
	{
		if (m_bNeedResetSkinModelVisibleState)
		{
			GetParentProxy()->SetClientModelVisibleById(true);
			m_bNeedResetSkinModelVisibleState = false;
		}
	}

	bool m_bNeedResetSkinModelTickSpeed;
	void ResetSkinModelTickSpeed()
	{
		if (m_bNeedResetSkinModelTickSpeed)
		{
			GetParentProxy()->RestoreClientModelPlaySpeed();
			m_bNeedResetSkinModelTickSpeed = false;
		}
	}
public:
	ActivedState(A3DGFXModelProxy* pProxy)
		: ModelState(pProxy)
		, m_bNeedResetSkinModelVisibleState(false)
		, m_bNeedResetSkinModelTickSpeed(false)
	{

	}
	~ActivedState()
	{
		ResetSkinModelVisibleState();
		ResetSkinModelTickSpeed();
	}

	virtual void OnTick(DWORD dwTickTime);
	virtual void OnStop();
	
	virtual void OnEnterState() 
	{
		// 处理代理模型是否显示
		if (GetParentProxy()->IsOriginModelInVisible())
		{
			GetParentProxy()->SetClientModelVisibleById(false);
			m_bNeedResetSkinModelVisibleState = true;
		}
	}
	
	virtual void OnLeaveState() 
	{
		ResetSkinModelVisibleState();
		ResetSkinModelTickSpeed();
	}
};

class FinishedState : public ModelState
{
public:
	FinishedState(A3DGFXModelProxy* pProxy)
		: ModelState(pProxy)
	{

	}
	~FinishedState()
	{

	}
};

void ModelState::OnPlay()
{
	GetParentProxy()->SetCurrentState(new UnActivedState(GetParentProxy()));
}

void UnActivedState::OnTick(DWORD dwTickTime) 
{
	if (GetParentProxy()->IsActive() && GetParentProxy()->IsVisible())
		GetParentProxy()->SetCurrentState(new ActivedState(GetParentProxy()));
}

void UnActivedState::OnStop()
{
	GetParentProxy()->SetCurrentState(new FinishedState(GetParentProxy()));
}

void ActivedState::OnTick(DWORD dwTickTime) 
{
	if (GetParentProxy()->IsFinished())
	{
		GetParentProxy()->SetCurrentState(new FinishedState(GetParentProxy()));
		return;
	}
	else if (!GetParentProxy()->IsVisible())
	{
		GetParentProxy()->SetCurrentState(new UnActivedState(GetParentProxy()));
		return;
	}

	// 设置代理模型的tick速度
	if (GetParentProxy()->ApplyClientModelPlaySpeed())
	{
		m_bNeedResetSkinModelTickSpeed = true;
	}
}

void ActivedState::OnStop()
{
	GetParentProxy()->SetCurrentState(new FinishedState(GetParentProxy()));
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DGFXModelProxy
//	
///////////////////////////////////////////////////////////////////////////

static const char * const PropertyName_ModelTickSpeed = "A3DGFXModelProxy_ModelTickSpeed";

A3DGFXModelProxy::A3DGFXModelProxy(A3DGFXEx* pGfx)
: base_class(pGfx)
, m_bMakeOriginModelInVisible(true)
, m_bIncludeChildModels(true)
, m_pCurState(NULL)
, m_fLastFrameValue(-1.0f)
, m_bUseSkinModelTMs(false)
, m_bLastActive(false)
{
	m_nEleType = ID_ELE_TYPE_MODEL_PROXY;
	m_pCurState = new UnActivedState(this);

	m_Shader.SrcBlend = A3DBLEND_SRCALPHA;
	m_Shader.DestBlend = A3DBLEND_INVSRCALPHA;

}

A3DGFXModelProxy::~A3DGFXModelProxy()
{
	delete m_pCurState;
}

A3DGFXModelProxy& A3DGFXModelProxy::operator = (const A3DGFXModelProxy& rhs)
{
	if (this == &rhs)
		return *this;

	_CloneBase(&rhs);
	Init(rhs.m_pDevice);
	m_bMakeOriginModelInVisible = rhs.m_bMakeOriginModelInVisible;
	m_bUseSkinModelTMs = rhs.m_bUseSkinModelTMs;
	m_bIncludeChildModels = rhs.m_bIncludeChildModels;
	return *this;
}

bool A3DGFXModelProxy::CreateModelTickSpeedProperty()
{
	// 如果load的时候发现没有这个属性，那就创建出来，下次存储的时候就会存到文件里了
	if (!m_AnimatedProperty.GetAnimatable(PropertyName_ModelTickSpeed))
	{
		if (A3DAnimatableBase* pAnimableBase = m_AnimatedProperty.AddNewAnimatable(PropertyName_ModelTickSpeed, A3DAnimatableUValue::FLOAT, A3DAnimatable::APPLY_NONE))
		{
			pAnimableBase->SetDefaultValue(A3DAnimatableUValue::FromFloat(1.0f));
			return true;
		}
	}

	return false;
}

bool A3DGFXModelProxy::Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion)
{
	Init(pDevice);

	if (pFileToLoad->IsBinary())
	{

	}
	else
	{
		char szLine[AFILE_LINEMAXLEN];
		DWORD dwReadLen;

		pFileToLoad->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		int iRead = 0;
		sscanf(szLine, _format_hideclientmodel, &iRead);
		m_bMakeOriginModelInVisible = iRead != 0;


		if (dwVersion >= 110)
		{
			pFileToLoad->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			int iRead = 0;
			sscanf(szLine, _format_usestaticframe, &iRead);
			m_bUseSkinModelTMs = iRead != 0;
		}

		if (dwVersion >= 120)
		{
			pFileToLoad->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			int iRead = 0;
			sscanf(szLine, _format_includechild, &iRead);
			m_bIncludeChildModels = iRead != 0;
		}
	}

	CreateModelTickSpeedProperty();

	return true;
}

bool A3DGFXModelProxy::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_bMakeOriginModelInVisible, sizeof(m_bMakeOriginModelInVisible), &dwWrite);
	}
	else
	{
		char szLine[AFILE_LINEMAXLEN];
		_snprintf(szLine, sizeof(szLine), _format_hideclientmodel, m_bMakeOriginModelInVisible);
		pFile->WriteLine(szLine);

		_snprintf(szLine, sizeof(szLine), _format_usestaticframe, m_bUseSkinModelTMs);
		pFile->WriteLine(szLine);

		_snprintf(szLine, sizeof(szLine), _format_includechild, m_bIncludeChildModels);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DGFXModelProxy::Init(A3DDevice* pDevice)
{
	base_class::Init(pDevice);
	CreateModelTickSpeedProperty();
	return true;
}

A3DGFXElement* A3DGFXModelProxy::Clone(A3DGFXEx* pGfx) const
{
	A3DGFXModelProxy* pNewEle = new A3DGFXModelProxy(pGfx);
	*pNewEle = *this;
	return pNewEle;
}

bool A3DGFXModelProxy::Play()
{
	if (!IsInit())
	{
		InitBaseData();

		A3DSHADER sh;
		sh.SrcBlend = A3DBLEND_SRCALPHA;
		sh.DestBlend = A3DBLEND_INVSRCALPHA;
		m_dwRenderSlot = AfxGetGFXExMan()->GetRenderMan().RegisterSlot(
			m_nRenderLayer, 0, 0, GFX_MODEL_PRIM_TYPE, AString(), sh, 
			false, false, m_pGfx->IsZTestEnable() && m_bZEnable, HasPixelShader(), m_bSoftEdge, m_bAbsTexPath);
		SetInit(true);
	}

	if (m_pGfx->IsResourceReady())
	{
		m_pCurState->OnPlay();
		
		m_Root.Clear();
		m_fLastFrameValue = -1.0f;
	}

	return true;
}

bool A3DGFXModelProxy::Stop()
{
	m_pCurState->OnStop();
	m_Root.Clear();
	base_class::Stop();
	return true;
}

void A3DGFXModelProxy::SetCurrentState(ModelState* pState)
{
	m_pCurState->OnLeaveState();
	delete m_pCurState;
	m_pCurState = pState;
	m_pCurState->OnEnterState();
}

void A3DGFXModelProxy::SetClientModelVisibleById(bool bVisible)
{
	AfxGetGFXExMan()->GetGfxInterface()->SetSkinModelVisibleById(GetGfx()->GetId(), bVisible);
}

bool A3DGFXModelProxy::ApplyClientModelPlaySpeed()
{
	if (A3DAnimatableBase* pAnimable = m_AnimatedProperty.GetAnimatable(PropertyName_ModelTickSpeed))
	{
		float fValue = pAnimable->GetInterpValue(m_dwEleTickTime).GetFloat();
		ASSERT( fValue >= 0.0f && fValue < 10.0f );
		if (m_fLastFrameValue != fValue)
		{
			AfxGetGFXExMan()->GetGfxInterface()->SetSkinModelTickSpeed(m_pGfx->GetId(), fValue);
			m_fLastFrameValue = fValue;
		}
		return true;
	}
	return false;
}

void A3DGFXModelProxy::RestoreClientModelPlaySpeed()
{
	AfxGetGFXExMan()->GetGfxInterface()->SetSkinModelTickSpeed(m_pGfx->GetId(), 1.0f);
}

bool A3DGFXModelProxy::Render(A3DViewport*)
{
	bool bCanRender = CanRender();

	if (IsOriginModelInVisible())		//对隐藏客户端，多渲染一帧
	{
		if (!m_bLastActive && !bCanRender)
		{
			return true;
		}

		m_bLastActive = bCanRender;
	}
	else
	{
		if (!bCanRender)
			return true;
	}
	
	if (IsEleShouldBeDelayRendered())
	{
		AfxGetGFXExMan()->AddDelayedEle(this);
		return false;
	}

	if (m_dwRenderSlot)
		AfxGetGFXExMan()->GetRenderMan().RegisterEleForRender(m_dwRenderSlot, this);

	
		
	return true;
}

void A3DGFXModelProxy::PrepareRenderSkinModel()
{
//	m_pDevice->SetZWriteEnable(true);
	m_pDevice->SetZFunc(A3DCMP_LESSEQUAL);
}

void A3DGFXModelProxy::SkinModelRenderAtOnce(A3DSkinModel* pSkinModel, A3DViewport* pView, DWORD dwFlags, A3DReplaceHLSL* pRepHLSL)
{
#ifdef _ANGELICA22
	pSkinModel->RenderAtOnce(pView, dwFlags | (IsUseSkinModelStaticFrame() ? A3DSkinModel::RAO_NOUPDATEBLENDMAT : 0), true, pRepHLSL, m_bIncludeChildModels);
#else
	pSkinModel->RenderAtOnce(pView, dwFlags | (IsUseSkinModelStaticFrame() ? A3DSkinModel::RAO_NOUPDATEBLENDMAT : 0), true, pRepHLSL);
#endif
}

void A3DGFXModelProxy::RenderSkinModel(A3DViewport* pView, A3DSkinModel* pSkinModel, A3DRenderTarget* pFrameBuffer)
{
	//	Only runtime temp property
	A3DSHADER OriginShader;

	OriginShader.SrcBlend = (A3DBLEND)m_pDevice->GetDeviceRenderState(D3DRS_SRCBLEND);
	OriginShader.DestBlend = (A3DBLEND)m_pDevice->GetDeviceRenderState(D3DRS_DESTBLEND);
	m_pDevice->SetSourceAlpha(m_Shader.SrcBlend);
	m_pDevice->SetDestAlpha(m_Shader.DestBlend);
	
	bool OriginZTest = m_pDevice->GetZTestEnable();
	bool OriginAlphaTest = m_pDevice->GetAlphaTestEnable();
	bool OriginZWrite = m_pDevice->GetZWriteEnable();
    DWORD dwOldAlphaRef = m_pDevice->GetAlphaRef();
	m_pDevice->SetZWriteEnable(IsOriginModelInVisible()/* && !IsUseSkinModelStaticFrame()*/);
	m_pDevice->SetZTestEnable(true);
	m_pDevice->SetAlphaTestEnable(true);
	m_pDevice->SetAlphaRef(84);

	BlendTMNode tmpRootNode;
	if (IsUseSkinModelStaticFrame())
	{
		tmpRootNode.GenerateBlendTMNodeFrom(pSkinModel, true);
		if (!m_Root.IsEmpty())
			m_Root.UpdateTMToSkinModel(pSkinModel, true);

		//GetOrSetupSkinModelStaticFrameTMs(pSkinModel, &tmpRootNode);
	}

	base_class::RenderSkinModel(pView, pSkinModel, pFrameBuffer);

	if (IsUseSkinModelStaticFrame())
	{
		// reset original mats back
		tmpRootNode.UpdateTMToSkinModel(pSkinModel, true);

		if (m_Root.IsEmpty())
			m_Root.swap(tmpRootNode);
	}
	
	m_pDevice->SetZWriteEnable(OriginZWrite);
	m_pDevice->SetZTestEnable(OriginZTest);
	m_pDevice->SetAlphaTestEnable(OriginAlphaTest);
	m_pDevice->SetSourceAlpha(OriginShader.SrcBlend);
	m_pDevice->SetDestAlpha(OriginShader.DestBlend);
    m_pDevice->SetAlphaRef(dwOldAlphaRef);

	
#ifdef _ANGELICA22
	//render child models
	if (!m_bIncludeChildModels)
	{
		PrepareRenderSkinModel();

		int nChildNum = pSkinModel->GetChildModelNum();
		for (int i=0; i<nChildNum; ++i)
		{
			A3DSkinModel* pChild = pSkinModel->GetChildModel(i);
			if (pChild)
				pChild->RenderAtOnce(pView, (IsUseSkinModelStaticFrame() ? A3DSkinModel::RAO_NOUPDATEBLENDMAT : 0), true);
		}

		RestoreRenderSkinModel();
	}
#endif
}

bool A3DGFXModelProxy::GetOrSetupSkinModelStaticFrameTMs(A3DSkinModel* pSkinModel, BlendTMNode* pOutCurTmp)
{
	//const A3DMATRIX4* pMats = pSkinModel->GetTVSBlendMatrices();
	//const int iBoneNum = pSkinModel->GetSkeleton()->GetBoneNum();
	//pOutCurTmp->insert(pOutCurTmp->begin(), pMats, pMats + iBoneNum);
	//if (m_aTMSkinModel.empty())
	//{
	//	// First time, get this frame's tms
	//	m_aTMSkinModel.insert(m_aTMSkinModel.begin(), pMats, pMats + iBoneNum);
	//}
	//else if (m_aTMSkinModel.size() == iBoneNum)
	//{
	//	A3DMATRIX4* pMats = const_cast<A3DMATRIX4*>(pSkinModel->GetTVSBlendMatrices());
	//	memcpy(pMats, &m_aTMSkinModel[0], sizeof(A3DMATRIX4) * iBoneNum);
	//}

	return true;
}

bool A3DGFXModelProxy::GetAnimatableProperty(const char* name, GFXEleAnimtableProperty* pProperty/*out*/)
{
	if (0 == strcmp(name, PropertyName_ModelTickSpeed))
	{
		pProperty->dispName = "模型动作速率";
		pProperty->desc = "改变模型动作速率，默认为 1.0";
		pProperty->uiWay = GFXEleAnimtableProperty::UIWAY_SLIDER;
		return true;
	}
	else
	{
		return base_class::GetAnimatableProperty(name, pProperty);
	}
}

bool A3DGFXModelProxy::TickAnimation(DWORD dwTickTime)
{
	m_pCurState->OnTick(dwTickTime);

	if (!base_class::TickAnimation(dwTickTime))
		return false;

	return true;
}

A3DSkinModel* A3DGFXModelProxy::GetSkinModel()
{
	if (m_pGfx->GetUseParentModelPtr())
	{
		extern CECModel* model_handle_to_ecm(modelh_t mh);
		CECModel* pECModel = model_handle_to_ecm(m_pGfx->GetHostModel());
		return pECModel ? pECModel->GetA3DSkinModel() : NULL;
	}
	else if (CECModel* pECModel = AfxGetGFXExMan()->GetGfxInterface()->GetECModelById(m_pGfx->GetId(), m_pGfx->GetHostModel()))
		return pECModel->GetA3DSkinModel();

	return NULL;
}

bool A3DGFXModelProxy::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch (nOp)
	{
	case ID_GFXOP_MODELPROXY_HIDECLIENTMODEL:
		m_bMakeOriginModelInVisible = prop;
		return true;
	case ID_GFXOP_MODELPROXY_USESTATICFRAME:
		m_bUseSkinModelTMs = prop;
		return true;
	case ID_GFXOP_MODELPROXY_INCLUDECHILDMODELS:
		m_bIncludeChildModels = prop;
		return true;
	default:
		return base_class::SetProperty(nOp, prop);
	}
}

GFX_PROPERTY A3DGFXModelProxy::GetProperty(int nOp) const
{
	switch (nOp)
	{
	case ID_GFXOP_MODELPROXY_HIDECLIENTMODEL:
		return GFX_PROPERTY(m_bMakeOriginModelInVisible);
	case ID_GFXOP_MODELPROXY_USESTATICFRAME:
		return GFX_PROPERTY(m_bUseSkinModelTMs);
	case ID_GFXOP_MODELPROXY_INCLUDECHILDMODELS:
		return GFX_PROPERTY(m_bIncludeChildModels);
	}

	return base_class::GetProperty(nOp);
}