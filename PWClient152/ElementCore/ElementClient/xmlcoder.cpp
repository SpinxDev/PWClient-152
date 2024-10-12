#include "xmlcoder.h"
#include <AWString.h>

namespace GNET
{
	XmlCoder::Entities XmlCoder::entities;
	
	XmlCoder::Entities & XmlCoder::getEntities()
	{
		if (entities.empty())
		{
			entities[0] = true;
			entities['"'] = true;
			entities['&'] = true;
			entities['\''] = true;
			entities['<'] = true;
			entities['>'] = true;
		}
		return entities;
	}

	void XmlCoder::append_header(const char *encoding)
	{
		data = "<?xml version=\"1.0\"";
		if (encoding && strlen(encoding))
			data += String(" encoding=\"") + encoding + "\"";
		data += "?>\n";
	}
	
	void XmlCoder::begin_node(const char *name)
	{
		data += String("<") + name + ">\n";
	}
	void XmlCoder::begin_node(const char *name, const char *prop, const String &value)
	{
		data += String("<") + name + " " + prop + "=\"" + value + "\"" + ">\n";
	}
	void XmlCoder::begin_node(const char *name, const PropertyVector &property)
	{
		data += String("<") + name;

		for (size_t i = 0; i < property.size(); ++ i)
		{
			// Format: name1="value1" name2="value2" ...
			data += " " + property[i].first + "=\"" + property[i].second + "\"";
		}
		
		data += ">\n";
	}
	void XmlCoder::end_node(const char *name)
	{
		data += String("</") + name + ">\n";
	}

	// Variable
	void XmlCoder::append_node(const char *name, const PropertyVector &property)
	{
		data += String("<") + name;

		for (size_t i = 0; i < property.size(); ++ i)
		{
			// Format: name1="value1" name2="value2" ...
			data += " " + property[i].first + "=\"" + property[i].second + "\"";
		}
		
		data += "/>\n";
	}
	void XmlCoder::append_node(const char *name, const char* prop, const String& value)
	{
		data += String("<") + name + " " + prop + "=\"" + value + "\"" + "/>\n";
	}

	// Convert
	XmlCoder::String XmlCoder::toString(char x)
	{
		String result;
		result.Format("%d", (int)x);
		return result;
	}
	XmlCoder::String XmlCoder::toString(short x)
	{
		String result;
		result.Format("%d", (int)x);
		return result;
	}
	XmlCoder::String XmlCoder::toString(int x)
	{
		String result;
		result.Format("%d", (int)x);
		return result;
	}
	XmlCoder::String XmlCoder::toString(float x)
	{
		String result;
		result.Format("%.9g", (int)x);
		return result;
	}
	XmlCoder::String XmlCoder::toString(__int64 x)
	{
		String result;
		result.Format("%lld", (int)x);
		return result;
	}
	XmlCoder::String XmlCoder::convert(const AString &str)
	{
		//	把字符转换成符合xml标准的实体字符
		String result;
		Entities &entities = getEntities();
		int len = str.GetLength();
		for (int i = 0; i < len; ++ i)
		{
			char c = str[i];
			if (entities.find(c)!=entities.end()) 
				result += String("&#") + toString(c) + ";";
			else
				result += (char)c;
		}
		return result;
	}
}