// ConvexHullAlgorithm.cpp: implementation of the CConvexHullAlgorithm class.
//
//////////////////////////////////////////////////////////////////////

#include "ConvexHullAlgorithm.h"

namespace CHBasedCD
{


//�ñ��Ƿ������������f��
bool Edge::InFace(Face f)
{
	return ((start==f.v1 || start==f.v2 || start==f.v3 ) &&
			(end==f.v1 || end==f.v2 || end==f.v3 ));

}

bool Face::operator ==(Face f)
{
	//���㷨��ǰ����f������vһ����ͬ��

	return ((v1==f.v1 || v1==f.v2 || v1==f.v3 ) &&
			(v2==f.v1 || v2==f.v2 || v2==f.v3 ) &&
			(v3==f.v1 || v3==f.v2 || v3==f.v3 ) );
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConvexHullAlgorithm::CConvexHullAlgorithm()
{
	m_pbVInvalid=NULL;
	m_pbExtremeVertex=NULL;

	m_vCentroid.Clear();			//��ʼ������

}

CConvexHullAlgorithm::~CConvexHullAlgorithm()
{
	if(m_pbVInvalid) delete [] m_pbVInvalid;

	if(m_pbExtremeVertex) delete [] m_pbExtremeVertex;

	for(int i=0;i<m_Planes.GetSize();i++)
		if(m_Planes[i])
			delete m_Planes[i];

}

//revised by Wenfeng, 04-12-01
//Add a parameter **bool bTranslate**
//��ʾ�Ƿ��ڶ��㸳ֵʱ��Ҫ��һ��ƽ�ƣ�����ǣ�����Ѷ���ƽ�����㼯����Ϊԭ�㴦��
//Ҫע����ǣ�����GiftWrap�ڲ���û�����ݣ����ƽ�Ʋ�������ı�ԭʼ�����ݡ�
void CConvexHullAlgorithm::SetVertexes(A3DVECTOR3* pVertexes,int vNum, bool bTranslate) 
{
	m_pVertexes=pVertexes;
	m_nVNum=vNum;
	
	m_vCentroid.Clear();			//��ʼ������

	if(bTranslate)
	{

		int i(0);
		for(i=0;i<m_nVNum;i++)
			m_vCentroid+=m_pVertexes[i];
		m_vCentroid/=(float)m_nVNum;
		
		//��������
		for(i=0;i<m_nVNum;i++)
			m_pVertexes[i]-=m_vCentroid;		
	}
	

	//��ʼ��͹�Ǳ߽�㼯״̬
	m_pbVInvalid=new bool[vNum];
	m_pbExtremeVertex=new bool[vNum];

	for(int i=0;i<vNum;i++)
	{
		m_pbVInvalid[i]=false;
		m_pbExtremeVertex[i]=false;
	}
}

void CConvexHullAlgorithm::ComputeConvexHull()
{
	//����� Faces�б�
	m_Faces.RemoveAll();
}

//�ж�f�Ƿ��Ѿ��д�����͹�ǵ��������б�����
bool CConvexHullAlgorithm::IsFaceInCH(Face f)
{
	int i;
	/*
	for(i=0;i<m_Faces.GetSize();i++)
		if(m_Faces[i]==f)
			return true;
	//*/

	//Ȼ���������Ƿ�����Ѿ����ڹ����ƽ������
	for(i=0;i<m_Planes.GetSize();i++)
		if(IsVInVSets(f.v1,m_Planes[i]) && 
		   IsVInVSets(f.v2,m_Planes[i]) && 
		   IsVInVSets(f.v3,m_Planes[i]))
			return true;
	
	return false;

}

//�ж�һ���Ƿ��ڵ㼯��
bool CConvexHullAlgorithm::IsVInVSets(int vID,AArray<int,int>* pVSets)
{
	for(int i=0;i<pVSets->GetSize();i++)
		if(vID==pVSets->GetAt(i))
			return true;
	return false;
}

//����͹���㷨������״̬
void CConvexHullAlgorithm::Reset()
{
	m_Faces.RemoveAll();
	if(m_pbVInvalid) delete [] m_pbVInvalid;
	if(m_pbExtremeVertex) delete [] m_pbExtremeVertex;

	for(int i=0;i<m_Planes.GetSize();i++)
		if(m_Planes[i])
			delete m_Planes[i];
	m_Planes.RemoveAll();

	m_pbVInvalid=NULL;
	m_pbExtremeVertex=NULL;

	
}

//�жϱ�e�Ƿ��Ѿ������ڵ�ǰ�������͹�ǵ��������б��ƽ���б�����
bool CConvexHullAlgorithm::IsEdgeInCH(Edge e)
{
	int i;
	for(i=0;i<m_Faces.GetSize();i++)
		if(e.InFace(m_Faces[i]))
			return true;
	

	//Ȼ���������Ƿ�����Ѿ����ڹ����ƽ������
	for(i=0;i<m_Planes.GetSize();i++)
		if(IsVInVSets(e.start,m_Planes[i]) && 
		   IsVInVSets(e.end,m_Planes[i]))
			return true;
	
	return false;
}

}	// end namespace