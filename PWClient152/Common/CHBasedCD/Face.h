//////////////////////////////////////////////////////////////////////
//	Created by He wenfeng
//  2004-11-25
//  A class describes the face of a ConvexHullData
//  It is derived from CHalfSpace and it consists 
//  a set of vertices'ids by order
//////////////////////////////////////////////////////////////////////

#ifndef	_FACE_H_
#define _FACE_H_

#define MAX_LEN	110

#include <AArray.h>
#include "HalfSpace.h"
//#include <fstream>

namespace CHBasedCD
{

class CConvexHullData;

class CFace : public CHalfSpace  
{
	static float AngleAcuteThresh;				// 两边夹锐角的阈值

public:

	const CFace& operator=(const CFace& face);		//重载一个赋值运算符
	CFace(const CFace& face);						//重载一个拷贝构造函数

	CFace();
	virtual ~CFace();
	
	static void SetAngleAcuteThresh(float AAThresh)		
	{
		AngleAcuteThresh=AAThresh;
	}

	void SetHS(const CHalfSpace& hs)
	{
		SetNormal(hs.GetNormal());
		SetD(hs.GetDist());
	}

	// 返回点vPos到m_arrEdgeHSs和m_arrExtraHSs中最短距离的Halfspace
	CHalfSpace GetNearestHS(const A3DVECTOR3& vPos )  const ;

	void SetCHData(CConvexHullData* pCHData) { m_pCHData=pCHData; }

	// 计算额外的边界半空间
	void ComputeExtraHS();

	// 对面片进行变换！变换矩阵为mtxTrans
	virtual void Transform(const A3DMATRIX4& mtxTrans);

	//重置，清空顶点和边平面数组
	void Reset() { m_arrVIDs.RemoveAll(); m_arrEdgeHSs.RemoveAll(); m_arrExtraHSs.RemoveAll(); }

	int GetEdgeNum() const { return m_arrVIDs.GetSize();}
	int GetVNum() const { return m_arrVIDs.GetSize();}
	int GetExtraHSNum() const { return m_arrExtraHSs.GetSize(); }

	CHalfSpace GetExtraHalfSpace(int id) const { return m_arrExtraHSs[id];}
	CHalfSpace GetEdgeHalfSpace(int eid) const { return m_arrEdgeHSs[eid];}
	int GetVID(int i) const { return m_arrVIDs[i];}

	void AddExtraHS(const CHalfSpace& hs) { m_arrExtraHSs.Add(hs); }

	//添加一个元素，必须同时添加一个vid和该vid与下一个id构成的边所对应的HalfSpace
	void AddElement(int vid,const CHalfSpace& hs){ m_arrVIDs.Add(vid);m_arrEdgeHSs.Add(hs);}

	/************************************************************************
	// Removed by wenfeng, 05-3-28
	// 避免引擎中出现std::fstream的库，由于其所需要VC的支持！

	//对应一个文件流
	bool LoadFromStream(std::ifstream& InFile);
	bool SaveToStream(std::ofstream& OutFile) const;
	/************************************************************************/

	//对应一个文件FILE对象
	bool LoadFromStream(FILE* InFile);
	bool SaveToStream(FILE* OutFile) const;	

protected:
	
	void AddExtraHS(int i);			// 对第i个顶点添加一个额外面片

private:
	CConvexHullData* m_pCHData;				// 记录所属的凸包数据指针
	AArray<int, int> m_arrVIDs;						//按顺序记录顶点的索引id；
	AArray<CHalfSpace,const CHalfSpace&> m_arrEdgeHSs;		//按顺序（同上）记录一条边和该面的法向决定的halfspace
	AArray<CHalfSpace,const CHalfSpace&> m_arrExtraHSs;		//处理尖锐夹角情况时附加的一组Hs!
};

}	// end namespace

#endif // _FACE_H_
