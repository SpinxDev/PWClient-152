/*
 * FILE: TaskClient.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/8/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once

#include "TaskInterface.h"

// 接受服务器通知
void OnServerNotify(TaskInterface* pTask, const void* pBuf, size_t sz);

