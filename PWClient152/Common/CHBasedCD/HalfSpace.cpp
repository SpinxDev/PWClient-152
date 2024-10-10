
#include "HalfSpace.h"
#include <assert.h>

namespace CHBasedCD
{
//////////////////////////////////////////////////////////////////////
// �޸���04-10-19
// ��halfspace�ľ�����ֵ��Ϊһ����̬��Ա���Ӷ�����֧�ֶԲ�ͬ��ֵ������
// #define DISTTHRESH 0.000001f				//��������ֵ����Ϊ�ж����ڰ�ռ��ڲ�or�߽���
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// static members 
//////////////////////////////////////////////////////////////////////
float CHalfSpace::DistThresh=HS_DEFAULT_DISTTHRESH;			//��ʾ��ʼ����̬��Ա

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
// ��v1��ƽ���ϣ�����1
// ��v2��ƽ���ϣ�����2
// v1,v2����ƽ���ϣ�����3
// �ཻ�ڵ����㣬����4
// �޽��㣬����0
///////////////////////////////////////////////////
int CHalfSpace::IntersectLineSeg(const A3DVECTOR3& v1,const A3DVECTOR3& v2,A3DVECTOR3& vIntersection) const
{

	vIntersection=A3DVECTOR3(0.0f,0.0f,0.0f);
	//�ȴ�����ƽ���ϵ����
	bool v1InPlane=OnPlane(v1);
	bool v2InPlane=OnPlane(v2);
	if(v1InPlane && v2InPlane) return 3;
	if(v1InPlane) return 1;
	if(v2InPlane) return 2;

	if(Outside(v1)^Outside(v2))
	{
		//������ֱ������࣬���㽻��
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
	//��mtxTrans�ֽ��Scale,Rotate,Translate����
	A3DVECTOR3 vTranslate=mtxTrans.GetRow(3);
	float fScale=mtxTrans.GetCol(0).Magnitude();
	
	// ��mtxTransһ�����Զ���Ϊ�ҳ˾���
	A3DMATRIX3 mtx3Rotate;		
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			mtx3Rotate.m[i][j]=mtxTrans.m[i][j]/fScale;
	
	//����ƽ�淽��N.X=D�ı任(s,R,t)���£�

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
