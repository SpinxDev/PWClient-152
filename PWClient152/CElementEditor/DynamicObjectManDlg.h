#if !defined(AFX_DYNAMICOBJECTMANDLG_H__6AB2C6FC_7BE0_4C68_85BE_B6FE6AA2F6CB__INCLUDED_)
#define AFX_DYNAMICOBJECTMANDLG_H__6AB2C6FC_7BE0_4C68_85BE_B6FE6AA2F6CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DynamicObjectManDlg.h : header file
//

#include "vector.h"
#include <hashmap.h>

/////////////////////////////////////////////////////////////////////////////
// CDynamicObjectManDlg dialog

struct DYNAMIC_OBJECT
{
	unsigned int id;
	char      szName[128];
	char      szObjectPath[256];
};


void InitDynamicObj(DYNAMIC_OBJECT * pObj);

class CDynamicObjectManDlg : public CDialog
{
// Construction
	enum
	{
		DYNAMIC_VERSION = 1,
	};
public:
	CDynamicObjectManDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDynamicObjectManDlg)
	enum { IDD = IDD_DIALOG_DYNAMIC_OBJECT_MAN };
	CStatic	m_FlagCtrl;
	CListCtrl	m_listObject;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDynamicObjectManDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	abase::vector<DYNAMIC_OBJECT> m_listDynamicObject;
	void FreshObjectList();
	void DelDynamicObj( unsigned int id);
	void GetDynamicObj( unsigned int id, DYNAMIC_OBJECT &obj);
	void SetDynamicObj( unsigned int id, char * szName, char * szPath);
	bool IsObjPathExsit(DYNAMIC_OBJECT* pObj);
	unsigned int m_uMaxID;
	bool Save();
	bool Load();
	bool Export(const char* szPath);

	bool m_bReadOnly;
	bool m_bChanged;
	HICON m_IconReadOnly;
	HICON m_IconWriteable;
	// Generated message map functions
	//{{AFX_MSG(CDynamicObjectManDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckOut();
	afx_msg void OnCheckIn();
	afx_msg void OnAddNewobject();
	afx_msg void OnDelObject();
	afx_msg void OnExport();
	afx_msg void OnDblclkListDynamicObject(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CDynamicObjectMan
{

public:
	CDynamicObjectMan(){}
	virtual ~CDynamicObjectMan(){Release(); }

protected:
	abase::hash_map<unsigned int , DYNAMIC_OBJECT> m_ObjectIDMap;
public:

	void GetObjByID(unsigned int ulID, DYNAMIC_OBJECT& obj)
	{
		abase::hash_map<unsigned int, DYNAMIC_OBJECT>::iterator it = m_ObjectIDMap.find(ulID);
		if(it == m_ObjectIDMap.end())
			return;
		obj.id = (it->second).id;
		strcpy(obj.szName, (it->second).szName);
		strcpy(obj.szObjectPath,(it->second).szObjectPath);
	}

	unsigned long GetIdNum()
	{
		return m_ObjectIDMap.size();
	}

	void GetObjByIndex( int idx, DYNAMIC_OBJECT& obj)
	{
		int index = 0;
		abase::hash_map<unsigned int, DYNAMIC_OBJECT>::iterator it = m_ObjectIDMap.begin();
		for( it; it != m_ObjectIDMap.end(); ++it)
		{
			if(index==idx)
			{
				obj.id = (it->second).id;
				strcpy(obj.szName, (it->second).szName);
				strcpy(obj.szObjectPath,(it->second).szObjectPath);
			}
			index++;
		}
	}
	
	void Release()
	{
		m_ObjectIDMap.clear();
	}

	//Return 0 if failed
	bool Load()
	{
		Release();
		
		FILE *pFile = fopen("ResTable\\DynamicObject.dat","rb");
		if(pFile==NULL) return false;
		DWORD dwVersion;
		fread(&dwVersion,sizeof(DWORD),1,pFile);
		if(dwVersion==1)
		{
			int size = 0;
			fread(&size,sizeof(DWORD),1,pFile);
			for( int i = 0; i < size; ++i)
			{
				DYNAMIC_OBJECT obj;
				fread(&obj.id,sizeof(unsigned int),1,pFile);
				fread(&obj.szName,128,1,pFile);
				fread(&obj.szObjectPath,256,1,pFile);
				m_ObjectIDMap[obj.id] = obj;
			}
		}
		fclose(pFile);
		return true;
	}
	
};

extern CDynamicObjectMan g_DynamicObjectMan;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DYNAMICOBJECTMANDLG_H__6AB2C6FC_7BE0_4C68_85BE_B6FE6AA2F6CB__INCLUDED_)
