// BezierMergeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "ElementMap.h"
#include "SceneObject.h"
#include "MainFrm.h"
#include "SceneObjectManager.h"
#include "EditerBezier.h"
#include "Render.h"
#include "BezierMergeDlg.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CBezierMergeDlg dialog


CBezierMergeDlg::CBezierMergeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBezierMergeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBezierMergeDlg)
	m_strBezierA = _T("");
	m_strBezierB = _T("");
	//}}AFX_DATA_INIT
}


void CBezierMergeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBezierMergeDlg)
	DDX_Text(pDX, IDC_EDIT_BEZIERA, m_strBezierA);
	DDX_Text(pDX, IDC_EDIT_BEZIERB, m_strBezierB);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBezierMergeDlg, CDialog)
	//{{AFX_MSG_MAP(CBezierMergeDlg)
	ON_BN_CLICKED(IDC_RADIO_LINK1, OnRadioLink1)
	ON_BN_CLICKED(IDC_RADIO_LINK2, OnRadioLink2)
	ON_BN_CLICKED(IDC_RADIO_LINK3, OnRadioLink3)
	ON_BN_CLICKED(IDC_RADIO_LINK4, OnRadioLink4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBezierMergeDlg message handlers

BOOL CBezierMergeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_nLink = 1;
	CButton *pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK1);
	if(pButton) pButton->SetCheck(true);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK2);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK3);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK4);
	if(pButton) pButton->SetCheck(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBezierMergeDlg::OnRadioLink1() 
{
	// TODO: Add your control notification handler code here
	m_nLink = 1;
	CButton *pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK2);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK3);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK4);
	if(pButton) pButton->SetCheck(false);
}

void CBezierMergeDlg::OnRadioLink2() 
{
	// TODO: Add your control notification handler code here
	m_nLink = 2;
	CButton *pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK1);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK3);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK4);
	if(pButton) pButton->SetCheck(false);
}

void CBezierMergeDlg::OnRadioLink3() 
{
	// TODO: Add your control notification handler code here
	m_nLink = 3;
	CButton *pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK1);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK2);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK4);
	if(pButton) pButton->SetCheck(false);
}

void CBezierMergeDlg::OnRadioLink4() 
{
	// TODO: Add your control notification handler code here
	m_nLink = 4;
	CButton *pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK1);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK2);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_LINK3);
	if(pButton) pButton->SetCheck(false);

}

void CBezierMergeDlg::UpdateUI()
{
	CButton *pButton = (CButton*)GetDlgItem(IDOK);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(pSManager->m_listSelectedObject.GetCount() !=2 ) goto error;
		CSceneObject *pobja = pSManager->m_listSelectedObject.GetTail();
		CSceneObject *pobjb = pSManager->m_listSelectedObject.GetHead();
		if(pobja->GetObjectType() != CSceneObject::SO_TYPE_BEZIER
			|| pobjb->GetObjectType() != CSceneObject::SO_TYPE_BEZIER) goto error;
		
		if(((CEditerBezier*)pobja)->IsBezier() != ((CEditerBezier*)pobjb)->IsBezier()) goto error;
		m_strBezierA = pobja->GetObjectName();
		m_strBezierB = pobjb->GetObjectName();
		if(pButton) pButton->EnableWindow(true);
		UpdateData(false);
		return;	
	}
error:
	if(pButton) pButton->EnableWindow(false);
}

void CBezierMergeDlg::OnOK()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(pSManager->m_listSelectedObject.GetCount() !=2 ) return;
		CSceneObject *pobja = pSManager->m_listSelectedObject.GetTail();
		CSceneObject *pobjb = pSManager->m_listSelectedObject.GetHead();
		if(pobja->GetObjectType() != CSceneObject::SO_TYPE_BEZIER
			|| pobjb->GetObjectType() != CSceneObject::SO_TYPE_BEZIER) return;

		if(((CEditerBezier*)pobja)->IsBezier() != ((CEditerBezier*)pobjb)->IsBezier()) return;
	
		CEditerBezier *pBezierNew = MergeBezier((CEditerBezier*)pobja,(CEditerBezier*)pobjb);
		if(pBezierNew) pSManager->InsertSceneObject(pBezierNew);
		AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
	}
	
	CDialog::OnOK();
}

CEditerBezier* CBezierMergeDlg::MergeBezier(CEditerBezier *pBezierA,CEditerBezier *pBezierB)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();

	
	CEditerBezier *pNew = new CEditerBezier(0,g_Render.GetA3DDevice());
	pNew->SetObjectID(pSManager->GenerateObjectID());
	if(pNew==NULL) return NULL;
	
	// Copy property data
	DWORD dwClr;
	pBezierA->GetProperty(dwClr);
	pNew->SetProperty(dwClr);
	pNew->m_float_anchorsize = pBezierA->m_float_anchorsize;
	pNew->m_float_pointradius = pBezierA->m_float_pointradius;
	pNew->m_float_segradius = pBezierA->m_float_segradius;
	pNew->SetObjectName(pBezierA->GetObjectName());
	pNew->SetBezier(pBezierA->IsBezier());
	
	// Link bezier
	POSITION pos;
	int nPoint = 0;
	switch(m_nLink)
	{
	case 1://1 head to 2 tail
		pos =  pBezierA->m_list_points.GetTailPosition();
		while(pos)
		{
			CEditerBezierPoint *pPt = (CEditerBezierPoint *)pBezierA->m_list_points.GetPrev(pos);
			pNew->CreatePoint(pPt->GetPos(),0,nPoint);
			nPoint ++;
		}

		pos =  pBezierB->m_list_points.GetTailPosition();
		while(pos)
		{
			CEditerBezierPoint *pPt = (CEditerBezierPoint *)pBezierB->m_list_points.GetPrev(pos);
			pNew->CreatePoint(pPt->GetPos(),0,nPoint);
			nPoint ++;
		}
		pSManager->DeleteSceneObjectForever(pBezierA);
		pSManager->DeleteSceneObjectForever(pBezierB);
		break;
	case 2://1 head to 2 head
		pos =  pBezierA->m_list_points.GetTailPosition();
		while(pos)
		{
			CEditerBezierPoint *pPt = (CEditerBezierPoint *)pBezierA->m_list_points.GetPrev(pos);
			pNew->CreatePoint(pPt->GetPos(),0,nPoint);
			nPoint ++;
		}

		pos =  pBezierB->m_list_points.GetHeadPosition();
		while(pos)
		{
			CEditerBezierPoint *pPt = (CEditerBezierPoint *)pBezierB->m_list_points.GetNext(pos);
			pNew->CreatePoint(pPt->GetPos(),0,nPoint);
			nPoint ++;
		}
		pSManager->DeleteSceneObjectForever(pBezierA);
		pSManager->DeleteSceneObjectForever(pBezierB);
		break;
	case 3://tail and tail
		pos =  pBezierA->m_list_points.GetHeadPosition();
		while(pos)
		{
			CEditerBezierPoint *pPt = (CEditerBezierPoint *)pBezierA->m_list_points.GetNext(pos);
			pNew->CreatePoint(pPt->GetPos(),0,nPoint);
			nPoint ++;
		}

		pos =  pBezierB->m_list_points.GetTailPosition();
		while(pos)
		{
			CEditerBezierPoint *pPt = (CEditerBezierPoint *)pBezierB->m_list_points.GetPrev(pos);
			pNew->CreatePoint(pPt->GetPos(),0,nPoint);
			nPoint ++;
		}
		pSManager->DeleteSceneObjectForever(pBezierA);
		pSManager->DeleteSceneObjectForever(pBezierB);
		break;
	case 4:
		pos =  pBezierA->m_list_points.GetHeadPosition();
		while(pos)
		{
			CEditerBezierPoint *pPt = (CEditerBezierPoint *)pBezierA->m_list_points.GetNext(pos);
			pNew->CreatePoint(pPt->GetPos(),0,nPoint);
			nPoint ++;
		}

		pos =  pBezierB->m_list_points.GetHeadPosition();
		while(pos)
		{
			CEditerBezierPoint *pPt = (CEditerBezierPoint *)pBezierB->m_list_points.GetNext(pos);
			pNew->CreatePoint(pPt->GetPos(),0,nPoint);
			nPoint ++;
		}
		pSManager->DeleteSceneObjectForever(pBezierA);
		pSManager->DeleteSceneObjectForever(pBezierB);
		break;
	}
	return pNew;
}
