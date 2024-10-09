#include "EC_ServerList.h"
#include "EC_CrossServerList.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GetHostByName.h"
#include "EC_CrossServer.h"
#include "EC_Reconnect.h"
#include "EC_Split.h"
#include "EC_TimeSafeChecker.h"

#include <AUICommon.h>
#include <achar.h>
#include <AIniFile.h>

#include <ctime>
#include <io.h>

CECServerList & CECServerList::Instance()
{
	static CECServerList dummy;
	return dummy;
}

CECServerList::CECServerList()
{
	m_pingInfo = NULL;
	m_iSelected = -1;
}

void CECServerList::Clear()
{
	delete [] m_pingInfo;
	m_pingInfo = NULL;
	
	m_serverVec.swap(ServerInfoVec());
	m_iSelected = -1;
	m_groupVec.swap(GroupInfoVec());
	
	m_strServerList.Empty();
}

void CECServerList::Init(const ACHAR *szHotGroupName)
{	
	//	清除原有设置
	Clear();
	CECCrossServerList::GetSingleton().Clear();

	// 按顺序查找第一个存在的 serverlist.txt 路径
	AString strServerLists[] = 
	{
		"..\\patcher\\server\\serverlist.txt",
			"userdata\\server\\serverlist.txt",
	};
	for (int j = 0; j < ARRAY_SIZE(strServerLists); ++ j)
	{
		if (access(strServerLists[j], 0) == 0)
		{
			m_strServerList = strServerLists[j];
			break;
		}
	}
	
	FILE *fStream;
	int i = 0;
	int group_id = -1;
	
	srand(time(NULL));
	
	if(!m_strServerList.IsEmpty() && (fStream = fopen(m_strServerList, "rb")) != NULL )
	{
		const ACHAR * szHotFlag = _AL("&1");           // 火爆新服标记
		const ACHAR * szCrossServerFlag = _AL("&2");   // 跨服中央服标记
		ACHAR szLine[256];
		a_fgetc(fStream);
		GroupInfo hotGroup;		
		hotGroup.bHot = true;
		hotGroup.group_name = szHotGroupName;
		do{
			if (!a_fgets(szLine, ARRAY_SIZE(szLine), fStream))
				break;
			if( a_strlen(szLine) > 0){
				int n = a_strlen(szLine) - 1;
				while( n >= 0 && (szLine[n] == '\r' || szLine[n] == '\n') )
					n--;
				szLine[n + 1] = '\0';
				bool bCrossServer = false;
				CECSplitHelperW s(szLine, _AL("\t"));
				if (s.Count() >= 2){
					CECSplitHelperW address(s.ToString(1), _AL(":"));
					if (address.Count() < 2) continue;
					ServerInfo info;
					info.group_id = group_id;

					//	神龙	
					info.server_name = s.ToString(0);

					//	29000-29005:10.68.16.108:1
					CECSplitHelperW ports(address.ToString(0), _AL("-"));
					if (ports.Count() == 0){
						info.port_min = info.port_max = 0;
					}else{
						int pmin = ports.ToInt(0);
						int pmax = pmin;							
						if (ports.Count() > 1){
							pmax = ports.ToInt(1);
						}							
						if (pmin > pmax){
							std::swap(pmin, pmax);
						}							
						info.port_min = pmin;
						info.port_max = pmax;
					}
					info.RandPort();
					info.address = AC2AS(address.ToString(1));
					info.line = address.Count() > 2 ? address.ToInt(2) : 0;

					//	&1 &2
					for (int u = 2; u < s.Count(); ++ u)
					{
						const AWString &cur = s.ToString(u);
						if (cur == szHotFlag)
							info.flag = 1;
						else if (cur == szCrossServerFlag)
							bCrossServer = true;
					}

					//	zoneid
					info.zone_id = (s.Count() > 2) ? s.ToInt(2) : 0;
					
					//	添加跨服服务器
					if (bCrossServer){
						CECCrossServerList::ServerInfo crossInfo;
						crossInfo.port		= info.port;
						crossInfo.port_max	= info.port_max;
						crossInfo.port_min	= info.port_min;
						crossInfo.address	= info.address;
						crossInfo.zoneid	= info.zone_id;
						crossInfo.server_name= info.server_name;
						crossInfo.line		= info.line;
						CECCrossServerList::GetSingleton().PutOneServer(crossInfo);
						continue;
					}

					//	添加到普通服务器列表
					m_serverVec.push_back(info);					
					if(group_id>=0)
						m_groupVec[group_id].server_vec.push_back(m_serverVec.size()-1);					
					if(info.flag==1)
						hotGroup.server_vec.push_back(m_serverVec.size()-1);
				}
				else if( a_strlen(szLine) > 0 ){
					//	王者电信
					GroupInfo info;
					info.group_name = szLine;
					m_groupVec.push_back(info);
					group_id++;
				}
			}
		} while(true);
		
		// 如果有火爆新服
		if (!hotGroup.server_vec.empty())
		{
			//	作为第一个 group，以显示在服务器列表第一项
			for (i=0;i<(int)m_serverVec.size();i++)
			{
				m_serverVec[i].group_id += 1;
			}			
			m_groupVec.insert(m_groupVec.begin(),hotGroup);
		}
		
		fclose(fStream);
	}
	else
		AUI_ReportError(__LINE__, 1,"CECServerList::Init(), server list file error!");
	
	// ping info
	ResetPing();
	
}

void CECServerList::Export()
{
	if (m_strServerList.IsEmpty()) return;

	//	计算 serverlist.txt 所在目录，导出后文件将存在同目录下
	int pos = m_strServerList.ReverseFind('\\');
	if (pos < 0) pos = m_strServerList.ReverseFind('/');
	AString strServerListPath;
	if (pos >= 0) strServerListPath = m_strServerList.Left(pos+1);

	//	打开 GT 文件开始更新
	AString strSaveFile = strServerListPath + "serverlist.sev";
	FILE * fp = fopen(strSaveFile, "wb");
	if (!fp)
	{
		a_LogOutput(1, "CECServerList::Export,Failed to open for save %s", strSaveFile);
		return;
	}

	typedef std::set<int>		ZoneIDSet;
	ZoneIDSet zoneids;

	typedef std::set<ACString>	NameSet;
	NameSet names;

	for (size_t u(0); u < m_serverVec.size(); ++ u)
	{
		const ServerInfo & server = m_serverVec[u];

		//	验证 zoneid 合法
		if (server.zone_id <= 0 ||
			zoneids.find(server.zone_id) != zoneids.end())
		{
			a_LogOutput(1, "CECServerList::Export,%s ignored for invalid/duplicated zoneid(%d)", AC2AS(server.server_name), server.zone_id);
			continue;
		}

		//	利用 TrimLeft 清除服务器名称中各类空格字符，否则将影响玩家名称、进而影响玩家聊天
		ACString strExportName;
		ACString strName = server.server_name;
		while (!strName.IsEmpty())
		{
			strName.TrimLeft();
			if (!strName.IsEmpty())
			{
				strExportName += strName.Left(1);
				strName.CutLeft(1);
			}
		}

		//	验证名称合法
		if (strExportName.IsEmpty() ||
			names.find(strExportName) != names.end())
		{
			a_LogOutput(1, "CECServerList::Export,%s ignored for empty/duplicated name %s(after processed)", AC2AS(server.server_name), AC2AS(strExportName));
			continue;
		}

		//	写进文件
		fwrite(&server.zone_id, sizeof(server.zone_id), 1, fp);

		int len = strExportName.GetLength();
		fwrite(&len, 1, sizeof(len), fp);
		fwrite((const ACHAR *)strExportName, len, sizeof(ACHAR), fp);

		//	添加进冗余检查
		zoneids.insert(server.zone_id);
		names.insert(strExportName);
	}

	fflush(fp);
	fclose(fp);

	//	打开文件并输出到日志
	fp = fopen(strSaveFile, "rb");
	if (!fp)
	{
		a_LogOutput(1, "CECServerList::Export,Failed to read %s", strSaveFile);
		return;
	}
	int zoneid(0), namelen(0);
	ACString strName;
	while (fread(&zoneid, sizeof(zoneid), 1, fp) == 1)
	{
		if (fread(&namelen, sizeof(namelen), 1, fp) != 1)
		{
			a_LogOutput(1, "CECServerList::Export,Read name len error");
			break;
		}
		if (namelen <= 0)
		{
			a_LogOutput(1, "CECServerList::Export,name len invalid(%d)", namelen);
			break;
		}
		wchar_t *pBuf = strName.GetBuffer(namelen+1);
		if (fread(pBuf, sizeof(wchar_t), namelen, fp) != namelen)
		{
			a_LogOutput(1, "CECServerList::Export,Read name error");
			break;
		}
		pBuf[namelen] = _AL('\0');
		a_LogOutput(1, "CECServerList::Export,zoneid=%d, name=%s", zoneid, AC2AS(strName));
	}
	fclose(fp);
}

int CECServerList::FindGroup(int iServer, bool bHot)
{
	if (iServer >= 0){
		if (bHot){
			int nHotGroup = 0;
			const GroupInfo &g = GetGroup(nHotGroup);
			if (g.bHot && std::find(g.server_vec.begin(), g.server_vec.end(), iServer) != g.server_vec.end()){
				return nHotGroup;
			}
		}		
		const ServerInfo &info = GetServer(iServer);
		return info.group_id;
	}
	return -1;
}

int CECServerList::FindServerByName(const ACString& name, int defaultLine)
{
	//	区别于 FindServerByAddress，按名称查找时，使用 port 比较已无意义
	int iServer(-1);
	for (int i = 0; i < GetServerCount(); i++)
	{
		const ServerInfo &info = m_serverVec[i];
		if(info.server_name == name)
		{
			iServer = i;
			if (info.line == defaultLine){
				//	能找到同指定 line 相同的则返回相同的，不能找到则返回最后一条地址和端口匹配的
				break;
			}
		}
	}
	return iServer;
}
int CECServerList::FindServerByAddress(const AString& address, int defaultLine, int port)
{
	int iServer(-1);
	for (int i = 0; i < GetServerCount(); i++)
	{
		const ServerInfo &info = m_serverVec[i];
		if(info.address == address && info.HasPort(port))
		{
			iServer = i;
			if (info.line == defaultLine){
				//	能找到同指定 line 相同的则返回相同的，不能找到则返回最后一条地址和端口匹配的
				break;
			}
		}
	}
	return iServer;
}

void CECServerList::ResetPing()
{
	if (!m_pingInfo){
		m_pingInfo = new SERVERPINGINFO[GetServerCount()];
	}
	for(int i = 0; i < GetServerCount(); i++ )
	{
		m_pingInfo[i].port = 0;
		ServerInfo & info = GetServer(i);
		info.addr_valid = false;
		info.need_ping = false;
		info.ping_done = false;
	}
	g_pGame->GetGameSession()->resetPing();
	CECGetHostByName::Instance().Clear();
}

void CECServerList::SetNeedPing(int iServer, bool bNeed)
{
	if (iServer >= 0 && iServer < GetServerCount()){
		ServerInfo &info = GetServer(iServer);
		info.need_ping = bNeed;
		if (bNeed){
			CECGetHostByName::Instance().Add(info.address);
		}
	}
}

void CECServerList::PingServer()
{
	int i(0);
	for(i = 0; i < GetServerCount(); i++)
	{
		const ServerInfo &info = GetServer(i);
		SERVERPINGINFO &pingInfo = m_pingInfo[i];
		if( info.need_ping && pingInfo.port > 0 && pingInfo.num < 4 &&
			CECTimeSafeChecker::ElapsedTimeFor(pingInfo.lasttime) > pingInfo.num * 10000 )
		{
			pingInfo.lasttime = GetTickCount();
			pingInfo.num++;
			g_pGame->GetGameSession()->server_Ping(i, &pingInfo.addr, pingInfo.port);
		}
	}
}
void CECServerList::OnTick()
{
	//	根据需要进行 Ping 获取服务器相关信息
	//	由于会建立新连接，有可能与正常连接同时进行并对正常连接进行干扰（“半开连接数”），
	//	因此只在必要的时候进行，

	int i;
	
	//	检查 gethostbyname 是否返回
	for(i=0; i<GetServerCount(); ++i)
	{
		CECServerList::ServerInfo &entry = GetServer(i);
		if (!entry.addr_valid && CECGetHostByName::Instance().Get(entry.address, entry.addr))
			entry.addr_valid = true;
	}
	
	// 有ip地址的服务器，进行一次 ping 操作
	for(i=0; i<GetServerCount();)
	{
		CECServerList::ServerInfo& server = GetServer(i);		
		SERVERPINGINFO &pingInfo = m_pingInfo[i];
		if (server.addr_valid && server.need_ping && !server.ping_done)
		{
			pingInfo.addr = server.addr;
			pingInfo.port = server.port;
			pingInfo.lasttime = GetTickCount();
			pingInfo.num = 1;
			g_pGame->GetGameSession()->server_Ping(i, &server.addr, pingInfo.port);
			server.ping_done = true;
			break;
		}
		
		i ++;
	}

	//	需要定期进行 ping 操作的服务器 (port 不为0) 进行ping
	PingServer();
}

bool CECServerList::ApplyServerSetting()
{
	if (!CanApply()){
		return false;
	}
	int iServer = GetSelected();
	ServerInfo &info = GetServer(iServer);
	
	//	当前服务器有多端口时每次登录都随机端口
	info.RandPort();
	
	//	设置当前连接
	CECGame::GAMEINIT &gi = g_pGame->GetGameInit();
	gi.iPort = info.port;
	strcpy(gi.szIP, info.address);
	strcpy(gi.szServerName, AC2AS(info.server_name));
	gi.iServerID =  info.zone_id;
	gi.iLine = info.line;
	
	int nGroup = FindGroup(iServer, false);
	ACString strServerGroup = GetGroup(nGroup).group_name;
	g_pGame->GetGameRun()->SetServerName(strServerGroup, info.server_name);
	
	//	记录原服地址，用于跨服中央服回到原服
	CECCrossServer::Instance().SetOrgServerIndex(iServer);

	//	记录原服地址，用于断线重连
	if (!CECReconnect::Instance().IsReconnecting()){
		CECReconnect::Instance().SetServerIndex(iServer);
	}

	//	保存到文件
	SaveServer();
	return true;
}

int CECServerList::FindServerByAll(const AString &address, const ACString &name, int line)
{
	//	根据文件中保存内容，查找最匹配服务器

	int iServer(-1);

	int iPort = 0;
	AString strDomain;
	if (!address.IsEmpty()){
		CECSplitHelperA ss(address, ":");
		if (ss.Count() > 1){
			iPort = ss.ToInt(0);
			strDomain = ss.ToString(1);
		}else{
			strDomain = ss.ToString(0);
		}
	}
	if (!strDomain.IsEmpty()){
		//	域名不为空时，推断其它参数
		iServer = CECServerList::Instance().FindServerByAddress(strDomain, line, iPort);
		if (iServer < 0){
			iServer = CECServerList::Instance().FindServerByName(name, line);
		}
	}else if (!name.IsEmpty()){
		//	域名为空、服务器名称不为空，根据服务器名称推断
		iServer = CECServerList::Instance().FindServerByName(name, line);
	}

	return iServer;
}

void CECServerList::LoadServer()
{
	ACString strName;	//	strName 服务器名称，允许为空
	AString strAddress;	//	strAddress 允许"29000:kuangshen.link.w2i.com.cn"、"kuangshen.link.w2i.com.cn"、或为空
	int iLine;			//	iLine 允许为0

	//	从文件中读取
	ACHAR szTemp[200];
	GetPrivateProfileString(_AL("Server"), _AL("CurrentServer"),
		_AL(""), szTemp, 200,  _AL("userdata\\currentserver.ini"));
	strName = szTemp;
	strName.TrimLeft();
	strName.TrimRight();

	GetPrivateProfileString(_AL("Server"), _AL("CurrentServerAddress"),
		_AL(""), szTemp, 200,  _AL("userdata\\currentserver.ini"));
	ACString strTemp = szTemp;
	strTemp.TrimLeft();
	strTemp.TrimRight();
	strAddress = AC2AS(strTemp);

	GetPrivateProfileString(_AL("Server"), _AL("CurrentLine"),
		_AL(""), szTemp, 200,  _AL("userdata\\currentserver.ini"));
	ACString strLine = szTemp;
	strLine.TrimLeft();
	strLine.TrimRight();
	iLine = strLine.ToInt();
	
	int iServer = FindServerByAll(strAddress, strName, iLine);
	SelectServer(iServer);
}

void CECServerList::SaveServer()
{
	CECGame::GAMEINIT &gi = g_pGame->GetGameInit();

	WritePrivateProfileString(_AL("Server"), _AL("CurrentServer"), AS2AC(gi.szServerName), 
		_AL("userdata\\currentserver.ini"));

	AString strAddress;
	strAddress.Format("%d:%s", gi.iPort, gi.szIP);
	WritePrivateProfileString(_AL("Server"), _AL("CurrentServerAddress"), AS2AC(strAddress),
		_AL("userdata\\currentserver.ini"));

	WritePrivateProfileString(_AL("Server"), _AL("CurrentLine"), ACString().Format(_AL("%d"), gi.iLine), 
		_AL("userdata\\currentserver.ini"));
}
