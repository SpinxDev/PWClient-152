
#ifndef __GNET_GETITEMDESC_RE_HPP
#define __GNET_GETITEMDESC_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include <iostream>
#include <AWString.h>
#include <AString.h>

namespace GNET
{

class GetItemDesc_Re : public GNET::Protocol
{
	#include "getitemdesc_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		using namespace std;
		if (retcode != 0)
		{
			cout << "返回错误" << endl;
			return;
		}
		putOneDemandStub(*this);
	}
	
private:
	void putOneDemandStub(GNET::GetItemDesc_Re &out)
	{
		// 输出可视化结果：用于调试	
		
		// 读取名称
		{
			putUnicode(L"名称：");
			if (out.name.size()>0)
			{
				putOctet(out.name);
			}
			else
			{
				putUnicode(L"空");
			}
			printf("\n");
		}
		
		// 读取ID
		{
			putUnicode(L"ID：");
			printf("%d\n", out.id);
		}
		
		// 读取等级
		{
			putUnicode(L"等级：");
			printf("%d\n", out.level);
		}
				
		// 读取图标路径
		{
			putUnicode(L"图标：");
			if (out.icon.size()>0)
			{
				putOctet(out.icon);
			}
			else
			{
				putUnicode(L"空");
			}
			printf("\n");
		}
		// 读取物品描述信息
		{
			putOctet(out.desc);
		}
		
		// 输出一条信息后换行，以方便调试
		printf("\n");
		printf("\n");
	}
	
	void putUnicode(const wchar_t *pszHint)
	{
		AWString strHint = pszHint;
		int len = strHint.GetLength();
		int i(0);
		while (i<len)
		{
			wchar_t w1 = strHint[i];
			putwchar(w1);
			
			// 在输出 /r 后换行，以方便调试查看
			if (w1==L'r')
			{
				if (i>0 && strHint[i-1]==L'\\')
				{
					putwchar(L'\n');
				}
			}
			++i;
		}
	}
	
	void putOctet(const Octets &data)
	{
		size_t len = data.size();
		if (!len)
			return;
		char *str = new char[len+1];
		memcpy(str, data.begin(), len);
		str[len] = '\0';
		putUnicode(UTF8ToUnicode(str));
		delete []str;
	}	
	
	AWString UTF8ToUnicode(const char *str)
	{
		int len = strlen(str);
		int wlen = ::MultiByteToWideChar(CP_UTF8, NULL, str, len, NULL, 0);
		wchar_t *wStr = new wchar_t[wlen+1];
		::MultiByteToWideChar(CP_UTF8, NULL, str, len, wStr, wlen);
		wStr[wlen] = L'\0';
		AWString ret = wStr;
		delete []wStr;
		return ret;
	}
};

};

#endif
