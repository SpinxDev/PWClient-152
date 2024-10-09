#include "A3DGDI.h"
#include "A3DDevice.h"
#include "A3DCamera.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"
#include "A3DTexture.h"
#include "DlgTask.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_Resource.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EC_Viewport.h"
#include "EC_Utility.h"
#include "EC_Configs.h"
#include "EC_Global.h"
#include "EC_GameRun.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "A3DFlatCollector.h"
#include "DlgTreasureMap.h"
#include "A3DSurface.h"
#include "A3DSurfaceMan.h"
#define new A_DEBUG_NEW

static const float WORLD_MAP_HALF_WIDTH_METER = 4096.f;
static const float WORLD_MAP_HALF_HEIGHT_METER = 5632.f;
static const int RESOURCE_MAP_SIZE = 1024;
static const int WORLD_MAP_X_NUM = 8;
static const int WORLD_MAP_Z_NUM = 11;
//------------------------------------------------------------------------
AUI_BEGIN_COMMAND_MAP(CDlgTreasureMap, CDlgBase)


AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgTreasureMap, CDlgBase)
AUI_ON_EVENT("Img_01*",			WM_RBUTTONUP,	OnEventRButtonUp)

AUI_END_EVENT_MAP()

CDlgTreasureMap::CDlgTreasureMap():m_iMapLowerLeftX(0),m_iMapLowerLeftZ(0),m_iTreasureLowerLeftX(0),m_iTreasureLowerLeftZ(0),m_iTreasureSide(0),m_pMapTexture(NULL)
{
}

CDlgTreasureMap::~CDlgTreasureMap()
{
	ClearTreasureMap();
}

void CDlgTreasureMap::OnTick()
{
	CDlgBase::OnTick();
}

bool CDlgTreasureMap::Render(void)
{
	bool bval = CDlgBase::Render();
	return bval;
}

void CopyPartTexture2Buffer(int iTexName1,int iTexName2,RECT rectTexPart,RECT rectBufferPart,CRenderBuffer* buffer)
{
	AString strIndex;
	strIndex.Format("Surfaces\\Maps\\%02d%d.dds", iTexName1, iTexName2);

	A3DSurface* pDestSurface = new A3DSurface;
	A3DSurface* pSourceSurface = new A3DSurface;
	
#ifdef ANGELICA_2_2
	typedef IDirect3DSurface9 ID3DSURFACE;
#else
	typedef IDirect3DSurface8 ID3DSURFACE;
#endif
	ID3DSURFACE* destsurface = pDestSurface->GetDXSurface();
	ID3DSURFACE* sourcesurface = pSourceSurface->GetDXSurface();

	bool bval = false;
	A3DTexture* pMap = new A3DTexture;
	A3DTexture* pTexture = new A3DTexture;

	while (true)
	{
		pMap->SetNoDownSample(true);
		bval = pMap->LoadFromFile(g_pGame->GetA3DDevice(), strIndex, strIndex);
		if (!bval)
			break;

		pMap->GetD3DTexture()->GetSurfaceLevel(0,&sourcesurface);

		pMap->SetNoDownSample(true);
		bval = pTexture->Create(g_pGame->GetA3DDevice(),RESOURCE_MAP_SIZE,RESOURCE_MAP_SIZE,A3DFMT_A8R8G8B8);
		if (!bval)
			break;

		pTexture->GetD3DTexture()->GetSurfaceLevel(0,&destsurface);

		RECT rect;
		rect.left = 0;
		rect.right = RESOURCE_MAP_SIZE;
		rect.top = 0;
		rect.bottom = RESOURCE_MAP_SIZE;

		HRESULT hr = D3DXLoadSurfaceFromSurface(destsurface,NULL,NULL,sourcesurface,NULL,&rect,D3DX_DEFAULT,0);
		if (hr != D3D_OK)
			break;

		unsigned char* pBits;
		int iPitch;
		pTexture->LockRect(&rectTexPart,(void**)&pBits,&iPitch,0);

		buffer->CopyArea(rectBufferPart,pBits,iPitch);
		pTexture->UnlockRect();

		break;
	}
	A3DRELEASE(pMap);
	A3DRELEASE(pTexture);
	A3DRELEASE(pDestSurface);
	A3DRELEASE(pSourceSurface);
}

bool CDlgTreasureMap::ShowTreasureMap(int iMapLowerLeftX,int iMapLowerLeftZ,int iTreasureLowerLeftX,int iTreasureLowerLeftZ,int iTreasureSide,int nHalfSizeMap)
{
	int iHalfTreasureMapSide = nHalfSizeMap * iTreasureSide;
	int iTreasureMapSide = 2 * iHalfTreasureMapSide;

	// 两次打开同样的藏宝图就不用计算两次了
	if (m_iMapLowerLeftX == iMapLowerLeftX 
		&& m_iMapLowerLeftZ == iMapLowerLeftZ 
		&& m_iTreasureLowerLeftX == iTreasureLowerLeftX 
		&& m_iTreasureLowerLeftZ == iTreasureLowerLeftZ 
		&& m_iTreasureSide == iTreasureSide)
	{
		Show(true);
		return true;
	}
	else
	{
		m_iMapLowerLeftX = iMapLowerLeftX;
		m_iMapLowerLeftZ = iMapLowerLeftZ;
		m_iTreasureLowerLeftX = iTreasureLowerLeftX;
		m_iTreasureLowerLeftZ = iTreasureLowerLeftZ;
		m_iTreasureSide = iTreasureSide;
	}
	if (iTreasureMapSide >= RESOURCE_MAP_SIZE)
	{
		//藏宝图不能大于资源位图的大小
		return false;
	}
	
	ClearTreasureMap();

	float fX = float(iMapLowerLeftX + iHalfTreasureMapSide);
	float fZ = float(iMapLowerLeftZ + iHalfTreasureMapSide);
	POINT ptTarget;
	ptTarget.x = int((fX + WORLD_MAP_HALF_WIDTH_METER) / RESOURCE_MAP_SIZE);
	if( ptTarget.x < 0 || ptTarget.x > WORLD_MAP_X_NUM - 1 ) return false;

	ptTarget.y = int((WORLD_MAP_HALF_HEIGHT_METER - fZ) / RESOURCE_MAP_SIZE);
	if( ptTarget.y < 0 || ptTarget.y > WORLD_MAP_Z_NUM - 1 ) return false;

	int nDeltaX = int(fX + WORLD_MAP_HALF_WIDTH_METER - ptTarget.x * RESOURCE_MAP_SIZE);
	int nDeltaY = int(WORLD_MAP_HALF_HEIGHT_METER - fZ - ptTarget.y * RESOURCE_MAP_SIZE);

	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	PAUIDIALOG pDlgDetail = pGameUI->GetDialog("Win_TreasureMap");
	SIZE s = pDlgDetail->GetSize();

	m_pRenderBuffer = new CRenderBuffer(iTreasureMapSide,iTreasureMapSide);


	int nLeftDistance = nDeltaX - iHalfTreasureMapSide;
	int nRightDistance = RESOURCE_MAP_SIZE - (nDeltaX + iHalfTreasureMapSide);
	int nUpperDistance = nDeltaY - iHalfTreasureMapSide;
	int nLowerDistance = RESOURCE_MAP_SIZE - (nDeltaY + iHalfTreasureMapSide);
	RECT rect,srcRect;
	rect.left = nLeftDistance	 < 0 ? 0 : nLeftDistance;
	rect.right = nRightDistance  < 0 ? RESOURCE_MAP_SIZE : RESOURCE_MAP_SIZE - nRightDistance;
	rect.top = nUpperDistance	 < 0 ? 0 : nUpperDistance;
	rect.bottom = nLowerDistance < 0 ? RESOURCE_MAP_SIZE : RESOURCE_MAP_SIZE - nLowerDistance;

	srcRect.left = nLeftDistance   < 0 ? -nLeftDistance : 0;
	srcRect.right = nRightDistance < 0 ? iTreasureMapSide + nRightDistance : iTreasureMapSide;
	srcRect.top = nUpperDistance   < 0 ? -nUpperDistance : 0;
	srcRect.bottom= nLowerDistance < 0 ? iTreasureMapSide + nLowerDistance : iTreasureMapSide;

	CopyPartTexture2Buffer(ptTarget.y,ptTarget.x,rect,srcRect,m_pRenderBuffer);
	if (nLeftDistance < 0)
	{
		rect.left = RESOURCE_MAP_SIZE + nLeftDistance;
		rect.right = RESOURCE_MAP_SIZE;
		rect.top = nUpperDistance	 < 0 ? 0 : nUpperDistance;
		rect.bottom = nLowerDistance < 0 ? RESOURCE_MAP_SIZE : RESOURCE_MAP_SIZE - nLowerDistance;

		srcRect.left = 0;
		srcRect.right = -nLeftDistance;
		srcRect.top = nUpperDistance   < 0 ? -nUpperDistance : 0;
		srcRect.bottom= nLowerDistance < 0 ? iTreasureMapSide + nLowerDistance : iTreasureMapSide;

		CopyPartTexture2Buffer(ptTarget.y,ptTarget.x - 1,rect,srcRect,m_pRenderBuffer);
	}
	if (nRightDistance < 0)
	{
		rect.left = 0;
		rect.right = -nRightDistance;
		rect.top = nUpperDistance	 < 0 ? 0 : nUpperDistance;
		rect.bottom = nLowerDistance < 0 ? RESOURCE_MAP_SIZE : RESOURCE_MAP_SIZE - nLowerDistance;

		srcRect.left = iTreasureMapSide + nRightDistance;
		srcRect.right = iTreasureMapSide;
		srcRect.top = nUpperDistance   < 0 ? -nUpperDistance : 0;
		srcRect.bottom= nLowerDistance < 0 ? iTreasureMapSide + nLowerDistance : iTreasureMapSide;
		
		CopyPartTexture2Buffer(ptTarget.y,ptTarget.x + 1,rect,srcRect,m_pRenderBuffer);
	}
	if (nUpperDistance < 0)
	{
		rect.left = nLeftDistance	 < 0 ? 0 : nLeftDistance;
		rect.right = nRightDistance  < 0 ? RESOURCE_MAP_SIZE : RESOURCE_MAP_SIZE - nRightDistance;
		rect.top = RESOURCE_MAP_SIZE + nUpperDistance;
		rect.bottom = RESOURCE_MAP_SIZE;

		srcRect.left = nLeftDistance   < 0 ? -nLeftDistance : 0;
		srcRect.right = nRightDistance < 0 ? iTreasureMapSide + nRightDistance : iTreasureMapSide;
		srcRect.top = 0;
		srcRect.bottom= -nUpperDistance;

		CopyPartTexture2Buffer(ptTarget.y - 1,ptTarget.x,rect,srcRect,m_pRenderBuffer);
	}
	if (nLowerDistance < 0)
	{
		rect.left = nLeftDistance	 < 0 ? 0 : nLeftDistance;
		rect.right = nRightDistance  < 0 ? RESOURCE_MAP_SIZE : RESOURCE_MAP_SIZE - nRightDistance;
		rect.top = 0;
		rect.bottom = -nLowerDistance;

		srcRect.left = nLeftDistance   < 0 ? -nLeftDistance : 0;
		srcRect.right = nRightDistance < 0 ? iTreasureMapSide + nRightDistance : iTreasureMapSide;
		srcRect.top = iTreasureMapSide + nLowerDistance;
		srcRect.bottom= iTreasureMapSide;
		
		CopyPartTexture2Buffer(ptTarget.y + 1,ptTarget.x,rect,srcRect,m_pRenderBuffer);
	}
	if (nLeftDistance < 0 && nUpperDistance < 0)
	{
		rect.left = RESOURCE_MAP_SIZE + nLeftDistance;
		rect.right = RESOURCE_MAP_SIZE;
		rect.top = RESOURCE_MAP_SIZE + nUpperDistance;
		rect.bottom = RESOURCE_MAP_SIZE;

		srcRect.left = 0;
		srcRect.right = -nLeftDistance;
		srcRect.top = 0;
		srcRect.bottom= -nUpperDistance;

		CopyPartTexture2Buffer(ptTarget.y - 1,ptTarget.x - 1,rect,srcRect,m_pRenderBuffer);
	}
	if (nLeftDistance < 0 && nLowerDistance < 0)
	{
		rect.left = RESOURCE_MAP_SIZE + nLeftDistance;
		rect.right = RESOURCE_MAP_SIZE;
		rect.top = 0;
		rect.bottom = -nLowerDistance;

		srcRect.left = 0;
		srcRect.right = -nLeftDistance;
		srcRect.top = iTreasureMapSide + nLowerDistance;
		srcRect.bottom= iTreasureMapSide;

		CopyPartTexture2Buffer(ptTarget.y + 1,ptTarget.x - 1,rect,srcRect,m_pRenderBuffer);
	}
	if (nRightDistance < 0 && nUpperDistance < 0)
	{
		rect.left = 0;
		rect.right = -nRightDistance;
		rect.top = RESOURCE_MAP_SIZE + nUpperDistance;
		rect.bottom = RESOURCE_MAP_SIZE;

		srcRect.left = iTreasureMapSide + nRightDistance;
		srcRect.right = iTreasureMapSide;
		srcRect.top = 0;
		srcRect.bottom= -nUpperDistance;

		CopyPartTexture2Buffer(ptTarget.y - 1,ptTarget.x + 1,rect,srcRect,m_pRenderBuffer);
	}
	if (nRightDistance < 0 && nLowerDistance < 0)
	{
		rect.left = 0;
		rect.right = -nRightDistance;
		rect.top = 0;
		rect.bottom = -nLowerDistance;

		srcRect.left = iTreasureMapSide + nRightDistance;
		srcRect.right = iTreasureMapSide;
		srcRect.top = iTreasureMapSide + nLowerDistance;
		srcRect.bottom= iTreasureMapSide;

		CopyPartTexture2Buffer(ptTarget.y + 1,ptTarget.x + 1,rect,srcRect,m_pRenderBuffer);
	}
	
	while (true)
	{
		bool bval = false;
		A3DTexture* pMap = new A3DTexture;

		pMap->SetNoDownSample(true);
		bval = pMap->Create(g_pGame->GetA3DDevice(),iTreasureMapSide,iTreasureMapSide,A3DFMT_A8R8G8B8);
		if (!bval)
			break;

		unsigned char* pBits;
		int iPitch;
		pMap->LockRect(NULL,(void**)&pBits,&iPitch,0);

		m_pRenderBuffer->ToGray();
		m_pRenderBuffer->BlitBGR(pBits,iPitch);
		pMap->UnlockRect();

		PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("TreasureMap");
		int iStartPosX(0),iStartPosY(0);
		if (pImage)
		{
			pImage->SetTexture(pMap);
			s = pImage->GetSize();
			iStartPosX = pImage->GetPos(true).x;
			iStartPosY = pImage->GetPos(true).y;
		}

		// 保存上一次打开的texture以供销毁
		m_pMapTexture = pMap;

		Show(true);

		pImage = (PAUIIMAGEPICTURE)pDlgDetail->GetDlgItem("Flag");
		if (pImage)
		{
			float fScaleX = (float)s.cx / iTreasureMapSide;
			float fScaleZ = (float)s.cy / iTreasureMapSide;
			float fPosX = (iTreasureLowerLeftX - iMapLowerLeftX) * fScaleX;
			FLOAT fPosY = s.cy - (iTreasureLowerLeftZ - iMapLowerLeftZ) * fScaleZ;
			pImage->SetPos((int)fPosX + iStartPosX - pImage->GetSize().cx / 2,(int)fPosY + iStartPosY - pImage->GetSize().cy / 2);
		}

		break;
	}

	delete m_pRenderBuffer;
	m_pRenderBuffer = NULL;
	return true;
}

void CDlgTreasureMap::OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	Show(false);
}

void CDlgTreasureMap::ClearTreasureMap()
{
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)GetDlgItem("TreasureMap");
	if (pImage)
	{
		pImage->SetTexture(NULL);
	}
	A3DRELEASE(m_pMapTexture);

}
