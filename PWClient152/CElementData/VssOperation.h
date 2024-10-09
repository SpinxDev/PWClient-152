// VssOperation.h: interface for the CVssOperation class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _VSS_OPERATION_
#define _VSS_OPERATION_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

typedef void CALLBACK CMDLOGFUNC(const TCHAR*szLog);

class CVssOperation  
{
	enum
	{
		CHECKED_IN,
		CHECKED_OUT
	};

public:
	CVssOperation();
	virtual ~CVssOperation();
	CString GetVssPath();
	
	void SetServerPath(const CString& szServerPath){ m_szServerPath = szServerPath; };
	void SetUserName(const CString& szUser){ m_szUserName = szUser; };
	void SetPassword(const CString& szPassWord) { m_szPassword = szPassWord; };
	void SetWorkFolder(const CString& szWorkingFolder){ m_szWorkingFolder = szWorkingFolder; };
	void InitVss( CMDLOGFUNC* pLogFunc,bool bClose = false);
	
	bool GetFile(const CString& szPath);
	void AddFile(const CString& szFileName);
	void AddAllFile(const CString& szPath);
	void CreateProject(const CString& szPath);
	void SetProjectPath(const CString& szPath);
	void GetAllFiles(const CString& szPath);
	void CheckInFile(const CString& szName);
	void CheckInAllFile(const CString& szPath);
	void CheckOutFile(const CString& szPathName);
	void CheckOutAllFile(const CString& szPath);
	void DeleteFile(const CString& szFileName);
	void UndoCheckOut(const CString& szName);
	bool IsCheckOutStatus(const CString& szFileName);
	void Property(const CString& szFileName);
	void UndoCheckOutFiles();
	void Rename(const CString szOldName, const CString szNewName);
	CString& GetFileTypes(const CString& szPathName);
	bool TestVssConnected(const CString& strMsg);
	
	inline CString& GetLastCommandResult(){ return m_szResult; }
	inline bool IsInited(){ return m_bInit; }
	void SetProperyToBinary(const CString& strFileName);
	bool IsFileExist(const CString& szFileName);	
	std::map<CString, int> GetFilesStatus(const CString& csPath);


protected:

	static DWORD WINAPI ReadAndHandleOutput(LPVOID param);
	void ProcessCommand(const CString& command);
	CString m_szResult;
	CString m_szUserName;
	CString m_szPassword;
	CString m_szServerPath;
	CString m_szWorkingFolder;
	CString m_szVssPath;
	CMDLOGFUNC *m_pLogFunc;
	bool m_bInit;
	std::map<CString, int> m_mapPath2Status;

};

#endif //_VSS_OPERATION_
