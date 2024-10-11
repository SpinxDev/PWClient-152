  //////////////////////////////////////////////////////////////////////
//	Created by He wenfeng
//  2004-11-25
//  A class describes a ConvexHullData which is used for Collision Detection
//  It consists a set of Vertices by order and a set of CFaces
//////////////////////////////////////////////////////////////////////

#ifndef	_CONVEXHULLDATA_H_
#define _CONVEXHULLDATA_H_

#include <A3DVector.h>
#include <A3DGeometry.h>
#include "Face.h"

#define DW_BITWISE_NOT(x) (0xFFFFFFFF ^ (x))	   //bitwise not!

// a set of DWORD flags
#define	CHDATA_BAD_FIT				0x00000001		//标志凸包对模型的近似效果不好，需要做进一步的模型碰撞，在CHEditor中用到。
#define CHDATA_NPC_CANNOT_PASS		0x00000002		//目前用于地形编辑器中，标志该凸包是否让npc可通过。
#define CHDATA_SKIP_COLLISION		0x00000004		//标志是否在碰撞检测中忽略此凸包
#define CHDATA_SKIP_RAYTRACE		0x00000008		//标志是否在射线跟踪中忽略此凸包

class A3DFlatCollector;
class AFile;
class A3DOBB;

namespace CHBasedCD
{

class CConvexHullData  
{
public:
	
	CConvexHullData();
	virtual ~CConvexHullData();

	const CConvexHullData& operator=(const CConvexHullData& CHData);		//重载一个赋值运算符
	CConvexHullData(const CConvexHullData& CHData);									//重载一个拷贝构造函数

	//////////////////////////////////////////////////
	// Test if two convex hull overlap.
	// Return value:
	//		0: no overlap
	//		1: overlap partly
	//		2: this CH is fully inside Another CH
	//		3: Another CH is fully inside this CH
	//////////////////////////////////////////////////
	int ConvexHullOverlapTest(const CConvexHullData& AnotherCH) const;

	// Generate convex hull from OBB directly
	void Import(const A3DOBB* pOBB, bool bImportVertices = true );		// 从OBB导入CHDATA
	
	// Generate convex hull from a triangle directly
	bool Import(const A3DVECTOR3 Vertices[3], float fThickness = 0.01f);

	// 计算每个face的额外边界halfspace!
	void ComputeFaceExtraHS();

	// 对ConvexHullData进行坐标变换！变换矩阵为mtxTrans
	// 注：不能处理任意变换，只能是带有相同scale值的刚性变换
	virtual void Transform(const A3DMATRIX4& mtxTrans);

	//绘制到pFC,其中，如果指定了pSpecialFace,将对其特殊显示
	void Render(A3DFlatCollector* pFC, bool bRenderV=false, CFace* pSpecialFace=NULL, DWORD dwVColor=0xffff0000, DWORD dwFColor=0xa0ffff00, const A3DVECTOR3& vRayDir=A3DVECTOR3(1.0f,1.0f,1.0f), const A3DMATRIX4* pTransMatrix=NULL);

	//重置，清空顶点和面片数组
	void Reset() 
	{
		m_arrVertices.RemoveAll();
		for(int i=0;i<m_arrFaces.GetSize();i++)
			delete m_arrFaces[i];
		m_arrFaces.RemoveAll();
	}

	//对应一个二进制文件对象AFile
	virtual bool LoadBinaryData(AFile* InFile, bool bLoadVertices = true);
	virtual bool SaveBinaryData(AFile* OutFile) const;

	//对应一个文件FILE对象
	virtual bool LoadFromStream(FILE* InFile);
	virtual bool SaveToStream(FILE* OutFile) const;

	/************************************************************************
	// Removed by wenfeng, 05-3-28
	// 避免引擎中出现std::fstream的库，由于其所需要VC的支持！

	//对应一个文件流
	virtual bool LoadFromStream(std::ifstream& InFile);
	virtual bool SaveToStream(std::ofstream& OutFile) const;
	
	//对应一个文件
	virtual bool LoadFromFile(const char* szFileName);
	virtual bool SaveToFile(const char* szFileName) const;
	
	/************************************************************************/

	bool HaveVertexDataLoaded()
	{
		return ( m_arrVertices.GetSize()==0 && m_arrFaces.GetSize() >0 );
	}

	int GetVertexNum() const { return m_arrVertices.GetSize(); }
	int GetFaceNum() const { return m_arrFaces.GetSize();}
	bool IsEmpty() { return GetFaceNum() == 0 || GetVertexNum() < 2; }

	A3DVECTOR3 GetVertex(int vid) const { return m_arrVertices[vid];}
	CFace& GetFace(int fid) const { return *(m_arrFaces[fid]);	}
	CFace* GetFacePtr(int fid)  const { return m_arrFaces[fid];}

	// build the aabb at runtime.
	bool GetAABB(A3DAABB& aabb)
	{
		if ( GetVertexNum() < 2 ) return false;

		aabb.Build( m_arrVertices.GetData(), m_arrVertices.GetSize());
		return true;
	}
	
	// get the aabb we precomputed.
	A3DAABB GetAABB() const
	{
		return m_aabb;
	}

	void AddVertex(const A3DVECTOR3& v) { m_arrVertices.Add(v); }
	void AddFace(const CFace& f) 
	{	
		CFace* pFace = new CFace(f);
		pFace->SetCHData(this);
		m_arrFaces.Add(pFace);
	}

	void AddFace(const CFace* pF)
	{
		CFace *pFace = new CFace(*pF);
		pFace->SetCHData(this);
		m_arrFaces.Add(pFace);
	}

	// 标志的设置和获取
	void SetFlags(DWORD flags) { m_dwFlags=flags;}
	DWORD GetFlags() { return m_dwFlags; }

	void SetBadFitFlag(bool bBadFit=true) 
	{ 
		if(bBadFit)
			m_dwFlags|=CHDATA_BAD_FIT;
		else
			m_dwFlags&=DW_BITWISE_NOT(CHDATA_BAD_FIT);
	}
	bool IsBadFit() { return (m_dwFlags & CHDATA_BAD_FIT);}

protected:
	virtual int ComputeBufSize() const;					//计算保存到AFile的缓冲区大小
	virtual bool WriteToBuf(char* buf) const;		  //将全部数据写入到缓冲区buf中，此前buf必须已经分配
	virtual bool ReadFromBuf(char* buf);				//从buf中读出数据

	void BuildAABB();
	
protected:
	DWORD		m_dwFlags;
	AArray<A3DVECTOR3,const A3DVECTOR3&> m_arrVertices;		//顶点列表，有序
	AArray<CFace*, CFace*> m_arrFaces;						//所包含的面的列表，无序，只需提供遍历手段即可
	int m_iVer;			// 版本信息，用于存取操作

	// Added by wenfeng, 05-11-21
	A3DAABB m_aabb;		// keep record of convex hull's AABB.
};

}	// end namespace

#endif // _CONVEXHULLDATA_H_
