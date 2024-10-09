
#include "Global.h"
#include "ProjList.h"
#include <AScriptFile.h>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ProjList::ProjList()
: m_iNumCol(0), m_iNumRow(0)
{
}

bool ProjList::Init()
{
	m_iNumCol = 0;
	m_iNumRow = 0;
	m_strList.clear();
	
	CString str;
	str.Format("%s%s\\ProjList.dat",g_szWorkDir,g_szEditMapDir);	
	AScriptFile File;
	if (File.Open(str))
	{
		if(File.GetNextToken(true))
		{
			CString strCol(File.m_szToken);
			if(strCol.Find("col=")!=-1)
			{
				strCol.Replace("col=","");
				m_iNumCol = (int)atof(strCol);
			}
			else 
			{
				m_iNumCol = 1;
				File.ResetScriptFile();
			}			
		}		
		while (File.GetNextToken(true))
		{
			m_strList.push_back(File.m_szToken);
		}		
		File.Close();
		if (m_iNumCol <= 0 || (m_strList.size() % m_iNumCol))
		{
			m_iNumCol = 0;
			m_strList.clear();
		}
	}
	if (IsEmpty()){
		m_iNumRow = 0;
	}else{
		m_iNumRow = (GetListCount() + GetNumCol() - 1)/GetNumCol();
	}
	return !IsEmpty();
}

bool ProjList::IsEmpty()const
{
	return GetNumCol() <= 0 || GetListCount() <= 0;
}

bool ProjList::GetRowCol(const char *szProj, int &iRow, int &iCol)const
{
	if (!IsEmpty()){
		const ProjNameList &l = GetList();
		const ProjNameList::const_iterator cit = std::find(l.begin(), l.end(), szProj);
		if (cit != l.end()){
			int index = std::distance(l.begin(), cit);
			iRow = index / GetNumCol();
			iCol = index % GetNumCol();
			return true;
		}
	}
	return false;
}