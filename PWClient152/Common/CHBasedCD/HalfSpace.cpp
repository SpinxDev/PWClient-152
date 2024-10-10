
#include "HalfSpace.h"
#include <assert.h>

namespace CHBasedCD
{
//////////////////////////////////////////////////////////////////////
// 修改于04-10-19
// 将halfspace的距离阈值改为一个静态成员，从而可以支持对不同阈值的需求
// #define DISTTHRESH 0.000001f				//距离差的阈值，作为判断是在半空间内部or边界上
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// static members 
//////////////////////////////////////////////////////////////////////
float CHalfSpace::DistThresh=HS_DEFAULT_DISTTHRESH;			//显示初始化静态成员

/*
void CHalfSpace::SetDistThresh(float DThresh)
{
	
}
*/

//////////////////////////////////////////////////////////////////////
// Definitions of Members of CHalfSpace
//////////////////////////////////////////////////////////////////////
void CHalfSpace::Set(const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& v3)
{
	m_vNormal=CrossProduct(v2-v1,v3-v1);
	m_vNormal.Normalize();
	m_d=DotProduct(v1,m_vNormal);
}

void CHalfSpace::Set(const A3DVECTOR3& v1, const A3DVECTOR3& v2)
{
	m_vNormal=CrossProduct(v2-v1,A3DVECTOR3(0.0f,0.0f,1.0f));
	m_vNormal.Normalize();
	m_d=DotProduct(v1,m_vNormal);
}

///////////////////////////////////////////////////
// 仅v1在平面上，返回1
// 仅v2在平面上，返回2
// v1,v2都在平面上，返回3
// 相交于第三点，返回4
// 无交点，返回0
///////////////////////////////////////////////////
int CHalfSpace::IntersectLineSeg(const A3DVECTOR3& v1,const A3DVECTOR3& v2,A3DVECTOR3& vIntersection) const
{

	vIntersection=A3DVECTOR3(0.0f,0.0f,0.0f);
	//先处理在平面上的情况
	bool v1InPlane=OnPlane(v1);
	bool v2InPlane=OnPlane(v2);
	if(v1InPlane && v2InPlane) return 3;
	if(v1InPlane) return 1;
	if(v2InPlane) return 2;

	if(Outside(v1)^Outside(v2))
	{
		//两顶点分别在两侧，计算交点
		A3DVECTOR3 vd=v2-v1;
		float t=m_d-DotProduct(m_vNormal,v1);
		t/=DotProduct(m_vNormal,vd);
		assert(t<1.0f && t>0.0f);
		vIntersection=v1+t*vd;

		return 4;
	}
	else
		return 0;

}

void CHalfSpace::Transform(const A3DMATRIX4& mtxTrans)
{
	//从mtxTrans分解出Scale,Rotate,Translate分量
	A3DVECTOR3 vTranslate=mtxTrans.GetRow(3);
	float fScale=mtxTrans.GetCol(0).Magnitude();
	
	// 与mtxTrans一样，仍定义为右乘矩阵
	A3DMATRIX3 mtx3Rotate;		
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			mtx3Rotate.m[i][j]=mtxTrans.m[i][j]/fScale;
	
	//对于平面方程N.X=D的变换(s,R,t)如下：

	// N'=NR
	SetNormal(m_vNormal*mtx3Rotate);

	//D'=s*D+N'.t
	SetD(fScale*m_d+DotProduct(m_vNormal,vTranslate));
}

void CHalfSpace::SetNV(const A3DVECTOR3& n, const A3DVECTOR3& v)
{
	assert(!n.IsZero());

	m_vNormal=n;
	m_vNormal.Normalize();
	m_d=DotProduct(m_vNormal,v);
}

}	// end namespace
