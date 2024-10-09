#pragma once

#include <hashmap.h>
#include <vector.h>
#include <AAssist.h>
#include <winsock.h>

class CECServerList
{
public:
	struct ServerInfo
	{
		ServerInfo() {
			group_id = -1;
			port = port_min = port_max = 0;
			zone_id=0;
			flag = 0;
			line = 0;
			addr_valid=false;
			need_ping=false;
			ping_done=false;
		}
		ACString server_name;
		int group_id;
		int port;
		int port_min;
		int port_max;
		AString address;
		int zone_id;
		int flag;			//	0: defalut, 1: 火爆
		int	line;			//	0: 无区分，1: 电信，2: 网通
		
		in_addr addr;
		bool	addr_valid;
		bool	need_ping;
		bool	ping_done;
		
		bool HasGroup() { return group_id > -1;}
		
		bool HasPort(int port)const{
			return port == 0					//	port 为 0 时不做检查，主要用于查询 port 条件判断
				|| port >= port_min && port <= port_max;	//	其它情况检查范围
		}
		
		void RandPort(){
			port = (port_min == port_max)
				? port_min
				: (port_min + (rand()%(1+port_max-port_min)));
		}
	};
	
	struct GroupInfo
	{
		GroupInfo():bHot(false){}
		bool bHot;
		ACString group_name;
		abase::vector<int> server_vec;
	};
	
	struct SERVERPINGINFO
	{
		in_addr addr;
		int		port;
		DWORD	num;
		DWORD	lasttime;
	};

public:
	~CECServerList(){ Clear(); }

	static CECServerList & Instance();
	
	void Clear();
	void Init(const ACHAR *szHotGroupName);
	
	int GetServerCount() const { return m_serverVec.size();}
	ServerInfo& GetServer(int i) { return m_serverVec[i];}
	int FindServerByName(const ACString& name, int defaultLine);
	int FindServerByAddress(const AString& address, int defaultLine, int port);
	int FindServerByAll(const AString &address, const ACString &name, int line);
	
	int GetGroupCount() const { return m_groupVec.size();}
	int FindGroup(int iServer, bool bHot);
	GroupInfo& GetGroup(int i) { return m_groupVec[i];}
	
	void OnTick();
	void SetNeedPing(int iServer, bool bNeed);
	void PingServer();
	void ResetPing();
	
	void LoadServer();
	void SaveServer();
	bool ApplyServerSetting();
	bool CanApply(){ return GetSelected() >= 0; }

	void SelectServer(int index){ m_iSelected = index; }
	int  GetSelected(){ return m_iSelected; }

	void Export();

private:
	CECServerList();
	CECServerList(const CECServerList &);
	CECServerList & operator =(const CECServerList &);
	
protected:
	typedef abase::vector<ServerInfo>	ServerInfoVec;
	ServerInfoVec m_serverVec;
	int			  m_iSelected;

	typedef abase::vector<GroupInfo> GroupInfoVec;
	GroupInfoVec	m_groupVec;
	
	AString		m_strServerList;
	
	SERVERPINGINFO* m_pingInfo;
};