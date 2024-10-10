/********************************************************************
  created:	   4/12/2012  
  filename:	   EC_CrossServerList.h
  author:      zhougaomin01305  
  description: ¿ç·þServerList½Ó¿Ú.
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
		}

		ACString server_name;
		int port;
		int port_min;
		int port_max;
		AString address;
		int zoneid;
		int	line;

		void RandPort()
		{
			port = (port_min == port_max)
				? port_min
				: (port_min + (rand()%(1+port_max-port_min)));
		}
	};
public:
	~CECCrossServerList();
	void PutOneServer(const ServerInfo& osi);
	void Clear();
	CECCrossServerList::ServerInfo* Find(const int zoneID, const int defaultLine);
	
	static CECCrossServerList& GetSingleton();
	
private:
	typedef abase::vector<ServerInfo> ServerList;
	ServerList m_servers;
	CECCrossServerList();		
};