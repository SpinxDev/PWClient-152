// ConvexHullAlgorithm.cpp: implementation of the CConvexHullAlgorithm class.
//
//////////////////////////////////////////////////////////////////////

#include "ConvexHullAlgorithm.h"

namespace CHBasedCD
{


//该边是否出现在三角面f中
bool Edge::InFace(Face f)
{
	return ((start==f.v1 || start==f.v2 || start==f.v3 ) &&
			(end==f.v1 || end==f.v2 || end==f.v3 ));

}

bool Face::operator ==(Face f)
{
	//该算法的前提是f的三个v一定不同！

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

	m_vCentroid.Clear();			//初始化清零

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
//表示是否在顶点赋值时需要作一个平移，如果是，将会把顶点平移至点集质心为原点处！
//要注意的是，由于GiftWrap内部并没有数据，这个平移操作将会改变原始的数据。
void CConvexHullAlgorithm::SetVertexes(A3DVECTOR3* pVertexes,int vNum, bool bTranslate) 
{
	m_pVertexes=pVertexes;
	m_nVNum=vNum;
	
	m_vCentroid.Clear();			//初始化清零

	if(bTranslate)
	{

		int i(0);
		for(i=0;i<m_nVNum;i++)
			m_vCentroid+=m_pVertexes[i];
		m_vCentroid/=(float)m_nVNum;
		
		//移至重心
		for(i=0;i<m_nVNum;i++)
			m_pVertexes[i]-=m_vCentroid;		
	}
	

	//初始化凸壳边界点集状态
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
	//先清空 Faces列表！
	m_Faces.RemoveAll();
}

//判断f是否已经有存在于凸壳的三角形列表中了
bool CConvexHullAlgorithm::IsFaceInCH(Face f)
{
	int i;
	/*
	for(i=0;i<m_Faces.GetSize();i++)
		if(m_Faces[i]==f)
			return true;
	//*/

	//然后，再需检查是否该面已经处于共面的平面中了
	for(i=0;i<m_Planes.GetSize();i++)
		if(IsVInVSets(f.v1,m_Planes[i]) && 
		   IsVInVSets(f.v2,m_Planes[i]) && 
		   IsVInVSets(f.v3,m_Planes[i]))
			return true;
	
	return false;

}

//判断一点是否在点集中
bool CConvexHullAlgorithm::IsVInVSets(int vID,AArray<int,int>* pVSets)
{
	for(int i=0;i<pVSets->GetSize();i++)
		if(vID==pVSets->GetAt(i))
			return true;
	return false;
}

//重置凸壳算法的所有状态
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

//判断边e是否已经存在于当前计算过的凸壳的三角形列表和平面列表中了
bool CConvexHullAlgorithm::IsEdgeInCH(Edge e)
{
	int i;
	for(i=0;i<m_Faces.GetSize();i++)
		if(e.InFace(m_Faces[i]))
			return true;
	

	//然后，再需检查是否该面已经处于共面的平面中了
	for(i=0;i<m_Planes.GetSize();i++)
		if(IsVInVSets(e.start,m_Planes[i]) && 
		   IsVInVSets(e.end,m_Planes[i]))
			return true;
	
	return false;
}

}	// end namespace