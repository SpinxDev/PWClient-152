//////////////////////////////////////////////////////////////////////
//	Created by He wenfeng
//  2005-01-10
//  A class derived from the class CConvexHullData 
//  It is used for Collision Detection and Path finding in 2 2D environment.
//  It uses the form of CConvexHullData, while, in fact it only represents a 2D obstruct
//  To note is that it is not  convex when it describes the Terrain obstruct or water area!
//////////////////////////////////////////////////////////////////////

#ifndef _2DOBSTRUCT_H_
#define _2DOBSTRUCT_H_

#include "convexhulldata.h"

#include <vector.h>
using namespace abase;

//extern ALog g_Log;

namespace CHBasedCD
{

class C2DObstruct : public CConvexHullData  
{
	//定义两个方向
	enum 
	{
		CLOCKWISE=1,
		ANTICLOCKWISE
	};

public:

	C2DObstruct();
	virtual ~C2DObstruct();
	
	//对数据进行变换
	virtual void Transform(const A3DMATRIX4& mtxTrans);

	//////////////////////////////////////////////////////////////////////////
	// 文件操作：保存和装载
	// 增加了成员m_arrInflateVecs，因此须覆盖基类的方法！
	//////////////////////////////////////////////////////////////////////////
	//对应一个文件FILE对象
	virtual bool LoadFromStream(FILE* InFile);
	virtual bool SaveToStream(FILE* OutFile) const;

	/************************************************************************
	// Removed by wenfeng, 05-3-28
	// 避免引擎中出现std::fstream的库，由于其所需要VC的支持！

	//对应一个文件流
	virtual bool LoadFromStream(std::ifstream& InFile);
	virtual bool SaveToStream(std::ofstream& OutFile) const;
	/************************************************************************/

	// 判断某一顶点是否为凹点！
	bool IsVertexConcave(int vid);

	//利用一组点集初始化，点集采用三维向量形式，但有效的仅为x,z坐标
	void Init(const vector<A3DVECTOR3>& Vertices, float fHeight=1.0f);

	///////////////////////////////////////////////////
	// 用于碰撞后的移动策略
	///////////////////////////////////////////////////

	//获得碰撞反应的信息，包括绕向，下一中继点
	void GetCRInfo(CFace* pCDFace,const A3DVECTOR3& Dir,int& RotateDir,int& RelayVID,int CurRotateDir=0);

	
	//根据方向找出下一个顶点id
	inline int GetNextVID(int CurVID,int RotateDir);

	//根据旋绕方向找到与vid邻接的Face
	//vid应该是底面上的id，所以应该为偶数
	inline CFace* GetFaceByVID(int vid, int RotateDir);

	A3DVECTOR3 GetInflateVector(int vid) {  return m_arrInflateVecs[vid]; }
	
	//对顶点作一个膨胀
	A3DVECTOR3 GetInflateVertex(int vid, float fInflateSize)
	{
		return (m_arrVertices[vid]+fInflateSize*m_arrInflateVecs[vid>>1]);
	}

protected:
	//覆盖基类的方法
	virtual int ComputeBufSize() const;					//计算保存到AFile的缓冲区大小
	virtual bool WriteToBuf(char* buf) const;		  //将全部数据写入到缓冲区buf中，此前buf必须已经分配
	virtual bool ReadFromBuf(char* buf);				//从buf中读出数据

protected:
	
	//膨胀向量列表，对应于每一个2D顶点。用于碰撞移动策略中对顶点的外扩操作！
	//设对应顶点v的膨胀向量为vInflate,则对于半径为r的球体，该顶点将被外扩至v+r*vInflate
	AArray<A3DVECTOR3,const A3DVECTOR3&> m_arrInflateVecs;		

};

inline int C2DObstruct::GetNextVID(int CurVID,int RotateDir)
{
	if(RotateDir==ANTICLOCKWISE) 
	{
		return (CurVID+2)%GetVertexNum();
	}
	else if(RotateDir==CLOCKWISE )
	{
		return (CurVID+GetVertexNum()-2)%GetVertexNum();
	}
	return -1;			//其他情况，返回一个无效值
}

inline CFace* C2DObstruct::GetFaceByVID(int vid, int RotateDir)
{
	int fid;
	if(RotateDir==CLOCKWISE )
		fid=GetNextVID(vid,RotateDir);
	else
		fid=vid;
	fid=(fid>>1)+2;			//这里一定要注意优先级 ＋高于>>!

	if(fid<0 || fid>GetFaceNum()-1)
		return NULL;
	
	//注意！应避免使用&m_arrFaces[i]
	return m_arrFaces[fid];
	
}

}	// end namespace

#endif // _2DOBSTRUCT_H_
