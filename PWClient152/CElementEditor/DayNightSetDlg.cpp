// DayNightSetDlg.cpp : implementation file
//

#include "Global.h"
#include "elementeditor.h"
#include "DayNightSetDlg.h"
#include "MainFrm.h"
#include "SceneObjectManager.h"
#include "SceneBoxArea.h"

//#define new A_DEBUG_NEW
/////////////////////////////////////////////////////////////////////////////
// CDayNightSetDlg dialog


CDayNightSetDlg::CDayNightSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDayNightSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDayNightSetDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDayNightSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDayNightSetDlg)
	DDX_Control(pDX, IDC_SLIDER_TIME, m_slider);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDayNightSetDlg, CDialog)
	//{{AFX_MSG_MAP(CDayNightSetDlg)
	ON_BN_CLICKED(IDC_BUTTON_DAY, OnButtonDay)
	ON_BN_CLICKED(IDC_BUTTON_NIGHT, OnButtonNight)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_TIME, OnReleasedcaptureSliderTime)
	ON_BN_CLICKED(IDC_BUTTON_RANGE_X, OnButtonRangeX)
	ON_BN_CLICKED(IDC_BUTTON_RANGE_Y, OnButtonRangeY)
	ON_BN_CLICKED(IDC_BUTTON_RANGE_Z, OnButtonRangeZ)
	ON_BN_CLICKED(IDC_BUTTON_RANGE_DIR, OnButtonRangeDir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDayNightSetDlg message handlers

void CDayNightSetDlg::OnButtonDay() 
{
	// TODO: Add your control notification handler code here
	g_fNightFactor = 0.0f;
	m_slider.SetPos(0);
	AUX_GetMainFrame()->SetFocus();

	if(AUX_GetMainFrame()->GetMap())
	{
		if( AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->GetCurrentArea() )
			AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->GetCurrentArea()->OnCamEntry(true);
	}
}

void CDayNightSetDlg::OnButtonNight() 
{
	// TODO: Add your control notification handler code here
	g_fNightFactor = 1.0f;
	m_slider.SetPos(100);
	AUX_GetMainFrame()->SetFocus();

	if(AUX_GetMainFrame()->GetMap())
	{
	if( AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->GetCurrentArea() )
		AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->GetCurrentArea()->OnCamEntry(true);
	}
}

BOOL CDayNightSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_slider.SetRange(0,100);
	m_slider.SetPos(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDayNightSetDlg::OnReleasedcaptureSliderTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	g_fNightFactor = (float)(m_slider.GetPos())/100.0f;
	AUX_GetMainFrame()->SetFocus();
	*pResult = 0;
	if(AUX_GetMainFrame()->GetMap())
	{
		if( AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->GetCurrentArea() )
		AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->GetCurrentArea()->OnCamEntry(true);
	}
}

void CDayNightSetDlg::OnButtonRangeX() 
{
	// TODO: Add your control notification handler code here
	AUX_GetMainFrame()->SetFocus();
	if(AUX_GetMainFrame()->GetMap())
	{
		if(AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetCount()>1)
		{
			if(IDNO==MessageBox("你确定要把选择的所有对象(X)对齐吗？",NULL,MB_YESNO)) return;
			AUX_GetMainFrame()->SetFocus();
			
			CSceneObject *pFirstObj = AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetHead();
			ALISTPOSITION pos = AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetTailPosition();
			while(pos)
			{
				CSceneObject *pObj = AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetPrev(pos);
				if(pObj!=pFirstObj)
				{
					A3DVECTOR3 vPos = pObj->GetPos();
					vPos.x = pFirstObj->GetPos().x;
					pObj->SetPos(vPos);
				}
			}
			AUX_GetMainFrame()->GetMap()->SetModifiedFlag(true);
		}
	}
}

void CDayNightSetDlg::OnButtonRangeY() 
{
	AUX_GetMainFrame()->SetFocus();
	// TODO: Add your control notification handler code here
	if(AUX_GetMainFrame()->GetMap())
	{
		if(AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetCount()>1)
		{
			if(IDNO==MessageBox("你确定要把选择的所有对象(Y)对齐吗？",NULL,MB_YESNO)) return;
			AUX_GetMainFrame()->SetFocus();

			CSceneObject *pFirstObj = AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetHead();
			ALISTPOSITION pos = AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetTailPosition();
			while(pos)
			{
				CSceneObject *pObj = AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetPrev(pos);
				if(pObj!=pFirstObj)
				{
					A3DVECTOR3 vPos = pObj->GetPos();
					vPos.y = pFirstObj->GetPos().y;
					pObj->SetPos(vPos);
				}
			}
			AUX_GetMainFrame()->GetMap()->SetModifiedFlag(true);
		}
	}
}

void CDayNightSetDlg::OnButtonRangeZ() 
{
	AUX_GetMainFrame()->SetFocus();
	// TODO: Add your control notification handler code here
	if(AUX_GetMainFrame()->GetMap())
	{
		if(AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetCount()>1)
		{
			if(IDNO==MessageBox("你确定要把选择的所有对象(Z)对齐吗？",NULL,MB_YESNO)) return;
			AUX_GetMainFrame()->SetFocus();

			CSceneObject *pFirstObj = AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetHead();
			ALISTPOSITION pos = AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetTailPosition();
			while(pos)
			{
				CSceneObject *pObj = AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetPrev(pos);
				if(pObj!=pFirstObj)
				{
					A3DVECTOR3 vPos = pObj->GetPos();
					vPos.z = pFirstObj->GetPos().z;
					pObj->SetPos(vPos);
				}
			}
			AUX_GetMainFrame()->GetMap()->SetModifiedFlag(true);
		}
	}
}

void CDayNightSetDlg::OnButtonRangeDir() 
{
	AUX_GetMainFrame()->SetFocus();
	// TODO: Add your control notification handler code here
	if(AUX_GetMainFrame()->GetMap())
	{
		if(AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetCount()>1)
		{
			if(IDNO==MessageBox("你确定要把选择的所有对象(角度)对齐吗？",NULL,MB_YESNO)) return;
			AUX_GetMainFrame()->SetFocus();
			
			CSceneObject *pFirstObj = AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetHead();
			A3DMATRIX4 matr = pFirstObj->GetRotateMatrix();
			float anglex, angley,anglez;
			float data[11] = { matr._11,matr._12,matr._13,matr._14,matr._21,matr._22,matr._23,matr._24,matr._31,matr._32,matr._33}; 
			AUX_ToEulerAnglesXYZ(data,anglex,angley,anglez);
			ALISTPOSITION pos = AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetTailPosition();
			while(pos)
			{
				CSceneObject *pObj = AUX_GetMainFrame()->GetMap()->GetSceneObjectMan()->m_listSelectedObject.GetPrev(pos);
				if(pObj!=pFirstObj)
				{
					pObj->ResetRotateMat();
					pObj->RotateX(anglex);
					pObj->RotateY(angley);
					pObj->RotateZ(anglez);
				}
			}
			AUX_GetMainFrame()->GetMap()->SetModifiedFlag(true);
		}
	}
}
