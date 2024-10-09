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

void CECCrossServerList::PutServerAllPort(const ServerInfo& osi)
{
	for (int i = osi.port_min; i <= osi.port_max; ++i)
	{
		ServerInfo info = osi;
		info.port = i;
		m_servers.push_back(info);
	}
}

CECCrossServerList::ServerInfo* CECCrossServerList::Find(const int zoneID, const int defaultLine)
{
	ServerInfo *pInfo = NULL;

	// 先找端口没有被测试过的服务器
	for (ServerList::iterator it = m_servers.begin(); it != m_servers.end(); ++ it)
	{
		ServerInfo &cur = *it;
		if (cur.zoneid != zoneID || cur.tested) continue;
		pInfo = &cur;
		if (defaultLine == cur.line) break;
	}
	// 如果所有端口都测试过了，则重找一个测试过的
	if (pInfo == NULL)
	{
		for (ServerList::iterator it = m_servers.begin(); it != m_servers.end(); ++ it)
		{
			ServerInfo &cur = *it;
			if (cur.zoneid != zoneID) continue;
			pInfo = &cur;
			if (defaultLine == cur.line) break;
		}
	}

	return pInfo;
}

void CECCrossServerList::GetOrgAddressToTest(OriServerInfoContainer& org_info)
{
	org_info.clear();
	for (ServerList::iterator it = m_servers.begin(); it != m_servers.end(); ++ it)
	{
		if (!it->tested) 
		{
			// 如果没有测试过，则查找能跨到该中央服的原服信息
			for (OrgCrossMatch::iterator it_cross = m_OrgCrossMatchInfo.begin(); it_cross != m_OrgCrossMatchInfo.end(); ++it_cross)
			{
				// 如果添加过匹配信息，说明存在原服能跨到改中央服。否则不存在原服，则不需要测试
				if (it_cross->cross_zonid == it->zoneid)
				{
					it_cross->org_server.dest_port = it->port;
					org_info.push_back(it_cross->org_server);
					break;
				}
			}
		}
	}
}

void CECCrossServerList::AddOneMatchInfo(int cross_zonid, const AString& org_address, int org_line)
{
	OrgCrossMatchInfo info;
	info.cross_zonid = cross_zonid;
	info.org_server.address = org_address;
	info.org_server.line = org_line;
	info.org_server.dest_port = 0;
	m_OrgCrossMatchInfo.push_back(info);
}
