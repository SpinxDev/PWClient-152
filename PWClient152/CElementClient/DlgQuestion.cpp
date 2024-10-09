// File		: DlgQuestion.cpp
// Creator	: Xiao Zhou
// Date		: 2007/7/12

#include "AUIRadioButton.h"
#include "DlgQuestion.h"
#include "DlgMinimizeBar.h"
#include "DlgInfo.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_TimeSafeChecker.h"
#include "acquestion.hpp"
#include "..\\celementdata\\vector_string.h"
#include "A3DWireCollector.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgQuestion, CDlgBase)

AUI_ON_COMMAND("minimize",		OnCommandMinimize)
AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

CDlgQuestion::CDlgQuestion()
{
	for(int i = 0; i < 6; i++)
		m_pAnswer[i] = NULL;
}

CDlgQuestion::~CDlgQuestion()
{
	for(int i = 0; i < 6; i++)
		if( m_pAnswer[i] )
			delete []m_pAnswer[i];
}

void CDlgQuestion::OnCommandMinimize(const char *szCommand)
{
	GetGameUIMan()->GetMiniBarMgr()->MinimizeDialog(this, 
		CECMiniBarMgr::BarInfo(GetStringFromTable(877), A3DCOLORRGB(255, 0, 0)));
}

void CDlgQuestion::OnCommandConfirm(const char *szCommand)
{
	int answer = 0;
	for(int i = 1; ; i++)
	{
		char szName[20];
		sprintf(szName, "Rdo_Answer%d", i);
		PAUIRADIOBUTTON pRadio = (PAUIRADIOBUTTON)GetDlgItem(szName);
		if( !pRadio )
			break;

		if( pRadio->IsChecked() )
			answer += 1 << (i - 1);
	}
	if( answer > 0 )
	{
		GetGameSession()->send_acanswer(type, seq, reserved, answer);
		Show(false);
	}
}

void CDlgQuestion::OnCommandCancel(const char *szCommand)
{
	GetGameUIMan()->RespawnMessage();
}

void CDlgQuestion::OnShowDialog()
{
	for(int i = 1; ; i++)
	{
		char szName[20];
		sprintf(szName, "Rdo_Answer%d", i);
		PAUIRADIOBUTTON pRadio = (PAUIRADIOBUTTON)GetDlgItem(szName);
		if( !pRadio )
			break;

		pRadio->Check(false);
	}
}

void CDlgQuestion::OnTick()
{
	GetDlgItem("Btn_Confirm")->Enable(false);
	for(int i = 1; ; i++)
	{
		char szName[20];
		sprintf(szName, "Rdo_Answer%d", i);
		PAUIRADIOBUTTON pRadio = (PAUIRADIOBUTTON)GetDlgItem(szName);
		if( !pRadio )
			break;

		if( pRadio && pRadio->IsChecked() )
		{
			GetDlgItem("Btn_Confirm")->Enable(true);
			break;
		}
	}
	int nRestTime = CECTimeSafeChecker::ElapsedTime(endtime, GetTickCount());
	if( nRestTime <= 0 )
		Show(false);
	ACString strText;
	if( nRestTime >= 60000 )
		strText.Format(GetStringFromTable(1602), nRestTime / 60000);
	else
		strText.Format(GetStringFromTable(1601), nRestTime / 1000);
	GetDlgItem("Txt_Time")->SetText(strText);
}

bool CDlgQuestion::Render()
{
	if( !CDlgBase::Render() )
		return false;
	
	if( type == 3 )
	{
		POINT pt = GetDlgItem("Image")->GetPos();
		A3DWireCollector *pWC = GetGame()->GetA3DEngine()->GetA3DWireCollector();
		const float fScaler = GetGameUIMan()->GetWindowScale();
		for(int j = 0; j < (int)m_vecPath.size(); j++)
		{
			A3DVECTOR3 *verts = (A3DVECTOR3*)a_malloctemp(sizeof(A3DVECTOR3)*m_vecPath[j].size());
			WORD *indices = (WORD*)a_malloctemp(sizeof(WORD) * (m_vecPath[j].size() * 2));
			for(int l = 0; l < (int)m_vecPath[j].size(); l++)
			{
				verts[l].x = (float)pt.x + m_vecPath[j][l].x * fScaler;
				verts[l].y = (float)pt.y + m_vecPath[j][l].y * fScaler;
				verts[l].z = 0.0f;
				if( l < (int)m_vecPath[j].size() - 1 )
				{
					indices[l * 2] = l;
					indices[l * 2 + 1] = l + 1;
				}
			}
			pWC->AddRenderData_2D(verts, m_vecPath[j].size(), indices, m_vecPath[j].size() * 2 - 2, A3DCOLORRGB(255, 255, 255));
			a_freetemp(indices);
			a_freetemp(verts);
		}
		pWC->Flush_2D();
	}
	if( type == 2 )
	{
		A3DWireCollector *pWC = GetGame()->GetA3DEngine()->GetA3DWireCollector();
		for(int i = 0; i < 6; i++ )
			if( m_pAnswer[i] )
			{
				char szName[20];
				sprintf(szName, "Txt_Answer%d", i + 1);
				PAUILABEL pAnswer = (PAUILABEL)GetDlgItem(szName);
				POINT pt = pAnswer->GetPos();
				CVectorString vstring;
				vstring.LoadFromMem(m_pAnswer[i]);
				for(int j = 0; j < vstring.GetCharNum(); j++)
				{
					VECTOR_CHAR *c = vstring.GetChar(j);

					int px = int(16 + 2) * j;
					for(int k = 0; k < c->m_nPathNum; k++)
					{
						VECTOR_PATH *p = c->m_pPathData[k];
						A3DVECTOR3 *verts = (A3DVECTOR3*)a_malloctemp(sizeof(A3DVECTOR3) * p->m_nPtNum);
						WORD *indices = (WORD*)a_malloctemp(sizeof(WORD) * (p->m_nPtNum * 2));
						for(int l = 0; l < p->m_nPtNum; l++)
						{
							verts[l].x = (float)px + pt.x + (p->m_pPtData[l] >> 4 & 15);
							verts[l].y = (float)pt.y + (15 - p->m_pPtData[l] & 15);
							verts[l].z = 0.0f;
							if( l < p->m_nPtNum - 1 )
							{
								indices[l * 2] = l;
								indices[l * 2 + 1] = l + 1;
							}
						}
						pWC->AddRenderData_2D(verts, p->m_nPtNum, indices, p->m_nPtNum * 2 - 2, A3DCOLORRGB(255, 255, 255));
						a_freetemp(indices);
						a_freetemp(verts);
					}
				}
			}
		pWC->Flush_2D();
	}
	return true;
}

void CDlgQuestion::SetQuestion(void *pData)
{
	GNET::ACQuestion* p = (GNET::ACQuestion*)pData;
	if( p->question.size() == 0 )
		return;

	type = p->qtype;
	seq = p->seq;
	reserved = p->reserved;

	static BYTE mask[32] = {
			0x1f, 0x90, 0x82, 0x99, 0x33, 0x23, 0x65, 0x7e,
			0x78, 0x89, 0x12, 0x56, 0xe9, 0x8e, 0x32, 0xf1, 
			0xf4, 0xe9, 0xe4, 0x3b, 0x88, 0x23, 0x11, 0xa5,
			0x3f, 0x4a, 0x3e, 0x5b, 0x79, 0x80, 0x31, 0x23
	};

	if( type == 3 )
	{
		char *pPath = (char*)a_malloctemp(sizeof(char)*p->question[0].size());
		memcpy(pPath, p->question[0].begin(), p->question[0].size());
		for(DWORD n=0; n < p->question[0].size(); n++)
		{
			BYTE t = pPath[n];
			pPath[n] = (((t & 0xe0) >> 5) | ((t & 0x1f) << 3));
			pPath[n] = pPath[n] ^ mask[n % 32];
		}

		char *pp = pPath;
		int nPathNum = *(int*)pp; // 笔划的个数
		m_vecPath.clear();
		pp += sizeof(int);
		for(int i=0; i<nPathNum; ++i)
		{
			VECTOR_POINT vecPoint;
			vecPoint.clear();
			unsigned char nPoint = *pp; // 每个笔划中点的个数
			pp += sizeof(unsigned char);

			for(unsigned char j=0; j<nPoint; ++j)
			{
				POINT pt;
				pt.x = *((BYTE*)pp);
				pp += sizeof(unsigned char);
				pt.y = *((BYTE*)pp);
				pp += sizeof(unsigned char);
				vecPoint.push_back(pt);
			}
			m_vecPath.push_back(vecPoint);
		}

		GetDlgItem("Txt_Question")->SetText(ACString((const ACHAR*)pp, (p->question[0].size() - (pp - pPath)) / sizeof (ACHAR)));
		a_freetemp(pPath);
		GetDlgItem("Shape1")->Show(true);
	}
	else
	{
		GetDlgItem("Txt_Question")->SetText(ACString((const ACHAR*)p->question[0].begin(), p->question[0].size() / sizeof (ACHAR)));
		GetDlgItem("Shape1")->Show(false);
	}

	int i(0);
	for(i = 0; i < 6; i++)
		if( m_pAnswer[i] )
		{
			delete []m_pAnswer[i];
			m_pAnswer[i] = NULL;
		}
	for(i = 1; ; i++)
	{
		char szName[20];
		sprintf(szName, "Txt_Answer%d", i);
		PAUILABEL pAnswer = (PAUILABEL)GetDlgItem(szName);
		sprintf(szName, "Rdo_Answer%d", i);
		PAUIRADIOBUTTON pRadio = (PAUIRADIOBUTTON)GetDlgItem(szName);
		if( pAnswer )
		{
			if( i < (int)p->question.size() )
			{
				if( type == 2 )
				{
					m_pAnswer[i - 1] = new BYTE[p->question[i].size()];
					memcpy(m_pAnswer[i - 1], p->question[i].begin(), p->question[i].size());
					for(DWORD n=0; n < p->question[i].size(); n++)
					{
						BYTE t = m_pAnswer[i - 1][n];
						m_pAnswer[i - 1][n] = (((t & 0xe0) >> 5) | ((t & 0x1f) << 3));
						m_pAnswer[i - 1][n] = m_pAnswer[i - 1][n] ^ mask[n % 32];
					}
					pAnswer->SetText(_AL(""));
				}
				else
					pAnswer->SetText(ACString((const ACHAR*)p->question[i].begin(), p->question[i].size() / sizeof(ACHAR)));
				pAnswer->Show(true);
				pRadio->Show(true);
			}
			else
			{
				pAnswer->Show(false);
				pRadio->Show(false);
			}
		}
		else
			break;
	}
	endtime = GetTickCount() + 60000 * 6;
	GetGameUIMan()->AddInformation(CDlgInfo::INFO_QUESTION, "Game_Question", 
		GetStringFromTable(905), 60000 * 5, 0, 0, 0);
}