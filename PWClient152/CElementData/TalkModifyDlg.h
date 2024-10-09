#if !defined(AFX_TALKMODIFYDLG_H__CDEAE070_3AD2_4D63_A388_4C9802FAFE49__INCLUDED_)
#define AFX_TALKMODIFYDLG_H__CDEAE070_3AD2_4D63_A388_4C9802FAFE49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TalkModifyDlg.h : header file
//
#include "vector.h"
#include "AString.h"
#include "AFile.h"
#include "XTreeCtrl.h"
#include "AObject.h"
#include "ExpTypes.h"

#define F_TALK_VERSION	 0x2;
/////////////////////////////////////////////////////////////////////////////
// Talk window struct

struct TALK_OPTION
{
	char strName[80];//为编辑用，该数据不需要导出
	AString strCommand;
	DWORD   dwID;//窗口的ID或是功能的ID
	DWORD   dwLinkID;//只有是该选项是服务才有效，
	                 //他表示某服务对应模板的ID,也可能是某种服务的参数
};

class TALK_WINDOW
{
public:
	TALK_WINDOW();
	virtual ~TALK_WINDOW();

	void		AddChild(TALK_OPTION *pTO);
	void        DeleteChild(DWORD id);
	void        DeleteAllChild();
	TALK_OPTION*GetChild(int index);
	int         GetChildNum();
	
	void		SetID(DWORD id);
	void        SetName(const AString name);
	void        SetDescribe(const AString desc);
	void        SetCommand(const AString command);
	
	DWORD       GetID();
	char*       GetName();
	AString     GetDescribe();
	AString     GetCommand();
	void        Release();

	bool        Save(AFile *pFile,DWORD dwVersion);
	bool        Load(AFile *pFile,DWORD dwVersion);
	
	
protected:
	CPtrList	listOptions;
	DWORD		dwID;
	char        strName[80];
	AString		strDescribe;
	AString     strCommand;

private:

};

/////////////////////////////////////////////////////////////////////////////
// Property table struct

struct TALK_ID_FUNCS : public CUSTOM_FUNCS
{
	~TALK_ID_FUNCS() {}
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

class CIDGenerator
{
public:
	CIDGenerator(){ m_nMax = 0; };
	~CIDGenerator(){};
	DWORD GenerateID(){ return ++m_nMax; };
	void  AddID(DWORD id){ if(id>m_nMax) m_nMax = id; };
	DWORD GetBaseData(){ return m_nMax; };
private:
	DWORD m_nMax;//已经产生的最大ID，该数据需要存盘
};

class CTalkModifyDlg : public CDialog
{
// Construction
public:
	CTalkModifyDlg(CWnd* pParent = NULL);   // standard constructor

	void ReBuildTalkWindow();//根据树列表控件重新生成各窗口之间的关系
	void UpdateMenu(CMenu *pMenu);
	void ReleaseWindow();
	bool LoadTalk();
	bool SaveTalk();
	TALK_WINDOW *GetWindow(CString strName);
	TALK_WINDOW *GetWindow(DWORD   id);
	TALK_OPTION *GetOption(CString strName);
	TALK_OPTION *GetOption(DWORD id);
	bool IsLeaf(CString name);
	bool FindName(CString name);
	
	bool ExportData(talk_proc* pData, const char* szTalkFile, unsigned int uID);
	void LoadTalkFromTalkProc(talk_proc* talk);

	CString  m_strPathName;

	talk_proc* init_talk;
	bool m_bPreset;
	DWORD m_dwSvrToSel;
	DWORD m_dwParamToSel;
	DWORD m_dwParamToSel2;

// Dialog Data
	//{{AFX_DATA(CTalkModifyDlg)
	enum { IDD = IDD_TALK_DIALOG };
	CXTreeCtrl	m_treeTalk;
	CString	m_strTalkName;
	DWORD	m_dwID;	//对话的ID,该ID必须是全编辑器唯一的.
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTalkModifyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bNewFile;
	void EnumTree(HTREEITEM hItem,TALK_WINDOW *pTalkWnd);
	void GetTreeData(HTREEITEM hItem,TALK_WINDOW *pTalkWnd);
	void DeleteItem(HTREEITEM hItem);
	void DeleteWindow(CString strName);
	void Repair_error1();//修补窗口数目不删除的错误(F_TALK_VERSION==0x1)
	bool FindIsUsed(TALK_WINDOW *pRoot,TALK_WINDOW *pTalkWnd);//查看该窗口是否被用
	void BackupData();
	void ReleaseBackupData();
	TALK_OPTION *GetOptionFromBackup(CString name);
	void FillWindowData(talk_proc::window*& pData, TALK_WINDOW* pW, unsigned int uParentID);

	CPtrList m_listTalkWindow;
	CPtrList m_listBackup;
	
	CIDGenerator m_IDGenerator;
	CImageList m_ImageList;
	

	void         FreshTreeView();
	// Generated message map functions
	//{{AFX_MSG(CTalkModifyDlg)
	afx_msg void OnRclickTreeTalk(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnCreateTalkWindow();
	afx_msg void OnButtonDeleteItem();
	afx_msg void OnButtonMoveItemUp();
	afx_msg void OnButtonMoveItemDown();
	afx_msg void OnModifyTalkWindow();
	afx_msg void OnButtonTalkTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TALKMODIFYDLG_H__CDEAE070_3AD2_4D63_A388_4C9802FAFE49__INCLUDED_)
