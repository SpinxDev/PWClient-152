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
		int				nAttackOrientation;		//	0:Ĭ�� 1:�� 2:�� 3:�� 4:�� 5: ��
	};

public:		//	Constructors and Destructor

	A3DGFXInterface() {}
	virtual ~A3DGFXInterface() = 0 {}

public:		//	Attributes

public:		//	Operations

	//	���������������ǰ��AfxPlaySkillGfx�ص�����
	virtual bool PlaySkillGfx(const PARAM_PLAYSKILLGFX* param) = 0;

	//	���ڴ������ͷ�AudioEngine�е�EventInstance����
	//	���º�������angelica.cfg�ļ��е�[a3d]���е�faudio_enable = 1�����û���fmod��AudioEngine��ʱ�ᱻ����
	//	���ڲ���AMedia������Ƶ���ŵ�����Ŀ�����Բ���ʵ��
	virtual AudioEngine::EventInstance* CreateAudioEventInstance(const char* szEventFullPath, bool bAutoDestroy) const { return NULL; }
	virtual void ReleaseAudioEventInstance(AudioEngine::EventInstance*& pInstance) const {}
	virtual void SetEventSystem(AudioEngine::EventSystem* pEventSystem) {}
	//	����ͳ��GFXϵͳ�е�ǰ���Event Instance����
	virtual LONG GetCurActEventInstanceNum() const { return 0; }

	//  GFX ���ڲ����õ��Ƿ���汾���ͻ�����Ҫʵ���麯���汾��GetECModelById
	//	��gfx�ĸ�ecmodel��ָ�봫�ݸ��ͻ��ˣ����pModel��ͨ��nIdȡ����ecmodel��һ�£���˵����������ģ�ͣ���Ҫ�ͻ��˷�����Ӧ����ģ��
	CECModel* GetECModelById(clientid_t nId, modelh_t hostModel);
	//	�ͻ���Ӧʵ�ָ���id����SkinModel�Ƿ�����������Ⱦ�Ľӿڣ����������Ϊinvisible����ͻ��˵�ECModel�е�SkinModel����������Ⱦ
	//	���� bool CECModel::Render(A3DViewport* pViewport, bool bRenderAtOnce/* false */, bool bRenderSkinModel/*=true*/, DWORD dwFlags/*=0*/, A3DSkinModelRenderModifier* pRenderModifier)
	//	��ʱ��Ӧ��ʹ��bRenderSkinModelΪfalse
	virtual bool SetSkinModelVisibleById(clientid_t nId, bool bVisible) { return false; }
	//	�ͻ���ͨ��id�ҵ���Ӧ��CECModel��������Ӧ�ӿڣ�����TickSpeed
	virtual bool SetSkinModelTickSpeed(clientid_t nId, float fTickSpeed) { return false; }

protected:	//	Attributes

protected:	//	Operations

	//	�ͻ���Ӧʵ��һ��ͨ��id��ȡ��ǰ��Ⱦģ�͵Ľӿ�
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
	//	ֱ�ӵ�����ǰ��AfxPlaySkillGfx
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
	//	�ͻ���ͨ��id�ҵ���Ӧ��CECModel��������Ӧ�ӿڣ�����TickSpeed
	virtual bool SetSkinModelTickSpeed(clientid_t nId, float fTickSpeed);
};

#endif

///////////////////////////////////////////////////////////////////////////
//
//	inline functions
//
///////////////////////////////////////////////////////////////////////////



#endif	//	_A3DGFXInterface_H_

