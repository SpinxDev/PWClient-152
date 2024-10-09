// SceneBoxArea.h: interface for the CSceneBoxArea class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEBOXAREA_H__73631896_32C9_416D_96F8_C6A0F53196C4__INCLUDED_)
#define AFX_SCENEBOXAREA_H__73631896_32C9_416D_96F8_C6A0F53196C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "A3DSky.h"
#include "SceneObject.h"

class AMSoundStream;
class A3DRain;
class A3DSnow;

typedef struct BOX_AREA_PROPERTY
{
	float   m_fLength;
	float   m_fHeight;
	float   m_fWidth;

	//fog 2
	float   m_fFogStart;
	float   m_fFogEnd;
	float   m_fFogDeepness;
	DWORD   m_fFogColor;

	float   m_fFogStartNight;
	float   m_fFogEndNight;
	float   m_fFogDeepnessNight;
	DWORD   m_fFogColorNight;

	//fog of under water 1
	float   m_fUnderWaterFogStart;
	float   m_fUnderWaterFogEnd;
	float   m_fUnderWaterFogDeepness;
	DWORD   m_dwUnderWaterClr;

	float   m_fUnderWaterFogStartNight;
	float   m_fUnderWaterFogEndNight;
	float   m_fUnderWaterFogDeepnessNight;
	DWORD   m_dwUnderWaterClrNight;
	
	//sky box textures
	AString m_strSky0;
	AString m_strSky1;
	AString m_strSky2;
	
	AString m_strSkyNight0;//Night sky
	AString m_strSkyNight1;
	AString m_strSkyNight2;

	float   m_fCloudSpeedU;
	float   m_fCloudSpeedV;
	
	DWORD   m_dwCloudDirClr;//云层白天直射光颜色
	DWORD   m_dwCloudAmbientClr;//云层白天环境颜色
	DWORD   m_dwCloudNightDirClr;//云层黑夜直射光颜色
	DWORD   m_dwCloudNightAmbientClr;//云层的黑夜的环境光颜色
	//light
	A3DVECTOR3 m_vLightDir; //白天直射光方向 new
	A3DVECTOR3 m_vLightDirNight;//夜晚直射光方向 new
	
	DWORD      m_dwDirLightClr;//直射光
	DWORD      m_dwAmbient;
	float      m_fSunPower;//new
	int	       m_nMapSize;//new

	DWORD      m_dwDirLightClrNight;//Night light
	DWORD      m_dwAmbientNight;
	float      m_fSunPowerNight;//new
	int	       m_nMapSizeNight;//new
	
	//transition time
	DWORD   m_dwTransTime;
	
	//music
	//AString m_strMusic;
	AString m_strSound;
	

	//Nature
	int     m_nRainSpeed;
	float   m_fRainSize;
	DWORD   m_dwRainClr;
	
	int     m_nSnowSpeed;
	float   m_fSnowSize;
	DWORD   m_dwSnowClr;
	DWORD   m_dwFlag;//1b 表示有没有灯光，2b 表示下雨，3b表示下雪。4b,表是有没太阳，5b，表示有没月亮
	
	DWORD   m_dwTerrainLight;
}BOXAREA_PROPERTY;

void Init_BoxArea_Data(BOX_AREA_PROPERTY &data);


struct NATURE_DATA_SET_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

struct LIGHTING_SET_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

class CSceneBoxArea : public CSceneObject
{
public:
	CSceneBoxArea();
	CSceneBoxArea(bool bDefault);
	virtual ~CSceneBoxArea();

	
	virtual void UpdateProperty(bool bGet);
	virtual void Render(A3DViewport* pA3DViewport);
	virtual void Tick(DWORD timeDelta);

	//Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);
	
	virtual void		Translate(const A3DVECTOR3& vDelta);
	virtual void		RotateX(float fRad);
	virtual void		RotateY(float fRad);
	virtual void		RotateZ(float fRad);
	virtual void        Rotate(const A3DVECTOR3& axis,float fRad);
	virtual void        Scale(float fScale);
	virtual void        SetPos(const A3DVECTOR3& vPos);
	virtual void        Release();
	virtual void        EntryScene();
	virtual void        LeaveScene();
	
	//  Copy object
	virtual CSceneObject*		CopyObject();

	//判断一个点是否在区域的内部,如果在内返回为TRUE
	bool IsInside(A3DVECTOR3 vPoint);

	CSceneBoxArea* GetLastBoxArea(){ return m_pLastArea; };

	void SetLastBoxArea( CSceneBoxArea* pLastArea){ m_pLastArea = pLastArea; };

	void GetProperty(BOXAREA_PROPERTY & data)
	{
		data = m_PropertyData;
	}

	void SetProperty(BOXAREA_PROPERTY & data)
	{
		m_PropertyData = data;
		ReBuildArea();
	}

	//光线跟踪程序
	bool	RayTrace(A3DVECTOR3 vStart,A3DVECTOR3 vEnd);
	
	void    SetSkyTexture(AString str1,AString str2, AString str3)
	{
			m_PropertyData.m_strSky0 = str1;
			m_PropertyData.m_strSky1 = str2;
			m_PropertyData.m_strSky2 = str3;
	}

	//背景音乐的处理函数
	void InitMusic();
	void PlayMusic();
	void StopMusic();

	void OnCamEntry(bool bForceUpdate=false);//摄象机进入区域事件
	void OnCamLeave();//摄象机离开区域事件

	bool IsDefaultArea(){ return m_bIsDefault; };
	bool InitNatureObjects();

	//判断区域是否跟该区域相交
	bool IsIntersectionWithArea(const A3DVECTOR3& vMin, const A3DVECTOR3& vMax);

	void ReBuildArea();
	void BuildAreaProperty();
	BOXAREA_PROPERTY m_PropertyData;

private:
	
	CSceneBoxArea *m_pLastArea;

	//测试点跟平面的关系，如果在平面的正前方返回TRUE
	bool DotWithPlane(A3DVECTOR3 testPoint,A3DVECTOR3 point1, A3DVECTOR3 point2, A3DVECTOR3 point3);
	
	//2D线的拾取
	bool IntersectionLineWithDot(int x1, int y1, int x2, int y2 , int dotX, int dotY);
	
	//保存了区域盒子的8个点
	A3DVECTOR3 Vertices[8];

	A3DVECTOR3 m_vMin;
	A3DVECTOR3 m_vMax;

	//当前区域的天空盒子
	A3DSky*		m_pA3DSky;

	//天气效果
	A3DRain*			m_pRain;					//	rain in the world
	A3DSnow*			m_pSnow;					//	snow in the world
	NATURE_DATA_SET_FUNCS *m_pNatureDataSetFunc;
	LIGHTING_SET_FUNCS    *m_pLightingSelFunc;
	bool m_bIsInside;

	//是否是默认的区域
	bool m_bIsDefault;

	APtrList<AString *> m_listMusic;
	
	/*
	 下面是插值用的临时变量
	 */
	
	DWORD m_dwEscapeTime;
	bool  m_bTransDone;

	AString strText;
	bool bSnow;
	bool bRain;
	bool bLight;
	bool bShowMoon;
	bool bShowSun;
};

typedef CSceneBoxArea* PSCENEBOXAREA;

#endif // !defined(AFX_SCENEBOXAREA_H__73631896_32C9_416D_96F8_C6A0F53196C4__INCLUDED_)
