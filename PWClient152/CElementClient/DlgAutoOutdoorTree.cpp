/*
 * FILE: DlgAutoOutdoorTree.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/3/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
/*
#include "DlgAutoOutdoorTree.h"
#include "DlgAutoAllRes.h"
#include "EC_HomeDlgsMgr.h"
#include "DlgAutoNote2.h"
#include "DlgAutoSwitchDlg.h"

#include "aui\\AUILabel.h"
#include "aui\\AUISlider.h"
#include "aui\\AUIListBox.h"
#include "aui\\AUIManager.h"
#include "aui\\AUIStillImageButton.h"

#include "A3DMacros.h"

#include "AutoForestPoly.h"
#include "AutoForestLine.h"
#include "AutoSelectedArea.h"
#include "AutoTerrainCommon.h"
#include "AutoTerrainAction.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoOutdoorTree, CDlgHomeBase)

AUI_ON_COMMAND("AddTreeInfo", OnCommandAddTreeInfo)
AUI_ON_COMMAND("DelTreeInfo", OnCommandDelTreeInfo)
AUI_ON_COMMAND("ClearAllTreeInfo", OnCommandClearAllTreeInfo)

AUI_ON_COMMAND("EditTreePoly", OnCommandEditTreePoly)
AUI_ON_COMMAND("EditTreeLine", OnCommandEditTreeLine)

AUI_ON_COMMAND("CreateTree", OnCommandCreateTree)

AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoOutdoorTree, CDlgHomeBase)

AUI_ON_EVENT( "ListBox_TreeSet", WM_LBUTTONDBLCLK, OnEventDblClkTreeSetList)
AUI_ON_EVENT( "ListBox_TreePlant", WM_LBUTTONDBLCLK, OnEventDblClkTreePlantList)

AUI_END_EVENT_MAP()

CDlgAutoOutdoorTree::CDlgAutoOutdoorTree()
{
	m_pSliderTreeSeed = NULL;
	m_pSliderTreeDensity = NULL;
	m_pSliderTreeSlope = NULL;
	m_pSliderTreeSpaceX = NULL;
	m_pSliderTreeSpaceZ = NULL;
	m_pSliderTreeSpaceLine = NULL;
	m_pSliderTreeSpaceNoise = NULL;
	m_pSliderTreeWeight = NULL;


	m_pLabelTreeSeed = NULL;
	m_pLabelTreeDensity = NULL;
	m_pLabelTreeSlope = NULL;
	m_pLabelTreeSpaceX = NULL;
	m_pLabelTreeSpaceZ = NULL;
	m_pLabelTreeSpaceLine = NULL;
	m_pLabelTreeSpaceNoise = NULL;
	m_pLabelTreeWeight = NULL;

	m_nTreeSeed = 0;
	m_nTreeDensity = 0;
	m_nTreeSlope = 0;
	m_nTreeSpaceX = 0;
	m_nTreeSpaceZ = 0;
	m_nTreeSpaceLine = 0;
	m_nTreeSpaceNoise = 0;
	m_nTreeWeight = 0;

	m_fTreeWeight = 0.0f;

	m_pListBoxTreeSet = NULL;
	m_pListBoxTreePlant = NULL;

	m_bCheckTreeRegular = false;

	m_pTreeInfo = NULL;
	m_nNumTreeInfos = 0;
	
	m_pBtnCreate = NULL;

	for( int i = 0; i < B_NUM; i++)
	{
		m_pBtns[i] = NULL;
	}

	m_nActionCount2 = 1;

}

CDlgAutoOutdoorTree::~CDlgAutoOutdoorTree()
{
}

AUIStillImageButton * CDlgAutoOutdoorTree::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnOutdoorTree;
}

void CDlgAutoOutdoorTree::OnShowDialog()
{
	CDlgHomeBase::OnShowDialog();
	SetCanMove(false);
	
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}
bool CDlgAutoOutdoorTree::OnInitContext()
{
	if( !InitResource())
		return false;

	SetSliderBound();
	LoadDefault();
	UpdateData(false);
	SetLabelsText();

	return true;
}

#define GET_BTN_POINTER(a) DDX_Control( "BTN_" # a, m_pBtns[a]);
void CDlgAutoOutdoorTree::DoDataExchange(bool bSave)
{
	DDX_Control("Slider_TreeSeed", m_pSliderTreeSeed);
	DDX_Control("Slider_TreeDensity", m_pSliderTreeDensity);
	DDX_Control("Slider_TreeSlope", m_pSliderTreeSlope);
	DDX_Control("Slider_TreeSpaceX", m_pSliderTreeSpaceX);
	DDX_Control("Slider_TreeSpaceZ", m_pSliderTreeSpaceZ);
	DDX_Control("Slider_TreeSpaceLine", m_pSliderTreeSpaceLine);
	DDX_Control("Slider_TreeSpaceNoise", m_pSliderTreeSpaceNoise);
	DDX_Control("Slider_TreeWeight", m_pSliderTreeWeight);

	DDX_Control("Label_TreeSeed", m_pLabelTreeSeed);
	DDX_Control("Label_TreeDensity", m_pLabelTreeDensity);
	DDX_Control("Label_TreeSlope", m_pLabelTreeSlope);
	DDX_Control("Label_TreeSpaceX", m_pLabelTreeSpaceX);
	DDX_Control("Label_TreeSpaceZ", m_pLabelTreeSpaceZ);
	DDX_Control("Label_TreeSpaceLine", m_pLabelTreeSpaceLine);
	DDX_Control("Label_TreeSpaceNoise", m_pLabelTreeSpaceNoise);
	DDX_Control("Label_TreeWeight", m_pLabelTreeWeight);

	DDX_Slider(bSave, "Slider_TreeSeed", m_nTreeSeed);
	DDX_Slider(bSave, "Slider_TreeDensity", m_nTreeDensity);
	DDX_Slider(bSave, "Slider_TreeSlope", m_nTreeSlope);
	DDX_Slider(bSave, "Slider_TreeSpaceX", m_nTreeSpaceX);
	DDX_Slider(bSave, "Slider_TreeSpaceZ", m_nTreeSpaceZ);
	DDX_Slider(bSave, "Slider_TreeSpaceLine", m_nTreeSpaceLine);
	DDX_Slider(bSave, "Slider_TreeSpaceNoise", m_nTreeSpaceNoise);
	DDX_Slider(bSave, "Slider_TreeWeight", m_nTreeWeight);

	m_fTreeWeight = float(m_nTreeWeight * 0.01f);

	DDX_Control("ListBox_TreeSet", m_pListBoxTreeSet);
	DDX_Control("ListBox_TreePlant", m_pListBoxTreePlant);
	
	DDX_CheckBox(bSave, "Check_TreeRegular", m_bCheckTreeRegular);

	DDX_Control( "Btn_Create", m_pBtnCreate);

	GET_BTN_POINTER(B_POLY);
	GET_BTN_POINTER(B_LINE);

}

void CDlgAutoOutdoorTree::LoadDefault()
{
	m_nTreeSeed = 1;
	m_nTreeDensity = 10;
	m_nTreeSlope = 200;
	m_nTreeSpaceX = 20;
	m_nTreeSpaceZ = 20;
	m_nTreeSpaceLine = 15;
	m_nTreeSpaceNoise = 3;

	m_nTreeWeight = 100;

	m_bCheckTreeRegular = false;

	m_nActionCount2 = 1;
	
	SetPolyAndLineBtnPush(true, false);
}
bool CDlgAutoOutdoorTree::InitResource()
{

	if(!FillTreeSetListBox())
		return false;

	m_TreeInfoArray.RemoveAll();
	m_pListBoxTreePlant->ResetContent();

	return true;
}

void CDlgAutoOutdoorTree::SetSliderBound()
{
	m_pSliderTreeSeed->SetTotal(100, 0);
	m_pSliderTreeDensity->SetTotal(100, 0); //* 0.01f
	m_pSliderTreeSlope->SetTotal(900, 0); // * 0.1f
	m_pSliderTreeSpaceX->SetTotal(50, 10);
	m_pSliderTreeSpaceZ->SetTotal(50, 10);
	m_pSliderTreeSpaceLine->SetTotal(50, 10);
	m_pSliderTreeSpaceNoise->SetTotal(50, 1);
	m_pSliderTreeWeight->SetTotal(100, 1);

}

void CDlgAutoOutdoorTree::SetLabelsText()
{
	ACHAR szName[100];

	swprintf(szName, _AL("%d"), m_nTreeSeed);
	m_pLabelTreeSeed->SetText(szName);

	swprintf(szName, _AL("%-4.2f"), (float)(m_nTreeDensity * 0.01f));
	m_pLabelTreeDensity->SetText(szName);

	swprintf(szName, _AL("%-4.1f"), (float)(m_nTreeSlope * 0.1f));
	m_pLabelTreeSlope->SetText(szName);

	swprintf(szName, _AL("%d"), m_nTreeSpaceX);
	m_pLabelTreeSpaceX->SetText(szName);

	swprintf(szName, _AL("%d"), m_nTreeSpaceZ);
	m_pLabelTreeSpaceZ->SetText(szName);

	swprintf(szName, _AL("%d"), m_nTreeSpaceLine);
	m_pLabelTreeSpaceLine->SetText(szName);

	swprintf(szName, _AL("%d"), m_nTreeSpaceNoise);
	m_pLabelTreeSpaceNoise->SetText(szName);

	swprintf(szName, _AL("%-4.2f"), m_fTreeWeight);
	m_pLabelTreeWeight->SetText(szName);
}

bool CDlgAutoOutdoorTree::FillTreeSetListBox()
{
	CAutoScene* pScene = GetAutoScene();
	if( !pScene)
		return false;

	CAutoForest* pForest = pScene->GetAutoForest();
	if( !pForest)
		return false;

	m_pTreeInfo = pForest->GetTreeInfos();
	m_nNumTreeInfos = pForest->GetNumTreeInfos();

	m_pListBoxTreeSet->ResetContent();
	
	//Fill
	for( int i = 0; i < m_nNumTreeInfos; i++)
	{
		m_pListBoxTreeSet->InsertString(i, AS2AC(m_pTreeInfo[i].strName));
	}

	return true;
}

void CDlgAutoOutdoorTree::OnCommandAddTreeInfo(const char* szCommand)
{
	UpdateData(true);

	PLANTIDWEIGHTINFO info;
	int nIdx = m_pListBoxTreeSet->GetCurSel();
	if( nIdx < 0 || nIdx >= m_pListBoxTreeSet->GetCount())
		return;

	info.dwID = m_pTreeInfo[nIdx].dwID;
	info.fWeight = m_fTreeWeight;

	if( MakeSureThisInfoNoExiting(m_TreeInfoArray, info))
	{
		m_TreeInfoArray.Add(info);
		ShowTreeInfos();
	}

	return;
}

void CDlgAutoOutdoorTree::OnCommandDelTreeInfo(const char* szCommand)
{
	if( m_TreeInfoArray.GetSize() == 0)
		return;

	int nIdx = m_pListBoxTreePlant->GetCurSel();
	if( nIdx < 0 || nIdx > m_pListBoxTreePlant->GetCount())
		return;

	m_TreeInfoArray.RemoveAt(nIdx);
	ShowTreeInfos();
}

void CDlgAutoOutdoorTree::OnCommandClearAllTreeInfo(const char* szCommand)
{
	m_TreeInfoArray.RemoveAll();
	m_pListBoxTreePlant->ResetContent();
}

void CDlgAutoOutdoorTree::ShowTreeInfos()
{
	CAutoForest* pForest = GetAutoScene()->GetAutoForest();
	CAutoForest::TREEINFO* pTypes = pForest->GetTreeInfos();

	m_pListBoxTreePlant->ResetContent();
	int nSize =  m_TreeInfoArray.GetSize();

	for( int i = 0; i < nSize; i ++)
	{
		int nIdx = pForest->GetGlobalTreeIndexById(m_TreeInfoArray[i].dwID);
		ASSERT(nIdx > -1);
		m_pListBoxTreePlant->AddString(AS2AC(pTypes[nIdx].strName));
	}
}

void CDlgAutoOutdoorTree::OnCommandEditTreePoly(const char* szCommand)
{
	SetPolyAndLineBtnPush(true, false);

	CAutoScene* pAutoScene = GetAutoScene();
	pAutoScene->SetOperationType(OT_TREE_POLY);

	GetHomeDlgsMgr()->m_pDlgAutoNote2->Show(true);
	GetHomeDlgsMgr()->m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_POLY);
}

void CDlgAutoOutdoorTree::OnCommandEditTreeLine(const char* szCommand)
{
	SetPolyAndLineBtnPush(false, true);

	CAutoScene* pAutoScene = GetAutoScene();
	pAutoScene->SetOperationType(OT_TREE_LINE);
	GetHomeDlgsMgr()->m_pDlgAutoNote2->Show(true);
	GetHomeDlgsMgr()->m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_LINE);
}

void CDlgAutoOutdoorTree::CreateTreePoly()
{
	UpdateData(true);

	if (GetOperationType() != OT_TREE_POLY)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	CAutoScene* pScene = GetAutoScene();
	CAutoForestPoly* pForestPoly = pScene->GetAutoForestPoly();
	if (NULL == pForestPoly)
		return;

	//Fill points
	CAutoSelectedArea* pSelArea = pScene->GetAutoSelectedArea();
	int nNum = pSelArea->GetNumPolyEditPoints();
	POINT_FLOAT* pPoints = pSelArea->GetPolyEditPoints();

	pForestPoly->SetAreaPoints(nNum, pPoints);

	bool bClosed = pSelArea->GetPolyEditAreaClosed();

	if (!bClosed)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	
	CAutoForest* pForest = pScene->GetAutoForest();

	int nTreeTypeCount = m_TreeInfoArray.GetSize();
	if (nTreeTypeCount <= 0)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	
	DWORD* pIDs = new DWORD[nTreeTypeCount];
	float* pWeights = new float[nTreeTypeCount];
	for (int i=0; i<nTreeTypeCount; i++)
	{
		pIDs[i] = m_TreeInfoArray[i].dwID;
		pWeights[i] = m_TreeInfoArray[i].fWeight;
	}
	int nPlantType;
	float fDensityX, fDensityZ;
	if (m_bCheckTreeRegular)
	{
		nPlantType = CAutoForest::PT_REGULAR;
		fDensityX = float(m_nTreeSpaceX);
		fDensityZ = float(m_nTreeSpaceZ);
	}
	else
	{
		nPlantType = CAutoForest::PT_RANDOM;
		fDensityX = (float)(m_nTreeDensity * 0.01f);
		fDensityZ = (float)(m_nTreeDensity * 0.01f);
	}

	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();


	if (!pForestPoly->CreateForestPoly(nPlantType, fDensityX, fDensityZ, (float)(m_nTreeSlope * 0.1f),
				nTreeTypeCount, pIDs, pWeights, m_nTreeSeed))
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	// Store action
	
	CClosedArea* pArea = pForestPoly->GetArea();

	if( GetHomeDlgsMgr()->GetModifying())
	{
		int nIdx = GetHomeDlgsMgr()->m_pDlgAutoAllRes->GetTreeViewSelItemIdx();
		if (!pAction->ChangeActionForestPoly(
			nPlantType,
			fDensityX,
			fDensityZ,
			(float)(m_nTreeSlope * 0.1f),
			nTreeTypeCount,
			pIDs,
			pWeights,
			m_nTreeSeed,
			pArea->GetNumPoints(),
			pArea->GetFinalPoints(), 
			nIdx))

		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1005), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			delete[] pIDs;
			delete[] pWeights;
			return;
		}

		GetAutoScene()->RefreshAllForestActions();

	}
	else
	{
		if (!pAction->AddActionForestPoly(
				nPlantType,
				fDensityX,
				fDensityZ,
				(float)(m_nTreeSlope * 0.1f),
				nTreeTypeCount,
				pIDs,
				pWeights,
				m_nTreeSeed,
				pArea->GetNumPoints(),
				pArea->GetFinalPoints()))

		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1005), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			delete[] pIDs;
			delete[] pWeights;
			return;
		}
	}

	delete[] pIDs;
	delete[] pWeights;

	//Clear points
	pSelArea->CopyPointsFromPolyEditToView();
	pSelArea->SetViewAreaClosed(true);
	pSelArea->DeleteAllPolyEditPoints();

	if( !GetHomeDlgsMgr()->GetModifying())
	{
		ACHAR szName[100];

		a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1006), m_nActionCount);
	
		m_nActionCount++;
		
		GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_TREE_POLY, szName);
	}
	else
	{
		GetHomeDlgsMgr()->SetModifying(false);
		SetBtnModifyToCreate();
		
	}
	
	return;

}

void CDlgAutoOutdoorTree::CreateTreeLine()
{
	UpdateData(true);

	if (GetOperationType() != OT_TREE_LINE)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(2001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	CAutoScene* pScene = GetAutoScene();
	CAutoForestLine* pForestLine = pScene->GetAutoForestLine();
	if (NULL == pForestLine)
		return;

	//Fill points
	CAutoSelectedArea* pSelArea = pScene->GetAutoSelectedArea();
	int nNum = pSelArea->GetNumLineEditPoints();
	POINT_FLOAT* pPoints = pSelArea->GetLineEditPoints();

	pForestLine->SetPoints(nNum, pPoints);

	if (!pForestLine->IsPointsValid())
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(2002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	if (float(m_nTreeSpaceLine) < float(m_nTreeSpaceNoise))
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(2003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	if (float(m_nTreeSpaceLine) <= 0)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(2004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	

	if (float(m_nTreeSpaceNoise) < 0)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(2005), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	CAutoForest* pForest = pScene->GetAutoForest();

	int nTreeTypeCount = m_TreeInfoArray.GetSize();
	if (nTreeTypeCount <= 0)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(2006), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	DWORD* pIDs = new DWORD[nTreeTypeCount];
	float* pWeights = new float[nTreeTypeCount];
	for (int i=0; i<nTreeTypeCount; i++)
	{
		pIDs[i] = m_TreeInfoArray[i].dwID;
		pWeights[i] = m_TreeInfoArray[i].fWeight;
	}

	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();


	if (!pForestLine->CreateForestLine((float)m_nTreeSpaceLine, (float)m_nTreeSpaceNoise, (float)(m_nTreeSlope * 0.1f),
				nTreeTypeCount, pIDs, pWeights, m_nTreeSeed))
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	
	if( GetHomeDlgsMgr()->GetModifying())
	{
		int nIdx = GetHomeDlgsMgr()->m_pDlgAutoAllRes->GetTreeViewSelItemIdx();

		// Store action
		if (!pAction->ChangeActionForestLine(
				(float)m_nTreeSpaceLine, 
				(float)m_nTreeSpaceNoise, 
				(float)(m_nTreeSlope * 0.1f),
				nTreeTypeCount,
				pIDs,
				pWeights,
				m_nTreeSeed,
				pForestLine->GetNumPoints(),
				pForestLine->GetFinalPoints(),
				nIdx))

		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1005), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			delete[] pIDs;
			delete[] pWeights;
			return;
		}
		
		GetAutoScene()->RefreshAllForestActions();
	}
	else
	{
		// Store action
		if (!pAction->AddActionForestLine(
				(float)m_nTreeSpaceLine, 
				(float)m_nTreeSpaceNoise, 
				(float)(m_nTreeSlope * 0.1f),
				nTreeTypeCount,
				pIDs,
				pWeights,
				m_nTreeSeed,
				pForestLine->GetNumPoints(),
				pForestLine->GetFinalPoints()))

		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1005), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			delete[] pIDs;
			delete[] pWeights;
			return;
		}

	}

	delete[] pIDs;
	delete[] pWeights;

		//Clear points
	pSelArea->CopyPointsFromLineEditToView();
	pSelArea->SetViewAreaClosed(false);
	pSelArea->DeleteAllLineEditPoints();



	if( !GetHomeDlgsMgr()->GetModifying())
	{
		ACHAR szName[100];
		a_sprintf( szName, _AL("%s%d"), GetStringFromTable(2007), m_nActionCount2);
		m_nActionCount2++;
		
		GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_TREE_LINE, szName);
	}
	else
	{
		GetHomeDlgsMgr()->SetModifying(false);
		SetBtnModifyToCreate();
	}
}



void CDlgAutoOutdoorTree::OnCommandCreateTree(const char* szCommand)
{
	int nOperateType;
	nOperateType = GetOperationType();

	switch(nOperateType)
	{
	case OT_TREE_LINE:
			CreateTreeLine();
			break;
	case OT_TREE_POLY:
			CreateTreePoly();
			break;
	default:
		break;
	}

}

void CDlgAutoOutdoorTree::OnCommandMoveSlider(const char * szCommand)
{
	UpdateData(true);
	SetLabelsText();
}

void CDlgAutoOutdoorTree::SetBtnCreateToModify()
{
	m_pBtnCreate->SetText( GetStringFromTable(2008) );
	return;
}

void CDlgAutoOutdoorTree::SetBtnModifyToCreate()
{
	m_pBtnCreate->SetText( GetStringFromTable(2009) );
	return;
}

void CDlgAutoOutdoorTree::OnEventDblClkTreeSetList(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	OnCommandAddTreeInfo("");

	return;
}

void CDlgAutoOutdoorTree::OnEventDblClkTreePlantList(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	OnCommandDelTreeInfo("");
	return;
}

void CDlgAutoOutdoorTree::SetPolyAndLineBtnPush(bool i_bPushedPoly, bool i_bPushedLine)
{
	ASSERT( i_bPushedPoly != i_bPushedLine);

	m_pBtns[B_POLY]->SetPushed(i_bPushedPoly);
	m_pBtns[B_LINE]->SetPushed(i_bPushedLine);

	return;
}

bool CDlgAutoOutdoorTree::MakeSureThisInfoNoExiting(const TreeInfoArray_t &i_InfoArray, const PLANTIDWEIGHTINFO &i_Info)
{
	for( int i = 0; i < i_InfoArray.GetSize(); i++)
	{
		if(i_Info.dwID == i_InfoArray[i].dwID)
			return false;
	}
	return true;
}
*/