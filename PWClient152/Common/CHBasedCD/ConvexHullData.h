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
#define	CHDATA_BAD_FIT				0x00000001		//��־͹����ģ�͵Ľ���Ч�����ã���Ҫ����һ����ģ����ײ����CHEditor���õ���
#define CHDATA_NPC_CANNOT_PASS		0x00000002		//Ŀǰ���ڵ��α༭���У���־��͹���Ƿ���npc��ͨ����
#define CHDATA_SKIP_COLLISION		0x00000004		//��־�Ƿ�����ײ����к��Դ�͹��
#define CHDATA_SKIP_RAYTRACE		0x00000008		//��־�Ƿ������߸����к��Դ�͹��

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

	const CConvexHullData& operator=(const CConvexHullData& CHData);		//����һ����ֵ�����
	CConvexHullData(const CConvexHullData& CHData);									//����һ���������캯��

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
	void Import(const A3DOBB* pOBB, bool bImportVertices = true );		// ��OBB����CHDATA
	
	// Generate convex hull from a triangle directly
	bool Import(const A3DVECTOR3 Vertices[3], float fThickness = 0.01f);

	// ����ÿ��face�Ķ���߽�halfspace!
	void ComputeFaceExtraHS();

	// ��ConvexHullData��������任���任����ΪmtxTrans
	// ע�����ܴ�������任��ֻ���Ǵ�����ͬscaleֵ�ĸ��Ա任
	virtual void Transform(const A3DMATRIX4& mtxTrans);

	//���Ƶ�pFC,���У����ָ����pSpecialFace,������������ʾ
	void Render(A3DFlatCollector* pFC, bool bRenderV=false, CFace* pSpecialFace=NULL, DWORD dwVColor=0xffff0000, DWORD dwFColor=0xa0ffff00, const A3DVECTOR3& vRayDir=A3DVECTOR3(1.0f,1.0f,1.0f), const A3DMATRIX4* pTransMatrix=NULL);

	//���ã���ն������Ƭ����
	void Reset() 
	{
		m_arrVertices.RemoveAll();
		for(int i=0;i<m_arrFaces.GetSize();i++)
			delete m_arrFaces[i];
		m_arrFaces.RemoveAll();
	}

	//��Ӧһ���������ļ�����AFile
	virtual bool LoadBinaryData(AFile* InFile, bool bLoadVertices = true);
	virtual bool SaveBinaryData(AFile* OutFile) const;

	//��Ӧһ���ļ�FILE����
	virtual bool LoadFromStream(FILE* InFile);
	virtual bool SaveToStream(FILE* OutFile) const;

	/************************************************************************
	// Removed by wenfeng, 05-3-28
	// ���������г���std::fstream�Ŀ⣬����������ҪVC��֧�֣�

	//��Ӧһ���ļ���
	virtual bool LoadFromStream(std::ifstream& InFile);
	virtual bool SaveToStream(std::ofstream& OutFile) const;
	
	//��Ӧһ���ļ�
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

	// ��־�����úͻ�ȡ
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
	virtual int ComputeBufSize() const;					//���㱣�浽AFile�Ļ�������С
	virtual bool WriteToBuf(char* buf) const;		  //��ȫ������д�뵽������buf�У���ǰbuf�����Ѿ�����
	virtual bool ReadFromBuf(char* buf);				//��buf�ж�������

	void BuildAABB();
	
protected:
	DWORD		m_dwFlags;
	AArray<A3DVECTOR3,const A3DVECTOR3&> m_arrVertices;		//�����б�����
	AArray<CFace*, CFace*> m_arrFaces;						//������������б�����ֻ���ṩ�����ֶμ���
	int m_iVer;			// �汾��Ϣ�����ڴ�ȡ����

	// Added by wenfeng, 05-11-21
	A3DAABB m_aabb;		// keep record of convex hull's AABB.
};

}	// end namespace

#endif // _CONVEXHULLDATA_H_
