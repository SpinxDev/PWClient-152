// Filename	: EC_IntelligentRoute.cpp
// Creator	: Xu Wenbin
// Date		: 2013/08/15

#include "EC_IntelligentRoute.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Global.h"
#include "EC_Instance.h"
#include "EC_RTDebug.h"
#include "EC_World.h"
#include "EC_Profile.h"

#include <AFI.h>
#include <A3DMacros.h>

#include <cfloat>

//	CECIntelligentRoute::RangedMoveAgent
bool CECIntelligentRoute::RangedMoveAgent::Contain(const A3DVECTOR3 &pos)const
{
	return agent && agent->IsContain(pos);
}

//	CECIntelligentRoute
CECIntelligentRoute & CECIntelligentRoute::Instance()
{
	static CECIntelligentRoute s_inst;
	return s_inst;
}

CECIntelligentRoute::CECIntelligentRoute()
: m_pInst(NULL)
, m_state(enumRouteIdle)
, m_start(0.0f)
, m_end(0.0f)
, m_iCurMoveAgent(-1)
, m_iCurDest(-1)
, m_lastPos(0.0f)
, m_lastMove(0.0f)
, m_dist2CurDest(0.0f)
, m_usage(enumUsageNone)
{
}

void CECIntelligentRoute::ChangeWorldInstance(int idInstance)
{
	CECInstance *pInst = g_pGame->GetGameRun()->GetInstance(idInstance);
	if (!pInst){
		Release();
		return;
	}
	if (pInst == m_pInst){
		return;
	}
	Release();
	m_pInst = pInst;
	const CECInstance::AStringArray &files = pInst->GetRouteFiles();
	if (files.empty()){
		return;
	}
	//	��顢ת���ļ�����ΪѰ·��Χ
	MoveAgentArray moveAgents;
	CECInstance::AStringArray validFiles;
	int rowFrom(-1), rowTo(-1), colFrom(-1), colTo(-1);
	int i(0);
	for (i = 0; i < (int)files.size(); ++ i)
	{
		const AString & strFileName = files[i];
		if (4 != sscanf(strFileName, "r%d_%d-c%d_%d", &rowFrom, &rowTo, &colFrom, &colTo) ||
			rowFrom < 0 || rowFrom >= pInst->GetRowNum() ||
			rowTo   < 0 || rowTo   >= pInst->GetRowNum() ||
			colFrom < 0 || colFrom >= pInst->GetColNum() ||
			colTo   < 0 || colTo   >= pInst->GetColNum() ||
			rowFrom > rowTo || colFrom > colTo){
			a_LogOutput(1, "CECIntelligentRoute::ChangeWorldInstance(%d), Invalid file name %s", idInstance, strFileName);
			continue;
		}
		validFiles.push_back(strFileName);

		ARectI tmpRect;
		tmpRect.left = colFrom * 1024;
		tmpRect.right = (colTo+1) * 1024;
		tmpRect.top = (pInst->GetRowNum() - rowTo - 1) * 1024;			//	row Դ�Գ����༭������Z��������Z�Ḻ��������� rect Ҫ�� bottom >= top
		tmpRect.bottom = (pInst->GetRowNum() - rowFrom) * 1024;

		A3DVECTOR3 tmpOrigin(0.0f);
		tmpOrigin.x = tmpRect.left - pInst->GetColNum()*1024*0.5f;
		tmpOrigin.z = tmpRect.top - pInst->GetRowNum()*1024*0.5f;

		RangedMoveAgent tmpRMA;
		tmpRMA.rect = tmpRect;
		tmpRMA.origin = tmpOrigin;
		moveAgents.push_back(tmpRMA);
	}
	//	�����飺���Է�Χ��ͬ���������������Χ���ļ����������ַ�Χ�غ�
	i = (int)moveAgents.size() - 1;
	while (i >= 0)
	{
		const ARectI &ri = moveAgents[i].rect;
		for (int j = 0; j < (int)moveAgents.size(); ++ j)
		{
			if (j != i){
				const ARectI &rj = moveAgents[j].rect;
				if ((ri & rj) == ri){
					a_LogOutput(1, "CECIntelligentRoute::ChangeWorldInstance(%d), %s is duplicated with or contained by %s", idInstance, validFiles[i], validFiles[j]);
					moveAgents.erase(moveAgents.begin() + i);
					validFiles.erase(validFiles.begin() + i);
					break;
				}
			}
		}
		-- i;	//	���������飬�Դﵽ����ʹ�������ֵ��ļ���Ŀ��
	}
	//	���Լ��غϷ��ļ�
	if (!moveAgents.empty()){
		AString strFilePath;
		int nLoaded(0);
		A3DVECTOR3 vOrigin(0.0f);
		for (i = 0; i < (int)moveAgents.size(); ++ i)
		{
			strFilePath.Format("maps\\%s\\movemap\\%s", pInst->GetPath(), validFiles[i]);
			if (!af_IsFileExist(strFilePath)){
				a_LogOutput(1, "CECIntelligentRoute::ChangeWorldInstance(%d), File %s NOT EXIST", idInstance, strFilePath);
				continue;
			}
			RangedMoveAgent &rma = moveAgents[i];
			rma.agent = new CMoveAgent;
			if (!rma.agent->Load(strFilePath, &rma.origin)){
				a_LogOutput(1, "CECIntelligentRoute::ChangeWorldInstance(%d), Load file %s FAILED", idInstance, strFilePath);
				delete rma.agent;
				continue;
			}
			if (!rma.agent->IsReady()){
				a_LogOutput(1, "CECIntelligentRoute::ChangeWorldInstance(%d), agent for file %s NOT READY for Routing", idInstance, strFilePath);
				delete rma.agent;
				continue;
			}
			++ nLoaded;
		}
		if (nLoaded > 0){
			m_moveAgents.reserve(nLoaded);
			for (i = 0; i < (int)moveAgents.size(); ++ i)
			{
				RangedMoveAgent &rma = moveAgents[i];
				if (rma.agent != NULL){
					m_moveAgents.push_back(rma);
				}
			}
		}
	}
}

void CECIntelligentRoute::Release()
{
	m_pInst = NULL;
	m_state = enumRouteIdle;
	m_start.Clear();
	m_end.Clear();
	m_iCurMoveAgent = -1;
	m_iCurDest = -1;
	for (MoveAgentArray::iterator it = m_moveAgents.begin(); it != m_moveAgents.end(); ++ it)
	{
		delete it->agent;
	}
	m_moveAgents.clear();
	m_lastPos.Clear();
	m_dist2CurDest = 0.0f;
	m_lastMove = 0.0f;
	m_usage = enumUsageNone;
}

void CECIntelligentRoute::ResetSearch()
{
	if (IsIdle()){
		//	��ǰ��������״̬
		return;
	}
	RangedMoveAgent *pCurAgent = GetCurAgent();
	if (pCurAgent){
		pCurAgent->agent->ResetSearch();
	}
	m_state = enumRouteIdle;	
	m_start.Clear();
	m_end.Clear();
	m_iCurMoveAgent = -1;
	m_iCurDest = -1;
	m_lastPos.Clear();
	m_lastMove = 0.0f;
	m_dist2CurDest = 0.0f;
}

#ifdef SHOW_AUTOMOVE_FOOTPRINTS
abase::vector<A3DVECTOR3> g_AutoPFFollowPoints;
abase::vector<A3DVECTOR3> g_AutoPFPathPoints;
#endif

CECIntelligentRoute::SearchResult CECIntelligentRoute::Search(const A3DVECTOR3 & start, const A3DVECTOR3 & end, CMoveAgent::BrushTest *pBrushTest/* =NULL */, int nMaxExpand/* =-1 */)
{
#ifdef SHOW_AUTOMOVE_FOOTPRINTS
	g_AutoPFFollowPoints.clear();
	g_AutoPFPathPoints.clear();
#endif

	//	�����������
	ResetSearch();
	if (m_moveAgents.empty()){
		//	��ǰȫͼû��Ѱ·����
		return enumSearchUnInitialized;
	}
	//	�ҳ�ͬʱ���������յ�� agent, �ж�����ο� CECIntelligentRoute::ChangeWorldInstance ������ʱֱ��ѡȡ��һ����ԭ��Ϊ�ٶ��غϵ�ͼ��Ѱ·����������
	bool bStartContained(false), bEndContained(false);
	int iMoveAgent(-1);
	int i(0);
	for (i = 0; i < (int)m_moveAgents.size(); ++ i)
	{
		RangedMoveAgent & rma = m_moveAgents[i];
		if (rma.Contain(start)){
			bStartContained = true;
			if (rma.Contain(end)){
				bEndContained = true;
				iMoveAgent = i;
				break;
			}
		}else if (rma.Contain(end)){
			bEndContained = true;
		}
	}
	if (iMoveAgent < 0){
		//	��ǰ��ͼ�鲻������Ѱ·
		if (bStartContained){
			g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("goal no routemap"));
			return enumSearchEndInvalid;
		}else if (bEndContained){
			g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("start no routemap"));
			return enumSearchStartInvalid;
		}else{
			g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("start and goal no routemap"));
			return enumSearchStartEndInvalid;
		}
	}
	//	������������
	CMoveAgent *pCurMoveAgent = m_moveAgents[iMoveAgent].agent;
	CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();
	float startDH = start.y - pWorld->GetTerrainHeight(start);
	float endDH = end.y - pWorld->GetTerrainHeight(end);
	int startLayer = pCurMoveAgent->WhichLayer(start, startDH);
	int endLayer = pCurMoveAgent->WhichLayer(end, endDH);
	if (startLayer < 0){
		startLayer = 0;		//	ָ��λ������λ�ò��ɴ�ʱ������ -1 ����ʱ������ CMoveAgent::SetStartEnd ������λ�ø����Ľڵ㣬�Ծ�������Ŀ��
	}
	if (endLayer < 0){
		endLayer = 0;
	}
	//	����
	ScopedElapsedTime _dummy(_AL("search time"));
	if (!pCurMoveAgent->SetStartEnd(start, startLayer, end, endLayer, pBrushTest)){
		int state = pCurMoveAgent->GetState();
		pCurMoveAgent->ResetSearch();
		switch (state)
		{
		case AutoMove::PF_STATE_INVALIDSTART:
			g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("start not suitable"));
			return enumSearchStartInvalid;
		case AutoMove::PF_STATE_INVALIDEND:
			g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("goal not suitable"));
			return enumSearchEndInvalid;
		}
		assert(false);
		return enumSearchStartEndInvalid;
	}
	if (!pCurMoveAgent->Search(NULL, nMaxExpand)){
		int state = pCurMoveAgent->GetState();
		pCurMoveAgent->ResetSearch();
		switch (state)
		{
		case AutoMove::PF_STATE_NOPATH:
			g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("no path"));
			return enumSearchNoPath;
		case AutoMove::PF_STATE_EXCEED:
			g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("exceed max step"));
			return enumSearchExceedMaxExpand;
		}
		assert(false);
		return enumSearchNoPath;
	}
	if (pCurMoveAgent->Get2DPath()->size() == 1){
		//	�����յ�Ϊͬһ����λ��ʱ
		pCurMoveAgent->ResetSearch();
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("start and goal coincide"));
		return enumSearchStartEndCoincide;
	}
	m_iCurMoveAgent = iMoveAgent;
	m_state = enumRouteMoving;
	m_start = start;
	m_end = end;
	m_lastPos = m_start;
	m_lastMove = 0.0f;
	m_iCurDest = FindNextNode(start, 0);
	m_dist2CurDest = (GetCurDest() - m_start).MagnitudeH();
	return enumSearchSuccess;
}

CECIntelligentRoute::RangedMoveAgent * CECIntelligentRoute::GetCurAgent()
{
	if (m_iCurMoveAgent >= 0 && m_iCurMoveAgent < (int)m_moveAgents.size()){
		return &m_moveAgents[m_iCurMoveAgent];
	}
	return NULL;
}

A3DVECTOR3 CECIntelligentRoute::GetCurDest()
{
	return GetNodePos(m_iCurDest);
}

A3DVECTOR3	CECIntelligentRoute::GetNodePos(int iNode)
{
	A3DVECTOR3 pos(0.0f);
	RangedMoveAgent *pCurAgent = GetCurAgent();
	if (pCurAgent){
		CMoveAgent * agent = pCurAgent->agent;
		int nPathCount = agent->GetPathCount();
		if (iNode >= 0 && iNode < nPathCount){
			pos = GetNodePosNoCheck(iNode);
		}
	}
	return pos;
}

A3DVECTOR3	CECIntelligentRoute::GetNodePosNoCheck(int iNode)
{
	RangedMoveAgent &curAgent = m_moveAgents[m_iCurMoveAgent];
	A3DVECTOR3 pos = curAgent.agent->Get3DPathNode(iNode);
	CECWorld *pWorld = g_pGame->GetGameRun()->GetWorld();
	pos.y += pWorld->GetTerrainHeight(pos);
	return pos;
}

A3DVECTOR3 CECIntelligentRoute::GetNodePosXZ(int iNode)
{
	if (!IsIdle()){
		CMoveAgent * agent = GetCurAgent()->agent;
		return agent->Get2DPathNode(iNode);
	}
	return A3DVECTOR3(0.0f);
}

int	 CECIntelligentRoute::GetLeftNodeCount()
{
	int nLeftNode(0);
	if (!IsIdle()){		
		CMoveAgent * agent = GetCurAgent()->agent;
		int pathCount = agent->GetPathCount();
		nLeftNode = pathCount - m_iCurDest;
	}
	return nLeftNode;
}

A3DVECTOR3 CECIntelligentRoute::GetLeftNodePosXZ(int iOffset)
{
	return GetNodePosXZ(m_iCurDest + iOffset);
}

int	CECIntelligentRoute::FindNearestNode(const A3DVECTOR3 &curPos, int iNodeFrom)
{
	//	����һ����Χ������ڵ㣬������һ���ƶ�Ŀ�� m_iCurDest
	if (!IsMoveOn()){
		assert(false);
		return -1;
	}
	CMoveAgent * agent = GetCurAgent()->agent;
	int pathCount = agent->GetPathCount();
	if (iNodeFrom < 0 || iNodeFrom >= pathCount){
		assert(false);
		return -1;
	}
	if (iNodeFrom == pathCount-1){
		return iNodeFrom;
	}
	int maxCheckIndex = iNodeFrom + agent->GetOptimizeCatchCount();
	a_ClampRoof(maxCheckIndex, pathCount-1);
	int bestIndex = -1;	
	double bestDist2 = DBL_MAX;
	for (int i = iNodeFrom; i <= maxCheckIndex; ++ i)
	{
		A3DVECTOR3 testPos = GetNodePosXZ(i);	//	ֻ���ƽ����룬��ΪԶ�����θ߶ȿ��ܼ��㲻׼ȷ
		testPos -= curPos;
		testPos.y = 0.0f;
		double dist2 = testPos.SquaredMagnitude();
		if (dist2 < bestDist2){
			bestIndex = i;
			bestDist2 = dist2;
		}
	}
	//	����·��λ��
	if (bestIndex >= 0){
		return bestIndex;
	}
	assert(false);
	return -1;
}

int	CECIntelligentRoute::FindFarthestNode(const A3DVECTOR3 &curPos, int iNodeFrom)
{
	//	�����ƶ�����ܽӽ������Ǿ���Զ�Ľڵ㣬������һ���ƶ�Ŀ��
	if (iNodeFrom < 0){
		return iNodeFrom;
	}
	CMoveAgent * agent = GetCurAgent()->agent;
	int pathCount = agent->GetPathCount();
	if (iNodeFrom == pathCount-1){
		return iNodeFrom;
	}
	A3DVECTOR3 moveDir = GetNodePosXZ(iNodeFrom);
	moveDir -= curPos;
	moveDir.y = 0;
	moveDir.Normalize();
	
	int maxCheckIndex = iNodeFrom + agent->GetOptimizeCatchCount();
	a_ClampRoof(maxCheckIndex, pathCount-1);
	for (int i = iNodeFrom+1; i <= maxCheckIndex; ++ i)
	{
		A3DVECTOR3 testDir = GetNodePosXZ(i);
		testDir -= curPos;
		testDir.y = 0;
		testDir.Normalize();
		float dtp = DotProduct(moveDir, testDir);
		static const double threshold = cos(5*A3D_PI/180.0);
		if (dtp < threshold){
			break;
		}
		iNodeFrom = i;
	}
	return iNodeFrom;
}

int	CECIntelligentRoute::FindNextNode(const A3DVECTOR3 &curPos, int iNodeFrom)
{
	int iCandidate = FindNearestNode(curPos, iNodeFrom);
	return FindFarthestNode(curPos, iCandidate);
}

void CECIntelligentRoute::OnPlayerPosChange(const A3DVECTOR3 & pos)
{
	if (!IsMoveOn()){
		return;
	}
#ifdef SHOW_AUTOMOVE_FOOTPRINTS
	g_AutoPFFollowPoints.push_back(pos);
#endif
	if (CanFinishPath(pos)){
		//	���·������
		m_state = enumRoutePathFinished;
		return;
	}
	//	�����ƶ����룬����Ƿ�����л�������·����
#ifdef SHOW_AUTOMOVE_FOOTPRINTS
	g_AutoPFPathPoints.push_back(GetCurDest());
#endif
	if (!CanMoveToNext(pos)){
		return;
	}
	CMoveAgent * agent = GetCurAgent()->agent;
	int pathCount = agent->GetPathCount();
	if (m_iCurDest == pathCount-1){
		//	�Ѿ�ǰ�����һ���ڵ㣬����·������
		m_state = enumRoutePathFinished;
		return;
	}
	//	���Ը�����һ��Ŀ����
	m_iCurDest = FindNextNode(pos, m_iCurDest+1);
	m_dist2CurDest = (GetCurDest() - pos).MagnitudeH();
	//	�Ż�·��
	if (agent->Optimize(m_iCurDest)){
		//	�Ż���agent �ڲ� COptimizePath::m_CurIndex ���ܴӱ� CECIntelligentRoute::m_iCurDest С��������
		//	�м�� m_CurIndex �� m_iCurDest ��·���ڵ㲻��
	}
}

bool CECIntelligentRoute::CanFinishPath(const A3DVECTOR3 & pos)
{
	if (IsMoveOn()){
		A3DVECTOR3 delta = m_end - pos;
		float dist = delta.Magnitude();		//	������ά���룬�Դ���߶����϶����
		return dist <= 0.5f;
	}
	return false;
}

bool CECIntelligentRoute::CanMoveToNext(const A3DVECTOR3 & pos)
{
	//	����ÿ���ƶ�λ�ú��ƶ�����
	float fMove = (pos - m_lastPos).MagnitudeH();
	m_lastPos = pos;

	float lastMove = m_lastMove;
	m_lastMove = fMove;

	//	������λ�õ�Ŀ��ľ���
	float dist2CurDest = m_dist2CurDest;	
	A3DVECTOR3 vCurDest = GetCurDest();
	m_dist2CurDest = (vCurDest - pos).MagnitudeH();

	if (fMove >= dist2CurDest){
		//	����Ŀ��λ�ã�������һ��
		return true;
	}
	if (fMove+0.1f >= dist2CurDest){
		//	��úܽ�ʱ(0.1�׷�Χ�ڣ�������������һ��
		return true;
	}
	if (lastMove * 0.5f >= m_dist2CurDest){
		//	���Ƶ�Ŀ��λ��ʱ�������ƹ��ˡ������ǰλ�ñ��´�Ԥ��������λ����Ŀ���������Ҳ����������һ��
		return true;
	}
	return false;
}