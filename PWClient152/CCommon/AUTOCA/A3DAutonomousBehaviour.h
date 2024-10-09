/*
 * FILE: A3DAutonomousBehaviour.h
 *
 * DESCRIPTION: The header and interface of Autonomous Behaviour in the engine
 *
 * CREATED BY: Changzhi Li, 2004/11/02
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DAUTONOMOUSBEHAVIOUR_H_
#define _A3DAUTONOMOUSBEHAVIOUR_H_

// forward decleration
class A3DTerrain2;
class A3DTerrainWater;
class A3DAutonomousGroup;

class A3DAutonomousBehaviour
{
public:
	A3DAutonomousBehaviour (void) {}

	// initialize position, velocity of all members
	virtual void Initialize (A3DAutonomousGroup* pGroup) {}

	// not affect members position, velocity
	virtual void Reset (A3DAutonomousGroup* pGroup) {}

	virtual void Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,A3DAutonomousGroup* pGroup, DWORD dwDeltaTime) {}
	void Release (void) {}
};

class A3DAutoBehaviourWander : public A3DAutonomousBehaviour
{
public:
	A3DAutoBehaviourWander () { }
	void Initialize (A3DAutonomousGroup * pGroup);
	void Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,A3DAutonomousGroup * pGroup, DWORD dwDeltaTime);
	void Reset (A3DAutonomousGroup* pGroup);
};

class A3DAutoBehaviourHover : public A3DAutonomousBehaviour
{
public:
	A3DAutoBehaviourHover () : m_vTarget(0, 0, 0) { }
	void Initialize (A3DAutonomousGroup * pGroup);
	void Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,A3DAutonomousGroup * pGroup, DWORD dwDeltaTime);
	void Reset (A3DAutonomousGroup * pGroup);

	void SetTargetPosition (A3DVECTOR3 target) { m_vTarget = target; }
	A3DVECTOR3 GetTargetPosition (void) { return m_vTarget; }
	
protected:
	A3DVECTOR3 m_vTarget;
};

class A3DAutoBehaviourBoid : public A3DAutonomousBehaviour
{
public:
	A3DAutoBehaviourBoid () { }
	void Initialize (A3DAutonomousGroup * pGroup);
	void Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,A3DAutonomousGroup * pGroup, DWORD dwDeltaTime);
	void Reset (A3DAutonomousGroup * pGroup);
};

class A3DAutoBehaviourArrival : public A3DAutonomousBehaviour
{
public:
	A3DAutoBehaviourArrival () {}
	void Initialize (A3DAutonomousGroup * pGroup);
	void Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,A3DAutonomousGroup * pGroup, DWORD dwDeltaTime);
	void Reset (A3DAutonomousGroup * pGroup);

	void SetTargetPosition (const A3DVECTOR3& vTarget) { m_vTarget = vTarget; }
	A3DVECTOR3 GetTargetPosition (void) { return m_vTarget; }

protected:
	A3DVECTOR3	m_vTarget;
};

class A3DAutoBehaviourFlee : public A3DAutonomousBehaviour
{
public:
	A3DAutoBehaviourFlee () {}
	void Initialize (A3DAutonomousGroup * pGroup);
	void Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,A3DAutonomousGroup * pGroup, DWORD dwDeltaTime);
	void Reset (A3DAutonomousGroup * pGroup);

	void SetTargetPosition (const A3DVECTOR3& vTarget) { m_vTarget = vTarget; }
	A3DVECTOR3 GetTargetPosition (void) { return m_vTarget; }

protected:
	A3DVECTOR3	m_vTarget;
};

enum FleeAndBackState_e
{
	BEGIN_STATE,
	FLEE_STATE,
	BACK_STATE,
};

class A3DAutoBehaviourFleeAndBack : public A3DAutonomousBehaviour
{
public:
	A3DAutoBehaviourFleeAndBack ();

	void Initialize (A3DAutonomousGroup * pGroup);
	void Update (A3DTerrain2 *pTerrain,A3DTerrainWater *pTerrainWater,A3DAutonomousGroup * pGroup, DWORD dwDeltaTime);
	void Reset (A3DAutonomousGroup * pGroup){};

	void SetTargetPosition (const A3DVECTOR3& vTarget) { m_vTarget = vTarget; }
	A3DVECTOR3 GetTargetPosition (void) { return m_vTarget; }

protected:
	void BeginProcess(A3DTerrain2 *pTerrain, A3DAutonomousGroup * pGroup, DWORD dwDeltaTime);
	void FleeProcess(A3DTerrain2 *pTerrain, A3DTerrainWater *pTerrainWater, A3DAutonomousGroup * pGroup, DWORD dwDeltaTime);
	void BackProcess(A3DTerrain2 *pTerrain, A3DTerrainWater *pTerrainWater, A3DAutonomousGroup * pGroup, DWORD dwDeltaTime);

	A3DVECTOR3	m_vTarget;
	FleeAndBackState_e m_State;
	bool m_bStartFlee;

};

#endif