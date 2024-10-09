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
	
	m_nCurVNum=2*pCHVs->GetSize();		//当前顶点数量为2D凸包的顶点数量的2倍

	VertexInfo vInfo;
	vInfo.cDegree=3;			//直棱拄所有顶点的面度数均为3

	//构造顶点及顶点信息
	int i(0);
	for(i=0;i<pCHVs->GetSize();i++)
	{
		//同时添加底面和顶面的一个顶点
		A3DVECTOR3 v=gw2D.m_pVertexes[pCHVs->GetAt(i)];
		v.y=0.0f;
		m_arrVertecies.Add(v);
		
		v.y=fHeight;
		m_arrVertecies.Add(v);

		m_arrVertexInfo.Add(vInfo);
		m_arrVertexInfo.Add(vInfo);
	}
	
	//开始构造平面面片

	//底面
	//取前三个点构造平面几何信息
	A3DVECTOR3 v1(gw2D.m_pVertexes[pCHVs->GetAt(0)]),v2(gw2D.m_pVertexes[pCHVs->GetAt(1)]),v3(gw2D.m_pVertexes[pCHVs->GetAt(2)]);
	CPatch* pPatch=new CPatch(this);
	pPatch->Set(v1,v2,v3);			//几何信息			
	
	//依次向Neighbors中添加元素
	VPNeighbor vpn;
	AArray<VPNeighbor,VPNeighbor>& arrNeighbors1=pPatch->GetNeighbors();
	for(i=0;i<pCHVs->GetSize();i++)
	{
		vpn.vid=2*i;			
		arrNeighbors1.Add(vpn);
	}
	//添加到链表
	m_listPatches.AddTail(pPatch);	
	
	//顶面
	//取前三个点，并按照逆序构造平面几何信息
	v1=m_arrVertecies[1];
	v2=m_arrVertecies[3];
	v3=m_arrVertecies[5];

	pPatch=new CPatch(this);
	pPatch->Set(v3,v2,v1);			//几何信息			
	
	//依次向Neighbors中添加元素
	AArray<VPNeighbor,VPNeighbor>& arrNeighbors2=pPatch->GetNeighbors();
	//顶面按照逆序添加
	for(i=pCHVs->GetSize()-1;i>=0;i--)
	{
		vpn.vid=2*i+1;			
		arrNeighbors2.Add(vpn);
	}
	//添加到链表
	m_listPatches.AddTail(pPatch);	
	
	//开始添加各个侧面
	for(i=0;i<pCHVs->GetSize();i++)
	{
		pPatch=new CPatch(this);		
		AArray<VPNeighbor,VPNeighbor>& arrNeighbors=pPatch->GetNeighbors();
		
		//每个侧面都是一个矩形
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
			//最后一个矩形的情况比较特殊
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
	//初始化删除误差

	//这里应考虑将误差的基置为距离最大的两顶点的距离！

	if(m_fArrRemovedError)
		delete[] m_fArrRemovedError;
	m_fArrRemovedError=new float[m_nOriginPatchNum+1];
	for(i=0;i<4;i++)
		m_fArrRemovedError[i]=-1.0f;			//0,1,2,3都置为无效值

	m_bExceptionOccur=false;
	//*/

	//计算每个patch的邻域patch
	ComputePatchNeighbors();

	//寻找最小删除误差对应的面片
	SearchLeastErrorPatch();	
	
	//如果需要简化则加上下面的代码！
	//ReduceAll();
	//Goto(9);

	return true;
}

void CRightPrism::Export2DObstruct(C2DObstruct* p2DObstruct)
{
	//由于本类支持简化操作，因此真正有效的数据仅为底面片的顶点
	//在初始化本类时，底面片为插入m_listPatches的第一个面片
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch=m_listPatches.GetNext(CurPatchPos);
	vector<A3DVECTOR3> Vertices(pCurPatch->GetVNum(), A3DVECTOR3(0.0f, 0.0f, 0.0f));
	for(int i=0;i<pCurPatch->GetVNum();i++)
		Vertices[i]=pCurPatch->GetVertex(i);

	p2DObstruct->Init(Vertices);
		
}


}	// end namespace