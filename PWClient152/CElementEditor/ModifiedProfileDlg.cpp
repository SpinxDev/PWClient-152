// ModifiedProfileDlg.cpp : implementation file
//

#include "Global.h"
#include "elementeditor.h"
#include "Terrain.h"
#include "Bitmap.h"
#include "AC.h"
#include "MainFrm.h"
#include "EditerBezier.h"
#include "TerrainObstruct.h"
#include "SceneObjectManager.h"
#include "Render.h"

#include "PathMap\TerrainObstructGenerator.h"
#include "ModifiedProfileDlg.h"



//#define  new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CModifiedProfileDlg dialog


CModifiedProfileDlg::CModifiedProfileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifiedProfileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifiedProfileDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pTerrain = NULL;
	m_pBitmap = NULL;
	m_pPreBitmap = NULL;
	m_pByteData = NULL;
	m_pProfileData = NULL;
	
	m_nDiffThresh = 10;	//0-255
	m_nThresh = 8;      //0-25
	m_nSizeThresh = 10;
	m_nDistThresh = 8;
	m_fFitThresh = 0.8f;

	m_bMedianFilter = false;
	m_bShowProfile = false;
}


void CModifiedProfileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifiedProfileDlg)
	DDX_Control(pDX, IDC_EDIT_PATH_MAP4, m_MapPath4);
	DDX_Control(pDX, IDC_EDIT_PATH_MAP3, m_MapPath3);
	DDX_Control(pDX, IDC_EDIT_PATH_MAP2, m_MapPath2);
	DDX_Control(pDX, IDC_EDIT_PATH_MAP1, m_MapPath1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifiedProfileDlg, CDialog)
	//{{AFX_MSG_MAP(CModifiedProfileDlg)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, OnButtonPreview)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_CHECK3, OnCheck3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifiedProfileDlg message handlers

BOOL CModifiedProfileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	AUX_FlatEditBox((CEdit*)GetDlgItem(IDC_EDIT_PATH_MAP1));
	AUX_FlatEditBox((CEdit*)GetDlgItem(IDC_EDIT_PATH_MAP2));
	AUX_FlatEditBox((CEdit*)GetDlgItem(IDC_EDIT_PATH_MAP3));
	AUX_FlatEditBox((CEdit*)GetDlgItem(IDC_EDIT_PATH_MAP4));
	AUX_FlatEditBox((CEdit*)GetDlgItem(IDC_EDIT_PATH_MAP5));

	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO1);
	if(pButton) pButton->SetCheck(!m_bShowProfile);
	pButton = (CButton*)GetDlgItem(IDC_RADIO2);
	if(pButton) pButton->SetCheck(m_bShowProfile);
	pButton = (CButton*)GetDlgItem(IDC_CHECK3);
	if(pButton) pButton->SetCheck(m_bMedianFilter);
	
	GetData(false);
	// TODO: Add extra initialization here
	RECT rcWnd;
	GetDlgItem(IDC_STATIC_HEIGHTMAPVIEW2)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	RebuildHeightMap();
	Build();
	ShowPreViewMap();
	//ShowProfileMap();
	m_MapView.Resize(rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top);
	GetDlgItem(IDC_STATIC_HEIGHTMAPVIEW3)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	m_PreView.Resize(rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModifiedProfileDlg::GetData(bool bGet)
{
	CString str;
	CWnd *pWnd = GetDlgItem(IDC_EDIT_PATH_MAP1);
	if(pWnd) 
	{
		if(bGet)
		{	
			pWnd->GetWindowText(str);
			m_nDiffThresh = (int)atof(str);
		}else
		{
			str.Format("%d",m_nDiffThresh);
			pWnd->SetWindowText(str);
		}
	}

	pWnd = GetDlgItem(IDC_EDIT_PATH_MAP2);
	if(pWnd) 
	{
		if(bGet)
		{	
			pWnd->GetWindowText(str);
			m_nThresh = (int)atof(str);
		}else
		{
			str.Format("%d",m_nThresh);
			pWnd->SetWindowText(str);
		}
	}

	pWnd = GetDlgItem(IDC_EDIT_PATH_MAP3);
	if(pWnd) 
	{
		if(bGet)
		{	
			pWnd->GetWindowText(str);
			m_nSizeThresh = (int)atof(str);
		}else
		{
			str.Format("%d",m_nSizeThresh);
			pWnd->SetWindowText(str);
		}
	}

	pWnd = GetDlgItem(IDC_EDIT_PATH_MAP4);
	if(pWnd) 
	{
		if(bGet)
		{	
			pWnd->GetWindowText(str);
			m_nDistThresh = (int)atof(str);
		}else
		{
			str.Format("%d",m_nDistThresh);
			pWnd->SetWindowText(str);
		}
	}

	pWnd = GetDlgItem(IDC_EDIT_PATH_MAP5);
	if(pWnd) 
	{
		if(bGet)
		{	
			pWnd->GetWindowText(str);
			m_fFitThresh = (float)atof(str);
		}else
		{
			str.Format("%4.2f",m_fFitThresh);
			pWnd->SetWindowText(str);
		}
	}
}

void CModifiedProfileDlg::RebuildHeightMap()
{
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();
	if (!pHeightMap->pHeightData)
		return;

	//	Release current bitmap
	ReleaseHeightMapBmp();
	if (!(m_pBitmap = new CELBitmap))
		return;

	if (!m_pBitmap->CreateBitmap(pHeightMap->iWidth, pHeightMap->iHeight, 8, NULL))
	{
		g_Log.Log("CModifiedProfileDlg::RebuildHeightMap, Failed to create bitmap");
		return;
	}
	
	if(m_pByteData) delete []m_pByteData;
	m_pByteData = new BYTE[pHeightMap->iWidth*pHeightMap->iHeight];
	ASSERT(m_pByteData);
	if(m_pProfileData) delete []m_pProfileData;
	m_pProfileData = new BYTE[pHeightMap->iWidth*pHeightMap->iHeight];
	ASSERT(m_pProfileData);
	//	Fill bitmap data
	BYTE* pBmpLine = m_pBitmap->GetBitmapData();
	float* pHeight = pHeightMap->pHeightData;
	
	for (int i=0; i < pHeightMap->iHeight; i++)
	{
		BYTE* pPixel = pBmpLine;
		pBmpLine += m_pBitmap->GetPitch();

		for (int j=0; j < pHeightMap->iWidth; j++)
		{
			*pPixel = (BYTE)(*pHeight * 255.0f);
			int index = i*pHeightMap->iWidth + j;
			m_pByteData[index] = (BYTE)(pHeightMap->pHeightData[index]*255.0f);
			pPixel++;
			pHeight++;
		}
	}

	m_MapView.SetHeightMap(m_pBitmap);
	g_TerrainObstuctGenerator.Init(m_pByteData,pHeightMap->iWidth,pHeightMap->iHeight);
}

void CModifiedProfileDlg::ReleaseHeightMapBmp()
{
	if (m_pBitmap)
	{
		m_pBitmap->Release();
		delete m_pBitmap;
		m_pBitmap = NULL;
	}
}

void CModifiedProfileDlg::ReleasePreViewMap()
{
	if(m_pPreBitmap)
	{
		m_pPreBitmap->Release();
		delete m_pPreBitmap;
		m_pPreBitmap = NULL;
	}
}

int CModifiedProfileDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	RECT rcWnd = {0, 0, 100, 100};
	if (!m_MapView.Create(m_pTerrain, rcWnd, this, 50))
	{
		g_Log.Log("CModifiedProfileDlg::OnCreate, Failed to height map view window\n");
		return -1;
	}

	if (!m_PreView.Create(m_pTerrain, rcWnd, this, 50))
	{
		g_Log.Log("CModifiedProfileDlg::OnCreate, Failed to preview window\n");
		return -1;
	}
	return 0;
}

void CModifiedProfileDlg::ShowPreViewMap()
{
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();
	if (!pHeightMap->pHeightData)
		return;
	
	ReleasePreViewMap();

	if (!(m_pPreBitmap = new CELBitmap))
		return;

	if (!m_pPreBitmap->CreateBitmap(pHeightMap->iWidth, pHeightMap->iHeight, 8, NULL))
	{
		g_Log.Log("CModifiedProfileDlg::ShowPreViewMap, Failed to create bitmap");
		return;
	}
	BYTE* pSrcLine = m_pBitmap->GetBitmapData();
	BYTE* pDestLine = 	m_pPreBitmap->GetBitmapData();
	
	BYTE* pResult = g_TerrainObstuctGenerator.GetResultBuf();
	BYTE* pBmpLine = m_pPreBitmap->GetBitmapData();
	for (int i=0; i < pHeightMap->iHeight; i++)
	{
		BYTE* pPixel = pBmpLine;
		pBmpLine += m_pBitmap->GetPitch();

		for (int j=0; j < pHeightMap->iWidth; j++)
		{
			*pPixel = *pResult;
			pPixel++;
			pResult++;
		}
	}
	
	//	Fill bitmap data
	m_PreView.SetHeightMap(m_pPreBitmap);
}

void CModifiedProfileDlg::Build()
{
	if(m_bMedianFilter) g_TerrainObstuctGenerator.MedianFilter();		
	g_TerrainObstuctGenerator.EdgeDetection(m_nDiffThresh);	//0-255
	
	if(m_bShowProfile)
	{
		g_TerrainObstuctGenerator.ProcessEdge(m_nThresh);        //0-25
		g_TerrainObstuctGenerator.SearchAllOutlines(m_nSizeThresh,m_nDistThresh);		//5-20, 5-10
		g_TerrainObstuctGenerator.FitAllOutlines(m_fFitThresh);		//0.5-1.0
	}
}

void CModifiedProfileDlg::DrawPixel(int x, int y)
{
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();
	if (!pHeightMap->pHeightData)
		return;
	m_pProfileData[y*pHeightMap->iWidth + x] = 255;
}

void CModifiedProfileDlg::ShowProfileMap()
{
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();
	if (!pHeightMap->pHeightData)
		return;
	memset(m_pProfileData,0,pHeightMap->iHeight*pHeightMap->iWidth);
	vector< vector<CPoint> > LineFitOutlines = g_TerrainObstuctGenerator.GetOutlines();
	int num = LineFitOutlines.size();
	for( int i = 0; i< num; i++)
	{
		vector<CPoint> vpt = LineFitOutlines.at(i);	
		int numPt = vpt.size();
		for( int j = 0; j < numPt; j++)
		{
			CPoint pt = vpt.at(j);
			DrawPixel(pt.x,pt.y);
		}
	}

	ReleasePreViewMap();

	if (!(m_pPreBitmap = new CELBitmap))
		return;

	if (!m_pPreBitmap->CreateBitmap(pHeightMap->iWidth, pHeightMap->iHeight, 8, NULL))
	{
		g_Log.Log("CModifiedProfileDlg::ShowPreViewMap, Failed to create bitmap");
		return;
	}
	
	BYTE* pResult = m_pProfileData;
	BYTE* pBmpLine = m_pPreBitmap->GetBitmapData();
	for (i=0; i < pHeightMap->iHeight; i++)
	{
		BYTE* pPixel = pBmpLine;
		pBmpLine += m_pBitmap->GetPitch();

		for (int j=0; j < pHeightMap->iWidth; j++)
		{
			*pPixel = *pResult;
			pPixel++;
			pResult++;
		}
	}
	//	Fill bitmap data
	m_PreView.SetHeightMap(m_pPreBitmap);
}

void CModifiedProfileDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	// TODO: Add your message handler code here
	if(m_pByteData) delete []m_pByteData;
	if(m_pProfileData) delete []m_pProfileData;
}

void CModifiedProfileDlg::OnButtonPreview() 
{
	// TODO: Add your control notification handler code here
	BeginWaitCursor(); 
	GetData(true);
	Build();
	ShowPreViewMap();
	EndWaitCursor();
}

void CModifiedProfileDlg::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	m_bShowProfile = false;
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO2);
	if(pButton) pButton->SetCheck(m_bShowProfile);
}

void CModifiedProfileDlg::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	m_bShowProfile = true;
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO1);
	if(pButton) pButton->SetCheck(!m_bShowProfile);
}

void CModifiedProfileDlg::OnCheck3() 
{
	// TODO: Add your control notification handler code here
	m_bMedianFilter = !m_bMedianFilter;
}

void CModifiedProfileDlg::OnOK()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	CSceneObjectManager *pMan = pMap->GetSceneObjectMan();
	CTerrainObstruct *pOb = pMan->GetTerrainObstruct();
	CTerrain::HEIGHTMAP* pHeightMap = m_pTerrain->GetHeightMap();
	if (!pHeightMap->pHeightData) return;
	
	ARectF rcTerrain = m_pTerrain->GetTerrainArea();
	float fTileSize = m_pTerrain->GetTileSize();
	
	BeginWaitCursor();
	// Release old data
	pOb->Release();
	GetData(true);
	// Build obstruct profile
	if(m_bMedianFilter) g_TerrainObstuctGenerator.MedianFilter();		
	g_TerrainObstuctGenerator.EdgeDetection(m_nDiffThresh);	//0-255
	g_TerrainObstuctGenerator.ProcessEdge(m_nThresh);        //0-25
	g_TerrainObstuctGenerator.SearchAllOutlines(m_nSizeThresh,m_nDistThresh);		//5-20, 5-10
	g_TerrainObstuctGenerator.FitAllOutlines(m_fFitThresh);		//0.5-1.0
	

	// Build profile data for render
	vector< vector<CPoint> > LineFitOutlines = g_TerrainObstuctGenerator.GetLineFitOutlines();
	int num = LineFitOutlines.size();
	int nMap,nPoint;
	nMap = 0;
	for( int i = 0; i< num; i++)
	{
		nPoint = 0;
		CEditerBezier *pBezier = new CEditerBezier(nMap, g_Render.GetA3DDevice(),1.0f);
		ASSERT(pBezier);
		
		DWORD color;

		int r = (int)(255*(float)rand()/(float)RAND_MAX);
		int g = (int)(255*(float)rand()/(float)RAND_MAX);
		int b = (int)(255*(float)rand()/(float)RAND_MAX);
		color = A3DCOLORRGB(r,g,b);
		pBezier->SetProperty(color);

		vector<CPoint> vpt = LineFitOutlines.at(i);	
		int numPt = vpt.size();
		for( int j = 0; j < numPt; j++)
		{
			CPoint pt = vpt.at(j);
			float startX = pt.x*fTileSize + rcTerrain.left;
			float startZ = rcTerrain.top - pt.y*fTileSize;
			A3DVECTOR3 pos = A3DVECTOR3(startX,0,startZ);
			pos.y = m_pTerrain->GetPosHeight(pos) + 0.1f;
			pBezier->CreatePoint(pos,nMap,nPoint);
			nPoint++;
		}
		pOb->AddBezier(pBezier);
		nMap++;
	}
	EndWaitCursor();

	CDialog::OnOK();
}
