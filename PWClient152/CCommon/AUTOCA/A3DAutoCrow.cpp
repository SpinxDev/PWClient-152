/*
 * FILE: A3DAutoCrow.cpp
 *
 * DESCRIPTION: The implementation A3DAutoCrow class
 *
 * CREATED BY: liyi, 2005/10/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.
 */

#include "A3DAutoCrow.h"
#include "A3DAutonomousGroup.h"


void A3DAutoCrow::Reset ()
{
	ResetLocalSpace();
	m_vAccelerationWander.Set(0, 0, 0);
	SetRadius(1);
	SetSpeed(0);
	
	SetMaxAcceleration(9);
	SetAcceleration(0, 0, 0);
	SetMaxSpeed(7.0f);
	SetMinSpeed(2.0f);
	SetBreakingRate(0);
	
	m_fBankAngle = 0;
	m_fBankAngleMax = 0.25*PI;
	m_bDoingAction	= FALSE;	
}

bool A3DAutoCrow::Init (A3DEngine *pEngine)
{
	Reset();
	A3DVECTOR3 f = randomUnitVector();
	if(f.y<0) f.y=-f.y;
	RegenerateLocalSpace(f, 0);

	m_pA3DEngine = pEngine;
	return m_pSkinModel->Init(pEngine);
}

void A3DAutoCrow::Update (DWORD dwDeltaTime, ActionList_e action /*= ACTION_FIRST*/)
{
	ApplySteeringForce(dwDeltaTime*0.001f);
	ApplyBreakingForce(m_fBreakingRate);

	if(m_CurAction != action)
	{
		if( m_bUseECModel)
		{
			if( action == ACTION_FIRST)
			{
				m_pECModel->PlayActionByName("øÏÀŸ∑…––", 1.0);
			}
			else if( action == ACTION_SECOND)
			{
				m_pECModel->PlayActionByName("–›œ–", 1.0);
			}

			m_CurAction = action;
		}
		else
		{
//			if( action == ACTION_FIRST)
//			{
//				m_pSkinModel->PlayActionByName("øÏÀŸ∑…––", 1.0);
//			}
//			else if( action == ACTION_SECOND)
//			{
//				m_pSkinModel->PlayActionByName("–›œ–", 1.0);
//			}

			m_pSkinModel->PlayActionByNameDC("…»∂Ø≥·∞Ú", -1); //test
			
			m_CurAction = action;
		}

		m_bDoingAction = true;
	}
	
	A3DVECTOR3 vel = GetVelocity();
	vel = truncateLength(vel, 1.0);
	
	if(vel.SquaredMagnitude()<0.01)
		vel = GetForward();

	A3DVECTOR3 acc = GetAcceleration();
	acc = truncateLength(acc, 1.0);

	A3DVECTOR3 temp = CrossProduct(vel, acc);
	float maxBank = GetMaxBankAngle();
	float newbank = temp.y*GetMaxBankAngle();

	float bank = GetBankAngle();

	blendIntoAccumulator(0.15, newbank, bank);

	RegenerateLocalSpace(vel, bank);
	A3DVECTOR3 up = GetUp();
	
	float animrate = GetSpeed()*0.5;
	if(up.y>0)	animrate *= 1.2;
	else		animrate *= 0.9;

	if( m_bUseECModel)
	{
		m_pECModel->SetPos(GetPosition());
		m_pECModel->SetDirAndUp(GetForward(), GetUp());

		if( m_CurAction == ACTION_SECOND)//–›œ–◊¥Ã¨
		{
			m_pECModel->Tick(dwDeltaTime);
		}
		else
		{
			m_pECModel->Tick(dwDeltaTime*animrate);
		}
	}
	else
	{

		m_pSkinModel->SetPosAndDir(GetPosition(), GetForward(), GetUp());

		m_pSkinModel->Update(dwDeltaTime*animrate);
		

	}
}

void A3DAutoCrow::ApplySteeringForce (float fElapsedTime)
{
	float rate = 0.21;
	A3DVECTOR3 newVelocity = m_vVelocity+m_vAcceleration*fElapsedTime;

	// smooth velocity, damping factor
	blendIntoAccumulator(rate, newVelocity, m_vVelocity);

	m_vVelocity = clip(m_vVelocity, m_fSpeedMin, m_fSpeedMax);
	m_vPosition += m_vVelocity*fElapsedTime;
}

A3DVECTOR3 A3DAutoCrow::RespondToBoundary (const A3DVECTOR3& vCenter, float fBoundaryRadius)
{
	// if inside the sphere do nothing
	A3DVECTOR3 dis = vCenter - GetPosition();
	float len;
	if ( (len = dis.Normalize()) < fBoundaryRadius)
		return A3DVECTOR3(0, 0, 0);
	
	float delta = len - fBoundaryRadius;
	float cushion = fBoundaryRadius;
	if(delta>cushion)	delta = cushion;
	A3DVECTOR3 seek = SteerForSeek(vCenter);	
	A3DVECTOR3 res = dis*delta + perpendicularComponent(seek, GetForward());
	return clip(res, 0, GetMaxAcceleration());
}

void A3DAutoCrow::ClampPosition (A3DTerrain2 * pTerrain, A3DTerrainWater * pTerrainWater, DWORD dwDeltaTime)
{
	A3DVECTOR3 newpos = PredictPosition(5*dwDeltaTime);		
	if(pTerrain)		ClampAboveTerrain(newpos, pTerrain);
	if(pTerrainWater)	ClampAboveTerrainWater(newpos, pTerrainWater);
}

void A3DAutoCrow::ClampAboveTerrain (A3DVECTOR3 pos, A3DTerrain2* pTerrain)
{
	// hard boundary
	A3DVECTOR3 vel = GetVelocity();
	float speed = vel.Magnitude();
	A3DVECTOR3 acc = GetAcceleration();
	float height = pTerrain->GetPosHeight(pos);
	float delta = height + speed*0.2 - pos.y;
	if(delta>0)
	{
		delta = clip(delta, 0, 0.8);
		A3DVECTOR3 newvel = A3DVECTOR3(vel.x*0.5, 0.1*GetMaxSpeed(), vel.z*0.5);
		if(vel.y>0.1*GetMaxSpeed())
			newvel.y = vel.y;

		blendIntoAccumulator(delta, newvel, vel);
		vel.Normalize();
		vel *= speed;
		SetVelocity(vel);
	}
}

void A3DAutoCrow::ClampAboveTerrainWater (A3DVECTOR3 pos, A3DTerrainWater* pTerrainWater)
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
		if(vel.y>1)
			newvel.y = vel.y;
		else
			newvel.y = 1;
		blendIntoAccumulator(delta, newvel, vel);
		vel.Normalize();
		vel *= speed;
		SetVelocity(vel);
	}
}

A3DVECTOR3 A3DAutoCrow::SteerForWander (float dt)
{
	float speed = 16 * dt;
	
	m_vAccelerationWander.x = scalarRandomWalk(m_vAccelerationWander.x, speed, -m_fAccelerationMax, m_fAccelerationMax);
	m_vAccelerationWander.y = scalarRandomWalk(m_vAccelerationWander.y, speed, -m_fAccelerationMax, m_fAccelerationMax);
	m_vAccelerationWander.z = scalarRandomWalk(m_vAccelerationWander.z, speed, -m_fAccelerationMax, m_fAccelerationMax);
//	m_vAccelerationWander.x = (frandom01()-0.5)*2.0f*m_fAccelerationMax;
//	m_vAccelerationWander.y = (frandom01()-0.5)*2.0f*m_fAccelerationMax;
//	m_vAccelerationWander.z = (frandom01()-0.5)*2.0f*m_fAccelerationMax;

	clip(m_vAccelerationWander, 0, m_fAccelerationMax);
    return m_vAccelerationWander;
}

A3DVECTOR3 A3DAutoCrow::SteerForSeek (const A3DVECTOR3& target)
{
    A3DVECTOR3 desiredVelocity = target - GetPosition();
    return desiredVelocity - GetVelocity();
}

A3DVECTOR3 A3DAutoCrow::SteerForFlee (const A3DVECTOR3& target)
{
    A3DVECTOR3 desiredVelocity = GetPosition() - target;
    A3DVECTOR3 flee = desiredVelocity - GetVelocity();
	return clip(flee, 0, GetMaxAcceleration());	
}

bool A3DAutoCrow::IsInBoidNeighborhood (const A3DAutonomousAgent* other, float minDistance, float maxDistance, float cosMaxAngle)
{
    if (other == this)	return false;
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

A3DVECTOR3 A3DAutoCrow::SteerForSeparation (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
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


A3DVECTOR3 A3DAutoCrow::SteerForAlignment (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
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


A3DVECTOR3 A3DAutoCrow::SteerForCohesion (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
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
            neighbors++;	            // count neighbors
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

void A3DAutoCrow::RegenerateLocalSpace (A3DVECTOR3 vNewForward, float fBankAngle)
{
	if(vNewForward.Normalize() <= 0.0001)
		return;

	float cos_theta = vNewForward.y; // DotProduct(Y, vNewForward);
	float theta;	// thrusting angle, the angle between vector local_z and Y

	// clamp thrusting angle theta

	float maxTheta = PI*0.4;
	float cos_maxTheta = cos(maxTheta);
	
	if(fabs(vNewForward.y) > cos_maxTheta)
	{
		if(vNewForward.y>0)
		{
			vNewForward.y = cos_maxTheta;
			cos_theta = cos_maxTheta;
			theta = maxTheta;
		}
		else
		{
			vNewForward.y = -cos_maxTheta;
			cos_theta = -cos_maxTheta;
			theta = PI-maxTheta;
		}
		vNewForward.Normalize();
	}
	else
		theta = acos(cos_theta);	// acos(x) return [0, PI]

	// it can be proved that bank angle should be no large than theta
	// clamp the fBankAngle

	float maxBank = min(m_fBankAngleMax, (theta<PI*0.5f) ? theta : PI-theta);
	if(fabs(fBankAngle) > maxBank)
		fBankAngle = fBankAngle>0 ? maxBank : -maxBank;
	float bank = GetBankAngle();
	blendIntoAccumulator(0.1, fBankAngle, bank);
	SetBankAngle(bank);

	A3DVECTOR3 q(-vNewForward.z, 0, vNewForward.x); //= CrossProduct(vNewForward, Y);
	q.Normalize();
	
	float sin_theta = sqrt(1-cos_theta*cos_theta);
	float cos_bank = cos(fBankAngle);
	float sin_bank = sin(fBankAngle);

	A3DVECTOR3 p = -vNewForward*cos_theta; p.y += 1; // p = Y - vNewForward*cos_theta;	// not abs(cos_theta) !!
	p.Normalize();
	p *= sin_bank/(cos_bank*sin_theta);

	float R = 1.0f/cos_bank;
	float r = sin_bank*cos_theta/(cos_bank*sin_theta);	
	q *= sqrt(R*R-r*r);

	SetForward(vNewForward);
	SetSide( Normalize(p+q) );
	SetUp(CrossProduct(GetSide(), GetForward()));
}
