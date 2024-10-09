// ModelContainer.cpp: implementation of the CModelContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "Global.h"
#include "elementeditor.h"
#include "ModelContainer.h"
#include "Render.h"
#include "a3d.h"
#include "AFI.h"
#include "EL_Building.h"
#include "MainFrm.h"


//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModelContainer::CModelContainer()
{
}

CModelContainer::~CModelContainer()
{
	ReleaseAllModel();
}

//这儿是把工作目录下的Models下的所有模型文件分类加载到列表中
bool CModelContainer::LoadAllModel()
{
	LoadStaticModel();
	AUX_GetMainFrame()->GetToolTabWnd()->UpdateModelList();
	return true;
}

void CModelContainer::ReleaseAllModel()
{
	
	ALISTPOSITION pos = m_listStaticModel.GetTailPosition();
	while( pos )
	{
		PESTATICMODEL ptemp = m_listStaticModel.GetPrev(pos);
		delete ptemp;
	}
	m_listStaticModel.RemoveAll();
}

bool CModelContainer::LoadStaticModel()
{
	CFileFind finder;
	CString msg;
	CString strWildcard;
	strWildcard = g_szWorkDir;
	strWildcard += "Models\\";
	strWildcard += "Static\\*.*";
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		
		//是"."或是".." 跳过.
		if (finder.IsDots())
			continue;
		
		// 是文件夹，继续搜寻.
		if (finder.IsDirectory())
		{
			ReadStaticModel(finder.GetFilePath());
			continue;
		}
	}
	finder.Close();
	return true;
}

bool CModelContainer::ReadStaticModel(CString path)
{
	/*
	CFileFind finder;
	CString msg;
	CString strWildcard;
	strWildcard = path + _T("\\*.mox");
	
	BOOL bWorking = finder.FindFile(strWildcard);
	int n = 0;
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		
		//是"."或是".." 跳过.
		if (finder.IsDots())
			continue;
		
		// 是文件夹，继续搜寻.
		if (finder.IsDirectory())
		{
			continue;
		}
		// 是文件.
		//PA3DModel pModel;
		AString temp;
		msg = finder.GetFilePath();
		af_GetRelativePath(msg,"models",temp);
		PESTATICMODEL pNewModel = new ESTATICMODEL();
		pNewModel->m_strPathName = temp; 
		pNewModel->pModel = NULL;
		m_listStaticModel.AddTail(pNewModel);
		//if (!g_Render.GetA3DEngine()->GetA3DModelMan()->LoadModelFile(temp, &pModel))
		//{
		//	g_Log.Log("Failed to load model %s !", msg);
		//	return false;
		//}
	}
	finder.Close();*/
	return true;
}

void CModelContainer::AddElement(const AString& strPath)
{
	/*
	AString temp;
	af_GetRelativePath(strPath,"Models",temp);
	PESTATICMODEL pNewModel = new ESTATICMODEL();
	ASSERT(pNewModel);
	pNewModel->m_strPathName = "Models\\" + temp; 
	pNewModel->pModel = NULL;
	m_listStaticModel.AddTail(pNewModel);*/
}

PESTATICMODEL CModelContainer::GetStaticModel(const AString& strName)
{
	//Old version
	/*
	ALISTPOSITION pos = m_listStaticModel.GetTailPosition();
	
	while( pos )
	{
		PESTATICMODEL ptemp = m_listStaticModel.GetPrev(pos);
		if(stricmp(strName,ptemp->m_strPathName)==0)
		{
			ESTATICMODEL* pNewModel = new ESTATICMODEL;
			pNewModel->pModel = new CELBuilding();
			ASSERT(pNewModel);  
			ASSERT(pNewModel->pModel);
			
			pNewModel->m_strPathName = ptemp->m_strPathName;
			AString strHull = pNewModel->m_strPathName;
			strHull.CutRight(4);
			strHull = strHull + ".chf";
			pNewModel->m_strHullPathName = strHull;
			if(!pNewModel->pModel->LoadFromMOXAndCHF(g_Render.GetA3DDevice(),pNewModel->m_strPathName,pNewModel->m_strHullPathName))
			{
				CString msg;
				msg.Format("CModelContainer::GetStaticModel(), Can't load mox(%s) file or hull(%s) file.",pNewModel->m_strPathName,pNewModel->m_strHullPathName);
				g_Log.Log(msg);
				g_LogDlg.Log(msg);
				delete pNewModel;
				pNewModel = NULL;
			}
			return pNewModel;
		}
	}
	return NULL;
	*/
	
	ESTATICMODEL* pNewModel = new ESTATICMODEL;
	pNewModel->pModel = new CELBuilding();
	ASSERT(pNewModel);  
	ASSERT(pNewModel->pModel);
	
	//pNewModel->m_strPathName = strName;
	pNewModel->id = 0;
	AFile sFile;
	if(!sFile.Open(strName,AFILE_OPENEXIST)) 
		goto failed;
	if(!pNewModel->pModel->Load(g_Render.GetA3DDevice(),&sFile))
		goto failed;
	sFile.Close();
	return pNewModel;
	
failed:
	CString msg;
	msg.Format("CModelContainer::GetStaticModel(), Can't load %s file",strName);
	delete pNewModel->pModel;
	delete pNewModel;
	g_Log.Log(msg);
	return NULL;
}

PESTATICMODEL CModelContainer::GetStaticModel(int index)
{ 
	PESTATICMODEL pEModel = m_listStaticModel.GetByIndex(index); 
	return pEModel;
}


CModelContainer g_ModelContainer;
