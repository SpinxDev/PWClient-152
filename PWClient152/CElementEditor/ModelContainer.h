// ModelContainer.h: interface for the CModelContainer class.
//
//////////////////////////////////////////////////////////////////////
//----------------------------------------------------------
// Filename	: ModelContainer.h
// Creator	: QingFeng Xin
// Date		: 2004.8.28
// Desc		: 编辑器中要用到的模型事先加载到该容器中
//-----------------------------------------------------------
#if !defined(AFX_MODELCONTAINER_H__C630B7B1_7B9D_4BD7_9251_583D0E278227__INCLUDED_)
#define AFX_MODELCONTAINER_H__C630B7B1_7B9D_4BD7_9251_583D0E278227__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "A3DModel.h"
#include "AList2.h"

class CELBuilding;


//便编辑器用的静态模型
typedef struct ESTATICMODEL
{

public:
	ESTATICMODEL(){ pModel = NULL; };
	~ESTATICMODEL(){};
	
	CELBuilding *pModel;
	//AString m_strPathName;
	//AString m_strHullPathName;
	unsigned int id; 

}* PESTATICMODEL;


class CModelContainer  
{
public:
	CModelContainer();
	virtual ~CModelContainer();

	//这儿是把工作目录下的Models下的所有模型文件分类加载到列表中
	bool LoadAllModel();
	void ReleaseAllModel();
	
	void AddElement(const AString& strPath);
	
	//静态模型
	int GetStaticModelNum(){ return m_listStaticModel.GetCount(); };
	PESTATICMODEL GetStaticModel(int index);
	PESTATICMODEL GetStaticModel(const AString& strName);

	//其他模型

private:
	bool LoadStaticModel();
	bool ReadStaticModel(CString path);
	//这儿存放A3D静态模型，如果有其他类型的模型就直接在后面加入一个表就好了
	APtrList<PESTATICMODEL> m_listStaticModel;
};

extern CModelContainer g_ModelContainer;

#endif // !defined(AFX_MODELCONTAINER_H__C630B7B1_7B9D_4BD7_9251_583D0E278227__INCLUDED_)
