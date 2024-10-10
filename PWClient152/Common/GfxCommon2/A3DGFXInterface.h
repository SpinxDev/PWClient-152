/*
 * FILE: A3DGFXInterface.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: zhangyachuan, 2011/2/24
 *
 * HISTORY:
 *
 * Copyright (c) 2011 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DGFXInterface_H_
#define _A3DGFXInterface_H_

#include <AString.h>
#include <ABaseDef.h>
#include "GfxCommonTypes.h"
#include "A3DGFXExMan.h"

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

namespace AudioEngine
{
	class EventInstance;
	class EventSystem;
}

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DGFXInterface
//
///////////////////////////////////////////////////////////////////////////

class A3DGFXInterface
{
public:		//	Types

	enum AttackOrientation
	{
		ATKORIENTATION_DEFAULT = 0, 
		ATKORIENTATION_UP,
		ATKORIENTATION_DOWN,
		ATKORIENTATION_LEFT,
		ATKORIENTATION_RIGHT,
		ATKORIENTATION_NONE,
		ATKORIENTATION_NUM,
	};

	struct PARAM_PLAYSKILLGFX
	{
		AString			strAtkFile;
		unsigned char	SerialID;
		clientid_t		nCasterID;
		clientid_t		nCastTargetID;
		A3DVECTOR3		vFixedPoint;
		int				nDivisions;
		TargetDataVec*	pTargets;
		DWORD			dwUserData;
		int				nAttIndex;
		int				nAttackOrientation;		//	0:默认 1:上 2:下 3:左 4:右 5: 无
	};

public:		//	Constructors and Destructor

	A3DGFXInterface() {}
	virtual ~A3DGFXInterface() = 0 {}

public:		//	Attributes

public:		//	Operations

	//	本函数用于替代以前的AfxPlaySkillGfx回调函数
	virtual bool PlaySkillGfx(const PARAM_PLAYSKILLGFX* param) = 0;

	//	用于创建和释放AudioEngine中的EventInstance对象
	//	以下函数仅在angelica.cfg文件中的[a3d]块中的faudio_enable = 1（采用基于fmod的AudioEngine）时会被调用
	//	对于采用AMedia进行音频播放的老项目，可以不予实现
	virtual AudioEngine::EventInstance* CreateAudioEventInstance(const char* szEventFullPath, bool bAutoDestroy) const { return NULL; }
	virtual void ReleaseAudioEventInstance(AudioEngine::EventInstance*& pInstance) const {}
	virtual void SetEventSystem(AudioEngine::EventSystem* pEventSystem) {}
	//	用于统计GFX系统中当前活动的Event Instance数量
	virtual LONG GetCurActEventInstanceNum() const { return 0; }

	//  GFX 库内部调用的是非虚版本，客户端需要实现虚函数版本的GetECModelById
	//	把gfx的父ecmodel的指针传递给客户端，如果pModel与通过nId取到的ecmodel不一致，则说明可能是子模型，需要客户端返回相应的子模型
	CECModel* GetECModelById(clientid_t nId, modelh_t hostModel);
	//	客户端应实现根据id设置SkinModel是否被正常流程渲染的接口，如果被设置为invisible，则客户端的ECModel中的SkinModel则无需再渲染
	//	调用 bool CECModel::Render(A3DViewport* pViewport, bool bRenderAtOnce/* false */, bool bRenderSkinModel/*=true*/, DWORD dwFlags/*=0*/, A3DSkinModelRenderModifier* pRenderModifier)
	//	的时候，应当使得bRenderSkinModel为false
	virtual bool SetSkinModelVisibleById(clientid_t nId, bool bVisible) { return false; }
	//	客户端通过id找到对应的CECModel，调用相应接口，设置TickSpeed
	virtual bool SetSkinModelTickSpeed(clientid_t nId, float fTickSpeed) { return false; }

protected:	//	Attributes

protected:	//	Operations

	//	客户端应实现一个通过id获取当前渲染模型的接口
	//	Change GetSkinModelById to ChangeECModelById
	virtual CECModel* GetECModelById(clientid_t nId) { return NULL; }

private:

	A3DGFXInterface(const A3DGFXInterface&);
	A3DGFXInterface& operator = (const A3DGFXInterface&);

};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DGFXInterfaceDefImpl
//
///////////////////////////////////////////////////////////////////////////

class A3DGFXInterfaceDefImpl : public A3DGFXInterface
{
public:
	A3DGFXInterfaceDefImpl() {}
	virtual ~A3DGFXInterfaceDefImpl() {}
protected:
	//	直接调用以前的AfxPlaySkillGfx
	virtual bool PlaySkillGfx(const PARAM_PLAYSKILLGFX* param);
};


//	This is used in Angelica 2.2
class A3DGFXInterfaceDefImpl2 : public A3DGFXInterfaceDefImpl
{
public:
	A3DGFXInterfaceDefImpl2(AudioEngine::EventSystem* pEventSystem);
	virtual ~A3DGFXInterfaceDefImpl2() {}

protected:
	virtual AudioEngine::EventInstance* CreateAudioEventInstance(const char* szEventFullPath, bool bAutoDestroy) const;
	virtual void ReleaseAudioEventInstance(AudioEngine::EventInstance*& pInstance) const;
	virtual LONG GetCurActEventInstanceNum() const { return m_iCurActEventInstanceNum; }
	virtual void SetEventSystem(AudioEngine::EventSystem* pEventSystem) { m_pEventSystem = pEventSystem; }
private:
	AudioEngine::EventSystem* m_pEventSystem;
	mutable LONG m_iCurActEventInstanceNum;
};

#ifdef GFX_EDITOR

///////////////////////////////////////////////////////////////////////////
//
//	Class EditorGFXInterfaceDefImpl
//
///////////////////////////////////////////////////////////////////////////

class EditorGFXInterfaceDefImpl : public A3DGFXInterfaceDefImpl2
{
public:
	EditorGFXInterfaceDefImpl(AudioEngine::EventSystem* pEventSystem)
		: A3DGFXInterfaceDefImpl2(pEventSystem)
	{

	}
	virtual ~EditorGFXInterfaceDefImpl() {}
protected:
	
	virtual bool PlaySkillGfx(const PARAM_PLAYSKILLGFX* param);
	//	客户端通过id找到对应的CECModel，调用相应接口，设置TickSpeed
	virtual bool SetSkinModelTickSpeed(clientid_t nId, float fTickSpeed);
};

#endif

///////////////////////////////////////////////////////////////////////////
//
//	inline functions
//
///////////////////////////////////////////////////////////////////////////



#endif	//	_A3DGFXInterface_H_

