/*
 * FILE: A3DAutoButterfly.cpp
 *
 * DESCRIPTION: The implementation A3DAutoButterfly class
 *
 * CREATED BY: Changzhi Li, 2004/11/22
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.
 */

#include "A3DAutoButterfly.h"
#include "A3DAutonomousGroup.h"

void A3DAutoButterfly::Reset ()
{
	// Reset LocalSpace state
	ResetLocalSpace();
	SetSpeed(0);

	SetRadius(0.2);		
	SetMaxAcceleration(1.5);
	SetAcceleration(0, 0, 0);
	SetMaxSpeed(2.4);
	SetMinSpeed(0);
	SetBreakingRate(0);
	m_vAccelerationWander.Set(0, 0, 0);
	m_fBankAngle = 0;
	m_fBankAngleMax = 0.05;
	m_bDoingAction	= FALSE;
}

bool A3DAutoButterfly::Init (A3DEngine *pEngine)
{
	Reset();
	A3DVECTOR3 f = randomUnitVector();
	f.y=0;
	RegenerateLocalSpace(f, 0);

	m_pA3DEngine = pEngine;
	return m_pSkinModel->Init(pEngine);

}


void A3DAutoButterfly::Update (DWORD dwDeltaTime, ActionList_e action /*= ACTION_FIRST*/)
{
	ApplySteeringForce(dwDeltaTime*0.001f);
	ApplyBreakingForce(m_fBreakingRate);

	if(!m_bDoingAction)
	{
		if( m_bUseECModel)
		{
			if( action == ACTION_FIRST)
			{
				m_pECModel->PlayActionByName("·ÉÐÐ", 1.0f);
			}
		}
		else
		{
			if( action == ACTION_FIRST)
			{
				m_pSkinModel->PlayActionByNameDC("·ÉÐÐ", -1);
			}
		}
		m_bDoingAction = true;
	}
	
	A3DVECTOR3 vel = GetVelocity();

	if(vel.MagnitudeH()<0.0001)
		vel = GetForward();
	else
		vel.y = 0;

	RegenerateLocalSpace(vel, 0);
	float animrate = (GetSpeed()+1)*2;

	if( m_bUseECModel)
	{
		m_pECModel->SetPos(GetPosition());
		m_pECModel->SetDirAndUp(GetForward(), GetUp());
		m_pECModel->Tick(dwDeltaTime*animrate);
	}
	else
	{
		m_pSkinModel->SetPosAndDir(GetPosition(), GetForward(), GetUp());
		m_pSkinModel->Update(dwDeltaTime*animrate);
	}

}

A3DVECTOR3 A3DAutoButterfly::RespondToBoundary (const A3DVECTOR3& vCenter, float fBoundaryRadius)
{
	// if inside the sphere do nothing
	A3DVECTOR3 dis = vCenter - GetPosition();
	float len;
	if ( (len = dis.Normalize()) < fBoundaryRadius)
		return A3DVECTOR3(0, 0, 0);
	
	float delta = len - fBoundaryRadius;
//	if(fBoundaryRadius>1)
//		delta /= fBoundaryRadius;
//	if(delta>1)	delta = 1;

	return dis*delta*GetMaxAcceleration();
}

void A3DAutoButterfly::ClampPosition (A3DTerrain2 * pTerrain, A3DTerrainWater * pTerrainWater, DWORD dwDeltaTime)
{
	A3DVECTOR3 newpos = PredictPosition(5*dwDeltaTime);		
	if(pTerrain)		ClampBelowTerrainHeight(newpos, pTerrain, 5);
	if(pTerrain)		ClampAboveTerrain(newpos, pTerrain);
	if(pTerrainWater)	ClampAboveTerrainWater(newpos, pTerrainWater);
}

void A3DAutoButterfly::ClampAboveTerrain (A3DVECTOR3 pos, A3DTerrain2* pTerrain)
{
	// hard boundary
	A3DVECTOR3 vel = GetVelocity();
	float speed = vel.Magnitude();
	A3DVECTOR3 acc = GetAcceleration();
	float height = pTerrain->GetPosHeight(pos);
	float delta = height + 0.5 - pos.y;
	if(delta>0)
	{
		delta = clip(delta*0.5, 0, 1);
		A3DVECTOR3 newvel = A3DVECTOR3(vel.x*0.5, 0, vel.z*0.5);
		if(vel.y>0.3*GetMaxSpeed())
			newvel.y = vel.y;
		else
			newvel.y = 0.3*GetMaxSpeed();
		blendIntoAccumulator(delta, newvel, vel);
		vel.Normalize();
		vel *= speed;
		SetVelocity(vel);
	}
}

void A3DAutoButterfly::ClampAboveTerrainWater (A3DVECTOR3 pos, A3DTerrainWater* pTerrainWater)
{
	// hard boundary
	A3DVECTOR3 vel = GetVelocity();
	float speed = vel.Magnitude();
	A3DVECTOR3 acc = GetAcceleration();
	float delta = speed*0.1;
	pos.y -= 1.0;
	if(pTerrainWater->IsUnderWater(pos))
	{
		if(delta>0.7)	delta = 0.7;
		A3DVECTOR3 newvel = A3DVECTOR3(vel.x*0.5, 0, vel.z*0.5);
		if(vel.y>GetMaxSpeed()*0.5)
			newvel.y = vel.y;
		else
			newvel.y = GetMaxSpeed()*0.5;
		blendIntoAccumulator(delta, newvel, vel);
		vel.Normalize();
		vel *= speed;
		SetVelocity(vel);
	}
}

void A3DAutoButterfly::ClampBelowTerrainHeight (A3DVECTOR3 pos, A3DTerrain2* pTerrain, float fHeight)
{
	// hard boundary
	A3DVECTOR3 vel = GetVelocity();
	float speed = vel.Magnitude();
	A3DVECTOR3 acc = GetAcceleration();
	float height = pTerrain->GetPosHeight(pos);
	float delta = pos.y - (height + fHeight);
	if(delta>0)
	{
		delta = clip(delta*0.4, 0, 1);
		A3DVECTOR3 newvel = A3DVECTOR3(vel.x, 0, vel.z);
		if(vel.y<-0.2*GetMaxSpeed())
			newvel.y = vel.y;
		else
			newvel.y = -0.2*GetMaxSpeed();
		blendIntoAccumulator(delta, newvel, vel);
		vel.Normalize();
		vel *= speed;
		SetVelocity(vel);
	}
}


A3DVECTOR3 A3DAutoButterfly::SteerForWander (float fDeltaTime)
{
    float speed = 20 * fDeltaTime;
	
	m_vAccelerationWander.x = scalarRandomWalk(m_vAccelerationWander.x, speed, -m_fAccelerationMax, m_fAccelerationMax);
//	m_vAccelerationWander.y = scalarRandomWalk(m_vAccelerationWander.y, speed, -m_fAccelerationMax, m_fAccelerationMax);
	m_vAccelerationWander.z = scalarRandomWalk(m_vAccelerationWander.z, speed, -m_fAccelerationMax, m_fAccelerationMax);

    return m_vAccelerationWander;
}

A3DVECTOR3 A3DAutoButterfly::SteerForSeek (const A3DVECTOR3& target)
{
    A3DVECTOR3 desiredVelocity = target - GetPosition();
    A3DVECTOR3 seek = desiredVelocity - GetVelocity();
	return clip(seek, 0, GetMaxAcceleration());
}

A3DVECTOR3 A3DAutoButterfly::SteerForFlee (const A3DVECTOR3& target)
{
    A3DVECTOR3 desiredVelocity = GetPosition() - target;
    A3DVECTOR3 flee = desiredVelocity - GetVelocity();
	return clip(flee, 0, GetMaxAcceleration());	
}

bool A3DAutoButterfly::IsInBoidNeighborhood (const A3DAutonomousAgent* other, float minDistance, float maxDistance, float cosMaxAngle)
{
    if (other == this)       return false;
    else
    {
		A3DVECTOR3 offset = other->GetPosition() - GetPosition();
		float distance = offset.Magnitude();

        // definitely in neighborhood if inside minDistance sphere
        if (distance < minDistance)		return true;
        else
        {
            // definitely not in neighborhood if outside maxDistance sphere
            if (distance > maxDistance)	return false;
            else
            {
                // otherwise, test angular offset from forward axis
				offset.Normalize();
                float forwardness = DotProduct(GetForward(), offset);
                return forwardness > cosMaxAngle;
            }
        }
		return true;
    }
	return false;
}

A3DVECTOR3 A3DAutoButterfly::SteerForSeparation (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
{
    A3DVECTOR3 steering(0, 0, 0);
    int neighbors = 0;

	const A3DAutonomousAgentVector & members = pGroup->GetAllMembers();
	for( unsigned int i=0; i<members.size(); i++)
    {
		A3DAutonomousAgent * mbr = members[i];
		float minDistance = GetRadius()*4;
        if (IsInBoidNeighborhood (mbr, minDistance, maxDistance, cosMaxAngle))
        {
            A3DVECTOR3 offset = GetPosition() - mbr->GetPosition();
			if(offset.Magnitude() > GetRadius())
				steering += offset/offset.SquaredMagnitude();
			else
				steering += 5*offset/(GetRadius()*GetRadius());
            neighbors++;
        }
    }
    if (neighbors > 0)
	{
		steering /= neighbors;
	}
    return clip(steering, 0, GetMaxAcceleration());
}


A3DVECTOR3 A3DAutoButterfly::SteerForAlignment (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
{
    A3DVECTOR3 steering(0, 0, 0);
    int neighbors = 0;

	const A3DAutonomousAgentVector & members = pGroup->GetAllMembers();
	for( unsigned int i=0; i<members.size(); i++)
    {
        if (IsInBoidNeighborhood(members[i], GetRadius()*3, maxDistance, cosMaxAngle))
        {
            // accumulate sum of neighbor's heading
            steering += members[i]->GetForward();
            // count neighbors
            neighbors++;
        }
    }

    // divide by neighbors, subtract off current heading to get error-
    // correcting direction, then normalize to pure direction
    if (neighbors > 0) 
	{
		steering /= neighbors;
		steering -= GetForward();
	}
    return clip(steering, 0, GetMaxAcceleration());
}


A3DVECTOR3 A3DAutoButterfly::SteerForCohesion (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
{
    // steering accumulator and count of neighbors, both initially zero
    A3DVECTOR3 steering(0, 0, 0);
    int neighbors = 0;

	const A3DAutonomousAgentVector & members = pGroup->GetAllMembers();
	for( unsigned int i=0; i<members.size(); i++)
    {
        if (IsInBoidNeighborhood(members[i], GetRadius()*3, maxDistance, cosMaxAngle))
        {
            // accumulate sum of neighbor's positions
            steering += members[i]->GetPosition();
            // count neighbors
            neighbors++;
        }
    }

    // divide by neighbors, subtract off current position to get error-
    // correcting direction, then normalize to pure direction
    if (neighbors > 0) 
	{
		steering /= neighbors; 
		steering -= GetPosition();
	}
	
    return clip(steering, 0, GetMaxAcceleration());
}

void A3DAutoButterfly::ApplySteeringForce (float fElapsedTime)	// time in sec
{
	float rate = 0.5;
	A3DVECTOR3 newVelocity = m_vVelocity+m_vAcceleration*fElapsedTime;

	// smooth velocity, damping factor
	blendIntoAccumulator(rate, newVelocity, m_vVelocity);
	// add turbulence
	m_vVelocity += 
		A3DVECTOR3((frandom01()-0.5f)*0.05, (frandom01()-0.5f)*0.3, (frandom01()-0.5f)*0.05)
		*m_fSpeedMax;
	m_vVelocity = clip(m_vVelocity, m_fSpeedMin, m_fSpeedMax);
	m_vPosition += m_vVelocity*fElapsedTime;
}
