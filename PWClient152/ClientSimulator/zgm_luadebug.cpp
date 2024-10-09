
#include "zgm_luadebug.h"
#include <fstream>
#include <cctype>
#include <algorithm>
#include <shellapi.h>

int CHook(lua_State*L)
{
	char* pfilename = (char*)lua_tostring(L,1);
	int line        = (int)  lua_tonumber(L,2);
	char* pevt      = (char*)lua_tostring(L,3);
	
	ZgmLuaDebug::GetSingleton().DebugHook(pfilename,line,pevt);

	return 1;
}

int CCanContinue(lua_State*L)
{
	int iret = ZgmLuaDebug::GetSingleton().DebugCanContinue();
	lua_pushnumber(L,iret);

	return 1;
}
///////////////////////////////////////
//

ZgmLuaDebug::ZgmLuaDebug()
    :	m_filesend ("zgmsend.txt"),
		m_filesflag("zgmsflag.txt"),
		m_filerecv ("zgmrecv.txt"),
		m_filerflag("zgmrflag.txt"),
		m_debugContinue(1)

{
}

ZgmLuaDebug& ZgmLuaDebug::GetSingleton()
{
	static ZgmLuaDebug instance;
	return instance;
}

void ZgmLuaDebug::ZgmLuaDebugInit(lua_State *L)
{
	m_L = L;
	luaopen_debug(L);
	luaL_openlibs(L); 

	m_bRunThread =  true;

	HWND h = ::FindWindow(NULL,"ZgmLuaDebugger");
	// �����пͻ��ˣ�����luaԶ�̵�����
	if(h == NULL) ::ShellExecuteA(NULL,"open","ZgmLuaDebugger.exe",NULL,NULL,SW_SHOWNORMAL); 

	// ע��һ��hook����
	lua_register(L, "CHook", CHook);
	lua_register(L, "CCanContinue", CCanContinue);
	// ����һ��debugsetting���ڵ�����
	luaL_dofile(L, "ZgmDebugSetting.lua");  

	::CreateThread(0,0,ZgmLuaDebug::ThreadRun,this,0,0);
	SendStrings strs;
	strs.push_back("file ");
	SetSendStrings(strs);
}

void ZgmLuaDebug::ZgmLuaDebugRelease()
{
	HWND h = ::FindWindow(NULL,"ZgmLuaDebugger");
	::PostMessage(h,WM_CLOSE,0,0);
	m_bRunThread = false;
}

void ZgmLuaDebug::SplitString(const std::string& str,SplitStrings&splits,const char*sep)
{
	splits.clear();
	std::string temp = str;
	size_t pos;

	while(true)
	{
		pos = temp.find(sep);
		if(pos == -1)
		{
			splits.push_back(temp);
			break;
		}
		splits.push_back(temp.substr(0,pos));
		temp = temp.substr(pos);
		while(true)
		{
			if(temp[0] != sep[0]) break;
			temp = temp.substr(1);
		}
	}
}

DWORD ZgmLuaDebug::ThreadRun(void*p)
{
	ZgmLuaDebug&instance = ZgmLuaDebug::GetSingleton();
	while(instance.m_bRunThread)
	{
		/////////////////////////////
		Sleep(10);
		std::string sFlag,rFlag;
		// ��Է��ͺͽ��գ����д���
		std::ifstream fSendFlag(instance.m_filesflag);
		std::ifstream fRecvFlag(instance.m_filerflag);
		fSendFlag >> sFlag;
		fRecvFlag >> rFlag;
		fSendFlag.close();
		fRecvFlag.close();
		///////////////////////////////////////////////////////////////////
		// �յ��ļ�
		if(rFlag.compare("recved") == 0)
		{
			std::string recvstr;
			{
			std::ifstream f(instance.m_filerecv);
			f.seekg(0,std::ios::end);
			int count = f.tellg();
			if(count)
			{
				f.seekg(0,std::ios::beg);
				char*pbuf = new char[count + 1];
				f.read(pbuf,count);
				pbuf[count] = 0;
				recvstr = pbuf;
				delete[] pbuf;
			}
			else
			{
				recvstr = "";
			}
			f.close();
			}

			{
			std::ofstream f(instance.m_filerflag);
			f << "ready";
			f.close();
			}
			// ������������
			SplitStrings splits;
			instance.SplitString(recvstr,splits," ");
			if(!splits.empty())
			{
				std::string cmd = splits[0];
				//std::transform(cmd.begin(), cmd.end(), cmd.begin(), std::tolower);
				if(cmd.compare("wtch") == 0)
				{
					instance.SetWatch(0,true);
					for(size_t i = 1; i < splits.size(); ++i)
						instance.SetWatch(splits[i].c_str(),false);
				}
				else if(cmd.compare("setb") == 0)
				{
					if(splits.size() >= 2)
					{
						BreakPoints bps;
						for(size_t i = 2; i < splits.size(); ++i)
							bps.push_back( atoi(splits[i].c_str()) );
						instance.SetBreakPoint(splits[1].c_str(), bps);
					}
				}
				else if(cmd.compare("step") == 0
					 || cmd.compare("into") == 0
					 || cmd.compare("over") == 0
					 || cmd.compare("run") == 0)
				{
					instance.SetState(cmd.c_str());
				}
				else
				{
				}
			}
		}
		// ���Է����ļ�
		if(sFlag.compare("ready") != 0 && sFlag.compare("sending") != 0 && !instance.m_sendStrings.empty())
		{
			std::string sendstr;
			// ����
			{
				ZgmLockScoped lock(instance.m_csMutex);
				sendstr = instance.m_sendStrings.front();
				instance.m_sendStrings.pop_front();
			}

			{
			std::ofstream f(instance.m_filesend);
			f << sendstr;
			f.close();
			}

			{
			std::ofstream f(instance.m_filesflag);
			f << "ready";
			f.close();
			}
		}
	}
	return 0;
}

void ZgmLuaDebug::SetBreakPoint(const char* filename,const BreakPoints& bps)
{
	// ����
	{
		ZgmLockScoped lock(m_csMutex);
		m_bpMaps.erase(filename);
		m_bpMaps[filename] = bps;
	}
}

void ZgmLuaDebug::SetWatch(const char* varname,bool bUnWatch)
{
	// ����
	{
		ZgmLockScoped lock(m_csMutex);
		if(varname)
		{
			m_watchs.erase(varname);
			if(!bUnWatch) m_watchs[varname] = true;
		}
		else
		{
			if(bUnWatch) m_watchs.clear();
		}
	}
}

void ZgmLuaDebug::SetState(const char* state)
{
	// ����
	{
		ZgmLockScoped lock(m_csMutex);
		m_debugState = state;
		if( m_debugState.compare("step") == 0
		 || m_debugState.compare("into") == 0
		 || m_debugState.compare("over") == 0
		 || m_debugState.compare("run") == 0)
		 m_debugContinue = 1;
	}
}

void ZgmLuaDebug::GetBreakPoint(const char*filename,BreakPoints& bps)
{
	// ����
	{
		ZgmLockScoped lock(m_csMutex);
		bps.clear();

		BreakPoints& thebp = m_bpMaps[filename];
		BreakPoints::iterator it = thebp.begin(), itEnd = thebp.end();
		for(; it != itEnd; ++it)
		{
			bps.push_back((*it));
		}
	}
}

void ZgmLuaDebug::GetWatch(WatchVars& wvars)
{
	// ����
	{
		ZgmLockScoped lock(m_csMutex);
		wvars.clear();

		WatchVars::iterator it = m_watchs.begin(), itEnd = m_watchs.end();
		for(; it != itEnd; ++it)
		{
			wvars.insert( std::make_pair(it->first,it->second));
		}
	}
}

const char*ZgmLuaDebug::GetDebugState()
{
	// ����
	{
		ZgmLockScoped lock(m_csMutex);
		return m_debugState.c_str();
	}
}

void ZgmLuaDebug::SetSendStrings(const SendStrings& strs)
{
	// ����
	{
		ZgmLockScoped lock(m_csMutex);
		SendStrings::const_iterator it = strs.begin(), itEnd = strs.end();
		for(; it != itEnd; ++it)
		{
			m_sendStrings.push_back((*it));
		}
	}
}

int ZgmLuaDebug::DebugCanContinue()
{
	return m_debugContinue;
}

void ZgmLuaDebug::DebugHook(const std::string&filename,int line,const std::string&evt)
{
	m_debugContinue = 1;
	//////////////////////////////////////////////////////
	if(evt.compare("call") == 0)
	{
		int l = m_stack_levels[filename];
		m_stack_levels[filename] = l + 1;
	}
	else if(evt.compare("return") == 0)
	{
		int l = m_stack_levels[filename];
		m_stack_levels[filename] = l - 1;
	}
	else // line
	{
		// �������µ�lua�ļ�����
		FileSends::iterator fit = m_fileSends.find(filename);
		if(fit == m_fileSends.end())
		{
			m_fileSends.insert( std::make_pair(filename,true) );

			std::string luacode = "file ";
			luacode += filename;
		
			std::ifstream f(filename.c_str());
			f.seekg(0,std::ios::end);
			int count = f.tellg();
			if(count)
			{
				f.seekg(0,std::ios::beg);
				char*pbuf = new char[count + 1];
				f.read(pbuf,count);
				pbuf[count] = 0;
				std::string temp(pbuf,count);
				temp = temp.substr(0,count);
				luacode +=  temp;
				delete[] pbuf;
			}
			else
			{
			}
			f.close();

			SendStrings strs;
			strs.push_back(luacode);
			SetSendStrings(strs);
		}
		/////////////////////////////////////////////////
		std::string state = GetDebugState();
		if(state.compare("step") == 0 || state.compare("into") == 0)
		{
			STEP:
			SetState("step_set");

			SendStrings strs;
			char buf[32];
			std::string temp;
			// ���͵�ǰ��
			temp = "line ";
			temp += filename;
			temp += " ";
			itoa(line,buf,10);
			temp += buf;
			strs.push_back(temp);
			// ���͵�ǰ������Ϣ
			WatchVars wvars;
			GetWatch(wvars);
			std::string vstr = "wtch ";
			bool bsend = false;
			WatchVars::iterator it = wvars.begin(), itEnd = wvars.end();
			for(; it != itEnd; ++it)
			{
				temp = GetVarValue(it->first);
				if(temp.empty()) continue;
				if(temp.compare("nil") == 0) continue;
				temp = it->first + "=" + temp + ";";
				vstr += temp;
				bsend = true;
			}
			if(bsend) strs.push_back(vstr);

			SetSendStrings(strs);

			// �ȴ������źţ��´�step��into����over��
			m_debugContinue = 0;
		}
		else if(state.compare("over") == 0)
		{
			// �����´δ����¼�
			m_step_levels[filename] = m_stack_levels[filename];
			SetState("over_set");
		}
		else if(state.compare("step_set") == 0)
		{
			goto STEP;
		}
		else if(state.compare("over_set") == 0)
		{
			if(m_stack_levels[filename] < m_step_levels[filename])
			{
				SetState("step");
				goto STEP;
			}
		}
		else if(state.compare("run") == 0)
		{
			// ���ÿգ���ֱ���жϵ�Żᴥ��
			SetState("");
		}
		else // ��������жϵ�Ŵ���
		{
			BreakPoints bps;
			GetBreakPoint(filename.c_str(), bps);
			BreakPoints::iterator it = bps.begin(), itEnd = bps.end();
			for(; it != itEnd; ++it)
			{
				if(line == *it)
				{
					goto STEP;
				}
			}
		}
	}
}

std::string ZgmLuaDebug::GetVarValue(const std::string& varname)
{
	ZgmLockScoped lock(m_csMutex);

	lua_State *L = m_L;
	// ���ú���
	lua_getglobal(L, "zgm_getvarvalue");
	// ��Ҫһ������
	lua_pushstring(L, varname.c_str());
	// ���ú���
	lua_call(L, 1, 1);
	// ��÷���ֵ
	char*pvalue = (char*)lua_tostring(L, -1);
	// �����ջ
	lua_pop(L, 1);
	// ����
	return pvalue;
}