// Filename	: EC_SkillPanel.h
// Creator	: zhangyitian
// Date		: 2014/07/01

#ifndef _ELEMENTCLIENT_EC_TAOISTRANK_H_
#define _ELEMENTCLIENT_EC_TAOISTRANK_H_

#include "AAssist.h"

/* ��ʾ���漶����࣬TaoistRank��˼���޵�������Ϊ����ķ��� */
class CECTaoistRank {
public:
	enum {
		TotalRankCount		= 15,
		BaseRankCount		= 9,
		GodRankCount		= 3,
		EvilRankCount		= 3,
	};
	static const CECTaoistRank* GetBaseRankBegin();		// ��ȡ��һ����ͨ���漶��
	static const CECTaoistRank* GetBaseRankEnd();			// ��ȡ���һ�����漶�����һ��������NULL
	static const CECTaoistRank* GetLastBaseRank();			// ��ȡ���һ����ͨ���漶��
	static const CECTaoistRank* GetGodRankBegin();			// ��ȡ��һ���ɼ���
	static const CECTaoistRank* GetGodRankEnd();			// ��ȡ���һ���ɼ������һ��������NULL
	static const CECTaoistRank* GetLastGodRank();			// ��ȡ���һ���ɼ���
	static const CECTaoistRank* GetEvilRankBegin();			// ��ȡ��һ��ħ����
	static const CECTaoistRank* GetEvilRankEnd();			// ��ȡ���һ��ħ�������һ��������NULL
	static const CECTaoistRank* GetLastEvilRank();			// ��ȡ���һ��ħ����
	static const CECTaoistRank* GetTaoistRank(int id);		// ����ID�õ����漶��

	static int GetTotalTaoistRankCount();		// �������漶�������
	static int GetGodTaoistRankCount();			// ���������漶�������
	static int GetEvilTaoistRankCount();		// ����ħ���漶�������
	static int GetBaseTaoistRankCount();		// ������ͨ���漶�������

	const CECTaoistRank* GetNext() const;		// ��ȡ��һ������
	int GetID() const;							// ��ȡ���漶���ID
	ACString GetName() const;					// ��ȡ���漶�������

	bool IsGodRank() const;			//�Ƿ�������
	bool IsEvilRank() const;		// �Ƿ�ħ����
	bool IsBaseRank() const;		// �Ƿ���ͨ����

private:
	CECTaoistRank();
	CECTaoistRank(const CECTaoistRank&);
	CECTaoistRank& operator = (const CECTaoistRank&);
	static void init();
private:
	int m_id;			// ���漶���ID
	ACString m_name;	// ���漶�������
	CECTaoistRank* m_next;	//��һ�����漶��
	static CECTaoistRank s_allTaoistRanks[TotalRankCount];
};

#endif