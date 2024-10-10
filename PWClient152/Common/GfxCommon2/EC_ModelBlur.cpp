/*
* FILE: EC_ModelBlur.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/5/10
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "EC_ModelBlur.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECMMotionBlurPSTable
//	
///////////////////////////////////////////////////////////////////////////

const char* ECMMotionBlurPSTable::MBPS_STANDARD		= "Standard";

ECMMotionBlurPSTable::ECMMotionBlurPSTable()
: m_pA3DDevice(NULL)
{

}

ECMMotionBlurPSTable::~ECMMotionBlurPSTable()
{
	Release();
}

bool ECMMotionBlurPSTable::Init( A3DDevice* pA3DDevice )
{
	m_pA3DDevice = pA3DDevice;
	Release();

	struct Name2PSFile {
		const char* szName;
		const char* szFile;
	};
	static const Name2PSFile name2psfiletbl [] = 
	{
#ifdef _ANGELICA21
		{ MBPS_STANDARD, "Shaders\\HLSL\\skinmodel_motionblur.hlslp" },
#elif defined _ANGELICA22
		{ MBPS_STANDARD, "Shaders\\2.2\\HLSL\\SkinModel\\skinmodel_motionblur.hlsl" },
#endif
	};
	
	for (int i = 0; i < _countof(name2psfiletbl); ++i)
	{
		A3DHLPixelShader* pHLPS = a3d_CreateHLPixelShader();
		ASSERT( pHLPS );
		pHLPS->SetShaderFile(name2psfiletbl[i].szFile);
		pHLPS->LoadShader(pA3DDevice);
		m_MotionBlurPSTbl[name2psfiletbl[i].szName] = pHLPS;
	}

	return true;
}

void ECMMotionBlurPSTable::Release()
{
	for (Name2PSTable::iterator itr = m_MotionBlurPSTbl.begin()
		; itr != m_MotionBlurPSTbl.end()
		; ++itr)
	{
		delete itr->second;
	}

	m_MotionBlurPSTbl.clear();
}

A3DHLPixelShader* ECMMotionBlurPSTable::GetMotionBlurPSByName(const char* szName)
{
	if (!szName || !szName[0])
		return NULL;

	A3DHLPixelShader* pPS = m_MotionBlurPSTbl[szName];
	if (!pPS)
		return NULL;

#ifdef GFX_EDITOR
	//编辑器的话，每次都Reload一遍
	pPS->LoadShader(m_pA3DDevice);
#endif
	return pPS;
}


//////////////////////////////////////////////////////////////////////////
//
//	Implement BlendTMNode
//
//////////////////////////////////////////////////////////////////////////

BlendTMNode::~BlendTMNode()
{
	for (size_t i = 0; i < m_aChildren.size(); ++i)
	{
		delete m_aChildren[i];
	}
	m_aChildren.clear();
}

BlendTMNode::BlendTMNode(const BlendTMNode& rhs)
: m_aTMSkinModel(rhs.m_aTMSkinModel)
{
	for (size_t i = 0; i < rhs.m_aChildren.size(); ++i)
	{
		m_aChildren.push_back(new BlendTMNode(*rhs.m_aChildren[i]));
	}
}

BlendTMNode& BlendTMNode::operator = (const BlendTMNode& rhs)
{
	if (&rhs == this)
		return *this;
	
	BlendTMNode(rhs).swap(*this);
	return *this;
}

void BlendTMNode::GenerateBlendTMNodeFrom(A3DSkinModel* pSkinModel, bool bIncludeChildren)
{
	const A3DMATRIX4* pMats = pSkinModel->GetTVSBlendMatrices();
	const int iBoneNum = pSkinModel->GetSkeleton()->GetBoneNum();
	m_aTMSkinModel.resize(iBoneNum);
	std::copy(pMats, pMats + iBoneNum, m_aTMSkinModel.begin());

	if (!bIncludeChildren)
		return;

	for (int i = 0; i < pSkinModel->GetChildModelNum(); ++i)
	{
		if (static_cast<int>(m_aChildren.size()) <= i)
		{
			m_aChildren.push_back(new BlendTMNode());
		}

		m_aChildren[i]->GenerateBlendTMNodeFrom(pSkinModel->GetChildModel(i), bIncludeChildren);
	}
}

void BlendTMNode::UpdateTMToSkinModel(A3DSkinModel* pSkinModel, bool bIncludeChildren)
{
	if (pSkinModel->GetSkeleton()->GetBoneNum() == m_aTMSkinModel.size())
	{
		// reset original mats back
		A3DMATRIX4* pMats = const_cast<A3DMATRIX4*>(pSkinModel->GetTVSBlendMatrices());
		std::copy(m_aTMSkinModel.begin(), m_aTMSkinModel.end(), pMats);
	}

	if (!bIncludeChildren)
		return;

	if (pSkinModel->GetChildModelNum() == m_aChildren.size())
	{
		for (int i = 0; i < pSkinModel->GetChildModelNum(); ++i)
		{
			m_aChildren[i]->UpdateTMToSkinModel(pSkinModel->GetChildModel(i), bIncludeChildren);
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECMBlendMatricesLoopContainer
//	
///////////////////////////////////////////////////////////////////////////

class ECMBlendMatricesLoopContainer
{
	typedef APtrList<ECMBlendMatricesSet*> ECMBlendMatricesSetCollector;
public:
	ECMBlendMatricesLoopContainer(int nCapacity, int nBoneNum)
		: m_nCapacity(nCapacity)
		, m_nBoneNum(nBoneNum)
		, m_nHead(0)
	{
	}

	~ECMBlendMatricesLoopContainer()
	{
		ALISTPOSITION pos = m_aContainer.GetHeadPosition();

		while (pos)
			delete m_aContainer.GetNext(pos);
	}

	ECMBlendMatricesSet* GetNewAvaibale()
	{
		if (GetCount() < m_nCapacity)
		{
			ECMBlendMatricesSet* pSet = new ECMBlendMatricesSet(m_nBoneNum, A3DMATRIX4());
			m_aContainer.AddTail(pSet);
			return pSet;
		}
		else
		{
			ECMBlendMatricesSet* pSet = m_aContainer.RemoveHead();
			m_aContainer.AddTail(pSet);
			return pSet;
		}
	}

	bool RemoveHead()
	{
		if (m_nHead < GetCount())
		{
			m_nHead ++;
			return true;
		}

		return false;
	}

	int GetBeginIdx() const { return m_nHead; }
	int GetEndIdx() const { return GetCount(); }
	int GetColorIdx(int nIdx) const { return GetCount() - nIdx + m_nHead - 1; }
	bool IsEnd() const { return !IsEmpty() && m_nHead >= GetCount(); }

	const ECMBlendMatricesSet* GetByIndex(int nIdx) const { return m_aContainer.GetByIndex(nIdx); }

	bool IsEmpty() const { return GetCount() == 0; }
	int GetCount() const { return m_aContainer.GetCount(); }

private:
	ECMBlendMatricesSetCollector m_aContainer;
	int m_nHead;
	const int m_nCapacity;
	const int m_nBoneNum;
	ECMBlendMatricesLoopContainer(const ECMBlendMatricesLoopContainer&);
	ECMBlendMatricesLoopContainer& operator = (const ECMBlendMatricesLoopContainer&);
};


///////////////////////////////////////////////////////////////////////////
//	
//	Implement ECMActionBlurInfo
//	
///////////////////////////////////////////////////////////////////////////

ECMActionBlurInfo::ECMActionBlurInfo()
{
	m_bFirstTimeStarted = true;
	m_bCanAddFrame = false;
	m_uCurMatricesCount = 0;
	m_uStartIndex = 0;
	m_dwCurTime = 0;
	m_dwCurTick = 0;
	m_bStopWhenActChange = true;
	m_pMatricesCollector = NULL;
	// 默认情况下, m_strMotionBlurPS 为Empty String	
	// 并且m_pHLPixelShader为NULL，这样可以使用以往的渲染方案
	m_pHLPixelShader = NULL;
	m_iActChannel = 0;
	m_dwTotalTime = 0;
}

ECMActionBlurInfo::~ECMActionBlurInfo()
{
	delete m_pMatricesCollector;
	m_pMatricesCollector = NULL;
}

int ECMActionBlurInfo::GetCurrentBlurCount() const
{
	return m_pMatricesCollector ? m_pMatricesCollector->GetCount() : 0;
}

void ECMActionBlurInfo::CreateMatricesCollector(int nBoneNum)
{
	if (m_pMatricesCollector)
		delete m_pMatricesCollector;

	m_pMatricesCollector = new ECMBlendMatricesLoopContainer(m_Colors.size()/* + 1*/, nBoneNum);
}

void ECMActionBlurInfo::UpdateOriginalMatrices(A3DSkinModel* pSkinModel)
{
	m_RootOriginalTMs.GenerateBlendTMNodeFrom(pSkinModel, m_bApplyToChildren);
}

void ECMActionBlurInfo::RestoreOriginalMatrices(A3DSkinModel* pModel)
{
	m_RootOriginalTMs.UpdateTMToSkinModel(pModel, m_bApplyToChildren);
}

float ECMActionBlurInfo::GetCurrentLifePercent() const
{
	return (float)m_dwCurTime / m_dwTotalTime;
}

ECMBlendMatricesSet* ECMActionBlurInfo::GetNewAvaiableSlot() const
{
	if (m_pMatricesCollector)
		return m_pMatricesCollector->GetNewAvaibale();

	return NULL;
}

bool ECMActionBlurInfo::ApplyBlurMatrices(int nIndex, A3DSkinModel* pSkinModel)
{
	if (!m_pMatricesCollector || nIndex >= m_pMatricesCollector->GetCount())
		return false;

	const ECMBlendMatricesSet* pSet = m_pMatricesCollector->GetByIndex(nIndex);
	int nBoneNum = pSkinModel->GetSkeleton()->GetBoneNum();
	A3DMATRIX4* aTBlendMats = const_cast<A3DMATRIX4*>(pSkinModel->GetTVSBlendMatrices());
	size_t uBonesSelected = m_BoneIndices.size();

	if (uBonesSelected)
	{
		for (size_t i = 0; i < uBonesSelected; i++)
		{
			int nIndex = m_BoneIndices[i];

			if (nIndex >= 0 && nIndex < nBoneNum)
				memcpy(aTBlendMats + nIndex, pSet->begin() + nIndex, sizeof(A3DMATRIX4));
		}
	}
	else
		memcpy(aTBlendMats, pSet->begin(), sizeof(A3DMATRIX4) * nBoneNum);

	return true;
}

bool ECMActionBlurInfo::PushBlendMatrices(const A3DMATRIX4* aTBlendMats, int nBoneNum)
{
	if (!m_pMatricesCollector)
	{
		ASSERT( m_pMatricesCollector );
		return false;
	}

	ECMBlendMatricesSet* pSet = GetNewAvaiableSlot();
	memcpy(pSet->begin(), aTBlendMats, sizeof(A3DMATRIX4) * nBoneNum);
	return true;
}

bool ECMActionBlurInfo::Tick(DWORD dwDeltaTime)
{
	m_dwCurTime += dwDeltaTime;
	m_dwCurTick += dwDeltaTime;

	if (m_dwCurTick >= m_dwInterval)
	{
		m_dwCurTick -= m_dwInterval;

		if (m_dwCurTime > m_dwTotalTime)
		{
			if (!m_pMatricesCollector->RemoveHead())
				return false;
			else
				m_bCanAddFrame = false;
		}
		else
			m_bCanAddFrame = true;
	}
	else
	{
		m_bCanAddFrame = FirstTouch();
	}

	return true;
}

bool ECMActionBlurInfo::NeedRender()
{
	return m_pMatricesCollector && !m_pMatricesCollector->IsEmpty();
}

int ECMActionBlurInfo::GetBeginIdx() const
{
	return m_pMatricesCollector ? m_pMatricesCollector->GetBeginIdx() : 0;
}

int ECMActionBlurInfo::GetEndIdx() const
{
	return m_pMatricesCollector ? m_pMatricesCollector->GetEndIdx() : 0;
}

int ECMActionBlurInfo::GetColorIdx(int nIdx) const
{
	return m_pMatricesCollector ? m_pMatricesCollector->GetColorIdx(nIdx) : 0;
}

void ECMActionBlurInfo::SetUsePS(const char* szPS)
{
	// SetUsePS的地方，必须是已经存在引擎的地方么？ 这样子的全局依赖比较丑陋，如果能让ECMActionBlurInfo提供一个初始化函数接受A3DGFXExMan类的指针就更好了
	// 为什么不把GetMotionBlurPSByName放到Render里去，改成每帧获取的话，就无需考虑初始化顺序的问题了
	// 因为编辑器希望可以在每次重播的时候都重新从文件加载HLSL，使得HLSL的开发可以比较方便的看到效果，有更好的替代办法吗？
	ASSERT( AfxGetGFXExMan() && AfxGetGFXExMan()->GetMotionBlurPSTbl() );
	m_strMotionBlurPS = szPS;
	m_pHLPixelShader = AfxGetGFXExMan()->GetMotionBlurPSTbl()->GetMotionBlurPSByName(m_strMotionBlurPS);
}

void ECMActionBlurInfo::SetInChannel(int iChannel)
{
	m_iActChannel = iChannel;
}

A3DHLPixelShader* ECMActionBlurInfo::GetMotionBlurPS() const
{
	return m_pHLPixelShader;
}

class ECMActionBlurReplaceHLSL : public A3DReplaceHLSL
{
public:
	ECMActionBlurReplaceHLSL(A3DHLPixelShader* pPS)
		: m_pPS(pPS)
	{
	}
	virtual A3DHLSL* GetReplaceHLSL() const
	{
		if (m_pPS)
			return m_pPS->GetHLSL();
		return NULL;
	}
	virtual bool SetupAlphaMapMask(float masks[4]) const
	{
		if (m_pPS)
			return m_pPS->SetConstValueFloat4(HLSL_CONST_ALPHAMASK, masks[0], masks[1], masks[2], masks[3]);
		return false;
	}

private:
	virtual bool IsNeedReplaceTexture(const AString& strSrcParam, const AString& strSrcSemantic, AString* pReqParamName) const
	{
		return false;
	}

	A3DHLPixelShader* m_pPS;
};


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECModel Motion Blur Related
//	
///////////////////////////////////////////////////////////////////////////

void CECModel::ClearMotionBlur()
{
	if (!m_pBlurInfo)
		return;

	m_pBlurInfo->m_uCurMatricesCount = 0;

	if (!m_pBlurInfo->m_bApplyToChildren)
		return;

	for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
	{
		CECModel* pChild = it->second;
		pChild->ClearMotionBlur();
	}
}

void CECModel::RemoveMotionBlurInfo()
{
	if (m_pBlurInfo)
	{
		if (m_pBlurInfo->m_bApplyToChildren)
		{
			for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
			{
				CECModel* pChild = it->second;
				pChild->RemoveMotionBlurInfo();
			}
		}

		delete m_pBlurInfo;
		m_pBlurInfo = NULL;
	}
}

bool CECModel::CalcMotionBlurParams()
{
	assert(m_pBlurInfo && m_pA3DSkinModel);

	if (!m_pA3DSkinModel->GetBlendMatUpdated())
	{
		ClearMotionBlur();
		return false;
	}

	if (!m_pBlurInfo->m_bCanAddFrame)
		return true;

	int nBoneNum = m_pA3DSkinModel->GetSkeleton()->GetBoneNum();
	const A3DMATRIX4* aTBlendMats = m_pA3DSkinModel->GetTVSBlendMatrices();
	m_pBlurInfo->PushBlendMatrices(aTBlendMats, nBoneNum);

	if (m_pBlurInfo->m_bApplyToChildren)
	{
		for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		{
			CECModel* pChild = it->second;

			if (pChild->IsShown() && pChild->m_pBlurInfo && pChild->m_pA3DSkinModel)
				pChild->CalcMotionBlurParams();
		}
	}

	return true;
}

void CECModel::TickMotionBlur(DWORD dwDeltaTime)
{
	if (!m_pBlurInfo->Tick(dwDeltaTime))
		RemoveMotionBlurInfo();
}

void CECModel::RenderMotionBlur(A3DViewport* pViewport)
{
	assert(m_pA3DSkinModel && m_pBlurInfo && m_pBlurInfo->m_bRoot);

	if (!m_pA3DSkinModel)
		return;

	if (!CalcMotionBlurParams())
		return;

	if (m_pBlurInfo->NeedRender())
	{
		if (m_pBlurInfo->m_bHighlight)
		{
			m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
			m_pA3DDevice->SetDestAlpha(A3DBLEND_ONE);
		}
		else
		{
			m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
			m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
		}

		float fOldAlpha = m_fInnerAlpha;
		A3DCOLORVALUE OldColor = m_InnerColor;

		m_pBlurInfo->UpdateOriginalMatrices(m_pA3DSkinModel);

		for (int i = m_pBlurInfo->GetBeginIdx(); i < m_pBlurInfo->GetEndIdx(); ++i)
		{
			int iColorIdx = m_pBlurInfo->GetColorIdx(i);
			A3DCOLORVALUE c = m_pBlurInfo->m_Colors[iColorIdx];
			SetInnerAlpha(c.a);
			c.a = 1.0f;
			SetInnerColor(c);
			ApplyBlurMatrices(i);
			A3DCOLORVALUE curColor = GetColor();

			A3DHLPixelShader* pBlurRenderPS = m_pBlurInfo->GetMotionBlurPS();
			ECMActionBlurReplaceHLSL RepHLSL(pBlurRenderPS);
			if (pBlurRenderPS && pBlurRenderPS->GetHLSL())
			{
				pBlurRenderPS->SetConstValueFloat4("g_Color", curColor.r, curColor.g, curColor.b, m_fAlpha);
				pBlurRenderPS->SetConstValueFloat("g_Time", m_pBlurInfo->GetCurrentLifePercent());
				m_pA3DSkinModel->RenderAtOnce(pViewport, A3DGFXPIFuncs::GetSkinModelNoPixelShaderFlag(), false, &RepHLSL);
			}
			else
			{
				m_pA3DSkinModel->RenderAtOnce(pViewport, 0, false);
			}
		}

		m_pBlurInfo->RestoreOriginalMatrices(m_pA3DSkinModel);

		SetInnerColor(OldColor);
		SetInnerAlpha(fOldAlpha);

		if (m_pBlurInfo->m_bHighlight)
			m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	}
}

void CECModel::ApplyBlurMatrices(int nIndex)
{
	if (!m_pA3DSkinModel)
		return;

	m_pBlurInfo->ApplyBlurMatrices(nIndex, m_pA3DSkinModel);

	if (m_pBlurInfo->m_bApplyToChildren)
	{
		for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		{
			CECModel* pChild = it->second;

			if (pChild->IsShown() && pChild->m_pBlurInfo)
				pChild->ApplyBlurMatrices(nIndex);
		}
	}
}

void CECModel::SetMotionBlurInfo(ECMActionBlurInfo* pInfo)
{
	delete m_pBlurInfo;
	m_pBlurInfo = pInfo;


	if (m_pA3DSkinModel)
	{
		A3DSkeleton* pSke = m_pA3DSkinModel->GetSkeleton();

		if (pSke)
		{
			pInfo->CreateMatricesCollector(pSke->GetBoneNum());
		}
	}

	if (pInfo->m_bApplyToChildren)
	{
		for (ECModelMap::iterator it = m_ChildModels.begin(); it != m_ChildModels.end(); ++it)
		{
			CECModel* pChild = it->second;
			ECMActionBlurInfo* pChildInfo = new ECMActionBlurInfo;

			for (size_t i = 0; i < pInfo->m_Colors.size(); i++)
				pChildInfo->m_Colors.push_back(pInfo->m_Colors[i]);

			pChildInfo->m_dwTotalTime = pInfo->m_dwTotalTime;
			pChildInfo->m_dwInterval = pInfo->m_dwInterval;
			pChildInfo->m_bApplyToChildren = true;
			pChildInfo->m_bHighlight = pInfo->m_bHighlight;
			pChildInfo->m_bRoot = false;
			pChild->SetMotionBlurInfo(pChildInfo);
		}
	}
}

