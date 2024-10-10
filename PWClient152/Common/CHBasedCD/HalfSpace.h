/////////////////////////////////////////////////
//	created by He wenfeng
//  2004-8-17
//  A class describes the half 3D space devided by the plane
/////////////////////////////////////////////////

#ifndef	_HALFSPACE_H_
#define _HALFSPACE_H_

#include <A3DVector.h>
#include <A3DMatrix.h>
#define HS_DEFAULT_DISTTHRESH 0.000001f				//距离差的阈值，作为判断是在半空间内部or边界上
#define ABS(x) (((x)<0)?(-(x)):(x))


namespace CHBasedCD
{

////////////////////////////////////////////////////////////////////////////
// 
// 平面方程的定义为N.X=D，在本类中的形式为 m_vNormal.X=m_d
// 注意：这与一般的平面方程Ax+By+Cz+D=0不一样，区别在于D的正负！
// 
////////////////////////////////////////////////////////////////////////////

class CHalfSpace
{
	static float DistThresh;		//空间一顶点到halfspace平面的距离阈值，作为判断是在半空间内部、外部，or 边界上
									//该阈值属于整个类而不是某一个对象，因此定义为静态成员
public:

	//根据三个顶点构造平面及半空间区域
	CHalfSpace(const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& v3) { Set(v1,v2,v3); }
	//根据两个顶点构造平行于Z轴的平面及半空间区域
	CHalfSpace(const A3DVECTOR3& v1, const A3DVECTOR3& v2) { Set(v1,v2); }
	CHalfSpace(){}

	CHalfSpace(const CHalfSpace& hs):m_vNormal(hs.m_vNormal) { m_d=hs.m_d; }

	~CHalfSpace() {}

	// 对决定Halfspace的平面进行变换！变换矩阵为mtxTrans
	virtual void Transform(const A3DMATRIX4& mtxTrans);

	void Translate(float Delta) { m_d+= Delta;}				//沿法线方向作一个平移
	void Inverse() { m_d=-m_d;m_vNormal=-m_vNormal;}		//翻转半空间的正反方向

	//点v到半空间定义平面的距离，此距离带符号！
	inline float DistV2Plane(const A3DVECTOR3& v)const;				

	//与线段求交
	int IntersectLineSeg(const A3DVECTOR3& v1,const A3DVECTOR3& v2,A3DVECTOR3& vIntersection) const;

	////////////////////////////////////////////////////////////
	// 判断一个三维点是否处于半空间内部，或者说处于平面的一侧
	// 注意halfspace的OutSide,Inside定义方式
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
	static void SetDistThresh(float DThresh=HS_DEFAULT_DISTTHRESH)		//并设定一个缺省值
	{
		DistThresh=DThresh;
	}
	
	A3DVECTOR3 GetNormal() const {return m_vNormal;}
	float GetDist() const { return m_d;}
	
	void Set(const A3DVECTOR3& v1, const A3DVECTOR3& v2);
	void Set(const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& v3);
	
	//直接设置法向
	void SetNormal(const A3DVECTOR3& n) { m_vNormal=n;m_vNormal.Normalize();}
	//直接设置距离d
	void SetD(float d) { m_d=d;}

	//点法式
	void SetNV(const A3DVECTOR3& n, const A3DVECTOR3& v);

//Attributes
protected:
	A3DVECTOR3 m_vNormal;
	float m_d;

};

//在该半空间的内部！
inline bool CHalfSpace::Inside(const A3DVECTOR3& v) const
{
	float d=DotProduct(v,m_vNormal);
	bool bInside=(m_d-d>DistThresh);
	return (bInside);
}

//在该半空间的外部！
inline bool CHalfSpace::Outside(const A3DVECTOR3& v) const
{
	float d=DotProduct(v,m_vNormal);
	bool bOutside=(d-m_d>DistThresh);
	return (bOutside);
}

//在该半空间的分界平面上
inline bool CHalfSpace::OnPlane(const A3DVECTOR3& v) const
{
	float d=DotProduct(v,m_vNormal);
	bool bOnPlane=(ABS(m_d-d)<=DistThresh);
	return bOnPlane;
}

///////////////////////////////////////////////////////////////////
// 点v到平面的距离
// v在平面内部时，此距离为-
// v在平面外部时，此距离为+
///////////////////////////////////////////////////////////////////
inline float CHalfSpace::DistV2Plane(const A3DVECTOR3& v)const
{
	float d=DotProduct(v,m_vNormal);
	return (d-m_d);
}

}	// end namespace

#endif	//	_HALFSPACE_H_