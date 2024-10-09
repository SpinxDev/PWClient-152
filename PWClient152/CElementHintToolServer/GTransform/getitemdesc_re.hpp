
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
			cout << "���ش���" << endl;
			return;
		}
		putOneDemandStub(*this);
	}
	
private:
	void putOneDemandStub(GNET::GetItemDesc_Re &out)
	{
		// ������ӻ���������ڵ���	
		
		// ��ȡ����
		{
			putUnicode(L"���ƣ�");
			if (out.name.size()>0)
			{
				putOctet(out.name);
			}
			else
			{
				putUnicode(L"��");
			}
			printf("\n");
		}
		
		// ��ȡID
		{
			putUnicode(L"ID��");
			printf("%d\n", out.id);
		}
		
		// ��ȡ�ȼ�
		{
			putUnicode(L"�ȼ���");
			printf("%d\n", out.level);
		}
				
		// ��ȡͼ��·��
		{
			putUnicode(L"ͼ�꣺");
			if (out.icon.size()>0)
			{
				putOctet(out.icon);
			}
			else
			{
				putUnicode(L"��");
			}
			printf("\n");
		}
		// ��ȡ��Ʒ������Ϣ
		{
			putOctet(out.desc);
		}
		
		// ���һ����Ϣ���У��Է������
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
			
			// ����� /r ���У��Է�����Բ鿴
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
