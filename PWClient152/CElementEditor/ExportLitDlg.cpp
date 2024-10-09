// ExportLitDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "ExportLitDlg.h"
#include "SceneObjectManager.h"
#include "A3D.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CExportLitDlg dialog


CExportLitDlg::CExportLitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExportLitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExportLitDlg)
	m_strExpPath = _T("");
	//}}AFX_DATA_INIT
	m_pProgress = 0;
}


void CExportLitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportLitDlg)
	DDX_Control(pDX, IDC_PROGRESS_PROCESS, m_Progress);
	DDX_Text(pDX, IDC_EDIT_EXPORT_PATH, m_strExpPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportLitDlg, CDialog)
	//{{AFX_MSG_MAP(CExportLitDlg)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportLitDlg message handlers

void CExportLitDlg::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	static char BASED_CODE szFilter[] = "Light list Files (*.dat)|*.dat";
	CFileDialog dlg(
		FALSE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,szFilter);

	if (dlg.DoModal() == IDOK)
	{
		m_strExpPath = dlg.GetPathName();
		if(m_strExpPath.Find(".dat")==-1)
		{
			m_strExpPath +=".dat";
		}
		UpdateData(false);
	}
}

void  CExportLitDlg::OnOK()
{
	
	UpdateData(true);
	ExpLitData();
	CDialog::OnOK();
}

bool CExportLitDlg::ExpLitData()
{
	int   nLitNum = 0;
	DWORD nWrited;
	AFile fExp;
	if(!fExp.Open(m_strExpPath,AFILE_BINARY|AFILE_CREATENEW | AFILE_NOHEAD)) 
	{
		AfxMessageBox("CExportLitDlg::ExpLitData(), 不能创建输出文件!");
		return false;
	}
	fExp.Write(&nLitNum,sizeof(int),&nWrited);

	CString mapName;
	CElementMap emap;
	
	// Load project file list
	AScriptFile File;
	DWORD dwCol;
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
			dwCol = (int)atof(strCol);
		}else
		{
			File.ResetScriptFile();
			dwCol = 1;
		}
		
	}
	abase::vector<CString> projectList;
	while (File.GetNextToken(true))
	{
		CString strNew(File.m_szToken);
		projectList.push_back(strNew);
	}
	File.Close();
	if(m_pProgress==0)
	{
		m_Progress.SetRange(0,projectList.size());
	    m_Progress.SetStep(1);
	}else
	{
		m_pProgress->SetRange(0,projectList.size());
		m_pProgress->SetStep(1);
	}

	int iNumCol = dwCol;
	int iNumRow = projectList.size()/dwCol;
	
	// Proessing map
	int nProj = 0;
	abase::vector<CString>::iterator theIterator;
	for(theIterator = projectList.begin(); theIterator != projectList.end(); theIterator ++)
	{
		
		mapName.Format("%s%s\\%s\\%s.elproj",g_szWorkDir,g_szEditMapDir,(*theIterator),(*theIterator));
		if(!emap.Load(mapName))
		{
			CString error_msg;
			error_msg.Format("不能打开地图文件 :%s",mapName);
			MessageBox(error_msg);
			return false;
		}
		CTerrain *pExpTerrain = emap.GetTerrain();
		float m_fProjTrnSize	= pExpTerrain->GetTerrainSize();
		float m_fWorldWid		= m_fProjTrnSize * iNumCol;
		float m_fWorldLen		= m_fProjTrnSize * iNumRow;
		int m_iProjColCnt = nProj%iNumCol;
		int m_iProjRowCnt = nProj/iNumCol;
		float fSceneOffX = -m_fWorldWid * 0.5f + m_iProjColCnt * m_fProjTrnSize + m_fProjTrnSize * 0.5f;
		float fSceneOffZ = m_fWorldLen * 0.5f - m_iProjRowCnt * m_fProjTrnSize - m_fProjTrnSize * 0.5f;
		CSceneObjectManager *pMan =emap.GetSceneObjectMan();
		ALISTPOSITION pos = pMan->m_listSceneObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = pMan->m_listSceneObject.GetPrev(pos);
			int type = ptemp->GetObjectType();
			if(type==CSceneObject::SO_TYPE_LIGHT)
			{
				CSceneLightObject *pObj = (CSceneLightObject*)ptemp;
				LIGHTDATA dat;
				pObj->GetLightData(dat);
				A3DVECTOR3 vpos = pObj->GetPos();
				vpos.x += fSceneOffX;
				vpos.z += fSceneOffZ;
				if(!ExpLitData(&fExp, dat,vpos))
				{
					fExp.Close();
					emap.Release();
					MessageBox("Failed to ExpLitData()!");
					return false;	
				}
				++nLitNum;
			}
		}
		emap.Release();
		if(m_pProgress==0)
			m_Progress.StepIt();
		else m_pProgress->StepIt();
	    ++nProj;
	}
	fExp.Seek(0,AFILE_SEEK_SET);
	fExp.Write(&nLitNum,sizeof(int),&nWrited);
	fExp.Close();	
	return true;
}

bool CExportLitDlg::ExpLitData(AFile *pFile,LIGHTDATA dat,A3DVECTOR3 pos)
{
	DWORD nWrited;
	pFile->Write(&dat.m_nEffectType,sizeof(int),&nWrited);//0 白天，1黑夜，2白天黑夜
	if(nWrited!=sizeof(int)) return false;
	
	A3DLIGHTPARAM lit;
	ZeroMemory(&lit, sizeof(lit));
	switch(dat.m_nType) 
	{
	case 0:
		lit.Type = A3DLIGHT_POINT;
		lit.Position = pos;
		lit.Ambient = a3d_ColorRGBAToColorValue(dat.m_dwAbient) * dat.m_fColorScale;
		lit.Diffuse = a3d_ColorRGBAToColorValue(dat.m_dwColor) * dat.m_fColorScale;
		lit.Range = dat.m_fRange;
		break;
	case 1:
		lit.Type = A3DLIGHT_DIRECTIONAL;
		lit.Position = pos;
		lit.Ambient = a3d_ColorRGBAToColorValue(dat.m_dwAbient) * dat.m_fColorScale;
		lit.Diffuse = a3d_ColorRGBAToColorValue(dat.m_dwColor) * dat.m_fColorScale;
		lit.Direction = dat.m_vDir;
		break;
	case 2:
		lit.Type = A3DLIGHT_SPOT;
		lit.Position = pos;
		lit.Ambient = a3d_ColorRGBAToColorValue(dat.m_dwAbient) * dat.m_fColorScale;
		lit.Diffuse = a3d_ColorRGBAToColorValue(dat.m_dwColor) * dat.m_fColorScale;
		lit.Attenuation0 = dat.m_fAttenuation0;
		lit.Attenuation1 = dat.m_fAttenuation1;
		lit.Attenuation2 = dat.m_fAttenuation2;
		lit.Falloff = dat.m_fFalloff;
		lit.Phi = dat.m_fPhi;
		lit.Theta = dat.m_fTheta;
		lit.Range = dat.m_fRange;
		break;
	default:
		break;
	}
	pFile->Write(&lit,sizeof(A3DLIGHTPARAM),&nWrited);
	if(nWrited!=sizeof(A3DLIGHTPARAM)) return false;
	return true;
}


bool ReadLitList()
{
	AFile file;
	if(!file.Open("bbbb.dat",AFILE_BINARY|AFILE_OPENEXIST)) 
	{
		return false;
	}
	int nLitNum = 0;
	DWORD nReaded;
	file.Read(&nLitNum,sizeof(int),&nReaded);
	for( int i = 0; i < nLitNum; ++i)
	{
		int nEffectType;//0 白天，1黑夜，2白天黑夜
		file.Read(&nEffectType,sizeof(int),&nReaded);
		A3DLIGHTPARAM params;
		file.Read(&params,sizeof(A3DLIGHTPARAM),&nReaded);
	}
	return true;
}

void  CExportLitDlg::OnCancel()
{
	ReadLitList();
	CDialog::OnCancel();
}




