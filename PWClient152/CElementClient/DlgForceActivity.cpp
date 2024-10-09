// Filename	: DlgForceActivity.cpp
// Creator	: Feng Ning
// Date		: 2011/10/08

#include "DlgForceActivity.h"
#include "AUICTranslate.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "A3DDevice.h"
#include "A3DFlatCollector.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_ForceMgr.h"
#include "EC_Utility.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

#define DIAGRAM_PRECISION 30
#define DIAGRAM_VERTEX (DIAGRAM_PRECISION + 2) // center + split count + 1
#define DIAGRAM_INDEX (DIAGRAM_PRECISION * 3) // the index for render diagram

AUI_BEGIN_COMMAND_MAP(CDlgForceActivity, CDlgBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgForceActivity, CDlgBase)
AUI_ON_EVENT("Img_Canvas", WM_LBUTTONUP,		OnEvent_Canvas)
AUI_END_EVENT_MAP()

CDlgForceActivity::CDlgForceActivity()
{
	m_pCanvas = NULL;
	m_pDiagramRender = new CECDiagramRender();
}

CDlgForceActivity::~CDlgForceActivity()
{
	delete m_pDiagramRender;
	m_pDiagramRender = NULL;
}

void CDlgForceActivity::OnTick()
{
}

void CDlgForceActivity::OnShowDialog()
{
	m_pDiagramRender->ClearSummary();

	// show the diagram of activity first
	CECForceMgr::FORCE_GLOBAL detail;
	int activeType = ((int*)(&detail.activity)) - ((int*)&detail);

	m_pDiagramRender->RefreshSummary(activeType, m_pCanvas);

	// hide if no valid data
	if(!m_pDiagramRender->GetSummary())
	{
		Show(false);
	}
}

bool CDlgForceActivity::OnInitDialog()
{
	if( !CDlgBase::OnInitDialog() )
		return false;

	m_pDiagramRender->Init();
	m_pCanvas = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Canvas"));

	return true;
}

bool CDlgForceActivity::Render()
{
	if( !AUIDialog::Render() )
		return false;

	// render the diagram
	if(m_pDiagramRender->GetSummary())
	{
		m_pDiagramRender->Render(m_pDiagramRender->GetSummary(), m_pCanvas);
	}
	else
	{
		Show(false);
	}

	return true;
}


CECDiagramRender::CECDiagramRender()
:m_pVerts(NULL)
,m_pIndices(NULL)
{
	m_RatioType = -1;
	m_pSummary = NULL;
}

CECDiagramRender::~CECDiagramRender()
{
	delete[] m_pVerts;
	m_pVerts = NULL;

	delete[] m_pIndices;
	m_pIndices = NULL;

	ClearSummary();
}

bool CECDiagramRender::Init()
{
	m_pVerts = new A3DVECTOR3[DIAGRAM_VERTEX]; 
	m_pIndices = new WORD[DIAGRAM_INDEX];
	if(!m_pVerts || !m_pIndices)
		return false;

	for(DWORD i = 0; i < DIAGRAM_PRECISION; i++ )
	{
		m_pIndices[i * 3] = WORD(i + 1);
		m_pIndices[i * 3 + 1] = WORD(i + 2);
		m_pIndices[i * 3 + 2] = 0;
	}

	return true;
}

void CECDiagramRender::Render(const FORCE_GLOBAL_SUMMARY* pSummary, PAUIIMAGEPICTURE pCanvas)
{
	Render(pSummary->count, pSummary->pRatio, pSummary->ppConfig, pCanvas);
}

void CECDiagramRender::Render(int count, const int* pRatio, const FORCE_CONFIG** ppConfig, PAUIIMAGEPICTURE pCanvas)
{
	if(!count || !pRatio || !ppConfig || !pCanvas || !m_pVerts || !m_pIndices)
		return;

	int i;

	// normalize the ratio
	float sum = 0.f;
	for(i=0;i<count;i++) sum += pRatio[i];

	// create float buffer
	float* pNormalized = (float*)a_malloctemp(sizeof(float) * count);
	if(!pNormalized) return;

	// support all zero data
	if(sum < 0.0001f)
	{
		for(i=0;i<count;i++) pNormalized[i] = 1.f / count;
	}
	else
	{
		for(i=0;i<count;i++) pNormalized[i] = pRatio[i] / sum;
	}

	// calculate the center point of canvas
	A3DPOINT2 center;
	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	POINT pos = pCanvas->GetPos();
	SIZE size = pCanvas->GetSize();
	center.Set(p->X + pos.x + size.cx / 2, p->Y + pos.y + size.cy / 2);

	// the center stored in the first vertex
	m_pVerts[0].Set(float(center.x), float(center.y), 0.0f);

	// the scale is used to fit the UI
	float r = (float)(size.cx / 2);

	// calculate the vertices
	float delta = A3D_2PI / DIAGRAM_PRECISION;
	for(i = 0; i <= DIAGRAM_PRECISION ; i++ )
	{
		m_pVerts[i+1].x = center.x + r * cos(i * delta - A3D_PI / 2);
		m_pVerts[i+1].y = center.y + r * sin(i * delta - A3D_PI / 2);
		m_pVerts[i+1].z = 0.0f;
	}

	A3DCULLTYPE oldtype = g_pGame->GetA3DDevice()->GetFaceCull();
	g_pGame->GetA3DDevice()->SetFaceCull(A3DCULL_NONE);
	A3DFlatCollector *pFC = g_pGame->GetA3DEngine()->GetA3DFlatCollector();

	// draw the diagram, clockwise
	WORD* pIndex = m_pIndices;
	int indexLeft = DIAGRAM_INDEX;
	for(i = 0; i< count-1 && indexLeft > 0; i++)
	{
		int indexCount = (int)(pNormalized[i] * DIAGRAM_PRECISION + 0.5f) * 3;
		pFC->AddRenderData_2D(m_pVerts, DIAGRAM_VERTEX, pIndex, indexCount, ppConfig[i]->color);
		indexLeft -= indexCount;
		pIndex += indexCount;
	}
	// make sure all indices are used
	if(indexLeft > 0)
	{
		pFC->AddRenderData_2D(m_pVerts, DIAGRAM_VERTEX, pIndex, indexLeft, ppConfig[i]->color);
	}

	pFC->Flush_2D();
	g_pGame->GetA3DDevice()->SetFaceCull(oldtype);

	a_freetemp(pNormalized);
}

void CDlgForceActivity::OnEvent_Canvas(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
#ifdef _DEBUG
	// refresh the summary data
	int curRatioType = m_pDiagramRender->GetRatioType() + 1;
	if(curRatioType >= CECForceMgr::FORCE_GLOBAL::GetMaxType())
	{
		curRatioType = 0;
	}

	m_pDiagramRender->RefreshSummary(curRatioType, m_pCanvas);
#endif
}


void CECDiagramRender::ClearSummary()
{
	// clear old data
	m_RatioType = -1;
	delete m_pSummary;
	m_pSummary = NULL;
}

void CECDiagramRender::RefreshSummary(int type, PAUIIMAGEPICTURE pCanvas)
{
	CECForceMgr* pMgr = g_pGame->GetGameRun()->GetHostPlayer()->GetForceMgr();
	if(!pMgr) return;

	if(type != m_RatioType)
	{
		m_RatioType = type;

		delete m_pSummary;
		m_pSummary = pMgr->GetForceGlobalSummary(m_RatioType);
	}

	// prepare the summary string
	if(m_pSummary && pCanvas)
	{
		ACString strSummary;

		// add summary text
		for(int i=0;i<m_pSummary->count;i++)
		{
			ACString strMsg;

			const FORCE_CONFIG* pConfig = m_pSummary->ppConfig[i];

			ACString strTmp = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetStringFromTable(9412 + type);

			strMsg.Format(strTmp,  A3DCOLOR_TO_STRING(pConfig->color), pConfig->name, m_pSummary->pRatio[i]);

			if(!strSummary.IsEmpty()) strSummary += _AL("\\r");
			strSummary += strMsg;
		}

		AUICTranslate trans;
		pCanvas->SetHint(trans.Translate(strSummary));
	}
}