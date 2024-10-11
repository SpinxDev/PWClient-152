/*
 * FILE: Patch.h
 *
 * DESCRIPTION: a class defines a planar patch of a convex polytope
 *				In fact,it is a polygon or a triangle.
 *
 * CREATED BY: He wenfeng, 2004/9/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_PATCH_H_
#define _PATCH_H_


#include "halfspace.h"
#include <AArray.h>
#include <A3DVector.h>

namespace CHBasedCD
{



class CPatch;
class CConvexPolytope;


/////////////////////////////////////////////////////////////////
//  A struct describes the vertex and its' corresponding patches
//		 of a patch in the polytopes
//	Created by: He wenfeng, 2004-9-20
/////////////////////////////////////////////////////////////////
struct VPNeighbor
{
//public members:
	int vid;						//顶点在polytope的全局id
	CPatch* pNeighborPatch;			//对应于 该顶点与下一个顶点构成的边的邻接平面片

	VPNeighbor() {pNeighborPatch=NULL;}
};

class CPatch : public CHalfSpace  
{
public:
	CPatch(CConvexPolytope* pCP);
	virtual ~CPatch();
	//overload
	const CPatch& operator=(const CPatch& patch);		//重写赋值函数
	CPatch(const CPatch& patch);						//重写复制构造函数

	void DecreVDegree();
	void IncreVDegree();	

	bool InNeighbors(CPatch* pPatch);

	bool Removed();							//该面被从多面体中删除
	void UpdateRemovedError();				//更新该平面面片的删除误差！
	void Neighbor(CPatch* pPatch);			//判断pPatch是否是当前patch的邻接patch，如果是，插入到相应的位置
	

//set && get operations
	//获得顶点的个数
	int GetVNum() { return m_arrNeighbors.GetSize();}		//获得顶点的数量
	//获得第vid个顶点
	A3DVECTOR3 GetVertex(int vid);

	int GetNeighborCount() { return m_arrNeighbors.GetSize();}
	//返回一个引用形式的量
	AArray<VPNeighbor,VPNeighbor>& GetNeighbors(){	return m_arrNeighbors; }
	float GetRemovedError() { return m_fRemovedError;}

	void GetEdge(int id,A3DVECTOR3& v1,A3DVECTOR3& v2);
	
	int GetVID(int i) { return m_arrNeighbors[i].vid; }
	
protected:
	// 判断PatchesIntersectV中是否已经有相交于一个顶点的面片被包含在PIntersectV中
	int HasPIntersectVExist(AArray<int,int>* pPIntersectV,const AArray<AArray<int,int>*,AArray<int,int>*>& PatchesIntersectV);
	bool IsVAdjacent(AArray<int,int>* pArr1, AArray<int,int>* pArr2);						
	bool InArray(int pid, AArray<int,int>* pArr);		//pid是否出现在动态数组pArr中
	bool VInPatch(int vid);								//判断顶点vid是否为当前面片的一个顶点

	bool Processed(int n[3], AArray<AArray<int,int>*,AArray<int,int>*>& CoSolutionList);
	bool Solve3NPIntersection(int n[3], A3DVECTOR3& vIntersection);
	
	//返回第i个邻域顶点的下一个顶点的全局id，从而与当前邻域点构成一个邻边！
	int GetNextV(int i) { return ((i+1<m_arrNeighbors.GetSize())?m_arrNeighbors[i+1].vid:m_arrNeighbors[0].vid);}
	int GetNext(int i) {return ((i+1<m_arrNeighbors.GetSize())?i+1:0);}
	int GetPre(int i) {return (i-1<0?m_arrNeighbors.GetSize()-1:i-1);}


//Attributes
private:
	float m_fRemovedError;								//移除误差：如果将该面片从多面体中移除，所带来的误差！
	AArray<VPNeighbor,VPNeighbor> m_arrNeighbors;		//顶点及相邻的面片
	CConvexPolytope* m_pConvexPolytope;					//该面片所属的多面体的指针；
};

}	// end namespace

#endif // _PATCH_H_
