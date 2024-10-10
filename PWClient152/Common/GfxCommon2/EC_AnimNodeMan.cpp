/*
 * FILE: EC_AnimNodeMan.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: zhangyachuan, 2010/10/25
 *
 * HISTORY:
 *
 * Copyright (c) 2010 Archosaur Studio, All Rights Reserved.	
 */

#include "StdAfx.h"
#include "EC_AnimNodeMan.h"
#include "EC_Model.h"

#define IS_STRING_NULL(str) (str == NULL || *str == '\0')

#ifdef _ANGELICA21

A3DAnimNodeBlend3Children::A3DAnimNodeBlend3Children()
	: m_fBlendWeight1(0)
	, m_fBlendWeight2(0)
	, m_fTargetBlendWeight1(0)
	, m_fTargetBlendWeight2(0)
	, m_fBlendWeightDelta1(0)
	, m_fBlendWeightDelta2(0)
	, m_fBlendT(0)
	, m_iBlendInTime(0)
	, m_pNode( NULL )
	, m_pNode1( NULL )
	, m_pNode2( NULL )
{
}

A3DAnimNodeBlend3Children::~A3DAnimNodeBlend3Children()
{
	Release();
}

bool A3DAnimNodeBlend3Children::Init( A3DSkinModel* pSkinModel, const char* szNode, const char* szNode1, const char* szNode2 )
{
	if( ! pSkinModel || IS_STRING_NULL( szNode ) || IS_STRING_NULL( szNode1 ) || IS_STRING_NULL( szNode2 ) ) 
		return false;

	Release();

	m_pNode = new A3DAnimNodeAction;
	if( ! m_pNode->Init( pSkinModel, szNode ) )
	{
		delete m_pNode;
		m_pNode = NULL;
		return false;
	}
	m_pNode1 = new A3DAnimNodeAction;
	if( ! m_pNode1->Init( pSkinModel, szNode1 ) )
	{
		delete m_pNode1;
		m_pNode1 = NULL;
		return false;
	}
	m_pNode2 = new A3DAnimNodeAction;
	if( ! m_pNode2->Init( pSkinModel, szNode2 ) )
	{
		delete m_pNode2;
		m_pNode2 = NULL;
		return false;
	}
	SetSkinModel( pSkinModel );

	AnimBlendChild abcNode;
	abcNode.mName		= "Node";
	abcNode.mAnimNode	= m_pNode;
	abcNode.mWeight		= 1 - m_fBlendWeight1 - m_fBlendWeight2;

	AnimBlendChild abcNode1;
	abcNode1.mName		= "Node1";
	abcNode1.mAnimNode	= m_pNode1;
	abcNode1.mWeight	= m_fBlendWeight1;

	AnimBlendChild abcNode2;
	abcNode2.mName		= "Node2";
	abcNode2.mAnimNode	= m_pNode2;
	abcNode2.mWeight	= m_fBlendWeight2;

	AddChild( abcNode );
	AddChild( abcNode1 );
	AddChild( abcNode2 );

	return true;
}

void A3DAnimNodeBlend3Children::Release()
{
	A3DAnimNodeBlendBase::Release();

	if( m_pNode )
	{
		m_pNode->Stop();
		delete m_pNode;
		m_pNode = NULL;
	}
	if( m_pNode1 )
	{
		m_pNode1->Stop();
		delete m_pNode1;
		m_pNode1 = NULL;
	}
	if( m_pNode2 )
	{
		m_pNode2->Stop();
		delete m_pNode2;
		m_pNode2 = NULL;
	}
}

void A3DAnimNodeBlend3Children::SetBlendWeight( float fBlendWeight1, float fBlendWeight2, int iBlendInTime )
{
	if( ! m_pNode || ! m_pNode1 || ! m_pNode2 )
		return;

	if( iBlendInTime <= 0 )
	{
		m_fBlendWeight1 = m_fTargetBlendWeight1 =  fBlendWeight1;
		m_fBlendWeight2 = m_fTargetBlendWeight2 =  fBlendWeight2;

		SetChildWeight( 0, 1 - m_fBlendWeight1 - m_fBlendWeight2 );
		SetChildWeight( 1, m_fBlendWeight1 );
		SetChildWeight( 2, m_fBlendWeight2 );
	}
	else
	{
		m_fTargetBlendWeight1 = fBlendWeight1;
		m_fTargetBlendWeight2 = fBlendWeight2;
		m_fBlendWeightDelta1 = m_fTargetBlendWeight1 - m_fBlendWeight1;
		m_fBlendWeightDelta2 = m_fTargetBlendWeight2 - m_fBlendWeight2;
		m_fBlendT = 0;
	}
	m_iBlendInTime = iBlendInTime;
}

void A3DAnimNodeBlend3Children::SetPlaySpeed( float fPlaySpeed )
{
	if( ! m_pNode || ! m_pNode1 || ! m_pNode2 )
		return;

	m_pNode->SetPlaySpeed( fPlaySpeed );
	m_pNode1->SetPlaySpeed( fPlaySpeed );
	m_pNode2->SetPlaySpeed( fPlaySpeed );
}

void A3DAnimNodeBlend3Children::Update( int iDeltaTime )
{
	if( ! m_pNode || ! m_pNode1 || ! m_pNode2 )
		return;

	if( m_iBlendInTime > 0 && m_fBlendT < 1 )
	{
		m_fBlendT += float( iDeltaTime ) / m_iBlendInTime;
		a_ClampRoof( m_fBlendT, 1.0f );

		m_fBlendWeight1 = m_fTargetBlendWeight1 - m_fBlendWeightDelta1 *( 1 - m_fBlendT );
		m_fBlendWeight2 = m_fTargetBlendWeight2 - m_fBlendWeightDelta2 *( 1 - m_fBlendT );

		SetChildWeight( 0, 1 - m_fBlendWeight1 - m_fBlendWeight2 );
		SetChildWeight( 1, m_fBlendWeight1 );
		SetChildWeight( 2, m_fBlendWeight2 );
	}
	A3DAnimNodeBlendBase::Update( iDeltaTime );
}

#endif

CECAnimNodeMan::CECAnimNodeMan()
	: m_pECModel( NULL )
	, m_fModelScale( 1.0f )
{
	ZeroMemory( m_aBlendWalkRunNodes,		sizeof( m_aBlendWalkRunNodes ) );
	ZeroMemory( m_aBlendWalkRunUpDownTrees, sizeof( m_aBlendWalkRunUpDownTrees ) );
	ZeroMemory( m_aBlendRunTurnNodes,		sizeof( m_aBlendRunTurnNodes ) );
}

CECAnimNodeMan::~CECAnimNodeMan()
{
}

static void _InitBlendWalkRunUpDownTree( AnimBlendTree*& pBlendWalkRunUpDownTree )
{
	pBlendWalkRunUpDownTree = new AnimBlendTree;
	AnimBlendNode* pRootNode = pBlendWalkRunUpDownTree->AddNode( "root", 1 );

	A3DAnimNodeBlendWalkRun* pWalkRunNode = new A3DAnimNodeBlendWalkRun;
	pBlendWalkRunUpDownTree->AddNode( "walkrun", 1, pWalkRunNode, pRootNode->mName );

	A3DAnimNodeAction* pWalkUpNode = new A3DAnimNodeAction;
	pBlendWalkRunUpDownTree->AddNode( "walkup", 0, pWalkUpNode, pRootNode->mName );

	A3DAnimNodeAction* pWalkDownNode = new A3DAnimNodeAction;
	pBlendWalkRunUpDownTree->AddNode( "walkdown", 0, pWalkDownNode, pRootNode->mName );

	A3DAnimNodeAction* pRunUpNode = new A3DAnimNodeAction;
	pBlendWalkRunUpDownTree->AddNode( "runup", 0, pRunUpNode, pRootNode->mName );

	A3DAnimNodeAction* pRunDownNode = new A3DAnimNodeAction;
	pBlendWalkRunUpDownTree->AddNode( "rundown", 0, pRunDownNode, pRootNode->mName );
}

bool CECAnimNodeMan::Init( CECModel* pECModel )
{
	ASSERT( pECModel && "Caller's responsibility to pass valid parameter" );
	Release();

	m_pECModel = pECModel;

	for( int i = 0; i < A3DSkinModel::ACTCHA_MAX; ++ i )
	{
		m_aBlendWalkRunNodes[i] = new A3DAnimNodeBlendWalkRun;

#ifdef _ANGELICA21
		_InitBlendWalkRunUpDownTree( m_aBlendWalkRunUpDownTrees[i] );

		m_aBlendRunTurnNodes[i]	= new A3DAnimNodeBlend3Children;
#endif
	}
	return true;
}

// should be released after the binding skin model released
void CECAnimNodeMan::Release()
{
	for( int i = 0; i < A3DSkinModel::ACTCHA_MAX; ++ i )
	{
		A3DRELEASE( m_aBlendWalkRunNodes[i] );

#ifdef _ANGELICA21
		A3DRELEASE( m_aBlendWalkRunUpDownTrees[i] );
		A3DRELEASE( m_aBlendRunTurnNodes[i] );
#endif
	}
	m_pECModel = NULL;
}

A3DAnimNode* CECAnimNodeMan::GetAnimNode( int iSpecType, int iChannel ) const
{
	switch( iSpecType )
	{
	case A3DCombinedAction::SPECIALTYPE_WALKRUN:
		return m_aBlendWalkRunNodes[ iChannel ];

	case A3DCombinedAction::SPECIALTYPE_WALKRUNUPDOWN:
		return m_aBlendWalkRunUpDownTrees[ iChannel ];

	case A3DCombinedAction::SPECIALTYPE_RUNTURN:
		return m_aBlendRunTurnNodes[ iChannel ];

	default:
		ASSERT(0);
		return NULL;
	}
}

void CECAnimNodeMan::SetModelScale( float fModelScale )
{
	if( m_fModelScale == fModelScale )
		return;

	m_fModelScale = fModelScale;

	for( int i = 0; i < A3DSkinModel::ACTCHA_MAX; ++ i )
	{
		m_aBlendWalkRunNodes[i]->SetModelScale( m_fModelScale );

#ifdef _ANGELICA21
		AnimBlendNode* pBlendNode = m_aBlendWalkRunUpDownTrees[i]->FindNode( "walkrun" );
		if( pBlendNode )
		{
			A3DAnimNode* pNode = pBlendNode->GetAnimNode();
			A3DAnimNodeBlendWalkRun* pWalkRunNode = dynamic_cast< A3DAnimNodeBlendWalkRun* >( pNode );
			if( pWalkRunNode )
				pWalkRunNode->SetModelScale( m_fModelScale );
		}
#endif
	}
}

bool CECAnimNodeMan::SetWalkRunVelocity( int iChannel, float fVelocity, int iBlendInTime )
{
	m_aBlendWalkRunNodes[ iChannel ]->SetVelocity( fVelocity, iBlendInTime );
	return true;
}

bool CECAnimNodeMan::SetWalkRunVelocity( float fVelocity, int iBlendInTime )
{
	for( int i = 0; i < A3DSkinModel::ACTCHA_MAX; ++ i )
		m_aBlendWalkRunNodes[i]->SetVelocity( fVelocity, iBlendInTime );

	return true;
}

static bool _SetWalkRunUpDownVelocity( AnimBlendTree* pWalkRunUpDownTree, float fVelocity, int iBlendInTime )
{
	AnimBlendNode* pBlendNode = pWalkRunUpDownTree->FindNode( "walkrun" );
	if( ! pBlendNode )
		goto Error;

	A3DAnimNode* pNode = pBlendNode->GetAnimNode();
	A3DAnimNodeBlendWalkRun* pWalkRunNode = dynamic_cast< A3DAnimNodeBlendWalkRun* >( pNode );
	if( ! pWalkRunNode )
		goto Error;

	pWalkRunNode->SetVelocity( fVelocity, iBlendInTime );

	float fDuration = pWalkRunNode->GetDuration();

	pBlendNode = pWalkRunUpDownTree->FindNode( "walkup" );
	if( ! pBlendNode )
		goto Error;
	
	pBlendNode->SetDuration( fDuration );

	pBlendNode = pWalkRunUpDownTree->FindNode( "walkdown" );
	if( ! pBlendNode )
		goto Error;

	pBlendNode->SetDuration( fDuration );

	pBlendNode = pWalkRunUpDownTree->FindNode( "runup" );
	if( ! pBlendNode )
		goto Error;

	pBlendNode->SetDuration( fDuration );

	pBlendNode = pWalkRunUpDownTree->FindNode( "rundown" );
	if( ! pBlendNode )
		goto Error;

	pBlendNode->SetDuration( fDuration );

	return true;

Error:

	ASSERT(0);
	return false;
}

bool CECAnimNodeMan::SetWalkRunUpDownVelocity( int iChannel, float fVelocity, int iBlendInTime )
{
	return _SetWalkRunUpDownVelocity( m_aBlendWalkRunUpDownTrees[ iChannel ], fVelocity, iBlendInTime );
}

bool CECAnimNodeMan::SetWalkRunUpDownVelocity( float fVelocity, int iBlendInTime )
{
	for( int i = 0; i < A3DSkinModel::ACTCHA_MAX; ++ i )
	{
		bool bRet = _SetWalkRunUpDownVelocity( m_aBlendWalkRunUpDownTrees[i], fVelocity, iBlendInTime );
		if( ! bRet )
			return false;
	}
	return true;
}

bool CECAnimNodeMan::SetAnimNodeWeight( int iSpecType, float fWeight1, float fWeight2, int iBlendInTime )
{
#ifdef _ANGELICA21
	switch( iSpecType )
	{
	case A3DCombinedAction::SPECIALTYPE_RUNTURN:
		{
			for( int i = 0; i < A3DSkinModel::ACTCHA_MAX; ++ i )
				m_aBlendRunTurnNodes[i]->SetBlendWeight( fWeight1, fWeight2, iBlendInTime );
		}
		return true;
	default:
		ASSERT(0);
		return false;
	}
#else
	return false;
#endif
}

bool CECAnimNodeMan::SetAnimNodeSpeed( int iSpecType, float fSpeed )
{
#ifdef _ANGELICA21
	switch( iSpecType )
	{
	case A3DCombinedAction::SPECIALTYPE_RUNTURN:
		{
			for( int i = 0; i < A3DSkinModel::ACTCHA_MAX; ++ i )
				m_aBlendRunTurnNodes[i]->SetPlaySpeed( fSpeed );
		}
		return true;
	default:
		ASSERT(0);
		return false;
	}
	return true;
#else
	return false;
#endif
}

bool CECAnimNodeMan::SetAnimNodeSpeed( int iChannel, int iSpecType, float fSpeed )
{
#ifdef _ANGELICA21
	switch( iSpecType )
	{
	case A3DCombinedAction::SPECIALTYPE_RUNTURN:
		m_aBlendRunTurnNodes[ iChannel ]->SetPlaySpeed( fSpeed );
		return true;
	default:
		ASSERT(0);
		return false;
	}
	return true;
#else
	return false;
#endif
}