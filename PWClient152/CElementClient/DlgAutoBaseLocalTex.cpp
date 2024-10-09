/*
 * FILE: DlgAutoBaseLocalTex.cpp
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
#include "DlgAutoBaseLocalTex.h"
#include "DlgAutoLocalTexture.h"
#include "DlgAutoAllRes.h"
#include "DlgAutoSwitchDlg.h"
#include "DlgAutoChooseTexture.h"

#include "aui\\AUIListBox.h"
#include "aui\\AUIManager.h"
#include "aui\\AUIStillImageButton.h"

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
#include "AutoPartTexture.h"

#include "AutoParameters.h"
#include "PolygonMountain.h"
#include "AutoBuildingOperation.h"
#include "AutoSelectedArea.h"
#include "AutoTerrainAction.h"


#include "windowsx.h"

#include "A3DMacros.h"

#include "io.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoBaseLocalTex, CDlgHomeBase)

AUI_ON_COMMAND("CreateLocalTex", OnCommandCreateLocalTex)
AUI_ON_COMMAND("Advance", OnCommandAdvance)
AUI_ON_COMMAND("DelLocalTex", OnCommandDelPre)

AUI_ON_COMMAND("ApplyTex", OnCommandApplyTex)
AUI_ON_COMMAND("AdvTex", OnCommandAdvTex)
AUI_ON_COMMAND("DelTex", OnCommandDelTex)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoBaseLocalTex, CDlgHomeBase)

AUI_ON_EVENT("ListBox_LocalInfo", WM_LBUTTONUP, OnLButtonUpListBoxLocalTex)
AUI_ON_EVENT("ListBox_Texture", WM_LBUTTONUP, OnLButtonUpListBoxTex)

AUI_END_EVENT_MAP()



CDlgAutoBaseLocalTex::CDlgAutoBaseLocalTex()
{
	m_pListBoxTexture = NULL;
	m_nTextureIdx = 0;

	m_pListBoxLocalInfo = NULL;
	m_nLocalInfoIdx = 0;

	

	m_iPreLocalTexNum = 0;
	m_iPreTexNum = 0;

	for( int i = 0; i < BTN_NUM; i++)
	{
		m_pBtn[i] = NULL;
	}
}

CDlgAutoBaseLocalTex::~CDlgAutoBaseLocalTex()
{
}
	
AUIStillImageButton * CDlgAutoBaseLocalTex::GetSwithButton()
{
	return GetHomeDlgsMgr()->m_pBtnBaseLocalTex;
}

void CDlgAutoBaseLocalTex::OnShowDialog()
{
	SetCanMove(false);
	
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->GetSize();
	
	POINT pos = this->GetPos();
	this->SetPosEx(pos.x, size.cy);
}

bool CDlgAutoBaseLocalTex::OnInitContext()
{
	if(!InitResource())
		return false;
	
	LoadDefault();
	UpdateData(false);
	return true;
}

#define GET_BTN_POINTER(a) DDX_Control( #a, m_pBtn[a]);

void CDlgAutoBaseLocalTex::DoDataExchange(bool bSave)
{
	DDX_Control("ListBox_Texture", m_pListBoxTexture);
	DDX_ListBox(bSave, "ListBox_Texture", m_nTextureIdx);

	DDX_Control("ListBox_LocalInfo", m_pListBoxLocalInfo);
	DDX_ListBox(bSave, "ListBox_LocalInfo", m_nLocalInfoIdx);
	

	GET_BTN_POINTER(BTN_CREATE_LOCAL_TEX);
	GET_BTN_POINTER(BTN_ADV_LOCAL_TEX);
	GET_BTN_POINTER(BTN_DEL_LOCAL_TEX);

	GET_BTN_POINTER(BTN_APPLY_TEX);
	GET_BTN_POINTER(BTN_ADV_TEX);
	GET_BTN_POINTER(BTN_DEL_TEX);
}

void CDlgAutoBaseLocalTex::LoadDefault()
{
	m_nTextureIdx = 0;
	m_nLocalInfoIdx = 0;

	m_pBtn[BTN_CREATE_LOCAL_TEX]->Enable(true);
	m_pBtn[BTN_ADV_LOCAL_TEX]->Enable(true);
	m_pBtn[BTN_DEL_LOCAL_TEX]->Enable(false);

	m_pBtn[BTN_APPLY_TEX]->Enable(false);
	m_pBtn[BTN_ADV_TEX]->Enable(false);
	m_pBtn[BTN_DEL_TEX]->Enable(false);
}

bool CDlgAutoBaseLocalTex::InitResource()
{
	FillTextureListBox();
	FillLocalInfoListBox();

	return true;
}

void CDlgAutoBaseLocalTex::FillTextureListBox()
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
	long lFileHandle = _findfirst( "userdata\\home\\localtextex\\*.*", &fdIni);//��õ�һ���ļ��ľ��
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

void CDlgAutoBaseLocalTex::FillLocalInfoListBox()
{
	m_pListBoxLocalInfo->ResetContent();

	CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetPartTextureSet();

	//Ԥ������
	if( pInfo)
	{
		m_iPreLocalTexNum = GetAutoParam()->GetNumPartTextureSet();
		for( int i = 0; i < m_iPreLocalTexNum ; i++)
		{
			m_pListBoxLocalInfo->InsertString(i, AS2AC(pInfo[i].strName));
		}
	}

	//�Զ�������
	_finddata_t fdIni;//�ļ�����
	long lFileHandle = _findfirst( "userdata\\home\\localtex\\*.*", &fdIni);//��õ�һ���ļ��ľ��
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
		m_pListBoxLocalInfo->InsertString(m_pListBoxLocalInfo->GetCount(), AS2AC(fdIni.name));
		
		//��ȡ��һ���û��Զ��������ļ��ľ��
		nResult = _findnext( lFileHandle, &fdIni);
	}
	
	//�رվ��
	_findclose( lFileHandle);
}

bool CDlgAutoBaseLocalTex::GetTextureParameter(int a_nIdx, TEXTUREINFO* a_pTexInfo)
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
		strcpy(szFileName, "userdata\\home\\localtextex\\");
		strcat(szFileName, AC2AS(m_pListBoxTexture->GetText(a_nIdx)));
		if( !GetAutoParam()->LoadTextureInfo(szFileName, a_pTexInfo))
			return false;
	}

	return true;

}

bool CDlgAutoBaseLocalTex::GetLocalInfoParameter(int a_nIdx, CAutoParameters::PARTTEXTUREINFO* a_pLocalInfo)
{
	UpdateData(true);
	if( !IsValidIdx(a_nIdx, m_pListBoxLocalInfo))
		return false;
	
	if( a_nIdx < m_iPreLocalTexNum) //Ԥ������
	{
		CAutoParameters::PRESETINFO* pInfo = GetAutoParam()->GetPartTextureSet();
		if( !GetAutoParam()->LoadPartTextureInfo(pInfo[a_nIdx].strPath, a_pLocalInfo))
			return false;
	}
	else //�û�����
	{
		char szFileName[MAX_PATH];
		strcpy(szFileName, "userdata\\home\\localtex\\");
		strcat(szFileName, AC2AS(m_pListBoxLocalInfo->GetText(a_nIdx)));
		if( !GetAutoParam()->LoadPartTextureInfo(szFileName, a_pLocalInfo))
			return false;
	}


	return true;
}

void CDlgAutoBaseLocalTex::OnCommandCreateLocalTex(const char* szCommand)
{
	UpdateData(true);

	if (GetOperationType() != OT_TEXTURE_POLY)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	CAutoScene* pScene = GetAutoScene();
	CAutoPartTexture* pPartTexture = pScene->GetAutoPartTexture();
	if (NULL == pPartTexture)
		return;

	CAutoSelectedArea* pSelArea = pScene->GetAutoSelectedArea();
	int nNum = pSelArea->GetNumPolyEditPoints();
	POINT_FLOAT* pPoints = pSelArea->GetPolyEditPoints();
	
	
	pPartTexture->SetAreaPoints(nNum, pPoints);

	bool bClosed = pSelArea->GetPolyEditAreaClosed();
	if (!bClosed)
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}



	CAutoParameters::PARTTEXTUREINFO localInfo;
	if( !GetLocalInfoParameter(m_nLocalInfoIdx, &localInfo))
	{
		AutoHomeReport("CDlgAutoBaseLocalTex::OnCommandCreateLocalTex, Failed to Get LocalInfo!");
		return ;
	}

	if (!pPartTexture->CreatePartTexture(
		localInfo.fTextureRatio, 
		localInfo.fMainTexture,
		localInfo.fPatchTexture,
		localInfo.fPatchDivision,
		localInfo.dwSeed,
		localInfo.fBorderScope,
		localInfo.nTextureType))
	{
		GetAUIManager()->MessageBox("", GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	// Store action
	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();
	CClosedArea* pArea = pScene->GetAutoPartTexture()->GetArea();

	if( GetHomeDlgsMgr()->GetModifying())
	{

		int nIdx = GetHomeDlgsMgr()->m_pDlgAutoAllRes->GetTreeViewSelItemIdx();
		if (!pAction->ChangeActionPartTexture(
					localInfo.fTextureRatio, 
					localInfo.fMainTexture,
					localInfo.fPatchTexture,
					localInfo.fPatchDivision,
					localInfo.dwSeed,
					localInfo.fBorderScope,
					pArea->GetNumPoints(),
					pArea->GetFinalPoints(),
				localInfo.nTextureType, nIdx))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		
		GetAutoScene()->RefreshAllPartTextureActions();
	}
	else
	{
		if (!pAction->AddActionPartTexture(
					localInfo.fTextureRatio, 
					localInfo.fMainTexture,
					localInfo.fPatchTexture,
					localInfo.fPatchDivision,
					localInfo.dwSeed,
					localInfo.fBorderScope,
					pArea->GetNumPoints(),
					pArea->GetFinalPoints(),
				localInfo.nTextureType))
		{
			GetAUIManager()->MessageBox("", GetStringFromTable(1004), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		
	
		
	}



	GetAutoScene()->RecreateTerrainRender(true);

	//Clear points
	pSelArea->CopyPointsFromPolyEditToView();
	pSelArea->SetViewAreaClosed(true);
	pSelArea->DeleteAllPolyEditPoints();

	//Add Res item
	if( !GetHomeDlgsMgr()->GetModifying())
	{
		ACHAR szName[100];
		a_sprintf( szName, _AL("%s%d"),GetStringFromTable(1005), m_nActionCount );
		m_nActionCount++;
		
		GetHomeDlgsMgr()->m_pDlgAutoAllRes->InsertResTreeChildItem(CDlgAutoAllRes::ARTI_LOCALTEX, szName);
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



void CDlgAutoBaseLocalTex::SetBtnCreateToModify()
{
	m_pBtn[BTN_CREATE_LOCAL_TEX]->SetText( GetStringFromTable(1005) );
	return;
}

void CDlgAutoBaseLocalTex::SetBtnModifyToCreate()
{
	m_pBtn[BTN_CREATE_LOCAL_TEX]->SetText( GetStringFromTable(1006) );
	return;
}

void CDlgAutoBaseLocalTex::InsertLocalTexSettings(const ACHAR* szFileName)
{
	int iIdx = m_pListBoxLocalInfo->GetCount();
	m_pListBoxLocalInfo->InsertString(iIdx, szFileName);
}

void CDlgAutoBaseLocalTex::OnCommandAdvance(const char* szCommand)
{
	UpdateData(true);
	
	GetHomeDlgsMgr()->m_pDlgAutoLocalTexture->Show(true);
	GetHomeDlgsMgr()->SetUnAdvDlgsDisable();

	CAutoParameters::PARTTEXTUREINFO localtexInfo;
	if( !GetLocalInfoParameter(m_nLocalInfoIdx, &localtexInfo))
			AutoHomeReport("CDlgAutoBaseLocalTex::OnCommandAdvance, Can not Get Local Tex Parameter!");

	//Ԥ������
	if( m_nLocalInfoIdx < m_iPreLocalTexNum)
	{
	
		GetHomeDlgsMgr()->m_pDlgAutoLocalTexture->InitSettings(localtexInfo.fBorderScope, 
													localtexInfo.dwSeed, 
													localtexInfo.nTextureType, 
													localtexInfo.fTextureRatio,
													localtexInfo.fMainTexture,
													localtexInfo.fPatchTexture,
													localtexInfo.fPatchDivision,
													false);
	}
	else //�Զ�������
	{
			GetHomeDlgsMgr()->m_pDlgAutoLocalTexture->InitSettings(localtexInfo.fBorderScope, 
													localtexInfo.dwSeed, 
													localtexInfo.nTextureType, 
													localtexInfo.fTextureRatio,
													localtexInfo.fMainTexture,
													localtexInfo.fPatchTexture,
													localtexInfo.fPatchDivision,
													true,
													AC2AS(m_pListBoxLocalInfo->GetText(m_nLocalInfoIdx)));
	}
}

void CDlgAutoBaseLocalTex::OnCommandDelPre(const char* szCommand)
{
	UpdateData(true);

	if(m_nLocalInfoIdx < m_iPreLocalTexNum || m_nLocalInfoIdx >= m_pListBoxLocalInfo->GetCount())
		return;

	ACHAR szFileName[MAX_PATH];
	a_strcpy( szFileName, _AL("userdata\\home\\LocalTex\\"));
	a_strcat(szFileName, m_pListBoxLocalInfo->GetText(m_nLocalInfoIdx));

	//ɾ����ǰ�ļ�
	DeleteFile(szFileName);
	

	//ɾ����ǰlistboxѡ��
	m_pListBoxLocalInfo->DeleteString(m_nLocalInfoIdx);
	
	//���õ�ǰlistboxѡ��
	if(m_nLocalInfoIdx < m_pListBoxLocalInfo->GetCount())
		m_pListBoxLocalInfo->SetCurSel(m_nLocalInfoIdx);
	else
	{
		m_nLocalInfoIdx = m_pListBoxLocalInfo->GetCount() -1;

		m_pListBoxLocalInfo->SetCurSel(m_nLocalInfoIdx);
		if( m_nLocalInfoIdx == m_iPreLocalTexNum -1)
		{
			m_pBtn[BTN_DEL_LOCAL_TEX]->Enable(false);
		}
	}
	UpdateData(false);

}

void CDlgAutoBaseLocalTex::OnLButtonUpListBoxLocalTex(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
		//�ж��Ƿ�����б���е�ѡ��
	POINT ptPos = pObj->GetPos();

	A3DVIEWPORTPARAM *p = GetGame()->GetA3DEngine()->GetActiveViewport()->GetParam();
	int nMouseX = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int nMouseY = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if(m_pListBoxLocalInfo->GetHitArea(nMouseX, nMouseY)!= AUILISTBOX_RECT_TEXT)
		return;

	int nCurSel = m_pListBoxLocalInfo->GetCurSel();

	if( nCurSel >= 0 && nCurSel < m_iPreLocalTexNum) // Ԥ��
	{
		m_pBtn[BTN_CREATE_LOCAL_TEX]->Enable(true);
		m_pBtn[BTN_ADV_LOCAL_TEX]->Enable(true);
		m_pBtn[BTN_DEL_LOCAL_TEX]->Enable(false);
		
	}
	else if( nCurSel < m_pListBoxLocalInfo->GetCount()) //�Զ���
	{
		m_pBtn[BTN_CREATE_LOCAL_TEX]->Enable(true);
		m_pBtn[BTN_ADV_LOCAL_TEX]->Enable(true);
		m_pBtn[BTN_DEL_LOCAL_TEX]->Enable(true);
	}
	else //���ڿ�λ��											
	{
		m_pBtn[BTN_CREATE_LOCAL_TEX]->Enable(false);
		m_pBtn[BTN_ADV_LOCAL_TEX]->Enable(false);
		m_pBtn[BTN_DEL_LOCAL_TEX]->Enable(false);
	}

	return;
}

void CDlgAutoBaseLocalTex::OnCommandApplyTex(const char* szCommand)
{
	UpdateData(true);

	CAutoScene* pScene = GetAutoScene();
	
	CAutoTexture* pTexture = pScene->GetAutoTerrain()->GetTexture();
	
	CAutoParameters::TEXTUREINFO texInfo;

	if(!GetTextureParameter(m_nTextureIdx, &texInfo))
		AutoHomeReport("CDlgAutoBaseTerrain::OnCommandCreateTexture, Can not Get Texture Parameter!");

	CAutoTerrainAction* pAction = pScene->GetAutoTerrainAction();
	
	pTexture->ChangeTextureParam(
		TT_PART,
		TT_PART_PATCH,
		texInfo.fMainTextureU,
		texInfo.fMainTextureV,
		texInfo.fPatchTextureU,
		texInfo.fPatchTextureV,
		texInfo.dwMainTextureID,
		texInfo.dwPatchTextureID);


	pAction->AddActionPartTextureInfo(
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

void CDlgAutoBaseLocalTex::OnCommandDelTex(const char* szCommand)
{
	UpdateData(true);

	if(m_nTextureIdx < m_iPreTexNum || m_nTextureIdx >= m_pListBoxTexture->GetCount())
		return;

	ACHAR szFileName[MAX_PATH];
	a_strcpy( szFileName, _AL("userdata\\home\\localtextex\\"));
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

void CDlgAutoBaseLocalTex::InsertTexSettings(const ACHAR* szFileName)
{
	int iIdx = m_pListBoxTexture->GetCount();
	m_pListBoxTexture->InsertString(iIdx, szFileName);
	
}
void CDlgAutoBaseLocalTex::OnLButtonUpListBoxTex(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
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

void CDlgAutoBaseLocalTex::OnCommandAdvTex(const char* szCommand)
{
	UpdateData(true);
	
	
	CAutoParameters::TEXTUREINFO texInfo;
	if( !GetTextureParameter(m_nTextureIdx, &texInfo))
			AutoHomeReport("CDlgAutoBaseLocalTex::OnCommandAdvTex, Can not Get TextureParameter!");

	TextureParamInfo_t texParam;
	GetHomeDlgsMgr()->TransformTexInfoToTexParam(texInfo, texParam);

		//Ԥ������
	if( m_nTextureIdx < m_iPreTexNum)
	{
	
		GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->InitTextureParamInfo(texParam, CTIP_LOCALTEX);
	}
	else //�Զ�������
	{
		GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->InitTextureParamInfo(texParam, CTIP_LOCALTEX,
													true, 
													AC2AS(m_pListBoxTexture->GetText(m_nTextureIdx)));
	}

	GetHomeDlgsMgr()->m_pDlgAutoChooseTexture->Show(true);
	GetHomeDlgsMgr()->SetUnAdvDlgsDisable();

}

void CDlgAutoBaseLocalTex::SetTexOpEnable(bool i_bEnable)
{
	m_pBtn[BTN_ADV_TEX]->Enable(i_bEnable);
	m_pBtn[BTN_APPLY_TEX]->Enable(i_bEnable);
}
*/