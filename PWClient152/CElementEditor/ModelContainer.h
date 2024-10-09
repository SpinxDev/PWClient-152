// ModelContainer.h: interface for the CModelContainer class.
//
//////////////////////////////////////////////////////////////////////
//----------------------------------------------------------
// Filename	: ModelContainer.h
// Creator	: QingFeng Xin
// Date		: 2004.8.28
// Desc		: �༭����Ҫ�õ���ģ�����ȼ��ص���������
//-----------------------------------------------------------
#if !defined(AFX_MODELCONTAINER_H__C630B7B1_7B9D_4BD7_9251_583D0E278227__INCLUDED_)
#define AFX_MODELCONTAINER_H__C630B7B1_7B9D_4BD7_9251_583D0E278227__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "A3DModel.h"
#include "AList2.h"

class CELBuilding;


//��༭���õľ�̬ģ��
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

	//����ǰѹ���Ŀ¼�µ�Models�µ�����ģ���ļ�������ص��б���
	bool LoadAllModel();
	void ReleaseAllModel();
	
	void AddElement(const AString& strPath);
	
	//��̬ģ��
	int GetStaticModelNum(){ return m_listStaticModel.GetCount(); };
	PESTATICMODEL GetStaticModel(int index);
	PESTATICMODEL GetStaticModel(const AString& strName);

	//����ģ��

private:
	bool LoadStaticModel();
	bool ReadStaticModel(CString path);
	//������A3D��̬ģ�ͣ�������������͵�ģ�;�ֱ���ں������һ����ͺ���
	APtrList<PESTATICMODEL> m_listStaticModel;
};

extern CModelContainer g_ModelContainer;

#endif // !defined(AFX_MODELCONTAINER_H__C630B7B1_7B9D_4BD7_9251_583D0E278227__INCLUDED_)
