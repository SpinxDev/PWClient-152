/*
 * FILE: NPCGenMan.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _NPCGENMAN_H_
#define _NPCGENMAN_H_

#include "vector.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define AIGENFILE_VERSION		11

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

#pragma pack(1)

//	Data file header. AIGENFILE_VERSION < 6
struct NPCGENFILEHEADER
{
	int		iNumAIGen;		//	Number of NPC generator
	int		iNumResArea;	//	Number of resource area
};

//	Data file header. AIGENFILE_VERSION >= 6 && < 7
struct NPCGENFILEHEADER6
{
	int		iNumAIGen;		//	Number of NPC generator
	int		iNumResArea;	//	Number of resource area
	int		iNumDynObj;		//	Number of dynamic objects
};

//	Data file header. AIGENFILE_VERSION >= 7
struct NPCGENFILEHEADER7
{
	int		iNumAIGen;		//	Number of NPC generator
	int		iNumResArea;	//	Number of resource area
	int		iNumDynObj;		//	Number of dynamic objects
	int		iNumNPCCtrl;	//	Number of NPC controller
};

//	Data file header. AIGENFILE_VERSION < 7
struct NPCGENFILEAREA
{
	int		iType;			//	0, on terrain; 1, box area
	int		iNumGen;		//	Number of NPC generator in this area
	float	vPos[3];
	float	vDir[3];
	float	vExts[3];
	int		iNPCType;		//	NPC type
	int		iGroupType;		//	Monster group type

    bool	bInitGen;		//	怪物是否起始时生成
	bool	bAutoRevive;	//	区域怪物是否自动重生
	bool	bValidOnce;		//	区域是否同时生效一次
	size_t	dwGenID;		//	区域制定编号
};

//	Data file header. AIGENFILE_VERSION >= 7
struct NPCGENFILEAREA7
{
	int		iType;			//	0, on terrain; 1, box area
	int		iNumGen;		//	Number of NPC generator in this area
	float	vPos[3];
	float	vDir[3];
	float	vExts[3];
	int		iNPCType;		//	NPC type
	int		iGroupType;		//	Monster group type

    bool	bInitGen;		//	怪物是否起始时生成
	bool	bAutoRevive;	//	区域怪物是否自动重生
	bool	bValidOnce;		//	区域是否同时生效一次
	size_t	dwGenID;		//	区域制定编号

	int		idCtrl;			//	Controller id
	int		iLifeTime;		//	Monster's life time
	int		iMaxNum;		//	Maximum monster number
};

struct NPCGENFILEAIGEN10
{
	size_t	dwID;			//	NPC ID
	size_t	dwNum;			//	NPC number
	int		iRefresh;		//	Refresh time
	size_t	dwDiedTimes;	//	Time of death before new property generatored
	size_t	dwAggressive;	//	Aggressive monster
	float	fOffsetWater;	//	Offset to water face
	float	fOffsetTrn;		//	Offset to terrain surface
	
	size_t	dwFaction;		//	怪物自己的类别
	size_t	dwFacHelper;	//	怪物寻求帮助的类别
	size_t	dwFacAccept;	//	怪物响应帮助的类别
	bool	bNeedHelp;		//	是否要求救
	bool	bDefFaction;	//	用怪物默认类别
	bool	bDefFacHelper;	//	用怪物默认的帮助者
	bool	bDefFacAccept;	//	怪物响应默认的求助者

	int		iPathID;		//	路径ID, 怪物可能沿路径移动
	int		iLoopType;		//	循环方式 0 表示不循环，1表示：原路返回（循环），2表示到了终点又到开始点如此循环
	int		iSpeedFlag;		//	1, run; 0, walk
	int		iDeadTime;		//	Disappear time after died (s)
};

struct NPCGENFILEAIGEN
{
	size_t	dwID;			//	NPC ID
	size_t	dwNum;			//	NPC number
	int		iRefresh;		//	Refresh time up
	size_t	dwDiedTimes;	//	Time of death before new property generatored
	size_t	dwAggressive;	//	Aggressive monster
	float	fOffsetWater;	//	Offset to water face
	float	fOffsetTrn;		//	Offset to terrain surface
	
	size_t	dwFaction;		//	怪物自己的类别
	size_t	dwFacHelper;	//	怪物寻求帮助的类别
	size_t	dwFacAccept;	//	怪物响应帮助的类别
	bool	bNeedHelp;		//	是否要求救
	bool	bDefFaction;	//	用怪物默认类别
	bool	bDefFacHelper;	//	用怪物默认的帮助者
	bool	bDefFacAccept;	//	怪物响应默认的求助者

	int		iPathID;		//	路径ID, 怪物可能沿路径移动
	int		iLoopType;		//	循环方式 0 表示不循环，1表示：原路返回（循环），2表示到了终点又到开始点如此循环
	int		iSpeedFlag;		//	1, run; 0, walk
	int		iDeadTime;		//	Disappear time after died (s)

	int		iRefreshLower;	//  Refresh time lower
};

//	Resources area. AIGENFILE_VERSION < 6
struct NPCGENFILERESAREA
{
	float	vPos[3];
	float	fExtX;
	float	fExtZ;
	int		iNumRes;		//	Resource number

    bool	bInitGen;		//	怪物是否起始时生成
	bool	bAutoRevive;	//	区域怪物是否自动重生
	bool	bValidOnce;		//	区域是否同时生效一次
	size_t	dwGenID;		//	区域制定编号
};

//	Resources area. AIGENFILE_VERSION >= 6
struct NPCGENFILERESAREA6
{
	float	vPos[3];
	float	fExtX;
	float	fExtZ;
	int		iNumRes;		//	Resource number

    bool	bInitGen;		//	怪物是否起始时生成
	bool	bAutoRevive;	//	区域怪物是否自动重生
	bool	bValidOnce;		//	区域是否同时生效一次
	size_t	dwGenID;		//	区域制定编号

	unsigned char dir[2];
	unsigned char rad;
};

//	Resources area. AIGENFILE_VERSION >= 7
struct NPCGENFILERESAREA7
{
	float	vPos[3];
	float	fExtX;
	float	fExtZ;
	int		iNumRes;		//	Resource number

    bool	bInitGen;		//	怪物是否起始时生成
	bool	bAutoRevive;	//	区域怪物是否自动重生
	bool	bValidOnce;		//	区域是否同时生效一次
	size_t	dwGenID;		//	区域制定编号
	
	unsigned char dir[2];
	unsigned char rad;

	int		idCtrl;			//	Controller id
	int		iMaxNum;		//	Maximum resource number
};

struct NPCGENFILERES
{
	int		iResType;		//	Resource type
	int		idTemplate;		//	ID in template database
	size_t	dwRefreshTime;	//	Refresh time
	size_t	dwNumber;		//	Resource number
	float	fHeiOff;		//	Height offset
};

//	Dynamic object
struct NPCGENFILEDYNOBJ
{
	size_t	dwDynObjID;		//	Dynamic object ID
	float	vPos[3];
	unsigned char dir[2];
	unsigned char rad;
};

//	Dynamic object
struct NPCGENFILEDYNOBJ9
{
	size_t	dwDynObjID;		//	Dynamic object ID
	float	vPos[3];
	unsigned char dir[2];
	unsigned char rad;

	unsigned int idController;	//	Controller ID
};

//	Dynamic object
struct NPCGENFILEDYNOBJ10
{
	size_t	dwDynObjID;		//	Dynamic object ID
	float	vPos[3];
	unsigned char dir[2];
	unsigned char rad;

	unsigned int idController;	//	Controller ID
	
	unsigned char scale;
};

//	Controller time
struct NPCCTRLTIME
{
	int iYear;		//	正常区间 [1900-2100] -1 代表任意
	int iMouth;		//	正常取值区间[0,11]  -1 代表任意
	int iWeek;		//	正常取值区间[0,6]  0 代表星期日 -1 代表任意
	int iDay;		//	正常取值区间[1,31]  -1 代表任意
	int iHours;		//	正常取值区间[0,23]  -1 代表任意
	int iMinutes;	//	正常取值区间[0,59]
};

//	NPC controller object
struct NPCGENFILECTRL
{
	size_t	id;
	int		iControllerID;		// 	控制器的ID，这个ID由策划天数，和以前布置区的ID一致，用于对控制器进行一些操作
	char	szName[128];
	bool	bActived;			//	是否初始时就激活
	int		iWaitTime;			//	激活后等待多少时间才开始生成对象
	int		iStopTime;			//	激活后等待多少时间就自动停止激活 int  ０代表此项无效
	
	bool	bActiveTimeInvalid;
	bool	bStopTimeInvalid;

	NPCCTRLTIME ActiveTime;		//	是否存在定时激活的时间,以及时间值　 特殊结构 
	NPCCTRLTIME StopTime;		//	是否存在定时停止的时间,以及时间值   特殊结构
};

//	NPC controller object. AIGENFILE_VERSION >= 8
struct NPCGENFILECTRL8
{
	size_t	id;
	int		iControllerID;		// 	控制器的ID，这个ID由策划天数，和以前布置区的ID一致，用于对控制器进行一些操作
	char	szName[128];
	bool	bActived;			//	是否初始时就激活
	int		iWaitTime;			//	激活后等待多少时间才开始生成对象
	int		iStopTime;			//	激活后等待多少时间就自动停止激活 int  ０代表此项无效
	
	bool	bActiveTimeInvalid;
	bool	bStopTimeInvalid;

	NPCCTRLTIME ActiveTime;		//	是否存在定时激活的时间,以及时间值　 特殊结构 
	NPCCTRLTIME StopTime;		//	是否存在定时停止的时间,以及时间值   特殊结构

	int		iActiveTimeRange;	//	激活的时间范围值,单位是秒
};

#pragma pack()

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CNPCGenMan
//	
///////////////////////////////////////////////////////////////////////////

class CNPCGenMan
{
public:		//	Types

	//	Area type
	enum
	{
		AREA_ONTERRAIN = 0,
		AREA_BOX,
	};

	//	NPC type
	enum
	{
		NPC_MONSTER = 0,
		NPC_SERVER,
	};
	
	struct AREA
	{
		int		iType;			//	0, on terrain; 1, box area
		int		iNumGen;		//	Number of NPC generator in this area
		float	vPos[3];
		float	vDir[3];
		float	vExts[3];
		int		iFirstGen;		//	First generator's index
		int		iNPCType;		//	NPC type
		int		iGroupType;		//	Monster group type
		bool	bAutoRevive;	//	区域怪物是否自动重生
		int		idCtrl;			//	Controller id
		int		iLifeTime;		//	Resource's life time
		int		iMaxNum;		//	Maximum resource number
	};

	struct NPCGEN
	{
		int		iArea;			//	Index of area this NPC generator belongs to
		size_t	dwID;			//	NPC ID
		size_t	dwNum;			//	NPC number
		int		iRefresh;		//	Refresh time
		size_t	dwDiedTimes;	//	Time of death before new property generatored
		size_t  dwAggressive;	//	Aggressive monster
		float	fOffsetWater;	//	Offset to water face
		float	fOffsetTrn;		//	Offset to terrain surface
		size_t	dwFaction;		//	怪物自己的类别
		size_t	dwFacHelper;	//	怪物寻求帮助的类别
		size_t	dwFacAccept;	//	怪物响应帮助的类别
		bool	bNeedHelp;		//	是否要求救
		bool	bDefFaction;	//	用怪物默认类别
		bool	bDefFacHelper;	//	用怪物默认的帮助者
		bool	bDefFacAccept;	//	怪物响应默认的求助者
		int		iPathID;		//	路径ID, 怪物可能沿路径移动
		int		iLoopType;		//	循环方式 0 表示不循环，1表示：原路返回（循环），2表示到了终点又到开始点如此循环
		int		iSpeedFlag;		//	1, run; 0, walk
		int		iDeadTime;		//	Disappear time after died (s)

		int		iRefreshLower;  //	Refresh time lower
	};

	//	Resources area
	struct RESAREA
	{
		float	vPos[3];
		float	fExtX;
		float	fExtZ;
		int		iFirstRes;		//	First resource index
		int		iResNum;		//	Resources number
		bool	bAutoRevive;	//	区域怪物是否自动重生
		int		idCtrl;			//	Controller id
		int		iMaxNum;		//	Maximum resource number

		unsigned char dir[2];
		unsigned char rad;
	};

	struct RES
	{
		int		iResType;		//	Resource type
		int		idTemplate;		//	ID in template database
		size_t	dwRefreshTime;	//	Refresh time
		size_t	dwNumber;		//	Resource number
		float	fHeiOff;		//	Height offset
	};
	
	//	Dynamic object
	struct DYNOBJ
	{
		size_t	dwDynObjID;		//	Dynamic object ID
		float	vPos[3];
		unsigned char dir[2];
		unsigned char rad;
		unsigned int idCtrl;	//	Controller ID
		unsigned char scale;
	};

	//	Controller object data
	struct CONTROLLER
	{
		size_t	id;
		int		iControllerID;		// 	控制器的ID，这个ID由策划天数，和以前布置区的ID一致，用于对控制器进行一些操作
		char	szName[128];
		bool	bActived;			//	是否初始时就激活
		int		iWaitTime;			//	激活后等待多少时间才开始生成对象
		int		iStopTime;			//	激活后等待多少时间就自动停止激活 int  ０代表此项无效
		
		bool	bActiveTimeInvalid;
		bool	bStopTimeInvalid;
		int		iActiveTimeRange;	//	激活的时间范围值,单位是秒

		NPCCTRLTIME ActiveTime;		//	是否存在定时激活的时间,以及时间值　 特殊结构 
		NPCCTRLTIME StopTime;		//	是否存在定时停止的时间,以及时间值   特殊结构
	};
	
public:		//	Constructor and Destructor

	CNPCGenMan();
	virtual ~CNPCGenMan();

public:		//	Attributes

public:		//	Operations

	//	Load data from file
	bool Load(const char* szFileName);
	bool Save(const char* szFileName);

	//	Get generate area number
	int GetGenAreaNum() { return m_aAreas.size(); }
	//	Get generate area by index
	const AREA& GetGenArea(int n) { return m_aAreas[n]; }
	//	Get generator number
	int GetGeneratorNum() { return m_aNPCGens.size(); }
	//	Get generator by index
	const NPCGEN& GetGenerator(int n) { return m_aNPCGens[n]; }

	//	Get resource area number
	int GetResAreaNum() { return m_aResAreas.size(); }
	//	Get resource area by index
	const RESAREA& GetResArea(int n) { return m_aResAreas[n]; }
	//	Get resource number
	int GetResNum() { return m_aRes.size(); }
	//	Get resource by index
	const RES& GetRes(int n) { return m_aRes[n]; }
	
	//	Get dynamic object number
	int GetDynObjectNum() { return m_aDynObjs.size(); }
	//	Get dynamic object
	const DYNOBJ& GetDynObject(int n) { return m_aDynObjs[n]; }

	//	Get npc controller number
	int GetNPCCtrlNum() { return m_aControllers.size(); }
	//	Get npc controller
	const CONTROLLER& GetController(int n) { return m_aControllers[n]; }

	//  Get controller ptr for modify
	CONTROLLER* GetControllerPtr(int n){ return (CONTROLLER*)&m_aControllers[n];}

protected:	//	Attributes

	abase::vector<AREA>			m_aAreas;		//	Generate area data
	abase::vector<NPCGEN>		m_aNPCGens;		//	NPC generator data
	abase::vector<RESAREA>		m_aResAreas;	//	Resource areas
	abase::vector<RES>			m_aRes;			//	Resources
	abase::vector<DYNOBJ>		m_aDynObjs;		//	Dynamic objects
	abase::vector<CONTROLLER>	m_aControllers;	//	Controllers

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_NPCGENMAN_H_
