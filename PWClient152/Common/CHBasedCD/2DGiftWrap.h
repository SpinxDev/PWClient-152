/////////////////////////////////////////////////
//	created by He wenfeng
//  2004-11-22
//  A derived class from CGiftWrap:
//  Gift Wrap Algorithm in 2D space(XOZ space)
//  ��CGiftWrap��ͬ��C2DGiftWrap����������ȫ���������ڲ����棡
/////////////////////////////////////////////////

#ifndef	_2D_GIFTWRAP_H_
#define _2D_GIFTWRAP_H_

#include "RightPrism.h"
#include "GiftWrap.h"

namespace CHBasedCD
{
	
class C2DObstruct;

class C2DGiftWrap : public CGiftWrap  
{
	//����������Ϊ������Ϊ�˱���ͷ�ļ��ĵ�������
	friend bool CRightPrism::InitBase(const C2DGiftWrap& gw2D,float fHeight);

public:
	C2DGiftWrap();
	virtual ~C2DGiftWrap();
	
	//���ö�����Ϣ��A3DVECTOR3��ʽ������
	virtual void SetVertexes(A3DVECTOR3* pVertexes,int vNum);	

	//���ö�����Ϣ��X,Z��ʽ������
	void SetVertexes(float* Xs,float* Zs,int vNum);

	//���͹���ϵĶ��㣬�����˳��Ϊ��-Y��������ʱ��
	AArray<int,int>* GetCHVertecies() const { if(m_Planes.GetSize()==1) return m_Planes[0]; else return NULL;}

	//�������ݵ�C2DObstruct�С�
	void Export2DObstruct(C2DObstruct* p2DObstruct);	
	
	//override
	virtual void ComputeConvexHull();	

	
};

}	// end namespace

#endif // _2D_GIFTWRAP_H_
