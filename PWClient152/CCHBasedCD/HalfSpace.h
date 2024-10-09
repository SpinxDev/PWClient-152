/////////////////////////////////////////////////
//	created by He wenfeng
//  2004-8-17
//  A class describes the half 3D space devided by the plane
/////////////////////////////////////////////////

#ifndef	_HALFSPACE_H_
#define _HALFSPACE_H_

#include <A3DVector.h>
#include <A3DMatrix.h>
#define HS_DEFAULT_DISTTHRESH 0.000001f				//��������ֵ����Ϊ�ж����ڰ�ռ��ڲ�or�߽���
#define ABS(x) (((x)<0)?(-(x)):(x))


namespace CHBasedCD
{

////////////////////////////////////////////////////////////////////////////
// 
// ƽ�淽�̵Ķ���ΪN.X=D���ڱ����е���ʽΪ m_vNormal.X=m_d
// ע�⣺����һ���ƽ�淽��Ax+By+Cz+D=0��һ������������D��������
// 
////////////////////////////////////////////////////////////////////////////

class CHalfSpace
{
	static float DistThresh;		//�ռ�һ���㵽halfspaceƽ��ľ�����ֵ����Ϊ�ж����ڰ�ռ��ڲ����ⲿ��or �߽���
									//����ֵ���������������ĳһ��������˶���Ϊ��̬��Ա
public:

	//�����������㹹��ƽ�漰��ռ�����
	CHalfSpace(const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& v3) { Set(v1,v2,v3); }
	//�����������㹹��ƽ����Z���ƽ�漰��ռ�����
	CHalfSpace(const A3DVECTOR3& v1, const A3DVECTOR3& v2) { Set(v1,v2); }
	CHalfSpace(){}

	CHalfSpace(const CHalfSpace& hs):m_vNormal(hs.m_vNormal) { m_d=hs.m_d; }

	~CHalfSpace() {}

	// �Ծ���Halfspace��ƽ����б任���任����ΪmtxTrans
	virtual void Transform(const A3DMATRIX4& mtxTrans);

	void Translate(float Delta) { m_d+= Delta;}				//�ط��߷�����һ��ƽ��
	void Inverse() { m_d=-m_d;m_vNormal=-m_vNormal;}		//��ת��ռ����������

	//��v����ռ䶨��ƽ��ľ��룬�˾�������ţ�
	inline float DistV2Plane(const A3DVECTOR3& v)const;				

	//���߶���
	int IntersectLineSeg(const A3DVECTOR3& v1,const A3DVECTOR3& v2,A3DVECTOR3& vIntersection) const;

	////////////////////////////////////////////////////////////
	// �ж�һ����ά���Ƿ��ڰ�ռ��ڲ�������˵����ƽ���һ��
	// ע��halfspace��OutSide,Inside���巽ʽ
	//				   /\
	//		          /||\
	//				   ||
	//				   || normal    (Outside)
	//------------------------------------------Plane
	//								(Inside)
	//
	inline bool Inside(const A3DVECTOR3& v) const;
	inline bool Outside(const A3DVECTOR3& v) const;
	inline bool OnPlane(const A3DVECTOR3& v) const;

//set && get operations
	static void SetDistThresh(float DThresh=HS_DEFAULT_DISTTHRESH)		//���趨һ��ȱʡֵ
	{
		DistThresh=DThresh;
	}
	
	A3DVECTOR3 GetNormal() const {return m_vNormal;}
	float GetDist() const { return m_d;}
	
	void Set(const A3DVECTOR3& v1, const A3DVECTOR3& v2);
	void Set(const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& v3);
	
	//ֱ�����÷���
	void SetNormal(const A3DVECTOR3& n) { m_vNormal=n;m_vNormal.Normalize();}
	//ֱ�����þ���d
	void SetD(float d) { m_d=d;}

	//�㷨ʽ
	void SetNV(const A3DVECTOR3& n, const A3DVECTOR3& v);

//Attributes
protected:
	A3DVECTOR3 m_vNormal;
	float m_d;

};

//�ڸð�ռ���ڲ���
inline bool CHalfSpace::Inside(const A3DVECTOR3& v) const
{
	float d=DotProduct(v,m_vNormal);
	bool bInside=(m_d-d>DistThresh);
	return (bInside);
}

//�ڸð�ռ���ⲿ��
inline bool CHalfSpace::Outside(const A3DVECTOR3& v) const
{
	float d=DotProduct(v,m_vNormal);
	bool bOutside=(d-m_d>DistThresh);
	return (bOutside);
}

//�ڸð�ռ�ķֽ�ƽ����
inline bool CHalfSpace::OnPlane(const A3DVECTOR3& v) const
{
	float d=DotProduct(v,m_vNormal);
	bool bOnPlane=(ABS(m_d-d)<=DistThresh);
	return bOnPlane;
}

///////////////////////////////////////////////////////////////////
// ��v��ƽ��ľ���
// v��ƽ���ڲ�ʱ���˾���Ϊ-
// v��ƽ���ⲿʱ���˾���Ϊ+
///////////////////////////////////////////////////////////////////
inline float CHalfSpace::DistV2Plane(const A3DVECTOR3& v)const
{
	float d=DotProduct(v,m_vNormal);
	return (d-m_d);
}

}	// end namespace

#endif	//	_HALFSPACE_H_