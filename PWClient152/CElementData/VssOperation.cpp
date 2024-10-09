// VssOperation.cpp: implementation of the CVssOperation class.
//
//////////////////////////////////////////////////////////////////////
#include <afx.h>
#include <Afxwin.h>
#include "VssOperation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CString GetLine(const CString& cStr, int& nPos)
{
	CString csReturn;
	int nNext = cStr.Find(_T('\n'), nPos + 1);
	if (nNext == -1)
	{
		csReturn = cStr.Right(cStr.GetLength() - nPos-1);
	}
	else
	{
		csReturn = cStr.Mid(nPos+1, nNext - nPos -1);
	}	
	nPos = nNext;
	return csReturn;
}

CVssOperation::CVssOperation()
{
	m_bInit = false;
	m_pLogFunc = NULL;
}

CVssOperation::~CVssOperation()
{

}

CString CVssOperation::GetVssPath()
{
	static TCHAR strNull[2] = _T("");
	static TCHAR strPath[MAX_PATH];
	DWORD dwType;
	DWORD dwSize = MAX_PATH;
	HKEY  hKey;

	const TCHAR * vss_path[] = 
	{
		_T("Software\\Microsoft\\VisualStudio\\8.0\\setup\\vs\\vss\\"),
		_T("Software\\Microsoft\\VisualStudio\\6.0\\setup\\Microsoft Visual SourceSafe\\"),
		NULL
	};

	int i = 0;
	LONG lResult;
	while (vss_path[i])
	{
		lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, vss_path[i],0, KEY_READ, &hKey );
		if (lResult == ERROR_SUCCESS)
		{
			break;
		}
		++i;
	}

	if (lResult != ERROR_SUCCESS)
	{
		return strNull;
	}

	lResult = RegQueryValueEx( hKey, _T("ProductDir"), NULL,
		&dwType, (BYTE*)strPath, &dwSize );
	RegCloseKey( hKey );

	if( ERROR_SUCCESS != lResult )
		return strNull;
	CString path(strPath);

	if (i== 1)
	{//version 6.0
		path += "\\win32\\";
	}

	return path;
}

void CVssOperation::InitVss(CMDLOGFUNC* pLogFunc,bool bClose)
{
	if(bClose)
	{
		m_bInit = false;
		return;
	}

	CString path = GetVssPath();
	if(path.IsEmpty())
	{
		AfxMessageBox(_T("没有安装 Microsoft Visual SourceSafe！"));
		m_bInit = false;
		exit(0);
	}
	m_szVssPath = path + _T("ss");
	m_bInit = true;
	m_pLogFunc = pLogFunc;
}


void CVssOperation::GetAllFiles(const CString& szPath)
{
	if(!m_bInit) return;

	CString CommandLine;
	CommandLine.Format(_T(" get -GL\"%s \" \"$/%s\" -R -I-"),m_szWorkingFolder,szPath);
	ProcessCommand(CommandLine);
}

bool CVssOperation::GetFile(const CString& szPath)
{
	if(!m_bInit) return false;

	CString CommandLine;
	CommandLine.Format(_T(" get -GL\"%s \" \"$/%s\" -I-"),m_szWorkingFolder,szPath);
	ProcessCommand(CommandLine);
	if(m_szResult.Find(szPath,0)==-1)
		return false;
	return true;
}

void CVssOperation::AddFile(const CString& szFileName)
{
	if(!m_bInit) return;
	CString CommandLine;
	CommandLine.Format(_T(" Add \"%s\" -B -I-Y"),szFileName);//-B Assume the file is binary
	ProcessCommand(CommandLine);
}

void CVssOperation::AddAllFile(const CString& szPath)
{
	if(!m_bInit) return;
	CString CommandLine;
	CommandLine.Format(_T(" Add \"%s\" -R* -B -I-Y"),szPath);//-B Assume the file is binary
	ProcessCommand(CommandLine);
}

void CVssOperation::DeleteFile(const CString& szFileName)
{
	if(!m_bInit) return;

	CString CommandLine;
	CommandLine.Format(_T(" Destroy \"%s\" -I-Y"),szFileName);
	ProcessCommand(CommandLine);
}

void CVssOperation::CreateProject(const CString& szPath)
{
	if(!m_bInit) return;
	CString CommandLine;
	CommandLine.Format(_T(" Create \"%s\" -I-Y"),szPath);
	ProcessCommand(CommandLine);
}

void CVssOperation::SetProjectPath(const CString& szPath)
{
	if(!m_bInit) return;
	m_szWorkingFolder = szPath;
	CString CommandLine;
	CommandLine.Format(_T(" Cp \"$/%s\" -I-Y"),szPath);
	ProcessCommand(CommandLine);
}

void CVssOperation::CheckInFile(const CString& szName)
{
	if(!m_bInit) return;

	CString CommandLine;
	CommandLine.Format(_T(" checkin \"%s\" -GL\"%s \" -I-Y"),szName,m_szWorkingFolder);
	ProcessCommand(CommandLine);
}

void CVssOperation::CheckInAllFile(const CString& szPath)
{
	if(!m_bInit) return;

	CString CommandLine;
	CommandLine.Format(_T(" checkin \"%s\" -P -GL\"%s \" -I-Y"),_T("*"),szPath);
	ProcessCommand(CommandLine);
}

void CVssOperation::CheckOutFile(const CString& szPathName)
{
	if(!m_bInit) return;
	CString CommandLine;
	CommandLine.Format(_T(" checkout -GL\"%s \" \"$/%s\" -I-Y"),m_szWorkingFolder,szPathName);
	ProcessCommand(CommandLine);
}

void CVssOperation::CheckOutAllFile(const CString& szPath)
{
	if(!m_bInit) return;
	CString CommandLine;
	CommandLine.Format(_T(" checkout -R-GL\"%s \" \"$/%s\" -I-Y"),m_szWorkingFolder,szPath);
	ProcessCommand(CommandLine);
}

void CVssOperation::UndoCheckOut(const CString& szName)
{
	if(!m_bInit) return;
	CString CommandLine;
	CommandLine.Format(_T(" undocheckout -GL\"%s \" \"$/%s\" -I-Y"),m_szWorkingFolder,szName);
	ProcessCommand(CommandLine);	
}

bool CVssOperation::IsCheckOutStatus(const CString& szFileName)
{
	CString CommandLine;
	CommandLine.Format(_T(" Status \"$/%s\" -U\"%s\" -I-Y"),szFileName,m_szUserName);
	ProcessCommand(CommandLine);
	if(m_szResult.Find(_T("No files found checked out"),0)!=-1)
		return false;
	return true;
}

void CVssOperation::Property(const CString& szFileName)
{
	CString CommandLine;
	CommandLine.Format(_T(" Properties \"$/%s\" -I-Y"),szFileName);
	ProcessCommand(CommandLine);
}

void CVssOperation::UndoCheckOutFiles()
{
	if(!m_bInit) return;
	CString CommandLine;
	CommandLine.Format(_T(" Undocheckout * -P -I-Y"));
	ProcessCommand(CommandLine);
}

CString& CVssOperation::GetFileTypes(const CString& szPathName)
{

	if(!m_bInit) return m_szResult;
	CString CommandLine;
	CommandLine.Format(_T(" filetype \"%s\" -I-Y"), szPathName);
	ProcessCommand(CommandLine);
	return m_szResult;
}

bool CVssOperation::TestVssConnected(const CString& strMsg)
{
	CString strNOVSS=_T("No VSS database");
	if(strMsg.Find(strNOVSS,0)==-1) return true;
	return false;
}

void CVssOperation::SetProperyToBinary(const CString& strFileName)
{
	CString CommandLine;
	CommandLine.Format(_T(" Properties -B \"$/%s\" -I-Y"),strFileName);
	ProcessCommand(CommandLine);
}

bool CVssOperation::IsFileExist(const CString& szFileName)
{
	CString CommandLine;
	CommandLine.Format(_T(" Directory \"$/%s\" -I-Y"),szFileName);
	ProcessCommand(CommandLine);
	if(m_szResult.Find(_T("is not an existing filename or project"),0)!=-1)
		return false;
	return true;
}

void CVssOperation::Rename(const CString szOldName, const CString szNewName)
{
	CString CommandLine;
	CommandLine.Format(_T(" Rename \"$/%s\" \"$/%s\" -I-Y"),szOldName, szNewName);
	ProcessCommand(CommandLine);
}

struct DOUBLEHANDEL
{
	HANDLE hPipe;
	HANDLE hProcess;
	CString strResult;
};

DWORD WINAPI CVssOperation::ReadAndHandleOutput(LPVOID param)
{
	DWORD nBytesRead;
	char* szBuffer;
	HANDLE hRead = ((DOUBLEHANDEL*)param)->hPipe;
	HANDLE hProcess = ((DOUBLEHANDEL*)param)->hProcess;
	while(TRUE)
	{
		//use PeekNamedPipe to avoid dead lock.
		DWORD dwTotalBytes=0;
		DWORD dwTotalLeft=0;
		BOOL returnValue= PeekNamedPipe(hRead, NULL, 0,\
			&nBytesRead, &dwTotalBytes, &dwTotalLeft);
		szBuffer = new char[dwTotalBytes+1];
		ZeroMemory(szBuffer, dwTotalBytes+1);
		returnValue= PeekNamedPipe(hRead, szBuffer, dwTotalBytes+1,\
			&nBytesRead, &dwTotalBytes, &dwTotalLeft);
		if (returnValue) 
		{
			if (dwTotalBytes!=0) 
			{				
				if (ReadFile(hRead, szBuffer, strlen(szBuffer), &nBytesRead, NULL) \
					&& nBytesRead)
				{
					((DOUBLEHANDEL*)param)->strResult += szBuffer;
					CString msg = szBuffer;					
					if(msg.Find(_T("Username"))>=0 && msg.Find(_T("Password"))>=0)
					{
						TerminateProcess(hProcess,1);
						if (szBuffer)
						{
							delete []szBuffer;
							szBuffer = 0;
						}
						return 1;
					}
				}
			}
		}

		DWORD dwExitCode;
		if(GetExitCodeProcess(hProcess,&dwExitCode))
		{
			if(dwExitCode==STILL_ACTIVE)
			{
				Sleep(100);
			}
			else
			{
				if (szBuffer)
				{
					delete []szBuffer;
					szBuffer = 0;
				}
				return 1;
			}
		}
		if (szBuffer)
		{
			delete []szBuffer;
			szBuffer = 0;
		}
	}
	return 1;
}


void CVssOperation::ProcessCommand(const CString& command)
{
	m_szResult = "";

	//create pipe
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 

	HANDLE hRead1,hWrite1,hRead2,hWrite2;
	if (!CreatePipe(&hRead1, &hWrite1, &saAttr, 1024*1024*4) || !CreatePipe(&hRead2, &hWrite2, &saAttr, 1024*1024*4))
		return;


	//create process
	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES ;
	si.hStdOutput = hWrite1; 
	si.hStdInput  = hRead2;
	si.hStdError = hWrite1; 

	PROCESS_INFORMATION pi;
	TCHAR* buf = new TCHAR[command.GetLength()+1];
	_tcscpy(buf, command);

	VERIFY(CreateProcess(
		m_szVssPath + ".exe",
		buf,
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi));

	DWORD ThreadId;
	DOUBLEHANDEL dHandle;
	dHandle.hPipe = hRead1;
	dHandle.hProcess = pi.hProcess;
	HANDLE hThread = CreateThread(NULL, 0, ReadAndHandleOutput, (LPVOID)&dHandle, 0, &ThreadId);
	if (hThread == NULL) 
	{
		TerminateProcess(pi.hProcess,1);
		CloseHandle(hRead1);
		CloseHandle(hWrite1);
		CloseHandle(hRead2);
		CloseHandle(hWrite2);
		if (buf)
		{
			delete []buf;
			buf = 0;
		}
		return;
	}

	WaitForSingleObject(hThread,INFINITE);

	m_szResult = dHandle.strResult;
	if(m_pLogFunc) 
	{
		CString strTemp;
		strTemp.Format( _T("Vss command: \"%s\""), buf);
		m_pLogFunc(strTemp);
		if(m_szResult.GetLength() < 1024) m_pLogFunc(m_szResult);
		else m_pLogFunc(_T("......OK"));
	}

	CloseHandle(hRead1);
	CloseHandle(hWrite1);
	CloseHandle(hRead2);
	CloseHandle(hWrite2);
	if (buf)
	{
		delete []buf;
		buf = 0;
	}
}

std::map<CString, int> CVssOperation::GetFilesStatus(const CString& csPath)
{
	m_mapPath2Status.clear();
	CString csFolderPath = csPath;
	if(csPath.Right(1) == _T("\\"))
	{
		csFolderPath = csPath.Left(csPath.GetLength()-1);
	}
	CString CommandLine;
	CommandLine.Format(_T(" dir \"$/%s\" -I-Y"),csFolderPath);
	ProcessCommand(CommandLine);
	CString csResult(m_szResult);
	int nPos = -1;
	do 
	{
		CString csLine(GetLine(csResult, nPos));
		csLine.TrimLeft();
		csLine.TrimRight();
		if (csLine.GetLength() != 0 && csLine.Find(_T("item(s)")) == -1 && csLine.Right(1) != _T(":")  && csLine.Find(_T("No items found")) == -1)
		{
			if (csLine.GetAt(0) == _T('$'))
			{
				csLine = csLine.Right(csLine.GetLength()-1);
			}
			CString csFullPath = csFolderPath + _T("\\") + csLine;
			csFullPath.MakeLower();
			m_mapPath2Status[csFullPath] = CHECKED_IN;
		}		
	} while(nPos != -1);
	CommandLine.Format(_T(" Status \"$/%s\" -U\"%s\" -I-Y"),csFolderPath, m_szUserName);
	ProcessCommand(CommandLine);
	csResult = m_szResult;
	if (csResult.Find(_T("No files found checked out")) != -1)
	{
		return m_mapPath2Status;
	}
	nPos = -1;
	do 
	{
		CString csLine(GetLine(csResult, nPos));
		csLine.TrimLeft();
		csLine.TrimRight();
		int nBlank = csLine.Find(_T(" "));
		if(nBlank != -1)
		{
			CString csFileName = csLine.Left(nBlank);
			CString csFullPath = csFolderPath + _T("\\") + csFileName;
			csFullPath.MakeLower();
			std::map<CString,int>::iterator it = m_mapPath2Status.find(csFullPath);
			if (it == m_mapPath2Status.end())
			{
				for (std::map<CString,int>::iterator it = m_mapPath2Status.begin(); it != m_mapPath2Status.end(); ++it)
				{
					if(it->first.Find(csFullPath) != -1)
					{
						m_mapPath2Status[it->first] = CHECKED_OUT;
					}
				}
			}
			else
			{				
				m_mapPath2Status[csFullPath] = CHECKED_OUT;
			}			
		}
	} while(nPos != -1);
	return m_mapPath2Status;
}