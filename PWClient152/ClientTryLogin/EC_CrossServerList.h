/********************************************************************
  created:	   4/12/2012  
  filename:	   EC_CrossServerList.h
  author:      zhougaomin01305  
  description: ���ServerList�ӿ�.
  Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
*********************************************************************/

#pragma once

#include <vector.h>
#include <aassist.h>

///////////////////////////////////////////////////////////////////////////
//  
//  class CECCrossServerList
//  
///////////////////////////////////////////////////////////////////////////

class CECCrossServerList
{
public:
	struct ServerInfo
	{
		ServerInfo() 
		{
			port = port_min = port_max = 0;
			zoneid = 0;
			line = 0;
			tested = false;
		}

		ACString server_name;
		int port;
		int port_min;
		int port_max;
		AString address;
		int zoneid;
		int	line;
		bool tested; // �Ƿ񱻲��Թ��ı�־

		void RandPort()
		{
			port = (port_min == port_max)
				? port_min
				: (port_min + (rand()%(1+port_max-port_min)));
		}
		void SetTested(bool bTest) { tested = bTest; }
	};
	struct OrgServerInfo	// ԭ����Ϣ
	{
		AString address;	// ip��ַ
		int line;			// ��	
		int dest_port;		// Ŀ�����Ķ˿ں�
	};
	typedef abase::vector<OrgServerInfo> OriServerInfoContainer;

	struct OrgCrossMatchInfo  // ԭ���������ƥ����Ϣ
	{
		int cross_zonid;		// �����zoneid
		OrgServerInfo org_server; 
	};

public:
	~CECCrossServerList();
	void PutServerAllPort(const ServerInfo& osi);
	void Clear();
	CECCrossServerList::ServerInfo* Find(const int zoneID, const int defaultLine);
	void GetOrgAddressToTest(OriServerInfoContainer& org_info);
	void AddOneMatchInfo(int cross_zonid, const AString& org_address, int org_line);

	static CECCrossServerList& GetSingleton();
	
private:
	typedef abase::vector<ServerInfo> ServerList;
	ServerList m_servers;
	typedef abase::vector<OrgCrossMatchInfo> OrgCrossMatch;
	OrgCrossMatch m_OrgCrossMatchInfo;

	CECCrossServerList();
};