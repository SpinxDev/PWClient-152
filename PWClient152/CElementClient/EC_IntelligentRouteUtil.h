// Filename	: EC_IntelligentRouteUtil.h
// Creator	: Xu Wenbin
// Date		: 2013/11/21

#pragma once

#include "EC_Profile.h"

#include <MoveAgent.h>
#include <A3DVector.h>

class CECHostBrushTest : public AutoMove::CMoveAgent::BrushTest
{
	A3DVECTOR3	m_vHostPos, m_vHostExt;	//	玩家位置和碰撞范围
	float		m_fStepHeight;			//	楼梯附近角色跨高
	bool		m_bHostPosInSolid;		//	玩家初始位置即与周围碰撞
	mutable ScatteredElapsedTime	m_profile;	//	在这个类中消耗的时间统计

	float GetTerrainHeight(const A3DVECTOR3 &v)const;
	bool  IsCollideFree(const A3DVECTOR3 &v)const;
	bool  CalcCollideFreePos(const A3DVECTOR3 &v, A3DVECTOR3 &ret)const;

public:
	CECHostBrushTest(const A3DVECTOR3 &vPos, const A3DVECTOR3 &vExt, float fStepHeight);
	virtual bool Collide(const A3DVECTOR3& from, const A3DVECTOR3& to)const;
};