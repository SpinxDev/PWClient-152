
#include "el_randommapinfo.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Random map info 
//	
///////////////////////////////////////////////////////////////////////////

#ifdef _ELEMENT_EDITOR_

#include "Global.h"
#include "VssOperation.h"
#include "ElementMap.h"
#include "Terrain.h"
#include <AScriptFile.h>

extern const int g_randMapVersion;

bool CRandMapProp::SaveSev(const char* szPath)
{
	if(!szPath || strlen(szPath)<1) return false;

	AString str;
	str.Format("%s\\map_desp.sev",szPath);
	
	FILE* pf = fopen(str,"wb");
	if(pf==NULL) return false;
	

	fwrite(&g_randMapVersion,sizeof(int),1,pf);

	fwrite(&m_header,1,sizeof(m_header),pf);

	for (int i=0;i<m_header.iTileCount;i++)
	{
		MAP_INFO info = m_GridProp[i];
		fwrite(&info.index,sizeof(int),1,pf);
		fwrite(&info.type,sizeof(int),1,pf);
		fwrite(&info.connection,sizeof(int),1,pf);
	}
	
	fclose(pf);
	return true;
}
bool CRandMapProp::SaveClt(const char* szPath)
{
	if(!szPath || strlen(szPath)<1) return false;
	
	AString str;
	str.Format("%s\\map_desp.clt",szPath);
	
	FILE* pf = fopen(str,"wb");
	if(pf==NULL) return false;

	//	Write Unicode header
	wchar_t wcFlag = 0xfeff;
	fwprintf(pf, L"%c", wcFlag);
	fwprintf(pf, L"//  Element random map file (client version)\n\n");
	
	//	Write version
	fwprintf(pf, L"version %d\n", g_randMapVersion);

	fwprintf(pf,L"%d\n",m_GridProp.size());
	for (int i=0;i<m_GridProp.size();i++)
	{
		MAP_INFO info = m_GridProp[i];

		AWString strName = AS2WC(info.name);
		if (!strName.GetLength())
			strName = L"Unknown Area";
		fwprintf(pf,L"%d %s\n", info.index,strName);
	}
	
	fclose(pf);
	return true;
}
bool CRandMapProp::Save()
{
	CString str;
	str.Format("%s\\randommap.dat",g_szEditMapDir);

	if(AUX_FileIsReadOnly(str)) return false;

//	if(AUX_CheckOutRandomMapInfoFile())
	{
		AString str;
		str.Format("%s\\randommap.dat",g_szEditMapDir);
		
		FILE* pf = fopen(str,"w");
		if(pf==NULL) return false;
		fprintf(pf,"version: %d\n",g_randMapVersion);
		
		str.Format("%d %d %d %d %d %d %d %d\n",m_header.iTileCount,m_header.iTileSize,m_header.mainLineMaxLen,m_header.mainLineMinLen,m_header.branchLineNumMax,m_header.branchLineNumMin,
			m_header.branchLineMaxLen,m_header.branchLineMinLen);
	
		fprintf(pf,str.GetBuffer(str.GetLength()));

		str.Format("%f,%f,%f\n",m_header.fPosX,m_header.fPosY,m_header.fPosZ);

		fprintf(pf,str.GetBuffer(str.GetLength()));

		for(int i = 0; i < m_GridProp.size(); i++)
		{
			char szLine[128];
			MAP_INFO info = m_GridProp[i];
			sprintf(szLine,"%d %d %d %s\n",info.index+1,info.type,info.connection,info.name);
			fprintf(pf,szLine);
		}
		
		fclose(pf);
	}
	
	return true;
}
bool CRandMapProp::InitGird(int c)
{ 	
	CString str;
	str.Format("%s%s",g_szWorkDir,g_szEditMapDir);
	g_VSS.SetWorkFolder(str);
	g_VSS.SetProjectPath(str);
	str.Format("%s\\randommap.dat",g_szEditMapDir);
	g_VSS.GetFile(str);
	
	AScriptFile sf;
	if(!sf.Open(str))
		return false;

	CElementMap trnMap;
	AString mapName;
	mapName.Format("%sEditMaps\\%s\\1\\1.elproj",g_szWorkDir,g_Configs.szCurProjectName);
	bool bHasTerrain = true;
	if(!trnMap.Load(mapName))
	{
		CString err;
		err.Format("打开%s失败",mapName);
		AfxMessageBox(err);
	//	return false;
		bHasTerrain = false;
	}

	int terrainSize = bHasTerrain ? (int)trnMap.GetTerrain()->GetTerrainSize() : 128;

	trnMap.Release();

	m_GridProp.clear();
	memset(&m_header,0,sizeof(m_header));

	int i,mapVer = 0;
	if(sf.MatchToken("version:",false))
	{
		mapVer = sf.GetNextTokenAsInt(false);

		if (mapVer>1)
		{
			m_header.iTileCount = sf.GetNextTokenAsInt(true);
			m_header.iTileSize = sf.GetNextTokenAsInt(false);
			m_header.mainLineMaxLen = sf.GetNextTokenAsInt(false);
			m_header.mainLineMinLen = sf.GetNextTokenAsInt(false);
			m_header.branchLineNumMax = sf.GetNextTokenAsInt(false);
			m_header.branchLineNumMin = sf.GetNextTokenAsInt(false);
			m_header.branchLineMaxLen = sf.GetNextTokenAsInt(false);
			m_header.branchLineMinLen = sf.GetNextTokenAsInt(false);
			
			if (mapVer>=3)
			{
				m_header.fPosX = sf.GetNextTokenAsFloat(true);
				m_header.fPosY = sf.GetNextTokenAsFloat(false);
				m_header.fPosZ = sf.GetNextTokenAsFloat(false);
			}
		}

		m_header.iTileCount = c;

		m_header.iTileSize = terrainSize;

		for (i=0;i<c;i++)
		{
			if(sf.IsEnd())
				break;
			
			int mapID,mapType,mapCon;

			mapID = sf.GetNextTokenAsInt(true);
			mapType = sf.GetNextTokenAsInt(false);
			mapCon = sf.GetNextTokenAsInt(false);
			sf.GetNextToken(false);

			if (mapID>=1)
			{
				MAP_INFO info;
				info.index = mapID - 1;
				info.type = mapType;
				info.connection = mapCon;
				info.name = sf.m_szToken;
				
				m_GridProp.push_back(info);
			}		
		}
	}
	sf.Close();

	return i>=c;	
}
bool CRandMapProp::OnCreateOneGrid(const char* szProjName)
{
	if(!AUX_CheckOutRandomMapInfoFile())
	{
		AfxMessageBox("CDlgNewMap::OnOK(),Check out randommap.dat failed!");
		return false;
	}
	char szProj[256];
	sprintf(szProj,"%s\\randommap.dat",g_szEditMapDir);
	FILE* fp = fopen(szProj, "a");
	if(!fp)
		return false;

	fprintf(fp,"%s 3 0 maze\n",szProjName); // 名字必须为数字 1,2,3,4,5。。。 保持以往惯例
	fclose(fp);

	if(!AUX_CheckInRandomMapInfoFile())
	{
		AfxMessageBox("CDlgNewMap::OnOK(),Check in randommap.dat failed!");
		return false;
	}

	return true;
}
bool CRandMapProp::CreateRandomMapFile(const char* szDst)
{
	FILE* mapFile = fopen(szDst,"w");
	if (mapFile==NULL)		
		return false;
	
	fprintf(mapFile,"version: %d\n",g_randMapVersion);

	AString str;
	str.Format("0 0 0 0 0 0 0 0\n");
	fprintf(mapFile,str.GetBuffer(str.GetLength()));

	fclose(mapFile);

	return true;
}

#elif _ELEMENTCLIENT
#include <AWScriptFile.h>
bool CRandMapProp::LoadClt(const char* szPath)
{
	if(!szPath) return false;

//	AString str;
//	str.Format("%s\\map_desp.clt",szPath);
	
	AWScriptFile cltFile;
	if (!cltFile.Open(szPath))
		return false;
	
	//	Get version
	if (!cltFile.MatchToken(L"version", false))
		return false;

	m_GridProp.clear();

	int iVersion = cltFile.GetNextTokenAsInt(false);
	
	int count = cltFile.GetNextTokenAsInt(false);

	while (cltFile.PeekNextToken(true))
	{
		MAP_INFO info;

		info.index = cltFile.GetNextTokenAsInt(true);

		cltFile.GetNextToken(false);
		
		info.name = cltFile.m_szToken;

		m_GridProp.push_back(info);
	}

	cltFile.Close();

	return m_GridProp.size() == count;
}

#else 

bool CRandMapProp::LoadSev(const char* szPath)
{
	if(!szPath) return false;

//	char filePath[128] = {0};
//	sprintf(filePath,"%s\\map_desp.sev",szPath);

	FILE* pf = fopen(szPath,"rb");
	if(!pf) return false;

	m_GridProp.clear();

	int ver=0;
	fread(&ver,sizeof(int),1,pf);

	if(ver>=3)	
		fread(&m_header,sizeof(FILEHEADER),1,pf);
	else 
	{
		FILEHEADER2 h;
		fread(&h,sizeof(h),1,pf);

		m_header.iTileCount = h.iTileCount;
		m_header.iTileSize = h.iTileSize;				// 每块大小(米)
		m_header.mainLineMaxLen = h.mainLineMaxLen;			// 主线最大长度
		m_header.mainLineMinLen = h.mainLineMinLen;			// 主线最小长度
		m_header.branchLineNumMax = h.branchLineNumMax;		// 支线数最大值
		m_header.branchLineNumMin = h.branchLineNumMin;		// 支线数最小值
		m_header.branchLineMaxLen = h.branchLineMaxLen;		// 支线长度最大值
		m_header.branchLineMinLen = h.branchLineMinLen;
		m_header.fPosX = 0.0f;
		m_header.fPosY = 0.0f;
		m_header.fPosZ = 0.0f;
	}
	
	for (int i=0;i<m_header.iTileCount;i++)
	{
		MAP_INFO info;
		fread(&info.index,sizeof(int),1,pf);
		fread(&info.type,sizeof(int),1,pf);
		fread(&info.connection,sizeof(int),1,pf);
		m_GridProp.push_back(info);
	}

	fclose(pf);
	
	return true;
}
#endif

bool CRandMapProp::GetGirdProp(int iGrid,MAP_INFO& info) 
{
	if(iGrid<0 || iGrid>=m_GridProp.size())
		return false;
	
	info = m_GridProp[iGrid];

	return true;
}
void CRandMapProp::SetGridProp(int iGrid, MAP_INFO& info)
{
	if(iGrid<0 || iGrid> m_GridProp.size())
		return;
	m_GridProp[iGrid] = info;
}
////////////////////////////////////////////////////////////
 
