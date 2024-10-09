// DlgExportBHT.cpp : implementation file
//

#include "Global.h"
#include "DlgExportBHT.h"
#include "ExpElementMap.h"
#include "ExpTerrain.h"
#include "BrushTreeExporter.h"
#include "TerrainPlants.h"
#include "SceneObjectManager.h"
#include "SceneObject.h"
#include "StaticModelObject.h"
#include "EL_Forest.h"
#include "EL_Tree.h"
#include "EL_Building.h"
#include "SceneSkinModel.h"
#include "MD5Collector.h"

#include <A3DFuncs.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//	�ֲ�����

#ifdef EXPORT_BRUSH_MD5
MD5Collector	g_md5Collector;
CString			g_md5TempFile;
#endif

//	���ں� EXPORT_BRUSH_MD5	��˵��
//	��;:		��ÿ���ɹ�������͹���������� md5 ֵ��ÿ��һ����������С��������
//				������ԭ�е������� SvrCDTest �Ƚ���֤
//	����֤��:	Tree �ܵ�����ԭ������ȫ��ͬ��͹��
//				Static Model ��������м�����md5 ֵ��ȫ��ͬ��������Ӧ͹��������ȡ���������ͬ
//				Ecm Models ��ǰ������Ŀ����ĿΪ0���޷�����

void CollectBrushMD5(CCDBrush *pBrush)
{
#ifdef EXPORT_BRUSH_MD5
	AFile aFile;
	if (aFile.Open(g_md5TempFile, AFILE_CREATENEW | AFILE_BINARY | AFILE_NOHEAD)){
		pBrush->Save(&aFile);

// 		DWORD wrtLen(0);
// 
// 		int nSides = *(int *)((char *)pBrush+4);
// 		aFile.Write(&nSides, sizeof(int), &wrtLen);
// 
// 		CCDSide *pSides = *(CCDSide **)(pBrush);
// 		int a(0);
// 		for (int i = 0; i < nSides; i++) {			
// 			A3DVECTOR3 vNormal(pSides[i].plane.GetNormal());
// 			a = vNormal.x;
// 			aFile.Write(&a, sizeof(a), &wrtLen);
// 			a = vNormal.y;
// 			aFile.Write(&a, sizeof(a), &wrtLen);
// 			a = vNormal.z;
// 			aFile.Write(&a, sizeof(a), &wrtLen);
// 			
// 			a = pSides[i].plane.GetDist();
// 			aFile.Write(&a, sizeof(a), &wrtLen);
// 			
// 			bool bBevel = pSides[i].bevel;
// 			aFile.Write(&bBevel, sizeof(bool), &wrtLen);
// 		}
		
		aFile.Flush();
		aFile.Close();
		g_md5Collector.Add(g_md5TempFile);
		DeleteFileA(g_md5TempFile);
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CDlgExportBHT dialog


CDlgExportBHT::CDlgExportBHT(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportBHT::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportBHT)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDlgExportBHT::~CDlgExportBHT()
{
	delete m_pMapGridSelectPolicy;
	m_pMapGridSelectPolicy = NULL;
}


void CDlgExportBHT::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportBHT)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportBHT, CDialog)
	//{{AFX_MSG_MAP(CDlgExportBHT)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	ON_BN_CLICKED(IDC_UNSELECT_ALL, OnUnselectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportBHT message handlers

BOOL CDlgExportBHT::OnInitDialog() 
{	
	CDialog::OnInitDialog();
	
	if (!m_projList.Init()){
		g_Log.Log("CDlgExportBHT::OnInitDialog, Failed to init ProjList.dat.");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	
	RECT rcWnd;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	
	m_pMapGridSelectPolicy = new CSeperateMapGridSelectPolicy;
	m_mapGrid.SetSelectPolicy(m_pMapGridSelectPolicy);
	if (!m_mapGrid.Create(this, rcWnd, 40))
	{
		g_Log.Log("CDlgExportBHT::OnInitDialog, Failed to create map grid window.");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	m_mapGrid.SetTileAndRow(m_projList.GetList(), m_projList.GetNumCol(), true);	
	OnSelectAll();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

static bool l_RetrieveEcpPath(const char * ecmPath, AString& ecpPath)
{
	const char* szPath = strstr(ecmPath, "\\");
	if (!szPath)
	{
		return false;
	}
	char szProjFile[MAX_PATH];
	strcpy(szProjFile, "grasses\\ECModelHull");
	strcat(szProjFile, szPath);
	
	char* ext = strrchr(szProjFile, '.');
	if (!ext)
	{
		return false;
	}
	strcpy(ext, ".ecp");
	
	ecpPath = szProjFile;
	return true;
}

#define ECMODEL_PROJ_VERSION (unsigned int) (0x00000001)
struct ECModelBrushProjHead_t
{
	unsigned int	nMagic;
	unsigned int	nVersion;
	char			szECModelFile[MAX_PATH];
	int				nFrame;
	unsigned int	nNumHull;
};

//	͹������������
struct BrushExportCounter
{
	int	nBrushStaticModel;
	int	nBrushEcmModel;
	int	nBrushTree;
	int	nStaticModelBrushIgnored;
	int	nEcmModelBrushIgnored;
	int	nTreeBrushIgnored;
	
	BrushExportCounter()
		: nBrushStaticModel(0), nBrushEcmModel(0), nBrushTree(0)
		, nStaticModelBrushIgnored(0), nEcmModelBrushIgnored(0), nTreeBrushIgnored(0)
	{
	}
	
	int GetExported()const{
		return nBrushStaticModel + nBrushEcmModel + nBrushTree;
	}
	int GetIgnored()const{
		return nStaticModelBrushIgnored + nEcmModelBrushIgnored + nTreeBrushIgnored;
	}
};

//	�ṩ͹������Ĳ����ͷ���
class  ConvexHullDataCollection
{
	typedef	abase::vector<CConvexHullData *> ConvexHullDataArray;
	ConvexHullDataArray	m_brushes;

public:
	void Add(const CConvexHullData &rhs){
		m_brushes.push_back(new CConvexHullData(rhs));
	}
	void Add(const ConvexHullDataArray &rhs){
		for (ConvexHullDataArray::const_iterator cit = rhs.begin(); cit != rhs.end(); ++ cit)
		{
			CConvexHullData *pData = *cit;
			Add(*pData);
		}
	}
	void Release(){
		for (ConvexHullDataArray::iterator it = m_brushes.begin(); it != m_brushes.end(); ++ it)
		{
			delete *it;
		}
		m_brushes.clear();
	}
	int GetCount()const{
		return (int)m_brushes.size();
	}
	const CConvexHullData * GetAt(int index)const{
		return m_brushes[index];
	}
};

//	���� Project �ڿɵ������������� bht �ļ��е�͹���б�
class ProjectBrush 
{
	CString	m_strProjName;
	bool	m_bNeedExport;
	int		m_iRow;
	int		m_iCol;
	float	m_fTrnSize;	

	ConvexHullDataCollection	m_staticModelBrushes;	//	ÿ�� Brush ���ڵ�ǰ Project �Ķ�������ϵ��
	ConvexHullDataCollection	m_ecmModelBrushes;
	ConvexHullDataCollection	m_treeBrushes;

public:
	ProjectBrush()
		: m_bNeedExport(false), m_iRow(0), m_iCol(0), m_fTrnSize(0.0f){
	}

	bool operator == (const ProjectBrush &rhs)const{
		return rhs.m_iRow == m_iRow && rhs.m_iCol == m_iCol;
	}
	bool operator != (const ProjectBrush & rhs)const{
		return !operator==(rhs);
	}

	//	Set
	void SetProjName(const CString &strProjName){
		m_strProjName = strProjName;
	}
	void SetNeedExport(bool bVal){
		m_bNeedExport = bVal;
	}
	void SetRowCol(int iRow, int iCol){
		m_iRow = iRow;
		m_iCol = iCol;
	}
	void SetTerrainSize(float fTrnSize){
		m_fTrnSize = fTrnSize;
	}

	//	Get
	const CString &GetProjName()const{
		return m_strProjName;
	}
	bool GetNeedExport()const{
		return m_bNeedExport;
	}
	float GetTerrainSize()const{
		return m_fTrnSize;
	}
	ConvexHullDataCollection & GetStaticModelBrushes(){
		return m_staticModelBrushes;
	}
	ConvexHullDataCollection & GetEcmModelBrushes(){
		return m_ecmModelBrushes;
	}
	ConvexHullDataCollection & GetTreeBrushes(){
		return m_treeBrushes;
	}
	const ConvexHullDataCollection & GetStaticModelBrushes()const{
		return m_staticModelBrushes;
	}
	const ConvexHullDataCollection & GetEcmModelBrushes()const{
		return m_ecmModelBrushes;
	}
	const ConvexHullDataCollection & GetTreeBrushes()const{
		return m_treeBrushes;
	}

	//	Collect
	void CollectBrushes(CExpElementMap &Map)
	{
		CollectStaticModels(Map);
		CollectEcmModels(Map);
		CollectTrees(Map);
	}

	//	Export
	A3DVECTOR3 GetOffsetRelativeTo(const ProjectBrush &rhs)const{
		//	��ȡ this ��������� rhs ���ĵ�ƫ�ƣ����ڽ� this ������ brushes �任�� rhs ������ϵ�£��Ӷ����� this ���� rhs ��Χ�غϵ� brushes
		A3DVECTOR3 offset(0.0f);
		offset.x = (m_iCol - rhs.m_iCol) * m_fTrnSize;
		offset.z = (rhs.m_iRow - m_iRow) * m_fTrnSize;	//	Z �᷽���� iRow ���ӷ����෴
		return offset;
	}
	void Export(CBrushTreeExporter &exporter, BrushExportCounter &counter)const
	{
		ExportCollection(GetStaticModelBrushes(), exporter, counter.nBrushStaticModel, counter.nStaticModelBrushIgnored);
		ExportCollection(GetEcmModelBrushes(), exporter, counter.nBrushEcmModel, counter.nEcmModelBrushIgnored);
		ExportCollection(GetTreeBrushes(), exporter, counter.nBrushTree, counter.nTreeBrushIgnored);
	}
	void ExportFor(const ProjectBrush &ref, CBrushTreeExporter &exporter, BrushExportCounter &counter)const
	{
		A3DVECTOR3 vOffset = GetOffsetRelativeTo(ref);
		ExportCollectionWithOffset(vOffset, GetStaticModelBrushes(), exporter, counter.nBrushStaticModel, counter.nStaticModelBrushIgnored);
		ExportCollectionWithOffset(vOffset, GetEcmModelBrushes(), exporter, counter.nBrushEcmModel, counter.nEcmModelBrushIgnored);
		ExportCollectionWithOffset(vOffset, GetTreeBrushes(), exporter, counter.nBrushTree, counter.nTreeBrushIgnored);
	}

	void Release(){
		m_staticModelBrushes.Release();
		m_ecmModelBrushes.Release();
		m_treeBrushes.Release();
	}

private:
	
	//	Collect
	void CollectStaticModels(CExpElementMap &Map)
	{
		CSceneObjectManager *pSceneObjectManager = Map.GetSceneObjectMan();
		ALISTPOSITION pos = pSceneObjectManager->m_listSceneObject.GetHeadPosition();
		while (pos)
		{
			CSceneObject* pSceneObject = pSceneObjectManager->m_listSceneObject.GetNext(pos);
			if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_STATIC_MODEL)
				continue;
			
			CStaticModelObject* pObject = (CStaticModelObject*)pSceneObject;
			ESTATICMODEL* pModel = pObject->GetA3DModel();
			if (!pModel || !pModel->pModel)
			{
				g_Log.Log("CollectStaticModels(project %s), model pointer == null!", GetProjName());
				continue;
			}
			
			GetStaticModelBrushes().Add(pModel->pModel->GetConvexHulls());
		}
	}	
	void CollectEcmModels(CExpElementMap &Map)
	{
		CSceneObjectManager *pSceneObjectManager = Map.GetSceneObjectMan();	
		ALISTPOSITION pos = pSceneObjectManager->m_listSceneObject.GetHeadPosition();
		while (pos)
		{
			CSceneObject* pSceneObject = pSceneObjectManager->m_listSceneObject.GetNext(pos);
			if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_ECMODEL)
				continue;
			
			CSceneSkinModel* pObject = (CSceneSkinModel*)pSceneObject;
			if (!pObject->GetECModel())
				continue;
			
			AString ecp_path;
			if (!l_RetrieveEcpPath(pObject->GetProperty().strPathName, ecp_path)){
				continue;
			}
			
			AFileImage file;
			if (!file.Open(ecp_path, AFILE_OPENEXIST | AFILE_BINARY)){
				g_Log.Log("CollectEcmModels(project %s), Failed to open %s", GetProjName(), ecp_path);
				continue;
			}
			
			ECModelBrushProjHead_t ProjHead;
			DWORD dwLen;			
			file.Read(&ProjHead, sizeof(ProjHead), &dwLen);	
			if (ProjHead.nVersion == ECMODEL_PROJ_VERSION && ProjHead.nNumHull > 0)
			{			
				A3DVECTOR3 vPos = pObject->GetPos();
				A3DVECTOR3 vDir = a3d_Normalize(pObject->GetTransMatrix().GetRow(2));
				A3DVECTOR3 vUp = a3d_Normalize(pObject->GetTransMatrix().GetRow(1));
				A3DMATRIX4 matTM = a3d_TransformMatrix(vDir, vUp, vPos);
				
				for (unsigned i = 0; i < ProjHead.nNumHull; i++)
				{
					CHBasedCD::CConvexHullData hull;
					hull.LoadBinaryData(&file);
					// currently, the ECModelHullEditor has turned the model 180 degree which is a mistake,
					// so we have to turn it back before we use it.
					hull.Transform(TransformMatrix(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1.0f, 0), A3DVECTOR3(0)));
					hull.Transform(matTM);
					GetEcmModelBrushes().Add(hull);
				}
			}
			file.Close();
		}
	}	
	void CollectTrees(CExpElementMap &Map)
	{
		CSceneObjectManager *pSceneObjectManager = Map.GetSceneObjectMan();
		CTerrainPlants* pTerrainPlants = pSceneObjectManager->GetPlants();
		CELForest *pForest = pSceneObjectManager->GetElForest();
		if (pTerrainPlants && pForest){			
			APtrList<PPLANT>& PlantList = pTerrainPlants->GetPlantList();			
			ALISTPOSITION pos1 = PlantList.GetHeadPosition();
			while (pos1)
			{
				PLANT* pPlants = PlantList.GetNext(pos1);
				if (pPlants->nPlantType != PLANT_TREE)
					continue;
				
				CELTree* pTree = pForest->GetTreeTypeByID(pPlants->nID);
				if (!pTree){
					g_Log.Log("CollectTrees(project %s), Failed to call GetTreeTypeByID(%d)", GetProjName(), pPlants->nID);
					CString strMsg;
					strMsg.Format(_T("�޷��ҵ���(nID=%d)�����Ե���"), pPlants->nID);
					AfxMessageBox(strMsg, MB_ICONSTOP);
					continue;
				}
				
				ALISTPOSITION pos2 = pPlants->m_listPlantsPos.GetHeadPosition();
				while (pos2)
				{
					PLANTPOS* pPlantInfo = pPlants->m_listPlantsPos.GetNext(pos2);
					
					A3DMATRIX4 mat;
					mat.Translate(pPlantInfo->x, pPlantInfo->y, pPlantInfo->z);
					for (int k = 0; k < pTree->GetNumHull(); k++)
					{
						CConvexHullData * pHull = pTree->GetHull(k);
						CConvexHullData   hull = *pHull;
						hull.Transform(mat);
						GetTreeBrushes().Add(hull);
					}
				}
			}
		}
	}
	
	//	Export
	void ExportCollection(const ConvexHullDataCollection &hulls, CBrushTreeExporter &exporter, int &nExported, int &nIgnored)const
	{
		for (int i = 0; i < hulls.GetCount(); ++i)
		{
			CConvexHullData hull = *hulls.GetAt(i);
			
			CQBrush qBrush;
			qBrush.AddBrushBevels(&hull);
			CCDBrush *pCDBrush = new CCDBrush;
			qBrush.Export(pCDBrush);
			if (exporter.AddBrush(pCDBrush)){
				CollectBrushMD5(pCDBrush);
				++ nExported;
			}else{
				delete pCDBrush;
				++ nIgnored;
			}
		}
	}

	void ExportCollectionWithOffset(const A3DVECTOR3 &vOffset, const ConvexHullDataCollection &hulls, CBrushTreeExporter &exporter, int &nExported, int &nIgnored)const
	{
		A3DMATRIX4 tm = a3d_IdentityMatrix();
		tm.Translate(vOffset.x, vOffset.y, vOffset.z);
		for (int i = 0; i < hulls.GetCount(); ++i)
		{
			CConvexHullData hull = *hulls.GetAt(i);
			hull.Transform(tm);
			
			CQBrush qBrush;
			qBrush.AddBrushBevels(&hull);
			CCDBrush *pCDBrush = new CCDBrush;
			qBrush.Export(pCDBrush);
			if (exporter.AddBrush(pCDBrush)){
				CollectBrushMD5(pCDBrush);
				++ nExported;
			}else{
				delete pCDBrush;
				++ nIgnored;
			}
		}
	}
};
typedef abase::vector<ProjectBrush>	ProjectsBrush;

CString ExportProjectBrush( ProjectBrush &projBrush, const CString &strExportDir)
{
	CBrushTreeExporter  exporter;
	
	float fTrnSize = projBrush.GetTerrainSize();
	A3DVECTOR3 map_ext;
	map_ext.x = fTrnSize * 0.5f;
	map_ext.y = 0.0f;
	map_ext.z = fTrnSize * 0.5f;
	exporter.Build(map_ext);
	
#ifdef EXPORT_BRUSH_MD5		
	g_md5Collector.Clear();
	g_md5TempFile.Format("%s\\md5.tmp", strExportDir);
#endif
	
	BrushExportCounter curProjCounter;
	projBrush.Export(exporter, curProjCounter);
	
	CString strBHTPath;
	strBHTPath.Format("%s\\%s.bht", strExportDir, projBrush.GetProjName());
	SetFileAttributes(strBHTPath, FILE_ATTRIBUTE_NORMAL);	//	��������ļ�ֻ�������ԣ�ȷ�������ɹ�
	exporter.Export(strBHTPath);
	
#ifdef EXPORT_BRUSH_MD5
	g_md5Collector.Sort();
	g_md5Collector.Output(strBHTPath+".md5");
#endif
	
	g_Log.Log("%d brushes(static model:%d, ecm models:%d, tree:%d) exported to %s. %d brushes Ignored in current proj(static model:%d, ecm models:%d, tree:%d)",
		curProjCounter.GetExported(),
		curProjCounter.nBrushStaticModel,
		curProjCounter.nBrushEcmModel,
		curProjCounter.nBrushTree,
		(LPCTSTR)strBHTPath,
		curProjCounter.GetIgnored(),
		curProjCounter.nStaticModelBrushIgnored,
		curProjCounter.nEcmModelBrushIgnored,
		curProjCounter.nTreeBrushIgnored);	
	
	return strBHTPath;
}

void CDlgExportBHT::OnExport() 
{
	int iTile(0);

	//	����Ƿ�ѡ������Ϊ��
	for (iTile = 0; iTile < m_pMapGridSelectPolicy->GetTileNum(); ++ iTile)
	{
		if (m_pMapGridSelectPolicy->IsSelected(iTile)){
			break;
		}
	}
	if (iTile >= m_pMapGridSelectPolicy->GetTileNum()){
		return;
	}
	
	//	���ѡ�е�ͼ����͹��
	CString strLastBHT;
	for (iTile = 0; iTile < m_pMapGridSelectPolicy->GetTileNum(); ++ iTile)
	{
		bool bNeedExport = m_pMapGridSelectPolicy->IsSelected(iTile);
		if (!bNeedExport){
			continue;
		}
		const CString &strProjName = m_projList.Get(iTile);
		CString strFile;
		strFile.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, strProjName, strProjName);
		
		CExpElementMap Map;
		if (!Map.Load(strFile, LOAD_EXPSCENE))
		{
			g_Log.Log("CDlgExportBHT::OnExport, Failed to call CExpElementMap::Load(%s)", strProjName);
			CString strMsg;
			strMsg.Format(_T("���� %s ʧ�ܣ����Ե�����"), strProjName);
			AfxMessageBox(strMsg, MB_ICONSTOP);
			continue;
		}

		ProjectBrush projBrush;

		projBrush.SetProjName(strProjName);
		projBrush.SetNeedExport(bNeedExport);		
		projBrush.SetTerrainSize(Map.GetTerrain()->GetTerrainSize());

		int iRow(0), iCol(0);
		m_pMapGridSelectPolicy->GetRowCol(iTile, iRow, iCol);
		projBrush.SetRowCol(iRow, iCol);

		projBrush.CollectBrushes(Map);
		strLastBHT = ExportProjectBrush(projBrush, m_strExportDir);
		projBrush.Release();
		
		Map.Release();
	}
	
	if (!strLastBHT.IsEmpty()){
		//	�򿪵����ļ���
		CString strCmd;
		strCmd.Format("/select, %s", strLastBHT);
		ShellExecute(NULL, _T("open"), _T("explorer.exe"), (LPCTSTR)strCmd, NULL, SW_SHOW);
		
		//	�򿪵�����־���鿴����
		strCmd.Format("%s%s\\ElementEditor.log", g_szWorkDir, g_Log.GetLogDir());
		ShellExecute(NULL, _T("open"), (LPCTSTR)strCmd, NULL, NULL, SW_SHOW);
	}
	
	//	��յ�ǰѡ��
	OnUnselectAll();
}

void CDlgExportBHT::OnSelectAll() 
{
	m_pMapGridSelectPolicy->SelectAll(true);
	m_mapGrid.InvalidateRect(NULL);
}

void CDlgExportBHT::OnUnselectAll() 
{
	m_pMapGridSelectPolicy->SelectAll(false);
	m_mapGrid.InvalidateRect(NULL);
}
