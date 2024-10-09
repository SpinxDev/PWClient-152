// ScenePrecinctObject.h: interface for the CScenePrecinctObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEPRECINCTOBJECT_H__CB4D141A_E7A6_481B_AEF8_CD3A63FB9A86__INCLUDED_)
#define AFX_SCENEPRECINCTOBJECT_H__CB4D141A_E7A6_481B_AEF8_CD3A63FB9A86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneObject.h"
#include "AObject.h"
#include "AList2.h"
#include "EL_BackMusic.h"

struct PRECINCT_DATA
{
	int     nPath;			//��ʾBezier����Χ�ɵĶ�����������nPath == 0,��ô�����ı�������
	AString strPathName;	//���ߵ�����
	AString strPrecinct;	//����ı���
	int     nPriority;		//����˳��
	int     nSelfCity;      //�Լ��ĸ�������ID
	int     nCity;			//�سǵ��Ӧ�ĸ���ID��Ĭ�������1,��ʾ�ص���Ҫ��������(world)
	A3DVECTOR3 vCityPoint;  //�سǵ�
	bool    bIsPrecinct;    //�Ƿ��ǹ�Ͻ���򣬸������Ѿ�����
	DWORD   dwPrecinctID;   //�����ţ�������������������ͬ����ô������������Χ�ɵ��������
						    //��Ϊ����һ������,���ID==0��ô����ʾ������ֻ���ڿͻ��ˣ������������
	
	int     nPartOfCity;    //�������� 0,��ʾ�������κγ���
	//music
	AString m_strSound;
	AString m_strSoundNight;
	int     nAlternation; //���׸�֮���ʱ����
	int     nLoopType;    //����֮���ѭ����ʽ�� 0 ��ѭ����1��ͷѭ����2�ӵڶ��׿�ʼѭ��
	bool	bPkProtect; //�Ƿ���PK������
};

struct PRECINCT_NPC  //���������NPC�б�
{
	CString     strName; //NPC������
	A3DVECTOR3  vPos;    //NPC��λ��
};

void init_precinct_data( PRECINCT_DATA& dat);

struct NPC_ID_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

struct BEZIER_ID_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

struct BEZIER_ID_FUNCS2 : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

struct MUSIC_SEL_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

class CScenePrecinctObject :public CSceneObject 
{
public:
	CScenePrecinctObject();
	virtual ~CScenePrecinctObject();


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

	PRECINCT_DATA&	GetProperty(){ return m_Data; };
	void			SetProperty(PRECINCT_DATA & dat) { m_Data = dat; };
	int				GetNpcNum(){ return m_listNpc.GetCount(); };
	PRECINCT_NPC*	GetNpc(int index){ return ((PRECINCT_NPC*)m_listNpc.GetByIndex(index)); };
	void            AddNpc( PRECINCT_NPC *pNewNpc);
	void            DeleteNpc( int idx );

	inline int      GetMusicNum(){ return m_listMusic.GetCount(); };
	AString         GetMusic( int index);
	void            AddMusic( AString *pMusic);
	void            DelMusic( AString *pMusic);
	void            PlayBackMusic();
	void            StopBackMusic();

	
protected:
	void ReBuildSound();
	void PlayMusic(bool bPlay);
	void BuildProperty();
	PRECINCT_DATA m_Data;
	BEZIER_ID_FUNCS* m_pBIDSelFuncs;
	NPC_ID_FUNCS* m_pFIDSelFuncs;
	AString m_strDummyName;			
	APtrList<PRECINCT_NPC*> m_listNpc;
    ASetTemplate<int> city_type;
	ASetTemplate<int> music_type;
	
	APtrList<AString *> m_listMusic;
	MUSIC_SEL_FUNCS *m_pMusicSelFuncs;
	AString strMusicList;
	bool    m_bIsPlaying;
	bool    m_bIsPlayingNight;

};


// Range object

class CSceneRangeObject :public CSceneObject 
{
public:
	CSceneRangeObject();
	virtual ~CSceneRangeObject();
	enum
	{
		RANGE_PK = 0,
		RANGE_SAFETY,
	};

public:

	virtual void Translate(A3DVECTOR3 vDelta);
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

	inline int GetBezierID(){ return m_nBezierID; };
	void SetBezierID( int nID ){ m_nBezierID = nID; };
	void SetBezierName( AString name){ m_strBezier = name; };
	inline AString GetRangeName(){ return m_strRange; };
	inline AString GetBezierName(){ return m_strBezier; };
	inline int     GetRangeType(){ return m_nType; };



protected:
	void BuildProperty();
	ASetTemplate<int> range_type;
	AString m_strRange;
	AString m_strBezier;
	int     m_nBezierID;
	int     m_nType;
	BEZIER_ID_FUNCS2* m_pBIDSelFuncs;

};

#endif // !defined(AFX_SCENEPRECINCTOBJECT_H__CB4D141A_E7A6_481B_AEF8_CD3A63FB9A86__INCLUDED_)
