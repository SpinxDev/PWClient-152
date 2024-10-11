/*
 * FILE: ConvexPolytope.h
 *
 * DESCRIPTION: a class defines a convex polytope
 *
 * CREATED BY: He wenfeng, 2004/9/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_CONVEXPOLYTOPE_H_
#define _CONVEXPOLYTOPE_H_

#include "Patch.h"
#include <AList2.h>
#include <AList.h>
#include <A3DFlatCollector.h>

namespace CHBasedCD
{

class CConvexHullData;

/////////////////////////////////////////////////////////////////
//  A struct describes the information of a vertex in Convex Polytope
//	Created by: He wenfeng, 2004-8-17
/////////////////////////////////////////////////////////////////
struct VertexInfo
{

//public members:

	bool bValid;					//是否为有效顶点，无效顶点不应出现在多面体的任何一个面片之中
	char cDegree;					//顶点的度数，这里标志有多少个面片过此顶点，或与该顶点相邻接！
	AList* pNeighborPatches;		//过此顶点的面片列表，暂时不使用，保留！

	VertexInfo() 
	{
		bValid=true;				//缺省的，初始化时顶点有效
		cDegree=0;					//初始化为0，表示一个无效的度数值，可在此基础上做增1操作
		pNeighborPatches=NULL;		//保留，暂不使用
	}
};

class CGiftWrap;
class C2DGiftWrap;


class CConvexPolytope  
{
	static float Hull2D_Half_Thickness;		//2D Convex Hull生成ConvexPolytope时的一半厚度

public:
	A3DAABB GetAABB();

	CPatch* IsVertexOnPatch(A3DVECTOR3 v, float fInflateRadius);
	bool IsVertexInside(A3DVECTOR3 v,float fInflateRadius);
	bool IsVertexOutside(A3DVECTOR3 v,float fInflateRadius=0.0f);
	CConvexPolytope();
	virtual ~CConvexPolytope();
	
	static void SetHull2DThickness(float fThickness) { Hull2D_Half_Thickness=fThickness*0.5f;}

	bool Init(const CGiftWrap& gw);		//此方法为CGiftWrap的一个友元
	
	bool Init(const CGiftWrap& gw,float fErrorBase)
	{
		//重载一个Init的方法，避免顺序耦合
		SetErrorBase(fErrorBase);
		return Init(gw);
	}

	void Goto(float fError=0.1f);	//直接跳到误差不大于fError指定的简化层次，缺省为10%
	bool Goto(int LeftPatchesNum);

	void UndoRemove();				//撤销操作，向前，即恢复到后面的操作
	void RedoRemove();				//撤销操作，向后，即回溯到以前的操作
	
	void ReduceAll();					//删除所有能删的面片，简化到头！

	bool RedoAll();					//恢复到头？
	bool UndoAll();					//撤销到头？

	//for render
	void Render(A3DFlatCollector* pFC, A3DMATRIX4& tMatrix,DWORD dwColor,CPatch* pSpecialPatch);
	void RenderLEPatchSpecial(A3DFlatCollector *pFC,A3DMATRIX4& tMatrix, DWORD dwColor);	
	
	//判断是否发生了异常
	bool ExceptionOccur() { return m_bExceptionOccur;}
	void ThrowException() { m_bExceptionOccur=true;}

	//添加一个顶点
	void AddV(A3DVECTOR3 &v,VertexInfo &vInfo){ m_arrVertecies.Add(v);m_arrVertexInfo.Add(vInfo);}

	//到处数据到CConvexHullData中
	void ExportCHData(CConvexHullData* pCHData);	

	bool IntersectPlanesProj2XOZ(CHalfSpace* Planes,int PlaneNum,C2DGiftWrap& gw2d,bool b2ParallelPlanes=false);
	
//set && get operations
	A3DVECTOR3 GetCentroid() { return m_vCentroid;}
	
	void SetErrorBase(float fErrorBase) { m_fErrorBase=fErrorBase;}
	float GetErrorBase() { return m_fErrorBase;}

	int GetVNum() { return m_arrVertecies.GetSize();}
	VertexInfo& GetVInfo(int vid) { return m_arrVertexInfo[vid];}
	A3DVECTOR3 GetV(int vid) { return m_arrVertecies[vid];}

	int GetPatchesNum() { return m_listPatches.GetCount(); }			//当前面片的总数
	int GetOriginPatchNum() { return m_nOriginPatchNum;}
	int GetMinPatchNum() { return m_nMinPatchNum;}

	float GetCurLeastError() 
	{ 
		if(m_pCurLeastErrorPatch) return m_pCurLeastErrorPatch->GetRemovedError();
		else return -1.0f;		//否则，一个无效值
	}

	AList2<CPatch*,CPatch*>& GetPatchesList() { return m_listPatches;}
	
	
protected:
	CPatch* SearchLeastErrorPatch();
	bool RemovePatch(CPatch* pPatch);		//删除一个面片
	bool RemoveLeastErrorPatch();
	int GetLPNByError(float fError);
	
	void ComputePatchNeighbors();						//计算各个面片的领域面片，私有，应该在Init()内最后调用
	void Reset();										//清空所有的资源，重置为初始状态！

// Attributes
protected:
	A3DVECTOR3	m_vCentroid;							//待计算的整个点集的质心
	
	AArray<A3DVECTOR3,A3DVECTOR3> m_arrVertecies;		//顶点列表，只记录纯几何信息，从而方便绘制
	AArray<VertexInfo,VertexInfo> m_arrVertexInfo;		//顶点的相关信息列表，应和顶点列表保持一致
	AList2<CPatch*,CPatch*> m_listPatches;				//所有的面片列表，采用链表结构存储，便于删除操作！
	
	int m_nOriginPatchNum;
	int m_nMinPatchNum;									//简化到头时，所乘面片的数量！实际证明该数量不一定为4，例如一个Cube，其各个面都无法简化了，因此，其面片数量为6
	
	int m_nCurVNum;										//当前有效的顶点数，由于每次在删除一个面时，会增加顶点，该变量记录了当前层次时的顶点数量

	WORD* m_pIndices;									//绘制时采用三角形方法，用以三角形的顶点id!
	WORD* m_pLEPIndices;								//绘制当前最小误差面片的三角形顶点id
	WORD* m_pLEPNIndices;								//绘制当前最小误差面片的邻域面片三角形顶点id

	A3DVECTOR3* m_pVerticesForRender;					//经过变换后得到的顶点的坐标，直接用于绘制
	
	CPatch* m_pCurLeastErrorPatch;						//记录当前删除误差最小的面片

	float			m_fErrorBase;						//将删除误差转化为相对误差的分母；
	float*			m_fArrRemovedError;					//删除误差数组，记录下来每一次的删除误差

	bool			m_bExceptionOccur;		//简化出现异常，最终不能简化成凸4面体！

	//一些嵌套结构或类，置为私有，因此只能在本类域内部使用
	
	//删除面片的邻域备份结构
	struct NeighborBak
	{
		/*
		CPatch* pBeforeRemoved;			//删除前的邻域指针
		CPatch* pAfterRemoved;			//更新后的邻域指针
		*/
		CPatch* pPatchCur;				//当前邻域在m_listPatches指针
		CPatch* pPatchBak;				//保存邻域指针内容的一个备份
		NeighborBak(CPatch* pCur,CPatch* pBak) { pPatchCur=pCur; pPatchBak=pBak;}
		NeighborBak() {pPatchCur=NULL;pPatchBak=NULL;}
	};

	struct RemoveOperatorRecord				//一次删除操作所需记录的信息
	{
		CPatch* pRemoved;									//被删除的面片指针
		AArray<NeighborBak,NeighborBak> arrNeighborBak;		//其邻域信息
		int VNumAdded;										//增加的顶点数量
	};

	//如果直接使用局部对象，则在其析构时，指针可能会出现问题
	AArray<RemoveOperatorRecord*,RemoveOperatorRecord*> m_arrRemoveOperatorRecords;	//记录依次删除操作的数组，从而可以完成撤销，反复等操作
	int m_nCurOperator;				//当前操作所在位置
};

//全局函数
int FindInArray(int id,const AArray<int,int>& arr);
void AddDifferentV(AArray<A3DVECTOR3,A3DVECTOR3>& Vertices, A3DVECTOR3& v);

}	// end namespace

#endif // _CONVEXPOLYTOPE_H_
