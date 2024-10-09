// ElementResMan.cpp: implementation of the ElementResMan class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "ec_md5hash.h"
#include "Render.h"
#include "VssOperation.h"
#include "Shlwapi.h"
#include "ElementResMan.h"
#include "SceneObjectManager.h"

#include "A3d.h"
#include "EL_Building.h"
#include "EL_Forest.h"
#include "EL_Tree.h"

//#define new A_DEBUG_NEW

char folder_name[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9'};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ElementResMan::ElementResMan()
{
	for(int i = 0; i < RES_TYPE_COUNT; i++)
	mIdCount[i] = 0;
}

ElementResMan::~ElementResMan()
{
	ReleaseResTable();
}

bool ElementResMan::AddRes( const int iType,const char *szName, char *szNewName)
{
	if(szName==NULL) return false;
	if(iType >= RES_TYPE_COUNT || iType < 0) return false;
	
	bool bResult = false;
	
	switch( iType )
	{
	case RES_TYPE_BUILDING:
		 bResult = ProcessBuildingRes(szName,szNewName);
	case RES_TYPE_GRASS:
		 //bResult = ProcessGrassRes(szName);
		 break;
	case RES_TYPE_TREE:
		 //bResult = ProcessTreeRes(szName,szNewName);
		 break;
	case RES_TYPE_GFX:
	     //bResult = ProcessGfxRes(szName);
		 break;
	}
	return bResult;
}

bool ElementResMan::ProcessBuildingRes(const char *szName, char *szNewName)
{
	Res_Item* pNewItem = new Res_Item;
	if(pNewItem==NULL) 
	{
		g_Log.Log("ElementResMan::ProcessBuildingRes(),没有足够的内存!");
		return false;
	}

	//检验文件是否已经加入
	GenerateMd5Code(pNewItem->szMd5Name, szName);
	g_Log.Log("%s---%s",pNewItem->szMd5Name,szName);
	Res_Item* pOldItem = ResIsExistByMd5(RES_TYPE_BUILDING,pNewItem->szMd5Name);
	if(pOldItem)
	{
		AfxMessageBox("内容相同资源已经存在,加入失败!");
		strcpy(szNewName,pOldItem->szID);
		delete pNewItem;
		return false;
	}

	CELBuilding mModel;
	CString strPathName = szName;
	strPathName.Replace(".mox",".chf");
	if(!mModel.LoadFromMOXAndCHF(g_Render.GetA3DDevice(),szName,strPathName))
	{
		CString msg;
		msg.Format("不能加载文件%s",msg);
		AfxMessageBox(msg);
		delete pNewItem;
		return false;
	}
	
	CString ext,temp;
	temp = szName;
	int pos = temp.Find(".");
	temp = temp.Left(pos);
	ext = szName;
	ext.Replace(temp,NULL);
	int folder_index = ResTable[RES_TYPE_BUILDING].GetCount()/300;

	sprintf(pNewItem->szID,"Building\\Models\\%d\\%x%s",folder_index,++mIdCount[RES_TYPE_BUILDING],ext);
	strcpy(pNewItem->szOrgName,TransOrgName(szName));

	if(ResIsExistByOrgName(RES_TYPE_BUILDING,pNewItem->szOrgName))
	{
		CString msg;
		msg.Format("原始资源的名字已经存在于资源管理器中(%s)",pNewItem->szOrgName);
		AfxMessageBox(msg);
		mModel.Release();
		delete pNewItem;
		return false;
	}

	//更改模型纹理路径，并把纹理文件加入到资源管理器中 
	int nMesh = mModel.GetA3DLitModel()->GetNumMeshes();
	for( int i = 0; i < nMesh; i++)
	{
		A3DLitMesh *pMesh = mModel.GetA3DLitModel()->GetMesh(i);
		if(pMesh)
		{
			char *szTextures = ProcessBuildingTextureRes(pMesh->GetTextureMap(),pNewItem->szID);
			if(szTextures==NULL)
			{
				g_Log.Log("ElementResMan::ProcessBuildingRes(),纹理 %s 加入失败!", pMesh->GetTextureMap());
				mModel.Release();
				delete pNewItem;
				return false;
			}
			pMesh->ChangeTextureMap(szTextures);
		}
	}

	//拷贝模型到building\models下
	CString path,name;
	name = pNewItem->szID;
	GetPathAndName(name,path,name);
	CreateFolder(path);
	AFile sFile;
	if(!sFile.Open(pNewItem->szID,AFILE_CREATENEW))
	{
		g_Log.Log("ElementResMan::ProcessBuildingRes(),创建新文件%s失败!",pNewItem->szID);
		mModel.Release();
		delete pNewItem;
		return false;
	}
	if(!mModel.Save(&sFile))
	{
		g_Log.Log("ElementResMan::ProcessBuildingRes(),保存文件%s失败!",pNewItem->szID);
		mModel.Release();
		delete pNewItem;
		return false;
	}
	sFile.Close();
	mModel.Release();
	
	CString dstPath,dstName;
	GetPathAndName(path,dstPath,dstName);
	g_VSS.SetProjectPath(dstPath);
	g_VSS.CreateProject(dstName);
	g_VSS.SetProjectPath(path);
	g_VSS.AddFile(pNewItem->szID);

	//文件写入资源表
	strcpy(szNewName,pNewItem->szID);
	ResTable[RES_TYPE_BUILDING].AddTail(pNewItem);
	return true;
}

bool ElementResMan::ProcessGfxRes( const char *szName)
{
	return true;
}

char* ElementResMan::ProcessBuildingTextureRes( const char *szName, const char *szReferenceName)
{
	Res_Item* pNewItem = new Res_Item;
	if(pNewItem==NULL) 
	{
		g_Log.Log("ElementResMan::ProcessTextureRes(),没有足够的内存!");
		return NULL;
	}

	//检验文件是否已经加入
	if(GenerateMd5Code(pNewItem->szMd5Name, szName)==NULL)
	{
		delete pNewItem;
		return NULL;
	}

	Res_Item* pOldItem = NULL;
	pOldItem = ResIsExistByMd5(RES_TYPE_BUILDING_TEXTURE,pNewItem->szMd5Name);
	CString sdr = szName;
	sdr.MakeLower();
	if(pOldItem && sdr.Find(".sdr")==-1)
	{	
		delete pNewItem;
		char *szNewRef = new char[LENGTH_ORG_NAME];
		strcpy(szNewRef,szReferenceName);
		bool bSkip = false;
		ALISTPOSITION pos = pOldItem->listReference.GetHeadPosition();
		while(pos)
		{
			char *szOldRef = pOldItem->listReference.GetNext(pos);
			if(0==stricmp(szOldRef,szNewRef))
			{
				delete szNewRef;
				szNewRef = NULL;
				bSkip = true;
				break;
			}
		}
		if( !bSkip ) pOldItem->listReference.AddTail(szNewRef);
		return pOldItem->szID;
	}
	
	CString ext,temp;
	temp = szName;
	int pos = temp.Find(".");
	if(pos==-1)
	{
		delete pNewItem;
		g_Log.Log("不能找出文件的扩展名!%s",szName);
		return NULL;
	}
	temp = temp.Left(pos);
	ext = szName;
	ext.Replace(temp,NULL);
	ext.MakeLower();
	int folder_index = ResTable[RES_TYPE_BUILDING_TEXTURE].GetCount()/300;
	if(ext.Find("sdr")==-1)
		sprintf(pNewItem->szID,"Building\\textures\\%d\\%x%s",folder_index,++mIdCount[RES_TYPE_BUILDING_TEXTURE],ext);
	else 
	{
		CString sPath,sName;
		GetPathAndName(szName,sPath,sName);
		sprintf(pNewItem->szID,"Building\\textures\\%d\\%s",folder_index,sName);
	}
	strcpy(pNewItem->szOrgName,TransOrgName(szName));
	char *szNewRef = new char[LENGTH_ORG_NAME];
	strcpy(szNewRef,szReferenceName);
	pNewItem->listReference.AddTail(szNewRef);
	
	//下面拷贝文件到Building\textures
	if(!CopyResFile(pNewItem->szID,szName))
	{
		delete pNewItem;
		--mIdCount[RES_TYPE_BUILDING_TEXTURE];
		return NULL; 
	}
	
	//文件写入资源表
	ResTable[RES_TYPE_BUILDING_TEXTURE].AddTail(pNewItem);
	return pNewItem->szID;
}

bool ElementResMan::CopyResFile( const char *szDst, const char *szSrc)
{
	CFile sFile,dFile;
	bool  bFileIsExist = false;
	bool  bFileReadOnly = false;
	CString path,name;
	
	bFileIsExist = AUX_FileIsExist(szDst);
	if(bFileIsExist)
		bFileReadOnly = AUX_FileIsReadOnly(szDst);
	
	GetPathAndName(szDst,path,name);
	CreateFolder(path);
	if(bFileReadOnly)
	{
		g_VSS.SetProjectPath(path);
		g_VSS.CheckOutFile(szDst);
	}
	
	if(!sFile.Open(szSrc,CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite))
	{
		g_Log.Log("ElementResMan::CopyResFile(),打开文件%s失败!",szSrc);
		return false;
	}
	if(!dFile.Open(szDst,CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		sFile.Close();
		g_Log.Log("ElementResMan::CopyResFile(),创建文件%s失败!",szDst);
		return false;
	}
	DWORD dwFileLen = sFile.GetLength();
	DWORD dwLoaded = 0;
	BYTE* pBuffer = new BYTE[dwFileLen];
	ASSERT(pBuffer);
	if(0==sFile.Read(pBuffer,dwFileLen))
		goto failed;
	dFile.Write(pBuffer,dwFileLen);
	
	sFile.Close();
	dFile.Close();
	delete pBuffer;

	if(!bFileIsExist)
	{
		CString dstPath,dstName;
		GetPathAndName(path,dstPath,dstName);
		g_VSS.SetProjectPath(dstPath);//Building\textures
		g_VSS.CreateProject(dstName);//e
		g_VSS.SetProjectPath(path);//Building\textures\e
		g_VSS.AddFile(szDst);
	}else
	{
		g_VSS.SetProjectPath(path);
		g_VSS.CheckInFile(name);
	}
	return true;

failed:
	sFile.Close();
	dFile.Close();
	delete pBuffer;
	if(bFileIsExist)
	{
		g_VSS.SetProjectPath(path);
		g_VSS.UndoCheckOut(name);
	}
	return false;
}


bool ElementResMan::CopyResFileEx( const char *szDst, const char *szSrc)
{
	CFile sFile,dFile;
	bool  bFileIsExist = false;
	bool  bFileReadOnly = false;
	CString path,name;
	
	bFileIsExist = AUX_FileIsExist(szDst);
	if(bFileIsExist)
		bFileReadOnly = AUX_FileIsReadOnly(szDst);
	
	GetPathAndName(szDst,path,name);
	CreateFolder(path);
	if(bFileReadOnly)
	{
		//g_VSS.SetProjectPath(path);
		//g_VSS.CheckOutFile(szDst);
	}
	
	if(!sFile.Open(szSrc,CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite))
	{
		g_Log.Log("ElementResMan::CopyResFile(),打开文件%s失败!",szSrc);
		return false;
	}
	if(!dFile.Open(szDst,CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		sFile.Close();
		g_Log.Log("ElementResMan::CopyResFile(),创建文件%s失败!",szDst);
		return false;
	}
	DWORD dwFileLen = sFile.GetLength();
	DWORD dwLoaded = 0;
	BYTE* pBuffer = new BYTE[dwFileLen];
	ASSERT(pBuffer);
	if(0==sFile.Read(pBuffer,dwFileLen))
		goto failed;
	dFile.Write(pBuffer,dwFileLen);
	
	sFile.Close();
	dFile.Close();
	delete pBuffer;
	/*
	if(!bFileIsExist)
	{
		CString dstPath,dstName;
		GetPathAndName(path,dstPath,dstName);
		g_VSS.SetProjectPath(dstPath);//Building\textures
		g_VSS.CreateProject(dstName);//e
		g_VSS.SetProjectPath(path);//Building\textures\e
		g_VSS.AddFile(szDst);
	}else
	{
		g_VSS.SetProjectPath(path);
		g_VSS.CheckInFile(name);
	}*/
	return true;

failed:
	sFile.Close();
	dFile.Close();
	delete pBuffer;
	if(bFileIsExist)
	{
		//g_VSS.SetProjectPath(path);
		//g_VSS.UndoCheckOut(name);
	}
	return false;
}

void ElementResMan::GetPathAndName(const CString strPathName, CString& strPath, CString& strName)
{
	AString temp = strPathName;
	
	int pos = temp.ReverseFind('\\');
	if(pos == -1)
	{
		pos = 0;
	}
	strPath = temp.Mid(0, pos);
	strName = temp.Mid(pos+1,temp.GetLength());
}

bool ElementResMan::ReplaceRes( const int iType, const char *szName, const char *szNewRes)
{
	bool bResult = false;
	switch(iType)
	{
	case RES_TYPE_BUILDING:
		bResult = ReplaceBuildingRes(szName,szNewRes);
		break;
	case RES_TYPE_BUILDING_TEXTURE:
		bResult = ReplaceTextureRes(iType,szName,szNewRes);
		break;
	}
	return bResult;
}

bool ElementResMan::ReplaceBuildingRes(const char *szName, const char *szNewRes)
{
	ALISTPOSITION pos = ResTable[RES_TYPE_BUILDING].GetHeadPosition();
	while(pos)
	{
		Res_Item *pItem = ResTable[RES_TYPE_BUILDING].GetNext(pos);
		if(strcmp(pItem->szID,szName)==0)
		{
			char szMd5[LENGTH_MD5_CODE];
			GenerateMd5Code(szMd5, szNewRes);
			Res_Item* pOldItem = NULL;
			pOldItem = ResIsExistByMd5(RES_TYPE_BUILDING,szMd5,pItem);
			if(pOldItem != NULL)
			{
				AfxMessageBox("ElementResMan::ReplaceBuildingRes(), 不能替换原资源! 因为替换文件已经存在于资源管理器中.");
				return false;
			}
			pOldItem = pItem;
			//去掉旧的纹理的引用，添加新的纹理
			CELBuilding mNewModel,mOldModel;
			CString strHullName = szNewRes;
			strHullName.Replace(".mox",".chf");
			if(!mNewModel.LoadFromMOXAndCHF(g_Render.GetA3DDevice(),szNewRes,strHullName))
			{
				AfxMessageBox("ElementResMan::ReplaceBuildingRes(),替换失败! 不能加载新文件.");
				return false;
			}
			AFile mOldFile;
			if(!mOldFile.Open(pOldItem->szID,AFILE_OPENEXIST))
			{
				AfxMessageBox("ElementResMan::ReplaceBuildingRes(),替换失败! 不能打开旧文件.");
				return false;
			}
			if(!mOldModel.Load(g_Render.GetA3DDevice(),&mOldFile))
			{
				AfxMessageBox("ElementResMan::ReplaceBuildingRes(),替换失败! 不能加载旧文件.");
				return false;
			}
			mOldFile.Close();

			//释放旧模型纹理引用
			int nMesh = mOldModel.GetA3DLitModel()->GetNumMeshes();
			for( int i = 0; i < nMesh; i++)
			{
				A3DLitMesh *pMesh = mOldModel.GetA3DLitModel()->GetMesh(i);
				if(pMesh)
				{
					DeleteReference(RES_TYPE_BUILDING_TEXTURE,pMesh->GetTextureMap(),pOldItem->szID);
				}
			}
			
			//添加新模型纹理或是增加引用
			nMesh = mNewModel.GetA3DLitModel()->GetNumMeshes();
			for(i = 0; i < nMesh; i++)
			{
				A3DLitMesh *pMesh = mNewModel.GetA3DLitModel()->GetMesh(i);
				if(pMesh)
				{
					AString txPath = pMesh->GetTextureMap();
					if(-1==txPath.Find("."))
					{
						txPath.Format("正替换的模型第(%d)号MESH未指定纹理!",i);
						mNewModel.Release();
						mOldModel.Release();
						AfxMessageBox(txPath);
						return false;
					}
					pMesh->ChangeTextureMap(ProcessBuildingTextureRes(pMesh->GetTextureMap(),pOldItem->szID));
				}
			}
			CString name,path;
			name = pOldItem->szID;
			GetPathAndName(name,path,name);
			g_VSS.SetProjectPath(path);
			g_VSS.CheckOutFile(pOldItem->szID);
			//替换旧的模型文件
			AFile sFile;
			if(!sFile.Open(pOldItem->szID,AFILE_CREATENEW))
			{
				g_VSS.CheckInFile(name);
				g_Log.Log("ElementResMan::ReplaceBuildingRes(),创建新文件%s失败!", pOldItem->szID);
				mNewModel.Release();
				mOldModel.Release();
				return false;
			}
			if(!mNewModel.Save(&sFile))
			{
				sFile.Close();
				g_VSS.CheckInFile(name);
				g_Log.Log("ElementResMan::ReplaceBuildingRes(),保存文件%s失败!", pOldItem->szID);
				mNewModel.Release();
				mOldModel.Release();
				return false;
			}
			sFile.Close();
			g_VSS.CheckInFile(name);
			
			//更新资源表数据
			strcpy(pOldItem->szMd5Name,szMd5);
			strcpy(pOldItem->szOrgName,TransOrgName(szNewRes));
			mNewModel.Release();
			mOldModel.Release();
			return true;
		}
	}
	AfxMessageBox("ElementResMan::ReplaceBuildingRes(),替换失败! 被替换文件不存在资源表中.");
	return false;
}

Res_Item* ElementResMan::ResIsExistByPath( const int iType, const char *szPath)
{
	ALISTPOSITION pos = ResTable[iType].GetHeadPosition();
	while(pos)
	{
		Res_Item *pItem = ResTable[iType].GetNext(pos);
		if(stricmp(pItem->szID,szPath)==0)
			return pItem;
	}
	return NULL;
}

bool ElementResMan::ReplaceTreeRes(const char *szName, const char* szNewRes)
{
	/*
	CELTree tree;
	if( !tree.Init(g_Render.GetA3DDevice(), NULL, 0 , szNewRes, szCompositeMap, &g_Log) )
	{
		g_Log.Log("ElementResMan::ProcessTreeRes(), failed to init tree type [%s]", szName);
		return false;
	}
	//先把原资源Check out
	g_VSS.CheckOutFile(szName);
	//用新资源覆盖老的资源
	CopyResFile(szName,szNewRes);
	//把新的纹理放到服务器
	CString strMap = tree.GetCompositeMap();
	g_VSS.AddFile(strMap);
	strMap = tree.GetBranchMap();
	g_VSS.AddFile(strMap);
	//把资源Check in
	CString strPath,strName;
	GetPathAndName(CString(szName),strPath,strName);
	g_VSS.SetProjectPath(strPath);
	g_VSS.CheckInFile(strName);
	*/
	return true;
}

bool ElementResMan::ReplaceTextureRes(const int iType, const char *szName, const char *szNewRes)
{
	ALISTPOSITION pos = ResTable[iType].GetHeadPosition();
	while(pos)
	{
		Res_Item *pItem = ResTable[RES_TYPE_BUILDING].GetNext(pos);
		if(strcmp(pItem->szID,szName)==0)
		{
			char szMd5[LENGTH_MD5_CODE];
			GenerateMd5Code(szMd5, szNewRes);
			Res_Item* pOldItem = NULL;
			pOldItem = ResIsExistByMd5(iType,szMd5);
			if(pOldItem != NULL)
			{
				AfxMessageBox("ElementResMan::ReplaceTextureRes(), 不能替换原资源! 因为替换文件已经存在于资源管理器中.");
				return false;
			}
			pOldItem = pItem;
			if(!CopyResFile(pOldItem->szID,szNewRes))
			{
				AfxMessageBox("ElementResMan::ReplaceTextureRes(),替换失败! 不能拷贝新文件到指定位置.");
				return false; 
			}
			strcpy(pOldItem->szMd5Name,szMd5);
			strcpy(pOldItem->szOrgName,szNewRes);
			return true;
		}
	}
	AfxMessageBox("ElementResMan::ReplaceTextureRes(),替换失败! 被替换文件不存在资源表中.");
	return false;
}

bool ElementResMan::ProcessGrassRes( const char *szName)
{
	return true;
}

bool ElementResMan::ProcessTreeRes(const char *szName, const char *szCompositeMap)
{
	CELTree tree;
	if( !tree.Init(g_Render.GetA3DDevice(), NULL, 0 , szName, szCompositeMap, &g_Log) )
	{
		g_Log.Log("ElementResMan::ProcessTreeRes(), failed to init tree type [%s]", szName);
		return false;
	}
	
	//把spt文件以及其中的纹理都加入服务器
	CString strPathName = szName;
	CString strName,strPath;
	strPathName.MakeLower();
	int pos = strPathName.Find("trees");
	if(pos==-1) 
	{
		AfxMessageBox("原文件的路径中不包括<trees>文件夹!");
		tree.Release();
		return false;
	}
	strPath = strPathName.Left(pos);
	strPathName = szName;
	strPathName.Replace(strPath,NULL);
	g_VSS.SetProjectPath("");
	strPath = "";
	while(1)
	{
		pos = strPathName.Find("\\");
		if(pos==-1) 
		{
			strName = strPathName;
			break;
		}
		strName = strPathName.Left(pos + 1);
		strPathName.Replace(strName,NULL);
		strName.Replace("\\",NULL);
		strPath +=strName;
		g_VSS.CreateProject(strName);
		g_VSS.SetProjectPath(strPath);
		strPath +="\\";
	}
	g_VSS.AddFile(szName);
	//把纹理放到服务器上
	CString strMap = tree.GetCompositeMap();
	g_VSS.AddFile(strMap);
	strMap = tree.GetBranchMap();
	g_VSS.AddFile(strMap);
	tree.Release();
	
	//文件写入资源表
	Res_Item *pNewItem = new Res_Item;
	strcpy(pNewItem->szID,szName);
	strcpy(pNewItem->szOrgName,szName);
	ResTable[RES_TYPE_BUILDING].AddTail(pNewItem);
	tree.Release();
	return true;
}

char* ElementResMan::GenerateMd5Code(char*szBuffer, const char* szName)
{
	char szInput[LENGTH_BLOCK_FILE];
	
	INT64 i64FileSize = GetFileSize (szName); 
	FILE* pSrcFile = fopen(szName,"rb");
	if(pSrcFile == NULL) 
	{
		g_Log.Log("ElementResMan::GenerateMd5Code(), Cannot open file %s",szName);
		return NULL;
	}
	
	int iBlockCount = i64FileSize/LENGTH_BLOCK_FILE;
	int iExtern = i64FileSize%LENGTH_BLOCK_FILE;
	
	PATCH::MD5Hash md;
	
	for(int i = 0; i < iBlockCount; i++) 
	{
		fread(szInput,LENGTH_BLOCK_FILE,1,pSrcFile);
		md.Update(szInput,LENGTH_BLOCK_FILE);
	}
	if(iExtern!=0) 
	{
		fread(szInput,iExtern,1,pSrcFile);
		md.Update(szInput,iExtern);
	}
	
	md.Final();
	unsigned int nLenght = LENGTH_MD5_CODE; 
	md.GetString(szBuffer,nLenght);

	fclose(pSrcFile);
	return szBuffer;
}

Res_Item *ElementResMan::ResIsExistByMd5( const int iType, const char *szMd5Code, Res_Item* pTest)
{
	ALISTPOSITION pos = ResTable[iType].GetHeadPosition();
	while(pos)
	{
		Res_Item *pItem = ResTable[iType].GetNext(pos);
		if(strcmp(pItem->szMd5Name,szMd5Code)==0)
		{
			if(pTest) 
			{
				if(pTest!=pItem) return pItem;
			}else return pItem;
		}
	}
	return NULL;
}

Res_Item *ElementResMan::ResIsExistByName( const int iType, const char *szName)
{
	ALISTPOSITION pos = ResTable[iType].GetHeadPosition();
	while(pos)
	{
		Res_Item *pItem = ResTable[iType].GetNext(pos);
		if(strcmp(pItem->szID,szName)==0)
			return pItem;
	}
	return NULL;
}

bool ElementResMan::ResIsExistByOrgName( const int iType, const char *szOrgName)
{
	ALISTPOSITION pos = ResTable[iType].GetHeadPosition();
	while(pos)
	{
		Res_Item *pItem = ResTable[iType].GetNext(pos);
		CString orgName1 = pItem->szOrgName;
		CString orgName2 = szOrgName;
		int temp = orgName1.ReverseFind('\\');
		if(temp!=-1) orgName1 = orgName1.Right(orgName1.GetLength() - temp);
		temp = orgName2.ReverseFind('\\');
		if(temp!=-1) orgName2 = orgName2.Right(orgName2.GetLength() - temp);

		if(orgName1 == orgName2)
			return true;
	}
	return false;
}

int	ElementResMan::GetResCount( const int iType) const
{
	return (int)(ResTable[iType].GetCount());
}

Res_Item* ElementResMan::GetRes(const int iType, int iIndex)
{
	return ResTable[iType].GetByIndex(iIndex);
}


APtrList<char *>* ElementResMan::GetReferenceList( const int iType, const char* szName )
{
	ALISTPOSITION pos = ResTable[iType].GetHeadPosition();
	while(pos)
	{
		Res_Item *pItem = ResTable[iType].GetNext(pos);
		if(strcmp(pItem->szID,szName)==0)
			return &(pItem->listReference);
	}
	return NULL;
}

void ElementResMan::DeleteReference( const int iType, const char *szID, const char*szReferenceName )
{
	ALISTPOSITION pos1 = ResTable[iType].GetHeadPosition();
	ALISTPOSITION pos2;
	while(pos1)
	{
		Res_Item *pItem = ResTable[iType].GetNext(pos1);
		if(strcmp(pItem->szID,szID)==0)
		{
			pos2 = pItem->listReference.GetHeadPosition();
			while(pos2)
			{
				ALISTPOSITION oldPos = pos2;
				char *pRef = pItem->listReference.GetNext(pos2);
				if(strcmp(pRef,szReferenceName)==0)
				{
					delete []pRef;
					pItem->listReference.RemoveAt(oldPos);
					return;
				}
			}
		}
	}
}

void ElementResMan::DeleteRedundanceReference(const int iType, const char *szMap,CSceneObjectManager *pSMan)
{
	APtrList<Res_Item*> tempArray;
	ALISTPOSITION pos1 = ResTable[iType].GetHeadPosition();
	ALISTPOSITION pos2; 
	AString tempMap,strRef,objName;
	while(pos1)
	{
		Res_Item* pItem = ResTable[iType].GetNext(pos1);
		pos2 = pItem->listReference.GetHeadPosition();
		while(pos2)
		{
			tempMap = pItem->listReference.GetNext(pos2);
			strRef = tempMap;
			int pos = tempMap.Find("..");
			tempMap = tempMap.Left(pos);
			objName = strRef;
			pos = objName.GetLength() - pos - 2;
			objName = objName.Right(pos);
			if(strcmp(tempMap,szMap)==0)
			{
				if(!pSMan->IsBuildingObjectInScene(objName))
				{
					Res_Item *pNew = new Res_Item;
					strcpy(pNew->szID,pItem->szID);
					strcpy(pNew->szOrgName, strRef);
					tempArray.AddTail(pNew);
				}
			}
		}
	}

	pos2 = tempArray.GetHeadPosition();
	while(pos2)
	{
		Res_Item *pTemp = tempArray.GetNext(pos2);
		DeleteReference(iType,pTemp->szID,pTemp->szOrgName);
		delete pTemp; 
	}

	tempArray.RemoveAll();

}

void ElementResMan::AddReference( const int iType, const char * szID, const char *szReferenceName )
{
	ALISTPOSITION pos = ResTable[iType].GetHeadPosition();
	while(pos)
	{
		Res_Item* pItem = ResTable[iType].GetNext(pos);
		if(strcmp(pItem->szID,szID)==0)
		{
			
			ALISTPOSITION pos2 = pItem->listReference.GetHeadPosition();
			while(pos2)
			{
				char *ptemp = pItem->listReference.GetNext(pos2);
				if(strcmp(ptemp,szReferenceName)==0)
					return;
			}
			
			char *pNewName = new char[LENGTH_ORG_NAME];
			strcpy(pNewName,szReferenceName);
			pItem->listReference.AddTail(pNewName);
			return;
		}
	}
}

void ElementResMan::DeleteRes( const int iType, const char*szID)
{
	ALISTPOSITION pos = ResTable[iType].GetHeadPosition();
	
	while(pos)
	{
		ALISTPOSITION oldPos = pos;
		Res_Item* pItem = ResTable[iType].GetNext(pos);
		if(strcmp(pItem->szID,szID)==0)
		{
			
			if(pItem->listReference.GetCount()>0)
				return;
			
			if(iType == RES_TYPE_BUILDING)
			{
				
				//去掉旧的纹理的引用
				CELBuilding mModel;
				AFile mFile;
				if(!mFile.Open(pItem->szID,AFILE_OPENEXIST))
				{
					AfxMessageBox("ElementResMan::DeleteRes(),删除失败! 不能模型文件.");
					return;
				}
				if(!mModel.Load(g_Render.GetA3DDevice(),&mFile))
				{
					AfxMessageBox("ElementResMan::DeleteRes(),删除失败! 不能加模型文件.");
					return;
				}
				mFile.Close();
				
				//释放模型纹理引用
				int nMesh = mModel.GetA3DLitModel()->GetNumMeshes();
				for( int i = 0; i < nMesh; i++)
				{
					A3DLitMesh *pMesh = mModel.GetA3DLitModel()->GetMesh(i);
					if(pMesh)
					{
						DeleteReference(RES_TYPE_BUILDING_TEXTURE,pMesh->GetTextureMap(),szID);
						DeleteRes(RES_TYPE_BUILDING_TEXTURE,pMesh->GetTextureMap());
					}
				}
			}
			
			//依次在本地和服务器删除模型文件
			CString path,name,org;
			org = pItem->szID;
			GetPathAndName(org,path,name);
			g_VSS.SetProjectPath(path);
			g_VSS.CheckOutFile(pItem->szID);
			
			if(!DeleteFile(pItem->szID))
				AfxMessageBox("ElementResMan::DeleteRes(),删除本地文件失败!");
			g_VSS.CheckInFile(name);
			g_VSS.DeleteFile(name);
			
			delete pItem;
			ResTable[iType].RemoveAt(oldPos);
			return;
		}
	}
}

bool ElementResMan::WriteTable()
{
	AFile sFile;
	if(!sFile.Open("ResTable\\ElementRes.table",AFILE_CREATENEW))
	{
		g_Log.Log("ElementResMan::Save(),不能打开文件ElementRes.table.");
		return false;
	}
	DWORD dwVersion = RES_FILE_VERSION;
	DWORD dwWrited;
	sFile.Write(&dwVersion,sizeof(DWORD),&dwWrited);
	sFile.Write(mIdCount,sizeof(DWORD)*RES_TYPE_COUNT,&dwWrited);
	DWORD dwResType[RES_TYPE_COUNT] = { RES_TYPE_BUILDING, RES_TYPE_GRASS,RES_TYPE_TREE, RES_TYPE_GFX, RES_TYPE_BUILDING_TEXTURE };
	for(int i = 0; i < RES_TYPE_COUNT; i++)
	{
		DWORD dwResCount = ResTable[dwResType[i]].GetCount();
		sFile.Write(dwResType+i,sizeof(DWORD),&dwWrited);
		sFile.Write(&dwResCount,sizeof(DWORD),&dwWrited);	
		sFile.Write(&iFolderIndex[i],sizeof(int),&dwWrited);
		ALISTPOSITION pos = ResTable[dwResType[i]].GetTailPosition();
		ALISTPOSITION pos2;
		while(pos)
		{
			Res_Item* pItem = ResTable[dwResType[i]].GetPrev(pos);
			sFile.Write(pItem->szID,LENGTH_ID,&dwWrited);
			sFile.Write(pItem->szMd5Name,LENGTH_MD5_CODE,&dwWrited);
			sFile.Write(pItem->szOrgName,LENGTH_ORG_NAME,&dwWrited);
			
			DWORD dwRefCount = pItem->listReference.GetCount();
			sFile.Write(&dwRefCount,sizeof(DWORD),&dwWrited);	
			pos2 = pItem->listReference.GetTailPosition();
			while(pos2)
			{
				char *ptemp = pItem->listReference.GetPrev(pos2);
				sFile.Write(ptemp,LENGTH_ORG_NAME,&dwWrited);
			}
		}
	}
	sFile.Close();
	return true;
}

bool ElementResMan::ReadTable(const char *szPath)
{
	ReleaseResTable();
	
	AFile sFile;
	if(!sFile.Open(szPath,AFILE_OPENEXIST))
	{
		g_Log.Log("ElementResMan::Load(),不能打开文件 %s",szPath);
		return false;
	}
	DWORD dwVersion;
	DWORD dwReaded;
	sFile.Read(&dwVersion,sizeof(DWORD),&dwReaded);
	if(dwVersion>0x1)
	{
		g_Log.Log("ElementResMan::Load(),不认识的文件版本!");
		return false;
	}
	sFile.Read(mIdCount,sizeof(DWORD)*RES_TYPE_COUNT,&dwReaded);
	DWORD dwResType,dwResCount, dwRefCount;
	for( int n = 0; n < RES_TYPE_COUNT; n++)
	{
		sFile.Read(&dwResType,sizeof(DWORD),&dwReaded);
		sFile.Read(&dwResCount,sizeof(DWORD),&dwReaded);	
		sFile.Read(&iFolderIndex[n],sizeof(int),&dwReaded);
		for( int i = 0; i < dwResCount; i++)
		{
			Res_Item* pItem = new Res_Item;
			ASSERT(pItem);
			sFile.Read(pItem->szID,LENGTH_ID,&dwReaded);
			sFile.Read(pItem->szMd5Name,LENGTH_MD5_CODE,&dwReaded);
			sFile.Read(pItem->szOrgName,LENGTH_ORG_NAME,&dwReaded);
			sFile.Read(&dwRefCount,sizeof(DWORD),&dwReaded);
			for(int j = 0; j < dwRefCount; j++)
			{
				char *pNewRef = new char[LENGTH_ORG_NAME];
				ASSERT(pNewRef);
				sFile.Read(pNewRef,LENGTH_ORG_NAME,&dwReaded);
				
				if(dwVersion<0x1)
				{//该处是改版后为了节约资源表占用的空间，而做的减肥操作，以后的版本不会有这个问题
					bool bExist = false;
					ALISTPOSITION pos = pItem->listReference.GetHeadPosition();
					while(pos)
					{
						char *szRef = pItem->listReference.GetNext(pos);
						if(0==strcmp(szRef,pNewRef))
						{
							bExist = true;
							break;
						}
					}
					if(!bExist) pItem->listReference.AddHead(pNewRef);
					else delete pNewRef;

				}else pItem->listReference.AddHead(pNewRef);
			}
			ResTable[dwResType].AddHead(pItem);
		}
	}
	sFile.Close();
	return true;
}


bool ElementResMan::Load(const char *szPath)
{
    return ReadTable(szPath);		
}

bool ElementResMan::Load()
{
	return ReadTable("ResTable\\ElementRes.table");	
}

bool ElementResMan::Save()
{
	return WriteTable();
}

INT64 ElementResMan::GetFileSize(const char *szName) const
{
	CFileException ex;
	CFile sourceFile;
	CString pathName = szName;
	if (!sourceFile.Open(pathName,
		CFile::modeRead, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		CString error;
		error.Format(_T("Couldn't open source file: %s,%s"),pathName,szError);
		g_Log.Log(error);
		return 0;
	}
    INT64 size = sourceFile.GetLength();
	sourceFile.Close();
	return size;
}

void ElementResMan::ReleaseResTable()
{
	for( int i = 0; i < RES_TYPE_COUNT; i++)
	{
		
		ALISTPOSITION pos = ResTable[i].GetHeadPosition();
		while(pos)
		{
			Res_Item* pItem = ResTable[i].GetNext(pos);
			ALISTPOSITION pos2 = pItem->listReference.GetHeadPosition();
			while(pos2)
			{
				char * ptemp = pItem->listReference.GetNext(pos2);
				delete []ptemp;
			}
			pItem->listReference.RemoveAll();
			delete pItem;
		}
		ResTable[i].RemoveAll();
	}
}

bool ElementResMan::RefIsExist( const int iType, const char *szName, const char *szSceneObjectName)
{
	int n = GetResCount(iType);
	Res_Item *pItem = NULL;
	for( int i = 0; i < n; i++)
	{
		pItem = GetRes(iType,i);
		if(strcmp(pItem->szID,szName)==0)
		{
			ALISTPOSITION pos = pItem->listReference.GetHeadPosition();
			while(pos)
			{
				char *ptemp = pItem->listReference.GetNext(pos);
				if(strcmp(ptemp,szSceneObjectName)==0)
					return true;
			}
		}
	}
	return false;
}

CString ElementResMan::TransOrgName(const char *szOrg)
{
	CString org = szOrg;
	org.MakeLower();
	int pos = org.Find("auto_sorts\\");
	if(pos==-1) 
	{
		org.Replace("models\\static\\",NULL);
		return org;
	}
	CString temp = org.Left(pos);
	temp = temp + "auto_sorts\\";
	org.Replace(temp,NULL);
	return org;
}

CString ElementResMan::GetParentPath(CString  path)
{
	if(path[path.GetLength() - 1] == '\\')
	{
		path = path.Mid(0, path.GetLength() - 1);
	}

	CString parent;
	int pos = path.ReverseFind('\\');
	if(pos == -1)
	{
		pos = 0;
	}

	parent = path.Mid(0, pos);
	return parent;
}

BOOL ElementResMan::CreateFolder(CString path)
{
	if(PathFileExists(path))
		return TRUE;
	
	if(!CreateDirectory(path, NULL))
	{
		if(path.GetLength() == 2)
		{
			return FALSE;
		}

		if(!CreateFolder(GetParentPath(path)))
		{
			return FALSE;
		}

		if(!CreateDirectory(path, NULL))
		{
			return FALSE;
		}
	}
	return TRUE;
}

void ElementResMan::FastAddBuildingRes(const char* szOrg, const char* szName)
{
	Res_Item* pNew = new Res_Item;
	strcpy(pNew->szOrgName,szOrg);
	strcpy(pNew->szID,szName);
	memset(pNew->szMd5Name,0,LENGTH_MD5_CODE);
	ResTable[RES_TYPE_BUILDING].AddTail(pNew);
}

