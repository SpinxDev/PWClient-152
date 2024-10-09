/////////////////////////////////////////////////
//	Created by He wenfeng
//  2004-11-22
//  A derived class from CConvexPolytope:
//  Inplement a convex right prism whose bases is defined by a 2D Convex Hull.
//  By this class, we can implement the reduction of a 2D convex hull!
/////////////////////////////////////////////////

#ifndef	_RIGHTPRISM_H_
#define _RIGHTPRISM_H_

#include "ConvexPolytope.h"

namespace CHBasedCD
{


class C2DGiftWrap;
class C2DObstruct;

class CRightPrism : public CConvexPolytope  
{
public:
	CRightPrism();
	virtual ~CRightPrism();
	
	//用一个2DGiftWrap计算出来的CH来对当前的棱拄进行初始化！
	bool InitBase(const C2DGiftWrap& gw2D,float fHeight=1.0f);

	//返回简化后直棱拄对应的2D凸包的顶点个数
	int Get2DCHVNum() { return m_listPatches.GetHead()->GetVNum();}
	
	//返回简化后直棱拄对应的2D凸包中的第vid个顶点，顺序为向-Y逆时针方向！
	A3DVECTOR3 Get2DCHV(int vid){ return m_listPatches.GetHead()->GetVertex(vid);}
	
	//获得碰撞反应的信息，包括绕向，下一中继点
	void GetCRInfo(CPatch* pCDPatch,const A3DVECTOR3& Dir,int& RotateDir,int& RelayVID,int CurRotateDir=0);
	
	int GetNextRelayVID(int CurRelayVID,int RotateDir);
	
	//导出数据到C2DObstruct中。
	void Export2DObstruct(C2DObstruct* p2DObstruct);	

private:
	//现在看来似乎不需要这个成员变量
	//float		m_fHeight;		//棱拄的高，我们认为底座置于Y=0.0f处
};

inline void CRightPrism::GetCRInfo(CPatch* pCDPatch,const A3DVECTOR3& Dir,int& RotateDir,int& RelayVID,int CurRotateDir)
{
	if(!pCDPatch) return;
	
	if(CurRotateDir>0)
	{
		//当前已处于绕行状态
		RotateDir=CurRotateDir;
		RelayVID=(RotateDir==2 /* CSimplePFSphere::MOVE_ANTICLOCKWISE */)?pCDPatch->GetVID(3):pCDPatch->GetVID(0);
		return;
	}
	///////////////////////////////////////////////
	//			v1 ----> v2
	//		   /|\		 |
	//		     |	     \|/
	//			v0 <---- v3
	///////////////////////////////////////////////
	A3DVECTOR3 vAnticlockwise=pCDPatch->GetVertex(3)-pCDPatch->GetVertex(0);
	if(DotProduct(Dir,vAnticlockwise)>0.0f)
	{
		RotateDir=2 /* CSimplePFSphere::MOVE_ANTICLOCKWISE */;
		RelayVID=pCDPatch->GetVID(3);
	}
	else
	{
		RotateDir=1 /* CSimplePFSphere::MOVE_CLOCKWISE */;
		RelayVID=pCDPatch->GetVID(0);
	}
}

inline 	int CRightPrism::GetNextRelayVID(int CurRelayVID,int RotateDir)
{
	if(RotateDir==2 /* CSimplePFSphere::MOVE_ANTICLOCKWISE */)
	{
		return (CurRelayVID+2)%GetVNum();
	}
	else if(RotateDir==1 /* CSimplePFSphere::MOVE_CLOCKWISE */)
	{
		return (CurRelayVID+GetVNum()-2)%GetVNum();
	}
	return -1;			//其他情况，返回一个无效值
}

}	// end namespace

#endif // _RIGHTPRISM_H_
