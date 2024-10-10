// RightPrism.cpp: implementation of the CRightPrism class.
//
//////////////////////////////////////////////////////////////////////

#include "RightPrism.h"
#include "2DGiftWrap.h"
#include "2DObstruct.h"

#include <vector.h>
using namespace abase;

namespace CHBasedCD
{


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRightPrism::CRightPrism()
{

}

CRightPrism::~CRightPrism()
{

}

bool CRightPrism::InitBase(const C2DGiftWrap& gw2D,float fHeight)
{
	if (gw2D.m_bExceptionOccur)
		return false;
	AArray<int,int>* pCHVs=gw2D.GetCHVertecies();

	if (!pCHVs) 
		return false;

	m_vCentroid=gw2D.m_vCentroid;
	
	m_nCurVNum=2*pCHVs->GetSize();		//��ǰ��������Ϊ2D͹���Ķ���������2��

	VertexInfo vInfo;
	vInfo.cDegree=3;			//ֱ�������ж�����������Ϊ3

	//���춥�㼰������Ϣ
	int i(0);
	for(i=0;i<pCHVs->GetSize();i++)
	{
		//ͬʱ��ӵ���Ͷ����һ������
		A3DVECTOR3 v=gw2D.m_pVertexes[pCHVs->GetAt(i)];
		v.y=0.0f;
		m_arrVertecies.Add(v);
		
		v.y=fHeight;
		m_arrVertecies.Add(v);

		m_arrVertexInfo.Add(vInfo);
		m_arrVertexInfo.Add(vInfo);
	}
	
	//��ʼ����ƽ����Ƭ

	//����
	//ȡǰ�����㹹��ƽ�漸����Ϣ
	A3DVECTOR3 v1(gw2D.m_pVertexes[pCHVs->GetAt(0)]),v2(gw2D.m_pVertexes[pCHVs->GetAt(1)]),v3(gw2D.m_pVertexes[pCHVs->GetAt(2)]);
	CPatch* pPatch=new CPatch(this);
	pPatch->Set(v1,v2,v3);			//������Ϣ			
	
	//������Neighbors�����Ԫ��
	VPNeighbor vpn;
	AArray<VPNeighbor,VPNeighbor>& arrNeighbors1=pPatch->GetNeighbors();
	for(i=0;i<pCHVs->GetSize();i++)
	{
		vpn.vid=2*i;			
		arrNeighbors1.Add(vpn);
	}
	//��ӵ�����
	m_listPatches.AddTail(pPatch);	
	
	//����
	//ȡǰ�����㣬������������ƽ�漸����Ϣ
	v1=m_arrVertecies[1];
	v2=m_arrVertecies[3];
	v3=m_arrVertecies[5];

	pPatch=new CPatch(this);
	pPatch->Set(v3,v2,v1);			//������Ϣ			
	
	//������Neighbors�����Ԫ��
	AArray<VPNeighbor,VPNeighbor>& arrNeighbors2=pPatch->GetNeighbors();
	//���水���������
	for(i=pCHVs->GetSize()-1;i>=0;i--)
	{
		vpn.vid=2*i+1;			
		arrNeighbors2.Add(vpn);
	}
	//��ӵ�����
	m_listPatches.AddTail(pPatch);	
	
	//��ʼ��Ӹ�������
	for(i=0;i<pCHVs->GetSize();i++)
	{
		pPatch=new CPatch(this);		
		AArray<VPNeighbor,VPNeighbor>& arrNeighbors=pPatch->GetNeighbors();
		
		//ÿ�����涼��һ������
		if(i<pCHVs->GetSize()-1)
		{
			v1=m_arrVertecies[2*i+2];
			v2=m_arrVertecies[2*i];
			v3=m_arrVertecies[2*i+1];
			pPatch->Set(v1,v2,v3);
			
			vpn.vid=2*i;
			arrNeighbors.Add(vpn);

			vpn.vid=2*i+1;
			arrNeighbors.Add(vpn);

			vpn.vid=2*i+3;
			arrNeighbors.Add(vpn);

			vpn.vid=2*i+2;
			arrNeighbors.Add(vpn);
			
		}
		else		
		{
			//���һ�����ε�����Ƚ�����
			v1=m_arrVertecies[0];
			v2=m_arrVertecies[2*i];
			v3=m_arrVertecies[2*i+1];
			pPatch->Set(v1,v2,v3);
			
			vpn.vid=2*i;
			arrNeighbors.Add(vpn);

			vpn.vid=2*i+1;
			arrNeighbors.Add(vpn);

			vpn.vid=1;
			arrNeighbors.Add(vpn);

			vpn.vid=0;
			arrNeighbors.Add(vpn);			
		}
		m_listPatches.AddTail(pPatch);
	}	

	m_nOriginPatchNum = m_listPatches.GetCount();
	
	//*
	//��ʼ��ɾ�����

	//����Ӧ���ǽ����Ļ���Ϊ��������������ľ��룡

	if(m_fArrRemovedError)
		delete[] m_fArrRemovedError;
	m_fArrRemovedError=new float[m_nOriginPatchNum+1];
	for(i=0;i<4;i++)
		m_fArrRemovedError[i]=-1.0f;			//0,1,2,3����Ϊ��Чֵ

	m_bExceptionOccur=false;
	//*/

	//����ÿ��patch������patch
	ComputePatchNeighbors();

	//Ѱ����Сɾ������Ӧ����Ƭ
	SearchLeastErrorPatch();	
	
	//�����Ҫ�����������Ĵ��룡
	//ReduceAll();
	//Goto(9);

	return true;
}

void CRightPrism::Export2DObstruct(C2DObstruct* p2DObstruct)
{
	//���ڱ���֧�ּ򻯲��������������Ч�����ݽ�Ϊ����Ƭ�Ķ���
	//�ڳ�ʼ������ʱ������ƬΪ����m_listPatches�ĵ�һ����Ƭ
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch=m_listPatches.GetNext(CurPatchPos);
	vector<A3DVECTOR3> Vertices(pCurPatch->GetVNum(), A3DVECTOR3(0.0f, 0.0f, 0.0f));
	for(int i=0;i<pCurPatch->GetVNum();i++)
		Vertices[i]=pCurPatch->GetVertex(i);

	p2DObstruct->Init(Vertices);
		
}


}	// end namespace