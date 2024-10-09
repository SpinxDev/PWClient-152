/********************************************************************
  created:	   4/12/2012  
  filename:	   EC_CrossServerList.cpp
  author:      zhougaomin01305  
  description: ���ServerList�ӿ�.
  Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
*********************************************************************/

#include "EC_CrossServerList.h"

///////////////////////////////////////////////////////////////////////////
//  
//  Implement class CECCrossServerList
//  
///////////////////////////////////////////////////////////////////////////

// ��ȡʵ��
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

	// ���Ҷ˿�û�б����Թ��ķ�����
	for (ServerList::iterator it = m_servers.begin(); it != m_servers.end(); ++ it)
	{
		ServerInfo &cur = *it;
		if (cur.zoneid != zoneID || cur.tested) continue;
		pInfo = &cur;
		if (defaultLine == cur.line) break;
	}
	// ������ж˿ڶ����Թ��ˣ�������һ�����Թ���
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
			// ���û�в��Թ���������ܿ絽���������ԭ����Ϣ
			for (OrgCrossMatch::iterator it_cross = m_OrgCrossMatchInfo.begin(); it_cross != m_OrgCrossMatchInfo.end(); ++it_cross)
			{
				// �����ӹ�ƥ����Ϣ��˵������ԭ���ܿ絽������������򲻴���ԭ��������Ҫ����
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
