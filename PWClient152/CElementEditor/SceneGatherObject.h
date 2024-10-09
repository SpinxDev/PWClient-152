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
	float fEdgeLen; //�ɼ���������ĳ���
	float fEdgeWth;
	
	int   nControllerId;
	int   nGatherNum;  // �ܹ����ɵ������Ŀ
	bool  bGatherRenascenceAuto;//�Ƿ��Զ�����
	
	//��������ݶ���
	bool  bGenGatherInit;//�Ƿ���ʼʱ����
	bool  bValidAtOneTime;//�Ƿ�ͬʱ��Чһ��
	DWORD dwGenID;//�ƶ����
};

struct GATHER
{
	DWORD dwType; //�ɲɼ���������ͣ��磺ͭ��ҩ��
	DWORD dwId;   //�òɼ�������ģ��༭���ж�Ӧ��ID
	AString strName;//��������,�������
	DWORD dwRefurbishTime;//ˢ��ʱ��
	DWORD dwNum;//�ö����ڸ������еĸ���
	float fOffset;//������������ĸ߶�
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
	
	int                         m_nGatherType;      //���ռ����������
	ASetTemplate<int>           gather_type;        //�ɲɼ����������
	GATHER_DATA		            m_Data;				//�ɲɼ����������
	CPtrList                    m_listGather;	    //�ɲɼ�������б�
	CPtrList                    m_listModels;       //�ɲɼ������ģ���б�
	GATHER_SEL_FUNCS*           m_pIDSelFuncs;
	CONTROLLER_GATHER_ID_FUNCS* m_pControllerSelFuncs;
	AString                     m_strGatherList;
	float                       m_fArea;
	int                         m_nCopy;//���������������ǵڼ�������(0,1)
};

#endif // !defined(AFX_SCENEGATHEROBJECT_H__D11BBBFF_2383_47A1_8217_08B5522B0E3D__INCLUDED_)
