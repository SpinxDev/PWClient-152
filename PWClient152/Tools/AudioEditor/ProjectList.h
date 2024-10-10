#pragma once

#include <vector>

typedef std::vector<CString> ProjectNameArray;

class ProjectList
{
protected:
	ProjectList(void);
	~ProjectList(void);
public:
	bool				LoadXML(const char* szPath);
	bool				SaveXML(const char* szPath);
	bool				Add(const char* szName);
	bool				Delete(const char* szName);
	bool				IsExist(const char* szName);
	static ProjectList&	GetInstance();
	const char*			GetName(int idx) const { return m_arrProjectName[idx]; }
	int					GetNum() const { return (int)m_arrProjectName.size(); }
protected:
	void				release();
protected:
	ProjectNameArray	m_arrProjectName;
};
