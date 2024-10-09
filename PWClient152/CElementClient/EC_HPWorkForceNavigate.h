/*
 * FILE: EC_HPWorkForceNavigate.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: 
 *
 * HISTORY: 
 *
 * Copyright (c) 2014
 */

#ifndef _ECHPWORKFORCENAVIGATE_H_
#define _ECHPWORKFORCENAVIGATE_H_

#include "EC_HPWork.h"
#include "EC_Player.h"

#include <AString.h>
#include <vector.h>

class CECBezierWalker;


class CECNavigateCtrl
{
public:
	enum
	{
		EM_PREPARE =0,
		EM_BEGIN,
		EM_END,
	};

	struct INFO
	{
		int taskID;
		int bezierID;
		float speed;
		float angleWithH;
		bool bezierDir;  // false: fix, true: bezier
		AString strModelPath;
	};
public:
	CECNavigateCtrl(CECHostPlayer* pHost):m_pHost(pHost),m_pBezierWalker(NULL),m_bForceNavigateState(false),m_taskID(0){}
	~CECNavigateCtrl();

public:
	bool LoadConfig(const char* szFile);

	void OnPrepareNavigate(int task);
	void OnBeginNavigate();
	void OnEndNavigate();

	bool GetNavigateInfo(int task,CECNavigateCtrl::INFO& info);
	bool IsInForceNavigateState() const { return m_bForceNavigateState;}

	CECBezierWalker* GetBezierWalker() { return m_pBezierWalker;}
	INFO	GetCurrentNavigateInfo() { return m_curNavigateInfo;}

protected:
	abase::vector<INFO>			m_configInfo;			// 数据不多，vector即可
	CECHostPlayer*				m_pHost;

	CECBezierWalker*			m_pBezierWalker;		// 曲线
	bool						m_bForceNavigateState;	// 强制移动状态

	INFO						m_curNavigateInfo;		// 当前强制移动的信息
	int							m_taskID;				// 对应的任务ID
};
//////////////////////////////////////////////////////////////////////////

class CECHPWorkNavigate : public CECHPWork
{
public:		//	Types
	
public:		//	Constructor and Destructor
	
	CECHPWorkNavigate(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkNavigate();
	
public:		//	Attributes
	
public:		//	Operations
	
	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);
	// begin to force move
	void BeginNavigate(); 	
	
protected:	//	Attributes
	
	bool m_bMove;		// move flag
	float m_fSpeed;		// move speed
	
protected:	//	Operations
	
	//	On first tick
	virtual void OnFirstTick();
	//	Stop skill move
	void Finish();
};

#endif


///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



