// EditerBezier.h: interface for the CEditerBezier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITERBEZIER_H__4F2959A8_860A_412A_A82E_5312873B2A2D__INCLUDED_)
#define AFX_EDITERBEZIER_H__4F2959A8_860A_412A_A82E_5312873B2A2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BEZIER_VERSION 0x3;

#include "EditerBezierPoint.h"
#include "EditerBezierSegment.h"
#include "A3DMacros.h"
#include "SceneObject.h"
class A3DViewport;
class A3DDevice;
class A3DVECTOR3;
class CECBezier;
class AFile;

class CEditerBezier :public CSceneObject
{
public:

	virtual A3DVECTOR3 GetMin();
	virtual A3DVECTOR3 GetMax();
	virtual void       UpdateProperty(bool bGet);
	
	//BOOL UniteMap(APathMap * pMap);
	//APathMap * GeneralMap();
	virtual void Tick(DWORD timeDelta);
	virtual void Render(A3DViewport* pA3DViewport);

	virtual A3DVECTOR3  GetPos();
	virtual void		Translate(const A3DVECTOR3& vDelta);
	virtual void        SetPos(const A3DVECTOR3& vPos);
	
	bool         ExportBezier(AFile*pFile, const A3DVECTOR3& vOffset);
	bool         ExportBezier(FILE *pFile, const A3DVECTOR3& vOffset);
	bool         ExportSevBezier(FILE *pFile, const A3DVECTOR3& vOffset);
	
	//	Load data
	virtual bool Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags);
	//	Save data
	virtual bool Save(CELArchive& ar,DWORD dwVersion);
	virtual void Release();
	
	//  Copy object
	virtual CSceneObject*		CopyObject();

	CEditerBezierPoint * GetTailPoint();
	int  GetPointCount();
	void OptimizeSmooth();
	void OptimizeSmooth(POSITION posPoint,POSITION posSeg);
	CEditerBezierPoint* CreatePoint(const A3DVECTOR3 & pos, int map, int point);
	void InsertPoint(const A3DVECTOR3 & pos, CEditerBezierSegment *pseg);
	void DeletePoint(int ptid);
	CEditerBezierPoint *FindPoint(int ptid);

	void AddPointTail(CEditerBezierPoint *pt);
	void AddSegmentTail(CEditerBezierSegment *seg);

	void DeleteAllPoint();
	void DeleteAllSegment();

	
	void ReleaseA3DObjects();
	void SetMap(int map);
	int  GetMap();

	CEditerBezier();
	CEditerBezier(int map, A3DDevice * pA3DDevice, float ptradius = 0.3f, float segradius = 0.2f, float anchorradius = 0.2f);
	virtual ~CEditerBezier();

	bool RayTrace(A3DVECTOR3& vStart,A3DVECTOR3& vEnd,bool bAdd);

	CEditerBezierSegment* GetCurCtrl(bool &bStart);
	CEditerBezierSegment* GetCurSegment(A3DVECTOR3 &vTarget);
	CEditerBezierPoint  * GetCurPoint(CEditerBezierSegment**segPre,CEditerBezierSegment**segNex);
	int GetHitFlag(){ return m_nFlag; };
	bool PickControl(A3DVECTOR3& vStart,A3DVECTOR3& vEnd, int &nFlag);
	
	void BuildBezierProperty();
	void GetProperty(DWORD& dwColor){ dwColor = m_dwColor; };
	void SetProperty(DWORD& dwColor){ m_dwColor = dwColor; };

	void SmoothBezier();
	void UpdateSegmentProperty();

	void ConvertToObstruct();
	// beizer or ploygon
	void Convert();

	int GeneratePointID();
	int GenerateSegmentID();

	void CalculateDirection();

	void EanbleTest(bool bTest){ m_bTestStart = bTest; };
	bool IsBezier(){ return m_bIsBezier; };
	void SetBezier( bool bezier){ m_bIsBezier = bezier; };

	A3DVECTOR3 GetHeadPos();
	A3DVECTOR3 GetTailPos();

	int        GetPointIdx(DWORD id);
	void       GeneralECBezier(CECBezier *pECBezier);
	void       BuildBoundingBox();
	
	bool        IntersectWithSelf();
	bool        LineIntersectWithLine(float ax1, float ay1,float ax2, float ay2,
										  float bx1, float by1,float bx2, float by2);
	//取得与该曲线相连的下一条曲线，这条曲线可能不在本地图中，如果没有下一条返回-1
	inline int         GetNextID(){ return m_nNextBezierID; };
	inline int         GetGlobalID(){ return m_nGlobalID; };
	//判断区域是否跟该区域相交
	bool        IsIntersectionWithArea(const A3DVECTOR3& vMin, const A3DVECTOR3& vMax);
	void        ReCalculateLength();
	//property data
	CPtrList	m_list_points;
	CPtrList	m_list_segments;

	float		m_float_pointradius;
	float		m_float_segradius;
	float		m_float_anchorsize;
protected:
	ASetTemplate<int> bezier_type;
	//对象属性
	DWORD       m_dwColor;//该路径的颜色
	bool        m_bIsBezier;//这儿不光是能表示B曲线也能表示直线
	int         m_nNextBezierID;//该ID记录了与该曲线相连的下一条曲线，导出是会合并成一条曲线
    AString     m_strActionName;//动作名。该名字只用于该曲线被带有动画的模型绑定
	int			m_int_map;//暂时没用
	int         m_int_link;
	A3DDevice * m_ptr_a3ddevice;
	
	int         m_nCurSegment;
	A3DVECTOR3  m_vSegHitPos;
	int         m_nCurPoint;
	int         m_nCurCtrlSeg;
	bool        m_bFirstCtrl;
	int         m_nFlag; //选取的标志
	int         m_nGlobalID;

	bool        m_bTestStart;
	float       m_fLenght;

private:
	A3DVECTOR3  m_vMins;
	A3DVECTOR3  m_vMaxs;
};

typedef CEditerBezier * PEDITERBEZIER;

#endif // !defined(AFX_EDITERBEZIER_H__4F2959A8_860A_412A_A82E_5312873B2A2D__INCLUDED_)
