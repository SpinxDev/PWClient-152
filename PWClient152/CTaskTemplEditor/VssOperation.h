// VssOperation.h: interface for the CVssOperation class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _VSS_OPERATION_
#define _VSS_OPERATION_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef void CALLBACK CMDLOGFUNC(const char*szLog);

extern	char		g_strUser[MAX_PATH];
extern	char		g_strPwd[MAX_PATH];
extern	char		g_strVSSPath[MAX_PATH];
extern	BOOL		g_bAutoUpdate;
extern	BOOL		g_bCfgInit;

void LoadConfigs();
void SaveConfigs();

class CVssOperation  
{

public:
	CVssOperation();
	virtual ~CVssOperation();
	CString GetVssPath();
	
	void SetServerPath(CString szServerPath){ m_szServerPath = szServerPath; };
	void SetUserName(CString szUser){ m_szUserName = szUser; };
	void SetPassword(CString szPassWord) { m_szPassword = szPassWord; };
	void SetWorkFolder(CString szWorkingFolder){ m_szWorkingFolder = szWorkingFolder; };
	void InitVss( CMDLOGFUNC* pLogFunc);
	
	void RunStartCode();
	bool GetFile(CString szPath);
	void AddFile(const CString& szFileName);
	void AddAllFile(CString szPath);
	void CreateProject(CString szPath);
	void SetProjectPath(const CString& szPath);
	void GetAllFiles(CString szPath);
	void CheckInFile(const CString& szName);
	void CheckInAllFile(CString szPath);
	void CheckOutFile(const CString& szPathName);
	void CheckOutAllFile(CString szPath);
	void DeleteFile(CString szFileName);
	void UndoCheckOut(CString szName);
	bool IsCheckOutStatus(const CString& szFileName);
	void Property(CString szFileName);
	void UndoCheckOutFiles();
	CString GetFileTypes(CString szPathName);
	bool TestVssConnected(CString strMsg);
	
	inline CString GetLastCommandResult(){ return m_szResult; }
	inline bool IsInited(){ return m_bInit; }
	


protected:

	static DWORD WINAPI ReadAndHandleOutput(LPVOID param);
	void ProcessCommand(CString command);
	CString m_szResult;
	CString m_szUserName;
	CString m_szPassword;
	CString m_szServerPath;
	CString m_szWorkingFolder;
	CString m_szVssPath;
	CMDLOGFUNC *m_pLogFunc;
	bool m_bInit;

};

extern CVssOperation g_VSS;

#endif //_VSS_OPERATION_
