/********************************************************************
	created:	2012/8/30
	author:		dongjie
	
	purpose:    ����detours��ͳ�ƿͻ��˵ĵ�ǰ�ڴ����ʷ�ڴ��¼�����
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