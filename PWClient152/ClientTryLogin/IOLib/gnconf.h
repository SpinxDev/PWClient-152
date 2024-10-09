#ifndef __CONF_H
#define __CONF_H

#pragma warning (disable:4786)
#include <io.h>
#include <map>
#include <string>
#include <sys/stat.h>
#include <cstdio>

#include "gnthread.h"

#define R_OK 04

namespace GNET
{

using std::string;
class Conf
{
public:
	typedef string section_type;
	typedef string key_type;
	typedef string value_type;
private:
	time_t mtime;
	struct stringcasecmp
	{
		bool operator() (const string &x, const string &y) const { return stricmp(x.c_str(), y.c_str()) < 0; }
	};
	static Conf instance;
	typedef std::map<key_type, value_type, stringcasecmp> section_hash;
	typedef std::map<section_type, section_hash, stringcasecmp> conf_hash;
	conf_hash confhash;
	string filename;
	static Thread::RWLock locker; 
	Conf() : mtime(0) { }
	void reload()
	{
		struct _stat st;
		Thread::RWLock::WRScoped l(locker);
		for ( _stat(filename.c_str(), &st); mtime != st.st_mtime; _stat(filename.c_str(), &st) )
		{
			mtime = st.st_mtime;
			FILE *fp = fopen(filename.c_str(), "rt");
			char buffer[1024] = {0};
			string line;
			section_type section;
			section_hash sechash;
			if (!confhash.empty()) confhash.clear();
			while (!feof(fp))
			{
				if (!fgets(buffer, sizeof(buffer)/sizeof(buffer[0]), fp))
					break;
				line = buffer;
				if (!line.empty() && *line.rbegin() == '\n')
					line.erase(line.end()-1);
				if (line.empty()) continue;

				const char c = line[0];
				if (c == '#' || c == ';') continue;
				if (c == '[')
				{
					string::size_type start = line.find_first_not_of(" \t", 1);
					if (start == string::npos) continue;
					string::size_type end   = line.find_first_of(" \t]", start);
					if (end   == string::npos) continue;
					if (!section.empty()) confhash[section] = sechash;
					section = section_type(line, start, end - start);
					sechash.clear();
				} else {
					string::size_type key_start = line.find_first_not_of(" \t");
					if (key_start == string::npos) continue;
					string::size_type key_end   = line.find_first_of(" \t=", key_start);
					if (key_end == string::npos) continue;
					string::size_type val_start = line.find_first_of("=", key_end);
					if (val_start == string::npos) continue;
					val_start = line.find_first_not_of(" \t", val_start + 1);
					if (val_start == string::npos) continue;
					sechash[key_type(line, key_start, key_end - key_start)] = value_type(line, val_start);
				}
			}
			if (!section.empty()) confhash[section] = sechash;
			fclose(fp);
		}
	}
public:
	value_type find(const section_type &section, const key_type &key)
	{
		Thread::RWLock::RDScoped l(locker);
		return confhash[section][key];
	}
	static Conf *GetInstance(const char *file = NULL)
	{
		if (file && _access(file, R_OK) == 0)
		{
			instance.filename = file;
			instance.reload();
		}
		return &instance; 
	}
};	

};

#endif

