#ifndef _TASKSERVER_H_
#define _TASKSERVER_H_

#include "TaskInterface.h"

#ifndef _TASK_CLIENT

class elementdataman;

// ���������ļ�
bool LoadTasksFromPack(elementdataman* pDataMan, const char* szPackPath, const char* szDynPackPath);

// �ͷ������ļ�
void ReleaseTaskTemplMan();

// ֪ͨ�ӳ�����
void OnTeamSetup(TaskInterface* pTask);

// ֪ͨ�ӳ���ɢ
void OnTeamDisband(TaskInterface* pTask);

// ֪ͨ�¶ӳ��ӳ�ת��
void OnTeamTransferCaptain(TaskInterface* pTask);

// ֪ͨ�ӳ������Ա
void OnTeamAddMember(TaskInterface* pTask, const task_team_member_info* pInfo);

// ֪ͨ�ӳ���Ա�˳�
void OnTeamDismissMember(TaskInterface* pTask, unsigned long ulPlayerId);

// ֪ͨ��Ա�˳�
void OnTeamMemberLeave(TaskInterface* pTask);

// �����������֪ͨ
void OnPlayerNotify(TaskInterface* pTask, unsigned long ulPlayerId, const void* pBuf, size_t sz);

// ���տͻ���֪ͨ
void OnClientNotify(TaskInterface* pTask, const void* pBuf, size_t sz);

// ��������ܷ񷢷�
bool OnTaskCheckDeliver(TaskInterface* pTask, unsigned long ulTaskId,unsigned long ulStorageId,bool bNotifyErr=true);

// NPC����������Ʒ
void OnNPCDeliverTaskItem(TaskInterface* pTask, unsigned long ulNPCId, unsigned long ulTaskId);

// �����NPC����ɵ�����
void OnTaskCheckAward(TaskInterface* pTask, unsigned long ulTasklId, int nChoice);

// ��ұ�ɱ
void OnTaskPlayerKilled(TaskInterface* pTask);

// ��ʱ�����������ϵͳ��ʱ���н��������������״̬
void OnTaskCheckAllTimeLimits(unsigned long ulCurTime);

// ����ɱ��
void OnTaskKillMonster(TaskInterface* pTask, unsigned long ulTemplId, unsigned long ulLev, float fRand,int dps,int dph);

// ���ɱ��
void OnTaskTeamKillMonster(TaskInterface* pTask, unsigned long ulTemplId, unsigned long ulLev, float fRand);

// �������
void OnTaskMining(TaskInterface* pTask, unsigned long ulTaskId);

// ��������ȫ������
void TaskQueryGlobalData(
	unsigned long ulTaskId,
	unsigned long ulPlayerId,
	const void* pPreservedData,
	size_t size);

// ��������ȫ������
void TaskUpdateGlobalData(
	unsigned long ulTaskId,
	const unsigned char pData[TASK_GLOBAL_DATA_SIZE]);

// ���ȫ������
void OnTaskReceivedGlobalData(
	TaskInterface* pTask,
	unsigned long ulTaskId,
	unsigned char pData[TASK_GLOBAL_DATA_SIZE],
	const void* pPreservedData,
	size_t size);

// ���������
void OnForgetLivingSkill(TaskInterface* pTask);

// �������
void OnTaskPlayerLevelUp(TaskInterface* pTask);

// PQ�����ʼ��
bool InitPublicQuestSystem();

// PQ�����heartbeat
void OnTaskPublicQuestHeartbeat();

// PQ����ɱ�ֹ��׶ȷ���
void OnTaskKillPQMonster(TaskInterface* pTask, unsigned long ulTemplId, bool bOwner,float fTeamContrib,int iTeamMemberCnt,float fPersonalContib);

// PQ�����ʼ��link��Ϣ
void OnTaskCheckPQEnterWorldInit(TaskInterface* pTask, unsigned long ulTaskId);

// �뿪��������ʧ��
void OnTaskLeaveForce(TaskInterface* pTask);

enum{
	EX_TK_SENDAUMAIL_LEVEL1 = 1,
	EX_TK_SENDAUMAIL_LEVEL2 = 2,
	EX_TK_SENDAUMAIL_LEVEL3 = 3,
	EX_TK_SENDAUMAIL_EXAWARD = 4,
};
// 
void OnTaskExternEvent(TaskInterface* pTask, int Event);

// ĳЩ���������񷢷ţ������˺Ų���
void OnTaskSpecialAwardMask(TaskInterface* pTask,unsigned long ulTaskID);

// ɱ������
void OnTaskKillPlayer(TaskInterface* pTask, int iOccupation, bool bGender, int iLevel, int iForce, float fRand);

// �ƺ�����
void OnTaskTitle(TaskInterface* pTask, unsigned long ulTaskID);

// ��ͨ��NPC��������
void OnTaskManualTrig(TaskInterface* pTask,unsigned long ulTaskId, bool bNotifyErr=true);

#endif

#endif
