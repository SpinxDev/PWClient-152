// Filename	: AUICTranslate.cpp
// Creator	: Tom Zhou
// Date		: October 20, 2004
// Desc		: AUICTranslate is like the "s" command of Perl.

#include "AUI.h"

#include "AUICTranslate.h"
#include "AUICommon.h"

AUICTranslate::AUICTranslate()
{
}

AUICTranslate::~AUICTranslate()
{
}

const char * AUICTranslate::Translate(const char *str)
{
	if( str == NULL )
		return NULL;

	m_AString = str;
	const char * tmp = str; 
	char * dest = new char[strlen(tmp) + 1];
	char * target = dest;
	while(*tmp) //一个字符串的形式为： "内容"  
	{  
		//测试是否非转义字符  
		if(*tmp != '\\')
		{
			*target++=*tmp++;
			continue;
		} //这里是转义字符  
		tmp ++;
		switch(*tmp)
		{
			case '\n':
				tmp++;
				break; //连接两行的标志   

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			{
				int value = 0;
				int count = 3;
				while(*tmp <='7' && *tmp >='0' && count)
				{
					count --;
					value *=8;
					value += *tmp-'0';
					tmp ++;
				}
				if(value > 255) break;
				*target++ = value & 0xFF;
				break;
			}

			case '"': 
			case '\'':
			case '\\':
				*target ++= *tmp++;
				break;
			case 'n':
				*target ++ = '\n';
				tmp++;
				break;
			case 'r':
				*target ++ = '\r';
				tmp++;
				break;
			case 't':
				*target ++ = '\t';
				tmp++;
				break;
			case 'v':
				*target ++ = '\v';
				tmp++;
				break;
			default:
				break;
		}
	}
	*target = 0;
	m_AString = dest;
	delete []dest;
	return m_AString;
}

const wchar_t * AUICTranslate::Translate(const wchar_t *str)
{
	if( !str )
		return NULL;

	m_AWString = str;
	const wchar_t * tmp = str; 
	wchar_t * dest = new wchar_t[wcslen(tmp) + 1];
	wchar_t * target = dest;
	while(*tmp) //一个字符串的形式为： "内容"  
	{  
		//测试是否非转义字符  
		if(*tmp != '\\')
		{
			*target++=*tmp++;
			continue;
		} //这里是转义字符  
		tmp ++;
		switch(*tmp)
		{
			case '\n':
				tmp++;
				break; //连接两行的标志   

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			{
				int value = 0;
				int count = 3;
				while(*tmp <='7' && *tmp >='0' && count)
				{
					count --;
					value *=8;
					value += *tmp-'0';
					tmp ++;
				}
				if(value > 255) break;
				*target++ = value & 0xFF;
				break;
			}

			case '"': 
			case '\'':
			case '\\':
				*target ++= *tmp++;
				break;
			case 'n':
				*target ++ = '\n';
				tmp++;
				break;
			case 'r':
				*target ++ = '\r';
				tmp++;
				break;
			case 't':
				*target ++ = '\t';
				tmp++;
				break;
			case 'v':
				*target ++ = '\v';
				tmp++;
				break;
			default:
				break;
		}
	}
	*target = 0;
	m_AWString = dest;
	delete []dest;
	return m_AWString;
}

const char * AUICTranslate::ReverseTranslate(const char *str)
{
	if( !str )
		return NULL;

	m_AString = str;
	const char * tmp = str; 
	char * dest = new char[strlen(tmp) * 2 + 1];
	char * target = dest;
	while(*tmp) //一个字符串的形式为： "内容"  
	{  
		switch(*tmp)
		{
			case '"': 
			case '\'':
			case '\\':
				*target ++ = '\\';
				*target ++ = *tmp;
				break;
			case '\n':
				*target ++ = '\\';
				*target ++ = 'n';
				break;
			case '\r':
				*target ++= '\\';
				*target ++ = 'r';
				break;
			case '\t':
				*target ++= '\\';
				*target ++ = 't';
				break;
			case '\v':
				*target ++= '\\';
				*target ++ = 'v';
				break;
			default:
				*target ++ = *tmp;
				break;
		}
		tmp++;
	}
	*target = 0;
	m_AString = dest;
	delete []dest;
	return m_AString;
}

const wchar_t * AUICTranslate::ReverseTranslate(const wchar_t *str)
{
	if( !str )
		return NULL;

	m_AWString = str;
	const wchar_t * tmp = str; 
	wchar_t * dest = new wchar_t[wcslen(tmp) * 2 + 1];
	wchar_t * target = dest;
	while(*tmp) //一个字符串的形式为： "内容"  
	{  
		switch(*tmp)
		{
			case '"': 
			case '\'':
			case '\\':
				*target ++ = '\\';
				*target ++ = *tmp;
				break;
			case '\n':
				*target ++ = '\\';
				*target ++ = 'n';
				break;
			case '\r':
				*target ++= '\\';
				*target ++ = 'r';
				break;
			case '\t':
				*target ++= '\\';
				*target ++ = 't';
				break;
			case '\v':
				*target ++= '\\';
				*target ++ = 'v';
				break;
			default:
				*target ++ = *tmp;
				break;
		}
		tmp++;
	}
	*target = 0;
	m_AWString = dest;
	delete []dest;
	return m_AWString;
}
