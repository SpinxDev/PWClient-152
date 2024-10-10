/*
* FILE: EC_ModelBlur.h
*
* DESCRIPTION: ecm模型的运动模糊功能，保存多帧动画的骨骼矩阵，在渲染的时候依次Apply到A3DSkinModel上再RenderAtOnce
*			可以使用自定义的PixelShader替换模型原有的ps
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/5/10
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#ifndef _EC_ModelBlur_H_
#define _EC_ModelBlur_H_

#include "A3DMatrix.h"
#include "vector.h"
#include "AList2.h"
#include "ABaseDef.h"
#include <A3DTypes.h>
#include <AString.h>
#include <vector>

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

class A3DDevice;
class A3DHLPixelShader;
class A3DSkinModel;

typedef abase::vector<A3DMATRIX4> ECMBlendMatricesSet;
typedef abase::vector<A3DCOLOR> ECMBlurColorArray;

class ECMBlendMatricesLoopContainer;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECMMotionBlurPSTable
//	
///////////////////////////////////////////////////////////////////////////

class ECMMotionBlurPSTable
{
public:
	static const char* MBPS_STANDARD;

public:
	ECMMotionBlurPSTable();
	~ECMMotionBlurPSTable();

	bool Init(A3DDevice* pA3DDevice);
	void Release();
	A3DHLPixelShader* GetMotionBlurPSByName(const char* szName);

private:
	A3DDevice* m_pA3DDevice;
	typedef abase::hash_map<AString, A3DHLPixelShader*> Name2PSTable;
	Name2PSTable m_MotionBlurPSTbl;

	ECMMotionBlurPSTable(const ECMMotionBlurPSTable&);
	ECMMotionBlurPSTable& operator = (const ECMMotionBlurPSTable&);
};

struct BlendTMNode
{
	BlendTMNode() {}
	std::vector<A3DMATRIX4> m_aTMSkinModel;
	std::vector<BlendTMNode*> m_aChildren;
	~BlendTMNode();
	BlendTMNode(const BlendTMNode&);
	BlendTMNode& operator = (const BlendTMNode&);

	void swap(BlendTMNode& rhs)
	{
		m_aTMSkinModel.swap(rhs.m_aTMSkinModel);
		m_aChildren.swap(rhs.m_aChildren);
	}

	void Clear() { BlendTMNode().swap(*this); }
	void GenerateBlendTMNodeFrom(A3DSkinModel* pSkinModel, bool bIncludeChildren);
	void UpdateTMToSkinModel(A3DSkinModel* pSkinModel, bool bIncludeChildren);
	bool IsEmpty() const { return m_aTMSkinModel.empty(); }

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECMActionBlurInfo
//	
///////////////////////////////////////////////////////////////////////////

class ECMActionBlurInfo
{
public:		//	Types
	
	class CECModel;

public:		//	Constructor and Destructor

	ECMActionBlurInfo(void);
	virtual ~ECMActionBlurInfo(void);

public:		//	Attributes

	ECMBlurColorArray				m_Colors;
	abase::vector<int>				m_BoneIndices;
	DWORD							m_dwTotalTime;
	DWORD							m_dwInterval;
	bool							m_bApplyToChildren;
	bool							m_bHighlight;

	bool							m_bRoot;
	bool							m_bCanAddFrame;
	size_t							m_uCurMatricesCount;
	size_t							m_uStartIndex;
	DWORD							m_dwCurTime;
	DWORD							m_dwCurTick;
	bool							m_bStopWhenActChange;

public:		//	Operations

	bool CanRenderCurFrame() const { return m_uCurMatricesCount > 1; }
	void UpdateOriginalMatrices(A3DSkinModel* pModel);
	void RestoreOriginalMatrices(A3DSkinModel* pModel);

	bool IsFirstTimeStarted() const { return m_bFirstTimeStarted; }
	void SetFirstTimeStarted(bool b) { m_bFirstTimeStarted = b; }
	bool FirstTouch() 
	{
		if (!m_bFirstTimeStarted)
			return false;
		else
		{
			m_bFirstTimeStarted = false;
			return true;
		}
	}
	float GetCurrentLifePercent() const; // [0, 1]

	int GetCurrentBlurCount() const;
	void CreateMatricesCollector(int nBoneNum);
	bool ApplyBlurMatrices(int nIndex, A3DSkinModel* pSkinModel);
	bool PushBlendMatrices(const A3DMATRIX4* aTBlendMats, int nBoneNum);
	bool Tick(DWORD dwDeltaTime);
	bool NeedRender();
	int GetBeginIdx() const;
	int GetEndIdx() const;
	int GetColorIdx(int nIdx) const;
	A3DHLPixelShader* GetMotionBlurPS() const;
	void SetUsePS(const char* szPS);
	void SetInChannel(int iChannel);
	int GetInChannel() const { return m_iActChannel; }

private:	//	Attributes

	ECMBlendMatricesLoopContainer*	m_pMatricesCollector;
	//ECMBlendMatricesSet				m_OriginalMatrices;			//	when apply matrices, we store the original matrice here
	BlendTMNode						m_RootOriginalTMs;			//	original tm root node
	bool							m_bFirstTimeStarted;
	AString							m_strMotionBlurPS;			//	selected motion blur ps
	A3DHLPixelShader*				m_pHLPixelShader;
	int								m_iActChannel;

private:	//	Operations

	ECMBlendMatricesSet* GetNewAvaiableSlot() const;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_EC_ModelBlur_H_

