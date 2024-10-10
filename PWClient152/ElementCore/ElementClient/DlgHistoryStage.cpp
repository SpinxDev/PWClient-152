// Filename	: DlgHistoryStage.cpp 
// Creator	: WYD


#include "DlgHistoryStage.h"

#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_HostPlayer.h"
#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameSession.h"
#include "EC_World.h"
#include "EC_UIConfigs.h"

#include "AUIProgress.h"

#define new A_DEBUG_NEW


AUI_BEGIN_EVENT_MAP(CDlgHistoryStage, CDlgBase)

AUI_END_EVENT_MAP()

CDlgHistoryStage::CDlgHistoryStage():m_stage_idx(-1)
{

}

CDlgHistoryStage::~CDlgHistoryStage()
{
}
void CDlgHistoryStage::OnShowDialog()
{	
	GetProgressData();
	OnRefreshPos();
}
void CDlgHistoryStage::OnRefreshPos()
{
	CECGameRun::HistoryProgConfig* p = GetGameRun()->GetCurStageProgConfig(GetGameRun()->GetWorld()->GetInstanceID());
	if (p)
	{
		PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_WorldMap");	
		PAUIOBJECT pObj = pDlg->GetDlgItem("WorldMap");
		
		SIZE s = pObj->GetSize();
		POINT pos = pObj->GetPos(true);
		//		int x = pos.x + s.cx * p->x/100;
		//		int y = pos.y + s.cy * 0.125 + s.cy * 0.75*p->y/100.f;
		int realW = s.cx;
		int realH = s.cy * 0.75; // 地图 （比如 world.dds） 实际高度 768 / 1024.
		int right_bottom_x = pos.x + s.cx;
		int right_bottom_y = pos.y + s.cy * 0.125 + realH; // s.cy * 0.125 是单个黑边的高度。 (1 - 768/1024) * 0.5
		right_bottom_x -= s.cx * p->x/100;
		right_bottom_y -= realH * p->y / 100.f;
		int x = right_bottom_x - GetSize().cx;
		int y = right_bottom_y - GetSize().cy;
		
		SetPosEx(x,y);
	}
}
void CDlgHistoryStage::GetProgressData()
{
	CECGameRun::unique_data* data = GetGameRun()->GetUniqueData(0);
	abase::vector<int> vec;
	if (data)
	{
		if(data->type ==1)
		{
			int id = GetGameRun()->GetStageID(data->GetValueAsInt());
			int key = GetGameRun()->GetStageVarKey(id);
			if(key>=0)
			{				
				vec.push_back(key);
				GetGameSession()->c2s_CmdQueryUniqueData(vec);
			}
		}
	}
	else
	{
		vec.push_back(0);
		GetGameSession()->c2s_CmdQueryUniqueData(vec);
	}
}
void CDlgHistoryStage::OnTick()
{
	m_query_counter += g_pGame->GetRealTickTime();
	if (m_query_counter>CECUIConfig::Instance().GetGameUI().nHistoryQueryTimeInterval) // 10s
	{
		m_query_counter = 0;
		GetProgressData();		
	}

	ACString desc,name;
	float prog = 0.0f;
	if(GetGameRun()->GetCurStageInfo(name,desc,prog))
	{
		// 设置界面
		GetDlgItem("Txt_Title")->SetText(name);
		GetDlgItem("Txt_Description")->SetText(desc);
		PAUIPROGRESS p = (PAUIPROGRESS)GetDlgItem("Pro_Stage");
		int iValue = (int)(prog * 100);
		a_Clamp(iValue,0,99);
		if (p)
			p->SetProgress(iValue);
		PAUIOBJECT pLab = GetDlgItem("Txt_Per");
		if(pLab)
		{
			ACString str;
			str.Format(_AL("%d%%"),iValue);
			pLab->SetText(str);
		}
	}
	//int id = GetGameRun()->GetStageID(GetGameRun()->GetCurStageIndex());
	
	if( !GetGameRun()->IsInCurStage(GetGameRun()->GetWorld()->GetInstanceID()))
	{
		Show(false);
	}
}
