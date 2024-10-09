// TerrainConnectDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "ExpElementMap.h"
#include "MainFrm.h"
#include "TerrainConnectDlg.h"
#include "Terrain.h"
#include "AF.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CTerrainConnectDlg dialog


CTerrainConnectDlg::CTerrainConnectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTerrainConnectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTerrainConnectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nQuadrant = 4;
	m_nWidth = 0;
	m_nHeight = 0;
	m_pCurrentBt = NULL;
	m_bLinkAllMap = false;
	m_pCurrentMap = NULL;
	for(int i = 0; i < 9; i++)
	{
		m_pMap[i] = NULL;
	}
}


void CTerrainConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerrainConnectDlg)
	DDX_Control(pDX, IDC_PROGRESS_LINK, m_Progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerrainConnectDlg, CDialog)
	//{{AFX_MSG_MAP(CTerrainConnectDlg)
	ON_BN_CLICKED(IDC_RADIO_2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO_1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO_3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO_4, OnRadio4)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainConnectDlg message handlers
bool CTerrainConnectDlg::LoadHeightMap()
{
	switch(m_nQuadrant)
	{
	case 0:// not init
		break;
	case 1:// map 1,2,5
		LoadProjMap(1);
		LoadProjMap(2);
		LoadProjMap(5);
		break;
	case 2:// map 0,1,3
		LoadProjMap(0);
		LoadProjMap(1);
		LoadProjMap(3);
		break;
	case 3:// map 3,6,7
		LoadProjMap(3);
		LoadProjMap(6);
		LoadProjMap(7);
		break;
	case 4:// map 5,7,8
		LoadProjMap(5);
		LoadProjMap(7);
		LoadProjMap(8);
		break;
	}
	return true;
}

bool CTerrainConnectDlg::LoadProjMap(int nProj)
{
	CString strProject = m_strExtProj[nProj];
	if(strProject.IsEmpty()) 
	{
		m_pMap[nProj] = NULL;
		return false;
	}
	
	CElementMap *pOld = GetMapFromOld(strProject);
	if(pOld!=NULL)
	{
		m_pMap[nProj] = pOld;
		return true;
	}else
	{
		
		CString strFile;
		strFile.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, strProject, strProject);
		CElementMap * pMap = new CElementMap;
		if(pMap)
		{
			if (!pMap->Load(strFile))
			{
				AUX_ProgressLog("CTerrainConnectDlg::LoadHeightMap, Failed to call CElementMap::Load");
				delete pMap;
				pMap = NULL;
			}
		}
		m_pMap[nProj] = pMap;
	}
	return true;
}

bool CTerrainConnectDlg::SaveHeightMap()
{
	return true;
}

void CTerrainConnectDlg::ConnectHeightMap()
{
	CElementMap *pMap = m_pCurrentMap;
	if(pMap==NULL) return;
	float *pCurrentMap = pMap->GetTerrain()->GetHeightMap()->pHeightData;
	CTerrain::HEIGHTMAP *pHMap = NULL;
	
	switch(m_nQuadrant)
	{
	case 0:// not init
		break;
	case 1:// map 1,2,5
		if(m_pMap[1])
		{
			pHMap = m_pMap[1]->GetTerrain()->GetHeightMap();
			float *pData = pHMap->pHeightData;
			for(int i = 0; i < m_nWidth; i++)
				pData[(m_nHeight - 1)*m_nWidth + i] = pCurrentMap[i];
		}

		if(m_pMap[2])
		{
			pHMap = m_pMap[2]->GetTerrain()->GetHeightMap();
			float *pData = pHMap->pHeightData;
			pData[(m_nHeight - 1)*m_nWidth] = pCurrentMap[m_nWidth - 1];
		}

		if(m_pMap[5])
		{
			pHMap = m_pMap[5]->GetTerrain()->GetHeightMap();
			float *pData = pHMap->pHeightData;
			for(int i = 0; i < m_nHeight; i++)
				pData[i*m_nWidth] = pCurrentMap[i*m_nWidth + m_nWidth - 1];
		}
		break;
	case 2:// map 0,1,3
		if(m_pMap[0])
		{
			pHMap = m_pMap[0]->GetTerrain()->GetHeightMap();
			float *pData = pHMap->pHeightData;
			pData[m_nHeight * m_nWidth - 1] = pCurrentMap[0];
		}

		if(m_pMap[1])
		{
			pHMap = m_pMap[1]->GetTerrain()->GetHeightMap();
			float *pData = pHMap->pHeightData;
			for( int i = 0; i < m_nWidth; i++)
				pData[(m_nHeight - 1)*m_nWidth + i] = pCurrentMap[i];
		}

		if(m_pMap[3])
		{
			pHMap = m_pMap[3]->GetTerrain()->GetHeightMap();
			float *pData = pHMap->pHeightData;
			for(int i = 0; i < m_nHeight; i++)
				pData[i*m_nWidth + m_nWidth - 1] = pCurrentMap[i*m_nWidth + 0];
		}
		break;
	case 3:// map 3,6,7
		
		if(m_pMap[3])
		{
			pHMap = m_pMap[3]->GetTerrain()->GetHeightMap();
			float *pData = pHMap->pHeightData;
			for(int i = 0; i < m_nHeight; i++)
				pData[i*m_nWidth + m_nWidth - 1] = pCurrentMap[i*m_nWidth];
		}

		if(m_pMap[6])
		{
			pHMap = m_pMap[6]->GetTerrain()->GetHeightMap();
			float *pData = pHMap->pHeightData;
			pData[m_nWidth - 1] = pCurrentMap[(m_nHeight - 1)*m_nWidth];
		}

		if(m_pMap[7])
		{
			pHMap = m_pMap[7]->GetTerrain()->GetHeightMap();
			float *pData = pHMap->pHeightData;
			for(int i = 0; i < m_nWidth; i++)
				pData[i] = pCurrentMap[(m_nHeight - 1)*m_nWidth + i];
		}
		break;
	case 4:// map 5,7,8
		
		if(m_pMap[5])
		{
			pHMap = m_pMap[5]->GetTerrain()->GetHeightMap();
			float *pData = pHMap->pHeightData;
			for(int i = 0; i < m_nHeight; i++)
				pData[i*m_nWidth + 0] = pCurrentMap[i*m_nWidth + m_nWidth - 1];
		}
		
		if(m_pMap[7])
		{
			pHMap = m_pMap[7]->GetTerrain()->GetHeightMap();
			float *pData = pHMap->pHeightData;
			for(int i = 0; i < m_nWidth; i++)
				pData[0*m_nWidth + i] = pCurrentMap[(m_nHeight-1)*m_nWidth + i];
		}

		if(m_pMap[8])
		{
			pHMap = m_pMap[8]->GetTerrain()->GetHeightMap();
			float *pData = pHMap->pHeightData;
			pData[0] = pCurrentMap[(m_nHeight-1)*m_nWidth + m_nWidth - 1];
		}

		break;
	}
}

bool CTerrainConnectDlg::LoadProjList()
{
	DWORD dwNumCol;
	AScriptFile File;
	char szProj[256];
	sprintf(szProj,"%s\\ProjList.dat",g_szEditMapDir);
	if (!File.Open(szProj))
	{
		g_Log.Log("Open file %s failed!",szProj);
		return false;
	}
	
	if(File.GetNextToken(true))
	{
		CString strCol(File.m_szToken);
		if(strCol.Find("col=")!=-1)
		{
			strCol.Replace("col=","");
			dwNumCol = (int)atof(strCol);
		}else
		{
			File.ResetScriptFile();
			dwNumCol = 1;
		}
	}
	
	while (File.GetNextToken(true))
	{
		CString *pNew = new CString(File.m_szToken);
		ASSERT(pNew);
		m_ProjArray.push_back(pNew);
	}
	File.Close();
	return true;
}

BOOL CTerrainConnectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(0==m_BitmapList[0].LoadBitmap(IDB_QUAD_1)) return false;
	if(0==m_BitmapList[1].LoadBitmap(IDB_QUAD_2)) return false;
	if(0==m_BitmapList[2].LoadBitmap(IDB_QUAD_3)) return false;
	if(0==m_BitmapList[3].LoadBitmap(IDB_QUAD_4)) return false;

	m_pCurrentBt = &m_BitmapList[3];
	
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_1);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_2);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_3);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_4);
	if(pButton) pButton->SetCheck(true);
	
	return LoadProjList();
}

void CTerrainConnectDlg::OnOK()
{
	if(m_bLinkAllMap && AUX_GetMainFrame()->GetMap()) 
	{
		AfxMessageBox("请先关闭编辑器打开的地图文件！");
		return;
	}
	if(IDNO == AfxMessageBox("警告:请注意检查你的每张地图拼接位置是否正确，如果你已经确认无误，请选[是]，否则请选取[否]，该计算需要较长时间，强烈建议做此操作前备份地图数据!",MB_YESNO)) 
		return;
	int proj[9];

	BeginWaitCursor();
	if(m_bLinkAllMap)
	{
		m_Progress.SetRange(0,m_ProjArray.size()*5);
		m_Progress.SetStep(1);
		for(int i = 0; i< m_ProjArray.size();i++)
		{
			CString* p = m_ProjArray.at(i);
			
			if(!m_pCurrentMap)
				m_pCurrentMap = new CElementMap();
			else 
			{
				if(m_pCurrentMap && (m_pCurrentMap != AUX_GetMainFrame()->GetMap())) 
				{
					CString pathName;
					pathName.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, m_strLastProj, m_strLastProj);
					m_pCurrentMap->GetTerrain()->SetUpdateRenderFlag(true);
					m_pCurrentMap->Save(pathName);
					m_pCurrentMap->Release();
				}
			}
			m_strLastProj = *p;
			ASSERT(m_pCurrentMap);
			CString pathName;
			pathName.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, *p, *p);
			SetWindowText("Loading current map ..");
			
			CElementMap *pOld = GetMapFromOld(*p);
			if(pOld!=NULL) 
			{
				delete m_pCurrentMap;
				m_pCurrentMap = pOld;
			}else if(AUX_GetMainFrame()->GetMap() != m_pCurrentMap)
			{
				if(!m_pCurrentMap->Load(pathName))
				{
					CString msg;
					delete m_pCurrentMap;
					m_pCurrentMap = NULL;
					msg.Format("CTerrainConnectDlg::OnOK(),Load element map %s failed!",pathName); 
					g_Log.Log(msg);
					continue;
				}
			}
			m_Progress.StepIt();
			m_nWidth = m_pCurrentMap->GetTerrain()->GetHeightMap()->iWidth;
			m_nHeight = m_pCurrentMap->GetTerrain()->GetHeightMap()->iHeight;
			for( int s = 0; s < 9; ++s ) m_strExtProj[s] = "";
			GetProjectListInfor(*p,proj,m_strExtProj);
			CalculateNextProj(i);
			CalculateMap();
		}	
	}else
	{
		m_Progress.SetRange(0,5);
		m_Progress.SetStep(1);
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			int proj[9];
			m_nWidth = pMap->GetTerrain()->GetHeightMap()->iWidth;
			m_nHeight = pMap->GetTerrain()->GetHeightMap()->iHeight;
			if(m_pCurrentMap && (m_pCurrentMap != AUX_GetMainFrame()->GetMap())) 
			{
				m_pCurrentMap->Release();
				delete m_pCurrentMap;
			}
			m_pCurrentMap = pMap;
			for( int s = 0; s < 9; ++s ) m_strExtProj[s] = "";
			GetProjectListInfor(pMap->GetMapName(),proj,m_strExtProj);
			CalculateMap();
		}
	}
	if(m_pCurrentMap && (m_pCurrentMap != AUX_GetMainFrame()->GetMap())) 
	{
		CString pathName;
		pathName.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, m_strLastProj, m_strLastProj);
		if(m_pCurrentMap->GetTerrain())
		{
			m_pCurrentMap->GetTerrain()->SetUpdateRenderFlag(true);
			m_pCurrentMap->Save(pathName);
		}
		m_pCurrentMap->Release();
		delete m_pCurrentMap;
		m_pCurrentMap = NULL;
	}
	ReleaseOldMap();
	EndWaitCursor();
	CDialog::OnOK();
}

void CTerrainConnectDlg::CalculateNextProj(int nCurrentProj)
{
	int nNext = nCurrentProj + 1;
	if(nNext>=m_ProjArray.size()) 
	{
		m_strNextProj[0] = "";
		m_strNextProj[1] = "";
		m_strNextProj[2] = "";
		m_strNextProj[3] = "";
		return;
	}
	CString   strProj[9];
	int       proj[9];
	CString* pName = m_ProjArray.at(nNext);
	GetProjectListInfor(*pName,proj,strProj);
	m_strNextProj[0] = *pName;

	switch(m_nQuadrant)
	{
	case 0:// not init
		break;
	case 1:// map 1,2,5
		m_strNextProj[1] = strProj[1];
		m_strNextProj[2] = strProj[2];
		m_strNextProj[3] = strProj[5];
		break;
	case 2:// map 0,1,3
		m_strNextProj[1] = strProj[0];
		m_strNextProj[2] = strProj[1];
		m_strNextProj[3] = strProj[3];
		break;
	case 3:// map 3,6,7
		m_strNextProj[1] = strProj[3];
		m_strNextProj[2] = strProj[6];
		m_strNextProj[3] = strProj[7];
		break;
	case 4:// map 5,7,8
		m_strNextProj[1] = strProj[5];
		m_strNextProj[2] = strProj[7];
		m_strNextProj[3] = strProj[8];
		break;
	}
}

void CTerrainConnectDlg::CalculateMap()
{
	SetWindowText("Loading neighbor map ..");
	// Load map data
	LoadHeightMap();
	m_Progress.StepIt();
	SetWindowText("Replace height map ..");
	ConnectHeightMap();
	m_Progress.StepIt();
	SetWindowText("Release old map ..");
	CString strProject;
	CString strFile;
	ReleaseOldMap();
	m_Progress.StepIt();
	
	SetWindowText("Save new map for future..");
	for(int i = 0; i < 9; i++)
	{
		bool bsave = false;
		for(int j = 0; j < 4; j++)
		{
			if(strcmp( m_strExtProj[i],m_strNextProj[j])==0)
			{
				if(!m_strExtProj[i].IsEmpty())
				{
					bsave = true;
					break;
				}
			}
		}
		if(bsave) 
		{
			OLD_MAP *pOldMap = new OLD_MAP;
			pOldMap->pMap = m_pMap[i];
			pOldMap->strProjectName = m_strExtProj[i];
			m_OldMapArray.push_back(pOldMap);
		}
		else 
		{// Release map
			if(m_pMap[i])
			{
				CString strFile;
				strFile.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, m_strExtProj[i], m_strExtProj[i]);
				m_pMap[i]->RecreateTerrainRender(true,false);
				m_pMap[i]->Save(strFile);
				m_pMap[i]->Release();
				delete m_pMap[i];
			}
		}
	}

	//Reset map data to null
	for(i = 0; i < 9; i++)
	{
		m_pMap[i] = NULL;
	}
	m_Progress.StepIt();
	SetWindowText("All done ..");
}

CElementMap *CTerrainConnectDlg::GetMapFromOld(CString strProj)
{
	abase::vector<OLD_MAP *>::iterator it;
	for( it = m_OldMapArray.begin(); it != m_OldMapArray.end(); it++)
	{
		
		if((*it)==NULL) continue;
 		if(strcmp((*it)->strProjectName,strProj)==0)
		{
			CElementMap *pReturn = (*it)->pMap;
			//m_OldMapArray.erase(it);
			delete (*it);
			(*it) = NULL;
			return pReturn;
		}
	}
	return NULL;
}

void CTerrainConnectDlg::ReleaseOldMap()
{
	for(int i = 0; i< m_OldMapArray.size();i++)
	{
		OLD_MAP * p = m_OldMapArray.at(i);
		if(p==NULL) continue;
		// Save map
		CString strProject = p->strProjectName;
		CString strFile;
		strFile.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, strProject, strProject);
		if(p->pMap)
		{
			p->pMap->GetTerrain()->SetUpdateRenderFlag(true);
			p->pMap->Save(strFile);
			p->pMap->Release();
		}
		delete p;
	}
	m_OldMapArray.clear();
}

void CTerrainConnectDlg::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	m_nQuadrant = 2;
	m_pCurrentBt = &m_BitmapList[1];
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_1);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_2);
	if(pButton) pButton->SetCheck(true);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_3);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_4);
	if(pButton) pButton->SetCheck(false);
	InvalidateRect(NULL);
}

void CTerrainConnectDlg::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	m_nQuadrant = 1;
	m_pCurrentBt = &m_BitmapList[0];
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_1);
	if(pButton) pButton->SetCheck(true);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_2);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_3);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_4);
	if(pButton) pButton->SetCheck(false);
	InvalidateRect(NULL);
}

void CTerrainConnectDlg::OnRadio3() 
{
	// TODO: Add your control notification handler code here
	m_nQuadrant = 3;
	m_pCurrentBt = &m_BitmapList[2];
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_1);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_2);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_3);
	if(pButton) pButton->SetCheck(true);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_4);
	if(pButton) pButton->SetCheck(false);
	InvalidateRect(NULL);
}

void CTerrainConnectDlg::OnRadio4() 
{
	// TODO: Add your control notification handler code here
	m_nQuadrant = 4;
	m_pCurrentBt = &m_BitmapList[3];
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_1);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_2);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_3);
	if(pButton) pButton->SetCheck(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_4);
	if(pButton) pButton->SetCheck(true);
	InvalidateRect(NULL);
}

void CTerrainConnectDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if(m_pCurrentBt == NULL) return;
	// TODO: Add your message handler code here
	CWnd *pWnd = GetDlgItem(IDC_STATIC_BITMAP);
	CDC *pDC = pWnd->GetDC();
	CRect rc;
	pWnd->GetClientRect(rc);
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap* hOldBmp = (CBitmap*)MemDC.SelectObject(m_pCurrentBt);

	int iStretchMode = pDC->SetStretchBltMode(HALFTONE);
	pDC->StretchBlt(0, 0, rc.Width(), rc.Height(), &MemDC, 0, 0, 48, 48, SRCCOPY);
	pDC->SetStretchBltMode(iStretchMode);

	MemDC.SelectObject(hOldBmp);
	MemDC.DeleteDC();
	pWnd->ReleaseDC(pDC);
	// Do not call CDialog::OnPaint() for painting messages
}

void CTerrainConnectDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	// Release array
	for(int i = 0; i< m_ProjArray.size();i++)
	{
		CString* p = m_ProjArray.at(i);
		delete p;
	}
	m_ProjArray.clear();
}


