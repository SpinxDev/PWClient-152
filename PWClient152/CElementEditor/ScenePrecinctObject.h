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
	int     nPath;			//表示Bezier曲线围成的多边形区域，如果nPath == 0,那么就用四边形区域
	AString strPathName;	//曲线的名字
	AString strPrecinct;	//区域的别名
	int     nPriority;		//优先顺序
	int     nSelfCity;      //自己的副本世界ID
	int     nCity;			//回城点对应的副本ID，默认情况是1,表示回到主要副本区域(world)
	A3DVECTOR3 vCityPoint;  //回城点
	bool    bIsPrecinct;    //是否是管辖区域，该属性已经废弃
	DWORD   dwPrecinctID;   //区域编号，如果两个区域的区域编号同，那么这两个区域所围成的面积我们
						    //认为它是一个区域,如果ID==0那么他表示该数据只用于客户端，不是真的区域
	
	int     nPartOfCity;    //所属城市 0,表示不属于任何城市
	//music
	AString m_strSound;
	AString m_strSoundNight;
	int     nAlternation; //两首歌之间的时间间隔
	int     nLoopType;    //歌曲之间的循环方式。 0 不循环；1从头循环；2从第二首开始循环
	bool	bPkProtect; //是否是PK保护区
};

struct PRECINCT_NPC  //区域里面的NPC列表
{
	CString     strName; //NPC的名字
	A3DVECTOR3  vPos;    //NPC的位置
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
