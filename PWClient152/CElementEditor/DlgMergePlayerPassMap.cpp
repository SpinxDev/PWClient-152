// DlgMergePlayerPassMap.cpp : implementation file
//

#include "Global.h"
#include "DlgMergePlayerPassMap.h"
#include "ElementMap.h"

#include <AScriptFile.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMergePlayerPassMap dialog


CDlgMergePlayerPassMap::CDlgMergePlayerPassMap(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMergePlayerPassMap::IDD, pParent)
	, m_pMapGridSelectPolicy(NULL)
{
	//{{AFX_DATA_INIT(CDlgMergePlayerPassMap)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDlgMergePlayerPassMap::~CDlgMergePlayerPassMap()
{
	delete m_pMapGridSelectPolicy;
	m_pMapGridSelectPolicy = NULL;
}

void CDlgMergePlayerPassMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMergePlayerPassMap)
	DDX_Control(pDX, IDC_COMBO_LAYER, m_cbSelPPassMapLayer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMergePlayerPassMap, CDialog)
	//{{AFX_MSG_MAP(CDlgMergePlayerPassMap)
	ON_BN_CLICKED(IDC_MERGE, OnMerge)
	ON_CBN_SELCHANGE(IDC_COMBO_LAYER, OnSelchangeComboLayer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMergePlayerPassMap message handlers

static CString GetPlayerPassMapMergeName(int iRow1, int iRow2, int iCol1, int iCol2, int iCurLayer, const char *szExt)
{
	CString strPath;
	strPath.Format("r%d_%d-c%d_%d-l%d.%s", iRow1, iRow2, iCol1, iCol2, iCurLayer, szExt);
	return strPath;
}
static CString GetPlayerPassMapMergePath(int iRow1, int iRow2, int iCol1, int iCol2, int iCurLayer, const char *szExt)
{
	CString strName = GetPlayerPassMapMergeName(iRow1, iRow2, iCol1, iCol2, iCurLayer, szExt);
	CString strPath;
	strPath.Format("%s%s\\%s", g_szWorkDir, g_szEditMapDir, strName);
	return strPath;
}

void CDlgMergePlayerPassMap::OnMerge() 
{
	//	�ϲ���ǰѡ�еĶ����ɫѰ��ͼ
	int iLayer = m_cbSelPPassMapLayer.GetCurSel();
	if (iLayer < 0){
		return;
	}
	int iRow1(0), iRow2(0), iCol1(0), iCol2(0);
	if (!m_pMapGridSelectPolicy->GetSelect(iRow1, iRow2, iCol1, iCol2)){
		AfxMessageBox("��ѡ�к��н�ɫѰ��ͼ�����ڿ����кϲ�!");
		return;
	}

	//	���ز��ϲ�
	CBitImage * final_rmap = NULL;			//	ֱ�Ӻϲ��� CBitImage �����У���ʡ�ڴ�
	FIX16 * final_arrHeightMap = NULL;		//	�ϲ��������У�����ٵ���Ϊ CBlockImage �������Ч��
	
	int sub_img_width(0), sub_img_length(0);
	int	final_img_width(0), final_img_length(0);
	float fPixelSize = 1.0f;

	int i(0), j(0), u(0), v(0);
	bool bError(false);
	CString strMsg;
	for (int row = iRow1; row <= iRow2; ++ row)
	{
		for (int col = iCol1; col <= iCol2; ++ col)
		{
			int iProj = row * m_projList.GetNumCol() + col;
			const CString &strProjName = m_projList.Get(iProj);

			//	���� prmap
			CString strProjPath = CElementMap::GetPlayerPassMapPath(strProjName, iLayer, "prmap");
			CBitImage * pRMap = new CBitImage;
			if (!pRMap->Load(strProjPath)){
				g_Log.Log("CDlgMergePlayerPassMap::OnMerge, Failed to load %s", strProjPath);
				strMsg.Format("���� %s ʧ��", strProjPath);
				A3DRELEASE(pRMap);
				bError = true;
				break;
			}
			const float ZERO_PIXEL_SIZE = 0.01f;
			int iTempWidth(0), iTempLength(0);
			float fTempSize = pRMap->GetPixelSize();
			pRMap->GetImageSize(iTempWidth, iTempLength);
			if (iTempWidth <= 0 || iTempLength <= 0 || fTempSize <= ZERO_PIXEL_SIZE ||
				sub_img_width > 0 && iTempWidth != sub_img_width ||
				sub_img_length > 0 && iTempLength != sub_img_length ||
				fPixelSize > ZERO_PIXEL_SIZE && fabs(fTempSize-fPixelSize) > ZERO_PIXEL_SIZE){
				g_Log.Log("CDlgMergePlayerPassMap::OnMerge, Invalid image size(width=%d, length=%d, pixel=%f) for %s", iTempWidth, iTempLength, fTempSize, strProjPath);
				strMsg.Format("%s ��С����(width=%d, length=%d, pixel=%f)", strProjPath, iTempWidth, iTempLength, fTempSize);
				A3DRELEASE(pRMap);
				bError = true;
				break;
			}
			if (sub_img_width <= 0){
				sub_img_width = iTempWidth;
				sub_img_length = iTempLength;
				final_img_width = sub_img_width * (iCol2 - iCol1 + 1);
				final_img_length = sub_img_length * (iRow2 - iRow1 + 1);
				fPixelSize = pRMap->GetPixelSize();
			}			
			//	�ϲ�����ͼ��
			if (!final_rmap){
				final_rmap = new CBitImage;
				final_rmap->InitZero(final_img_width, final_img_length, fPixelSize);
			}
			u = (col-iCol1) * sub_img_width;
			v = (iRow2-row) * sub_img_length;	//	(0,0)���Ӧ��ͼ���½�
			for (j = 0; j < sub_img_length; ++ j)
			{
				for (i = 0; i < sub_img_width; ++ i)
				{
					final_rmap->SetPixel(u+i, v+j, pRMap->GetPixel(i, j));
				}
			}
			//	�ͷ�
			A3DRELEASE(pRMap);
			
			//	���� pdhmap
			strProjPath = CElementMap::GetPlayerPassMapPath(strProjName, iLayer, "pdhmap");
			CBlockImage<FIX16> * pDHMap = new CBlockImage<FIX16>;
			if (!pDHMap->Load(strProjPath)){
				g_Log.Log("CDlgMergePlayerPassMap::OnMerge, Failed to load %s", strProjPath);
				strMsg.Format("���� %s ʧ��", strProjPath);
				A3DRELEASE(pDHMap);
				bError = true;
				break;
			}
			int iTempWidth2(0), iTempLength2(0);
			float fTempSize2 = pDHMap->GetPixelSize();
			pDHMap->GetImageSize(iTempWidth2, iTempLength2);
			if (iTempWidth2 <= 0 || iTempLength2 <= 0 || fTempSize2 <= ZERO_PIXEL_SIZE ||
				iTempWidth2 != sub_img_width || iTempLength2 != sub_img_length || fabs(fTempSize2-fPixelSize) > ZERO_PIXEL_SIZE){
				g_Log.Log("CDlgMergePlayerPassMap::OnMerge, Invalid image size(width=%d, length=%d, pixel=%f) for %s", iTempWidth2, iTempLength2, fTempSize2, strProjPath);
				strMsg.Format("%s ��С����(width=%d, length=%d, pixel=%f)", strProjPath, iTempWidth2, iTempLength2, fTempSize2);
				A3DRELEASE(pDHMap);
				bError = true;
				break;
			}
			//	�ϲ�����ͼ��
			if (!final_arrHeightMap){		
				final_arrHeightMap  = new FIX16[final_img_width * final_img_length];
			}
			for (j = 0; j < sub_img_length; ++ j)
			{
				for (i = 0; i < sub_img_width; ++ i)
				{
					final_arrHeightMap[(v+j)*final_img_width + (u+i)] = pDHMap->GetPixel(i, j);
				}
			}
			//	�ͷ�
			A3DRELEASE(pDHMap);
		}
		if (bError){
			break;
		}
	}

	//	���浽�ļ�
	while (!bError)
	{
		//	д��ϲ��� prmap
		CString strRMapPath = GetPlayerPassMapMergePath(iRow1, iRow2, iCol1, iCol2, iLayer, "prmap");
		if (!final_rmap->Save(strRMapPath)){
			g_Log.Log("CDlgMergePlayerPassMap::OnMerge, Save error %s", strRMapPath);
			strMsg.Format("д���ļ��������� %s", strRMapPath);
			break;
		}
		A3DRELEASE(final_rmap);
		
		//	д��ϲ��� pdhmap
		CString strDHMapPath = GetPlayerPassMapMergePath(iRow1, iRow2, iCol1, iCol2, iLayer, "pdhmap");
		CBlockImage<FIX16> final_dhmap;
		final_dhmap.Init(final_arrHeightMap, final_img_width, final_img_length, fPixelSize);
		if (!final_dhmap.Save(strDHMapPath)){
			g_Log.Log("CDlgMergePlayerPassMap::OnMerge, Save error %s", strDHMapPath);
			strMsg.Format("д���ļ��������� %s", strDHMapPath);
			break;
		}
		if (final_arrHeightMap){
			delete [] final_arrHeightMap;
			final_arrHeightMap = NULL;
		}

		//	д��Ѱ·�༭�� AutoPFImp �б༭�����ļ������ļ�
		CString strCfgPath = GetPlayerPassMapMergePath(iRow1, iRow2, iCol1, iCol2, iLayer, "txt");
		FILE * fConfig = fopen(strCfgPath, "wt");
		if (!fConfig){
			g_Log.Log("CDlgMergePlayerPassMap::OnMerge, Failed to open %s", strCfgPath);
			strMsg.Format("���������ļ��������� %s", strCfgPath);
			break;
		}else{
			fprintf(fConfig, "#config for pf map\n");
			fprintf(fConfig, "clu_param= %d %d\n", 100, 30);	//	iClusterSize, iEntranceWidth
			fprintf(fConfig, "#layer %d\n", 0);
			fprintf(fConfig, "layer\n");
			fprintf(fConfig, "\trmap= %s\n", GetPlayerPassMapMergeName(iRow1, iRow2, iCol1, iCol2, iLayer, "prmap"));
			fprintf(fConfig, "\tdhmap= %s\n", GetPlayerPassMapMergeName(iRow1, iRow2, iCol1, iCol2, iLayer, "pdhmap"));
			fprintf(fConfig, "end\n");
			fclose(fConfig);
		}
		
		//	���ļ�����Ŀ¼
		CString strCmd;
		strCmd.Format("/select, %s", strCfgPath);
		ShellExecute(NULL, _T("open"), _T("explorer.exe"), (LPCTSTR)strCmd, NULL, SW_SHOW);
		break;
	}
	A3DRELEASE(final_rmap);
	if (final_arrHeightMap){
		delete [] final_arrHeightMap;
		final_arrHeightMap = NULL;
	}
	if (!strMsg.IsEmpty()){
		AfxMessageBox(strMsg);
	}
}

BOOL CDlgMergePlayerPassMap::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (!m_projList.Init()){
		g_Log.Log("CDlgMergePlayerPassMap::OnInitDialog, Failed to init ProjList.dat.");
	}
	
	RECT rcWnd;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	
	m_pMapGridSelectPolicy = new CRangedMapGridSelectPolicy;
	m_mapGrid.SetSelectPolicy(m_pMapGridSelectPolicy);
	if (!m_mapGrid.Create(this, rcWnd, 40))
	{
		g_Log.Log("CDlgMergePlayerPassMap::OnInitDialog, Failed to create map grid window.");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	m_mapGrid.SetTileAndRow(m_projList.GetList(), m_projList.GetNumCol(), false);
	
	int nMaxLayer = CElementMap::GetPlayerPassMapLayerCount();
	CString strTemp;
	for (int i(0); i < nMaxLayer; ++ i)
	{
		strTemp.Format("Layer%d", i);
		m_cbSelPPassMapLayer.AddString(strTemp);
	}
	if (nMaxLayer > 0){
		m_cbSelPPassMapLayer.SetCurSel(0);
		OnSelchangeComboLayer();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMergePlayerPassMap::OnSelchangeComboLayer() 
{
	//	���ݸ��ֿ��ͼĿ¼���Ƿ����ѡ�� Layer ��Ѱ���ļ���������Щ��ɲ���ϲ�
	int iSel = m_cbSelPPassMapLayer.GetCurSel();
	if (iSel >= 0 && iSel < m_cbSelPPassMapLayer.GetCount()){
		m_mapGrid.ResetSelect();
		CString strPath;
		for (int i = 0; i < m_projList.GetListCount(); ++ i)
		{
			const CString &strProj = m_projList.Get(i);
			strPath = CElementMap::GetPlayerPassMapPath(strProj, iSel, "prmap");
			if (!AUX_FileIsExist(strPath)){
				continue;;
			}
			strPath = CElementMap::GetPlayerPassMapPath(strProj, iSel, "pdhmap");
			if (!AUX_FileIsExist(strPath)){
				continue;;
			}
			m_mapGrid.EnableSelect(i, true);
		}
	}
}
