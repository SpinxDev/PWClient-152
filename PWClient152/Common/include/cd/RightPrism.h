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
	
	//��һ��2DGiftWrap���������CH���Ե�ǰ���������г�ʼ����
	bool InitBase(const C2DGiftWrap& gw2D,float fHeight=1.0f);

	//���ؼ򻯺�ֱ������Ӧ��2D͹���Ķ������
	int Get2DCHVNum() { return m_listPatches.GetHead()->GetVNum();}
	
	//���ؼ򻯺�ֱ������Ӧ��2D͹���еĵ�vid�����㣬˳��Ϊ��-Y��ʱ�뷽��
	A3DVECTOR3 Get2DCHV(int vid){ return m_listPatches.GetHead()->GetVertex(vid);}
	
	//�����ײ��Ӧ����Ϣ������������һ�м̵�
	void GetCRInfo(CPatch* pCDPatch,const A3DVECTOR3& Dir,int& RotateDir,int& RelayVID,int CurRotateDir=0);
	
	int GetNextRelayVID(int CurRelayVID,int RotateDir);
	
	//�������ݵ�C2DObstruct�С�
	void Export2DObstruct(C2DObstruct* p2DObstruct);	

private:
	//���ڿ����ƺ�����Ҫ�����Ա����
	//float		m_fHeight;		//�����ĸߣ�������Ϊ��������Y=0.0f��
};

inline void CRightPrism::GetCRInfo(CPatch* pCDPatch,const A3DVECTOR3& Dir,int& RotateDir,int& RelayVID,int CurRotateDir)
{
	if(!pCDPatch) return;
	
	if(CurRotateDir>0)
	{
		//��ǰ�Ѵ�������״̬
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
	return -1;			//�������������һ����Чֵ
}

}	// end namespace

#endif // _RIGHTPRISM_H_
