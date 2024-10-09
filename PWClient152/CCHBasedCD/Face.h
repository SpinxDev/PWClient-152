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
	static float AngleAcuteThresh;				// ���߼���ǵ���ֵ

public:

	const CFace& operator=(const CFace& face);		//����һ����ֵ�����
	CFace(const CFace& face);						//����һ���������캯��

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

	// ���ص�vPos��m_arrEdgeHSs��m_arrExtraHSs����̾����Halfspace
	CHalfSpace GetNearestHS(const A3DVECTOR3& vPos )  const ;

	void SetCHData(CConvexHullData* pCHData) { m_pCHData=pCHData; }

	// �������ı߽��ռ�
	void ComputeExtraHS();

	// ����Ƭ���б任���任����ΪmtxTrans
	virtual void Transform(const A3DMATRIX4& mtxTrans);

	//���ã���ն���ͱ�ƽ������
	void Reset() { m_arrVIDs.RemoveAll(); m_arrEdgeHSs.RemoveAll(); m_arrExtraHSs.RemoveAll(); }

	int GetEdgeNum() const { return m_arrVIDs.GetSize();}
	int GetVNum() const { return m_arrVIDs.GetSize();}
	int GetExtraHSNum() const { return m_arrExtraHSs.GetSize(); }

	CHalfSpace GetExtraHalfSpace(int id) const { return m_arrExtraHSs[id];}
	CHalfSpace GetEdgeHalfSpace(int eid) const { return m_arrEdgeHSs[eid];}
	int GetVID(int i) const { return m_arrVIDs[i];}

	void AddExtraHS(const CHalfSpace& hs) { m_arrExtraHSs.Add(hs); }

	//���һ��Ԫ�أ�����ͬʱ���һ��vid�͸�vid����һ��id���ɵı�����Ӧ��HalfSpace
	void AddElement(int vid,const CHalfSpace& hs){ m_arrVIDs.Add(vid);m_arrEdgeHSs.Add(hs);}

	/************************************************************************
	// Removed by wenfeng, 05-3-28
	// ���������г���std::fstream�Ŀ⣬����������ҪVC��֧�֣�

	//��Ӧһ���ļ���
	bool LoadFromStream(std::ifstream& InFile);
	bool SaveToStream(std::ofstream& OutFile) const;
	/************************************************************************/

	//��Ӧһ���ļ�FILE����
	bool LoadFromStream(FILE* InFile);
	bool SaveToStream(FILE* OutFile) const;	

protected:
	
	void AddExtraHS(int i);			// �Ե�i���������һ��������Ƭ

private:
	CConvexHullData* m_pCHData;				// ��¼������͹������ָ��
	AArray<int, int> m_arrVIDs;						//��˳���¼���������id��
	AArray<CHalfSpace,const CHalfSpace&> m_arrEdgeHSs;		//��˳��ͬ�ϣ���¼һ���ߺ͸���ķ��������halfspace
	AArray<CHalfSpace,const CHalfSpace&> m_arrExtraHSs;		//�������н����ʱ���ӵ�һ��Hs!
};

}	// end namespace

#endif // _FACE_H_
