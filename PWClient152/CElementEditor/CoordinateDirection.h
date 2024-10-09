//----------------------------------------------------------
// Filename	: CoordinateDirection.h
// Creator	: QingFeng Xin
// Date		: 2004.8.20
// Desc		: ����ָʾ�ࡣ������ʾ����������Ϣ��ͬʱ����ѡ�е�
//			  �������������ƽ��
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

	//��������ԭ��λ��
	void SetPos(A3DVECTOR3 vPos);
	
	//����������ת����
	void SetRotate(A3DMATRIX4 matR){ m_matRotate = matR; };
	
	//����һ����������ʾ����ѡ�е�ƽ�����ĳһ��������
	//ͨ���������ֵ�����������ƶ���ת�������ƽ��
	int  RayTrace(A3DVECTOR3 vStart,A3DVECTOR3 vEnd);
	
	//��������
	void Draw();

	//�����Ƿ����ᱻ������  
	int GetSelectedFlags(){ return m_nSelectedAxis; };
	void SetSelectedFlags(int nFlag){ m_nSelectedAxis = nFlag; };
	//����������λ�ú�һ��Ҫ���ô˺��������¾���λ��
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
