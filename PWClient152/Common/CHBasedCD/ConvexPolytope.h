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

	bool bValid;					//�Ƿ�Ϊ��Ч���㣬��Ч���㲻Ӧ�����ڶ�������κ�һ����Ƭ֮��
	char cDegree;					//����Ķ����������־�ж��ٸ���Ƭ���˶��㣬����ö������ڽӣ�
	AList* pNeighborPatches;		//���˶������Ƭ�б���ʱ��ʹ�ã�������

	VertexInfo() 
	{
		bValid=true;				//ȱʡ�ģ���ʼ��ʱ������Ч
		cDegree=0;					//��ʼ��Ϊ0����ʾһ����Ч�Ķ���ֵ�����ڴ˻���������1����
		pNeighborPatches=NULL;		//�������ݲ�ʹ��
	}
};

class CGiftWrap;
class C2DGiftWrap;


class CConvexPolytope  
{
	static float Hull2D_Half_Thickness;		//2D Convex Hull����ConvexPolytopeʱ��һ����

public:
	A3DAABB GetAABB();

	CPatch* IsVertexOnPatch(A3DVECTOR3 v, float fInflateRadius);
	bool IsVertexInside(A3DVECTOR3 v,float fInflateRadius);
	bool IsVertexOutside(A3DVECTOR3 v,float fInflateRadius=0.0f);
	CConvexPolytope();
	virtual ~CConvexPolytope();
	
	static void SetHull2DThickness(float fThickness) { Hull2D_Half_Thickness=fThickness*0.5f;}

	bool Init(const CGiftWrap& gw);		//�˷���ΪCGiftWrap��һ����Ԫ
	
	bool Init(const CGiftWrap& gw,float fErrorBase)
	{
		//����һ��Init�ķ���������˳�����
		SetErrorBase(fErrorBase);
		return Init(gw);
	}

	void Goto(float fError=0.1f);	//ֱ������������fErrorָ���ļ򻯲�Σ�ȱʡΪ10%
	bool Goto(int LeftPatchesNum);

	void UndoRemove();				//������������ǰ�����ָ�������Ĳ���
	void RedoRemove();				//������������󣬼����ݵ���ǰ�Ĳ���
	
	void ReduceAll();					//ɾ��������ɾ����Ƭ���򻯵�ͷ��

	bool RedoAll();					//�ָ���ͷ��
	bool UndoAll();					//������ͷ��

	//for render
	void Render(A3DFlatCollector* pFC, A3DMATRIX4& tMatrix,DWORD dwColor,CPatch* pSpecialPatch);
	void RenderLEPatchSpecial(A3DFlatCollector *pFC,A3DMATRIX4& tMatrix, DWORD dwColor);	
	
	//�ж��Ƿ������쳣
	bool ExceptionOccur() { return m_bExceptionOccur;}
	void ThrowException() { m_bExceptionOccur=true;}

	//���һ������
	void AddV(A3DVECTOR3 &v,VertexInfo &vInfo){ m_arrVertecies.Add(v);m_arrVertexInfo.Add(vInfo);}

	//�������ݵ�CConvexHullData��
	void ExportCHData(CConvexHullData* pCHData);	

	bool IntersectPlanesProj2XOZ(CHalfSpace* Planes,int PlaneNum,C2DGiftWrap& gw2d,bool b2ParallelPlanes=false);
	
//set && get operations
	A3DVECTOR3 GetCentroid() { return m_vCentroid;}
	
	void SetErrorBase(float fErrorBase) { m_fErrorBase=fErrorBase;}
	float GetErrorBase() { return m_fErrorBase;}

	int GetVNum() { return m_arrVertecies.GetSize();}
	VertexInfo& GetVInfo(int vid) { return m_arrVertexInfo[vid];}
	A3DVECTOR3 GetV(int vid) { return m_arrVertecies[vid];}

	int GetPatchesNum() { return m_listPatches.GetCount(); }			//��ǰ��Ƭ������
	int GetOriginPatchNum() { return m_nOriginPatchNum;}
	int GetMinPatchNum() { return m_nMinPatchNum;}

	float GetCurLeastError() 
	{ 
		if(m_pCurLeastErrorPatch) return m_pCurLeastErrorPatch->GetRemovedError();
		else return -1.0f;		//����һ����Чֵ
	}

	AList2<CPatch*,CPatch*>& GetPatchesList() { return m_listPatches;}
	
	
protected:
	CPatch* SearchLeastErrorPatch();
	bool RemovePatch(CPatch* pPatch);		//ɾ��һ����Ƭ
	bool RemoveLeastErrorPatch();
	int GetLPNByError(float fError);
	
	void ComputePatchNeighbors();						//���������Ƭ��������Ƭ��˽�У�Ӧ����Init()��������
	void Reset();										//������е���Դ������Ϊ��ʼ״̬��

// Attributes
protected:
	A3DVECTOR3	m_vCentroid;							//������������㼯������
	
	AArray<A3DVECTOR3,A3DVECTOR3> m_arrVertecies;		//�����б�ֻ��¼��������Ϣ���Ӷ��������
	AArray<VertexInfo,VertexInfo> m_arrVertexInfo;		//����������Ϣ�б�Ӧ�Ͷ����б���һ��
	AList2<CPatch*,CPatch*> m_listPatches;				//���е���Ƭ�б���������ṹ�洢������ɾ��������
	
	int m_nOriginPatchNum;
	int m_nMinPatchNum;									//�򻯵�ͷʱ��������Ƭ��������ʵ��֤����������һ��Ϊ4������һ��Cube��������涼�޷����ˣ���ˣ�����Ƭ����Ϊ6
	
	int m_nCurVNum;										//��ǰ��Ч�Ķ�����������ÿ����ɾ��һ����ʱ�������Ӷ��㣬�ñ�����¼�˵�ǰ���ʱ�Ķ�������

	WORD* m_pIndices;									//����ʱ���������η��������������εĶ���id!
	WORD* m_pLEPIndices;								//���Ƶ�ǰ��С�����Ƭ�������ζ���id
	WORD* m_pLEPNIndices;								//���Ƶ�ǰ��С�����Ƭ��������Ƭ�����ζ���id

	A3DVECTOR3* m_pVerticesForRender;					//�����任��õ��Ķ�������ֱ꣬�����ڻ���
	
	CPatch* m_pCurLeastErrorPatch;						//��¼��ǰɾ�������С����Ƭ

	float			m_fErrorBase;						//��ɾ�����ת��Ϊ������ķ�ĸ��
	float*			m_fArrRemovedError;					//ɾ��������飬��¼����ÿһ�ε�ɾ�����

	bool			m_bExceptionOccur;		//�򻯳����쳣�����ղ��ܼ򻯳�͹4���壡

	//һЩǶ�׽ṹ���࣬��Ϊ˽�У����ֻ���ڱ������ڲ�ʹ��
	
	//ɾ����Ƭ�����򱸷ݽṹ
	struct NeighborBak
	{
		/*
		CPatch* pBeforeRemoved;			//ɾ��ǰ������ָ��
		CPatch* pAfterRemoved;			//���º������ָ��
		*/
		CPatch* pPatchCur;				//��ǰ������m_listPatchesָ��
		CPatch* pPatchBak;				//��������ָ�����ݵ�һ������
		NeighborBak(CPatch* pCur,CPatch* pBak) { pPatchCur=pCur; pPatchBak=pBak;}
		NeighborBak() {pPatchCur=NULL;pPatchBak=NULL;}
	};

	struct RemoveOperatorRecord				//һ��ɾ�����������¼����Ϣ
	{
		CPatch* pRemoved;									//��ɾ������Ƭָ��
		AArray<NeighborBak,NeighborBak> arrNeighborBak;		//��������Ϣ
		int VNumAdded;										//���ӵĶ�������
	};

	//���ֱ��ʹ�þֲ���������������ʱ��ָ����ܻ��������
	AArray<RemoveOperatorRecord*,RemoveOperatorRecord*> m_arrRemoveOperatorRecords;	//��¼����ɾ�����������飬�Ӷ�������ɳ����������Ȳ���
	int m_nCurOperator;				//��ǰ��������λ��
};

//ȫ�ֺ���
int FindInArray(int id,const AArray<int,int>& arr);
void AddDifferentV(AArray<A3DVECTOR3,A3DVECTOR3>& Vertices, A3DVECTOR3& v);

}	// end namespace

#endif // _CONVEXPOLYTOPE_H_
