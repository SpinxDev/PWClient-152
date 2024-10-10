// ConvexPolytope.cpp: implementation of the CConvexPolytope class.
//
//////////////////////////////////////////////////////////////////////

#include "ConvexPolytope.h"
#include "GiftWrap.h"
#include "ConvexHullData.h"
#include "2DGiftWrap.h"
#include <A3DGDI.h>

#define MAX_ERROR 1e8

#ifndef ABS
	#define ABS(x) (((x)<0)?(-(x)):(x))
#endif

namespace CHBasedCD
{


//初始化静态成员
float CConvexPolytope::Hull2D_Half_Thickness=0.05f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConvexPolytope::CConvexPolytope()
{
	m_pIndices=new WORD[1000*3];						//初始化为一个大值
	m_pLEPIndices=new WORD[60*3];						//最小误差面片的三角形个数*3
	m_pLEPNIndices=new WORD[60*3];						//最小误差面片的邻域面片三角形个数*3

	m_pVerticesForRender=new A3DVECTOR3[2000];		//初始化渲染用到的顶点
	m_nOriginPatchNum=0;
	m_nCurOperator=-1;

	m_fArrRemovedError=NULL;
	m_fErrorBase=1.0f;					//给一个缺省值1.0f，即绝对误差
	m_nMinPatchNum=4;
	m_bExceptionOccur=false;

	m_vCentroid.Clear();			//初始化清零

	CHalfSpace::SetDistThresh();  //将Halfspace恢复到缺省的阈值
}

CConvexPolytope::~CConvexPolytope()
{
	Reset();							//清空

	delete []m_pIndices;				//清空三角形顶点列表
	delete []m_pLEPIndices;				//清空三角形顶点列表
	delete []m_pLEPNIndices;			//清空三角形顶点列表

	delete []m_pVerticesForRender;
	
	if(m_fArrRemovedError) delete []m_fArrRemovedError;

		
}

//使用CGiftWrap中生成的凸包来初始化当前顶点和面片成员
bool CConvexPolytope::Init(const CGiftWrap &gw)
{
	assert(!gw.m_bExceptionOccur);			//没有发生异常
 
	Reset();								//重置为初始状态

	m_vCentroid=gw.m_vCentroid;				//质心置为初始化gw的质心

	int i(0);
	//分情况考虑构造CConvexPolytope
	if(gw.GetHullType()==CGiftWrap::HULL_3D)	//3D Hull的情况	
	{
		int* Map=new int[gw.m_nVNum];		//构造一个映射表
		int count=0;
		
		//添加有效顶点&构造映射表
		for(i=0;i<gw.m_nVNum;i++)
		{
			Map[i]=-1;			
			if(gw.m_pbExtremeVertex[i])
			{
				//当前点有效，用其初始化v，并将其插入到顶点列表
				A3DVECTOR3 v(gw.m_pVertexes[i]);	
				m_arrVertecies.Add(v);
				m_arrVertexInfo.Add(VertexInfo());
				Map[i]=count++;
			}
		}

		//构造面片
		//添加三角形面片
		for(i=0;i<gw.m_Faces.GetSize();i++)
		{
			Face f=gw.m_Faces[i];
			if(!f.InPolygon)		//不属于任何多边形，添加
			{
				A3DVECTOR3 v1(gw.m_pVertexes[f.v1]),v2(gw.m_pVertexes[f.v2]),v3(gw.m_pVertexes[f.v3]);
				CPatch* pPatch=new CPatch(this);
				pPatch->Set(v1,v2,v3);			//几何信息
				
				//依次向Neighbors中添加元素
				VPNeighbor vpn;
				AArray<VPNeighbor,VPNeighbor>& arrNeighbors=pPatch->GetNeighbors();
				
				vpn.vid=Map[f.v1];				//这里必须作一个映射
				assert(vpn.vid>=0);
				arrNeighbors.Add(vpn);

				vpn.vid=Map[f.v2];
				assert(vpn.vid>=0);
				arrNeighbors.Add(vpn);

				vpn.vid=Map[f.v3];
				assert(vpn.vid>=0);
				arrNeighbors.Add(vpn);

				//各顶点度数增1
				m_arrVertexInfo[Map[f.v1]].cDegree++;
				m_arrVertexInfo[Map[f.v2]].cDegree++;
				m_arrVertexInfo[Map[f.v3]].cDegree++;

				//添加到链表
				m_listPatches.AddTail(pPatch);
			}
		}

		//添加多边形面片
		for(i=0;i<gw.m_Planes.GetSize();i++)
		{
			AArray<int,int>* pPlane=gw.m_Planes[i];
			
			//取前三个点构造平面几何信息
			A3DVECTOR3 v1(gw.m_pVertexes[pPlane->GetAt(0)]),v2(gw.m_pVertexes[pPlane->GetAt(1)]),v3(gw.m_pVertexes[pPlane->GetAt(2)]);
			CPatch* pPatch=new CPatch(this);
			pPatch->Set(v1,v2,v3);			//几何信息			

			//依次向Neighbors中添加元素
			VPNeighbor vpn;
			AArray<VPNeighbor,VPNeighbor>& arrNeighbors=pPatch->GetNeighbors();

			for(int j=0;j<pPlane->GetSize();j++)
			{
				vpn.vid=Map[pPlane->GetAt(j)];			//这里必须作一个映射
				arrNeighbors.Add(vpn);
				
				m_arrVertexInfo[vpn.vid].cDegree++;		//顶点度数增1
			}
			
			//添加到链表
			m_listPatches.AddTail(pPatch);
		}
		
		delete [] Map;

	}
	else
	{

		//说明是2D Hull的情况
		AArray<int,int>* pCHVs=gw.GetCHVertecies();
		if (!pCHVs) 
			return false;	
		if(pCHVs->GetSize()<3)		//至少是一个三角形
			return false;

		//顶点信息
		VertexInfo vInfo;
		vInfo.cDegree=3;			//直棱拄所有顶点的面度数均为3

		CHalfSpace PlaneOut,PlaneIn;

		//取前三个点构造平面几何信息
		A3DVECTOR3 v1(gw.m_pVertexes[pCHVs->GetAt(0)]),v2(gw.m_pVertexes[pCHVs->GetAt(1)]),v3(gw.m_pVertexes[pCHVs->GetAt(2)]);
		
		//构造两个平面PlaneOut和PlaneIn，分别表示顶面和底面
		PlaneOut.Set(v1,v2,v3);			
		
		PlaneIn.SetNormal(-PlaneOut.GetNormal());
		PlaneIn.SetD(-PlaneOut.GetDist());
		
		PlaneIn.Translate(Hull2D_Half_Thickness);
		PlaneOut.Translate(Hull2D_Half_Thickness);
		
		A3DVECTOR3 vOutNormal=PlaneOut.GetNormal();
		A3DVECTOR3 vInNormal=PlaneIn.GetNormal();
		
		//分别求出PlaneOut,PlaneIn上的两点
		A3DVECTOR3 vOut=v1+Hull2D_Half_Thickness*vOutNormal;
		A3DVECTOR3 vIn=v1+Hull2D_Half_Thickness*vInNormal;

		//构造顶点及顶点信息
		for(i=0;i<pCHVs->GetSize();i++)
		{
			//同时添加底面和顶面的一个顶点
			A3DVECTOR3 vec1=gw.m_pVertexes[pCHVs->GetAt(i)];
			A3DVECTOR3 vec2=vec1;
			if(i<3)
			{
				vec1+=Hull2D_Half_Thickness*vOutNormal;
				vec2+=Hull2D_Half_Thickness*vInNormal;
			}
			else
			{
				A3DVECTOR3 vDiff=vec1-vOut;
				vec1-=DotProduct(vDiff,vOutNormal)*vOutNormal;

				vDiff=vec2-vIn;
				vec2-=DotProduct(vDiff,vInNormal)*vInNormal;
			}

			m_arrVertecies.Add(vec1);
			m_arrVertecies.Add(vec2);

			//相应的，添加两个顶点信息
			m_arrVertexInfo.Add(vInfo);
			m_arrVertexInfo.Add(vInfo);
		}
	
		//开始构造平面面片

		//向外的面
		CPatch* pPatch=new CPatch(this);
		pPatch->SetNormal(vOutNormal);			//几何信息
		pPatch->SetD(PlaneOut.GetDist());
		
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
		
		//向内的面

		pPatch=new CPatch(this);
		pPatch->SetNormal(vInNormal);			//几何信息
		pPatch->SetD(PlaneIn.GetDist());
		
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
			

	}

	
	m_nOriginPatchNum = m_listPatches.GetCount();
	m_nCurVNum=m_arrVertecies.GetSize();

	//初始化删除误差
	if(m_fArrRemovedError)
		delete[] m_fArrRemovedError;
	m_fArrRemovedError=new float[m_nOriginPatchNum+1];
	for(i=0;i<m_nOriginPatchNum+1;i++)
		m_fArrRemovedError[i]=-1.0f;			//0,1,2,3都置为无效值
	
	m_bExceptionOccur=false;

	//计算每个patch的邻域patch
	ComputePatchNeighbors();
 
	//CHalfSpace::SetDistThresh(1e-3);  //恢复到缺省的阈值

	//寻找最小删除误差对应的面片
	SearchLeastErrorPatch();		

	//开始简化，简化到头
	ReduceAll();

	return true;
}

void CConvexPolytope::ComputePatchNeighbors()
{
	//一个两重循环遍历
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	ALISTPOSITION TraPatchPos;
	CPatch *pCurPatch,*pTraPatch;

	for(int i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);
		TraPatchPos=m_listPatches.GetHeadPosition();
		for(int j=0;j<m_listPatches.GetCount();j++)
		{
			pTraPatch=m_listPatches.GetNext(TraPatchPos);
			if(i!=j)
				pCurPatch->Neighbor(pTraPatch);
		}
		//在这里添加完当前Patch的所有邻域面片后，
		//就可以计算其删除误差了
		pCurPatch->UpdateRemovedError();
	}
}

void CConvexPolytope::UndoRemove()
{
	if(m_nCurOperator==-1)			//已经回溯到最初始的状态了，不能继续回溯了
		return;
	
	int RemOprCount=m_arrRemoveOperatorRecords.GetSize();
	RemoveOperatorRecord& ror=*(m_arrRemoveOperatorRecords[m_nCurOperator]); 
	
	//插入当前操作记录中删除的面片
	m_listPatches.AddTail(ror.pRemoved);
	ror.pRemoved->IncreVDegree();			//顶点度数都增1

	//修改当前顶点数量
	m_nCurVNum-=ror.VNumAdded;

	CPatch *pCur,*pBak;
	CPatch patchTemp(this);
	int nNBNum=ror.arrNeighborBak.GetSize();
	//ALISTPOSITION lpos;
	for(int i=0;i<nNBNum;i++)
	{
		//for debug
		int vid=ror.pRemoved->GetNeighbors()[i].vid;
		A3DVECTOR3 v=m_arrVertecies[vid];
		VertexInfo vInfo=m_arrVertexInfo[vid];


		pCur=ror.arrNeighborBak[i].pPatchCur;
		pBak=ror.arrNeighborBak[i].pPatchBak;


		patchTemp=*pCur;
		*pCur=*pBak;
		*pBak=patchTemp;

	}

	m_nCurOperator--;						//前移一步

	m_pCurLeastErrorPatch=ror.pRemoved;		//修改当前误差最小面片

}

void CConvexPolytope::RedoRemove()
{
	int RemOprCount=m_arrRemoveOperatorRecords.GetSize();
	if(m_nCurOperator==RemOprCount-1)	//已经是最后一个操作了，不能再往前恢复了
		return;

	RemoveOperatorRecord& ror=*(m_arrRemoveOperatorRecords[m_nCurOperator+1]); 
	
	//再一次删除当前操作记录中删除的面片，因为前面的恢复过程已经将其插入了
	//注意：该面片应该是被插入在整个listpatch的最后！因为前面的恢复操作，将其插入到尾部了。
	//这样，就可以避免再作一次遍历寻找！
	CPatch* ptail=m_listPatches.GetTail();
	assert(ptail==ror.pRemoved);			//这里再作一下验证！
	m_listPatches.RemoveTail();
	ror.pRemoved->DecreVDegree();			//顶点度数都减1
	
	//修改当前顶点数量
	m_nCurVNum+=ror.VNumAdded;

	CPatch *pCur,*pBak;
	CPatch patchTemp(this);
	int nNBNum=ror.arrNeighborBak.GetSize();
	for(int i=0;i<nNBNum;i++)
	{

		pCur=ror.arrNeighborBak[i].pPatchCur;
		pBak=ror.arrNeighborBak[i].pPatchBak;

		patchTemp=*pCur;
		*pCur=*pBak;
		*pBak=patchTemp;
	}

	m_nCurOperator++;		//前移一步
	
	if(m_nCurOperator==RemOprCount-1)		//后移一步后变成了最后一个操作
		SearchLeastErrorPatch();			//此时要重新计算最小误差面片
	else
		m_pCurLeastErrorPatch=m_arrRemoveOperatorRecords[m_nCurOperator+1]->pRemoved;		//修改当前误差最小面片
}

void CConvexPolytope::Reset()
{
	m_arrVertecies.RemoveAll();
	m_arrVertexInfo.RemoveAll();
	
	//m_fErrorBase=1.0f;					//给一个缺省值1.0f

	//清空ListPatch
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch* pCurPatch;
	int i(0);
	for(i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);
		delete pCurPatch;
	}
	m_listPatches.RemoveAll();		
	
	//清空RemoveOperatorRecords
	for(i=0;i<m_arrRemoveOperatorRecords.GetSize();i++)
	{
		RemoveOperatorRecord* pror=m_arrRemoveOperatorRecords[i];
		RemoveOperatorRecord& ror=*pror;
		for(int j=0;j<ror.arrNeighborBak.GetSize();j++)
			delete ror.arrNeighborBak[j].pPatchBak;		//删除他们的备份指针

		if(i<=m_nCurOperator)		//在m_nCurOperator之前的操作
		{
			//被删除的面片指针实际上并未删除，同时也不在m_listPatches中，因此，这里要删除！
			delete ror.pRemoved;
		}
		else
		{

		}
		
		delete pror;		//删除自身
		
	}
	m_nCurOperator=-1;
	m_arrRemoveOperatorRecords.RemoveAll();
}

//////////////////////////////////////////////////////////////
// 删除面片pPatch
// 首先要将删除前的现场保存好，这样就可以支持撤销删除操作了
// 保存后，直接调用pPatch自己的被删除方法
//////////////////////////////////////////////////////////////
bool CConvexPolytope::RemovePatch(CPatch *pPatch)
{
	//return pPatch->Removed();
	
	//*
	RemoveOperatorRecord* pror=new RemoveOperatorRecord;
	RemoveOperatorRecord& ror=*pror;			//当前的删除操作记录
	ror.pRemoved=pPatch;

	//将邻域面片复制并保存好
	CPatch *pNPBak,*pCurNP;
	AArray<VPNeighbor,VPNeighbor>& Neighbors=pPatch->GetNeighbors();
	int iNBSize=Neighbors.GetSize();
	int i(0);
	for(i=0;i<iNBSize;i++)
	{
		//for debug
		int vid=Neighbors[i].vid;
		A3DVECTOR3 v=m_arrVertecies[vid];
		VertexInfo vInfo=m_arrVertexInfo[vid];

		pCurNP=Neighbors[i].pNeighborPatch;
		pNPBak=new CPatch(*pCurNP);				//复制给patchNeighborBak
		
		//由于要考虑撤销和恢复操作，因此这里似乎不能直接添加到最后！
		//而应改为：将m_nCurOperator游标其后的所有删除操作删除
		//然后再添加到最后的位置！
		ror.arrNeighborBak.Add(NeighborBak(pCurNP,pNPBak));
	}
	
	if(	pPatch->Removed() )		//删除成功
	{
		
		//移除当前面片
		ALISTPOSITION pos=m_listPatches.Find(pPatch);
		m_listPatches.RemoveAt(pos);

		//记录下来增加的顶点数
		ror.VNumAdded=m_arrVertecies.GetSize()-m_nCurVNum;
		m_nCurVNum=m_arrVertecies.GetSize();

		//备份操作！
		m_arrRemoveOperatorRecords.Add(&ror);
		m_nCurOperator++;
		return true;
	}
	else						//删除失败
	{
		//释放内存
		for(i=0;i<iNBSize;i++)
		{
			delete ror.arrNeighborBak[i].pPatchBak;
		}
		delete pror;
		return false;
	}
	
	 //*/
}

//利用A3DFlatCollector对自身进行绘制
//最后一个参数表示是否特殊绘制最小误差面片，缺省为true

//04-10-23 重新修改此接口
//定义一个pSpecialPatch，对其及其邻域面片，将进行特殊的绘制处理
void CConvexPolytope::Render(A3DFlatCollector *pFC,A3DMATRIX4& tMatrix, DWORD dwColor,CPatch* pSpecialPatch)
{
	//邻域绘制的色彩
	DWORD dwNCol[]={
					0x8000ff00,
					0x8000ffff,
					0x800000ff,
					0x80ff00ff,
					0x80ffff00,
					0x80007f00,
					0x8000007f
					/*
					0x80000050,
					0x800000a0,
					0x800000ff,
					0x80005050,
					0x800050a0,
					0x800050ff,
					0x8000a050,
					0x8000a0a0,
					0x8000a0ff,
					0x8000ff50,
					0x8000ffa0,
					0x8000ffff
					*/
					};
	
	int colSize=sizeof(dwNCol)/sizeof(dwNCol[0]);

	//顶点
	A3DVECTOR3* pVertices=m_arrVertecies.GetData();
	int vNum=m_arrVertecies.GetSize();
	
	int i,j;	

	
	//用于渲染的顶点，要作一个变换
	for(i=0;i<m_nCurVNum;i++)
	{
		//只渲染有效的顶点，即该顶点的度数必须不小于3
		if(m_arrVertexInfo[i].cDegree<3) continue;
		
		//变换并渲染顶点
		m_pVerticesForRender[i]=pVertices[i]*tMatrix;
		//暂不绘制顶点	
		g_pA3DGDI->Draw3DPoint(m_pVerticesForRender[i],0xff00ff00,3.0f);
	}

	//构造三角形的顶点id列表
	//这个量应使用作用域为整个类的一个一个变量。
	//否则，在渲染时，可能其地址信息出错
	//WORD* pIndices=new WORD(10000);			//先预先分配一个足够大的空间
	int cur=0;								//一个指向当前id的计数器
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch* pCurPatch;
	if(pSpecialPatch)	
	{
		//pSpecialPatch不为空
		//对面片pSpecialPatch及其邻域作一个特殊处理，绘制成不同颜色，从而便于观察效果

		//选择性绘制
		for(i=0;i<m_listPatches.GetCount();i++)
		{
			pCurPatch=m_listPatches.GetNext(CurPatchPos);
			if(pCurPatch!=pSpecialPatch && ! pSpecialPatch->InNeighbors(pCurPatch))
			{
				//不是pSpecialPatch，且不在其邻域中

				AArray<VPNeighbor,VPNeighbor>& Neighbors=pCurPatch->GetNeighbors();
				assert(Neighbors.GetSize()>=3);
				for(j=2;j<Neighbors.GetSize();j++)
				{
					//构造三角形
					m_pIndices[cur]=Neighbors[0].vid;			//v1始终为首元素
					m_pIndices[cur+1]=Neighbors[j-1].vid;		//v2为上一元素
					m_pIndices[cur+2]=Neighbors[j].vid;			//v3为当前元素
					cur+=3;
				}
			}
		}

		//绘制
		pFC->AddRenderData_3D(m_pVerticesForRender,vNum,m_pIndices,cur,dwColor);
		
		//绘制最小误差面片及其邻域
		cur=0;
		int ncur=0;			//邻域的三角形面片计数器
		AArray<VPNeighbor,VPNeighbor>& Neighbors=pSpecialPatch->GetNeighbors();
		for(j=0;j<Neighbors.GetSize();j++)
		{
			if(j>=2)
			{
				//构造自身三角形
				m_pLEPIndices[cur]=Neighbors[0].vid;			//v1始终为首元素
				m_pLEPIndices[cur+1]=Neighbors[j-1].vid;		//v2为上一元素
				m_pLEPIndices[cur+2]=Neighbors[j].vid;		//v3为当前元素
				cur+=3;
			}

			//构造邻域的三角形
			pCurPatch=Neighbors[j].pNeighborPatch;
			AArray<VPNeighbor,VPNeighbor>& NN=pCurPatch->GetNeighbors();	//邻域的邻域描述
			for(i=2;i<NN.GetSize();i++)				//从2开始循环
			{
				//构造三角形
				m_pLEPNIndices[ncur]=NN[0].vid;			//v1始终为首元素
				m_pLEPNIndices[ncur+1]=NN[i-1].vid;		//v2为上一元素
				m_pLEPNIndices[ncur+2]=NN[i].vid;		//v3为当前元素
				ncur+=3;			
			}
			//绘制当前邻域
			DWORD curCol=dwNCol[j%colSize];
			pFC->AddRenderData_3D(m_pVerticesForRender,vNum,m_pLEPNIndices,ncur,curCol);	//蓝色渲染
			ncur=0;
		}
		pFC->AddRenderData_3D(m_pVerticesForRender,vNum,m_pLEPIndices,cur,0x80ff0000);		//红色渲染
		


	}
	else
	{
		//pSpecialPatch为空！
		//全部绘制！
		for(i=0;i<m_listPatches.GetCount();i++)
		{
			pCurPatch=m_listPatches.GetNext(CurPatchPos);
			//不是最小误差面片，且不在其邻域中

			AArray<VPNeighbor,VPNeighbor>& Neighbors=pCurPatch->GetNeighbors();
			assert(Neighbors.GetSize()>=3);
			for(j=2;j<Neighbors.GetSize();j++)
			{
				
				//构造三角形
				m_pIndices[cur]=Neighbors[0].vid;			//v1始终为首元素
				m_pIndices[cur+1]=Neighbors[j-1].vid;		//v2为上一元素
				m_pIndices[cur+2]=Neighbors[j].vid;		//v3为当前元素
				cur+=3;
			}

		}
		//绘制
		pFC->AddRenderData_3D(m_pVerticesForRender,vNum,m_pIndices,cur,dwColor);	

	}

	pFC->Flush();
}

//删除一个误差最小的面片，为系统自动调用时使用
bool CConvexPolytope::RemoveLeastErrorPatch()
{
	//寻找最小删除误差对应的面片
	SearchLeastErrorPatch();
	if(!m_pCurLeastErrorPatch)
		return false;

	return RemovePatch(m_pCurLeastErrorPatch);
	
	/*
	//新策略仍有问题！wf于04-12-2
	//采用新的策略，如果当前最小误差面片删除失败，则重新寻找继续删除！
	
	while(!RemovePatch(m_pCurLeastErrorPatch))				
	{
		if(SearchLeastErrorPatch()==NULL)		//继续找一个最小删除面片
			return false;						//无法找到		
	}
	//删除该面片成功
	return true;
	//*/
	
}

CPatch* CConvexPolytope::SearchLeastErrorPatch()
{
	int PatchNum=m_listPatches.GetCount();
	if(PatchNum<=4)
	{
		//如果多面体的面数小于等于4，则不在进行简化了！
		m_pCurLeastErrorPatch= NULL;
		return NULL;			
	}


	//查找误差最小的patch
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch,*pLeastErrorPatch;
	float LeastError=MAX_ERROR;			//初始化为一个大值
	for(int i=0;i<PatchNum;i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);
		float error=pCurPatch->GetRemovedError();
		if(error!=-1.0f && error<LeastError)
		{
			LeastError=pCurPatch->GetRemovedError();
			pLeastErrorPatch=pCurPatch;
		}
	}

	
	if(LeastError!=MAX_ERROR)			//该误差应该为大于等于0
		m_pCurLeastErrorPatch=pLeastErrorPatch;
		
	else	//说明没有最小误差，不能再删除了
		m_pCurLeastErrorPatch= NULL;

	return m_pCurLeastErrorPatch;
}

//已经撤销了所有的删除操作吗
//如果没有任何删除操作，也返回true!
bool CConvexPolytope::UndoAll()
{
	//if(m_arrRemoveOperatorRecords.GetSize()==0)
	//	return true;

	if(m_nCurOperator==-1)		//说明撤销到头了
		return true;
	else 
		return false;
}

//已经恢复了所有的删除操作吗
bool CConvexPolytope::RedoAll()
{
	return (m_nCurOperator==m_arrRemoveOperatorRecords.GetSize()-1);
}

// 直接跳到简化后的某一个层次
// 参数由LeftPatchesNum指定：表示剩下的面片数量或简化层次
bool CConvexPolytope::Goto(int LeftPatchesNum)
{
	if(LeftPatchesNum<m_nMinPatchNum || LeftPatchesNum> m_nOriginPatchNum )
		return false;
	int i;
	int RemovedPatchesNum=m_nOriginPatchNum-LeftPatchesNum;
	int UnRemoveTimes=m_nCurOperator+1-RemovedPatchesNum;
	if(UnRemoveTimes>0)
	{
		for(i=0;i<UnRemoveTimes;i++)
			UndoRemove();
	}
	else
	{
		for(i=0;i<-UnRemoveTimes;i++)
			RedoRemove();
	}
	return true;
}

//最小误差面片为特殊面片时的绘制，直接调用Render(...)即可
void CConvexPolytope::RenderLEPatchSpecial(A3DFlatCollector *pFC, A3DMATRIX4 &tMatrix, DWORD dwColor)
{
	Render(pFC,tMatrix,dwColor,m_pCurLeastErrorPatch);
}

void CConvexPolytope::ReduceAll()
{
	int curPatchNum=m_nOriginPatchNum;
	do{
		m_fArrRemovedError[curPatchNum]=GetCurLeastError()/m_fErrorBase;
		curPatchNum--;
	}while(RemoveLeastErrorPatch());
	
	m_nMinPatchNum=++curPatchNum;

	if(curPatchNum<4)		//不可能比4还少！
		m_bExceptionOccur=true;
}

void CConvexPolytope::Goto(float fError)
{
	if(fError<0) return;
	int LeftPatchesNum=GetLPNByError(fError);
	Goto(LeftPatchesNum);
}

int CConvexPolytope::GetLPNByError(float fError)
{
	int i=m_nOriginPatchNum;
	//改为遍历查找  //二分查找
	while(m_fArrRemovedError[i]<fError && i > m_nMinPatchNum)
		i--;
	/*
		i>>=1;		//等价于i/=2
	while(m_fArrRemovedError[i]>fError)
		i++;
	*/
	return i;
}

//将简化好的数据导出到CConvexHullData对象中
void CConvexPolytope::ExportCHData(CConvexHullData* pCHData)
{
	AArray<int,int> Map;		//一个由顶点在本类中id到CConvexHullData中id的一个映射表；
	
	//遍历每一个Patch
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch;
	for(int i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);
		A3DVECTOR3 n=pCurPatch->GetNormal();
		CFace face;
		face.SetNormal(n);
		face.SetD(pCurPatch->GetDist());
		
		CHalfSpace hs;
		for(int j=0;j<pCurPatch->GetVNum();j++)
		{
			int vid=pCurPatch->GetVID(j);
			
			//构造垂直于该边法向朝外的HalfSpace
			A3DVECTOR3 v1,v2;
			pCurPatch->GetEdge(j,v1,v2);
			hs.Set(v1,v2,v2+n);
			
			// for debug, by wenfeng, 05-04-08
			float d = face.DistV2Plane(v1);

			int ExistID;
			if((ExistID=FindInArray(vid,Map))==-1)
			{
				//说明是新的顶点

				//插入到CConvexHullData的Vertices中
				pCHData->AddVertex(v1);
				int newID=pCHData->GetVertexNum()-1;	//在pCHData中的id
				face.AddElement(newID,hs);
				
				Map.Add(vid);
			}
			else
			{
				//说明是已经存在的顶点
				face.AddElement(ExistID,hs);
			}
		}

		//插入该面片
		pCHData->AddFace(face);
		
	}
	
	// 计算额外边界半空间
	pCHData->ComputeFaceExtraHS();

}

int FindInArray(int id,const AArray<int,int>& arr)
{
	for(int i=0;i<arr.GetSize();i++)
		if(arr[i]==id)
			return i;
	return -1;
}

//在数组Vertices中插入不重复的点
void AddDifferentV(AArray<A3DVECTOR3,A3DVECTOR3>& Vertices, A3DVECTOR3& v)
{
	A3DVECTOR3 vDiff;
	for(int i=0;i<Vertices.GetSize();i++)
	{
		vDiff=Vertices[i]-v;
		if(vDiff.SquaredMagnitude()<1e-6f)			//如果两者相等说明有和v重复的点！因此不作任何操作！
			return;
	}
	Vertices.Add(v);
}

//计算凸多面体与平面组相交所得到的交点投影到XOZ平面构成的2D凸包
//要求，平面法向不能垂直于Y轴，即平面不能与Y平行，一般的，我们的求交平面都垂直于Y轴
//交点少于2个，否则返回false；
//传出参数：gw2d表明了由这些交点构成的2D凸包投影到了XOZ平面
//通过参数b2ParallelPlanes，表明计算是否为求解两平行平面截得的部分的2D凸包
bool CConvexPolytope::IntersectPlanesProj2XOZ(CHalfSpace* Planes,int PlaneNum,C2DGiftWrap& gw2d,bool b2ParallelPlanes)
{
	A3DVECTOR3 n1,n2;
	if (b2ParallelPlanes) 
	{
		//两平行面片凸包
		if(PlaneNum!=2) return false;
		n1=Planes[0].GetNormal();
		n2=Planes[1].GetNormal();
		if(!(n1==n2 || n1==-n2))
			return false;
	}
	
	if (PlaneNum<1) return false;

	int i,j,k;
	//判断Planes的法向
	for(i=0;i<PlaneNum;i++)
	{
		//平面与Y轴的夹角至少为30 degree
		//该阈值可调
		if (ABS(Planes[i].GetNormal().y)<0.5f) 
		{
			return false;
		}
	}

	//开始求交运算
	AArray<A3DVECTOR3,A3DVECTOR3> Vertices;		//存放交点的顶点集
	//对每一个面片遍历
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch;
	for(i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);
		A3DVECTOR3 v1,v2;
		//逐边进行考虑
		for(k=0;k<pCurPatch->GetVNum();k++)
		{
			pCurPatch->GetEdge(k,v1,v2);
			for(j=0;j<PlaneNum;j++)
			{
				A3DVECTOR3 vI;
				int res=Planes[j].IntersectLineSeg(v1,v2,vI);
				switch(res) {
				case 0:
					break;
				case 1:
					AddDifferentV(Vertices,v1);
					break;
				case 2:
					AddDifferentV(Vertices,v2);
					break;
				case 3:
					AddDifferentV(Vertices,v1);
					AddDifferentV(Vertices,v2);
					break;
				case 4:
					AddDifferentV(Vertices,vI);
					break;
				default:
					break;
				}
			}
		}

	}
	
	if (b2ParallelPlanes) 
	{
		//还需添加本凸多面体在两平面间的顶点
		if(n1==-n2)
			Planes[1].Inverse();	//对P2转向，使得P1,P2法向相同
		
		//遍历所有有效顶点
		for(int i=0;i<m_nCurVNum;i++)
			if(m_arrVertexInfo[i].cDegree>=3) 		//有效顶点
				if(Planes[0].Outside(m_arrVertecies[i]) && Planes[1].Inside(m_arrVertecies[i]) ||
				   Planes[0].Inside(m_arrVertecies[i])  && Planes[1].Outside(m_arrVertecies[i]))
				   Vertices.Add(m_arrVertecies[i]);
	}

	//求交完毕，开始构造2D凸包
	int vNum=Vertices.GetSize();
	if (vNum<3) return false;

	A3DVECTOR3* Vs=new A3DVECTOR3[vNum];
	for(i=0;i<vNum;i++)
		Vs[i]=Vertices[i];

	gw2d.SetVertexes(Vs,vNum);
	gw2d.ComputeConvexHull();
	delete [] Vs;
	return true;
}

//判断顶点是否在凸多面体之外
bool CConvexPolytope::IsVertexOutside(A3DVECTOR3 v,float fInflateRadius)
{
	CHalfSpace hs;
	//对每一个面片遍历
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch;
	for(int i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);		
		hs.SetNormal(pCurPatch->GetNormal());
		hs.SetD(pCurPatch->GetDist()+fInflateRadius);
		if(hs.Outside(v))
			return true;
	}
	return false;
}

//判断顶点是否在凸多面体之内
bool CConvexPolytope::IsVertexInside(A3DVECTOR3 v, float fInflateRadius)
{
	CHalfSpace hs;
	//对每一个面片遍历
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch;
	for(int i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);		
		hs.SetNormal(pCurPatch->GetNormal());
		hs.SetD(pCurPatch->GetDist()+fInflateRadius);
		if(!hs.Inside(v))
			return false;
	}
	return true;
}

//顶点v是否在某个边界平面上？
CPatch* CConvexPolytope::IsVertexOnPatch(A3DVECTOR3 v, float fInflateRadius)
{
	CHalfSpace hs;
	//对每一个面片遍历
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch;
	for(int i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);		
		hs.SetNormal(pCurPatch->GetNormal());
		hs.SetD(pCurPatch->GetDist()+fInflateRadius);
		//wf于04.12.25，这里似乎有问题，因为在平面上不见得一定在有界平面上
		if(hs.OnPlane(v))
			return pCurPatch;
	}
	return NULL;
	
}

A3DAABB CConvexPolytope::GetAABB()
{
	A3DAABB aabb;
	aabb.Clear();
	for(int i=0; i<m_nCurVNum; i++)
	{
		//只考虑度数必须不小于3的顶点
		if(m_arrVertexInfo[i].cDegree<3) continue;
		
		aabb.AddVertex(m_arrVertecies[i]);
	}
	aabb.CompleteCenterExts();
	return aabb;
}

}	// end namespace