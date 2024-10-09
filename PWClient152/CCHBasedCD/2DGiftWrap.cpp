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

	//如果以前已经分配数据，先释放
	if(m_pVertexes)	delete [] m_pVertexes;
	
	CConvexHullAlgorithm::SetVertexes(pVertexes,vNum);

	//将顶点数据拷贝到内部
	m_pVertexes=new A3DVECTOR3[vNum];
	memcpy(m_pVertexes,pVertexes,vNum*sizeof(A3DVECTOR3));

	//这里如果不进行清零，则可以实现对任意平面上点集的凸包计算

	//置顶点的Y坐标为0.0f;
	for(int i=0;i<vNum;i++)
		m_pVertexes[i].y=0.0f;
	

}

void C2DGiftWrap::SetVertexes(float* Xs,float* Zs,int vNum)
{
	//如果以前已经分配数据，先释放
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
	//设置HalfSpace的共面距离阈值	
	float hsDistThresh=1e-1f;
		
	//保证没有异常的求出凸包！
	do{
		hsDistThresh*=0.1f;							//放大阈值10倍
		CHalfSpace::SetDistThresh(hsDistThresh);	//调整阈值

		//先调用父类函数从而完成清空操作
		CConvexHullAlgorithm::ComputeConvexHull();
		ResetSameVertices();

		//寻找第一条边
		Edge e=SearchFirstEdge();
		m_eFirst=e;

		//由于都是共面因此，不需要作压栈等处理了！
		
		//处理该边
		DealE(e);

	}while(m_bExceptionOccur && hsDistThresh>5e-7);

	if(hsDistThresh<5e-7f)
	{
		//说明已经超出了阈值范围，如果还有异常发生
		//则说明确实该模型的凸壳计算存在异常！
		m_bExceptionOccur=true;
	}

	//计算完后，m_Planes中保存了2D凸包的顶点连接信息
	
	//清空边栈
	m_EdgeStack.Clear();
	
	CHalfSpace::SetDistThresh();  //恢复到缺省的阈值

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