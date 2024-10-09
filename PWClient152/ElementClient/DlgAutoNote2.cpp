/*
 * FILE: DlgAutoNote.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/2/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

/*
#include "DlgAutoNote2.h"
#include "aui/AUILabel.h"
#include "aui/AUIStillImageButton.h"

#include "AutoSelectedArea.h"
#include "DlgAutoCamera.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoNote2, CDlgHomeBase)

AUI_ON_COMMAND("CloseArea", OnCommandCloseArea)
AUI_ON_COMMAND("DeleteEditPoint", OnCommandDeleteEditPoint)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoNote2, CDlgHomeBase)
AUI_END_EVENT_MAP()

CDlgAutoNote2::CDlgAutoNote2()
{
	m_pLabelNote2 = NULL;
	m_pBtnCloseArea = NULL;
	m_pBtnDelPoint = NULL;
}

CDlgAutoNote2::~CDlgAutoNote2()
{
}

void CDlgAutoNote2::OnShowDialog()
{
	SetCanMove(false);

	POINT pos = GetHomeDlgsMgr()->m_pDlgAutoCamera->GetPos();
	SIZE size = GetHomeDlgsMgr()->m_pDlgAutoCamera->GetSize();

	this->SetPosEx(pos.x, pos.y + size.cy);
}

bool CDlgAutoNote2::OnInitContext()
{
	return true;
}

void CDlgAutoNote2::OnCommandCloseArea(const char* szCommand)
{
	GetAutoScene()->GetAutoSelectedArea()->SetPolyEditAreaClosed(true);
}


void CDlgAutoNote2::OnCommandDeleteEditPoint(const char* szCommand)
{
	CAutoSelectedArea* pSelArea = GetAutoScene()->GetAutoSelectedArea();

	if( pSelArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
	{
		if( pSelArea->GetPolyEditAreaClosed())
		{
			pSelArea->SetPolyEditAreaClosed(false);
		}
		else
		{
			pSelArea->DeletePolyLastEditPoint();
		}
	}
	else if( pSelArea->GetAreatype() == CAutoSelectedArea::AT_LINE)
	{
		pSelArea->DeleteLineLastEditPoint();
	}
}

void CDlgAutoNote2::DoDataExchange(bool bSave)
{
	DDX_Control("Label_Note2", m_pLabelNote2);
	DDX_Control("Btn_CloseArea", m_pBtnCloseArea);
	DDX_Control("Btn_DelPoint", m_pBtnDelPoint);
}

void CDlgAutoNote2::SetNote2(AreaType a_areaType)
{
	switch(a_areaType)
	{
	case AT_POLY:
		m_pLabelNote2->SetText(GetStringFromTable(1001));
		m_pBtnCloseArea->Enable(true);
		m_pBtnDelPoint->Enable(true);
		break;
	case AT_LINE:
		m_pLabelNote2->SetText(GetStringFromTable(1002));
		m_pBtnCloseArea->Enable(false);
		m_pBtnDelPoint->Enable(true);
		break;
	case AT_MODEL:
		m_pLabelNote2->SetText(GetStringFromTable(1003));
		m_pBtnCloseArea->Enable(false);
		m_pBtnDelPoint->Enable(false);
		break;
	default:
		break;
	}
}
*/