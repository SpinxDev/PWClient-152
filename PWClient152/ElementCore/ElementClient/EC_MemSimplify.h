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
		// ���ڸ���Ұ������ȼ��������ȼ��ߵ�������Ƚ���ģ�;���
		bool operator () (const PlayerQueueNode & lhs, const PlayerQueueNode & rhs) const;

		// ��ȡ��ҵ���Ҫ�ȼ�
		int GetImportantLevel(const CECElsePlayer* pPlayer) const;
	};

public:
	CECMemSimplify();

	void Tick(DWORD dwDeltaTime);

	// ��ҽ�������
	void OnPlayerEnter(CECElsePlayer* pPlayer);

	// ����뿪����
	void OnPlayerLeave(CECElsePlayer* pPlayer);

	// ������Ϸ״̬ʱ����
	void OnEndGameState();

	// ���������ͬһ�л���ָ��STATE
	void TransferUsageAll(int iNewState);

	// ��ȡ��ǰ�ڴ��С
	DWORD GetMemSize() const { return m_dwMemSize; }

	// ��ǰȫ���ڴ�ʹ�����
	int GetGlobalMemUsage() const { return m_iMemUsage; }

	// �������ǲ�����Ҫ���
	bool IsImportant(const CECPlayer* pPlayer ) const;
	bool IsMostImportant(const CECPlayer* pPlayer)const;

	// �����Զ��Ż��ڴ�
	void SetAutoOptimize(bool bAuto) { m_bAutoOptimize = bAuto; }

	// �Ƿ����Զ��Ż�
	bool IsAutoOptimize() const { return m_bAutoOptimize; }

protected:
	DWORD m_dwMemSize;		// ��ǰ�ڴ��С
	CECCounter m_cntCheckMem;	// ÿ����ʱ����һ���ڴ�״��
	PlayerQueue m_PlayerQueue[MEMUSAGE_NUM];	// �����ڴ�״̬�µ����
	int m_iMemUsage;		// ��ǰ�ڴ����ʹ��״��
	bool m_bAutoOptimize;	// �Ƿ����Զ��Ż�
	DWORD m_dwMemLow;		// ���ڴ�ֵ�����ỹԭ�򻯵�ģ��
	DWORD m_dwMemHigh;		// ���ڴ�ֵ�����ᰴ�����ȼ�����ģ��

protected:

	// ת���ڴ�״̬
	void TransferUsage(CECElsePlayer* pPlayer, int iNewUsage);

	// �ı�FACEģʽ
	void ToggleFaceMode(CECElsePlayer* pPlayer);

	// �л���MODELģʽ
	void ToggleSimpleModel(CECElsePlayer* pPlayer);

	// �л�����Ҫ�����ģ��ģʽ
	void ToggleNoModelNoImportant(CECElsePlayer* pPlayer);

	// �л��������������ģ��ģʽ
	void ToggleNoModel(CECElsePlayer* pPlayer);

	// �������ģ��״̬
	void UpdatePlayerModels();
	typedef bool (CECMemSimplify::*ImportanceFunction)(const CECPlayer* pPlayer)const;
	void LoadUnloadByImportance(int queueIndex, DWORD dwReloadingMask, ImportanceFunction isImportant);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////
