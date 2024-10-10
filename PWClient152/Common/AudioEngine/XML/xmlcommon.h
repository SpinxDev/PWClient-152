#ifndef XMLCOMMON_INCLUDED
#define XMLCOMMON_INCLUDED

#include "tinyxml.h"
#include <guiddef.h>
#include <string>

inline void AddElement(TiXmlNode* parent, const char* szName, const std::string& val)
{
	TiXmlElement* ele = new TiXmlElement(szName);
	ele->SetAttribute("value", val.c_str());
	parent->LinkEndChild(ele);
}

inline void AddElement(TiXmlNode* parent, const char* szName, const GUID& val)
{
	TiXmlElement* ele = new TiXmlElement(szName);
	char sz[1024] = {0};	
	sprintf_s(sz, 1024, "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}", val.Data1,val.Data2,val.Data3,
		val.Data4[0],
		val.Data4[1],
		val.Data4[2],
		val.Data4[3],
		val.Data4[4],
		val.Data4[5],
		val.Data4[6],
		val.Data4[7]);
	std::string str(sz);
	ele->SetAttribute("value", str.c_str());
	parent->LinkEndChild(ele);
}

inline void AddElement(TiXmlNode* parent, const char* szName, int val)
{
	TiXmlElement* ele = new TiXmlElement(szName);
	ele->SetAttribute("value", val);
	parent->LinkEndChild(ele);
}

inline void AddElement(TiXmlNode* parent, const char* szName, unsigned int val)
{
	TiXmlElement* ele = new TiXmlElement(szName);
	ele->SetAttribute("value", (int)val);
	parent->LinkEndChild(ele);
}

inline void AddElement(TiXmlNode* parent, const char* szName, bool val)
{
	TiXmlElement* ele = new TiXmlElement(szName);
	ele->SetAttribute("value", val ? "True" : "False");
	parent->LinkEndChild(ele);
}

inline void AddElement(TiXmlNode* parent, const char* szName, float val)
{
	TiXmlElement* ele = new TiXmlElement(szName);
	ele->SetDoubleAttribute("value", val);
	parent->LinkEndChild(ele);
}

inline void QueryElement(TiXmlNode* parent, const char* szName, std::string& val)
{
	if (TiXmlNode* pNode = parent->FirstChild(szName))
	{
		pNode->ToElement()->QueryStringAttribute("value", &val);
	}
	else
		val.clear();
}

inline void QueryElement(TiXmlNode* parent, const char* szName, GUID& val)
{
	if (TiXmlNode* pNode = parent->FirstChild(szName))
	{
		std::string str;
		unsigned int uData4;
		unsigned int uData5;
		unsigned int uData6;
		unsigned int uData7;
		pNode->ToElement()->QueryStringAttribute("value", &str);
		sscanf_s(str.c_str(), "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}", &val.Data1,&val.Data2,&val.Data3,
			&val.Data4[0],
			&val.Data4[1],
			&val.Data4[2],
			&val.Data4[3],
			&uData4,
			&uData5,
			&uData6,
			&uData7);
		val.Data4[4] = (char)uData4;
		val.Data4[5] = (char)uData5;
		val.Data4[6] = (char)uData6;
		val.Data4[7] = (char)uData7;
	}
}

inline void QueryElement(TiXmlNode* parent, const char* szName, int& val, int default_val = 0)
{
	if (TiXmlNode* pNode = parent->FirstChild(szName))
	{
		pNode->ToElement()->QueryIntAttribute("value", &val);
	}
	else
		val = default_val;
}

inline void QueryElement(TiXmlNode* parent, const char* szName, unsigned int& val, unsigned int default_val = 0)
{
	if (TiXmlNode* pNode = parent->FirstChild(szName))
	{
		pNode->ToElement()->QueryIntAttribute("value", (int*)&val);
	}
	else
		val = default_val;
}

inline void QueryElement(TiXmlNode* parent, const char* szName, bool& val, bool default_val = false)
{
	if (TiXmlNode* pNode = parent->FirstChild(szName))
	{
		if (TiXmlElement* pElement = pNode->ToElement())
		{
			std::string strVal;
			pElement->QueryStringAttribute("value", &strVal);
			if (strVal == "True")
				val = true;
			else if (strVal == "False")
				val = false;
			else
				val = default_val;
		}
	}
}

inline void QueryElement(TiXmlNode* parent, const char* szName, float& val,float default_val = 0)
{
	if (TiXmlNode* pNode = parent->FirstChild(szName))
	{
		pNode->ToElement()->QueryFloatAttribute("value", (float*)&val);
	}
	else
		val = default_val;
}

#endif