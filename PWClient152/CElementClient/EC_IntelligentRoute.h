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
		enumSearchSuccess,			//	搜索成功
		enumSearchStartEndCoincide,	//	搜索起点和终点在同一搜索位置
		enumSearchUnInitialized,	//	搜索数据未初始化
		enumSearchStartInvalid,		//	起点非法
		enumSearchEndInvalid,		//	终点非法
		enumSearchStartEndInvalid,	//	起点终点均非法
		enumSearchNoPath,			//	未搜到路径
		enumSearchExceedMaxExpand,	//	搜索展开结点过多
	};
	enum RouteState
	{
		enumRouteIdle,				//	未进行寻路操作
		enumRouteMoving,			//	路径跟随中
		enumRoutePathFinished,		//	所有路径节点均已完成
	};
	enum Usage
	{
		enumUsageNone,				//	无用途
		enumUsageWorkMove,			//	CECHPWorkMove 中使用
		enumUsageWorkTrace,			//	CECHPWorkTrace 中使用
	};

	~CECIntelligentRoute(){ Release(); }
	static CECIntelligentRoute & Instance();

	Usage GetUsage()const{ return m_usage; }
	void SetUsage(Usage iUsage){ m_usage = iUsage; }
	bool IsUsageMove()const{ return GetUsage() == enumUsageWorkMove; }
	bool IsUsageTrace()const{ return GetUsage() == enumUsageWorkTrace; }

	//	数据加载
	void ChangeWorldInstance(int idInstance);
	void Release();

	//	状态查询
	RouteState GetState() const { return m_state; }
	const A3DVECTOR3 & GetDest()const{ return m_end; }
	
	//	路径搜索
	SearchResult Search(const A3DVECTOR3 & start, const A3DVECTOR3 & end, CMoveAgent::BrushTest *pBrushTest=NULL, int nMaxExpand=-1);
	void ResetSearch();

	//	路径跟随
	bool IsIdle()const{ return GetState() == enumRouteIdle; }
	bool IsMoveOn()const{ return GetState() == enumRouteMoving; }
	bool IsPathFinished()const{ return GetState() == enumRoutePathFinished; }
	A3DVECTOR3 GetCurDest();
	void OnPlayerPosChange(const A3DVECTOR3 & pos);

	//	路径显示
	int	 GetLeftNodeCount();
	A3DVECTOR3 GetLeftNodePosXZ(int iOffset);

private:
	struct RangedMoveAgent			//	当前地图的单个寻路块
	{
		ARectI		rect;			//	能提供的搜索范围（所在全图左下角为(0,0)，右上角为(CECInstance::GetColNum(), CECInstance::GetRowNum())）
		A3DVECTOR3	origin;			//	搜索图左下角对应世界中的三维位置（用于世界坐标和搜索图坐标之间变换）
		CMoveAgent*	agent;			//	搜索服务对象（内含搜索辅助内容、搜索结果和路径跟随结果）
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

	CECInstance *	m_pInst;		//	寻路数据对应的当前地图
	MoveAgentArray	m_moveAgents;	//	该地图所有的寻路块

	RouteState		m_state;		//	寻路当前状态
	A3DVECTOR3		m_start, m_end;	//	本次寻路的起点、终点
	int				m_iCurMoveAgent;//	上次调用 Search 时确认的寻路块
	int				m_iCurDest;		//	当前前往的路径节点下标
	A3DVECTOR3		m_lastPos;		//	上次移动到的位置，当有新位置时，用于检测是否应移往下一步
	float			m_lastMove;		//	上次移动距离，用于估计下一步将移动的距离，从而判断是否应移往下一步
	float			m_dist2CurDest;	//	到达 m_iCurDest 还需移动的距离，用于判断是否应移往下一步

	Usage			m_usage;		//	当前寻路的用途
};