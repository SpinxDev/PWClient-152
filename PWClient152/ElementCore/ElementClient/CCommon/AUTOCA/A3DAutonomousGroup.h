/*
 * FILE: A3DAutonomousGroup.h
 *
 * DESCRIPTION: The basic implementation of IAutonomousGroup interface
 *
 * CREATED BY: Changzhi Li, 2004/11/02
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DAUTONOMOUSGROUP_H_
#define _A3DAUTONOMOUSGROUP_H_

#include "vector.h"
#include "A3DAutonomousAgent.h"

#ifdef _ELEMENTCLIENT
#include "EC_Viewport.h"
#endif

class A3DTerrain2;
class A3DTerrainWater;
class A3DAutonomousBehaviour;


class A3DAutonomousGroup
{
public:
	A3DAutonomousGroup ()
		:	m_pActiveBehaviour(NULL),
			m_vCenter(0, 0, 0),
			m_fBoundaryRadius(20) { }
	
	void Release (void);

	void Init (void);
	void Reset (void);
	void Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,DWORD dwDeltaTime);

#ifdef _ELEMENT_EDITOR_
	void Render (A3DViewport* pViewport, bool bCheckVis = TRUE);
#endif 

#ifdef _AUTOCA_TEST_
	void Render (A3DViewport* pViewport, bool bCheckVis = TRUE);
#endif 

#ifdef _ELEMENTCLIENT
	void Render(CECViewport* pViewport, bool bCheckVis = TRUE);
#endif

	void AddMember (A3DAutonomousAgent* agent) { m_vpMembers.push_back(agent); }

	A3DAutonomousAgentVector& GetAllMembers (void)	{ return m_vpMembers; }
	const A3DAutonomousAgentVector& GetAllMembers (void) const { return m_vpMembers; }

	//void	SetTerrain (A3DTerrain2 * pTerrain)	{ m_pTerrain = pTerrain; }
	//A3DTerrain2 *	GetTerrain (void) const	{ return m_pTerrain; }

	//void	SetTerrainWater (A3DTerrainWater * pTerrainWater) { m_pTerrainWater = pTerrainWater; }
	//A3DTerrainWater * GetTerrainWater (void) const	{ return m_pTerrainWater; }

	void SetActiveBehaviour (A3DAutonomousBehaviour * pBehaviour) { m_pActiveBehaviour = pBehaviour; Reset(); }
	A3DAutonomousBehaviour * GetActiveBehaviour (void) const { return m_pActiveBehaviour; }	

	A3DVECTOR3 GetCenter (void) const { return m_vCenter; }
	void SetCenter (A3DVECTOR3 vCenter) { m_vCenter = vCenter; }
	void MoveCenter (A3DVECTOR3 vDelta) { m_vCenter += vDelta; }

	float	GetBoundaryRadius (void) { return m_fBoundaryRadius; }
	void	SetBoundaryRadius (float r)	{ m_fBoundaryRadius = r; }

protected:
	A3DAutonomousAgentVector m_vpMembers;
	A3DAutonomousBehaviour * m_pActiveBehaviour;

	A3DVECTOR3		m_vCenter;
	float			m_fBoundaryRadius;

	//A3DTerrain2 *			m_pTerrain;
	//A3DTerrainWater *		m_pTerrainWater;	
};

#endif