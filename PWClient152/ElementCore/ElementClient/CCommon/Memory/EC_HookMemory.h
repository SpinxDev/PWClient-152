/********************************************************************
	created:	2012/8/30
	author:		dongjie
	
	purpose:    利用detours库统计客户端的当前内存和历史内存记录并输出
	Copyright (C) All Rights Reserved
*********************************************************************/
#pragma once

#ifdef _PROFILE_MEMORY

void g_EnableMemoryHistory();
void g_DisableMemoryHistory();
void g_GenerateAdditionFile(const bool stop_hook=false);
void g_MemoryDump();

void g_TickMemoryHistory();

#endif