/*
 * FILE: EC_AnimNodeMan.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: zhangyachuan, 2010/10/25
 *
 * HISTORY:
 *
 * Copyright (c) 2010 Archosaur Studio, All Rights Reserved.	
 */

#pragma once

#include <hashmap.h>
#include <A3DSkinModel.h>

#ifdef _ANGELICA21 
#include <A3DAnimNode.h>
#endif

class CECModel;
class CECAnimNodeMan;
class A3DAnimNodeBlendWalkRun;
class AnimBlendTree;
class A3DAnimNodeBlendBase;
class A3DAnimNodeBlend3Children;
class A3DAnimNode;

#ifdef _ANGELICA21
class A3DAnimNodeBlend3Children: public A3DAnimNodeBlendBase 
{
public:

	explicit A3DAnimNodeBlend3Children();
	virtual ~A3DAnimNodeBlend3Children();

	bool Init( A3DSkinModel* pSkinModel, const char* szNode, const char* szNode1, const char* szNode2 );
	virtual void Release();

	void SetBlendWeight( float fBlendWeight1, float fBlendWeight2, int iBlendInTime );
	void SetPlaySpeed( float fPlaySpeed );

	virtual void Update( int iDeltaTime );

protected:

	float	m_fBlendWeight1;
	float	m_fBlendWeight2;
	float	m_fTargetBlendWeight1;
	float	m_fTargetBlendWeight2;
	float	m_fBlendWeightDelta1;
	float	m_fBlendWeightDelta2;
	float	m_fBlendT;
	int		m_iBlendInTime;

	A3DAnimNodeAction*	m_pNode;
	A3DAnimNodeAction*	m_pNode1;
	A3DAnimNodeAction*	m_pNode2;
};
#endif

class CECAnimNodeMan
{
public:

	friend class CECModel;	

public:

	CECAnimNodeMan();
	virtual ~CECAnimNodeMan();

public:
	
	bool Init( CECModel* pECModel );
	void Release();	// should be released after the binding skin model released

	A3DAnimNode* GetAnimNode( int iSpecType, int iChannel ) const;

	void SetModelScale( float fModelScale );

	bool SetWalkRunVelocity( int iChannel, float fVelocity, int iBlendInTime );
	bool SetWalkRunVelocity( float fVelocity, int iBlendInTime );

	bool SetWalkRunUpDownVelocity( int iChannel, float fVelocity, int iBlendInTime );
	bool SetWalkRunUpDownVelocity( float fVelocity, int iBlendInTime );

	bool SetAnimNodeWeight( int iSpecType, float fWeight1, float fWeight2, int iBlendInTime );
	bool SetAnimNodeSpeed( int iChannel, int iSpecType, float fSpeed );
	bool SetAnimNodeSpeed( int iSpecType, float fSpeed );

protected:

	CECModel*	m_pECModel;
	float		m_fModelScale;

	A3DAnimNodeBlendWalkRun*	m_aBlendWalkRunNodes[ A3DSkinModel::ACTCHA_MAX ];
	AnimBlendTree*				m_aBlendWalkRunUpDownTrees[ A3DSkinModel::ACTCHA_MAX ];
	A3DAnimNodeBlend3Children*	m_aBlendRunTurnNodes[ A3DSkinModel::ACTCHA_MAX ];
};