/*
 * FILE: A3DAutonomousAgent.cpp
 *
 * DESCRIPTION: The implementation A3DAutonomousAgent class
 *
 * CREATED BY: Changzhi Li, 2004/11/02
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.
 */

#include "Utilities.h"
#include "A3DAutonomousAgent.h"

class A3DAutonomousGroup;


A3DAutonomousAgent::A3DAutonomousAgent ()
{
	m_pSkinModel = new A3DSkinModel(); 
	m_pECModel = new CECModel();
	
	m_bUseECModel = false;
	m_CurAction = ACTION_NONE;
	
	Reset(); 
}

A3DAutonomousAgent::~A3DAutonomousAgent()
{
	Release();
}

//A3DAutonomousAgent::A3DAutonomousAgent (A3DSkinModel * pSkinmodel) 
//{
//	m_pSkinModel = pSkinmodel; 
//	Reset(); 
//}

bool A3DAutonomousAgent::Init (A3DEngine *pEngine)
{
	Reset();
	A3DVECTOR3 f = randomUnitVector();
	RegenerateLocalSpace(f, 0);

	m_pA3DEngine = pEngine;
	return m_pSkinModel->Init(pEngine);
}

void A3DAutonomousAgent::Release ()
{
	A3DRELEASE(m_pSkinModel);
	A3DRELEASE(m_pECModel);
}

void A3DAutonomousAgent::SetLightInfo (const A3DSkinModel::LIGHTINFO& Info)
{
	if( m_bUseECModel)
	{
		m_pECModel->GetA3DSkinModel()->SetLightInfo(Info);
	}
	else
	{
		m_pSkinModel->SetLightInfo(Info);
	}
}

bool A3DAutonomousAgent::Load (const char* szFile)
{
	const char* ext = strrchr(szFile, '.');
	if( ext == NULL) return false;

	ext++;
	if( stricmp(ext, "smd") == 0)
	{
		
		if(m_pSkinModel->Load(szFile))
		{
			m_bUseECModel = false;
			return true;
		}
		else
		{
			return false;
		}
	}
	else if( stricmp(ext, "ecm") == 0)
	{
		
#ifdef _ELEMENTCLIENT		
		if( m_pECModel->Load(szFile))		//¿Í»§¶Ë
#else
		if(m_pECModel->Load(szFile))
#endif
		{
			m_bUseECModel = true;
			return true;
		}
		else
		{
			return false;
		}

	}
	else
	{
		return false;
	}
}

#ifdef _ELEMENT_EDITOR_
bool A3DAutonomousAgent::Render (A3DViewport* pViewport, bool bCheckVis)
{
	if( m_bUseECModel)
	{	
		return m_pECModel->Render(pViewport);	
	}
	else
	{
		return m_pSkinModel->Render(pViewport, bCheckVis);
	}
}
#endif

#ifdef _AUTOCA_TEST_
bool A3DAutonomousAgent::Render (A3DViewport* pViewport, bool bCheckVis)
{
	if( m_bUseECModel)
	{	
		return m_pECModel->Render(pViewport);	
	}
	else
	{
		return m_pSkinModel->Render(pViewport, bCheckVis);
	}
}
#endif

#ifdef _ELEMENTCLIENT
bool A3DAutonomousAgent::Render(CECViewport* pViewport, bool bCheckVis /*= TRUE*/)
{
	if( m_bUseECModel)
	{	
		return m_pECModel->Render(pViewport->GetA3DViewport());	
	}
	else
	{
		return m_pSkinModel->Render(pViewport->GetA3DViewport(), bCheckVis);
	}
}
#endif

void A3DAutonomousAgent::ResetLocalSpace (void)
{
	SetForward(0, 0, 1);
	SetSide(1, 0, 0);
	SetUp(0, 1, 0);
	SetPosition(0, 0, 0);
	SetBankAngle(0);
	SetMaxBankAngle(0.25*PI);
}

//void A3DAutonomousAgent::InitRandomPosAndVel()
//{
//	
//}


void A3DAutonomousAgent::RegenerateLocalSpace (A3DVECTOR3 vNewForward, float fBankAngle)
{
/*			
			Y global vertical axis
			|		vNewForward                 local side (x-axis)
			|    /                             /
			|   /                             /
			|  /                             /
  theta-----|-/                             / --bank
			|/							   /------------- global X-Z plane


 */

	// clamp the cos_theta eg. |cos_theta| <= 1/sqrt(2) <=> theta <- [PI/4, 3*PI/4] ;
	// no possiblity of vertically thrusting to the sky or the ground
	
	if(vNewForward.Normalize() <= 0.0001)
		return;

	float cos_theta = vNewForward.y; // DotProduct(Y, vNewForward);
	float theta;	// thrusting angle, the angle between vector local_z and Y

	float maxTheta = PI*0.25;
	float cos_maxTheta = 1.0f/sqrt(2.0f);
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

void A3DAutonomousAgent::SetAcceleration (const A3DVECTOR3& a)
{ 
	m_vAcceleration = a; 
	m_vAcceleration = clip(m_vAcceleration, 0, m_fAccelerationMax); 
}

void A3DAutonomousAgent::SetAcceleration (float x, float y, float z) 
{ 
	m_vAcceleration.Set(x,y,z);
	m_vAcceleration = clip(m_vAcceleration, 0, m_fAccelerationMax); 
}

void A3DAutonomousAgent::ApplyBreakingForce (float fBreakingRate)
{	
	fBreakingRate = clip(fBreakingRate, 0, 1);
	blendIntoAccumulator(fBreakingRate, A3DVECTOR3(0, 0, 0), m_vVelocity);
}

A3DVECTOR3 A3DAutonomousAgent::PredictPosition (DWORD dwDeltaTime) const
{
	float fElapsedTime = dwDeltaTime*0.001f;
	A3DVECTOR3 vVelocity = m_vVelocity;
	A3DVECTOR3 vPosition = m_vPosition;
	float rate = 0.21;
	A3DVECTOR3 newVelocity = vVelocity+m_vAcceleration*fElapsedTime;

	// smooth velocity, damping factor
	blendIntoAccumulator(rate, newVelocity, vVelocity);

	vVelocity = clip(vVelocity, m_fSpeedMin, m_fSpeedMax);
	vPosition += vVelocity*fElapsedTime;
	return vPosition;
}