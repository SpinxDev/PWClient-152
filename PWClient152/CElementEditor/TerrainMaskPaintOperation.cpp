#include "global.h"
#include "MainFrm.h"
#include "Terrain.h"
#include "TerrainRender.h"
#include "Render.h"
#include "TerrainMaskPaintOperation.h"
#include "Bitmap.h"
#include "MaskModifier.h"
#include "UndoMan.h"
#include "a3d.h"


//#define new A_DEBUG_NEW

bool CTerrainMaskPaintOperation::OnEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	return true;
}

void CTerrainMaskPaintOperation::Render(A3DViewport* pA3DViewport)
{
	if(m_pBrush)
		m_pBrush->Render(pA3DViewport);

}

void CTerrainMaskPaintOperation::Tick(DWORD dwTimeDelta)
{
	if(m_pBrush)
		m_pBrush->Tick(dwTimeDelta);
//	UpdateTerrainHeight();
}


bool CTerrainMaskPaintOperation::OnMouseMove(int x, int y, DWORD dwFlags) 
{ 
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrainRender *pTR = pMap->GetTerrain()->GetRender();
		if(pTR)
		{
			A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
			A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
			A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
			A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
			pViewport->InvTransform(vStart,vStart);
			pViewport->InvTransform(vPos,vPos);
			
			RAYTRACERT TraceRt;	//	Used to store trace result
			if(pTR->RayTrace(vStart,vPos - vStart,1.0f,&TraceRt))
				m_pBrush->SetCenter(TraceRt.vHitPos.x,TraceRt.vHitPos.z);
			if(m_bLMouseDown)
			{
				UpdateMaskMap();
				return false;
			}
		}
	}
	return true;
}

bool CTerrainMaskPaintOperation::OnLButtonDown(int x, int y, DWORD dwFlags) 
{ 
	
	m_bLMouseDown = true;
	m_ptOldPos.x = x;
	m_ptOldPos.y = y;

	//mask Undo
	StartPaint(true);
	//m_rcUpdateRect.Clear();
	//m_pUndoAction = new CUndoMaskAction();
	
	UpdateMaskMap();
	return false; 
}

bool CTerrainMaskPaintOperation::OnRButtonDown(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

bool CTerrainMaskPaintOperation::OnLButtonUp(int x, int y, DWORD dwFlags) 
{ 
	m_bLMouseDown = false;
	StartPaint(false);

	if(m_pUndoAction)
	{
		m_pUndoAction->SetUpdateRect(m_rcUpdateRect,m_nCurrentLayer);
		g_UndoMan.AddUndoAction(m_pUndoAction);
		m_pUndoAction = NULL;
	}	
	return false; 
}

bool CTerrainMaskPaintOperation::OnRButtonUp(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

bool CTerrainMaskPaintOperation::OnLButtonDbClk(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

bool CTerrainMaskPaintOperation::OnRButtonDbClk(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

void CTerrainMaskPaintOperation::SetCurrentLayer( int nLay)
{
	if(nLay<=0)
	{ 
		m_nCurrentLayer = -1;
		return;
	}
	m_nCurrentLayer = nLay;

	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	CTerrain *pTerrain = pMap->GetTerrain();
	if(pTerrain == NULL) return;
	CMaskModifier *pMaskModifier = pTerrain->GetMaskModifier();
	if(pMaskModifier == NULL) return;

// 	if(pMaskModifier->HasModify())
// 		pMap->RecreateTerrainRender(false,false);
	
	if(m_pUndoAction)
	{
		delete m_pUndoAction;
		m_pUndoAction = NULL;
	}


	if(!pMaskModifier->ChangeEditLayer(m_nCurrentLayer)) AfxMessageBox("CTerrainMaskPaintOperation::SetCurrentLayer(),Changed layer failed, please close editor and save map!");
	
}

void CTerrainMaskPaintOperation::UpdateMaskMap()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL || m_nCurrentLayer== -1) return;
	CTerrain *pTerrain = pMap->GetTerrain();
	if(pTerrain == NULL) return;
	
	
	if(m_nCurrentLayer>pTerrain->GetLayerNum()-1 || m_nCurrentLayer<0)
		return;

	//update mask data
	CTerrainLayer *pLayer = pTerrain->GetLayer(m_nCurrentLayer);
	CMaskModifier *pMaskModifier = pTerrain->GetMaskModifier();
	if(pLayer)
	{
		CELBitmap *pMaskMap = pLayer->GetMaskBitmap();
		if(pMaskMap)
		{
			m_nMaskHeight = pMaskMap->GetHeight();
			m_nMaskWidth  = pMaskMap->GetWidth();
		}else
		{
			m_nMaskHeight = DEFMASK_SIZE;
			m_nMaskWidth = DEFMASK_SIZE;
			pLayer->CreateMaskMap(DEFMASK_SIZE);
		}

		if(m_pUndoAction == NULL)
		{
			m_rcUpdateRect.Clear();
			m_pUndoAction = new CUndoMaskAction(m_nMaskWidth);
		}

		CalculateMask();
	}
}

void CTerrainMaskPaintOperation::StartPaint(bool bStart)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	CTerrain *pTerrain = pMap->GetTerrain();
	if(pTerrain == NULL) return;
	CMaskModifier *pMaskModifier = pTerrain->GetMaskModifier();
	if(pMaskModifier == NULL) return;

	if(bStart)
		pMaskModifier->BeginModify();
	else pMaskModifier->EndModify();

	if(pMaskModifier->HasModify())
		pMap->SetModifiedFlag(true);
	
}

void CTerrainMaskPaintOperation::CalculateMask()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	
	float fCenterX,fCenterZ;
	m_pBrush->GetCenter(fCenterX,fCenterZ);
	
	CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
	CTerrain *pTerrain =  pMap->GetTerrain();

	if(!pHeightMap || !pTerrain) return;
	
	ARectF m_rcTerrain = pTerrain->GetTerrainArea();
	float m_fTileSize = pTerrain->GetTileSize();
	float m_fRadius = g_Configs.iBrushRadius;

	ARect<float> updateRect;
	updateRect.left = fCenterX - m_fRadius;
	updateRect.right = fCenterX + m_fRadius;
	updateRect.top = fCenterZ + m_fRadius;
	updateRect.bottom = fCenterZ - m_fRadius;

	float fMaskScaleX = (float)m_nMaskWidth/((m_rcTerrain.right - m_rcTerrain.left));
	float fMaskScaleZ = (float)m_nMaskHeight/((m_rcTerrain.top - m_rcTerrain.bottom));

	float fMaskGrid = (pHeightMap->iWidth - 1.0f) / m_nMaskWidth;
	float fMaskLen = fMaskGrid * m_fTileSize;
	
	ARectI maskRect;
	maskRect.left = (int)((updateRect.left - m_rcTerrain.left) / fMaskLen);
	maskRect.top = (int)(-(updateRect.top - m_rcTerrain.top) / fMaskLen);
	maskRect.right = (int)((updateRect.right - m_rcTerrain.left) / fMaskLen) + 1;
	maskRect.bottom = (int)(-(updateRect.bottom - m_rcTerrain.top) / fMaskLen) + 1;

	//这儿保留Undo
	/*
	CTerrainLayer *pCurrentLay = pTerrain->GetLayer(m_nCurrentLayer);
	CELBitmap *pMaskBitmap = pCurrentLay->GetMaskBitmap();
	if(pMaskBitmap==NULL) return;
	CELBitmap::LOCKINFO lockInfor;
	pMaskBitmap->LockRect(maskRect,&lockInfor);
	for(i = 0; i< lockInfor.iHeight; i++)
	{
		for(n = 0; n<lockInfor.iWidth; n++)
		{
	
			if(m_pUndoAction)
			{
				int h = maskRect.top + i;
				int w = maskRect.left + n;
				m_pUndoAction->AddGrid(w,h,lockInfor.pData[i*lockInfor.iPitch + n]);
			}
		}
	}
	*/
	int nCenterX = (fCenterX - m_rcTerrain.left)*fMaskScaleX;
	int nCenterZ = -(m_rcTerrain.bottom + fCenterZ)*fMaskScaleZ;
	a_Clamp(nCenterX, 0, m_nMaskWidth);
	a_Clamp(nCenterZ, 0, m_nMaskWidth);

	a_Clamp(maskRect.left, 0, m_nMaskWidth-1);
	a_Clamp(maskRect.top, 0, m_nMaskWidth-1);
	a_Clamp(maskRect.right, 0, m_nMaskWidth);
	a_Clamp(maskRect.bottom, 0, m_nMaskWidth);
	maskRect.Normalize();
	nCenterX = nCenterX - maskRect.left;
	nCenterZ = nCenterZ - maskRect.top;
	m_rcUpdateRect |= maskRect;

	CTerrainLayer *pCurrentLay = pTerrain->GetLayer(m_nCurrentLayer);
	if(pCurrentLay == NULL) return;
	CELBitmap *pMaskBitmap = pCurrentLay->GetMaskBitmap();
	if(pMaskBitmap==NULL) return;
	CELBitmap::LOCKINFO lockInfor;
	pMaskBitmap->LockRect(maskRect,&lockInfor);
	//Undo process
	if(m_pUndoAction) m_pUndoAction->AddData(maskRect,lockInfor);


	int NumX = maskRect.Width();
	int NumZ = maskRect.Height();
	int i,n;
	//分配内存，等更新后释放
	int nSize = NumX*NumZ*sizeof(int); 
	int *pData = new int[NumX*NumZ];
	if(pData == NULL) return;
	memset(pData,0,nSize);

	if(!m_bUserBrush || m_pUserData==NULL)
	{
		float r = (NumX/2 > NumZ/2)? NumX/2:NumZ/2;
		float a = (float)NumX/100.0f * m_pBrush->GetBrushHard();
		float d;
		
		for( i=0; i < NumZ; i++)
		{
			for( n = 0; n < NumX; n++)
			{
				d = sqrt(float((i-nCenterZ)*(i-nCenterZ)+(n-nCenterX)*(n-nCenterX)));
				
				if(g_Configs.bCircleBrush)
				{
					if(r<d) continue;
					if(d < a)
					{
						if(m_bColorAdd)
							pData[i*NumX + n] = m_pBrush->GetBrushGray();
						else pData[i*NumX + n] = -m_pBrush->GetBrushGray();
					}
					else
					{
						if(m_bColorAdd)
							pData[i*NumX + n] = m_pBrush->GetBrushGray() * (1.0f-(d - a)/(r - a));
						else pData[i*NumX + n] = -m_pBrush->GetBrushGray() * (1.0f-(d - a)/(r - a));
					}
				}else
				{
					if(m_bColorAdd)
						pData[i*NumX + n] = m_pBrush->GetBrushGray();
					else pData[i*NumX + n] = -m_pBrush->GetBrushGray();
				}
			}
		}
	}else
	{//Paint by user brush
		TranslateData(m_pUserData,m_nUserBrushW,m_nUserBrushH,pData,NumX,NumZ);
	}
	
	CMaskModifier *pMaskModifier = pTerrain->GetMaskModifier();
	if(pMaskModifier)
		pMaskModifier->Modify(maskRect,pData);

	if(pData) delete []pData;
}

void CTerrainMaskPaintOperation::TranslateData(BYTE *pSrc,int sw,int sh,int*pDst,int dw,int dh)
{
	
	int width = dw;
	int height = dh;
	
	double widthScale = (double)sw/(double)width;
	double heightScale = (double)sh/(double)height;
	for( int x = 0; x < width; ++x)
	{
		for( int y = 0; y < height; ++y)
		{
			 double xx = x * widthScale; 
			 double yy = y * heightScale;
			
			 if(widthScale <= 0.2 || widthScale >5)  
			 {
				 int col = pSrc[int(yy)*sw + int(xx)];
				 col = col * (m_pBrush->GetBrushHard())/100.0f;
				 if(m_bColorAdd) pDst[y*width + x] = col; 
				 else pDst[y*width + x] = -col; 
			 }else
			 {
				 int a = int(xx);  
				 int b = int(yy);
				 if (a + 1 >= sw || b + 1 >= sh)
					 pDst[y*width + x] = pSrc[int(yy)*sw + int(xx)];
				 else
				 {
					 
					 double x11 = double(pSrc[b*sw + a]); 
					 double x12 = double(pSrc[(b+1)*sw + a]); 
					 double x21 = double(pSrc[b*sw + a + 1]);  
					 double x22 = double(pSrc[(b+1)*sw + a + 1]);       
					 int col = int( (b+1-yy) * ((xx-a)*x21 + (a+1-xx)*x11) + (yy-b) * ((xx-a)*x22 +(a+1-xx) * x12) );
					 col = col * (m_pBrush->GetBrushHard())/100.0f;
				     if(m_bColorAdd)
						pDst[y*width + x] = col; 
					 else pDst[y*width + x] = -col; 
				 }
			 }
		}
	}
}

//--------------------------------------------------------------------
//Name: RecreateTerrainRender()
//Desc: 重新计算地形渲染数据
//--------------------------------------------------------------------
void CTerrainMaskPaintOperation::RecreateTerrainRender()
{
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap != NULL)
	{
		CTerrain *pTerrain = pMap->GetTerrain();
		if(pTerrain != NULL)
		{
			CMaskModifier *pMaskModifier = pTerrain->GetMaskModifier();
			if(pMaskModifier != NULL)
				if(pMaskModifier->HasModify())
					pMap->RecreateTerrainRender(false,false);
		}
	}
}
