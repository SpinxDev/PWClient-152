#ifndef _NPC_CONTOROLLER_MAN_DLG_
#define _NPC_CONTOROLLER_MAN_DLG_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DynamicObjectManDlg.h : header file
//

#include "vector.h"
#include <hashmap.h>

/////////////////////////////////////////////////////////////////////////////
// CDynamicObjectManDlg dialog

struct CONTROLLER_TIME 
{
	int nYear;//�������� [1900-2100] -1 ��������
	int nMouth;//����ȡֵ����[0,11]  -1 ��������
	int nWeek;//����ȡֵ����[0,6]  ������������ -1 ��������
	int nDay; //����ȡֵ����[1,31]  -1 ��������
	int nHours; //����ȡֵ����[0,23]  -1 ��������
	int nMinutes;//����ȡֵ����[0,59]
};

struct CONTROLLER_OBJECT
{
	unsigned int id;
	int       nControllerID;// 	��������ID�����ID�ɲ߻�����������ǰ��������IDһ�£����ڶԿ���������һЩ����
	char      szName[128];
	bool      bActived;				//�Ƿ��ʼʱ�ͼ���
	int       nWaitTime;			//�����ȴ�����ʱ��ſ�ʼ���ɶ���
	int       nStopTime;			//�����ȴ�����ʱ����Զ�ֹͣ���� int  �����������Ч
	
	bool      bActiveTimeInvalid;   
	bool      bStopTimeInvalid;
	CONTROLLER_TIME mActiveTime;	//�Ƿ���ڶ�ʱ�����ʱ��,�Լ�ʱ��ֵ�� ����ṹ 
	CONTROLLER_TIME mStopTime;    //�Ƿ���ڶ�ʱֹͣ��ʱ��, �Լ�ʱ��ֵ   ����ṹ
	int       nActiveTimeRange;   //�����ʱ�䷶Χֵ,��λ����
};

struct CONTROLLER_OBJECT_VERSION1
{
	unsigned int id;
	int       nControllerID;
	char      szName[128];
	bool      bActived;			
	int       nWaitTime;		
	int       nStopTime;		
	
	bool      bActiveTimeInvalid;   
	bool      bStopTimeInvalid;
	CONTROLLER_TIME mActiveTime;
	CONTROLLER_TIME mStopTime;    
};


void InitControllerObj(CONTROLLER_OBJECT * pObj);
void InitControllerTime(CONTROLLER_TIME* pObj);

class CNpcControllerManDlg : public CDialog
{
// Construction
	enum
	{
		CONTROLLER_VERSION = 2,
	};
public:
	CNpcControllerManDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDynamicObjectManDlg)
	enum { IDD = IDD_DIALOG_CONTROLLER_OBJECT_MAN };
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
	abase::vector<CONTROLLER_OBJECT> m_listControllerObject;
	void FreshObjectList();
	void DelDynamicObj( unsigned int id);
	void GetDynamicObj( unsigned int id, CONTROLLER_OBJECT &obj);
	void SetDynamicObj( unsigned int id, CONTROLLER_OBJECT &obj);
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
	afx_msg void OnDblclkListControllerObject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnExportTxt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CNpcControllerMan
{

public:
	CNpcControllerMan(){}
	virtual ~CNpcControllerMan(){Release(); }

protected:
	abase::hash_map<unsigned int , CONTROLLER_OBJECT> m_ObjectIDMap;
public:

	void GetObjByID(unsigned int ulID, CONTROLLER_OBJECT& obj)
	{
		abase::hash_map<unsigned int, CONTROLLER_OBJECT>::iterator it = m_ObjectIDMap.find(ulID);
		if(it == m_ObjectIDMap.end())
			return;
		obj = it->second;
	}

	unsigned long GetIdNum()
	{
		return m_ObjectIDMap.size();
	}

	void GetObjByIndex( int idx, CONTROLLER_OBJECT& obj)
	{
		int index = 0;
		abase::hash_map<unsigned int, CONTROLLER_OBJECT>::iterator it = m_ObjectIDMap.begin();
		for( it; it != m_ObjectIDMap.end(); ++it)
		{
			if(index==idx)
			{
				obj = (it->second);
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
		
		FILE *pFile = fopen("ResTable\\ControllerObject.dat","rb");
		if(pFile==NULL) return false;
		DWORD dwVersion;
		fread(&dwVersion,sizeof(DWORD),1,pFile);
		if(dwVersion==1)
		{
			int size = 0;
			fread(&size,sizeof(DWORD),1,pFile);
			for( int i = 0; i < size; ++i)
			{
				CONTROLLER_OBJECT_VERSION1 obj1;
				fread(&obj1,sizeof(CONTROLLER_OBJECT_VERSION1),1,pFile);
				
				CONTROLLER_OBJECT obj;
				memcpy(&obj,&obj1,sizeof(CONTROLLER_OBJECT_VERSION1));
				obj.nActiveTimeRange = 60;
				m_ObjectIDMap[obj.id] = obj;
			}	
		}else if(dwVersion==2)
		{
			int size = 0;
			fread(&size,sizeof(DWORD),1,pFile);
			for( int i = 0; i < size; ++i)
			{
				CONTROLLER_OBJECT obj;
				fread(&obj,sizeof(CONTROLLER_OBJECT),1,pFile);
				m_ObjectIDMap[obj.id] = obj;
			}
		}
		fclose(pFile);
		return true;
	}
	
};

extern CNpcControllerMan g_ControllerObjectMan;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DYNAMICOBJECTMANDLG_H__6AB2C6FC_7BE0_4C68_85BE_B6FE6AA2F6CB__INCLUDED_)
