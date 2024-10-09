// ObjectPropertyPanel.cpp : implementation file
//

#include "Global.h"
#include "elementeditor.h"
#include "ObjectPropertyPanel.h"
#include "MainFrm.h"
#include "ElementMap.h"
#include "SceneObjectManager.h"
#include "SceneObject.h"
#include "SceneWaterObject.h"
#include "UndoMan.h"
#include "UndoLightPropertyAction.h"


//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CObjectPropertyPanel dialog


CObjectPropertyPanel::CObjectPropertyPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CObjectPropertyPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectPropertyPanel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectPropertyPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectPropertyPanel)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectPropertyPanel, CDialog)
	//{{AFX_MSG_MAP(CObjectPropertyPanel)
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_BN_CLICKED(IDC_BUTTON_OBJECT_UPDATE, OnButtonObjectUpdate)
	ON_BN_CLICKED(IDC_BUTTON_OBJECT_RESET, OnButtonObjectReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectPropertyPanel message handlers

BOOL CObjectPropertyPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rc;
	CWnd * pRect = GetDlgItem(IDC_SCENE_OBJECT_PROPERTY_LIST);
	if(pRect)
	{
		pRect->GetWindowRect(rc);
		ScreenToClient(rc);
	}
	else
	{
		MessageBox(_T("不能创建属性列表"));
		assert(false && "不能创建属性列表");
		return false;
	}
	SetWindowText(_T("属性"));
	
	m_pList = new CPropertyList();
	BOOL bResult = m_pList->Create("属性表", WS_CHILD | WS_VISIBLE, rc, this, 101);
	ASSERT(bResult);
	//m_pList->AttachDataObject(m_pData);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CObjectPropertyPanel::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_pList)
	{
		m_pList->CleanItems();
		m_pList->DestroyWindow();
		delete m_pList;
		m_pList = NULL;
	}
}

void CObjectPropertyPanel::AttachDataObject(APropertyObject *pPropertyObject)
{
	m_pList->AttachDataObject(pPropertyObject);
}


void CObjectPropertyPanel::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);
	// TODO: Add your message handler code here
}

void CObjectPropertyPanel::OnButtonObjectUpdate() 
{
	// TODO: Add your control notification handler code here
	// 需要更新数据
	// 按次序取回数据
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject* pObj = pManager->m_listSelectedObject.GetHead();
			if(pObj->GetObjectType() == CSceneObject::SO_TYPE_LIGHT)
			{//这儿为灯光属性加UNDO
				CUndoLightPropertyAction *pUndo = new CUndoLightPropertyAction();
				pUndo->GetLightData((CSceneLightObject*)pObj);
				g_UndoMan.AddUndoAction(pUndo);
			}
			//应该还有其他对象的UNDO
			//………………………………
			
			
			//最后更新列表以及对象属性
			if(m_pList) 
				m_pList->UpdateData(true);
			pObj->UpdateProperty(true);
			if(pObj->GetObjectType()== CSceneObject::SO_TYPE_WATER)
			{
				((CSceneWaterObject*)pObj)->BuildRenderWater();
			}
			if(m_pList) 
				m_pList->UpdateData(false);
		}
	}
}

void CObjectPropertyPanel::OnButtonObjectReset() 
{
	// TODO: Add your control notification handler code here
	//不需要更新数据
	if(m_pList) m_pList->UpdateData(false);
}

void CObjectPropertyPanel::UpdatePropertyData(bool bGet)
{
	if(m_pList) 
		m_pList->UpdateData(bGet);
}
