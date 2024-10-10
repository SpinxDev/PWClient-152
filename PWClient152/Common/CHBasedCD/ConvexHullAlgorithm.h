// ConvexHullAlgorithm.h: interface for the CConvexHullAlgorithm class.
//
//////////////////////////////////////////////////////////////////////

#ifndef	_CONVEXHULL_ALGORITHM_H_
#define _CONVEXHULL_ALGORITHM_H_

#include <A3DVector.h>
#include <AArray.h>

namespace CHBasedCD
{


//用顶点索引的方式定义结构边、面
struct Face;

struct Edge
{
	unsigned int start;
	unsigned int end;

	Edge() {}		//缺省构造函数,do nothing
	Edge(unsigned int v1,unsigned int v2){Set(v1,v2);}
	void Set(unsigned int v1,unsigned int v2){start=v1; end=v2;}
	bool InFace(Face f);
};

struct Face
{
	unsigned int v1;
	unsigned int v2;
	unsigned int v3;
	
	//添加于2004-9-20
	//判断该三角形是否为通过多边形构建的，即在生成凸壳的算法中
	//有些三角形的产生是为了最后的绘制，其实他们是一个大的多边形的一部分！
	//对于标记为true的三角形，我们不用在将其其添加至CConvexPolytope的patch中
	//而仅需直接将其所处的polygon添加一次即可
	bool InPolygon;			

	Face() {}		//缺省构造函数,do nothing
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
	virtual void SetVertexes(A3DVECTOR3* pVertexes,int vNum,bool bTranslate=false);	//修改成虚函数，可能在子类中覆盖
	AArray<Face,Face>& GetCHFaces() { return m_Faces;}	//大型对象，用引用方式返回

//virtual operations
	virtual void ComputeConvexHull()=0;		//纯虚，必须在子类中重载	

//Attributes
protected:
	A3DVECTOR3				m_vCentroid;	//待计算的整个点集的质心
	
	A3DVECTOR3 *			m_pVertexes;	//顶点集合，以数组方式组织，
											//注意该变量仅为该类和接口调用者的一个引用，并不负责new和delete!

	int						m_nVNum;		//顶点总数量
	
	AArray<Face,Face>		m_Faces;		//根据顶点计算出的凸壳，用面片表示
	
	bool*					m_pbVInvalid;	//对应于每一个顶点，标志该点是否处于共面共线的情况而被视为无效
	
	bool*					m_pbExtremeVertex;		//对应于每一个顶点，标志该点是否为Extreme Vertex!凸壳上的点
	
	//修改后发现这样速度会减慢，原因显然是m_planes的数量大幅增加
	//因此，在一些在m_planes中查找face和edge的操作会大大增加时间复杂度！
	//故此修改方案放弃！


	//修改于2004-9-20
	//为了便于构造相应的凸多面体，这里修改m_Planes，使其包含所有的平面
	//包括三角形！以前的定义不含三角形！

	//对应于所有多点(>3)共面的情况，这些平面集将被添加至此列表中
	AArray<AArray<int,int>*,AArray<int,int>*>		m_Planes;
};


}	// end namespace

#endif // _CONVEXHULL_ALGORITHM_H_