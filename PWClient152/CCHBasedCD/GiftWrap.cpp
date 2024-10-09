      // GiftWrap.cpp: implementation of the CGiftWrap class.
//
//////////////////////////////////////////////////////////////////////

#include "GiftWrap.h"
#include "halfspace.h"

//#include <fstream>


#define ABS(x) (((x)<0)?(-(x)):(x))

#define CH_FLT_EPSILON 2.0e-7F			// 绝对值最小的浮点数
#define CH_VALID_TOLERANCE  0.03f

namespace CHBasedCD
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGiftWrap::CGiftWrap()
{
	m_fInitHSDistThresh=0.01f;			
	m_nHullType=HULL_3D;				//缺省的，凸包为3D的
}

CGiftWrap::~CGiftWrap()
{

}

CGiftWrap::CGiftWrap(A3DVECTOR3* pVertexes,int vNum)
{
	//设置顶点信息
	SetVertexes(pVertexes,vNum);

	m_bExceptionOccur=false;
	
	m_fInitHSDistThresh=0.01f;
	m_nHullType=HULL_3D;				//缺省的，凸包为3D的
}

void CGiftWrap::ComputeConvexHull()
{
	//////////////////////////////////////////////////////////////////////////
	// Noted by wenfeng, 05-01-31
	// 由于求解凸包的过程在一定程度上依赖于CHalfSpace类的距离阈值，因此
	// 这里将设法找到一个可以得到正确结果的距离阈值。
	// 一个重要的问题是能否求解出凸包与该阈值并非是单调的关系，即并非是阈值越大或越小
	// 就一定可以得到结果。实际上，阈值的一个微小的扰动，将可能导致异常出现或求出凸包
	// 但目前还没有找出两者之间的必然关系和规律。
	// 因此，目前的阈值设定，采用了宽度优先的尝试法。简单说明如下：
	// 首先尝试一个基本值hsDistThreshBase，然后将阈值设为hsDistThreshBase×0.1，
	// 直到阈值<CH_FLT_EPSILON。
	// 如果上述所有的阈值仍不能满足要求，则对hsDistThreshBase-0.1*hsDistThreshBase，
	// 然后继续上面的循环！
	// 可考虑另外的方法，如随机数的方法
	//////////////////////////////////////////////////////////////////////////
	

	float hsDistThreshBase=m_fInitHSDistThresh*10.0f;	//每次递减循环的基数，先放大10倍
	float hsDistThreshBaseStep=m_fInitHSDistThresh;		//递减步长
	float hsDistThresh;			//当前的阈值
	
	//保证没有异常的求出凸包！
	//双重循环
	do{
	
		hsDistThresh=hsDistThreshBase;
		do{
			
			m_nHullType=HULL_3D;						//重新计算时，初始化为3D凸包

			hsDistThresh*=0.1f;								//缩小阈值10倍

			CHalfSpace::SetDistThresh(hsDistThresh);	//调整阈值
			
			//先调用父类函数从而完成清空操作
			CConvexHullAlgorithm::ComputeConvexHull();
			ResetSameVertices();

			Edge e=SearchFirstEdge();
			m_eFirst=e;
			m_EdgeStack.Push(e);
			m_EdgeStack.Push(e);			//注意，这里应该push两次！从而保证在while条件中的弹出后，仍有一个边被保存在栈中！
			
			while(!m_EdgeStack.IsEmpty() && !m_bExceptionOccur && m_nHullType==HULL_3D)
			{
				e=m_EdgeStack.Pop();
				DealE(e);
			}
			
		}while( ExceptionOccur()  && hsDistThresh>CH_FLT_EPSILON);
		
		hsDistThreshBase-=hsDistThreshBaseStep;		//外层循环再进行一个扰动

	}while( ExceptionOccur() && hsDistThreshBase>0.0f);

	if(hsDistThreshBase<=0.0f)
	{
		//说明已经超出了阈值范围，如果还有异常发生
		//则说明确实该模型的凸壳计算存在异常！
		m_bExceptionOccur=true;
	}

	//hsDistThresh*=0.01f;	
	//CHalfSpace::SetDistThresh(hsDistThresh);  //恢复到缺省的阈值
	
	CHalfSpace::SetDistThresh();  //恢复到缺省的阈值
}

/////////////////////////////////////////////////////////
// added by wenfeng,2004.8.20
// 对SearchV()方法的改进，使其可以处理多点共面的情况！
/////////////////////////////////////////////////////////
bool CGiftWrap::DealE(Edge e)
{

	int i(0);
	for(i=0;i==(int)e.start || i==(int)e.end || IsFaceInCH(Face(e.start,e.end,i)) || m_pbVInvalid[i];i++);

	//构造e和该点组成的半空间，此时的hs必定有效，因为，保证了不共线条件
	CHalfSpace hs(m_pVertexes[e.start],m_pVertexes[e.end],m_pVertexes[i]);

	AArray<int,int>* pCoPlanarVertexes=new AArray<int,int>;	//构造一个所有共面点的列表
	AArray<int,int>& CoPlanarVertexes=*pCoPlanarVertexes;
	CoPlanarVertexes.Add(i);			//添加当前第一个元素
	//开始主循环！注意循环开始条件！从下一个元素开始！
	for(i++;i<m_nVNum;i++)
	{
		//不是e端点，没有考察过，且不在半空间内部！
		if(i!=(int)e.start && i!=(int)e.end && !m_pbVInvalid[i] && !IsFaceInCH(Face(e.start,e.end,i)) && ! hs.Inside(m_pVertexes[i]))	
		{
			if(hs.OnPlane(m_pVertexes[i]))
			{
				//在半空间的边界上，则添加到共面点列表
				CoPlanarVertexes.Add(i);			//添加当前点
				
			}
			else
			{

				//说明该点不在内部，则该点为新的目标点，并重构半空间
				hs.Set(m_pVertexes[e.start],m_pVertexes[e.end],m_pVertexes[i]);

				//同时清空共面列表，并将该点添加至共面列表
				CoPlanarVertexes.RemoveAll();
				CoPlanarVertexes.Add(i);			//添加当前点

				//注意，根据凸壳的性质，这里不需从头开始遍历

			}
		}

	}
	
	//经过上面的循环后，CoPlanarVertexes中就存储了由当前边e找到的一个凸面上的所有点

	//在这里判断是否是2D的凸包
	m_nHullType=HULL_2D;
	for(i=0;i<m_nVNum;i++)			//遍历所有顶点
	{
		if(i!=(int)e.start && i!=(int)e.end && !m_pbVInvalid[i])	//非e的端点，而且有效
		{
			if(!IsVInVSets(i,pCoPlanarVertexes))	// 如果当前顶点不再公面集中！
			{
				// Added by wenfeng, 05-5-9
				// 增加一个判断，从而使得结果更严密
				CHalfSpace hs(m_pVertexes[e.start], m_pVertexes[e.end], m_pVertexes[CoPlanarVertexes[0]]);
				if(hs.OnPlane(m_pVertexes[i]))
					CoPlanarVertexes.Add(i);
				else
				{
					m_nHullType=HULL_3D;
					break;
				}
			}
		}
	}


	//下面开始进行处理，这里将原本在ComputeConvexHull的主循环中的处理放在这里！
	if(CoPlanarVertexes.GetSize()==1)		
	{
		//最简单的情况：没有共面点
		
		int v3=CoPlanarVertexes[0];
		//将另两条边压栈
		Edge e1(e.start,v3),e2(v3,e.end);
		SelectivePushStack(e1);	//m_EdgeStack.CheckPush(e1);
		SelectivePushStack(e2);	//m_EdgeStack.CheckPush(e2);

		//构造三角面片，并插入队列中
		m_Faces.Add(Face(e.start,e.end,v3));
		//同时，该三角面片的三个顶点为凸壳的ExtremeVertex!
		m_pbExtremeVertex[e.start]=true;
		m_pbExtremeVertex[e.end]=true;
		m_pbExtremeVertex[v3]=true;

		if(m_nHullType==HULL_2D)			// 凸包为一个三角形的情况
		{
			CoPlanarVertexes.Add(e.start);
			CoPlanarVertexes.Add(e.end);
			m_Planes.Add(pCoPlanarVertexes);
		}
		else
			delete pCoPlanarVertexes;				//没有多点共面

	}
	else		//多点共面的情况！
	{
		//在由e.start,e.end和CoPlanarVertexes[0]组成的平面上找凸壳
		DealCoPlanarV(e,CoPlanarVertexes);

	}


	return true;
}

/////////////////////////////////////////////////////////
// added by wenfeng,2004.8.20
// 该函数用来处理对于边e，找到的目标点是多点共面的情况
// 注意，参数中的CoPlanarVertexes所含的点数应>1
/////////////////////////////////////////////////////////
void CGiftWrap::DealCoPlanarV(Edge e, AArray<int,int> & CoPlanarVertexes)
{
	/*
	//该方法暂时放弃使用，wenfeng 04-12-02
	
	 //Revised by He wenfeng, 2004-11-29
	//这里必须修正顶点：将其他除v1,v2,v3外的顶点位置进行修正，
	//使他们投影到v1,v2,v3构成的平面上！
	
	CHalfSpace Plane(m_pVertexes[e.start],m_pVertexes[e.end],m_pVertexes[CoPlanarVertexes[0]]);
	for(int j=1;j<CoPlanarVertexes.GetSize();j++)
	{
		A3DVECTOR3 vDiff=m_pVertexes[CoPlanarVertexes[j]]-m_pVertexes[e.start];
		vDiff=DotProduct(vDiff,Plane.GetNormal())*Plane.GetNormal();
		m_pVertexes[CoPlanarVertexes[j]]-=vDiff;
	}
	//*/
	
	AArray<int,int> *parrCoPlanarVertexes=&CoPlanarVertexes;

	//如果出现该边的两点无效的情况，直接返回
	if(m_pbVInvalid[e.start]||m_pbVInvalid[e.end])
	{
		//异常退出时，一定要释放内存
		delete parrCoPlanarVertexes;
		return;
	}

	//构造初始的halfspace
	//注意构造的方法：该halfspace的分割面过e.end和CoPlanarVertexes[0]，同时平行于法向
	CHalfSpace hs;
	
	AArray<int,int> ExtremeVertexes;				//平面上所有点的凸壳点集，用来存储结果

	AArray<int,int> CoLinearVertexes;				//共线的情况，用一个点集表示
	//CoLinearVertexes.Add(CoPlanarVertexes[0]);

	//将e.start和e.end也存入CoPlanarVertexes中
	CoPlanarVertexes.Add(e.start);
	CoPlanarVertexes.Add(e.end);
	


	
	//在新的算法中，每条边e已经是延伸到最大的情况，因此，不用考虑
	//还能将其延伸的情况！
	//被修改于2004-8-25

	int vidNotCL=CoPlanarVertexes[0];		
	//由于上面的条件，CoPlanarVertexes[0]一定不会和e共线的
	//利用vidNotCL，首先求出平面的法向
	A3DVECTOR3 normal=CrossProduct(m_pVertexes[e.end]-m_pVertexes[e.start],m_pVertexes[vidNotCL]-m_pVertexes[e.start]);
	normal.Normalize();

	Edge curE(e.end,vidNotCL);						//当前的边

	//微改于04-10-19下午
	//该while可能会出现死循环，因此必须加上判断条件避免
	int vSize=CoPlanarVertexes.GetSize();

	int i(0);
	//main loop
	while(curE.start!=e.start)				//当最后找到e.start这一点时，表明找到了所有点处在凸壳上的点
	{
		//初始化hs
		hs.Set(m_pVertexes[curE.start],m_pVertexes[curE.end],m_pVertexes[curE.start]+normal);
		
		CoLinearVertexes.RemoveAll();
		CoLinearVertexes.Add(curE.end);		//当前点添加至共线点集

		//寻找满足当前curE的目标点
		for(i=0;i<CoPlanarVertexes.GetSize();i++)
		{
			int id=CoPlanarVertexes[i];
			if(CoPlanarVertexes[i]!=(int)curE.start && 
				CoPlanarVertexes[i]!=(int)curE.end &&
				!m_pbVInvalid[CoPlanarVertexes[i]] &&			//必须为有效点！
				!hs.Inside(m_pVertexes[CoPlanarVertexes[i]]))
			{
				//判断是否共面，实际上为是否共线
				if(hs.OnPlane(m_pVertexes[CoPlanarVertexes[i]]))
				{
					//对于与e共线的情况，不能添加
					//if(CoPlanarVertexes[i]!=e.start && CoPlanarVertexes[i]!=e.end)
						//&& DotProduct(m_pVertexes[curE.end]-m_pVertexes[curE.start],m_pVertexes[CoPlanarVertexes[i]]-))
					CoLinearVertexes.Add(CoPlanarVertexes[i]);
				}
				else
				{
					//更改当前边
					curE.end=CoPlanarVertexes[i];
					//重新计算halfspace
					hs.Set(m_pVertexes[curE.start],m_pVertexes[curE.end],m_pVertexes[curE.start]+normal);

					//清空，并重新计算共线点集
					CoLinearVertexes.RemoveAll();
					CoLinearVertexes.Add(CoPlanarVertexes[i]);

				}
			}
		}
		//循环完成，此时应该已经找到一个点或一组点集
		if(CoLinearVertexes.GetSize()==1)
		{
			//一个点的情况
			//添加至凸壳边界点集
			ExtremeVertexes.Add(CoLinearVertexes[0]);
			//找到的点成为当前边的起点
			curE.start=CoLinearVertexes[0];

			//找一个点作为curE.end
			for(i=0;i<CoPlanarVertexes.GetSize() && CoPlanarVertexes[i]==(int)curE.start;i++);	//不能等于start!
			curE.end=CoPlanarVertexes[i];

			vSize--;
			if(vSize<0)
			{
				this->m_bExceptionOccur=true;		//出现异常了，退出！
				
				//异常退出时，一定要释放内存
				delete parrCoPlanarVertexes;

				return;
			}

		}
		else
		{
			//04-10-30
			//由于只找最远点，所以这里没必要作一个排序操作。
			
			//多个点集的情况
			//按点到curE.start欧氏距离的远近排序，从近到远
			SortByDist(curE.start,CoLinearVertexes);



			//重置起点为最远点
			curE.start=CoLinearVertexes[CoLinearVertexes.GetSize()-1];
			//插入最远点！
			ExtremeVertexes.Add(curE.start);

			//找一个点作为curE.end
			for(i=0;i<CoPlanarVertexes.GetSize() && (CoPlanarVertexes[i]==(int)curE.start || m_pbVInvalid[CoPlanarVertexes[i]]);i++);	//不能等于start!
			assert(i<CoPlanarVertexes.GetSize());
			curE.end=CoPlanarVertexes[i];			

			vSize--;
			if(vSize<0)
			{
				this->m_bExceptionOccur=true;		//出现异常了，退出！
				//异常退出时，一定要释放内存
				delete parrCoPlanarVertexes;

				return;
			}

		}

	} // end of main loop

	//注意，经过前面的循环ExtremeVertexes的最后一个元素应该就是e.start
	//assert(ExtremeVertexes.GetSize()>1);
	if(ExtremeVertexes.GetSize()<=1)
	{
		m_bExceptionOccur=true;		//说明异常发生，退出！
		//异常退出时，一定要释放内存
		delete parrCoPlanarVertexes;
		return;
	}

	//此时，已经得到了一组按顺序的顶点，可以向边堆栈和面列表中添加了
	Edge ep(ExtremeVertexes[0],e.end);
	SelectivePushStack(ep);	//m_EdgeStack.CheckPush(ep);
	m_Faces.Add(Face(e.start,e.end,ExtremeVertexes[0],true)); //该面将被剖分
	
	for(i=0;i<ExtremeVertexes.GetSize()-2;i++)
	{
		//添加边到堆栈，一定要注意顺序！
		ep.Set(ExtremeVertexes[i+1],ExtremeVertexes[i]);		//应该反向添加
		SelectivePushStack(ep);	//m_EdgeStack.CheckPush(ep);
		m_Faces.Add(Face(e.start,ExtremeVertexes[i],ExtremeVertexes[i+1],true));
		
	}
	//最后一条边
	//注意：对该边的处理应该直接使用m_EdgeStack.CheckPush
	//因为，此前已经添加了包含该边的三角面，所以
	//如果使用SelectivePushStack，则会认为该边已经被
	//添加到三角形列表中，从而漏掉了这条边！
	//这样会导致最终凸壳缺少了一个面！
	ep.Set(ExtremeVertexes[i+1],ExtremeVertexes[i]);
	m_EdgeStack.CheckPush(ep);

	//将所有共面点都置为无效
	for(i=0;i<CoPlanarVertexes.GetSize();i++)
		m_pbVInvalid[CoPlanarVertexes[i]]=true;

	//重新整理CoPlanarVertexes，使其仅包括边界点
	CoPlanarVertexes.RemoveAll();	
	//将e.end插入到ExtremeVertexes中
	ExtremeVertexes.Add(e.end);
	//然后将所有边界点置为有效
	for(i=0;i<ExtremeVertexes.GetSize();i++)
	{
		m_pbVInvalid[ExtremeVertexes[i]]=false;
		m_pbExtremeVertex[ExtremeVertexes[i]]=true;
		CoPlanarVertexes.Add(ExtremeVertexes[i]);
	}
	
	//此时再将CoPlanarVertexes插入到m_Planes中
	//将共面的所有点构成的平面添加至m_Planes
	m_Planes.Add(&CoPlanarVertexes);


}

/////////////////////////////////////////////////////////
// added by wenfeng,2004.8.20
// 按照到v距离远近，对vList中的点，重新排序
/////////////////////////////////////////////////////////
void CGiftWrap::SortByDist(int v, AArray<int,int> & vList)
{
	if(vList.GetSize()<2) return;

	int i(0);

	//先计算距离
	float* pDist=new float[vList.GetSize()];
	for(i=0;i<vList.GetSize();i++)
		pDist[i]=(m_pVertexes[vList[i]]-m_pVertexes[v]).SquaredMagnitude();

	int minDistID;
	int tmp;			//用作交换
	//根据距离排序
	for(i=0;i<vList.GetSize();i++)
	{
		minDistID=i;
		for(int j=i+1;j<vList.GetSize();j++)
			if(pDist[j]<pDist[minDistID])
				minDistID=j;
		//子循环扫描一遍，找到从i到GetSize()中距离最小的点

		//开始交换
		tmp=vList[i];
		vList[i]=vList[minDistID];
		vList[minDistID]=tmp;
		
		//同时将距离重置
		pDist[minDistID]=pDist[i];

	}

	delete [] pDist;
}

//判断空间中三点是否共线
bool CGiftWrap::IsTriVsCoLinear(int v1, int v2, int v3)
{
	A3DVECTOR3 vd1=m_pVertexes[v2]-m_pVertexes[v1],vd2=m_pVertexes[v3]-m_pVertexes[v1];
	vd1.Normalize();
	vd2.Normalize();

	if(1- ABS(DotProduct(vd1,vd2)) < 0.00006f)
		return true;
	else return false;

	
}

//判断边e和点v之间的关系
//返回值情况如下：
//0:v在e外！
//1:v在直线e上，但在线段e左侧，即e.start外
//2:v在直线e上，但在线段e右侧，即e.end外
//3:v在线段e内
//-1:v和e.start重合
//-2:v和e.end重合
int CGiftWrap::EVRelation(Edge e, int v)
{
	if(v==(int)e.start) return -1;
	if(v==(int)e.end) return -2;

	if(!IsTriVsCoLinear(e.start,e.end,v)) return 0;

	A3DVECTOR3 de=m_pVertexes[e.end]-m_pVertexes[e.start];		//线段的方向
	A3DVECTOR3 vd1=m_pVertexes[v]-m_pVertexes[e.start];
	A3DVECTOR3 vd2=m_pVertexes[v]-m_pVertexes[e.end];

	if(DotProduct(vd1,vd2)<0.0f) return 3;		//在线段e内

	if(DotProduct(vd1,de)>0.0f) return 2;

	return 1;

}

//寻找第一条边！
Edge CGiftWrap::SearchFirstEdge()
{
	Edge e;
	int i(0);
	//先寻找y取值最小的一组点
	AArray<int,int> LowestVertexes;
	float yMin=m_pVertexes[0].y;
	LowestVertexes.Add(0);
	for(i=1;i<m_nVNum;i++)
	{
		if(yMin-m_pVertexes[i].y>0.00001f)	// 说明比yMin更小
		{
			yMin=m_pVertexes[i].y;
			LowestVertexes.RemoveAll();
			LowestVertexes.Add(i);
		}
		else if( ABS(yMin-m_pVertexes[i].y)<0.00001f)
		{
			//共面
			LowestVertexes.Add(i);
		}

	}

	//循环完成后，LowestVertexes将保存了y值最小的一组点集
	if(LowestVertexes.GetSize()==1) //仅有一个点
	{
		e.start=LowestVertexes[0];

		//寻找第2个点，方法为在XOY平面上寻找！
		for(i=0;i==(int)e.start;i++);
		
		//构造初始的平行于z轴的半空间
		CHalfSpace hs(m_pVertexes[e.start],m_pVertexes[i]);
		AArray<int,int> CoPlanarVertexes;
		CoPlanarVertexes.Add(i);			//添加当前第一个元素		
		for(i++;i<m_nVNum;i++)
			if(i!=(int)e.start && !hs.Inside(m_pVertexes[i]))
				if(hs.OnPlane(m_pVertexes[i]))
					CoPlanarVertexes.Add(i);			//添加当前点
				else
				{
					//说明该点不在内部，则该点为新的目标点，并重构半空间
					hs.Set(m_pVertexes[e.start],m_pVertexes[i]);

					//同时清空共面列表，并将该点添加至共面列表
					CoPlanarVertexes.RemoveAll();
					CoPlanarVertexes.Add(i);			//添加当前点

				}
		
		if(CoPlanarVertexes.GetSize()==1)
		{
			//仅一个点，则该点即为e.end
			e.end=CoPlanarVertexes[0];
			return e;
		}
		else
		{
			//有多于一个点共面的情况
			A3DVECTOR3 vd3=hs.GetNormal();
			//构造初始的halfspace
			hs.Set(m_pVertexes[e.start],m_pVertexes[CoPlanarVertexes[0]],m_pVertexes[e.start]+vd3);
			AArray<int,int> CoLinearVertexes;
			CoLinearVertexes.Add(CoPlanarVertexes[0]);
			for(i=1;i<CoPlanarVertexes.GetSize();i++)
				if(!hs.Inside(m_pVertexes[CoPlanarVertexes[i]]))
					if(hs.OnPlane(m_pVertexes[CoPlanarVertexes[i]]))
						CoLinearVertexes.Add(CoPlanarVertexes[i]);
					else
					{

						//重新计算halfspace
						hs.Set(m_pVertexes[e.start],m_pVertexes[CoPlanarVertexes[i]],m_pVertexes[e.start]+vd3);

						//清空，并重新计算共线点集
						CoLinearVertexes.RemoveAll();
						CoLinearVertexes.Add(CoPlanarVertexes[i]);
					}
			//循环结束
			if(CoLinearVertexes.GetSize()==1)
			{
				//仅一个共线点，则该点即为另一端点
				e.end = CoLinearVertexes[0];
				return e;
			}
			else
			{
				//对共线点集进行距离的排序
				SortByDist(e.start,CoLinearVertexes);

				//置最远点为e的另一个端点，并置其他点为无效
				for(int i=0;i<CoLinearVertexes.GetSize()-1;i++)
					m_pbVInvalid[CoLinearVertexes[i]]=true;

				e.end=CoLinearVertexes[CoLinearVertexes.GetSize()-1];
				return e;
			}
		}


	}
	else
	{
		//y取得最小值多于一个点的情况！
		//则只需在XOZ平面上考虑即可
		if(LowestVertexes.GetSize()==2)		//如果仅有两点，则直接返回
		{
			e.start=LowestVertexes[0];
			e.end=LowestVertexes[1];
			return e;
		}
		//找z值取最小的点或点集
		AArray<int,int> ZMinVertexes;
		float ZMin=m_pVertexes[LowestVertexes[0]].z;
		ZMinVertexes.Add(LowestVertexes[0]);
		for(i=1;i<LowestVertexes.GetSize();i++)
		{
			if(ZMin-m_pVertexes[LowestVertexes[i]].z>0.00001f)	// 说明比yMin更小
			{
				ZMin=m_pVertexes[LowestVertexes[i]].z;
				ZMinVertexes.RemoveAll();
				ZMinVertexes.Add(LowestVertexes[i]);
			}
			else if( ABS(ZMin-m_pVertexes[LowestVertexes[i]].z)<0.00001f)
			{
				//共面
				ZMinVertexes.Add(LowestVertexes[i]);
			}

		}
		
		if(ZMinVertexes.GetSize()==1)		
		{
			//则该点就被置为第一点
			e.start=ZMinVertexes[0];
			AArray<int,int> CoLinearVertexes;
			
			//在LowestVertexes中开始寻找另一点
			for(i=0;LowestVertexes[i]==(int)e.start;i++);
			CoLinearVertexes.Add(LowestVertexes[i]);
			A3DVECTOR3 vd3=A3DVECTOR3(0.0f,-1.0f,0.0f);
			CHalfSpace hs(m_pVertexes[e.start],m_pVertexes[LowestVertexes[i]],m_pVertexes[e.start]+vd3);

			for(i++;i<LowestVertexes.GetSize();i++)
				if(LowestVertexes[i]!=(int)e.start && !hs.Inside(m_pVertexes[LowestVertexes[i]]))
					if(hs.OnPlane(m_pVertexes[LowestVertexes[i]]))
						CoLinearVertexes.Add(LowestVertexes[i]);
					else
					{

						//重新计算halfspace
						hs.Set(m_pVertexes[e.start],m_pVertexes[LowestVertexes[i]],m_pVertexes[e.start]+vd3);

						//清空，并重新计算共线点集
						CoLinearVertexes.RemoveAll();
						CoLinearVertexes.Add(LowestVertexes[i]);
					}			
			
			//循环结束
			if(CoLinearVertexes.GetSize()==1)
			{
				//仅一个共线点，则该点即为另一端点
				e.end = CoLinearVertexes[0];
				return e;
			}
			else
			{
				//对共线点集进行距离的排序
				SortByDist(e.start,CoLinearVertexes);

				//置最远点为e的另一个端点，并置其他点为无效
				for(int i=0;i<CoLinearVertexes.GetSize()-1;i++)
					m_pbVInvalid[CoLinearVertexes[i]]=true;

				e.end=CoLinearVertexes[CoLinearVertexes.GetSize()-1];
				return e;
			}			
		}
		else
		{
			//有多个点都取到了z最小
			//找出x最小和最大的情况
			int xminID=ZMinVertexes[0],xmaxID=ZMinVertexes[0];
			float XMin=m_pVertexes[ZMinVertexes[0]].x;
			float XMax=m_pVertexes[ZMinVertexes[0]].x;
			for(i=1;i<ZMinVertexes.GetSize();i++)
			{
				if(XMin-m_pVertexes[ZMinVertexes[i]].x>0.00001f)	// 说明比xMin更小
				{
					XMin=m_pVertexes[ZMinVertexes[i]].x;
					xminID=ZMinVertexes[i];
				}
				if(m_pVertexes[ZMinVertexes[i]].x-XMax>0.00001f)	// 说明比xMin更小
				{
					XMax=m_pVertexes[ZMinVertexes[i]].x;
					xmaxID=ZMinVertexes[i];
				}
			}

			//置ZMinVertexes中所有点为无效点
			for(i=0;i<ZMinVertexes.GetSize();i++)
				m_pbVInvalid[ZMinVertexes[i]]=true;
			
			m_pbVInvalid[xmaxID]=false;
			m_pbVInvalid[xminID]=false;
			m_pbExtremeVertex[xmaxID]=true;
			m_pbExtremeVertex[xminID]=true;

			e.start=xminID;
			e.end=xmaxID;
			return e;
		}

	}

}


// some interface for outer step-by-step call
bool CGiftWrap::IsOver()
{
	return m_EdgeStack.IsEmpty();
}



void CGiftWrap::GoOneStep()
{
	Edge e;
	e=m_EdgeStack.Pop();
	DealE(e);	
}

void CGiftWrap::Start()
{
		//先调用父类函数从而完成清空操作
	CConvexHullAlgorithm::ComputeConvexHull();
	Edge e=SearchFirstEdge();
	m_EdgeStack.Push(e);
	m_EdgeStack.Push(e);			//注意，这里应该push两次！从而保证在while条件中的弹出后，仍有一个边被保存在栈中！

	m_eFirst=e;
}

void CGiftWrap::Reset()
{
	CConvexHullAlgorithm::Reset();
	m_EdgeStack.Clear();
	m_bExceptionOccur=false;
}

void CGiftWrap::ResetSameVertices()
{
	CConvexHullAlgorithm::Reset();
	m_EdgeStack.Clear();
	m_bExceptionOccur=false;

	//初始化凸壳边界点集状态
	m_pbVInvalid=new bool[m_nVNum];
	m_pbExtremeVertex=new bool[m_nVNum];

	for(int i=0;i<m_nVNum;i++)
	{
		m_pbVInvalid[i]=false;
		m_pbExtremeVertex[i]=false;
	}

	//*m_pnIDsNum=0;		//从头开始计数！
}

//选择性压栈
//考察边e，看其是否已经被处理过了：即
//是否已经被两次压过栈，并弹出了
//对于这种情况，不再压栈。
//否则，将可能陷入死循环！
void CGiftWrap::SelectivePushStack(Edge e)
{
	if(m_EdgeStack.CheckPush(e))
	{
		//压栈成功，表明压栈前栈中没有e存在
		//此时应判断该边是否已经被处理过
		if(IsEdgeInCH(e))
		{
			//正常情况下，本算法不会出现一条边连续3次
			//被访问到！因此，这里出现了异常！
			m_bExceptionOccur=true;		//说明异常发生！
			
			e=m_EdgeStack.Pop();		//new interface!
		}

	}
}

//将顶点保存到文件
bool CGiftWrap::SaveVerticesToFile(const char *szFileName)
{
	FILE* OutFile = fopen(szFileName, "wt");;

	if(!OutFile)
		return false;

	fprintf(OutFile, "%d\n", m_nVNum);		//顶点数量
	for(int i=0;i<m_nVNum;i++)
		fprintf(OutFile, "%f %f %f\n", m_pVertexes[i].x, m_pVertexes[i].y, m_pVertexes[i].z);
	
	fclose(OutFile);
	return true;
}

//从文件中读取顶点信息
bool LoadVerticesFromFile(const char *szFileName,int& nVNum,A3DVECTOR3* & pVertices )
{
	FILE* InFile = fopen(szFileName, "rt");;
	if(!InFile)
		return false;

	fscanf(InFile, "%d\n", nVNum);
	pVertices=new A3DVECTOR3[nVNum];
	for(int i=0;i<nVNum;i++)
		fscanf(InFile,"%f %f %f\n",&pVertices[i].x,&pVertices[i].y,&pVertices[i].z);
	
	fclose(InFile);
	return true;

}

bool CGiftWrap::ValidConvexHull()
{
	// 检查共面的顶点是否会出现顶点到平面的距离超出阈值的情况
	CHalfSpace hs;
	for(int i=0; i<m_Planes.GetSize(); i++ )
	{
		hs.Set(m_pVertexes[m_Planes[i]->GetAt(0)], m_pVertexes[m_Planes[i]->GetAt(1)], m_pVertexes[m_Planes[i]->GetAt(2)]);

		for(int j=3; j<m_Planes[i]->GetSize(); j++)
		{
			if(fabs(hs.DistV2Plane(m_pVertexes[m_Planes[i]->GetAt(j)]) ) > CH_VALID_TOLERANCE )
				return false;
		}
	}

	return true;
}


}	// end namespace


