#ifndef _TASKCLIENT_H_
#define _TASKCLIENT_H_

#include "TaskInterface.h"

#ifdef _TASK_CLIENT

// ��ʱ��������б�״̬
void OnTaskCheckStatus(TaskInterface* pTask);

// ���ܷ�����֪ͨ
void OnServerNotify(TaskInterface* pTask, const void* pBuf, size_t sz);

#endif

#endif
