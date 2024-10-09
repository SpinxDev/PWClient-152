/********************************************************************
  created:	   4/12/2012  
  filename:	   EC_CrossServerList.cpp
  author:      zhougaomin01305  
  description: 跨服ServerList接口.
  Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
*********************************************************************/

#include "EC_CrossServerList.h"

///////////////////////////////////////////////////////////////////////////
//  
//  Implement class CECCrossServerList
//  
///////////////////////////////////////////////////////////////////////////

// 获取实例
CECCrossServerList& CECCrossServerList::GetSingleton()
{
	static CECCrossServerList csl;
	return csl;
}

CECCrossServerList::CECCrossServerList()
{
}

CECCrossServerList::~CECCrossServerList()
{
}

void CECCrossServerList::Clear()
{
	m_servers.clear();
}

void CECCrossServerList::PutOneServer(const ServerInfo& osi)
{
	m_servers.push_back(osi);
}

CECCrossServerList::ServerInfo* CECCrossServerList::Find(const int zoneID, const int defaultLine)
{
	ServerInfo *pInfo = NULL;

	for (ServerList::iterator it = m_servers.begin(); it != m_servers.end(); ++ it)
	{
		ServerInfo &cur = *it;
		if (cur.zoneid != zoneID) continue;
		pInfo = &cur;
		if (defaultLine == cur.line) break;
	}

	return pInfo;
}