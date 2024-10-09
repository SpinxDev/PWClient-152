// 2DGiftWrap.cpp: implementation of the C2DGiftWrap class.
//
//////////////////////////////////////////////////////////////////////

#include "2DGiftWrap.h"
#include "2DObstruct.h"

#include <vector.h>
using namespace abase;

namespace CHBasedCD
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2DGiftWrap::C2DGiftWrap()
{
	m_pVertexes=NULL;
}

C2DGiftWrap::~C2DGiftWrap()
{
	if(m_pVertexes) delete [] m_pVertexes;
}

void C2DGiftWrap::SetVertexes(A3DVECTOR3* pVertexes,int vNum)
{

	//�����ǰ�Ѿ��������ݣ����ͷ�
	if(m_pVertexes)	delete [] m_pVertexes;
	
	CConvexHullAlgorithm::SetVertexes(pVertexes,vNum);

	//���������ݿ������ڲ�
	m_pVertexes=new A3DVECTOR3[vNum];
	memcpy(m_pVertexes,pVertexes,vNum*sizeof(A3DVECTOR3));

	//����������������㣬�����ʵ�ֶ�����ƽ���ϵ㼯��͹������

	//�ö����Y����Ϊ0.0f;
	for(int i=0;i<vNum;i++)
		m_pVertexes[i].y=0.0f;
	

}

void C2DGiftWrap::SetVertexes(float* Xs,float* Zs,int vNum)
{
	//�����ǰ�Ѿ��������ݣ����ͷ�
	if(m_pVertexes)	delete [] m_pVertexes;	

	CConvexHullAlgorithm::SetVertexes(NULL,vNum);
	
	m_pVertexes=new A3DVECTOR3[vNum];
	for(int i=0;i<vNum;i++)
	{
		m_pVertexes[i].x=Xs[i];
		m_pVertexes[i].y=0.0f;
		m_pVertexes[i].z=Zs[i];
	}

}

void C2DGiftWrap::ComputeConvexHull()
{
	//����HalfSpace�Ĺ��������ֵ	
	float hsDistThresh=1e-1f;
		
	//��֤û���쳣�����͹����
	do{
		hsDistThresh*=0.1f;							//�Ŵ���ֵ10��
		CHalfSpace::SetDistThresh(hsDistThresh);	//������ֵ

		//�ȵ��ø��ຯ���Ӷ������ղ���
		CConvexHullAlgorithm::ComputeConvexHull();
		ResetSameVertices();

		//Ѱ�ҵ�һ����
		Edge e=SearchFirstEdge();
		m_eFirst=e;

		//���ڶ��ǹ�����ˣ�����Ҫ��ѹջ�ȴ����ˣ�
		
		//����ñ�
		DealE(e);

	}while(m_bExceptionOccur && hsDistThresh>5e-7);

	if(hsDistThresh<5e-7f)
	{
		//˵���Ѿ���������ֵ��Χ����������쳣����
		//��˵��ȷʵ��ģ�͵�͹�Ǽ�������쳣��
		m_bExceptionOccur=true;
	}

	//�������m_Planes�б�����2D͹���Ķ���������Ϣ
	
	//��ձ�ջ
	m_EdgeStack.Clear();
	
	CHalfSpace::SetDistThresh();  //�ָ���ȱʡ����ֵ

}

void C2DGiftWrap::Export2DObstruct(C2DObstruct* p2DObstruct)
{
	AArray<int,int>* pBaseFaceVIDs=GetCHVertecies();
	if(!pBaseFaceVIDs) return ;

	vector<A3DVECTOR3> Vertices(pBaseFaceVIDs->GetSize(), A3DVECTOR3(0.0f, 0.0f, 0.0f));
	for(int i=0;i<pBaseFaceVIDs->GetSize();i++)
		Vertices[i]=m_pVertexes[pBaseFaceVIDs->GetAt(i)];

	p2DObstruct->Init(Vertices);
	
}


}	// end namespace