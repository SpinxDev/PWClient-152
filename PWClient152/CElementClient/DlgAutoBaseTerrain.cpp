/*
 * FILE: DlgAutoBaseTerrain.cpp
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
#include "DlgAutoBaseTerrain.h"
#include "DlgAutoTerrain.h"
#include "DlgAutoSwitchDlg.h"
#include "DlgAutoChooseTexture.h"

#include "EC_HomeDlgsMgr.h"
#include "AUI/AUIListBox.h"
#include "AUI/AUIStillImageButton.h"
#include "AutoScene.h"
#include "AutoParameters.h"
#include "AutoTerrain.h"
#include "AutoBuildingOperation.h"
#include "AutoTerrainAction.h"

#include "EC_Game.h"
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "windowsx.h"
#include "AUI\\AUIManager.h"
#include "A3DMacros.h"
#include "io.h"


AUI_BEGIN_COMMAND_MAP(CDlgAutoBaseTerrain, CDlgHomeBase)

AUI_ON_COMMAND("CreateTerrain", OnCommandCreateTerrain)
AUI_ON_COMMAND("Advance", OnCommandAdvance)
AUI_ON_COMMAND("DelTerrain", OnCommandDelPre)

AUI_ON_COMMAND("ApplyTex", OnCommandApplyTex)
AUI_ON_COMMAND("AdvTex", OnCommandAdvTex)
AUI_ON_COMMAND("DelTex", OnCommandDelTex)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoBaseTerrain, CDlgHomeBase)

AUI_ON_EVENT("ListBox_Terrain", WM_LBUTTONUP, OnLButtonUpListBoxTerrain)
AUI_ON_EVENT("ListBox_Texture", WM_LBUTTONUP, OnLButtonUpListBoxTex)

AUI_END_EVENT_MAP()

CDlgAutoBaseTerrain::CDlgAutoBaseTerrain()
{
	m_pListBoxTexture = NULL;
	m_pListBoxTerrain = NULL;
	m_nTerrainIdx = 0;
	m_nTextureIdx = 0;

	m_iPreTerrainNum = 0;
	m_iPreTexNum = 0;
	
	for(int i = 0; i < BTN_NUM; i++)
	{
		m_pBtn[i] = NULL;
	}
}
CDlgAutoBaseTerrain::~CDlgAutoBaseTerrain()
{
}
	
AUIStillImageButton * CDlgAutoBaseTerrain::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnBaseTerrain;
}

void CDlgAutoBaseTerrain::OnShowDialog()
{
	SetCanMove(false);

	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}

bool CDlgAutoBaseTerrain::OnInitContext()
{
	if(!InitResource())
		return false;
	
	LoadDefault();
	UpdateData(false);

	return true;
}

void CDlgAutoBaseTerrain::LoadDefault()
{

	m_nTerrainIdx = 0;
	m_nTextureIdx = 0;

	m_pBtn[BTN_CREATE_TERRAIN]->Enable(true);
	m_pBtn[BTN_ADV_TERRAIN]->Enable(true);
	m_pBtn[BTN_DEL_TERRAIN]->Enable(false);

	m_pBtn[BTN_APPLY_TEX]->Enable(true);
	m_pBtn[BTN_ADV_TEX]->Enable(true);
	m_pBtn[BTN_DEL_TEX]->Enable(false);
}

bool CDlgAutoBaseTerrain::InitResource()
{

	FillTerrainListBox();
	FillTextureListBox();

	return true;
}
void CDlgAutoBaseTerrain::FillTerrainListBox()
{
	m_pListBoxTerrain->ResetContent();

	//Ԥ��
	CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetMainHeightSet();
	if( pInfo)
	{
		m_iPreTerrainNum  = GetAutoParam()->GetNumMainHeightSet();
		for( int i = 0; i < m_iPreTerrainNum ; i++)
		{
			m_pListBoxTerrain->InsertString(i, AS2AC(pInfo[i].strName));
		}
	}
	
	//�Զ�������
	_finddata_t fdIni;//�ļ�����
	long lFileHandle = _findfirst( "userdata\\home\\terrain\\*.*", &fdIni);//��õ�һ���ļ��ľ��
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
		m_pListBoxTerrain->InsertString(m_pListBoxTerrain->GetCount(), AS2AC(fdIni.name));
		
		//��ȡ��һ���û��Զ��������ļ��ľ��
		nResult = _findnext( lFileHandle, &fdIni);
	}
	
	//�رվ��
	_findclose( lFileHandle);
	
	return;

}
void CDlgAutoBaseTerrain::FillTextureListBox()
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
	long lFileHandle = _findfirst( "userdata\\home\\terraintex\\*.*", &fdIni);//��õ�һ���ļ��ľ��
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

#define GET_BTN_POINTER(a) DDX_Control( #a, m_pBtn[a]);

void CDlgAutoBaseTerrain::DoDataExchange(bool bSave)
{
	DDX_Control("ListBox_Terrain", m_pListBoxTerrain);
	DDX_Control("ListBox_Texture", m_pListBoxTexture);

	GET_BTN_POINTER(BTN_DEL_TERRAIN);
	GET_BTN_POINTER(BTN_ADV_TERRAIN);
	GET_BTN_POINTER(BTN_CREATE_TERRAIN);
	
	GET_BTN_POINTER(BTN_APPLY_TEX);
	GET_BTN_POINTER(BTN_ADV_TEX);
	GET_BTN_POINTER(BTN_DEL_TEX);

	DDX_ListBox(bSave, "ListBox_Terrain", m_nTerrainIdx);
	DDX_ListBox(bSave, "ListBox_Texture", m_nTextureIdx);



}


void CDlgAutoBaseTerrain::OnCommandCreateTerrain(const char* szCommand)
{
	UpdateData(true);
	
	if(m_nTerrainIdx >= m_pListBoxTerrain->GetCount())
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	CAutoScene* pAutoScene = GetAutoScene();
	

	CAutoTerrain* pAutoTerrain = pAutoScene->GetAutoTerrain();

	CAutoParameters::MAINTERRAINHEIGHT mainTerr;
	if( !GetTerrainParameter(m_nTerrainIdx, &mainTerr))
		AutoHomeReport("CDlgAutoBaseTerrain::OnCommandCreateTerrain, Can not Get TerrainParameter!");
	

	pAutoTerrain->CreateTerrain(mainTerr.fMaxHeight, mainTerr.nMainSmooth, mainTerr.fPartSmooth, 
		mainTerr.fTextureRatio, mainTerr.fMainTexture, mainTerr.fPatchTexture, mainTerr.fPatchDivision, mainTerr.dwSeed);

	pAutoScene->GetAutoTerrainAction()->AddActionBaseTerrain(mainTerr.fMaxHeight, mainTerr.nMainSmooth, mainTerr.fPartSmooth, 
		mainTerr.fTextureRatio, mainTerr.fMainTexture, mainTerr.fPatchTexture, mainTerr.fPatchDivision, mainTerr.dwSeed);

	pAutoScene->RecreateTerrainRender(true);

	pAutoTerrain->Tick(0);// Update terrain height
	CAutoBuildingOperation* pBuildingOperation = pAutoScene->GetAutoBuildingOperation();
	pBuildingOperation->AdjustModelsAfterTerrainChange();
		
}

bool CDlgAutoBaseTerrain::GetTerrainParameter(int a_nIdx, CAutoParameters::MAINTERRAINHEIGHT* a_pMainTerr)
{

	if( !IsValidIdx(a_nIdx, m_pListBoxTerrain))
		return false;

	//Ԥ������
	if( a_nIdx < m_iPreTerrainNum)
	{
		CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetMainHeightSet();
	
		if( !GetAutoParam()->LoadMainTerrainHeight(pInfo[a_nIdx].strPath, a_pMainTerr))
			return false;
	}	
	else //�û�����
	{
		char szFileName[MAX_PATH];
		strcpy(szFileName, "userdata\\home\\terrain\\");
		strcat(szFileName, AC2AS(m_pListBoxTerrain->GetText(a_nIdx)));
		if( !GetAutoParam()->LoadMainTerrainHeight(szFileName, a_pMainTerr))
			return false;
	}


	return true;
}

void CDlgAutoBaseTerrain::OnCommandApplyTex(const char* szCommand)
{
	UpdateData(true);

	CAutoScene* pScene = GetAutoScene();
	
	CAutoTexture* pTexture = pScene->GetAutoTerrain()->GetTexture();
	
	CAutoParameters::TEXTUREINFO texInfo;

	if(!GetTextureParameter(m_nTextureIdx, &texInfo))
		AutoHomeReport("CDlgAutoBaseTerrain::OnCommandCreateTexture, Can not Get Texture Parameter!");

	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();
	
	pTexture->ChangeTextureParam(
		TT_MAIN,
		TT_MAIN_PATCH,
		texInfo.fMainTextureU,
		texInfo.fMainTextureV,
		texInfo.fPatchTextureU,
		texInfo.fPatchTextureV,
		texInfo.dwMainTextureID,
		texInfo.dwPatchTextureID);


	pAction->AddActionBaseTextureInfo(
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

bool CDlgAutoBaseTerrain::GetTextureParameter(int a_nIdx, CAutoParameters::TEXTUREINFO* a_pTexInfo)
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
		strcpy(szFileName, "userdata\\home\\terraintex\\");
		strcat(szFileName, AC2AS(m_pListBoxTexture->GetText(a_nIdx)));
		if( !GetAutoParam()->LoadTextureInfo(szFileName, a_pTexInfo))
			return false;
	}

	return true;
}


void CDlgAutoBaseTerrain::OnCommandAdvance(const char* szCommand)
{
	//this->Show(false);

	UpdateData(true);
	
	GetHomeDlgsMgr()->m_pDlgAutoTerrain->Show(true);
	GetHomeDlgsMgr()->SetUnAdvDlgsDisable();

	CAutoParameters::MAINTERRAINHEIGHT mainTerr;
	if( !GetTerrainParameter(m_nTerrainIdx, &mainTerr))
			AutoHomeReport("CDlgAutoBaseTerrain::OnCommandCreateTerrain, Can not Get TerrainParameter!");

	//Ԥ������
	if( m_nTerrainIdx < m_iPreTerrainNum)
	{
	
		GetHomeDlgsMgr()->m_pDlgAutoTerrain->InitSettings(mainTerr.fMaxHeight, 
													mainTerr.nMainSmooth, 
													mainTerr.fPartSmooth, 
													mainTerr.dwSeed,
													mainTerr.fTextureRatio,
													mainTerr.fMainTexture,
													mainTerr.fPatchTexture,
													mainTerr.fPatchDivision,
													false);
	}
	else //�Զ�������
	{
		GetHomeDlgsMgr()->m_pDlgAutoTerrain->InitSettings(mainTerr.fMaxHeight, 
													mainTerr.nMainSmooth, 
													mainTerr.fPartSmooth, 
													mainTerr.dwSeed,
													mainTerr.fTextureRatio,
													mainTerr.fMainTexture,
													mainTerr.fPatchTexture,
													mainTerr.fPatchDivision,
													true, 
													AC2AS(m_pListBoxTerrain->GetText(m_nTerrainIdx)));
	}


	
}



void CDlgAutoBaseTerrain::OnCommandDelPre(const char* szCommand)
{
	UpdateData(true);

	if(m_nTerrainIdx < m_iPreTerrainNum || m_nTerrainIdx >= m_pListBoxTerrain->GetCount())
		return;

	ACHAR szFileName[MAX_PATH];
	a_strcpy( szFileName, _AL("userdata\\home\\terrain\\"));
	a_strcat(szFileName, m_pListBoxTerrain->GetText(m_nTerrainIdx));

	//ɾ����ǰ�ļ�
	DeleteFile(szFileName);
	

	//ɾ����ǰlistboxѡ��
	m_pListBoxTerrain->DeleteString(m_nTerrainIdx);
	
	//���õ�ǰlistboxѡ��
	if(m_nTerrainIdx < m_pListBoxTerrain->GetCount())
		m_pListBoxTerrain->SetCurSel(m_nTerrainIdx);
	else
	{
		m_nTerrainIdx = m_pListBoxTerrain->GetCount() -1;

		m_pListBoxTerrain->SetCurSel(m_nTerrainIdx);
		if( m_nTerrainIdx == m_iPreTerrainNum -1)
		{
			m_pBtn[BTN_DEL_TERRAIN]->Enable(false);
		}
	}
	UpdateData(false);

}


void CDlgAutoBaseTerrain::InsertTerrainSettings(const ACHAR* szFileName)
{
	int iIdx = m_pListBoxTerrain->GetCount();
	m_pListBoxTerrain->InsertString(iIdx, szFileName);
	
}

void CDlgAutoBaseTerrain::OnLButtonUpListBoxTerrain(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	//�ж��Ƿ�����б���е�ѡ��
	POINT ptPos = pObj->GetPos();

	A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
	int nMouseX = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int nMouseY = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if(m_pListBoxTerrain->GetHitArea(nMouseX, nMouseY)!= AUILISTBOX_RECT_TEXT)
		return;

	int nCurSel = m_pListBoxTerrain->GetCurSel();

	if( nCurSel >= 0 && nCurSel < m_iPreTerrainNum) // Ԥ��
	{
		m_pBtn[BTN_CREATE_TERRAIN]->Enable(true);
		m_pBtn[BTN_ADV_TERRAIN]->Enable(true);
		m_pBtn[BTN_DEL_TERRAIN]->Enable(false);
		
	}
	else if( nCurSel < m_pListBoxTerrain->GetCount()) //�Զ���
	{
		m_pBtn[BTN_CREATE_TERRAIN]->Enable(true);
		m_pBtn[BTN_ADV_TERRAIN]->Enable(true);
		m_pBtn[BTN_DEL_TERRAIN]->Enable(true);
	}
	else //���ڿ�λ��											
	{
		m_pBtn[BTN_CREATE_TERRAIN]->Enable(false);
		m_pBtn[BTN_ADV_TERRAIN]->Enable(false);
		m_pBtn[BTN_DEL_TERRAIN]->Enable(false);
	}

	return;
}

void CDlgAutoBaseTerrain::OnCommandAdvTex(const char* szCommand)
{
	UpdateData(true);
	
	
	CAutoParameters::TEXTUREINFO texInfo;
	if( !GetTextureParameter(m_nTextureIdx, &texInfo))
			AutoHomeReport("CDlgAutoBaseTerrain::OnCommandAdvTex, Can not Get TextureParameter!");

	TextureParamInfo_t texParam;
	GetHomeDlgsMgr()->TransformTexInfoToTexParam(texInfo, texParam);

	//Ԥ������
	if( m_nTextureIdx < m_iPreTexNum)
	{
	
		GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->InitTextureParamInfo(texParam, CTIP_TERRAIN);
	}
	else //�Զ�������
	{
		GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->InitTextureParamInfo(texParam, CTIP_TERRAIN,
													true, 
													AC2AS(m_pListBoxTexture->GetText(m_nTextureIdx)));
	}

	GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->Show(true);
	GetHomeDlgsMgr()->SetUnAdvDlgsDisable();

}

void CDlgAutoBaseTerrain::OnCommandDelTex(const char* szCommand)
{
	UpdateData(true);

	if(m_nTextureIdx < m_iPreTexNum || m_nTextureIdx >= m_pListBoxTexture->GetCount())
		return;

	ACHAR szFileName[MAX_PATH];
	a_strcpy( szFileName, _AL("userdata\\home\\terraintex\\"));
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

void CDlgAutoBaseTerrain::InsertTexSettings(const ACHAR* szFileName)
{
	int iIdx = m_pListBoxTexture->GetCount();
	m_pListBoxTexture->InsertString(iIdx, szFileName);
	
}

void CDlgAutoBaseTerrain::OnLButtonUpListBoxTex(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
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
*/