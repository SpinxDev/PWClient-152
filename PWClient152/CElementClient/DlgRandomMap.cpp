// Filename	: DlgRandomMap.cpp
// Creator	: WYD
// Date		: 20142/8/19

#include "DlgRandomMap.h"
#include "EC_GameUIMan.h"
#include "EC_RandomMapPreProcessor.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "DlgWorldMap.h"
#include "EC_UIManager.h"
#include "EC_HostPlayer.h"
#include "EC_Gamesession.h"
#include "AUIDEF.h"
#include "EC_UIHelper.h"
#include <A3DFlatCollector.h>
#include <A2DSprite.h>

AUI_BEGIN_COMMAND_MAP(CDlgRandomMap, CDlgBase)


AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgRandomMap, CDlgBase)
AUI_ON_EVENT("*",		WM_LBUTTONUP,	OnEventLButtonUp)
AUI_ON_EVENT("*",		WM_RBUTTONUP,	OnEventRButtonUp)
AUI_END_EVENT_MAP()


// convert team member pos to screen pixel
struct PosConvertorRandomWar : public CECMapDlgsMgr::PosConvertor
{
	PosConvertorRandomWar(int nInstanceID)
	{
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		PAUIDIALOG pDlg = pGameUI->GetDialog("Win_RandomMap");
		
		// get the background center
		A3DRECT rcItem = pDlg->GetDlgItem("Img_Map")->GetRect();
		A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
		absCenter = rcItem.CenterPoint() + A3DPOINT2(p->X, p->Y);
		
		this->nInstanceID = nInstanceID;
	}
	
	virtual A3DPOINT2 operator()(const A3DVECTOR3& v) const
	{		
		CECMapDlgsMgr* pMgr = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetMapDlgsMgr();
		A3DPOINT2 ret = absCenter + pMgr->GetPixelsByPos(v.x, v.z, nInstanceID);
		
		return ret;
	}
	
	virtual A3DVECTOR3 operator()(const A3DPOINT2& ret) const
	{		
		CECMapDlgsMgr* pMgr = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetMapDlgsMgr();
		APointF	v = pMgr->GetPosByPixels(ret.x - absCenter.x, ret.y - absCenter.y, nInstanceID);
		
		return A3DVECTOR3(v.x, 0.f, v.y);
	}
	
private:
	int nInstanceID;
	A3DPOINT2 absCenter;
};

//////////////////////////////////////////////////////////////////////////

CDlgRandomMap::CDlgRandomMap()
{

}

CDlgRandomMap::~CDlgRandomMap()
{
	A3DRELEASE(m_pMapSprite);
}

bool CDlgRandomMap::OnInitDialog()
{
	m_pImg_Map = NULL;
	m_pMapSprite = NULL;

//	DDX_Control("Img_Map",m_pImg_Map);
	m_pImg_Map = (PAUIIMAGEPICTURE)GetDlgItem("Img_Map");
	if(m_pImg_Map)		
		m_pImg_Map->SetRenderTop(true);

	return true;
}
bool CDlgRandomMap::GetRandomMapTex(const AString& strFile,unsigned char*& pData,int& iLen)
{
	CECRandomMapProcess* proc = g_pGame->GetGameRun()->GetRandomMapProc();
	if(proc)
		return proc->EncodeMap(strFile,pData,iLen);

	return false;
}
bool CDlgRandomMap::Render()
{
	bool bval = CDlgBase::Render();
	int nInstanceID = GetWorld()->GetInstanceID();
	PosConvertorRandomWar conv(nInstanceID);

	UpdateRandomMap(nInstanceID);

	GetGameUIMan()->GetMapDlgsMgr()->DrawTeamMark(conv);
	GetGameUIMan()->GetMapDlgsMgr()->DrawHostMark(conv);
	

	return bval;
}
bool CDlgRandomMap::UpdateRandomMap(int iInst)
{
	if (GetWorld()->IsRandomMap(iInst)){

		// 有开启全部地图道具
		CECRandomMapProcess* pRandomMap = GetGameRun()->GetRandomMapProc();
		if(!pRandomMap || pRandomMap->HaveClearMapFogItem())
			return true;

		PosConvertorRandomWar conv(iInst);

		PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)GetDlgItem("Img_Map");
		int w = pImage->GetSize().cx;
		int h = pImage->GetSize().cy;
		A3DFlatCollector *pFC = m_pA3DEngine->GetA3DFlatCollector();
		A3DVECTOR3 hostPos = GetHostPlayer()->GetPos();
		A3DPOINT2 hostPosInMap = conv(hostPos);
		
		A3DRECT rcItem = pImage->GetRect();
		A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
		int x = rcItem.CenterPoint().x + p->X;
		int y = rcItem.CenterPoint().y + p->Y;
						
		SIZE s = pImage->GetSize();
		POINT pos = pImage->GetPos(true); // 左上角

		int realW = s.cx;
		int realH = s.cy ; // 地图 （比如 world.dds） 实际高度 768 / 1024.
		int right_bottom_x = pos.x + s.cx;
		int right_bottom_y = pos.y + s.cy;

		int left_top_x = pos.x;
		int left_top_y = right_bottom_y - realH;
		
		int realShowH = min(realW,realH);
		int curCell = 0;

		// 实际显示区域,
		int realTop = y - realH / 2 - 1;
		int realBottom = y + realH / 2 + 1;
		int realLeft = x - realW / 2 - 1;
		int realRight = x + realW  / 2 + 1;

		realH += 2;
		realW += 2;
		
		const static int step = 1;

		A3DCOLOR color = A3DCOLORRGBA(4,4,4,255);

		// 遍历M图
		for (int j=realTop;j<realBottom;j+=step){	
			int r = (j - realTop) * CECRandomMapFogMask::FOGMASK_RESOLUTION / realH;
			for (int i=realLeft;i<realRight;i+=step){
				int c = (i-realLeft) * CECRandomMapFogMask::FOGMASK_RESOLUTION / realW;
				int curCell = r * CECRandomMapFogMask::FOGMASK_RESOLUTION + c;
				
				if(pRandomMap && !pRandomMap->GetRandomMapFogMask().GetTileVisbile(curCell))
				{					
					A3DRECT rc;
					rc.left = i;
					rc.right = i+step;
					rc.top = j;
					rc.bottom = j+step;
					pFC->AddRect_2D(rc.left, rc.top, rc.right, rc.bottom, color);
				}
			}
		}

		// 处理黑边
/*		if (realLeft > 0){
			pFC->AddRect_2D(0, realTop, realLeft, realBottom, color);
			pFC->AddRect_2D(realRight, realTop, GetSize().cx, realBottom, color);
		}
		if (realRight > 0){
			pFC->AddRect_2D(realLeft, 0, realRight, realTop,color);
			pFC->AddRect_2D(realLeft, realBottom, realRight, GetSize().cy, color);
		}*/
	}
	
	return true;
}
void CDlgRandomMap::OnShowDialog()
{
	// 随机地图
	if (GetWorld()->IsRandomMap()){

		
		A3DRELEASE(m_pMapSprite);
		m_pImg_Map->ClearCover();

		int iLen = 0;
		AString strFile = "Temp\\surfaces\\ingame\\" + GetGameRun()->GetRandomMapProc()->GetMapName();
		strFile += ".dds";
		
		unsigned char* pImageData = NULL;
		m_pMapSprite = new A2DSprite;
		m_pMapSprite->SetLinearFilter(true);
		GetRandomMapTex(strFile,pImageData,iLen);
		if (!pImageData || !m_pMapSprite->InitFromFileInMemory(m_pA3DDevice,strFile,pImageData,iLen,0)){
			
			if (!m_pMapSprite->Init(m_pA3DDevice, "InGame\\map_bg.dds", 0)){
				A3DRELEASE(m_pMapSprite);
				AUI_ReportError(__LINE__, __FILE__);
				return ;
			}
		}
		if(pImageData)
			delete [] pImageData;
		
		m_pImg_Map->SetCover(m_pMapSprite,0);

		PAUIIMAGEPICTURE pBg = (PAUIIMAGEPICTURE)GetDlgItem("Edi_BG");
		if(pBg && pBg->GetImage())
			pBg->GetImage()->SetLinearFilter(true);
	}	
}
void CDlgRandomMap::OnHideDialog()
{

}
void CDlgRandomMap::OnResize()
{
	POINT p = GetDlgItem("Edi_BG")->GetPos();
	SIZE bgSize = GetDlgItem("Edi_BG")->GetSize();

	SIZE s = m_pImg_Map->GetSize();
	m_pImg_Map->SetPos(p.x + bgSize.cx * 0.5 - s.cx * 0.5, p.y + bgSize.cy * 0.5 - s.cy * 0.5);
}
void CDlgRandomMap::OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int nInstanceID = GetWorld()->GetInstanceID();
	if(!GetGameUIMan()->GetMapDlgsMgr()->IsMapCanShowHostPos(nInstanceID))
		return;
	
	A3DRECT rcItem = pObj->GetRect();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - rcItem.CenterPoint().x - p->X;
	int y = GET_Y_LPARAM(lParam) - rcItem.CenterPoint().y - p->Y;

	APointF ptClick = GetGameUIMan()->GetMapDlgsMgr()->GetPosByPixels(x, y, nInstanceID);
	float fX = ptClick.x;
	float fZ = ptClick.y;

	if( wParam & MK_CONTROL )
	{
		OnCommand_CANCEL("IDCANCEL");
		g_pGame->GetGameSession()->c2s_CmdGoto(fX, 1.0f, fZ);
	}
	else if (AUI_PRESS(VK_LMENU) || AUI_PRESS(VK_RMENU) || AUI_PRESS(VK_MENU))
	{
		CECUIHelper::AutoMoveStartComplex(A3DVECTOR3(fX, 0.0f, fZ));
	}
}
void CDlgRandomMap::OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	Show(false);
}

