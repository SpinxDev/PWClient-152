#ifndef __XMLCODER_H
#define __XMLCODER_H

#include <AString.h>
#include <hashmap.h>
#include <hashtab.h>
#include <vector.h>

class AWString;

namespace GNET
{

class XmlCoder
{
public:
	typedef AString String;
	typedef abase::pair<String, String>	StringPair;
	typedef abase::vector<StringPair> PropertyVector;

protected:
	String	data;

	typedef abase::hash_map<unsigned short, bool> Entities;
	static Entities entities;

	static Entities &getEntities();

public:
	const char * c_str() { return data; }

	// Header
	void append_header(const char *encoding = "UTF-8");

	// Node
	void begin_node(const char *name);
	void begin_node(const char *name, const char *prop, const String &value);
	void begin_node(const char *name, const PropertyVector &property);
	void end_node(const char *name);

	// Single Node
	void append_node(const char *name, const PropertyVector &property);
	void append_node(const char *name, const char* prop, const String& value);

	// Convert
	static String toString(char x);
	static String toString(short x);
	static String toString(int x);
	static String toString(float x);
	static String toString(__int64 x);
	static String convert(const AString &str);
};

}

#endif
