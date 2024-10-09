// EditerBezierSegment.h: interface for the CEditerBezierSegment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITERBEZIERSEGMENT_H__6F517999_516E_4C36_8698_ACFC4718BF77__INCLUDED_)
#define AFX_EDITERBEZIERSEGMENT_H__6F517999_516E_4C36_8698_ACFC4718BF77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "A3DBezierSegment.h"
#include "EditerBezierPoint.h"
#include "A3DString.h"

class CEditerBezier;

#define CALCLENGTHDIVIDE	36

class CEditerBezierSegment : public A3DBezierSegment 
{
public:

	A3DVECTOR3 Bezier(float u);//ȡ��λ��
	A3DVECTOR3 Vector(float u);//ȡ�÷���

	void SetAnchor2(const A3DVECTOR3 & anchor);
	A3DVECTOR3 GetAnchor2();
	void SetAnchor1(const A3DVECTOR3 & anchor);
	A3DVECTOR3 GetAnchor1();
	void SetTail(const A3DVECTOR3 & tail);
	A3DVECTOR3 GetTail();
	void SetHead(const A3DVECTOR3 & head);
	A3DVECTOR3 GetHead();
	void SetLink(int link);
	int GetLink();
	void SetHeadSpot(const A3DVECTOR3& vec){ m_av3_head_spot = vec; };
	void SetTailSpot(const A3DVECTOR3& vec){ m_av3_tail_spot = vec; };
	
	A3DVECTOR3 GetHeadSpot(){ return m_av3_head_spot; };
	A3DVECTOR3 GetTailSpot(){ return m_av3_tail_spot;};

	void SetHeadID(int id){m_nHeadID = id;}
	void SetTailID(int id){m_nTailID = id;}
	int  GetHeadID(){ return m_nHeadID; }
	int  GetTailID(){ return m_nTailID; }

	void ReCalculateLength();
	float GetLenght(){ return m_fLenght; };

	bool Init(
		A3DDevice * pA3DDevice, 
		A3DVECTOR3& vecStart, 
		A3DVECTOR3& vecEnd, 
		A3DVECTOR3& vecStartCtrl, 
		A3DVECTOR3& vecEndCtrl, 
		float vSegmentRadius = 1.0f, 
		float vControlRadius = 2.0f,
		A3DCOLOR colorSegment = A3DCOLORRGB(255,255,0), 
		A3DCOLOR colorControl = A3DCOLORRGB(0,255,0));

	
	CEditerBezierSegment();
	virtual ~CEditerBezierSegment();

protected:
	int			m_int_link;			// ��Ӧ������ID 
	A3DVECTOR3	m_av3_anchor1;		// ����ê��1
	A3DVECTOR3	m_av3_anchor2;		// ����ê��2
	A3DVECTOR3	m_av3_head;			// ͷ��
	A3DVECTOR3	m_av3_tail;			// β��
	A3DVECTOR3  m_av3_head_spot;    // ͷ�ӵ�
	A3DVECTOR3  m_av3_tail_spot;    // β�ӵ�
	
	int         m_nHeadID;          //ͷ���ڵ��б��е�ID
	int         m_nTailID;          //β���ڵ��б��е�ID
	
	A3DVECTOR3  m_av3_saves[CALCLENGTHDIVIDE];//��ʱ����
	float       m_fLenght;                    //��ʱ����
};

#endif // !defined(AFX_EDITERBEZIERSEGMENT_H__6F517999_516E_4C36_8698_ACFC4718BF77__INCLUDED_)
