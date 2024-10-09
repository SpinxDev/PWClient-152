      /*
 * FILE: A3DAutoFish.cpp
 *
 * DESCRIPTION: The implementation A3DAutoFish class
 *
 * CREATED BY: Changzhi Li, 2004/11/22
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.
 */

#include "A3DAutoFish.h"
#include "A3DAutonomousGroup.h"

void A3DAutoFish::Reset()
{
	// Reset LocalSpace state
	ResetLocalSpace();
	m_vAccelerationWander.Set(0, 0, 0);
	SetSpeed(0);

	SetRadius(0.2);
		
	SetMaxAcceleration(3);
	SetAcceleration(0, 0, 0);
	SetMaxSpeed(2);
	SetMinSpeed(0);
	SetBreakingRate(0);
	m_fBankAngle = 0;
	m_fBankAngleMax = 0.05*PI;
	m_bDoingAction	= FALSE;
}

void A3DAutoFish::Update (DWORD dwDeltaTime, ActionList_e action /*= ACTION_FIRST*/)
{
	ApplySteeringForce(dwDeltaTime*0.001f);
	ApplyBreakingForce(m_fBreakingRate);

	if(!m_bDoingAction)
	{
		if( m_bUseECModel)
		{
			if( action == ACTION_FIRST)
			{
				m_pECModel->PlayActionByName("游动", 1.0f);
			}
		}
		else
		{
			if( action == ACTION_FIRST)
			{
				m_pSkinModel->PlayActionByName("游动", 1.0f);
			}
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
	
	float animrate = GetSpeed()*1.5;

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

A3DVECTOR3 A3DAutoFish::RespondToBoundary (const A3DVECTOR3& vCenter, float fBoundaryRadius)
{
	A3DVECTOR3 dis = vCenter - GetPosition();
	float len;
	if ( (len = dis.Normalize()) < fBoundaryRadius)
		return A3DVECTOR3(0, 0, 0);
	
	float delta = len - fBoundaryRadius;
	float cushion = fBoundaryRadius*0.4;
	if(delta>cushion)	delta = cushion;
	A3DVECTOR3 seek = SteerForSeek(vCenter);	
	A3DVECTOR3 res = dis*delta*2 + perpendicularComponent(seek, GetForward());
	return clip(res, 0, GetMaxAcceleration());

/*	// if inside the sphere do nothing
	A3DVECTOR3 dis = vCenter - GetPosition();
	float len;
	if ( (len = dis.Normalize()) < fBoundaryRadius)
		return A3DVECTOR3(0, 0, 0);
	
	float delta = len - fBoundaryRadius;
//	if(fBoundaryRadius>1)
//		delta /= fBoundaryRadius;
//	if(delta>1)	delta = 1;
	
	return dis*delta*delta*GetMaxAcceleration();
*/
}

void A3DAutoFish::ClampPosition (A3DTerrain2 * pTerrain, A3DTerrainWater * pTerrainWater, DWORD dwDeltaTime)
{
	A3DVECTOR3 newpos = PredictPosition(4*dwDeltaTime);		
	if(pTerrain)		ClampAboveTerrain(newpos, pTerrain);
	if(pTerrainWater)	ClampUnderTerrainWater(newpos, pTerrainWater);
}

void A3DAutoFish::ClampAboveTerrain (A3DVECTOR3 pos, A3DTerrain2* pTerrain)
{
	A3DVECTOR3 vel = GetVelocity();
	float speed = vel.Magnitude();
	A3DVECTOR3 acc = GetAcceleration();
	float height = pTerrain->GetPosHeight(pos);
	float delta = height + 1 - pos.y;
	if(delta>0)
	{
		delta = clip(delta, 0.6, 0.9);
		A3DVECTOR3 newvel = A3DVECTOR3(-0.4*vel.x, -0.8*vel.y, -0.4*vel.z);
		if(vel.y>0.1*GetMaxSpeed())
			newvel.y = vel.y;
		
		blendIntoAccumulator(delta, newvel, vel);
		vel.Normalize();
		vel *= speed;
		SetVelocity(vel);
	}
}

void A3DAutoFish::ClampUnderTerrainWater (A3DVECTOR3 pos, A3DTerrainWater* pTerrainWater)
{
	A3DVECTOR3 vel = GetVelocity();
	float speed = vel.Magnitude();
	A3DVECTOR3 acc = GetAcceleration();
	float delta = speed*0.1;
	pos.y += 0.1; 
	if(!pTerrainWater->IsUnderWater(pos))
	{
		delta = clip(delta, 0.3, 0.7);
		A3DVECTOR3 newvel = A3DVECTOR3(vel.x, -0.2*GetMaxSpeed(), vel.z);
		if(vel.y<-0.2*GetMaxSpeed())
			newvel.y = vel.y;

		float dis = pos.y - pTerrainWater->GetWaterHeight(pos);
		if(dis>GetRadius())
		{
			newvel.x = -newvel.x;
			newvel.z = -newvel.z;
			delta = 0.8;
		}

		blendIntoAccumulator(delta, newvel, vel);
		vel.Normalize();
		vel *= speed;
		SetVelocity(vel);
	}
}

A3DVECTOR3 A3DAutoFish::SteerForWander (float dt)
{
    float speed = 5*dt;
	
	m_vAccelerationWander.x = scalarRandomWalk(m_vAccelerationWander.x, speed, -m_fAccelerationMax, m_fAccelerationMax);
	m_vAccelerationWander.y = scalarRandomWalk(m_vAccelerationWander.y, speed, -m_fAccelerationMax, m_fAccelerationMax);
	m_vAccelerationWander.z = scalarRandomWalk(m_vAccelerationWander.z, speed, -m_fAccelerationMax, m_fAccelerationMax);

    return clip(m_vAccelerationWander, 0, GetMaxAcceleration());
}

A3DVECTOR3 A3DAutoFish::SteerForSeek (const A3DVECTOR3& target)
{
    A3DVECTOR3 desiredVelocity = target - GetPosition();
    A3DVECTOR3 seek = desiredVelocity - GetVelocity();
	return clip(seek, 0, GetMaxAcceleration());
}

A3DVECTOR3 A3DAutoFish::SteerForFlee (const A3DVECTOR3& target)
{
    A3DVECTOR3 desiredVelocity = GetPosition() - target;
    A3DVECTOR3 flee = desiredVelocity - GetVelocity();
	return clip(flee/GetRadius(), 0, GetMaxAcceleration());	
}

bool A3DAutoFish::IsInBoidNeighborhood (const A3DAutonomousAgent* other, float minDistance, float maxDistance, float cosMaxAngle)
{
    if (other == this)	return false;
    else
    {
		A3DVECTOR3 offset = other->GetPosition() - GetPosition();
		float distance = offset.Magnitude();

        // definitely in neighborhood if inside minDistance sphere
        if (distance < minDistance)	return true;
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
}

A3DVECTOR3 A3DAutoFish::SteerForSeparation (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
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


A3DVECTOR3 A3DAutoFish::SteerForAlignment (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
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


A3DVECTOR3 A3DAutoFish::SteerForCohesion (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
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


void A3DAutoFish::ApplySteeringForce (float fElapsedTime)
{
	float rate = 0.21;
	A3DVECTOR3 newVelocity = m_vVelocity+m_vAcceleration*fElapsedTime;
	// smooth velocity, damping factor
	blendIntoAccumulator(rate, newVelocity, m_vVelocity);

	// thrust suddenly
	if(rand()%10 == 0)
	{
		if(GetSpeed()<0.05*GetMaxSpeed())
		{
			SetSpeed(0.8*GetMaxSpeed());
			SetBreakingRate(frandom01()*0.01);
		}
		else
		{
			SetBreakingRate(0);
		}
	}

	m_vVelocity = clip(m_vVelocity, m_fSpeedMin, m_fSpeedMax);
	m_vPosition += m_vVelocity*fElapsedTime;
}

void A3DAutoFish::RegenerateLocalSpace (A3DVECTOR3 vNewForward, float fBankAngle)
{
	if(vNewForward.Normalize() <= 0.0001)
		return;

	float cos_theta = vNewForward.y; // DotProduct(Y, vNewForward);
	float theta;	// thrusting angle, the angle between vector local_z and Y

	// clamp thrusting angle theta

	float maxTheta = PI*0.1;
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

	if(fBankAngle > maxBank)
		fBankAngle = maxBank;
	else if(fBankAngle < -maxBank)
		fBankAngle = -maxBank;

	SetBankAngle(fBankAngle);

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

//////////////////////////////////////////////////////////////////////////

void A3DAutoBriskFish::Reset ()
{
	// Reset LocalSpace state
	ResetLocalSpace();
	m_vAccelerationWander.Set(0, 0, 0);
	SetSpeed(0);

	SetRadius(0.2);
		
	SetMaxAcceleration(5);
	SetAcceleration(0, 0, 0);
	SetMaxSpeed(3);
	SetMinSpeed(0);
	SetBreakingRate(0);
	m_fBankAngle = 0;
	m_fBankAngleMax = 0.1*PI;
	m_bDoingAction	= FALSE;
}

void A3DAutoBriskFish::Update (DWORD dwDeltaTime, ActionList_e action /*= ACTION_FIRST*/)
{
	ApplySteeringForce(dwDeltaTime*0.001f);
	ApplyBreakingForce(m_fBreakingRate);

	if(!m_bDoingAction)
	{
		if( m_bUseECModel)
		{
			if( action == ACTION_FIRST)
			{
				m_pECModel->PlayActionByName("游动", 1.0f);
			}
		}
		else
		{
			if( action == ACTION_FIRST)
			{
				m_pSkinModel->PlayActionByNameDC("游动", -1);
			}
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

	blendIntoAccumulator(0.2, newbank, bank);

	RegenerateLocalSpace(vel, bank);
	A3DVECTOR3 up = GetUp();
	
	float animrate = (GetSpeed()+1)*4;

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

A3DVECTOR3 A3DAutoBriskFish::RespondToBoundary (const A3DVECTOR3& vCenter, float fBoundaryRadius)
{
	A3DVECTOR3 dis = vCenter - GetPosition();
	float len;
	if ( (len = dis.Normalize()) < fBoundaryRadius)
		return A3DVECTOR3(0, 0, 0);
	
	float delta = len - fBoundaryRadius;
	float cushion = fBoundaryRadius*0.4;
	if(delta>cushion)	delta = cushion;
	A3DVECTOR3 seek = SteerForSeek(vCenter);	
	A3DVECTOR3 res = dis*delta*2 + perpendicularComponent(seek, GetForward());
	return clip(res, 0, GetMaxAcceleration());

/*	// if inside the sphere do nothing
	A3DVECTOR3 dis = vCenter - GetPosition();
	float len;
	if ( (len = dis.Normalize()) < fBoundaryRadius)
		return A3DVECTOR3(0, 0, 0);
	
	float delta = len - fBoundaryRadius;
//	if(fBoundaryRadius>1)
//		delta /= fBoundaryRadius;
//	if(delta>1)	delta = 1;
	
	return dis*delta*delta*GetMaxAcceleration();
*/
}

void A3DAutoBriskFish::ClampPosition (A3DTerrain2 * pTerrain, A3DTerrainWater * pTerrainWater, DWORD dwDeltaTime)
{
	A3DVECTOR3 newpos = PredictPosition(5*dwDeltaTime);		
	if(pTerrain)		ClampAboveTerrain(newpos, pTerrain);
	if(pTerrainWater)	ClampUnderTerrainWater(newpos, pTerrainWater);
}

void A3DAutoBriskFish::ClampAboveTerrain (A3DVECTOR3 pos, A3DTerrain2* pTerrain)
{
	A3DVECTOR3 vel = GetVelocity();
	float speed = vel.Magnitude();
	A3DVECTOR3 acc = GetAcceleration();
	float height = pTerrain->GetPosHeight(pos);
	float delta = height + 1 - pos.y;
	if(delta>0)
	{
		delta = clip(delta, 0.6, 0.9);
		A3DVECTOR3 newvel = A3DVECTOR3(-0.5*vel.x, -0.8*vel.y, -0.5*vel.z);
		if(vel.y>0.1*GetMaxSpeed())
			newvel.y = vel.y;
		
		blendIntoAccumulator(delta, newvel, vel);
		vel.Normalize();
		vel *= speed;
		SetVelocity(vel);
	}
}

void A3DAutoBriskFish::ClampUnderTerrainWater (A3DVECTOR3 pos, A3DTerrainWater* pTerrainWater)
{
	A3DVECTOR3 vel = GetVelocity();
	float speed = vel.Magnitude();
	A3DVECTOR3 acc = GetAcceleration();
	float delta = speed*0.1;

	if(!pTerrainWater->IsUnderWater(pos))
	{
		delta = clip(delta, 0.3, 0.7);
		A3DVECTOR3 newvel = A3DVECTOR3(vel.x, -0.2*GetMaxSpeed(), vel.z);
		if(vel.y<-0.2*GetMaxSpeed())
			newvel.y = vel.y;

		float dis = pos.y - pTerrainWater->GetWaterHeight(pos);
		if(dis>GetRadius())
		{
			newvel.x = -newvel.x;
			newvel.z = -newvel.z;
			delta = 0.9;
		}

		blendIntoAccumulator(delta, newvel, vel);
		vel.Normalize();
		vel *= speed;
		SetVelocity(vel);
	}
}

A3DVECTOR3 A3DAutoBriskFish::SteerForWander (float dt)
{
    float speed = 5*dt;
	
	m_vAccelerationWander.x = scalarRandomWalk(m_vAccelerationWander.x, speed, -m_fAccelerationMax, m_fAccelerationMax);
	m_vAccelerationWander.y = scalarRandomWalk(m_vAccelerationWander.y, speed, -m_fAccelerationMax, m_fAccelerationMax);
	m_vAccelerationWander.z = scalarRandomWalk(m_vAccelerationWander.z, speed, -m_fAccelerationMax, m_fAccelerationMax);

    return clip(m_vAccelerationWander, 0, GetMaxAcceleration());
}

A3DVECTOR3 A3DAutoBriskFish::SteerForSeek (const A3DVECTOR3& target)
{
    A3DVECTOR3 desiredVelocity = target - GetPosition();
    A3DVECTOR3 seek = desiredVelocity - GetVelocity();
	return clip(seek, 0, GetMaxAcceleration());
}

A3DVECTOR3 A3DAutoBriskFish::SteerForFlee (const A3DVECTOR3& target)
{
    A3DVECTOR3 desiredVelocity = GetPosition() - target;
    A3DVECTOR3 flee = desiredVelocity - GetVelocity();
	return clip(flee/GetRadius(), 0, GetMaxAcceleration());	
}

bool A3DAutoBriskFish::IsInBoidNeighborhood (const A3DAutonomousAgent* other, float minDistance, float maxDistance, float cosMaxAngle)
{
    if (other == this)	return false;
    else
    {
		A3DVECTOR3 offset = other->GetPosition() - GetPosition();
		float distance = offset.Magnitude();

        // definitely in neighborhood if inside minDistance sphere
        if (distance < minDistance)	return true;
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
}

A3DVECTOR3 A3DAutoBriskFish::SteerForSeparation (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
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


A3DVECTOR3 A3DAutoBriskFish::SteerForAlignment (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
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


A3DVECTOR3 A3DAutoBriskFish::SteerForCohesion (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
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


void A3DAutoBriskFish::ApplySteeringForce (float fElapsedTime)
{
	float rate = 0.21;
	A3DVECTOR3 newVelocity = m_vVelocity+m_vAcceleration*fElapsedTime;

	// smooth velocity, damping factor
	blendIntoAccumulator(rate, newVelocity, m_vVelocity);

	// add turbulence
	if(rand()%30 == 0)
	{
		if(GetSpeed()<0.1*GetMaxSpeed())
			SetSpeed(0.8*GetMaxSpeed());
		SetBreakingRate(frandom01()*0.001);
	}

	m_vVelocity += 
		A3DVECTOR3((frandom01()-0.5f)*0.04, (frandom01()-0.5f)*0.04, (frandom01()-0.5f)*0.04)
		*m_fSpeedMax;


	m_vVelocity = clip(m_vVelocity, m_fSpeedMin, m_fSpeedMax);
	m_vPosition += m_vVelocity*fElapsedTime;
}

void A3DAutoBriskFish::RegenerateLocalSpace (A3DVECTOR3 vNewForward, float fBankAngle)
{
	if(vNewForward.Normalize() <= 0.0001)
		return;

	float cos_theta = vNewForward.y; // DotProduct(Y, vNewForward);
	float theta;	// thrusting angle, the angle between vector local_z and Y

	// clamp thrusting angle theta

	float maxTheta = PI*0.02;
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
	SetBankAngle(fBankAngle);

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

//////////////////////////////////////////////////////////////////////////

void A3DAutoDoltishFish::Reset ()
{
	// Reset LocalSpace state
	ResetLocalSpace();
	m_vAccelerationWander.Set(0, 0, 0);
	SetSpeed(0);

	SetRadius(0.2);
		
	SetMaxAcceleration(5);
	SetAcceleration(0, 0, 0);
	SetMaxSpeed(4);
	SetMinSpeed(0);
	SetBreakingRate(0);
	m_fBankAngle = 0;
	m_fBankAngleMax = 0.02*PI;
	m_bDoingAction	= FALSE;
}

void A3DAutoDoltishFish::Update (DWORD dwDeltaTime, ActionList_e action /*= ACTION_FIRST*/)
{
	

	ApplySteeringForce(dwDeltaTime*0.001f);
	ApplyBreakingForce(m_fBreakingRate);

	if(!m_bDoingAction)
	{
		if( m_bUseECModel)
		{
			if( action == ACTION_FIRST)
			{
				m_pECModel->PlayActionByName("游动", 1.0f);
			}
		}
		else
		{
			if( action == ACTION_FIRST)
			{
				m_pSkinModel->PlayActionByName("游动", 1.0f);
			}
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

	blendIntoAccumulator(0.10, newbank, bank);

	RegenerateLocalSpace(vel, bank);
	A3DVECTOR3 up = GetUp();
	
	float animrate = (GetSpeed()+0.5)*2;

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

A3DVECTOR3 A3DAutoDoltishFish::RespondToBoundary (const A3DVECTOR3& vCenter, float fBoundaryRadius)
{
	A3DVECTOR3 dis = vCenter - GetPosition();
	float len;
	if ( (len = dis.Normalize()) < fBoundaryRadius)
		return A3DVECTOR3(0, 0, 0);
	
	float delta = len - fBoundaryRadius;
	float cushion = fBoundaryRadius*0.4;
	if(delta>cushion)	delta = cushion;
	A3DVECTOR3 seek = SteerForSeek(vCenter);	
	A3DVECTOR3 res = dis*delta*2 + perpendicularComponent(seek, GetForward());
	return clip(res, 0, GetMaxAcceleration());

/*	// if inside the sphere do nothing
	A3DVECTOR3 dis = vCenter - GetPosition();
	float len;
	if ( (len = dis.Normalize()) < fBoundaryRadius)
		return A3DVECTOR3(0, 0, 0);
	
	float delta = len - fBoundaryRadius;
//	if(fBoundaryRadius>1)
//		delta /= fBoundaryRadius;
//	if(delta>1)	delta = 1;
//	
	return dis*delta*GetMaxAcceleration();
*/
}

void A3DAutoDoltishFish::ClampPosition (A3DTerrain2 * pTerrain, A3DTerrainWater * pTerrainWater, DWORD dwDeltaTime)
{
	A3DVECTOR3 newpos = PredictPosition(4*dwDeltaTime);		
	if(pTerrain)		ClampAboveTerrain(newpos, pTerrain);
	if(pTerrainWater)	ClampUnderTerrainWater(newpos, pTerrainWater);
}

void A3DAutoDoltishFish::ClampAboveTerrain (A3DVECTOR3 pos, A3DTerrain2* pTerrain)
{
	A3DVECTOR3 vel = GetVelocity();
	float speed = vel.Magnitude();
	A3DVECTOR3 acc = GetAcceleration();
	float height = pTerrain->GetPosHeight(pos);
	float delta = height + 1 - pos.y;
	if(delta>0)
	{
		delta = clip(delta, 0.5, 0.9);
		A3DVECTOR3 newvel = A3DVECTOR3(-0.2*vel.x, -0.3*vel.y, -0.2*vel.z);
		if(vel.y>0.1*GetMaxSpeed())
			newvel.y = vel.y;
		
		blendIntoAccumulator(delta, newvel, vel);
		vel.Normalize();
		vel *= speed;
		SetVelocity(vel);
	}
}

void A3DAutoDoltishFish::ClampUnderTerrainWater (A3DVECTOR3 pos, A3DTerrainWater* pTerrainWater)
{
	A3DVECTOR3 vel = GetVelocity();
	float speed = vel.Magnitude();
	A3DVECTOR3 acc = GetAcceleration();
	float delta = speed*0.1;

	if(!pTerrainWater->IsUnderWater(pos))
	{
		delta = clip(delta, 0.3, 0.7);
		A3DVECTOR3 newvel = A3DVECTOR3(vel.x, -0.2*GetMaxSpeed(), vel.z);
		if(vel.y<-0.2*GetMaxSpeed())
			newvel.y = vel.y;

		float dis = pos.y - pTerrainWater->GetWaterHeight(pos);
		if(dis>GetRadius())
		{
			newvel.x = -newvel.x;
			newvel.z = -newvel.z;
			delta = 0.8;
		}

		blendIntoAccumulator(delta, newvel, vel);
		vel.Normalize();
		vel *= speed;
		SetVelocity(vel);
	}
}

A3DVECTOR3 A3DAutoDoltishFish::SteerForWander (float dt)
{
    float speed = 5*dt;
	
	m_vAccelerationWander.x = scalarRandomWalk(m_vAccelerationWander.x, speed, -m_fAccelerationMax, m_fAccelerationMax);
	m_vAccelerationWander.y = scalarRandomWalk(m_vAccelerationWander.y, speed, -m_fAccelerationMax, m_fAccelerationMax);
	m_vAccelerationWander.z = scalarRandomWalk(m_vAccelerationWander.z, speed, -m_fAccelerationMax, m_fAccelerationMax);

    return clip(m_vAccelerationWander, 0, GetMaxAcceleration());
}

A3DVECTOR3 A3DAutoDoltishFish::SteerForSeek (const A3DVECTOR3& target)
{
    A3DVECTOR3 desiredVelocity = target - GetPosition();
    A3DVECTOR3 seek = desiredVelocity - GetVelocity();
	return clip(seek, 0, GetMaxAcceleration());
}

A3DVECTOR3 A3DAutoDoltishFish::SteerForFlee (const A3DVECTOR3& target)
{
    A3DVECTOR3 desiredVelocity = GetPosition() - target;
    A3DVECTOR3 flee = desiredVelocity - GetVelocity();
	return clip(flee/GetRadius(), 0, GetMaxAcceleration());	
}

bool A3DAutoDoltishFish::IsInBoidNeighborhood (const A3DAutonomousAgent* other, float minDistance, float maxDistance, float cosMaxAngle)
{
    if (other == this)	return false;
    else
    {
		A3DVECTOR3 offset = other->GetPosition() - GetPosition();
		float distance = offset.Magnitude();

        // definitely in neighborhood if inside minDistance sphere
        if (distance < minDistance)	return true;
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
}

A3DVECTOR3 A3DAutoDoltishFish::SteerForSeparation (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
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


A3DVECTOR3 A3DAutoDoltishFish::SteerForAlignment (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
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


A3DVECTOR3 A3DAutoDoltishFish::SteerForCohesion (float maxDistance, float cosMaxAngle, const A3DAutonomousGroup* pGroup)
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


void A3DAutoDoltishFish::ApplySteeringForce (float fElapsedTime)
{
	float rate = 0.15;
	A3DVECTOR3 newVelocity = m_vVelocity+m_vAcceleration*fElapsedTime;

	// smooth velocity, damping factor
	blendIntoAccumulator(rate, newVelocity, m_vVelocity);

	// thrust suddenly
	if(rand()%20 == 0)
	{
		if(GetSpeed()<0.05*GetMaxSpeed())
			SetSpeed(0.9*(frandom01()*0.5+0.5)*GetMaxSpeed());
		SetBreakingRate(frandom01()*0.03);
	}

	m_vVelocity = clip(m_vVelocity, m_fSpeedMin, m_fSpeedMax);
	m_vPosition += m_vVelocity*fElapsedTime;
}

void A3DAutoDoltishFish::RegenerateLocalSpace (A3DVECTOR3 vNewForward, float fBankAngle)
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
	SetBankAngle(fBankAngle);

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