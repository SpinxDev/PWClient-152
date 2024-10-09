#ifndef _TASKCLIENT_H_
#define _TASKCLIENT_H_

#include "TaskInterface.h"

#ifdef _TASK_CLIENT

// 定时检查任务列表状态
void OnTaskCheckStatus(TaskInterface* pTask);

// 接受服务器通知
void OnServerNotify(TaskInterface* pTask, const void* pBuf, size_t sz);

#endif

#endif
