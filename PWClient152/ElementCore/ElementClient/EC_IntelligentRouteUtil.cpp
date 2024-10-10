// Filename	: EC_IntelligentRouteUtil.cpp
// Creator	: Xu Wenbin
// Date		: 2013/11/21

#include "EC_IntelligentRouteUtil.h"
#include "EC_CDR.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"

extern CECGame * g_pGame;

CECHostBrushTest::CECHostBrushTest(const A3DVECTOR3 &vPos, const A3DVECTOR3 &vExt, float fStepHeight)
: m_vHostPos(vPos), m_vHostExt(vExt)
, m_fStepHeight(fStepHeight)
, m_bHostPosInSolid(false)
, m_profile(_AL("brush test in search"))
{
	ScatteredElapsedTimeWrapper _dummy(m_profile);
	m_bHostPosInSolid = !IsCollideFree(m_vHostPos);
}

bool CECHostBrushTest::Collide(const A3DVECTOR3& from, const A3DVECTOR3& to)const
{
	//	from �����ڴ˴����ԣ���Ϊ����������ҵ�ǰλ�� m_vHostPos �ܷ�����ײ��������λ�� to
	
	ScatteredElapsedTimeWrapper _dummy(m_profile);

	bool bCollide(true);
	while (true)
	{
		if (m_bHostPosInSolid){
			break;
		}
		A3DVECTOR3 vCollideFreeTo(0.0f);
		if (!CalcCollideFreePos(to, vCollideFreeTo)){
			break;
		}
		{
			//	ֱ�����߼��
			env_trace_t  trcInfo;
			trcInfo.dwCheckFlag = CDR_BRUSH;
			trcInfo.vExt = m_vHostExt;
			trcInfo.vStart = A3DVECTOR3(m_vHostPos.x, m_vHostPos.y + m_vHostExt.y, m_vHostPos.z);
			trcInfo.vDelta = vCollideFreeTo - m_vHostPos;
			if (!CollideWithEnv(&trcInfo)){
				bCollide = false;
				break;
			}
		}
		//	������¥����֮�ർ����ײ���ʧ�ܣ��������ᵽ��ͬ�߶��ټ��һ��
		{
			env_trace_t  trcInfo;
			trcInfo.dwCheckFlag = CDR_BRUSH;
			trcInfo.vExt = m_vHostExt;
			trcInfo.vDelta = vCollideFreeTo - m_vHostPos;
			trcInfo.vDelta.y = 0.0f;
			if (m_vHostPos.y > 0.01f + vCollideFreeTo.y){
				//	����վ��¥�����ϣ�������
				trcInfo.vStart = A3DVECTOR3(m_vHostPos.x, m_vHostPos.y + m_vHostExt.y, m_vHostPos.z);//	�����Ƶ�Ŀ��λ�ò����䵽Ŀ�ĵ�
			}else if (vCollideFreeTo.y > 0.01f + m_vHostPos.y && vCollideFreeTo.y <= m_vHostPos.y + m_fStepHeight){
				//	����վ��¥�����£������ߣ��п������
				trcInfo.vStart = A3DVECTOR3(m_vHostPos.x, vCollideFreeTo.y + m_vHostExt.y, m_vHostPos.z);
			}else{
				//	����������ʺ�
				break;
			}
			if (!CollideWithEnv(&trcInfo)){
				bCollide = false;
				break;
			}
		}
		break;
	}
	return bCollide;
}

float CECHostBrushTest::GetTerrainHeight(const A3DVECTOR3 &v)const
{
	return g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(v);
}

bool CECHostBrushTest::IsCollideFree(const A3DVECTOR3 &v)const
{
	bool bAvailable(false);
	
	while (true)
	{
		float terrianHeight = GetTerrainHeight(v);
		if (terrianHeight > v.y + 1E-4f )
			break;
		
		env_trace_t  trcInfo;
		trcInfo.dwCheckFlag = CDR_BRUSH;
		trcInfo.vExt = m_vHostExt;
		trcInfo.vStart = A3DVECTOR3(v.x, v.y + m_vHostExt.y, v.z);
		trcInfo.vDelta.Clear();
		if (CollideWithEnv(&trcInfo))
			break;
		
		bAvailable = true;
		break;
	}
	
	return bAvailable;
}

bool  CECHostBrushTest::CalcCollideFreePos(const A3DVECTOR3 &v, A3DVECTOR3 &ret)const
{
	//	��ȡ�����λ�õ� y �߶ȣ����� v.y �д洢��������εĸ߶Ȳ�
	float terrianHeight = GetTerrainHeight(v);
	ret.x = v.x;
	ret.y = terrianHeight + v.y;
	ret.z = v.z;
	int count = 4;
	bool bOK(false);
	do
	{
		if (IsCollideFree(ret)){
			bOK = true;
			break;
		}
		ret.y += 0.1f;
	} while (--count > 0);
	return bOK;
}
