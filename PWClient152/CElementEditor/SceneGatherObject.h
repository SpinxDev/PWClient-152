// SceneGatherObject.h: interface for the CSceneGatherObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEGATHEROBJECT_H__D11BBBFF_2383_47A1_8217_08B5522B0E3D__INCLUDED_)
#define AFX_SCENEGATHEROBJECT_H__D11BBBFF_2383_47A1_8217_08B5522B0E3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObject.h"

class AString;

struct GATHER_DATA
{
	float fEdgeLen; //采集产生区域的长宽
	float fEdgeWth;
	
	int   nControllerId;
	int   nGatherNum;  // 能够生成的最大数目
	bool  bGatherRenascenceAuto;//是否自动重生
	
	//下面的数据丢弃
	bool  bGenGatherInit;//是否起始时生成
	bool  bValidAtOneTime;//是否同时生效一次
	DWORD dwGenID;//制定编号
};

struct GATHER
{
	DWORD dwType; //可采集对象的类型，如：铜矿，药草
	DWORD dwId;   //该采集对象在模板编辑器中对应的ID
	AString strName;//对象名字,不用输出
	DWORD dwRefurbishTime;//刷新时间
	DWORD dwNum;//该对象在该区域中的个数
	float fOffset;//该类对象离地面的高度
};

void Init_Gather_Data(GATHER_DATA &data);

struct GATHER_SEL_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

struct CONTROLLER_GATHER_ID_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

class CSceneGatherObject :public CSceneObject
{
	
public:
	CSceneGatherObject();
	virtual ~CSceneGatherObject();

public:

	virtual void Translate(const A3DVECTOR3& vDelta);
	virtual void Render(A3DViewport* pA3DViewport);
	virtual void Tick(DWORD timeDelta);
	virtual void UpdateProperty(bool bGet);
	virtual void Release();
	virtual void EntryScene();
	virtual void LeaveScene();
	//Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);
	
	//Exp data
	bool ExpData(AFile *pFile,DWORD dwVersion);
	bool ImpData(AFile *pFile,DWORD dwVersion);
	//  Copy object
	virtual CSceneObject*		CopyObject();
	GATHER_DATA					GetProperty(){ return m_Data; };
	void						SetProperty(const GATHER_DATA& data){ m_Data = data; };

	DWORD						GetGatherNum();
	AString                     GetGatherModelPath(int index);
	//CECModel*                   GetGatherModel(int index);
	GATHER*				        GetGather(int index); 
	GATHER*				        GetGather(AString name); 
	void                        AddGather(GATHER* gather);
	void                        DeleteGather(int index);
	int                         GetCopyFlags(){ return m_nCopy; }
	void                        SetCopyFlags( int nCopy){ m_nCopy = nCopy; }

private:
	AString                     GetModelPath(DWORD dwId,DWORD  stype);
	void                        DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum,DWORD clr);
	void						DrawRect();
	void						BuildProperty();
	void                        ReloadModels();
	void                        ReleaseModels();
	void                        RenderModels(A3DViewport *pView);
	void                        DrawMineName();
	
	int                         m_nGatherType;      //可收集对象的类型
	ASetTemplate<int>           gather_type;        //可采集对象的类型
	GATHER_DATA		            m_Data;				//可采集对象的数据
	CPtrList                    m_listGather;	    //可采集对象的列表
	CPtrList                    m_listModels;       //可采集对象的模型列表
	GATHER_SEL_FUNCS*           m_pIDSelFuncs;
	CONTROLLER_GATHER_ID_FUNCS* m_pControllerSelFuncs;
	AString                     m_strGatherList;
	float                       m_fArea;
	int                         m_nCopy;//数据套数，决定是第几套数据(0,1)
};

#endif // !defined(AFX_SCENEGATHEROBJECT_H__D11BBBFF_2383_47A1_8217_08B5522B0E3D__INCLUDED_)
