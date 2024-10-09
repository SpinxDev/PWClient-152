// DlgSceneObjectFinder.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "DlgSceneObjectFinder.h"
#include "MainFrm.h"

#include "ElementMap.h"
#include "SceneObjectManager.h"
#include "SceneObject.h"
#include "SceneAIGenerator.h"
#include "SceneGatherObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSceneObjectIDNameMatcher
class CSceneObjectIDNameMatcher{
public:
	virtual ~CSceneObjectIDNameMatcher(){}
	virtual bool IsMatch(int id, const AString *name)const=0;
};

/////////////////////////////////////////////////////////////////////////////
// CSceneObjectIDMatcher
class CSceneObjectIDMatcher : public CSceneObjectIDNameMatcher{
	int		m_id;
public:
	bool SetIDByString(const CString &str){
		bool result(false);
		if (!str.IsEmpty()){
			int id = atoi((LPCTSTR)str);			//	转换成整数
			CString strValidator;
			strValidator.Format("%d", id);
			if (!strValidator.Compare(str)){		//	再转换回字符串，比较两者是否相同，以确认输入正确
				m_id = atoi((LPCTSTR)str);
				result = true;
			}
		}
		return result;
	}
	virtual bool IsMatch(int id, const AString *name)const{
		return m_id > 0 && id == m_id;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CDlgSceneObjectFinder dialog

static CString s_strLastKey;


CDlgSceneObjectFinder::CDlgSceneObjectFinder(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSceneObjectFinder::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSceneObjectFinder)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDlgSceneObjectFinder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSceneObjectFinder)
	DDX_Control(pDX, IDC_STATIC_COUNT, m_staticCount);
	DDX_Control(pDX, IDC_EDIT_KEY, m_editKey);
	DDX_Control(pDX, IDC_LIST_OBJECT, m_listObject);
	//}}AFX_DATA_MAP
}

bool CDlgSceneObjectFinder::CanSelectSceneObject(CSceneObject *pObj, CSceneObjectIDNameMatcher *pMatcher)const{
	bool result(false);
	if (pObj){
		switch(pObj->GetObjectType())
		{
		case CSceneObject::SO_TYPE_AIAREA:
			result = CanSelect((CSceneAIGenerator *)(pObj), pMatcher);
			break;
		case CSceneObject::SO_TYPE_FIXED_NPC:
			result = CanSelect((CSceneFixedNpcGenerator *)(pObj), pMatcher);
			break;
		case CSceneObject::SO_TYPE_GATHER:
			result = CanSelect((CSceneGatherObject *)(pObj), pMatcher);
			break;
		}
	}
	return result;
}

bool CDlgSceneObjectFinder::CanSelect(CSceneAIGenerator *pObj, CSceneObjectIDNameMatcher *pMatcher)const{
	bool result(false);
	if (g_Configs.bPickAIArea && pObj && (pObj->GetCopyFlags()==g_Configs.nShowCopyNum)){
		for (int i(0); i < pObj->GetNPCNum(); ++ i){
			if (const NPC_PROPERTY *pNPC = pObj->GetNPCProperty(i)){
				if (pMatcher->IsMatch(pNPC->dwID, &pNPC->strNpcName)){
					result = true;
					break;
				}
			}
		}
	}
	return result;
}

bool CDlgSceneObjectFinder::CanSelect(CSceneFixedNpcGenerator *pObj, CSceneObjectIDNameMatcher *pMatcher)const{
	bool result(false);
	if (g_Configs.bPickFixedNpc && pObj && (pObj->GetCopyFlags()==g_Configs.nShowCopyNum)){
		const FIXED_NPC_DATA &pFixedNPC = pObj->GetProperty();
		if (pMatcher->IsMatch(pFixedNPC.dwID, &pFixedNPC.strNpcName)){
			result = true;
		}
	}
	return result;
}

bool CDlgSceneObjectFinder::CanSelect(CSceneGatherObject *pObj, CSceneObjectIDNameMatcher *pMatcher)const{
	bool result(false);
	if (g_Configs.bPickGather && pObj && (pObj->GetCopyFlags()==g_Configs.nShowCopyNum)){
		for (int i(0); i < pObj->GetGatherNum(); ++ i){
			if (const GATHER *pGather = pObj->GetGather(i)){
				if (pMatcher->IsMatch(pGather->dwId, &pGather->strName)){
					result = true;
					break;
				}
			}
		}
	}
	return result;
}

void CDlgSceneObjectFinder::RebuildSceneObjectList(){
	m_listObject.ResetContent();
	ALISTPOSITION pos = m_listSelectedObject.GetHeadPosition();
	while (pos){
		PSCENEOBJECT ptemp = m_listSelectedObject.GetNext(pos);
		m_listObject.AddString(ptemp->GetObjectName());
	}
}

void CDlgSceneObjectFinder::RefreshSceneObjectCount(){
	CString strCount;
	strCount.Format("%d", m_listSelectedObject.GetCount());
	m_staticCount.SetWindowText(strCount);
}

BEGIN_MESSAGE_MAP(CDlgSceneObjectFinder, CDialog)
	//{{AFX_MSG_MAP(CDlgSceneObjectFinder)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnButtonFind)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSceneObjectFinder message handlers

void CDlgSceneObjectFinder::GetSearchKey(CString &strKey)const{
	m_editKey.GetWindowText(strKey);
}

void CDlgSceneObjectFinder::SaveSearchKey(){
	GetSearchKey(s_strLastKey);
}

void CDlgSceneObjectFinder::LoadLastSearchKey(){
	m_editKey.SetWindowText(s_strLastKey);
}

void CDlgSceneObjectFinder::OnButtonFind() 
{
	m_listSelectedObject.RemoveAll();
	if (CElementMap *pMap = AUX_GetMainFrame()->GetMap()){
		CString strKey;
		GetSearchKey(strKey);
		CSceneObjectIDMatcher matcher;
		if (!matcher.SetIDByString(strKey)){
			m_editKey.SetFocus();
		}else{
			CSceneObjectManager *pMan = pMap->GetSceneObjectMan();
			ALISTPOSITION pos = pMan->m_listSceneObject.GetTailPosition();
			while (pos){
				PSCENEOBJECT ptemp = pMan->m_listSceneObject.GetPrev(pos);
				if (CanSelectSceneObject(ptemp, &matcher)){
					m_listSelectedObject.AddTail(ptemp);
				}
			}
		}
	}
	RebuildSceneObjectList();
	RefreshSceneObjectCount();
}

void CDlgSceneObjectFinder::OnOK() 
{
	SaveSearchKey();
	CDialog::OnOK();
}

BOOL CDlgSceneObjectFinder::OnInitDialog() 
{
	CDialog::OnInitDialog();
	LoadLastSearchKey();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSceneObjectFinder::OnCancel() 
{
	SaveSearchKey();
	CDialog::OnCancel();
}

BOOL CDlgSceneObjectFinder::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN){
		if (pMsg->hwnd == m_editKey.GetSafeHwnd()){
			OnButtonFind();
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
