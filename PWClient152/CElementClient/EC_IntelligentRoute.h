// Filename	: EC_IntelligentRoute.h
// Creator	: Xu Wenbin
// Date		: 2013/08/15

#pragma once

#include <ARect.h>
#include <vector.h>
#include <A3DVector.h>
#include <MoveAgent.h>

class CECInstance;

class CECIntelligentRoute
{
public:
	typedef AutoMove::CMoveAgent	CMoveAgent;
	typedef CMoveAgent::BrushTest	BrushTest;

	enum SearchResult
	{
		enumSearchSuccess,			//	�����ɹ�
		enumSearchStartEndCoincide,	//	���������յ���ͬһ����λ��
		enumSearchUnInitialized,	//	��������δ��ʼ��
		enumSearchStartInvalid,		//	���Ƿ�
		enumSearchEndInvalid,		//	�յ�Ƿ�
		enumSearchStartEndInvalid,	//	����յ���Ƿ�
		enumSearchNoPath,			//	δ�ѵ�·��
		enumSearchExceedMaxExpand,	//	����չ��������
	};
	enum RouteState
	{
		enumRouteIdle,				//	δ����Ѱ·����
		enumRouteMoving,			//	·��������
		enumRoutePathFinished,		//	����·���ڵ�������
	};
	enum Usage
	{
		enumUsageNone,				//	����;
		enumUsageWorkMove,			//	CECHPWorkMove ��ʹ��
		enumUsageWorkTrace,			//	CECHPWorkTrace ��ʹ��
	};

	~CECIntelligentRoute(){ Release(); }
	static CECIntelligentRoute & Instance();

	Usage GetUsage()const{ return m_usage; }
	void SetUsage(Usage iUsage){ m_usage = iUsage; }
	bool IsUsageMove()const{ return GetUsage() == enumUsageWorkMove; }
	bool IsUsageTrace()const{ return GetUsage() == enumUsageWorkTrace; }

	//	���ݼ���
	void ChangeWorldInstance(int idInstance);
	void Release();

	//	״̬��ѯ
	RouteState GetState() const { return m_state; }
	const A3DVECTOR3 & GetDest()const{ return m_end; }
	
	//	·������
	SearchResult Search(const A3DVECTOR3 & start, const A3DVECTOR3 & end, CMoveAgent::BrushTest *pBrushTest=NULL, int nMaxExpand=-1);
	void ResetSearch();

	//	·������
	bool IsIdle()const{ return GetState() == enumRouteIdle; }
	bool IsMoveOn()const{ return GetState() == enumRouteMoving; }
	bool IsPathFinished()const{ return GetState() == enumRoutePathFinished; }
	A3DVECTOR3 GetCurDest();
	void OnPlayerPosChange(const A3DVECTOR3 & pos);

	//	·����ʾ
	int	 GetLeftNodeCount();
	A3DVECTOR3 GetLeftNodePosXZ(int iOffset);

private:
	struct RangedMoveAgent			//	��ǰ��ͼ�ĵ���Ѱ·��
	{
		ARectI		rect;			//	���ṩ��������Χ������ȫͼ���½�Ϊ(0,0)�����Ͻ�Ϊ(CECInstance::GetColNum(), CECInstance::GetRowNum())��
		A3DVECTOR3	origin;			//	����ͼ���½Ƕ�Ӧ�����е���άλ�ã������������������ͼ����֮��任��
		CMoveAgent*	agent;			//	������������ں������������ݡ����������·����������
		RangedMoveAgent(): rect(0, 0, 0, 0), origin(0.0f), agent(NULL)
		{}
		bool Contain(const A3DVECTOR3 &pos)const;
	};
	typedef abase::vector<RangedMoveAgent>	MoveAgentArray;

	//	Forbidden
	CECIntelligentRoute(const CECIntelligentRoute &);
	CECIntelligentRoute & operator = (const CECIntelligentRoute &);

	CECIntelligentRoute();
	RangedMoveAgent * GetCurAgent();
	A3DVECTOR3	GetNodePosXZ(int iNode);
	A3DVECTOR3	GetNodePosNoCheck(int iNode);
	A3DVECTOR3	GetNodePos(int iNode);
	int			FindNearestNode(const A3DVECTOR3 &curPos, int iNodeFrom);
	int			FindFarthestNode(const A3DVECTOR3 &curPos, int iNodeFrom);
	int			FindNextNode(const A3DVECTOR3 &curPos, int iNodeFrom);
	bool		CanFinishPath(const A3DVECTOR3 & pos);
	bool		CanMoveToNext(const A3DVECTOR3 & pos);

	CECInstance *	m_pInst;		//	Ѱ·���ݶ�Ӧ�ĵ�ǰ��ͼ
	MoveAgentArray	m_moveAgents;	//	�õ�ͼ���е�Ѱ·��

	RouteState		m_state;		//	Ѱ·��ǰ״̬
	A3DVECTOR3		m_start, m_end;	//	����Ѱ·����㡢�յ�
	int				m_iCurMoveAgent;//	�ϴε��� Search ʱȷ�ϵ�Ѱ·��
	int				m_iCurDest;		//	��ǰǰ����·���ڵ��±�
	A3DVECTOR3		m_lastPos;		//	�ϴ��ƶ�����λ�ã�������λ��ʱ�����ڼ���Ƿ�Ӧ������һ��
	float			m_lastMove;		//	�ϴ��ƶ����룬���ڹ�����һ�����ƶ��ľ��룬�Ӷ��ж��Ƿ�Ӧ������һ��
	float			m_dist2CurDest;	//	���� m_iCurDest �����ƶ��ľ��룬�����ж��Ƿ�Ӧ������һ��

	Usage			m_usage;		//	��ǰѰ·����;
};