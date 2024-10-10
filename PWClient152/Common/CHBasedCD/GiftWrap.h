/////////////////////////////////////////////////
//	Created by He wenfeng 2004-8-17
//  A derived class from CConvexHullAlgorithm:
//  Gift Wrap Algorithm
//
//  Revised by He wenfeng 2004-12-2
//		Add some data and function to support 
//		the 2D Convex Hull!
//
/////////////////////////////////////////////////

#ifndef	_GIFTWRAP_H_
#define _GIFTWRAP_H_

#include "EdgeStack.h"
#include "ConvexHullAlgorithm.h"	// Added by ClassView
#include "ConvexPolytope.h"


namespace CHBasedCD
{


class CGiftWrap : public CConvexHullAlgorithm  
{
	//友元函数
	friend bool CConvexPolytope::Init(const CGiftWrap& gw);

public:
	enum { HULL_2D, HULL_3D };				//凸包的类型：二维或三维

	CGiftWrap();
	virtual ~CGiftWrap();
	
	//重置全部状态
	void Reset();						
	
	//单步执行的函数
	void Start();
	void GoOneStep();
	bool IsOver();

	CGiftWrap(A3DVECTOR3* pVertexes,int vNum);
	bool ExceptionOccur(){ return m_bExceptionOccur || ( ! ValidConvexHull() );}

//set && get operations
	void SetInitHSDistThresh(float fInitHSDistThresh) {m_fInitHSDistThresh=fInitHSDistThresh;}
	int GetHullType() const { return m_nHullType; }
	
	//应用于2D凸包的求解
	AArray<int,int>* GetCHVertecies() const { if(m_Planes.GetSize()==1) return m_Planes[0]; else return NULL;}

//override
	virtual void ComputeConvexHull();

protected:
	Edge SearchFirstEdge();
	int EVRelation(Edge e, int v);
	bool IsTriVsCoLinear(int v1,int v2,int v3);
	void SortByDist(int v, AArray<int,int> & vList);
	void DealCoPlanarV(Edge e, AArray<int,int> & CoPlanarVertexes);		
	bool DealE(Edge e);
	void SelectivePushStack(Edge e);
	void ResetSameVertices();			//重置全部状态，但仍作用再相同的顶点集上的

//Attributes
protected:

	CEdgeStack		m_EdgeStack;			//一个边堆栈
	Edge			m_eFirst;				//找到的第一条边	
	bool			m_bExceptionOccur;		//计算过程中出现异常，计算结果可能有错误
	
	float			m_fInitHSDistThresh;	//计算凸包的初始Halfspace距离阈值
	
	int				m_nHullType;			//计算出的凸包的类型。二维或三维


public:
	bool ValidConvexHull();			// 计算生成的凸包是否合理？

	bool SaveVerticesToFile(const char* szFileName);

};

//该函数为全局函数
bool LoadVerticesFromFile(const char *szFileName,int& nVNum,A3DVECTOR3* & pVertices );

}	// end namespace


#endif // _GIFTWRAP_H_
