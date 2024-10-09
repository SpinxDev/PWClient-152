/*
 * FILE: DlgAutoGenLightMapProgress.cpp
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
#include "DlgAutoGenLightMapProgress.h"
#include "DlgAutoTools.h"

#include "AutoLightMap.h"
#include "AutoTerrain.h"

#include "aui\\AUIProgress.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoGenLightMapProgress, CDlgHomeBase)

AUI_ON_COMMAND("OnCancel", OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoGenLightMapProgress, CDlgHomeBase)

AUI_END_EVENT_MAP()

CDlgAutoGenLightMapProgress::CDlgAutoGenLightMapProgress()
{
	m_pProgressGenLightMap = NULL;
	m_bRunLightMap = false;
	m_nStep = 0;
}

CDlgAutoGenLightMapProgress::~CDlgAutoGenLightMapProgress()
{
}

void CDlgAutoGenLightMapProgress::SetProgress(int i_nProgress)
{
	m_pProgressGenLightMap->SetProgress(i_nProgress);
	return;
}

void CDlgAutoGenLightMapProgress::OnShowDialog()
{
	this->SetCanMove(false);

	POINT pos = GetHomeDlgsMgr()->GetAutoToolsDlg()->GetPos();
	SIZE s = this->GetSize();
	int x = pos.x;
	int y = pos.y - s.cy;
	this->SetPosEx(x, y);
}

bool CDlgAutoGenLightMapProgress::OnInitContext()
{
	return true;
}

void CDlgAutoGenLightMapProgress::DoDataExchange(bool bSave)
{
	DDX_Control("Progress_GenLightMap", m_pProgressGenLightMap);
}

void CDlgAutoGenLightMapProgress::OnTick()
{
	static bool bEnable = true;

	if( m_bRunLightMap )
	{
		if( bEnable)
		{
			GetAutoScene()->GetAutoTerrain()->SetLightMapEnable(true);
			bEnable = false;
		}
		

		GetAutoScene()->UpdateLightMap(m_nStep);
		m_nStep++;
		
		SetProgress(m_nStep * 10);

		if( m_nStep >LMS_RELEASE_NIGHT)
		{
			m_bRunLightMap = false;
			m_nStep = 0;
			
		}
	}
	else
	{
		this->Show(false);
		
		bEnable = true;
	}
	return;
	
}

void CDlgAutoGenLightMapProgress::OnCommandCancel(const char* pszCommand)
{
	m_bRunLightMap = false;
	m_nStep = 0;
	GetAutoScene()->GetAutoLightMap()->Release();
	
	this->Show(false);
}
*/