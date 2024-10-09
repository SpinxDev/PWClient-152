#pragma once

#include <afx.h>
#include <vector>

class ProjList
{	
public:
	typedef std::vector<CString> ProjNameList;

private:
	int				m_iNumCol;
	int				m_iNumRow;
	ProjNameList	m_strList;

public:
	ProjList();
	bool	Init();
	int		GetListCount()const{ return (int)GetList().size();}
	int		GetNumCol()const{ return m_iNumCol; }
	int		GetNumRow()const{ return m_iNumRow; }
	const ProjNameList & GetList()const{ return m_strList; }
	const CString & Get(int index)const{ return m_strList[index]; }
	bool	IsEmpty()const;
	bool	GetRowCol(const char *szProj, int &iRow, int &iCol)const;
};