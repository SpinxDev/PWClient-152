// Patch.cpp: implementation of the CPatch class.
//
//////////////////////////////////////////////////////////////////////

#include "Patch.h"
#include "LEquations.h"			//求解线性方程组的头
#include "CombGenerator.h"		//产生组合数的头
#include "ConvexPolytope.h"


#ifdef WF_DEBUG					//调试状态
	#define ASSERT_IN_REMOVE_PATCH(x) assert(x)
	#define ASSERT_IN_UPDATE_ERROR(x) assert(x)
#else							//非调试状态，进入异常的处理
	#define ASSERT_IN_REMOVE_PATCH(x) if(!(x)) goto ExceptionOccur
	#define ASSERT_IN_UPDATE_ERROR(x) if(!(x)) goto Exit
#endif

namespace CHBasedCD
{


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPatch::CPatch(CConvexPolytope* pCP)
{
	m_pConvexPolytope=pCP;
	m_fRemovedError=-1.0f;			//初始化为一个负值！
}

CPatch::~CPatch()
{

}

void CPatch::Neighbor(CPatch *pPatch)
{
	int start,end;
	for(int i=0;i<m_arrNeighbors.GetSize();i++)
	{
		start=m_arrNeighbors[i].vid;
		end=(i==m_arrNeighbors.GetSize()-1)?m_arrNeighbors[0].vid:m_arrNeighbors[i+1].vid;
		if(pPatch->VInPatch(start) && pPatch->VInPatch(end))
			m_arrNeighbors[i].pNeighborPatch=pPatch;
	}

}

bool CPatch::VInPatch(int vid)
{
	for(int i=0;i<m_arrNeighbors.GetSize();i++)
	{
		if(vid==m_arrNeighbors[i].vid)
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
// 注意：
// 本方法仅仅计算删除误差，虽然其计算过程已经产生了新产生的有效顶点，
// 但并不考虑这些新顶点的添加、本面片的删除、及邻域面片的更新等操作
// 在计算中，也没有考虑共面的情况。
//////////////////////////////////////////////////////////////////////
void CPatch::UpdateRemovedError()
{
	// Revised at 10-14 晚
	m_fRemovedError=-1.0f;			//每次更新时，首先初始化为一个负值！
	
	//计算所有邻域的可能交点，找出其中距离该平面最远的点
	//其距离即为该平面的删除误差
	int npCount=m_arrNeighbors.GetSize();
	
	if(npCount<3) return;			//邻域面片数小于3，出现了异常！


	// Added at 10-15 afternoon
	// 防止出现有效交点但拓扑错误的情况
	char* NPValid= new char[npCount];		//记录每一个邻域面片是否有效，即是否经过一个有效交点！
	memset(NPValid,0,npCount);

	int n[3];		//存储选出的三个相邻面片的id
	A3DVECTOR3 v;
	bool bValid;	

	CCombGenerator cg(3,npCount);

	ASSERT_IN_UPDATE_ERROR(cg.IsSolvable());			//确保npCount>=3

	while(!cg.IsOver())
	{
		//产生一个组合
		cg.GetNextComb(n);

		//求解
		if(Solve3NPIntersection(n,v))
		{
			bValid=true;
			
			//如果有解，先代入到当前平面的方程
			//注意：考察当前面片时，则应该是不在当前面片的内部！
			if(this->Inside(v)) 
				continue;			//直接进入下一轮while循环
			
			//继续代入其他的邻域面片方程检验
			for(int i=0;i<npCount;i++)
			{
				if(i!=n[0] && i!=n[1] && i!=n[2])
				{
					//不能在这些面片的外部

					if(m_arrNeighbors[i].pNeighborPatch->Outside(v))
					{
						bValid=false;
						break;		//在某个平面外部，跳出for循环
					}
				}
			} // end of for-loop

			if(bValid)
			{
				//说明该点确实是一个有效的交点，可以计算误差
				// for debug

				float error=OnPlane(v)?0.0f:DistV2Plane(v);			//如果顶点满足OnPlane()，则置距离为0.0f

				if(error>m_fRemovedError) 
					m_fRemovedError=error;

				//将产生该交点的三个面片置为有效
				NPValid[n[0]]=1;
				NPValid[n[1]]=1;
				NPValid[n[2]]=1;
			}
		}//无解的情况暂不考虑
		else
		{
		}
	} // end of while-loop

	//是否所有的邻域面片都有效？
	if(memchr(NPValid,0,npCount))
	{
		m_fRemovedError=-1.0f;		//还能找到无效的面片
	}

Exit:
	delete[] NPValid;
}

//////////////////////////////////////////////////////////////////////
// 求解第n[0,1,2]个邻域面片的交点，结果存入vIntersection
// 无解及其他情况返回false!
//////////////////////////////////////////////////////////////////////
bool CPatch::Solve3NPIntersection(int n[3], A3DVECTOR3 &vIntersection)
{
	int npCount=m_arrNeighbors.GetSize();
	
	//输入无效
	if(n[0]<0 || n[0]>=npCount ||
	   n[1]<0 || n[1]>=npCount ||
	   n[2]<0 || n[2]>=npCount )
	   return false;
	
	//构造线性方程组
	CMatrix	mtxCoef(3,3);		// 系数矩阵
	CMatrix mtxConst(3,1);		// 常数矩阵
	CMatrix	mtxResult(3,1);		// 结果

	for(int i=0;i<3;i++)
	{
		CPatch* pPatch=m_arrNeighbors[n[i]].pNeighborPatch;

		A3DVECTOR3 v(pPatch->GetNormal());
		mtxCoef.SetElement(i,0,v.x);
		mtxCoef.SetElement(i,1,v.y);
		mtxCoef.SetElement(i,2,v.z);
		mtxConst.SetElement(i,0,pPatch->GetDist());
	}
	
	//用高斯全选主元法求解
	CLEquations le(mtxCoef,mtxConst);
	if(!le.GetRootsetGauss(mtxResult))
		return false;
	
	vIntersection.x=(float)mtxResult.GetElement(0,0);
	vIntersection.y=(float)mtxResult.GetElement(1,0);
	vIntersection.z=(float)mtxResult.GetElement(2,0);

	return true;

}

//////////////////////////////////////////////////////////////////////
// 本类最核心的方法： 将本面片从凸多面体中删除！
// 涉及到了很多相关的操作，包括：
// 计算新的顶点、新顶点的添加、本面片的删除、及邻域面片的更新等操作
//////////////////////////////////////////////////////////////////////
bool CPatch::Removed() 
{
	int npCount=m_arrNeighbors.GetSize();
	if(npCount<3) return false;

	//一些重要的数据结构
	AArray<A3DVECTOR3,A3DVECTOR3> VerteciesAdded;			//删除后将产生的新的顶点集
	AArray<char,char> VDegree;								//新产生顶点的度数，即邻接平面数。与上动态数组一一对应

	//哪些邻域平面相交于相同的顶点
	//对应于上面的要添加的顶点
	//如对于添加的顶点v,其在VerteciesAdded中的id为2
	//则PatchesIntersectV[2]对应了一个动态数组，其元素
	//即为相交于点v的所有patch领域面片的id
	//＝＝＝＝＝＝＝注意：＝＝＝＝＝＝
	//AArray<AArray<int,int>,AArray<int,int>>在编译时会报错！
	//说明模板类的使用暂时不支持嵌套定义。因此，这里采用了指针！
	AArray<AArray<int,int>*,AArray<int,int>*> PatchesIntersectV;
	
	//邻域面片包含了哪些新加入的顶点
	AArray<AArray<int,int>*,AArray<int,int>*> VerticesInPatch;
	VerticesInPatch.SetSize(npCount,0);
	int i(0);
	for(i=0;i<npCount;i++)
	{	
		//初始化VerticesInPatch的每一个元素
		VerticesInPatch[i]=new AArray<int,int>;
	}

	/////////////////////////////////////////////////////////////////////
	// 求解新交点部分！
	/////////////////////////////////////////////////////////////////////
	
	int n[3];		//存储选出的三个相邻面片的id
	CCombGenerator cg(3,npCount);
	//assert(cg.IsSolvable());			//确保npCount>=3
	A3DVECTOR3 v;
	bool bValid;	
	
	//主循环
	while(!cg.IsOver())
	{
		//产生一个组合
		cg.GetNextComb(n);
		if(Processed(n,PatchesIntersectV))			//已经处理过，直接跳过，进入下一轮循环！
			continue;

		if(Solve3NPIntersection(n,v))				//有唯一解
		{
			bValid=true;
			
			//如果有解，先代入到当前平面的方程
			//注意：考察当前面片时，则应该是不在当前面片的内部！
			//在该平面上或其外部都可以
			if(this->Inside(v)) 
				continue;			//直接进入下一轮while循环
			AArray<int,int>* pPatchesPassV=new AArray<int,int>;		//定义一个过v的所有面片的动态数组
			
			//依次代入其他的邻域面片方程检验
			for(i=0;i<npCount;i++)
			{
				if(i!=n[0] && i!=n[1] && i!=n[2])
				{
					//不能在这些面片的外部
					if(m_arrNeighbors[i].pNeighborPatch->Outside(v))
					{
						bValid=false;
						break;		//在某个平面外部，跳出for循环
					}
					//在平面上的情况
					if(m_arrNeighbors[i].pNeighborPatch->OnPlane(v))
					{
						pPatchesPassV->Add(i);			//出现共面的情况！
					}
				}
			} // end of for-loop
			
			if(bValid)	//有效的交点
			{
				VerteciesAdded.Add(v);		//添加到新有效顶点列表
				
				//添加n[0,1,2]到相交于该顶点的邻域平面列表
				pPatchesPassV->Add(n[0]);
				pPatchesPassV->Add(n[1]);
				pPatchesPassV->Add(n[2]);
				
				//modified by wf, 04-10-09
				//为保证前后计算的一致性而作的代码修改：
				int ExistPIV;
				if((ExistPIV=HasPIntersectVExist(pPatchesPassV,PatchesIntersectV))==-1)
				{
					//不存在不一致的情况

					//添加上述列表到PatchesIntersectV中
					PatchesIntersectV.Add(pPatchesPassV);	
					
					//将顶点度数记录下来
					VDegree.Add(pPatchesPassV->GetSize());
					
					//将该顶点在VerteciesAdded中的id添加到
					//相交于该顶点的各个面片对应的VerticesInPatch中
					int vid=VerteciesAdded.GetSize()-1;
					int npid;
					for(i=0;i<pPatchesPassV->GetSize();i++)
					{
						npid=pPatchesPassV->GetAt(i);
						VerticesInPatch[npid]->Add(vid);
					}
				}
				else
				{
					//出现了不一致的情况，此时按照最后最多面片相交的情况处理！
					VerteciesAdded.RemoveTail();		//删除该点

					AArray<int,int>* pExistPIV=PatchesIntersectV[ExistPIV];
					PatchesIntersectV[ExistPIV]=pPatchesPassV;
					VDegree[ExistPIV]=pPatchesPassV->GetSize();		
					//将该顶点在VerteciesAdded中的id添加到
					//相交于该顶点的各个面片对应的VerticesInPatch中
					//int vid=VerteciesAdded.GetSize()-1;
					int npid;
					for(i=0;i<pPatchesPassV->GetSize();i++)
					{
						npid=pPatchesPassV->GetAt(i);
						if(!InArray(npid,pExistPIV))			//如果该npid没有出现过，则添加vid
							VerticesInPatch[npid]->Add(ExistPIV);
					}
					
					delete pExistPIV;		//释放内存
				}

			}
			else
				delete pPatchesPassV;			//当前顶点无效，回收前面分配的内存

		}//无解的情况暂不考虑
	}// end of while-loop

	//如果没有合法的新交点，则这里应该返回false了
	//正常而言，应该不太回出现这种情况，因为在计算最小误差时
	//就应该排除掉了无解的情况，因此还有一个最小误差求解和本方法结果一致性的问题
	if(VerteciesAdded.GetSize()==0)
	{
		//释放内存

		for(i=0;i<PatchesIntersectV.GetSize();i++)
			delete PatchesIntersectV[i];
		for(i=0;i<VerticesInPatch.GetSize();i++)
			delete VerticesInPatch[i];

		return false;
	}

	/////////////////////////////////////////////////////////////////////
	// 向Polytope中插入新交点部分！
	/////////////////////////////////////////////////////////////////////
	//记录插入前Polytope已有的顶点数目，新交点插入后的全局id则依次为：g_vid,g_vid+1,g_vid+2...
	//因此可供以后更新面片的邻域顶点列表使用
	int g_vid=m_pConvexPolytope->GetVNum();	
	for(i=0;i<VerteciesAdded.GetSize();i++)
	{
		VertexInfo vInfo;
		vInfo.cDegree=VDegree[i];
		m_pConvexPolytope->AddV(VerteciesAdded[i],vInfo);
	}
	
	
	//更新之前应先将以前的邻域面片备份，这一步可以考虑放在CPolytope调用Removed()函数
	//前进行！

	/////////////////////////////////////////////////////////////////////
	// 更新邻域面片的邻域列表(包括顶点和相邻面片)
	/////////////////////////////////////////////////////////////////////
	CPatch* pCurNP;			//当前的邻域面片
	int g_vid1,g_vid2;		//当前的邻域面片对应的顶点，及邻边上的另一个顶点（都是全局id）
	/////////////////////////////////////////////////////////////////////
	// ＝＝＝＝＝＝＝注意在两相邻面片中，边的方向正好相反＝＝＝＝＝
	//
	//				 当前面片P
	//             \  ---->   /
	//	对应(Pnk) v1\________/ v2 对应邻域面片(Pn2)
	//              /        \
	//             / <-----   \
	//				  邻域面片(Pn1)
	//
	/////////////////////////////////////////////////////////////////////
	int next,pre;
	AArray<int,int> VSorted;				//按照顶点连接顺序连接好的数组
	AArray<int,int>* pArrVertices;
	int vid,lastvid;
	for(i=0;i<npCount;i++)					//外层循环，对每一个邻域面片分别考虑！循环体内是一个邻域面片的处理
	{
		VSorted.RemoveAll();				//清空上次循环中的顶点排序列表

		next=(i+1<npCount)?i+1:0;			//下一个neighbor的索引，由于是用数组表示环状结构，因此要作一个运算
		pre=(i-1<0)?npCount-1:i-1;			//上一个neighbor的索引

		pCurNP=m_arrNeighbors[i].pNeighborPatch;
		g_vid1=m_arrNeighbors[i].vid;
		g_vid2=m_arrNeighbors[next].vid;
		pArrVertices=VerticesInPatch[i];
		
		/////////////////////////////////////////////////////////////////////
		// 开始顶点连接
		// 注意下面的顶点连接算法似乎没有必然得出正确结果的保障
		// 因此，这一段也应作为测试和调试的重点！
		/////////////////////////////////////////////////////////////////////

		//先查找与vid2邻接的顶点
		int j(0);
		for(j=0;j<pArrVertices->GetSize();j++)
		{
			vid=pArrVertices->GetAt(j);
			if(InArray(i,PatchesIntersectV[vid]) &&		//vid1对应的邻域面片(Pn1)过vid
			   InArray(next,PatchesIntersectV[vid]))	//vid2对应的邻域面片(Pn2)过vid
			{
				//说明vid与v2相邻接
				VSorted.Add(vid);		//插入数组中
				break;					//找到了，跳出
			}
		}
		//确保找到！
		ASSERT_IN_REMOVE_PATCH(VSorted.GetSize()==1);	

		//然后，依次寻找并连接相邻的两个顶点！
		
		//lastvid标志最后一个已被连接的顶点
		//定义llvid则标志倒数第二个被连接的顶点，引入改变量是为了避免重复连接！
		int llvid;	
		int counter=1;			//引入counter防止死循环
		while(counter<pArrVertices->GetSize())
		{
			counter++;

			lastvid=VSorted[VSorted.GetSize()-1];
			if(VSorted.GetSize()<2)
				llvid=-1;		//一个无效值，因为此时还没有倒数第二个
			else
				llvid=VSorted[VSorted.GetSize()-2];

			for(j=0;j<pArrVertices->GetSize();j++)
			{
				vid=pArrVertices->GetAt(j);
				//这个地方应该增加判断：即不能重复连接！
				if((vid!=lastvid) && (vid!=llvid) && IsVAdjacent(PatchesIntersectV[lastvid],PatchesIntersectV[vid]))	//相邻
				{
					VSorted.Add(vid);
					break;
				}
			} // end of for-loop
		} // end of while-loop
		
		ASSERT_IN_REMOVE_PATCH(counter==VSorted.GetSize());
		
		lastvid=VSorted[VSorted.GetSize()-1];
		
		//确保最后一个顶点lastvid应该是和v1邻接的
		ASSERT_IN_REMOVE_PATCH(InArray(pre,PatchesIntersectV[lastvid]) && 
							   InArray(i,PatchesIntersectV[lastvid]) );
			   
		/////////////////////////////////////////////////////////////////////
		// 确定顶点连接关系后，则可以开始邻域面片的顶点插入操作了
		/////////////////////////////////////////////////////////////////////
		//判断v1和v2是否应该从当前邻域面片的Neighbor中删除！
		//判断方法：以v2为例，如果过v2的面片仅有P,Pn1,Pn2，则v2可以删除了，这可以简化为看其度数是否为3或>3
		AArray<VPNeighbor,VPNeighbor>& Neighbors=pCurNP->GetNeighbors();
		//寻找v2在当前邻域面片中的位置
		for(j=0;j<Neighbors.GetSize();j++)
		{
			int tmp=Neighbors[j].vid;
			if(g_vid2==Neighbors[j].vid)
				break;
		}
		//循环结束，j保存了当前的v2位置
		int v2pos=j;
		ASSERT_IN_REMOVE_PATCH(v2pos<Neighbors.GetSize());				//确保一定找到
		ASSERT_IN_REMOVE_PATCH(g_vid1==pCurNP->GetNextV(v2pos));		//并确保v2的下一个领接顶点是v1
		
		bool bv2Last=(v2pos==Neighbors.GetSize()-1);	//v2是否是Neighbor中最后一个元素？

		//先处理v1，因为对其的操作比较简单！直接删除或保留即可！
		//判断v1是否应该从当前邻域面片的Neighbor中删除
		VertexInfo& v1Info=m_pConvexPolytope->GetVInfo(g_vid1);
		if(v1Info.cDegree==3)
		{
			//删除该邻域，v1在当前patch中的索引为pCurNP->GetNext(j)
			Neighbors.RemoveAt(pCurNP->GetNext(v2pos));	
			if(bv2Last)		//说明v1是第一个元素，因此，删除v1后，应将v2pos自减
				v2pos--;
		}
		int InsertPos;				//新邻域添加的位置

		//处理v2是否删除或保留！
		VertexInfo& v2Info=m_pConvexPolytope->GetVInfo(g_vid2);
		if(v2Info.cDegree==3)
		{
			//则应删除v2
			Neighbors.RemoveAt(v2pos);
			InsertPos=v2pos;						//v2已被删除，直接从v2pos插入
		}
		else
		{
			//不删除v2，但必须修改其pNeighborPatch
			Neighbors[v2pos].pNeighborPatch=m_arrNeighbors[next].pNeighborPatch;	//修改其对应NeighborPatch为Pn2!
			InsertPos=pCurNP->GetNext(v2pos);		//插入点在v2pos后面第一个元素起
		}		
		
		//构造新要添加的邻域
		AArray<VPNeighbor,VPNeighbor> NeighborsToAdd;		//新要添加的邻域
		VPNeighbor vpn;
		for(j=0;j<VSorted.GetSize();j++)					
		{
			vpn.vid =VSorted[j]+g_vid;						//注意，这里要用全局id，因此应加上g_vid
			if(j==VSorted.GetSize()-1)	
			{
				//最后一个邻域节点要特殊处理
				//其邻域面片应等于本面片的上一个邻域面片
				vpn.pNeighborPatch=m_arrNeighbors[pre].pNeighborPatch;	
			}
			else
			{
				//要用一个查找算法！找出对应于该点的邻域面片！
				for(int k=0;k<npCount;k++)
				{
					if(k!=i)		//不是当前正在考察的面片
					{
						AArray<int,int>* pVertecies=VerticesInPatch[k];
						if(InArray(VSorted[j],pVertecies) && InArray(VSorted[j+1],pVertecies) )
						{
							//由VSorted[j,j+1]构成的边出现在pVertecies中
							//说明第k个邻域面片对应了顶点VSorted[j]
							vpn.pNeighborPatch=m_arrNeighbors[k].pNeighborPatch;
						}

					}
				}
			}
			ASSERT_IN_REMOVE_PATCH(vpn.pNeighborPatch);
			NeighborsToAdd.Add(vpn);
		}

		//插入新的邻域
		Neighbors.InsertAt(InsertPos,NeighborsToAdd);
		
		//当前邻域面片pCurNP的新的邻域构造OK后，需要对其进行更新删除误差操作！
		pCurNP->UpdateRemovedError();

	} // end of for-loop
	
	//循环遍历当前面片的每一个顶点，并将其度数自减1
	int curvid;
	for(i=0;i<npCount;i++)
	{
		curvid=m_arrNeighbors[i].vid;
		VertexInfo& curVInfo=m_pConvexPolytope->GetVInfo(curvid);
		curVInfo.cDegree--;
	}

	//释放内存
	for(i=0;i<PatchesIntersectV.GetSize();i++)
		delete PatchesIntersectV[i];
	for(i=0;i<VerticesInPatch.GetSize();i++)
		delete VerticesInPatch[i];

	return true;		//删除成功

//出现异常
ExceptionOccur:
	//释放内存
	for(i=0;i<PatchesIntersectV.GetSize();i++)
		delete PatchesIntersectV[i];
	for(i=0;i<VerticesInPatch.GetSize();i++)
		delete VerticesInPatch[i];
	
	//将删除误差置为无效！
	m_fRemovedError=-1.0f;

	//抛出异常
	m_pConvexPolytope->ThrowException();
	
	//返回，删除失败
	return false;
}

//////////////////////////////////////////////////////////////
// 判断n[0,1,2]这三个元素是否已经出现在多于三个面相交于一点
// 的同解列表中。
// 如果是，则不用重新计算这三个id对应的面片的交点了！
// 供Removed()进行调用
//////////////////////////////////////////////////////////////
bool CPatch::Processed(int n[], AArray<AArray<int,int>*,AArray<int,int>*>& CoSolutionList)
{
	bool bInArray[3];
	AArray<int,int>* pArr;

	for(int i=0;i<CoSolutionList.GetSize();i++)
	{
		pArr=CoSolutionList[i];
		assert(pArr->GetSize()>=3);
		if(pArr->GetSize()==3)			//标准情况，三个面有一个解，直接跳过
			continue;
		
		bInArray[0]=false;				//初始化
		bInArray[1]=false;				//初始化
		bInArray[2]=false;				//初始化

		//多于三个面有同解的情况
		for(int j=0;j<pArr->GetSize();j++)
		{
			if(n[0]==pArr->GetAt(j)) bInArray[0]=true;
			if(n[1]==pArr->GetAt(j)) bInArray[1]=true;
			if(n[2]==pArr->GetAt(j)) bInArray[2]=true;
		}

		if(bInArray[0] && bInArray[1] && bInArray[2])
			return true;
	}
	
	return false;
}

bool CPatch::InArray(int pid, AArray<int,int>* pArr)
{
	for(int i=0;i<pArr->GetSize();i++)
		if(pid==pArr->GetAt(i))
			return true;
	return false;
}

//////////////////////////////////////////////////////////////
// 判断新加入的两个顶点是否邻接
// 其方法为，判断这两个顶点对应的面片集合中，是否有两个完全相同！
// 供Removed()进行调用
//////////////////////////////////////////////////////////////
bool CPatch::IsVAdjacent(AArray<int,int>* pArr1, AArray<int,int>* pArr2)
{
	int count=0;
	for(int i=0;i<pArr1->GetSize();i++)
		if(InArray(pArr1->GetAt(i),pArr2))
		{
			count++;
			if(count==2) return true;
		}
	return false;
}

//判断pPatch是否在当前面片的邻域面片之中
bool CPatch::InNeighbors(CPatch *pPatch)
{
	for(int i=0;i<m_arrNeighbors.GetSize();i++)
	{
		if(pPatch==m_arrNeighbors[i].pNeighborPatch)
			return true;
	}
	
	return false;

}

//////////////////////////////////////////////////////////////
// 重写一个复制构造函数，覆盖缺省的复制构造函数
// 从而避免在拷贝时对m_arrNeighbors的直接赋值
// 这会造成指针的拷贝，从而引起内存的删除错误！
//////////////////////////////////////////////////////////////
CPatch::CPatch(const CPatch &patch)
{
	m_vNormal=patch.m_vNormal;
	m_d=patch.m_d;
	m_fRemovedError=patch.m_fRemovedError;
	m_pConvexPolytope=patch.m_pConvexPolytope;

	//注意这一项的不同
	int nSize=patch.m_arrNeighbors.GetSize();
	for(int i=0;i<nSize;i++)
	{
		VPNeighbor vpNB=patch.m_arrNeighbors[i];
		m_arrNeighbors.Add(patch.m_arrNeighbors[i]);
	}

}

//////////////////////////////////////////////////////////////
// 重写一个赋值函数，从而避免指针出错！
// 非常类似与上面重写的复制构造函数，但略有不同
//////////////////////////////////////////////////////////////
const CPatch& CPatch::operator=(const CPatch& patch)
{
	if(this!=&patch)	//防止自身赋值
	{
		m_vNormal=patch.m_vNormal;
		m_d=patch.m_d;
		m_fRemovedError=patch.m_fRemovedError;
		m_pConvexPolytope=patch.m_pConvexPolytope;

		//注意这一项的不同
		int nSize=patch.m_arrNeighbors.GetSize();

		m_arrNeighbors.RemoveAll();			//这里多一步清空自身
		
		for(int i=0;i<nSize;i++)
		{
			VPNeighbor vpNB=patch.m_arrNeighbors[i];
			m_arrNeighbors.Add(patch.m_arrNeighbors[i]);
		}

	}
	
	return *this;
}

//////////////////////////////////////////////////////////////
// Added by wf,04-10-09
// 为保证计算交点的一致性而添加的函数
// 对于以前计算出的每一个相交于一点的平面片集，查找是否有
// 可以包含在当前待插入的交于一点的平面片集PIntersectV
// 如果有，则返回其在PatchesIntersectV的索引id
// 否则，返回-1
//////////////////////////////////////////////////////////////
int CPatch::HasPIntersectVExist(AArray<int,int>* pPIntersectV,const AArray<AArray<int,int>*,AArray<int,int>*>& PatchesIntersectV)
{
	if(pPIntersectV->GetSize()==3)		
		return -1;			//只有三个元素，因此不存在包含关系，直接返回－1

	AArray<int,int>* pPIV;
	int pid;
	bool bIn;
	for(int i=0;i<PatchesIntersectV.GetSize();i++)
	{
		bIn=true;
		pPIV=PatchesIntersectV[i];
		for(int j=0;j<pPIV->GetSize();j++)
		{
			pid=pPIV->GetAt(j);
			if(!InArray(pid,pPIntersectV))
			{
				bIn=false;
				break;
			}
		}
		if(bIn)
			return i;
	}

	return -1;

}
///////////////////////////////////////////////////
// 将本面片上所有顶点的度数增1
// 这将用在面片被删除后作恢复的时候
///////////////////////////////////////////////////
void CPatch::IncreVDegree()
{
	int vid;
	for(int i=0;i<m_arrNeighbors.GetSize();i++)
	{
		vid=m_arrNeighbors[i].vid;
		m_pConvexPolytope->GetVInfo(vid).cDegree++;
	}
}

///////////////////////////////////////////////////
// 将本面片上所有顶点的度数减1
// 这将用在面片被删除恢复后再重新删除的时候
///////////////////////////////////////////////////
void CPatch::DecreVDegree()
{
	int vid;
	for(int i=0;i<m_arrNeighbors.GetSize();i++)
	{
		vid=m_arrNeighbors[i].vid;
		m_pConvexPolytope->GetVInfo(vid).cDegree--;
	}

}

A3DVECTOR3 CPatch::GetVertex(int vid) 
{ 
	return m_pConvexPolytope->GetV(m_arrNeighbors[vid].vid);
}

void CPatch::GetEdge(int id,A3DVECTOR3& v1,A3DVECTOR3& v2)
{
	v1=m_pConvexPolytope->GetV(m_arrNeighbors[id].vid);
	v2=m_pConvexPolytope->GetV(GetNextV(id));
}

}	// end namespace