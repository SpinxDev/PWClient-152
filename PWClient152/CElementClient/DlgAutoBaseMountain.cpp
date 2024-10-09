/*
 * FILE: DlgAutoBaseMountain.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
/*
#include "DlgAutoBaseMountain.h"
#include "DlgAutoMountain.h"
#include "DlgAutoAllRes.h"
#include "DlgAutoSwitchDlg.h"
#include "DlgAutoChooseTexture.h"

#include "EC_HomeDlgsMgr.h"
#include "aui/AUIListBox.h"
#include "aui/AUIManager.h"
#include "aui/AUIStillImageButton.h"

#include "AutoTerrainCommon.h"

#include "A3DEngine.h"
#include "A3DViewport.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "AutoScene.h"
#include "AutoTerrain.h"
#include "AutoHillPoly.h"
#include "AutoParameters.h"
#include "PolygonMountain.h"
#include "AutoBuildingOperation.h"
#include "AutoSelectedArea.h"
#include "AutoTerrainAction.h"


#include "windowsx.h"

#include "A3DMacros.h"

#include "io.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoBaseMountain, CDlgHomeBase)

AUI_ON_COMMAND("CreateMountainPoly", OnCommandCreateMountainPoly)

AUI_ON_COMMAND("Advance", OnCommandAdvance)
AUI_ON_COMMAND("DelHill", OnCommandDelPre)

AUI_ON_COMMAND("ApplyTex", OnCommandApplyTex)
AUI_ON_COMMAND("AdvTex", OnCommandAdvTex)
AUI_ON_COMMAND("DelTex", OnCommandDelTex)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoBaseMountain, CDlgHomeBase)

AUI_ON_EVENT("ListBox_Mountain", WM_LBUTTONUP, OnLButtonUpListBoxHill)
AUI_ON_EVENT("ListBox_Texture", WM_LBUTTONUP, OnLButtonUpListBoxTex)

AUI_END_EVENT_MAP()



CDlgAutoBaseMountain::CDlgAutoBaseMountain()
{
	m_pListBoxTexture = NULL;
	m_pListBoxMountain = NULL;
	m_nMountainIdx = 0;
	m_nTextureIdx = 0;


	m_iPreHillNum = 0;
	m_iPreTexNum = 0;

	for(int i = 0; i < BTN_NUM; i++)
	{
		m_pBtn[i] = NULL;
	}
}

CDlgAutoBaseMountain::~CDlgAutoBaseMountain()
{
}
	
AUIStillImageButton * CDlgAutoBaseMountain::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnBaseMountain;
}

void CDlgAutoBaseMountain::OnShowDialog()
{
	SetCanMove(false);
	
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}

bool CDlgAutoBaseMountain::OnInitContext()
{
	if(!InitResource())
		return false;

	
	LoadDefault();
	UpdateData(false);
	
	return true;
}

#define GET_BTN_POINTER(a) DDX_Control( #a, m_pBtn[a]);
void CDlgAutoBaseMountain::DoDataExchange(bool bSave)
{
	DDX_Control("ListBox_Mountain", m_pListBoxMountain);
	DDX_Control("ListBox_Texture", m_pListBoxTexture);
	DDX_ListBox(bSave, "ListBox_Mountain", m_nMountainIdx);
	DDX_ListBox(bSave, "ListBox_Texture", m_nTextureIdx);

	GET_BTN_POINTER(BTN_CREATE_HILL);
	GET_BTN_POINTER(BTN_ADV_HILL);
	GET_BTN_POINTER(BTN_DEL_HILL);

	GET_BTN_POINTER(BTN_APPLY_TEX);
	GET_BTN_POINTER(BTN_ADV_TEX);
	GET_BTN_POINTER(BTN_DEL_TEX);
}
void CDlgAutoBaseMountain::LoadDefault()
{
	m_nMountainIdx = 0;
	m_nTextureIdx = 0;

	m_pBtn[BTN_CREATE_HILL]->Enable(true);
	m_pBtn[BTN_ADV_HILL]->Enable(true);
	m_pBtn[BTN_DEL_HILL]->Enable(false);

	m_pBtn[BTN_APPLY_TEX]->Enable(false);
	m_pBtn[BTN_ADV_TEX]->Enable(false);
	m_pBtn[BTN_DEL_TEX]->Enable(false);
}
bool CDlgAutoBaseMountain::InitResource()
{

	FillMountainListBox();
	FillTextureListBox();
	return true;
}
void CDlgAutoBaseMountain::FillMountainListBox()
{
	m_pListBoxMountain->ResetContent();
	CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetHillPolySet();

	//Ԥ������
	if( pInfo)
	{
		m_iPreHillNum = GetAutoParam()->GetNumHillPolySet();
		for( int i = 0; i < m_iPreHillNum ; i++)
		{
			m_pListBoxMountain->InsertString(i, AS2AC(pInfo[i].strName));
		}
	}

	//�Զ�������
	_finddata_t fdIni;//�ļ�����
	long lFileHandle = _findfirst( "userdata\\home\\hill\\*.*", &fdIni);//��õ�һ���ļ��ľ��
	if( lFileHandle == -1)
	{
		return ;
	}

	int nResult = 0;//����findnext�ķ���ֵ
	while( nResult != -1)
	{
		//����ǵ�ǰ��Ŀ¼�ļ�,������
		if( fdIni.attrib & 0x10)//16��ʾĿ¼(0x10)
		{
			nResult = _findnext( lFileHandle, &fdIni);
			continue;
		}
	
		//��listbox 
		m_pListBoxMountain->InsertString(m_pListBoxMountain->GetCount(), AS2AC(fdIni.name));
		
		//��ȡ��һ���û��Զ��������ļ��ľ��
		nResult = _findnext( lFileHandle, &fdIni);
	}
	
	//�رվ��
	_findclose( lFileHandle);
}

void CDlgAutoBaseMountain::FillTextureListBox()
{
	m_pListBoxTexture->ResetContent();
	CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetTextureSet();

	if( pInfo)
	{
		m_iPreTexNum = GetAutoParam()->GetNumTextureSet();
		for( int i = 0; i < m_iPreTexNum ; i++)
		{
			m_pListBoxTexture->InsertString(i, AS2AC(pInfo[i].strName));
		}
	}

	//�Զ�������
	_finddata_t fdIni;//�ļ�����
	long lFileHandle = _findfirst( "userdata\\home\\hilltex\\*.*", &fdIni);//��õ�һ���ļ��ľ��
	if( lFileHandle == -1)
	{
		return ;
	}

	int nResult = 0;//����findnext�ķ���ֵ
	while( nResult != -1)
	{
		//����ǵ�ǰ��Ŀ¼�ļ�,������
		if( fdIni.attrib & 0x10)//16��ʾĿ¼(0x10)
		{
			nResult = _findnext( lFileHandle, &fdIni);
			continue;
		}
	
		//��listbox 
		m_pListBoxTexture->InsertString(m_pListBoxTexture->GetCount(), AS2AC(fdIni.name));
		
		//��ȡ��һ���û��Զ��������ļ��ľ��
		nResult = _findnext( lFileHandle, &fdIni);
	}
	
	//�رվ��
	_findclose( lFileHandle);
}


bool CDlgAutoBaseMountain::GetTextureParameter(int a_nIdx, CAutoParameters::TEXTUREINFO* a_pTexInfo)
{
	UpdateData(true);
	
	if( !IsValidIdx(a_nIdx, m_pListBoxTexture))
		return false;

	if( a_nIdx < m_iPreTexNum)
	{
		CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetTextureSet();
		if( !GetAutoParam()->LoadTextureInfo(pInfo[a_nIdx].strPath, a_pTexInfo))
			return false;

	}	
	else //�û�����
	{
		char szFileName[MAX_PATH];
		strcpy(szFileName, "userdata\\home\\hilltex\\");
		strcat(szFileName, AC2AS(m_pListBoxTexture->GetText(a_nIdx)));
		if( !GetAutoParam()->LoadTextureInfo(szFileName, a_pTexInfo))
			return false;
	}

	return true;

}
bool CDlgAutoBaseMountain::GetMountainPolyParameter(int a_nIdx, CAutoParameters::HILLINFO* a_pMounPolyInfo)
{
	if( !IsValidIdx(a_nIdx, m_pListBoxMountain))
		return false;
	if( a_nIdx < m_iPreHillNum)
	{
		CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetHillPolySet();
		if( !GetAutoParam()->LoadHillInfo(pInfo[a_nIdx].strPath, a_pMounPolyInfo))
			return false;

	}	
	else //�û�����
	{
		char szFileName[MAX_PATH];
		strcpy(szFileName, "userdata\\home\\hill\\");
		strcat(szFileName, AC2AS(m_pListBoxMountain->GetText(a_nIdx)));
		if( !GetAutoParam()->LoadHillInfo(szFileName, a_pMounPolyInfo))
			return false;
	}
	return true;
}

void CDlgAutoBaseMountain::OnCommandCreateMountainPoly(const char* szCommand)
{
	PROFILE_BEGIN();

	UpdateData(true);

	if( GetOperationType() != OT_HILL_POLY)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}


	CAutoScene* pScene = GetAutoScene();
	CAutoTexture* pTexture = pScene->GetAutoTerrain()->GetTexture();
	CAutoHillPoly* pAutoHillPoly = pScene->GetAutoHillPoly();
	
	if (NULL == pAutoHillPoly)
		return;

	CAutoSelectedArea* pSelArea = pScene->GetAutoSelectedArea();
	int nNum = pSelArea->GetNumPolyEditPoints();
	POINT_FLOAT* pPoints = pSelArea->GetPolyEditPoints();
	bool bClosed = pSelArea->GetPolyEditAreaClosed();

	pAutoHillPoly->SetAreaPoints(nNum, pPoints);

	if (!bClosed)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	
	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();

	 
	CAutoParameters::HILLINFO mounPolyInfo;
	if(!GetMountainPolyParameter(m_nMountainIdx, &mounPolyInfo))
	{
		AutoHomeReport("CDlgAutoBaseMountain::OnCommandCreateMountainPoly, Failed to get MountainPolyParameter!");
		return;
	}

	// Store action
	CPolygonMountain* pArea = pScene->GetAutoHillPoly()->GetArea();
	if( GetHomeDlgsMgr()->GetModifying())
	{
		int nIdx = GetHomeDlgsMgr()->m_pDlgAutoAllRes->GetTreeViewSelItemIdx();
		if (!pAction->ChangeActionHillPoly(
				 mounPolyInfo.fMaxHeightHill,
				 mounPolyInfo.fMainSmoothHill,
				 mounPolyInfo.fPartSmoothHill,
				 mounPolyInfo.dwHillSeed,
				 mounPolyInfo.nNumHeightCurve,
				 mounPolyInfo.pHeightCurve,
				 mounPolyInfo.bHillUp,
				 mounPolyInfo.fTextureRatio,
				 mounPolyInfo.fMainTexture,
				 mounPolyInfo.fPatchTexture,
				mounPolyInfo.fPatchDivision,
				mounPolyInfo.fBridgeScope,
				 mounPolyInfo.fBridgeDepth,
				pArea->GetNumPoints(), 
				pArea->GetFinalPoints(),
				 mounPolyInfo.fBorderErode, nIdx))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		GetAutoScene()->RefreshAllHillActions();
	}
	else
	{
			// Create height
		if (!pAutoHillPoly->CreateHillPoly(
				 mounPolyInfo.fMaxHeightHill,
				 mounPolyInfo.fMainSmoothHill,
				 mounPolyInfo.fPartSmoothHill,
				 mounPolyInfo.dwHillSeed,
				 mounPolyInfo.nNumHeightCurve,
				 mounPolyInfo.pHeightCurve,
				 mounPolyInfo.bHillUp,
				 mounPolyInfo.fBorderErode))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		
		if( mounPolyInfo.bHillUp)
		{
			if( !pAutoHillPoly->RenderHillTop(
				mounPolyInfo.fTextureRatio,
				mounPolyInfo.fMainTexture,
				mounPolyInfo.fPatchTexture,
				mounPolyInfo.fPatchDivision,
				mounPolyInfo.fBridgeScope,
				mounPolyInfo.fBridgeDepth))
			{
				GetAUIManager()->MessageBox("", GetStringFromTable(1005), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				return;
			}
		}

		if (!pAction->AddActionHillPoly(
				 mounPolyInfo.fMaxHeightHill,
				 mounPolyInfo.fMainSmoothHill,
				 mounPolyInfo.fPartSmoothHill,
				 mounPolyInfo.dwHillSeed,
				 mounPolyInfo.nNumHeightCurve,
				 mounPolyInfo.pHeightCurve,
				 mounPolyInfo.bHillUp,
				 mounPolyInfo.fTextureRatio,
				 mounPolyInfo.fMainTexture,
				 mounPolyInfo.fPatchTexture,
				mounPolyInfo.fPatchDivision,
				mounPolyInfo.fBridgeScope,
				 mounPolyInfo.fBridgeDepth,
				pArea->GetNumPoints(), 
				pArea->GetFinalPoints(),
				 mounPolyInfo.fBorderErode))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		
		
	}	

	GetAutoScene()->RecreateTerrainRender(true);
	// Update buildings
	pScene->GetAutoTerrain()->Tick(0);// Update terrain height
	CAutoBuildingOperation* pBuildingOperation = pScene->GetAutoBuildingOperation();
	pBuildingOperation->AdjustModelsAfterTerrainChange();

	PROFILE_END();


	pSelArea->CopyPointsFromPolyEditToView();
	pSelArea->SetViewAreaClosed(bClosed);
	pSelArea->DeleteAllPolyEditPoints();

	//Add Item to TerrainRes Tree View
	if( !GetHomeDlgsMgr()->GetModifying())
	{
		ACHAR szName[100];
		a_sprintf( szName, _AL("%s%d"), GetStringFromTable(1006), m_nActionCount );
		m_nActionCount++;
		
		GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_MOUNTAIN_POLY, szName);
	}
	else
	{
		GetHomeDlgsMgr()->SetModifying(false);
		SetBtnModifyToCreate();
		
	}

	//���������������
	SetTexOpEnable(true);
	
	return;
}


void CDlgAutoBaseMountain::OnCommandAdvance(const char* szCommand)
{
	//this->Show(false);
	
	UpdateData(true);
	GetHomeDlgsMgr()->m_pDlgAutoMountain->Show(true);
	GetHomeDlgsMgr()->SetUnAdvDlgsDisable();

	CAutoParameters::HILLINFO mounInfo;
	if( !GetMountainPolyParameter(m_nMountainIdx, &mounInfo))
	{
		AutoHomeReport("CDlgAutoBaseMountain::OnCommandAdavnce, Failed to Get GetMountainPolyParameter!");
		return;
	}

	//Ԥ������
	if( m_nMountainIdx < m_iPreHillNum)
	{
		GetHomeDlgsMgr()->m_pDlgAutoMountain->InitSettings(mounInfo,
													false);
	}
	else //�Զ�������
	{
		GetHomeDlgsMgr()->m_pDlgAutoMountain->InitSettings(mounInfo,
													true, 
													AC2AS(m_pListBoxMountain->GetText(m_nMountainIdx)));
	}

}

void CDlgAutoBaseMountain::SetBtnCreateToModify()
{
	m_pBtn[BTN_CREATE_HILL]->SetText( GetStringFromTable(1007) );
	return;
}

void CDlgAutoBaseMountain::SetBtnModifyToCreate()
{
	m_pBtn[BTN_CREATE_HILL]->SetText( GetStringFromTable(1008) );
	return;
}

void CDlgAutoBaseMountain::InsertHillSettings(const ACHAR* szFileName)
{
	int iIdx = m_pListBoxMountain->GetCount();
	m_pListBoxMountain->InsertString(iIdx, szFileName);
	
}

void CDlgAutoBaseMountain::OnCommandDelPre(const char* szCommand)
{
	UpdateData(true);

	if(m_nMountainIdx < m_iPreHillNum || m_nMountainIdx >= m_pListBoxMountain->GetCount())
		return;

	ACHAR szFileName[MAX_PATH];
	a_strcpy( szFileName, _AL("userdata\\home\\hill\\"));

	a_strcat(szFileName, m_pListBoxMountain->GetText(m_nMountainIdx));

	//ɾ����ǰ�ļ�
	DeleteFile(szFileName);
	

	//ɾ����ǰlistboxѡ��
	m_pListBoxMountain->DeleteString(m_nMountainIdx);
	
	//���õ�ǰlistboxѡ��
	if(m_nMountainIdx < m_pListBoxMountain->GetCount())
		m_pListBoxMountain->SetCurSel(m_nMountainIdx);
	else
	{
		m_nMountainIdx = m_pListBoxMountain->GetCount() -1;

		m_pListBoxMountain->SetCurSel(m_nMountainIdx);
		if( m_nMountainIdx == m_iPreHillNum -1)
		{
			m_pBtn[BTN_DEL_HILL]->Enable(false);
		}
	}
	UpdateData(false);

}

void CDlgAutoBaseMountain::OnLButtonUpListBoxHill(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//�ж��Ƿ�����б���е�ѡ��
	POINT ptPos = pObj->GetPos();

	A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
	int nMouseX = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int nMouseY = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if(m_pListBoxMountain->GetHitArea(nMouseX, nMouseY)!= AUILISTBOX_RECT_TEXT)
		return;

	int nCurSel = m_pListBoxMountain->GetCurSel();

	if( nCurSel >= 0 && nCurSel < m_iPreHillNum) // Ԥ��
	{
		m_pBtn[BTN_CREATE_HILL]->Enable(true);
		m_pBtn[BTN_ADV_HILL]->Enable(true);
		m_pBtn[BTN_DEL_HILL]->Enable(false);
		
	}
	else if( nCurSel < m_pListBoxMountain->GetCount()) //�Զ���
	{
		m_pBtn[BTN_CREATE_HILL]->Enable(true);
		m_pBtn[BTN_ADV_HILL]->Enable(true);
		m_pBtn[BTN_DEL_HILL]->Enable(true);
	}
	else //���ڿ�λ��											
	{
		m_pBtn[BTN_CREATE_HILL]->Enable(false);
		m_pBtn[BTN_ADV_HILL]->Enable(false);
		m_pBtn[BTN_DEL_HILL]->Enable(false);
	}

	return;
}

void CDlgAutoBaseMountain::OnCommandApplyTex(const char* szCommand)
{
	UpdateData(true);

	CAutoScene* pScene = GetAutoScene();
	
	CAutoTexture* pTexture = pScene->GetAutoTerrain()->GetTexture();
	
	CAutoParameters::TEXTUREINFO texInfo;

	if(!GetTextureParameter(m_nTextureIdx, &texInfo))
		AutoHomeReport("CDlgAutoBaseTerrain::OnCommandCreateTexture, Can not Get Texture Parameter!");

	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();
	
	pTexture->ChangeTextureParam(
		TT_BRIDGE,
		TT_BRIDGE_PATCH,
		texInfo.fMainTextureU,
		texInfo.fMainTextureV,
		texInfo.fPatchTextureU,
		texInfo.fPatchTextureV,
		texInfo.dwMainTextureID,
		texInfo.dwPatchTextureID);


	pAction->AddActionHillTextureInfo(
		texInfo.fMainTextureU,
		texInfo.fMainTextureV,
		texInfo.fPatchTextureU,
		texInfo.fPatchTextureV,
		texInfo.dwMainTextureID,
		texInfo.dwPatchTextureID);

	if (!pScene->RecreateTerrainRender(true))
	{
		AutoHomeReport( "DlgAutoTerrain::OnCommandCreateTexture, Failed to recreate terrain render!");
		return;
	}
}

void CDlgAutoBaseMountain::OnCommandDelTex(const char* szCommand)
{
	UpdateData(true);

	if(m_nTextureIdx < m_iPreTexNum || m_nTextureIdx >= m_pListBoxTexture->GetCount())
		return;

	ACHAR szFileName[MAX_PATH];
	a_strcpy( szFileName, _AL("userdata\\home\\hilltex\\"));
	a_strcat(szFileName, m_pListBoxTexture->GetText(m_nTextureIdx));

	//ɾ����ǰ�ļ�
	DeleteFile(szFileName);
	

	//ɾ����ǰlistboxѡ��
	m_pListBoxTexture->DeleteString(m_nTextureIdx);
	
	//���õ�ǰlistboxѡ��
	if(m_nTextureIdx < m_pListBoxTexture->GetCount())
		m_pListBoxTexture->SetCurSel(m_nTextureIdx);
	else
	{
		m_nTextureIdx = m_pListBoxTexture->GetCount() -1;

		m_pListBoxTexture->SetCurSel(m_nTextureIdx);
		if( m_nTextureIdx == m_iPreTexNum -1)
		{
			m_pBtn[BTN_DEL_TEX]->Enable(false);
		}
	}
	UpdateData(false);
}

void CDlgAutoBaseMountain::InsertTexSettings(const ACHAR* szFileName)
{
	int iIdx = m_pListBoxTexture->GetCount();
	m_pListBoxTexture->InsertString(iIdx, szFileName);
	
}
void CDlgAutoBaseMountain::OnLButtonUpListBoxTex(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//�ж��Ƿ�����б���е�ѡ��
	POINT ptPos = pObj->GetPos();

	A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
	int nMouseX = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int nMouseY = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if(m_pListBoxTexture->GetHitArea(nMouseX, nMouseY)!= AUILISTBOX_RECT_TEXT)
		return;

	int nCurSel = m_pListBoxTexture->GetCurSel();

	if( nCurSel >= 0 && nCurSel < m_iPreTexNum) // Ԥ��
	{
		m_pBtn[BTN_APPLY_TEX]->Enable(true);
		m_pBtn[BTN_ADV_TEX]->Enable(true);
		m_pBtn[BTN_DEL_TEX]->Enable(false);
		
	}
	else if( nCurSel < m_pListBoxTexture->GetCount()) //�Զ���
	{
		m_pBtn[BTN_APPLY_TEX]->Enable(true);
		m_pBtn[BTN_ADV_TEX]->Enable(true);
		m_pBtn[BTN_DEL_TEX]->Enable(true);
	}
	else //���ڿ�λ��											
	{
		m_pBtn[BTN_APPLY_TEX]->Enable(false);
		m_pBtn[BTN_ADV_TEX]->Enable(false);
		m_pBtn[BTN_DEL_TEX]->Enable(false);
	}

	return;
}

void CDlgAutoBaseMountain::OnCommandAdvTex(const char* szCommand)
{
	UpdateData(true);
	
	
	CAutoParameters::TEXTUREINFO texInfo;
	if( !GetTextureParameter(m_nTextureIdx, &texInfo))
			AutoHomeReport("CDlgAutoBaseMountain::OnCommandAdvTex, Can not Get TextureParameter!");

	TextureParamInfo_t texParam;
	GetHomeDlgsMgr()->TransformTexInfoToTexParam(texInfo, texParam);

		//Ԥ������
	if( m_nTextureIdx < m_iPreTexNum)
	{
	
		GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->InitTextureParamInfo(texParam, CTIP_HILL);
	}
	else //�Զ�������
	{
		GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->InitTextureParamInfo(texParam, CTIP_HILL,
													true, 
													AC2AS(m_pListBoxTexture->GetText(m_nTextureIdx)));
	}

	GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->Show(true);
	 GetHomeDlgsMgr()->SetUnAdvDlgsDisable();

}

void CDlgAutoBaseMountain::SetTexOpEnable(bool i_bEnable)
{
	m_pBtn[BTN_ADV_TEX]->Enable(i_bEnable);
	m_pBtn[BTN_APPLY_TEX]->Enable(i_bEnable);
}
*/