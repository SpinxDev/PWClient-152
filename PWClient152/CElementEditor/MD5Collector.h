#pragma once

#include <vector>
#include <string>

class MD5Collector
{
	typedef std::vector< std::string > MD5Array;
	MD5Array	m_md5;
public:
	void Clear();
	void Add(const char *szFileName);
	void Sort();
	void Output(const char *szFileName);
};