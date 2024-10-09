/////////////////////////////////////////////////
//	created by He wenfeng
//  2004-11-22
//  A derived class from CGiftWrap:
//  Gift Wrap Algorithm in 2D space(XOZ space)
//  与CGiftWrap不同，C2DGiftWrap将顶点数据全部放在类内部保存！
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
	//采用引用作为参数是为了避免头文件的迭代包含
	friend bool CRightPrism::InitBase(const C2DGiftWrap& gw2D,float fHeight);

public:
	C2DGiftWrap();
	virtual ~C2DGiftWrap();
	
	//设置顶点信息，A3DVECTOR3形式的数据
	virtual void SetVertexes(A3DVECTOR3* pVertexes,int vNum);	

	//设置顶点信息，X,Z形式的数据
	void SetVertexes(float* Xs,float* Zs,int vNum);

	//获得凸包上的顶点，顶点的顺序为向-Y方向看是逆时针
	AArray<int,int>* GetCHVertecies() const { if(m_Planes.GetSize()==1) return m_Planes[0]; else return NULL;}

	//导出数据到C2DObstruct中。
	void Export2DObstruct(C2DObstruct* p2DObstruct);	
	
	//override
	virtual void ComputeConvexHull();	

	
};

}	// end namespace

#endif // _2D_GIFTWRAP_H_
