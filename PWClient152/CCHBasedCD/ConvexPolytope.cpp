// ConvexPolytope.cpp: implementation of the CConvexPolytope class.
//
//////////////////////////////////////////////////////////////////////

#include "ConvexPolytope.h"
#include "GiftWrap.h"
#include "ConvexHullData.h"
#include "2DGiftWrap.h"
#include <A3DGDI.h>

#define MAX_ERROR 1e8

#ifndef ABS
	#define ABS(x) (((x)<0)?(-(x)):(x))
#endif

namespace CHBasedCD
{


//��ʼ����̬��Ա
float CConvexPolytope::Hull2D_Half_Thickness=0.05f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConvexPolytope::CConvexPolytope()
{
	m_pIndices=new WORD[1000*3];						//��ʼ��Ϊһ����ֵ
	m_pLEPIndices=new WORD[60*3];						//��С�����Ƭ�������θ���*3
	m_pLEPNIndices=new WORD[60*3];						//��С�����Ƭ��������Ƭ�����θ���*3

	m_pVerticesForRender=new A3DVECTOR3[2000];		//��ʼ����Ⱦ�õ��Ķ���
	m_nOriginPatchNum=0;
	m_nCurOperator=-1;

	m_fArrRemovedError=NULL;
	m_fErrorBase=1.0f;					//��һ��ȱʡֵ1.0f�����������
	m_nMinPatchNum=4;
	m_bExceptionOccur=false;

	m_vCentroid.Clear();			//��ʼ������

	CHalfSpace::SetDistThresh();  //��Halfspace�ָ���ȱʡ����ֵ
}

CConvexPolytope::~CConvexPolytope()
{
	Reset();							//���

	delete []m_pIndices;				//��������ζ����б�
	delete []m_pLEPIndices;				//��������ζ����б�
	delete []m_pLEPNIndices;			//��������ζ����б�

	delete []m_pVerticesForRender;
	
	if(m_fArrRemovedError) delete []m_fArrRemovedError;

		
}

//ʹ��CGiftWrap�����ɵ�͹������ʼ����ǰ�������Ƭ��Ա
bool CConvexPolytope::Init(const CGiftWrap &gw)
{
	assert(!gw.m_bExceptionOccur);			//û�з����쳣
 
	Reset();								//����Ϊ��ʼ״̬

	m_vCentroid=gw.m_vCentroid;				//������Ϊ��ʼ��gw������

	int i(0);
	//��������ǹ���CConvexPolytope
	if(gw.GetHullType()==CGiftWrap::HULL_3D)	//3D Hull�����	
	{
		int* Map=new int[gw.m_nVNum];		//����һ��ӳ���
		int count=0;
		
		//�����Ч����&����ӳ���
		for(i=0;i<gw.m_nVNum;i++)
		{
			Map[i]=-1;			
			if(gw.m_pbExtremeVertex[i])
			{
				//��ǰ����Ч�������ʼ��v����������뵽�����б�
				A3DVECTOR3 v(gw.m_pVertexes[i]);	
				m_arrVertecies.Add(v);
				m_arrVertexInfo.Add(VertexInfo());
				Map[i]=count++;
			}
		}

		//������Ƭ
		//�����������Ƭ
		for(i=0;i<gw.m_Faces.GetSize();i++)
		{
			Face f=gw.m_Faces[i];
			if(!f.InPolygon)		//�������κζ���Σ����
			{
				A3DVECTOR3 v1(gw.m_pVertexes[f.v1]),v2(gw.m_pVertexes[f.v2]),v3(gw.m_pVertexes[f.v3]);
				CPatch* pPatch=new CPatch(this);
				pPatch->Set(v1,v2,v3);			//������Ϣ
				
				//������Neighbors�����Ԫ��
				VPNeighbor vpn;
				AArray<VPNeighbor,VPNeighbor>& arrNeighbors=pPatch->GetNeighbors();
				
				vpn.vid=Map[f.v1];				//���������һ��ӳ��
				assert(vpn.vid>=0);
				arrNeighbors.Add(vpn);

				vpn.vid=Map[f.v2];
				assert(vpn.vid>=0);
				arrNeighbors.Add(vpn);

				vpn.vid=Map[f.v3];
				assert(vpn.vid>=0);
				arrNeighbors.Add(vpn);

				//�����������1
				m_arrVertexInfo[Map[f.v1]].cDegree++;
				m_arrVertexInfo[Map[f.v2]].cDegree++;
				m_arrVertexInfo[Map[f.v3]].cDegree++;

				//��ӵ�����
				m_listPatches.AddTail(pPatch);
			}
		}

		//��Ӷ������Ƭ
		for(i=0;i<gw.m_Planes.GetSize();i++)
		{
			AArray<int,int>* pPlane=gw.m_Planes[i];
			
			//ȡǰ�����㹹��ƽ�漸����Ϣ
			A3DVECTOR3 v1(gw.m_pVertexes[pPlane->GetAt(0)]),v2(gw.m_pVertexes[pPlane->GetAt(1)]),v3(gw.m_pVertexes[pPlane->GetAt(2)]);
			CPatch* pPatch=new CPatch(this);
			pPatch->Set(v1,v2,v3);			//������Ϣ			

			//������Neighbors�����Ԫ��
			VPNeighbor vpn;
			AArray<VPNeighbor,VPNeighbor>& arrNeighbors=pPatch->GetNeighbors();

			for(int j=0;j<pPlane->GetSize();j++)
			{
				vpn.vid=Map[pPlane->GetAt(j)];			//���������һ��ӳ��
				arrNeighbors.Add(vpn);
				
				m_arrVertexInfo[vpn.vid].cDegree++;		//���������1
			}
			
			//��ӵ�����
			m_listPatches.AddTail(pPatch);
		}
		
		delete [] Map;

	}
	else
	{

		//˵����2D Hull�����
		AArray<int,int>* pCHVs=gw.GetCHVertecies();
		if (!pCHVs) 
			return false;	
		if(pCHVs->GetSize()<3)		//������һ��������
			return false;

		//������Ϣ
		VertexInfo vInfo;
		vInfo.cDegree=3;			//ֱ�������ж�����������Ϊ3

		CHalfSpace PlaneOut,PlaneIn;

		//ȡǰ�����㹹��ƽ�漸����Ϣ
		A3DVECTOR3 v1(gw.m_pVertexes[pCHVs->GetAt(0)]),v2(gw.m_pVertexes[pCHVs->GetAt(1)]),v3(gw.m_pVertexes[pCHVs->GetAt(2)]);
		
		//��������ƽ��PlaneOut��PlaneIn���ֱ��ʾ����͵���
		PlaneOut.Set(v1,v2,v3);			
		
		PlaneIn.SetNormal(-PlaneOut.GetNormal());
		PlaneIn.SetD(-PlaneOut.GetDist());
		
		PlaneIn.Translate(Hull2D_Half_Thickness);
		PlaneOut.Translate(Hull2D_Half_Thickness);
		
		A3DVECTOR3 vOutNormal=PlaneOut.GetNormal();
		A3DVECTOR3 vInNormal=PlaneIn.GetNormal();
		
		//�ֱ����PlaneOut,PlaneIn�ϵ�����
		A3DVECTOR3 vOut=v1+Hull2D_Half_Thickness*vOutNormal;
		A3DVECTOR3 vIn=v1+Hull2D_Half_Thickness*vInNormal;

		//���춥�㼰������Ϣ
		for(i=0;i<pCHVs->GetSize();i++)
		{
			//ͬʱ��ӵ���Ͷ����һ������
			A3DVECTOR3 vec1=gw.m_pVertexes[pCHVs->GetAt(i)];
			A3DVECTOR3 vec2=vec1;
			if(i<3)
			{
				vec1+=Hull2D_Half_Thickness*vOutNormal;
				vec2+=Hull2D_Half_Thickness*vInNormal;
			}
			else
			{
				A3DVECTOR3 vDiff=vec1-vOut;
				vec1-=DotProduct(vDiff,vOutNormal)*vOutNormal;

				vDiff=vec2-vIn;
				vec2-=DotProduct(vDiff,vInNormal)*vInNormal;
			}

			m_arrVertecies.Add(vec1);
			m_arrVertecies.Add(vec2);

			//��Ӧ�ģ��������������Ϣ
			m_arrVertexInfo.Add(vInfo);
			m_arrVertexInfo.Add(vInfo);
		}
	
		//��ʼ����ƽ����Ƭ

		//�������
		CPatch* pPatch=new CPatch(this);
		pPatch->SetNormal(vOutNormal);			//������Ϣ
		pPatch->SetD(PlaneOut.GetDist());
		
		//������Neighbors�����Ԫ��
		VPNeighbor vpn;
		AArray<VPNeighbor,VPNeighbor>& arrNeighbors1=pPatch->GetNeighbors();
		for(i=0;i<pCHVs->GetSize();i++)
		{
			vpn.vid=2*i;			
			arrNeighbors1.Add(vpn);
		}
		//��ӵ�����
		m_listPatches.AddTail(pPatch);	
		
		//���ڵ���

		pPatch=new CPatch(this);
		pPatch->SetNormal(vInNormal);			//������Ϣ
		pPatch->SetD(PlaneIn.GetDist());
		
		//������Neighbors�����Ԫ��
		AArray<VPNeighbor,VPNeighbor>& arrNeighbors2=pPatch->GetNeighbors();
		//���水���������
		for(i=pCHVs->GetSize()-1;i>=0;i--)
		{
			vpn.vid=2*i+1;			
			arrNeighbors2.Add(vpn);
		}
		//��ӵ�����
		m_listPatches.AddTail(pPatch);	
		
		//��ʼ��Ӹ�������
		for(i=0;i<pCHVs->GetSize();i++)
		{
			pPatch=new CPatch(this);		
			AArray<VPNeighbor,VPNeighbor>& arrNeighbors=pPatch->GetNeighbors();
			
			//ÿ�����涼��һ������
			if(i<pCHVs->GetSize()-1)
			{
				v1=m_arrVertecies[2*i+2];
				v2=m_arrVertecies[2*i];
				v3=m_arrVertecies[2*i+1];
				pPatch->Set(v1,v2,v3);
				
				vpn.vid=2*i;
				arrNeighbors.Add(vpn);

				vpn.vid=2*i+1;
				arrNeighbors.Add(vpn);

				vpn.vid=2*i+3;
				arrNeighbors.Add(vpn);

				vpn.vid=2*i+2;
				arrNeighbors.Add(vpn);
				
			}
			else		
			{
				//���һ�����ε�����Ƚ�����
				v1=m_arrVertecies[0];
				v2=m_arrVertecies[2*i];
				v3=m_arrVertecies[2*i+1];
				pPatch->Set(v1,v2,v3);
				
				vpn.vid=2*i;
				arrNeighbors.Add(vpn);

				vpn.vid=2*i+1;
				arrNeighbors.Add(vpn);

				vpn.vid=1;
				arrNeighbors.Add(vpn);

				vpn.vid=0;
				arrNeighbors.Add(vpn);			
			}
			m_listPatches.AddTail(pPatch);
		}	
			

	}

	
	m_nOriginPatchNum = m_listPatches.GetCount();
	m_nCurVNum=m_arrVertecies.GetSize();

	//��ʼ��ɾ�����
	if(m_fArrRemovedError)
		delete[] m_fArrRemovedError;
	m_fArrRemovedError=new float[m_nOriginPatchNum+1];
	for(i=0;i<m_nOriginPatchNum+1;i++)
		m_fArrRemovedError[i]=-1.0f;			//0,1,2,3����Ϊ��Чֵ
	
	m_bExceptionOccur=false;

	//����ÿ��patch������patch
	ComputePatchNeighbors();
 
	//CHalfSpace::SetDistThresh(1e-3);  //�ָ���ȱʡ����ֵ

	//Ѱ����Сɾ������Ӧ����Ƭ
	SearchLeastErrorPatch();		

	//��ʼ�򻯣��򻯵�ͷ
	ReduceAll();

	return true;
}

void CConvexPolytope::ComputePatchNeighbors()
{
	//һ������ѭ������
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	ALISTPOSITION TraPatchPos;
	CPatch *pCurPatch,*pTraPatch;

	for(int i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);
		TraPatchPos=m_listPatches.GetHeadPosition();
		for(int j=0;j<m_listPatches.GetCount();j++)
		{
			pTraPatch=m_listPatches.GetNext(TraPatchPos);
			if(i!=j)
				pCurPatch->Neighbor(pTraPatch);
		}
		//����������굱ǰPatch������������Ƭ��
		//�Ϳ��Լ�����ɾ�������
		pCurPatch->UpdateRemovedError();
	}
}

void CConvexPolytope::UndoRemove()
{
	if(m_nCurOperator==-1)			//�Ѿ����ݵ����ʼ��״̬�ˣ����ܼ���������
		return;
	
	int RemOprCount=m_arrRemoveOperatorRecords.GetSize();
	RemoveOperatorRecord& ror=*(m_arrRemoveOperatorRecords[m_nCurOperator]); 
	
	//���뵱ǰ������¼��ɾ������Ƭ
	m_listPatches.AddTail(ror.pRemoved);
	ror.pRemoved->IncreVDegree();			//�����������1

	//�޸ĵ�ǰ��������
	m_nCurVNum-=ror.VNumAdded;

	CPatch *pCur,*pBak;
	CPatch patchTemp(this);
	int nNBNum=ror.arrNeighborBak.GetSize();
	//ALISTPOSITION lpos;
	for(int i=0;i<nNBNum;i++)
	{
		//for debug
		int vid=ror.pRemoved->GetNeighbors()[i].vid;
		A3DVECTOR3 v=m_arrVertecies[vid];
		VertexInfo vInfo=m_arrVertexInfo[vid];


		pCur=ror.arrNeighborBak[i].pPatchCur;
		pBak=ror.arrNeighborBak[i].pPatchBak;


		patchTemp=*pCur;
		*pCur=*pBak;
		*pBak=patchTemp;

	}

	m_nCurOperator--;						//ǰ��һ��

	m_pCurLeastErrorPatch=ror.pRemoved;		//�޸ĵ�ǰ�����С��Ƭ

}

void CConvexPolytope::RedoRemove()
{
	int RemOprCount=m_arrRemoveOperatorRecords.GetSize();
	if(m_nCurOperator==RemOprCount-1)	//�Ѿ������һ�������ˣ���������ǰ�ָ���
		return;

	RemoveOperatorRecord& ror=*(m_arrRemoveOperatorRecords[m_nCurOperator+1]); 
	
	//��һ��ɾ����ǰ������¼��ɾ������Ƭ����Ϊǰ��Ļָ������Ѿ����������
	//ע�⣺����ƬӦ���Ǳ�����������listpatch�������Ϊǰ��Ļָ�������������뵽β���ˡ�
	//�������Ϳ��Ա�������һ�α���Ѱ�ң�
	CPatch* ptail=m_listPatches.GetTail();
	assert(ptail==ror.pRemoved);			//��������һ����֤��
	m_listPatches.RemoveTail();
	ror.pRemoved->DecreVDegree();			//�����������1
	
	//�޸ĵ�ǰ��������
	m_nCurVNum+=ror.VNumAdded;

	CPatch *pCur,*pBak;
	CPatch patchTemp(this);
	int nNBNum=ror.arrNeighborBak.GetSize();
	for(int i=0;i<nNBNum;i++)
	{

		pCur=ror.arrNeighborBak[i].pPatchCur;
		pBak=ror.arrNeighborBak[i].pPatchBak;

		patchTemp=*pCur;
		*pCur=*pBak;
		*pBak=patchTemp;
	}

	m_nCurOperator++;		//ǰ��һ��
	
	if(m_nCurOperator==RemOprCount-1)		//����һ�����������һ������
		SearchLeastErrorPatch();			//��ʱҪ���¼�����С�����Ƭ
	else
		m_pCurLeastErrorPatch=m_arrRemoveOperatorRecords[m_nCurOperator+1]->pRemoved;		//�޸ĵ�ǰ�����С��Ƭ
}

void CConvexPolytope::Reset()
{
	m_arrVertecies.RemoveAll();
	m_arrVertexInfo.RemoveAll();
	
	//m_fErrorBase=1.0f;					//��һ��ȱʡֵ1.0f

	//���ListPatch
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch* pCurPatch;
	int i(0);
	for(i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);
		delete pCurPatch;
	}
	m_listPatches.RemoveAll();		
	
	//���RemoveOperatorRecords
	for(i=0;i<m_arrRemoveOperatorRecords.GetSize();i++)
	{
		RemoveOperatorRecord* pror=m_arrRemoveOperatorRecords[i];
		RemoveOperatorRecord& ror=*pror;
		for(int j=0;j<ror.arrNeighborBak.GetSize();j++)
			delete ror.arrNeighborBak[j].pPatchBak;		//ɾ�����ǵı���ָ��

		if(i<=m_nCurOperator)		//��m_nCurOperator֮ǰ�Ĳ���
		{
			//��ɾ������Ƭָ��ʵ���ϲ�δɾ����ͬʱҲ����m_listPatches�У���ˣ�����Ҫɾ����
			delete ror.pRemoved;
		}
		else
		{

		}
		
		delete pror;		//ɾ������
		
	}
	m_nCurOperator=-1;
	m_arrRemoveOperatorRecords.RemoveAll();
}

//////////////////////////////////////////////////////////////
// ɾ����ƬpPatch
// ����Ҫ��ɾ��ǰ���ֳ�����ã������Ϳ���֧�ֳ���ɾ��������
// �����ֱ�ӵ���pPatch�Լ��ı�ɾ������
//////////////////////////////////////////////////////////////
bool CConvexPolytope::RemovePatch(CPatch *pPatch)
{
	//return pPatch->Removed();
	
	//*
	RemoveOperatorRecord* pror=new RemoveOperatorRecord;
	RemoveOperatorRecord& ror=*pror;			//��ǰ��ɾ��������¼
	ror.pRemoved=pPatch;

	//��������Ƭ���Ʋ������
	CPatch *pNPBak,*pCurNP;
	AArray<VPNeighbor,VPNeighbor>& Neighbors=pPatch->GetNeighbors();
	int iNBSize=Neighbors.GetSize();
	int i(0);
	for(i=0;i<iNBSize;i++)
	{
		//for debug
		int vid=Neighbors[i].vid;
		A3DVECTOR3 v=m_arrVertecies[vid];
		VertexInfo vInfo=m_arrVertexInfo[vid];

		pCurNP=Neighbors[i].pNeighborPatch;
		pNPBak=new CPatch(*pCurNP);				//���Ƹ�patchNeighborBak
		
		//����Ҫ���ǳ����ͻָ���������������ƺ�����ֱ����ӵ����
		//��Ӧ��Ϊ����m_nCurOperator�α���������ɾ������ɾ��
		//Ȼ������ӵ�����λ�ã�
		ror.arrNeighborBak.Add(NeighborBak(pCurNP,pNPBak));
	}
	
	if(	pPatch->Removed() )		//ɾ���ɹ�
	{
		
		//�Ƴ���ǰ��Ƭ
		ALISTPOSITION pos=m_listPatches.Find(pPatch);
		m_listPatches.RemoveAt(pos);

		//��¼�������ӵĶ�����
		ror.VNumAdded=m_arrVertecies.GetSize()-m_nCurVNum;
		m_nCurVNum=m_arrVertecies.GetSize();

		//���ݲ�����
		m_arrRemoveOperatorRecords.Add(&ror);
		m_nCurOperator++;
		return true;
	}
	else						//ɾ��ʧ��
	{
		//�ͷ��ڴ�
		for(i=0;i<iNBSize;i++)
		{
			delete ror.arrNeighborBak[i].pPatchBak;
		}
		delete pror;
		return false;
	}
	
	 //*/
}

//����A3DFlatCollector��������л���
//���һ��������ʾ�Ƿ����������С�����Ƭ��ȱʡΪtrue

//04-10-23 �����޸Ĵ˽ӿ�
//����һ��pSpecialPatch�����估��������Ƭ������������Ļ��ƴ���
void CConvexPolytope::Render(A3DFlatCollector *pFC,A3DMATRIX4& tMatrix, DWORD dwColor,CPatch* pSpecialPatch)
{
	//������Ƶ�ɫ��
	DWORD dwNCol[]={
					0x8000ff00,
					0x8000ffff,
					0x800000ff,
					0x80ff00ff,
					0x80ffff00,
					0x80007f00,
					0x8000007f
					/*
					0x80000050,
					0x800000a0,
					0x800000ff,
					0x80005050,
					0x800050a0,
					0x800050ff,
					0x8000a050,
					0x8000a0a0,
					0x8000a0ff,
					0x8000ff50,
					0x8000ffa0,
					0x8000ffff
					*/
					};
	
	int colSize=sizeof(dwNCol)/sizeof(dwNCol[0]);

	//����
	A3DVECTOR3* pVertices=m_arrVertecies.GetData();
	int vNum=m_arrVertecies.GetSize();
	
	int i,j;	

	
	//������Ⱦ�Ķ��㣬Ҫ��һ���任
	for(i=0;i<m_nCurVNum;i++)
	{
		//ֻ��Ⱦ��Ч�Ķ��㣬���ö���Ķ������벻С��3
		if(m_arrVertexInfo[i].cDegree<3) continue;
		
		//�任����Ⱦ����
		m_pVerticesForRender[i]=pVertices[i]*tMatrix;
		//�ݲ����ƶ���	
		g_pA3DGDI->Draw3DPoint(m_pVerticesForRender[i],0xff00ff00,3.0f);
	}

	//���������εĶ���id�б�
	//�����Ӧʹ��������Ϊ�������һ��һ��������
	//��������Ⱦʱ���������ַ��Ϣ����
	//WORD* pIndices=new WORD(10000);			//��Ԥ�ȷ���һ���㹻��Ŀռ�
	int cur=0;								//һ��ָ��ǰid�ļ�����
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch* pCurPatch;
	if(pSpecialPatch)	
	{
		//pSpecialPatch��Ϊ��
		//����ƬpSpecialPatch����������һ�����⴦�����Ƴɲ�ͬ��ɫ���Ӷ����ڹ۲�Ч��

		//ѡ���Ի���
		for(i=0;i<m_listPatches.GetCount();i++)
		{
			pCurPatch=m_listPatches.GetNext(CurPatchPos);
			if(pCurPatch!=pSpecialPatch && ! pSpecialPatch->InNeighbors(pCurPatch))
			{
				//����pSpecialPatch���Ҳ�����������

				AArray<VPNeighbor,VPNeighbor>& Neighbors=pCurPatch->GetNeighbors();
				assert(Neighbors.GetSize()>=3);
				for(j=2;j<Neighbors.GetSize();j++)
				{
					//����������
					m_pIndices[cur]=Neighbors[0].vid;			//v1ʼ��Ϊ��Ԫ��
					m_pIndices[cur+1]=Neighbors[j-1].vid;		//v2Ϊ��һԪ��
					m_pIndices[cur+2]=Neighbors[j].vid;			//v3Ϊ��ǰԪ��
					cur+=3;
				}
			}
		}

		//����
		pFC->AddRenderData_3D(m_pVerticesForRender,vNum,m_pIndices,cur,dwColor);
		
		//������С�����Ƭ��������
		cur=0;
		int ncur=0;			//�������������Ƭ������
		AArray<VPNeighbor,VPNeighbor>& Neighbors=pSpecialPatch->GetNeighbors();
		for(j=0;j<Neighbors.GetSize();j++)
		{
			if(j>=2)
			{
				//��������������
				m_pLEPIndices[cur]=Neighbors[0].vid;			//v1ʼ��Ϊ��Ԫ��
				m_pLEPIndices[cur+1]=Neighbors[j-1].vid;		//v2Ϊ��һԪ��
				m_pLEPIndices[cur+2]=Neighbors[j].vid;		//v3Ϊ��ǰԪ��
				cur+=3;
			}

			//���������������
			pCurPatch=Neighbors[j].pNeighborPatch;
			AArray<VPNeighbor,VPNeighbor>& NN=pCurPatch->GetNeighbors();	//�������������
			for(i=2;i<NN.GetSize();i++)				//��2��ʼѭ��
			{
				//����������
				m_pLEPNIndices[ncur]=NN[0].vid;			//v1ʼ��Ϊ��Ԫ��
				m_pLEPNIndices[ncur+1]=NN[i-1].vid;		//v2Ϊ��һԪ��
				m_pLEPNIndices[ncur+2]=NN[i].vid;		//v3Ϊ��ǰԪ��
				ncur+=3;			
			}
			//���Ƶ�ǰ����
			DWORD curCol=dwNCol[j%colSize];
			pFC->AddRenderData_3D(m_pVerticesForRender,vNum,m_pLEPNIndices,ncur,curCol);	//��ɫ��Ⱦ
			ncur=0;
		}
		pFC->AddRenderData_3D(m_pVerticesForRender,vNum,m_pLEPIndices,cur,0x80ff0000);		//��ɫ��Ⱦ
		


	}
	else
	{
		//pSpecialPatchΪ�գ�
		//ȫ�����ƣ�
		for(i=0;i<m_listPatches.GetCount();i++)
		{
			pCurPatch=m_listPatches.GetNext(CurPatchPos);
			//������С�����Ƭ���Ҳ�����������

			AArray<VPNeighbor,VPNeighbor>& Neighbors=pCurPatch->GetNeighbors();
			assert(Neighbors.GetSize()>=3);
			for(j=2;j<Neighbors.GetSize();j++)
			{
				
				//����������
				m_pIndices[cur]=Neighbors[0].vid;			//v1ʼ��Ϊ��Ԫ��
				m_pIndices[cur+1]=Neighbors[j-1].vid;		//v2Ϊ��һԪ��
				m_pIndices[cur+2]=Neighbors[j].vid;		//v3Ϊ��ǰԪ��
				cur+=3;
			}

		}
		//����
		pFC->AddRenderData_3D(m_pVerticesForRender,vNum,m_pIndices,cur,dwColor);	

	}

	pFC->Flush();
}

//ɾ��һ�������С����Ƭ��Ϊϵͳ�Զ�����ʱʹ��
bool CConvexPolytope::RemoveLeastErrorPatch()
{
	//Ѱ����Сɾ������Ӧ����Ƭ
	SearchLeastErrorPatch();
	if(!m_pCurLeastErrorPatch)
		return false;

	return RemovePatch(m_pCurLeastErrorPatch);
	
	/*
	//�²����������⣡wf��04-12-2
	//�����µĲ��ԣ������ǰ��С�����Ƭɾ��ʧ�ܣ�������Ѱ�Ҽ���ɾ����
	
	while(!RemovePatch(m_pCurLeastErrorPatch))				
	{
		if(SearchLeastErrorPatch()==NULL)		//������һ����Сɾ����Ƭ
			return false;						//�޷��ҵ�		
	}
	//ɾ������Ƭ�ɹ�
	return true;
	//*/
	
}

CPatch* CConvexPolytope::SearchLeastErrorPatch()
{
	int PatchNum=m_listPatches.GetCount();
	if(PatchNum<=4)
	{
		//��������������С�ڵ���4�����ڽ��м��ˣ�
		m_pCurLeastErrorPatch= NULL;
		return NULL;			
	}


	//���������С��patch
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch,*pLeastErrorPatch;
	float LeastError=MAX_ERROR;			//��ʼ��Ϊһ����ֵ
	for(int i=0;i<PatchNum;i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);
		float error=pCurPatch->GetRemovedError();
		if(error!=-1.0f && error<LeastError)
		{
			LeastError=pCurPatch->GetRemovedError();
			pLeastErrorPatch=pCurPatch;
		}
	}

	
	if(LeastError!=MAX_ERROR)			//�����Ӧ��Ϊ���ڵ���0
		m_pCurLeastErrorPatch=pLeastErrorPatch;
		
	else	//˵��û����С��������ɾ����
		m_pCurLeastErrorPatch= NULL;

	return m_pCurLeastErrorPatch;
}

//�Ѿ����������е�ɾ��������
//���û���κ�ɾ��������Ҳ����true!
bool CConvexPolytope::UndoAll()
{
	//if(m_arrRemoveOperatorRecords.GetSize()==0)
	//	return true;

	if(m_nCurOperator==-1)		//˵��������ͷ��
		return true;
	else 
		return false;
}

//�Ѿ��ָ������е�ɾ��������
bool CConvexPolytope::RedoAll()
{
	return (m_nCurOperator==m_arrRemoveOperatorRecords.GetSize()-1);
}

// ֱ�������򻯺��ĳһ�����
// ������LeftPatchesNumָ������ʾʣ�µ���Ƭ������򻯲��
bool CConvexPolytope::Goto(int LeftPatchesNum)
{
	if(LeftPatchesNum<m_nMinPatchNum || LeftPatchesNum> m_nOriginPatchNum )
		return false;
	int i;
	int RemovedPatchesNum=m_nOriginPatchNum-LeftPatchesNum;
	int UnRemoveTimes=m_nCurOperator+1-RemovedPatchesNum;
	if(UnRemoveTimes>0)
	{
		for(i=0;i<UnRemoveTimes;i++)
			UndoRemove();
	}
	else
	{
		for(i=0;i<-UnRemoveTimes;i++)
			RedoRemove();
	}
	return true;
}

//��С�����ƬΪ������Ƭʱ�Ļ��ƣ�ֱ�ӵ���Render(...)����
void CConvexPolytope::RenderLEPatchSpecial(A3DFlatCollector *pFC, A3DMATRIX4 &tMatrix, DWORD dwColor)
{
	Render(pFC,tMatrix,dwColor,m_pCurLeastErrorPatch);
}

void CConvexPolytope::ReduceAll()
{
	int curPatchNum=m_nOriginPatchNum;
	do{
		m_fArrRemovedError[curPatchNum]=GetCurLeastError()/m_fErrorBase;
		curPatchNum--;
	}while(RemoveLeastErrorPatch());
	
	m_nMinPatchNum=++curPatchNum;

	if(curPatchNum<4)		//�����ܱ�4���٣�
		m_bExceptionOccur=true;
}

void CConvexPolytope::Goto(float fError)
{
	if(fError<0) return;
	int LeftPatchesNum=GetLPNByError(fError);
	Goto(LeftPatchesNum);
}

int CConvexPolytope::GetLPNByError(float fError)
{
	int i=m_nOriginPatchNum;
	//��Ϊ��������  //���ֲ���
	while(m_fArrRemovedError[i]<fError && i > m_nMinPatchNum)
		i--;
	/*
		i>>=1;		//�ȼ���i/=2
	while(m_fArrRemovedError[i]>fError)
		i++;
	*/
	return i;
}

//���򻯺õ����ݵ�����CConvexHullData������
void CConvexPolytope::ExportCHData(CConvexHullData* pCHData)
{
	AArray<int,int> Map;		//һ���ɶ����ڱ�����id��CConvexHullData��id��һ��ӳ���
	
	//����ÿһ��Patch
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch;
	for(int i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);
		A3DVECTOR3 n=pCurPatch->GetNormal();
		CFace face;
		face.SetNormal(n);
		face.SetD(pCurPatch->GetDist());
		
		CHalfSpace hs;
		for(int j=0;j<pCurPatch->GetVNum();j++)
		{
			int vid=pCurPatch->GetVID(j);
			
			//���촹ֱ�ڸñ߷������HalfSpace
			A3DVECTOR3 v1,v2;
			pCurPatch->GetEdge(j,v1,v2);
			hs.Set(v1,v2,v2+n);
			
			// for debug, by wenfeng, 05-04-08
			float d = face.DistV2Plane(v1);

			int ExistID;
			if((ExistID=FindInArray(vid,Map))==-1)
			{
				//˵�����µĶ���

				//���뵽CConvexHullData��Vertices��
				pCHData->AddVertex(v1);
				int newID=pCHData->GetVertexNum()-1;	//��pCHData�е�id
				face.AddElement(newID,hs);
				
				Map.Add(vid);
			}
			else
			{
				//˵�����Ѿ����ڵĶ���
				face.AddElement(ExistID,hs);
			}
		}

		//�������Ƭ
		pCHData->AddFace(face);
		
	}
	
	// �������߽��ռ�
	pCHData->ComputeFaceExtraHS();

}

int FindInArray(int id,const AArray<int,int>& arr)
{
	for(int i=0;i<arr.GetSize();i++)
		if(arr[i]==id)
			return i;
	return -1;
}

//������Vertices�в��벻�ظ��ĵ�
void AddDifferentV(AArray<A3DVECTOR3,A3DVECTOR3>& Vertices, A3DVECTOR3& v)
{
	A3DVECTOR3 vDiff;
	for(int i=0;i<Vertices.GetSize();i++)
	{
		vDiff=Vertices[i]-v;
		if(vDiff.SquaredMagnitude()<1e-6f)			//����������˵���к�v�ظ��ĵ㣡��˲����κβ�����
			return;
	}
	Vertices.Add(v);
}

//����͹��������ƽ�����ཻ���õ��Ľ���ͶӰ��XOZƽ�湹�ɵ�2D͹��
//Ҫ��ƽ�淨���ܴ�ֱ��Y�ᣬ��ƽ�治����Yƽ�У�һ��ģ����ǵ���ƽ�涼��ֱ��Y��
//��������2�������򷵻�false��
//����������gw2d����������Щ���㹹�ɵ�2D͹��ͶӰ����XOZƽ��
//ͨ������b2ParallelPlanes�����������Ƿ�Ϊ�����ƽ��ƽ��صõĲ��ֵ�2D͹��
bool CConvexPolytope::IntersectPlanesProj2XOZ(CHalfSpace* Planes,int PlaneNum,C2DGiftWrap& gw2d,bool b2ParallelPlanes)
{
	A3DVECTOR3 n1,n2;
	if (b2ParallelPlanes) 
	{
		//��ƽ����Ƭ͹��
		if(PlaneNum!=2) return false;
		n1=Planes[0].GetNormal();
		n2=Planes[1].GetNormal();
		if(!(n1==n2 || n1==-n2))
			return false;
	}
	
	if (PlaneNum<1) return false;

	int i,j,k;
	//�ж�Planes�ķ���
	for(i=0;i<PlaneNum;i++)
	{
		//ƽ����Y��ļн�����Ϊ30 degree
		//����ֵ�ɵ�
		if (ABS(Planes[i].GetNormal().y)<0.5f) 
		{
			return false;
		}
	}

	//��ʼ������
	AArray<A3DVECTOR3,A3DVECTOR3> Vertices;		//��Ž���Ķ��㼯
	//��ÿһ����Ƭ����
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch;
	for(i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);
		A3DVECTOR3 v1,v2;
		//��߽��п���
		for(k=0;k<pCurPatch->GetVNum();k++)
		{
			pCurPatch->GetEdge(k,v1,v2);
			for(j=0;j<PlaneNum;j++)
			{
				A3DVECTOR3 vI;
				int res=Planes[j].IntersectLineSeg(v1,v2,vI);
				switch(res) {
				case 0:
					break;
				case 1:
					AddDifferentV(Vertices,v1);
					break;
				case 2:
					AddDifferentV(Vertices,v2);
					break;
				case 3:
					AddDifferentV(Vertices,v1);
					AddDifferentV(Vertices,v2);
					break;
				case 4:
					AddDifferentV(Vertices,vI);
					break;
				default:
					break;
				}
			}
		}

	}
	
	if (b2ParallelPlanes) 
	{
		//������ӱ�͹����������ƽ���Ķ���
		if(n1==-n2)
			Planes[1].Inverse();	//��P2ת��ʹ��P1,P2������ͬ
		
		//����������Ч����
		for(int i=0;i<m_nCurVNum;i++)
			if(m_arrVertexInfo[i].cDegree>=3) 		//��Ч����
				if(Planes[0].Outside(m_arrVertecies[i]) && Planes[1].Inside(m_arrVertecies[i]) ||
				   Planes[0].Inside(m_arrVertecies[i])  && Planes[1].Outside(m_arrVertecies[i]))
				   Vertices.Add(m_arrVertecies[i]);
	}

	//����ϣ���ʼ����2D͹��
	int vNum=Vertices.GetSize();
	if (vNum<3) return false;

	A3DVECTOR3* Vs=new A3DVECTOR3[vNum];
	for(i=0;i<vNum;i++)
		Vs[i]=Vertices[i];

	gw2d.SetVertexes(Vs,vNum);
	gw2d.ComputeConvexHull();
	delete [] Vs;
	return true;
}

//�ж϶����Ƿ���͹������֮��
bool CConvexPolytope::IsVertexOutside(A3DVECTOR3 v,float fInflateRadius)
{
	CHalfSpace hs;
	//��ÿһ����Ƭ����
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch;
	for(int i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);		
		hs.SetNormal(pCurPatch->GetNormal());
		hs.SetD(pCurPatch->GetDist()+fInflateRadius);
		if(hs.Outside(v))
			return true;
	}
	return false;
}

//�ж϶����Ƿ���͹������֮��
bool CConvexPolytope::IsVertexInside(A3DVECTOR3 v, float fInflateRadius)
{
	CHalfSpace hs;
	//��ÿһ����Ƭ����
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch;
	for(int i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);		
		hs.SetNormal(pCurPatch->GetNormal());
		hs.SetD(pCurPatch->GetDist()+fInflateRadius);
		if(!hs.Inside(v))
			return false;
	}
	return true;
}

//����v�Ƿ���ĳ���߽�ƽ���ϣ�
CPatch* CConvexPolytope::IsVertexOnPatch(A3DVECTOR3 v, float fInflateRadius)
{
	CHalfSpace hs;
	//��ÿһ����Ƭ����
	ALISTPOSITION CurPatchPos=m_listPatches.GetHeadPosition();
	CPatch *pCurPatch;
	for(int i=0;i<m_listPatches.GetCount();i++)
	{
		pCurPatch=m_listPatches.GetNext(CurPatchPos);		
		hs.SetNormal(pCurPatch->GetNormal());
		hs.SetD(pCurPatch->GetDist()+fInflateRadius);
		//wf��04.12.25�������ƺ������⣬��Ϊ��ƽ���ϲ�����һ�����н�ƽ����
		if(hs.OnPlane(v))
			return pCurPatch;
	}
	return NULL;
	
}

A3DAABB CConvexPolytope::GetAABB()
{
	A3DAABB aabb;
	aabb.Clear();
	for(int i=0; i<m_nCurVNum; i++)
	{
		//ֻ���Ƕ������벻С��3�Ķ���
		if(m_arrVertexInfo[i].cDegree<3) continue;
		
		aabb.AddVertex(m_arrVertecies[i]);
	}
	aabb.CompleteCenterExts();
	return aabb;
}

}	// end namespace