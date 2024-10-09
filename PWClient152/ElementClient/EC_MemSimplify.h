/*
 * FILE: EC_MemSimplify.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/4/3
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */

#pragma once

#include <ABaseDef.h>
#include <AArray.h>
#include <AAssist.h>
#include <vector>
#include <hashmap.h>
#include "EC_Counter.h"


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECPlayer;
class CECElsePlayer;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECMemSimplify
//	
///////////////////////////////////////////////////////////////////////////

class CECMemSimplify
{
public:

	//  Memory usage
	enum
	{
		MEMUSAGE_NORMAL,
		MEMUSAGE_NOFACE,
		MEMUSAGE_SIMPLEMODEL,
		MEMUSAGE_NOMODEL_UNIMPORTANT,
		MEMUSAGE_NOMODEL,
		
		MEMUSAGE_NUM,
	};

	typedef void (CECMemSimplify::*FN_TRANSFER_USAGE) (CECElsePlayer* pPlayer);

	struct PlayerQueueNode{
		CECElsePlayer * m_pPlayer;
		DWORD			m_dwMask;
		PlayerQueueNode()
			: m_pPlayer(NULL)
			, m_dwMask(0)
		{}
		PlayerQueueNode(CECElsePlayer *pPlayer)
			: m_pPlayer(pPlayer)
			, m_dwMask(0)
		{
		}
		bool operator == (const CECElsePlayer *rhs)const{
			return m_pPlayer == rhs;
		}
		bool operator == (const PlayerQueueNode &rhs)const{
			return m_pPlayer == rhs.m_pPlayer;
		}
		void Mask(DWORD newMask){
			m_dwMask |= newMask;
		}
		void Unmask(DWORD newMask){
			m_dwMask &= ~newMask;
		}
		bool HasMask(DWORD dwMask)const{
			return (m_dwMask & dwMask) != 0;
		}
	};
	typedef std::vector<PlayerQueueNode> PlayerQueue;
	struct PlayerSorter
	{
		// 用于给玩家按照优先级排序，优先级高的玩家最先接受模型精简
		bool operator () (const PlayerQueueNode & lhs, const PlayerQueueNode & rhs) const;

		// 获取玩家的重要等级
		int GetImportantLevel(const CECElsePlayer* pPlayer) const;
	};

public:
	CECMemSimplify();

	void Tick(DWORD dwDeltaTime);

	// 玩家进入世界
	void OnPlayerEnter(CECElsePlayer* pPlayer);

	// 玩家离开世界
	void OnPlayerLeave(CECElsePlayer* pPlayer);

	// 结束游戏状态时调用
	void OnEndGameState();

	// 将所有玩家同一切换到指定STATE
	void TransferUsageAll(int iNewState);

	// 获取当前内存大小
	DWORD GetMemSize() const { return m_dwMemSize; }

	// 当前全局内存使用情况
	int GetGlobalMemUsage() const { return m_iMemUsage; }

	// 检查玩家是不是重要玩家
	bool IsImportant(const CECPlayer* pPlayer ) const;
	bool IsMostImportant(const CECPlayer* pPlayer)const;

	// 开启自动优化内存
	void SetAutoOptimize(bool bAuto) { m_bAutoOptimize = bAuto; }

	// 是否开启自动优化
	bool IsAutoOptimize() const { return m_bAutoOptimize; }

protected:
	DWORD m_dwMemSize;		// 当前内存大小
	CECCounter m_cntCheckMem;	// 每隔段时间检测一次内存状况
	PlayerQueue m_PlayerQueue[MEMUSAGE_NUM];	// 各个内存状态下的玩家
	int m_iMemUsage;		// 当前内存大体使用状况
	bool m_bAutoOptimize;	// 是否开启自动优化
	DWORD m_dwMemLow;		// 低于此值，将会还原简化的模型
	DWORD m_dwMemHigh;		// 高于此值，将会按照优先级来简化模型

protected:

	// 转换内存状态
	void TransferUsage(CECElsePlayer* pPlayer, int iNewUsage);

	// 改变FACE模式
	void ToggleFaceMode(CECElsePlayer* pPlayer);

	// 切换简单MODEL模式
	void ToggleSimpleModel(CECElsePlayer* pPlayer);

	// 切换非重要玩家无模型模式
	void ToggleNoModelNoImportant(CECElsePlayer* pPlayer);

	// 切换所有其它玩家无模型模式
	void ToggleNoModel(CECElsePlayer* pPlayer);

	// 更新玩家模型状态
	void UpdatePlayerModels();
	typedef bool (CECMemSimplify::*ImportanceFunction)(const CECPlayer* pPlayer)const;
	void LoadUnloadByImportance(int queueIndex, DWORD dwReloadingMask, ImportanceFunction isImportant);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////
