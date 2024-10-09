/*
 * FILE: A3DAutonomousGroup.cpp
 *
 * DESCRIPTION: The basic implementation of IAutonomousGroup interface
 *
 * CREATED BY: Changzhi Li, 2004/11/02
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.
 */

#include "Utilities.h"
#include "A3DAutonomousAgent.h"
#include "A3DAutonomousGroup.h"
#include "A3DAutonomousBehaviour.h"


void A3DAutonomousGroup::Init (void)
{
	if(m_pActiveBehaviour)
		m_pActiveBehaviour->Initialize(this);
	else
	{
		for(unsigned int i=0; i<m_vpMembers.size(); i++)
		{
			A3DAutonomousAgent * mbr = m_vpMembers[i];
			mbr->SetPosition(randomVectorInUnitSphere()*GetBoundaryRadius()+GetCenter());
			mbr->SetSpeed(mbr->GetMaxSpeed()*(frandom01()+1)*0.5);
	 		mbr->SetMinSpeed(0);
			mbr->SetBreakingRate(0);
		}
	}	
}

void A3DAutonomousGroup::Reset (void)
{
	if(m_pActiveBehaviour)
		m_pActiveBehaviour->Reset(this);
	else
		Init();
}


void A3DAutonomousGroup::Release (void) 
{ 
	for(unsigned int i=0; i<m_vpMembers.size(); i++)
		A3DRELEASE(m_vpMembers[i]);
	m_vpMembers.clear(); 
}

void A3DAutonomousGroup::Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,DWORD dwDeltaTime)
{
	if(m_pActiveBehaviour == NULL)	return;
	m_pActiveBehaviour->Update(pTerrain,pTerrainWater,this, dwDeltaTime);
}

#ifdef _ELEMENT_EDITOR_
void A3DAutonomousGroup::Render (A3DViewport* pViewport, bool bCheckVis)
{
	for(unsigned int i=0; i<m_vpMembers.size(); i++)
		m_vpMembers[i]->Render(pViewport, bCheckVis);
}
#endif

#ifdef _AUTOCA_TEST_
void A3DAutonomousGroup::Render (A3DViewport* pViewport, bool bCheckVis)
{
	for(unsigned int i=0; i<m_vpMembers.size(); i++)
		m_vpMembers[i]->Render(pViewport, bCheckVis);
}
#endif

#ifdef _ELEMENTCLIENT
void A3DAutonomousGroup::Render (CECViewport* pViewport, bool bCheckVis)
{
	for(unsigned int i=0; i<m_vpMembers.size(); i++)
		m_vpMembers[i]->Render(pViewport, bCheckVis);
}
#endif





