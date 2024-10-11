// ConvexHullAlgorithm.h: interface for the CConvexHullAlgorithm class.
//
//////////////////////////////////////////////////////////////////////

#ifndef	_CONVEXHULL_ALGORITHM_H_
#define _CONVEXHULL_ALGORITHM_H_

#include <A3DVector.h>
#include <AArray.h>

namespace CHBasedCD
{


//�ö��������ķ�ʽ����ṹ�ߡ���
struct Face;

struct Edge
{
	unsigned int start;
	unsigned int end;

	Edge() {}		//ȱʡ���캯��,do nothing
	Edge(unsigned int v1,unsigned int v2){Set(v1,v2);}
	void Set(unsigned int v1,unsigned int v2){start=v1; end=v2;}
	bool InFace(Face f);
};

struct Face
{
	unsigned int v1;
	unsigned int v2;
	unsigned int v3;
	
	//�����2004-9-20
	//�жϸ��������Ƿ�Ϊͨ������ι����ģ���������͹�ǵ��㷨��
	//��Щ�����εĲ�����Ϊ�����Ļ��ƣ���ʵ������һ����Ķ���ε�һ���֣�
	//���ڱ��Ϊtrue�������Σ����ǲ����ڽ����������CConvexPolytope��patch��
	//������ֱ�ӽ���������polygon���һ�μ���
	bool InPolygon;			

	Face() {}		//ȱʡ���캯��,do nothing
	Face(unsigned int p1,unsigned int p2,unsigned int p3,bool InP=false) {v1=p1;v2=p2;v3=p3;InPolygon=InP;}
	bool operator == (Face f);
};

/////////////////////////////////////////////////
//	created by He wenfeng
//  2004-8-17
//  A base class to compute the convex hull of 3D point set
/////////////////////////////////////////////////
class CConvexHullAlgorithm  
{
public:
	bool IsEdgeInCH(Edge e);
	void Reset();
	bool IsVInVSets(int vID,AArray<int,int>* pVSets);
	bool IsFaceInCH(Face f);
	CConvexHullAlgorithm();
	virtual ~CConvexHullAlgorithm();

	A3DVECTOR3 GetCentroid() { return m_vCentroid;}

//operations
	virtual void SetVertexes(A3DVECTOR3* pVertexes,int vNum,bool bTranslate=false);	//�޸ĳ��麯���������������и���
	AArray<Face,Face>& GetCHFaces() { return m_Faces;}	//���Ͷ��������÷�ʽ����

//virtual operations
	virtual void ComputeConvexHull()=0;		//���飬����������������	

//Attributes
protected:
	A3DVECTOR3				m_vCentroid;	//������������㼯������
	
	A3DVECTOR3 *			m_pVertexes;	//���㼯�ϣ������鷽ʽ��֯��
											//ע��ñ�����Ϊ����ͽӿڵ����ߵ�һ�����ã���������new��delete!

	int						m_nVNum;		//����������
	
	AArray<Face,Face>		m_Faces;		//���ݶ���������͹�ǣ�����Ƭ��ʾ
	
	bool*					m_pbVInvalid;	//��Ӧ��ÿһ�����㣬��־�õ��Ƿ��ڹ��湲�ߵ����������Ϊ��Ч
	
	bool*					m_pbExtremeVertex;		//��Ӧ��ÿһ�����㣬��־�õ��Ƿ�ΪExtreme Vertex!͹���ϵĵ�
	
	//�޸ĺ��������ٶȻ������ԭ����Ȼ��m_planes�������������
	//��ˣ���һЩ��m_planes�в���face��edge�Ĳ�����������ʱ�临�Ӷȣ�
	//�ʴ��޸ķ���������


	//�޸���2004-9-20
	//Ϊ�˱��ڹ�����Ӧ��͹�����壬�����޸�m_Planes��ʹ��������е�ƽ��
	//���������Σ���ǰ�Ķ��岻�������Σ�

	//��Ӧ�����ж��(>3)������������Щƽ�漯������������б���
	AArray<AArray<int,int>*,AArray<int,int>*>		m_Planes;
};


}	// end namespace

#endif // _CONVEXHULL_ALGORITHM_H_