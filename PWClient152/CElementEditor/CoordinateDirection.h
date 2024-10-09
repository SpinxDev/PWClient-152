//----------------------------------------------------------
// Filename	: CoordinateDirection.h
// Creator	: QingFeng Xin
// Date		: 2004.8.20
// Desc		: 坐标指示类。该类显示物体坐标信息，同时返回选中的
//			  坐标轴或是坐标平面
//-----------------------------------------------------------
#if !defined(AFX_COORDINATEDIRECTION_H__CB49D590_C6CB_464D_995C_7A70DA8B070E__INCLUDED_)
#define AFX_COORDINATEDIRECTION_H__CB49D590_C6CB_464D_995C_7A70DA8B070E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class A3DVector;
#include <a3dwirecollector.h>

//  intersects for ray trace\xqf
bool intersectsBoxWithLine(const A3DVECTOR3& linemiddle, const A3DVECTOR3& linevect,
						   float halflength,const A3DVECTOR3 &MaxEdge,const A3DVECTOR3 &MinEdge);


class CCoordinateDirection  
{
public:
	enum
	{
		AXIS_PLANE_NO = 0,
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
		PLANE_XY,
		PLANE_XZ,
		PLANE_YZ,
	};
	
public:
	CCoordinateDirection();
	virtual ~CCoordinateDirection();

	//设置坐标原点位置
	void SetPos(A3DVECTOR3 vPos);
	
	//设置坐标旋转矩阵
	void SetRotate(A3DMATRIX4 matR){ m_matRotate = matR; };
	
	//返回一个整数，表示光线选中的平面或是某一个坐标轴
	//通过这个返回值来决定对象移动旋转的轴向或平面
	int  RayTrace(A3DVECTOR3 vStart,A3DVECTOR3 vEnd);
	
	//绘制坐标
	void Draw();

	//返回是否有轴被鼠标点中  
	int GetSelectedFlags(){ return m_nSelectedAxis; };
	void SetSelectedFlags(int nFlag){ m_nSelectedAxis = nFlag; };
	//当你设置了位置后一定要调用此函数来更新绝对位置
	void Update();
	
	bool OnMouseMove(int x, int y, DWORD dwFlags);
	bool OnLButtonDown(int x, int y, DWORD dwFlags);
	bool OnLButtonUp(int x, int y, DWORD dwFlags);

	void SetCursor(HCURSOR hc){ m_hCursor = hc; }; 

private:

	void DrawArrow();
	void DrawPlane();
	void DrawAxis();
	void ReEdge(A3DVECTOR3 &vMaxEdge,A3DVECTOR3 &vMinEdge);
	
	bool TraceAxis(A3DVECTOR3 vAxis, A3DVECTOR3 vStart, A3DVECTOR3 vEnd, float& dis);
	bool TracePlane(A3DVECTOR3* pVertices, A3DVECTOR3 vStart,A3DVECTOR3 vEnd, float& dis);


	int  m_nSelectedAxis;
	int  m_nSelectedTemp;
	A3DMATRIX4 m_matRotate;
	A3DMATRIX4 m_matTranslate;
	A3DMATRIX4 m_matABS;
	A3DVECTOR3 m_vPos;
	bool       m_bLMouseDown;
	HCURSOR    m_hCursor;

	float m_fDelta;

};

#endif // !defined(AFX_COORDINATEDIRECTION_H__CB49D590_C6CB_464D_995C_7A70DA8B070E__INCLUDED_)
