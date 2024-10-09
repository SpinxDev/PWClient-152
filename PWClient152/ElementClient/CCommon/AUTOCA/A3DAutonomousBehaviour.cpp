/*
 * FILE: A3DAutonomousBehaviour.cpp
 *
 * DESCRIPTION: The vavious implementations of IAutonomousBeheviour interface
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
#include "A3DTerrain2.h"
#include "A3DTerrainWater.h"

#ifdef _A3DAUTO_TEST_  //生物群组测试程序
#include "game.h"
extern CGame g_theGame;
#endif

#ifdef _ELEMENT_EDITOR_	//模型编辑器
#include "Render.h"
#endif

#ifdef _ELEMENTCLIENT	//客户端
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#endif
//////////////////////////////////////////////////////////////////////////

void A3DAutoBehaviourWander::Initialize (A3DAutonomousGroup* pGroup)
{
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for(unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		//设置每个Agent位置
		mbr->SetPosition(randomVectorInUnitSphere()*pGroup->GetBoundaryRadius()+pGroup->GetCenter());
		
		A3DVECTOR3 f = randomUnitVector();
		if(f.y<0) f.y=-f.y;
		mbr->RegenerateLocalSpace(f, 0);
		mbr->SetSpeed(mbr->GetMaxSpeed()*(frandom01()-1)*0.5);
	}
}


void A3DAutoBehaviourWander::Reset (A3DAutonomousGroup* pGroup)
{
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for(unsigned int i=0; i<members.size(); i++)
	{
		members[i]->SetBreakingRate(0);
	}
}

void A3DAutoBehaviourWander::Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater, A3DAutonomousGroup * pGroup, DWORD dwDeltaTime)
{
	if(pGroup == NULL) return;
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for( unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		A3DVECTOR3 accw = mbr->SteerForWander(dwDeltaTime*0.001f);
 		A3DVECTOR3 accb = mbr->RespondToBoundary(pGroup->GetCenter(), pGroup->GetBoundaryRadius());
		A3DVECTOR3 acc = accw + accb;
		clip(acc, 0, mbr->GetMaxAcceleration());

		mbr->SetAcceleration(acc);
		mbr->Update(dwDeltaTime);
		mbr->ClampPosition(pTerrain, pTerrainWater, dwDeltaTime);
	}
}

//////////////////////////////////////////////////////////////////////////

void A3DAutoBehaviourHover::Initialize (A3DAutonomousGroup* pGroup)
{
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for(unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		mbr->SetPosition(randomVectorInUnitSphere()*pGroup->GetBoundaryRadius()+pGroup->GetCenter());	
		A3DVECTOR3 f = randomUnitVector();
		if(f.y<0) f.y=-f.y;
		mbr->RegenerateLocalSpace(f, 0);
		mbr->SetSpeed(mbr->GetMaxSpeed()*(frandom01()+1)*0.005);
		mbr->SetMinSpeed(mbr->GetMaxSpeed()*0.5);
		mbr->SetBreakingRate(0);
	}
}

void A3DAutoBehaviourHover::Reset (A3DAutonomousGroup* pGroup)
{
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for(unsigned int i=0; i<members.size(); i++)
	{
 		A3DAutonomousAgent * mbr = members[i];
		mbr->SetMinSpeed(mbr->GetMaxSpeed()*0.5);
		mbr->SetBreakingRate(0);
	}
}

void A3DAutoBehaviourHover::Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,A3DAutonomousGroup * pGroup, DWORD dwDeltaTime)
{
	if(pGroup == NULL) return;
	
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for(unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		A3DVECTOR3 acc = mbr->SteerForSeek(m_vTarget)*2.0f;
		acc += mbr->SteerForWander(dwDeltaTime*0.001f)*0.4f;

		float separationRadius = mbr->GetRadius()*4;
		float separationAngle  = -0.707f;
		float separationWeight =  1;

		acc += mbr->SteerForSeparation(separationRadius, separationAngle, pGroup)*separationWeight;
		
		mbr->SetAcceleration(acc);
		mbr->Update(dwDeltaTime);
		mbr->ClampPosition(pTerrain, pTerrainWater, dwDeltaTime);
	}	
}

//////////////////////////////////////////////////////////////////////////
void A3DAutoBehaviourBoid::Initialize (A3DAutonomousGroup* pGroup)
{
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for(unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		mbr->SetPosition(randomVectorInUnitSphere()*pGroup->GetBoundaryRadius()+pGroup->GetCenter());	
		A3DVECTOR3 f = randomUnitVector();
//		if(f.y<0) f.y=-f.y;
		mbr->RegenerateLocalSpace(f, 0);
		mbr->SetSpeed(mbr->GetMaxSpeed()*(frandom01()-1)*0.5);
 		mbr->SetMinSpeed(0);
	}	
}

void A3DAutoBehaviourBoid::Reset (A3DAutonomousGroup* pGroup)
{
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for(unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		mbr->SetMinSpeed(0);		
		mbr->SetBreakingRate(0);
	}
}

void A3DAutoBehaviourBoid::Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,A3DAutonomousGroup * pGroup, DWORD dwDeltaTime)
{
	if(pGroup == NULL) return;

	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();

	for( unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		A3DVECTOR3 acc(0,0,0);
		
		float separationRadius =  mbr->GetRadius()*3;
		float separationAngle  = -0.707f;
		float separationWeight =  1.2;

		float alignmentRadius = mbr->GetRadius()*5;
		float alignmentAngle  = 0.7f;
		float alignmentWeight = 0.8;

		float cohesionRadius = mbr->GetRadius()*10;
		float cohesionAngle  = -0.15f;
		float cohesionWeight = 1;//0.8;

		float maxRadius = max(separationRadius, max(alignmentRadius, cohesionRadius));

		// determine each of the three component behaviors of flocking
		A3DVECTOR3 separation = mbr->SteerForSeparation(separationRadius, separationAngle, pGroup);
		A3DVECTOR3 alignment  = mbr->SteerForAlignment(alignmentRadius, alignmentAngle, pGroup);
		A3DVECTOR3 cohesion   = mbr->SteerForCohesion(cohesionRadius, cohesionAngle, pGroup);

		// apply weights to components
		A3DVECTOR3 separationW = separation * separationWeight;
		A3DVECTOR3 alignmentW = alignment * alignmentWeight;
		A3DVECTOR3 cohesionW = cohesion * cohesionWeight;
		
		acc += separationW + alignmentW + cohesionW;		
 		acc += mbr->RespondToBoundary(pGroup->GetCenter(), pGroup->GetBoundaryRadius())*2.0f;

		// ?? need some wandering ??
//		acc += mbr->SteerForWander(fElapsedTime);
		
		mbr->SetAcceleration(acc);
		mbr->Update(dwDeltaTime);
		mbr->ClampPosition(pTerrain, pTerrainWater, dwDeltaTime);
	}	
}

//////////////////////////////////////////////////////////////////////////

void A3DAutoBehaviourArrival::Initialize (A3DAutonomousGroup* pGroup)
{
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for(unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		mbr->SetPosition(randomVectorInUnitSphere()*pGroup->GetBoundaryRadius()+pGroup->GetCenter());	
		A3DVECTOR3 f = randomUnitVector();
		if(f.y<0) f.y=-f.y;
		mbr->RegenerateLocalSpace(f, 0);
		mbr->SetSpeed(mbr->GetMaxSpeed()*(frandom01()-1)*0.5);
		mbr->SetMinSpeed(0);
	}
}


void A3DAutoBehaviourArrival::Reset (A3DAutonomousGroup* pGroup)
{
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for(unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name
		mbr->SetSpeed(mbr->GetMaxSpeed()*(frandom01()-1)*0.5);
		mbr->SetMinSpeed(0);
		mbr->SetBreakingRate(0);
	}
}

void A3DAutoBehaviourArrival::Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,A3DAutonomousGroup * pGroup, DWORD dwDeltaTime)
{
	if(pGroup == NULL) return;

	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for( unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		A3DVECTOR3 acc(0, 0, 0);
		A3DVECTOR3 pos = mbr->GetPosition();
		A3DVECTOR3 delta = pos-GetTargetPosition();
		
		acc += mbr->SteerForSeek(GetTargetPosition());
		
		// ?? neglect boundary when trying to arrive some place ??
// 		acc += RespondToBoundary(mbr);

		float distance = delta.Magnitude();
		if(distance < mbr->GetRadius()*2)
		{
			if(distance<=mbr->GetRadius())
			{
 				mbr->SetBreakingRate(1);
				acc.Set(0,0,0);
			}
			else	// 2<rate<3
			{
				mbr->SetBreakingRate(1.0f/(distance-1.0));
			}
		}
		mbr->SetAcceleration(acc);		
		mbr->Update(dwDeltaTime);
		mbr->ClampPosition(pTerrain, pTerrainWater, dwDeltaTime);
	}
}

//////////////////////////////////////////////////////////////////////////

void A3DAutoBehaviourFlee::Initialize (A3DAutonomousGroup* pGroup)
{
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for(unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		mbr->SetPosition(randomVectorInUnitSphere()*pGroup->GetBoundaryRadius()+pGroup->GetCenter());	
		A3DVECTOR3 f = randomUnitVector();
		if(f.y<0) f.y=-f.y;
		mbr->RegenerateLocalSpace(f, 0);
		mbr->SetSpeed(mbr->GetMaxSpeed()*(frandom01()-1)*0.5);
		mbr->SetMinSpeed(0);
		mbr->SetBreakingRate(0);
	}
}


void A3DAutoBehaviourFlee::Reset (A3DAutonomousGroup* pGroup)
{
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for(unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name
		mbr->SetMinSpeed(0);
		mbr->SetBreakingRate(0);
	}
}

void A3DAutoBehaviourFlee::Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,A3DAutonomousGroup * pGroup, DWORD dwDeltaTime)
{
	if(pGroup == NULL) return;
	
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for( unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		A3DVECTOR3 acc(0, 0, 0);
		A3DVECTOR3 pos = mbr->GetPosition();
		A3DVECTOR3 delta = pos-GetTargetPosition();
		
		acc += mbr->SteerForFlee(GetTargetPosition());
 		acc += mbr->RespondToBoundary(pGroup->GetCenter(), pGroup->GetBoundaryRadius())*1.2f;
		mbr->SetAcceleration(acc);
		mbr->Update(dwDeltaTime);
		mbr->ClampPosition(pTerrain, pTerrainWater, dwDeltaTime);

	}
}

A3DAutoBehaviourFleeAndBack::A3DAutoBehaviourFleeAndBack()
{
	m_State = BEGIN_STATE;
	m_bStartFlee = false;

}

void A3DAutoBehaviourFleeAndBack::Initialize(A3DAutonomousGroup * pGroup)
{
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for(unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		//mbr->SetPosition(randomVectorInUnitSphere()*pGroup->GetBoundaryRadius()+pGroup->GetCenter());	
		A3DVECTOR3 f = randomUnitVector();
		if(f.y<0) f.y=-f.y;
		mbr->RegenerateLocalSpace(f, 0);
		mbr->SetSpeed(mbr->GetMaxSpeed()*(frandom01()-1)*3.0);
		mbr->SetMinSpeed(0);
		mbr->SetBreakingRate(0);
	}
}


#define FLEE_DIS 20		//触发逃散的距离
#define BACK_DIS 70		//触发回归的距离

void A3DAutoBehaviourFleeAndBack::Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,A3DAutonomousGroup * pGroup, DWORD dwDeltaTime)
{
	if(pGroup == NULL) return;

	//指定回归的目的地(目前编辑器和游戏里面没法指定m_vTarget,所以只能暂时如此)
	A3DVECTOR3 vCenter = pGroup->GetCenter();
	float fHeight = pTerrain->GetPosHeight(vCenter);
	m_vTarget = A3DVECTOR3(vCenter.x, fHeight, vCenter.z);

	if(m_State == BEGIN_STATE)//初始
	{
		BeginProcess(pTerrain, pGroup, dwDeltaTime);
	}

	//判断是否触发逃散
	if( m_State == BEGIN_STATE || m_State == BACK_STATE)
	{
		#ifdef _A3DAUTO_TEST_
			A3DVECTOR3 vDisFromPlayer = g_theGame.GetCamera()->GetPos() - pGroup->GetCenter();
		#endif
		
		#ifdef _ELEMENT_EDITOR_	
			A3DVECTOR3 vDisFromPlayer = g_Render.GetA3DEngine()->GetActiveCamera()->GetPos() - pGroup->GetCenter();
		#endif

		#ifdef _ELEMENTCLIENT		
			//A3DVECTOR3 vDisFromPlayer = g_pGame->GetA3DEngine()->GetActiveCamera()->GetPos() - pGroup->GetCenter();
			A3DVECTOR3 vDisFromPlayer = g_pGame->GetGameRun()->GetHostPlayer()->GetPos() - pGroup->GetCenter();
		#endif

		float fDisFromPlayer = vDisFromPlayer.Magnitude();

		if( fDisFromPlayer < FLEE_DIS) //逃散距离
		{
			m_State = FLEE_STATE;
			m_bStartFlee = true;
			
		}
	}

	if( m_State == FLEE_STATE)//逃离状态
	{
		FleeProcess(pTerrain, pTerrainWater, pGroup, dwDeltaTime);
	}	
	
	//判断是否返回
	if(m_State == FLEE_STATE )
	{
		#ifdef _A3DAUTO_TEST_
			A3DVECTOR3 vDisFromPlayer = g_theGame.GetCamera()->GetPos() - pGroup->GetCenter();
		#endif
		
		#ifdef _ELEMENT_EDITOR_	
			A3DVECTOR3 vDisFromPlayer = g_Render.GetA3DEngine()->GetActiveCamera()->GetPos() - pGroup->GetCenter();
		#endif

		#ifdef _ELEMENTCLIENT		
			//A3DVECTOR3 vDisFromPlayer = g_pGame->GetA3DEngine()->GetActiveCamera()->GetPos() - pGroup->GetCenter();
			A3DVECTOR3 vDisFromPlayer = g_pGame->GetGameRun()->GetHostPlayer()->GetPos() - pGroup->GetCenter();
		#endif

		float fDisFromPlayer = vDisFromPlayer.Magnitude();
		
		//float fGroupRadius = pGroup->GetBoundaryRadius();
		if( fDisFromPlayer > BACK_DIS)
		{
			m_State = BACK_STATE;
		}
	}

	if( m_State == BACK_STATE)//回归
	{
		BackProcess(pTerrain, pTerrainWater, pGroup, dwDeltaTime);
	}

	//判断回归是否结束
	if( m_State == BACK_STATE)
	{
		A3DAutonomousAgentVector& members = pGroup->GetAllMembers();

		bool bAllStop = true;
		for( unsigned int i=0; i<members.size(); i++)
		{
			A3DAutonomousAgent * mbr = members[i];

			if( mbr->GetSpeed() != 0.0f)
			{
				bAllStop = false;
			}
			else				//降落
			{
				A3DVECTOR3 pos = mbr->GetPosition();
				float fNewHeight = pTerrain->GetPosHeight(pos);
				A3DVECTOR3 vNewPos(pos.x, fNewHeight, pos.z);
				mbr->SetPosition(vNewPos);
			}
		}

		if( bAllStop )//都停下来了则转化为初始状态
		{
			m_State = BEGIN_STATE;
		}
	}
	
}


void A3DAutoBehaviourFleeAndBack::BeginProcess(A3DTerrain2 *pTerrain, A3DAutonomousGroup * pGroup, DWORD dwDeltaTime)
{

	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for( unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];

		A3DVECTOR3 pos = mbr->GetPosition();
		float fNewHeight = pTerrain->GetPosHeight(pos);
		A3DVECTOR3 vNewPos(pos.x, fNewHeight, pos.z);

		mbr->SetPosition(vNewPos);
		mbr->SetSpeed(0.0f);
		mbr->Update(dwDeltaTime, ACTION_SECOND);	
	}
}

void A3DAutoBehaviourFleeAndBack::FleeProcess(A3DTerrain2 *pTerrain, A3DTerrainWater *pTerrainWater, A3DAutonomousGroup * pGroup, DWORD dwDeltaTime)
{
	if( m_bStartFlee)
	{
		Initialize(pGroup);
		m_bStartFlee = false;
	}

	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for( unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		A3DVECTOR3 acc(0,0,0);
		
		float separationRadius =  mbr->GetRadius()*3;
		float separationAngle  = -0.707f;
		float separationWeight =  1.2;

		float alignmentRadius = mbr->GetRadius()*5;
		float alignmentAngle  = 0.7f;
		float alignmentWeight = 0.8;

		float cohesionRadius = mbr->GetRadius()*10;
		float cohesionAngle  = -0.15f;
		float cohesionWeight = 1;//0.8;

		float maxRadius = max(separationRadius, max(alignmentRadius, cohesionRadius));

		// determine each of the three component behaviors of flocking
		A3DVECTOR3 separation = mbr->SteerForSeparation(separationRadius, separationAngle, pGroup);
		A3DVECTOR3 alignment  = mbr->SteerForAlignment(alignmentRadius, alignmentAngle, pGroup);
		A3DVECTOR3 cohesion   = mbr->SteerForCohesion(cohesionRadius, cohesionAngle, pGroup);

		// apply weights to components
		A3DVECTOR3 separationW = separation * separationWeight;
		A3DVECTOR3 alignmentW = alignment * alignmentWeight;
		A3DVECTOR3 cohesionW = cohesion * cohesionWeight;
		
		acc += separationW + alignmentW + cohesionW;		
 		acc += mbr->RespondToBoundary(pGroup->GetCenter(), pGroup->GetBoundaryRadius())*2.0f;

		// ?? need some wandering ??
		//acc += mbr->SteerForWander(fElapsedTime);
		
		mbr->SetAcceleration(acc);
		mbr->Update(dwDeltaTime);
		mbr->ClampPosition(pTerrain, pTerrainWater, dwDeltaTime);
	}
}
void A3DAutoBehaviourFleeAndBack::BackProcess(A3DTerrain2 *pTerrain, A3DTerrainWater *pTerrainWater, A3DAutonomousGroup * pGroup, DWORD dwDeltaTime)
{
	A3DAutonomousAgentVector& members = pGroup->GetAllMembers();
	for( unsigned int i=0; i<members.size(); i++)
	{
		A3DAutonomousAgent * mbr = members[i];	// alias for short name

		A3DVECTOR3 acc(0, 0, 0);
		A3DVECTOR3 pos = mbr->GetPosition();
		A3DVECTOR3 delta = pos-GetTargetPosition();
		
		acc += mbr->SteerForSeek(GetTargetPosition());
		
		float distance = delta.Magnitude();
		if(distance < mbr->GetRadius() * 2)
		{
			if(distance<=mbr->GetRadius())
			{
 				mbr->SetBreakingRate(1);
				acc.Set(0,0,0);
			}
			else	// 2<rate<3
			{
				mbr->SetBreakingRate(1.0f/(distance-1.0));
			}
		}
		mbr->SetAcceleration(acc);		
		mbr->Update(dwDeltaTime);
		mbr->ClampPosition(pTerrain, pTerrainWater, dwDeltaTime);
	}
}