// Patch.cpp: implementation of the CPatch class.
//
//////////////////////////////////////////////////////////////////////

#include "Patch.h"
#include "LEquations.h"			//������Է������ͷ
#include "CombGenerator.h"		//�����������ͷ
#include "ConvexPolytope.h"


#ifdef WF_DEBUG					//����״̬
	#define ASSERT_IN_REMOVE_PATCH(x) assert(x)
	#define ASSERT_IN_UPDATE_ERROR(x) assert(x)
#else							//�ǵ���״̬�������쳣�Ĵ���
	#define ASSERT_IN_REMOVE_PATCH(x) if(!(x)) goto ExceptionOccur
	#define ASSERT_IN_UPDATE_ERROR(x) if(!(x)) goto Exit
#endif

namespace CHBasedCD
{


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPatch::CPatch(CConvexPolytope* pCP)
{
	m_pConvexPolytope=pCP;
	m_fRemovedError=-1.0f;			//��ʼ��Ϊһ����ֵ��
}

CPatch::~CPatch()
{

}

void CPatch::Neighbor(CPatch *pPatch)
{
	int start,end;
	for(int i=0;i<m_arrNeighbors.GetSize();i++)
	{
		start=m_arrNeighbors[i].vid;
		end=(i==m_arrNeighbors.GetSize()-1)?m_arrNeighbors[0].vid:m_arrNeighbors[i+1].vid;
		if(pPatch->VInPatch(start) && pPatch->VInPatch(end))
			m_arrNeighbors[i].pNeighborPatch=pPatch;
	}

}

bool CPatch::VInPatch(int vid)
{
	for(int i=0;i<m_arrNeighbors.GetSize();i++)
	{
		if(vid==m_arrNeighbors[i].vid)
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
// ע�⣺
// ��������������ɾ������Ȼ���������Ѿ��������²�������Ч���㣬
// ������������Щ�¶������ӡ�����Ƭ��ɾ������������Ƭ�ĸ��µȲ���
// �ڼ����У�Ҳû�п��ǹ���������
//////////////////////////////////////////////////////////////////////
void CPatch::UpdateRemovedError()
{
	// Revised at 10-14 ��
	m_fRemovedError=-1.0f;			//ÿ�θ���ʱ�����ȳ�ʼ��Ϊһ����ֵ��
	
	//������������Ŀ��ܽ��㣬�ҳ����о����ƽ����Զ�ĵ�
	//����뼴Ϊ��ƽ���ɾ�����
	int npCount=m_arrNeighbors.GetSize();
	
	if(npCount<3) return;			//������Ƭ��С��3���������쳣��


	// Added at 10-15 afternoon
	// ��ֹ������Ч���㵫���˴�������
	char* NPValid= new char[npCount];		//��¼ÿһ��������Ƭ�Ƿ���Ч�����Ƿ񾭹�һ����Ч���㣡
	memset(NPValid,0,npCount);

	int n[3];		//�洢ѡ��������������Ƭ��id
	A3DVECTOR3 v;
	bool bValid;	

	CCombGenerator cg(3,npCount);

	ASSERT_IN_UPDATE_ERROR(cg.IsSolvable());			//ȷ��npCount>=3

	while(!cg.IsOver())
	{
		//����һ�����
		cg.GetNextComb(n);

		//���
		if(Solve3NPIntersection(n,v))
		{
			bValid=true;
			
			//����н⣬�ȴ��뵽��ǰƽ��ķ���
			//ע�⣺���쵱ǰ��Ƭʱ����Ӧ���ǲ��ڵ�ǰ��Ƭ���ڲ���
			if(this->Inside(v)) 
				continue;			//ֱ�ӽ�����һ��whileѭ��
			
			//��������������������Ƭ���̼���
			for(int i=0;i<npCount;i++)
			{
				if(i!=n[0] && i!=n[1] && i!=n[2])
				{
					//��������Щ��Ƭ���ⲿ

					if(m_arrNeighbors[i].pNeighborPatch->Outside(v))
					{
						bValid=false;
						break;		//��ĳ��ƽ���ⲿ������forѭ��
					}
				}
			} // end of for-loop

			if(bValid)
			{
				//˵���õ�ȷʵ��һ����Ч�Ľ��㣬���Լ������
				// for debug

				float error=OnPlane(v)?0.0f:DistV2Plane(v);			//�����������OnPlane()�����þ���Ϊ0.0f

				if(error>m_fRemovedError) 
					m_fRemovedError=error;

				//�������ý����������Ƭ��Ϊ��Ч
				NPValid[n[0]]=1;
				NPValid[n[1]]=1;
				NPValid[n[2]]=1;
			}
		}//�޽������ݲ�����
		else
		{
		}
	} // end of while-loop

	//�Ƿ����е�������Ƭ����Ч��
	if(memchr(NPValid,0,npCount))
	{
		m_fRemovedError=-1.0f;		//�����ҵ���Ч����Ƭ
	}

Exit:
	delete[] NPValid;
}

//////////////////////////////////////////////////////////////////////
// ����n[0,1,2]��������Ƭ�Ľ��㣬�������vIntersection
// �޽⼰�����������false!
//////////////////////////////////////////////////////////////////////
bool CPatch::Solve3NPIntersection(int n[3], A3DVECTOR3 &vIntersection)
{
	int npCount=m_arrNeighbors.GetSize();
	
	//������Ч
	if(n[0]<0 || n[0]>=npCount ||
	   n[1]<0 || n[1]>=npCount ||
	   n[2]<0 || n[2]>=npCount )
	   return false;
	
	//�������Է�����
	CMatrix	mtxCoef(3,3);		// ϵ������
	CMatrix mtxConst(3,1);		// ��������
	CMatrix	mtxResult(3,1);		// ���

	for(int i=0;i<3;i++)
	{
		CPatch* pPatch=m_arrNeighbors[n[i]].pNeighborPatch;

		A3DVECTOR3 v(pPatch->GetNormal());
		mtxCoef.SetElement(i,0,v.x);
		mtxCoef.SetElement(i,1,v.y);
		mtxCoef.SetElement(i,2,v.z);
		mtxConst.SetElement(i,0,pPatch->GetDist());
	}
	
	//�ø�˹ȫѡ��Ԫ�����
	CLEquations le(mtxCoef,mtxConst);
	if(!le.GetRootsetGauss(mtxResult))
		return false;
	
	vIntersection.x=(float)mtxResult.GetElement(0,0);
	vIntersection.y=(float)mtxResult.GetElement(1,0);
	vIntersection.z=(float)mtxResult.GetElement(2,0);

	return true;

}

//////////////////////////////////////////////////////////////////////
// ��������ĵķ����� ������Ƭ��͹��������ɾ����
// �漰���˺ܶ���صĲ�����������
// �����µĶ��㡢�¶������ӡ�����Ƭ��ɾ������������Ƭ�ĸ��µȲ���
//////////////////////////////////////////////////////////////////////
bool CPatch::Removed() 
{
	int npCount=m_arrNeighbors.GetSize();
	if(npCount<3) return false;

	//һЩ��Ҫ�����ݽṹ
	AArray<A3DVECTOR3,A3DVECTOR3> VerteciesAdded;			//ɾ���󽫲������µĶ��㼯
	AArray<char,char> VDegree;								//�²�������Ķ��������ڽ�ƽ���������϶�̬����һһ��Ӧ

	//��Щ����ƽ���ཻ����ͬ�Ķ���
	//��Ӧ�������Ҫ��ӵĶ���
	//�������ӵĶ���v,����VerteciesAdded�е�idΪ2
	//��PatchesIntersectV[2]��Ӧ��һ����̬���飬��Ԫ��
	//��Ϊ�ཻ�ڵ�v������patch������Ƭ��id
	//��������������ע�⣺������������
	//AArray<AArray<int,int>,AArray<int,int>>�ڱ���ʱ�ᱨ��
	//˵��ģ�����ʹ����ʱ��֧��Ƕ�׶��塣��ˣ����������ָ�룡
	AArray<AArray<int,int>*,AArray<int,int>*> PatchesIntersectV;
	
	//������Ƭ��������Щ�¼���Ķ���
	AArray<AArray<int,int>*,AArray<int,int>*> VerticesInPatch;
	VerticesInPatch.SetSize(npCount,0);
	int i(0);
	for(i=0;i<npCount;i++)
	{	
		//��ʼ��VerticesInPatch��ÿһ��Ԫ��
		VerticesInPatch[i]=new AArray<int,int>;
	}

	/////////////////////////////////////////////////////////////////////
	// ����½��㲿�֣�
	/////////////////////////////////////////////////////////////////////
	
	int n[3];		//�洢ѡ��������������Ƭ��id
	CCombGenerator cg(3,npCount);
	//assert(cg.IsSolvable());			//ȷ��npCount>=3
	A3DVECTOR3 v;
	bool bValid;	
	
	//��ѭ��
	while(!cg.IsOver())
	{
		//����һ�����
		cg.GetNextComb(n);
		if(Processed(n,PatchesIntersectV))			//�Ѿ��������ֱ��������������һ��ѭ����
			continue;

		if(Solve3NPIntersection(n,v))				//��Ψһ��
		{
			bValid=true;
			
			//����н⣬�ȴ��뵽��ǰƽ��ķ���
			//ע�⣺���쵱ǰ��Ƭʱ����Ӧ���ǲ��ڵ�ǰ��Ƭ���ڲ���
			//�ڸ�ƽ���ϻ����ⲿ������
			if(this->Inside(v)) 
				continue;			//ֱ�ӽ�����һ��whileѭ��
			AArray<int,int>* pPatchesPassV=new AArray<int,int>;		//����һ����v��������Ƭ�Ķ�̬����
			
			//���δ���������������Ƭ���̼���
			for(i=0;i<npCount;i++)
			{
				if(i!=n[0] && i!=n[1] && i!=n[2])
				{
					//��������Щ��Ƭ���ⲿ
					if(m_arrNeighbors[i].pNeighborPatch->Outside(v))
					{
						bValid=false;
						break;		//��ĳ��ƽ���ⲿ������forѭ��
					}
					//��ƽ���ϵ����
					if(m_arrNeighbors[i].pNeighborPatch->OnPlane(v))
					{
						pPatchesPassV->Add(i);			//���ֹ���������
					}
				}
			} // end of for-loop
			
			if(bValid)	//��Ч�Ľ���
			{
				VerteciesAdded.Add(v);		//��ӵ�����Ч�����б�
				
				//���n[0,1,2]���ཻ�ڸö��������ƽ���б�
				pPatchesPassV->Add(n[0]);
				pPatchesPassV->Add(n[1]);
				pPatchesPassV->Add(n[2]);
				
				//modified by wf, 04-10-09
				//Ϊ��֤ǰ������һ���Զ����Ĵ����޸ģ�
				int ExistPIV;
				if((ExistPIV=HasPIntersectVExist(pPatchesPassV,PatchesIntersectV))==-1)
				{
					//�����ڲ�һ�µ����

					//��������б�PatchesIntersectV��
					PatchesIntersectV.Add(pPatchesPassV);	
					
					//�����������¼����
					VDegree.Add(pPatchesPassV->GetSize());
					
					//���ö�����VerteciesAdded�е�id��ӵ�
					//�ཻ�ڸö���ĸ�����Ƭ��Ӧ��VerticesInPatch��
					int vid=VerteciesAdded.GetSize()-1;
					int npid;
					for(i=0;i<pPatchesPassV->GetSize();i++)
					{
						npid=pPatchesPassV->GetAt(i);
						VerticesInPatch[npid]->Add(vid);
					}
				}
				else
				{
					//�����˲�һ�µ��������ʱ������������Ƭ�ཻ���������
					VerteciesAdded.RemoveTail();		//ɾ���õ�

					AArray<int,int>* pExistPIV=PatchesIntersectV[ExistPIV];
					PatchesIntersectV[ExistPIV]=pPatchesPassV;
					VDegree[ExistPIV]=pPatchesPassV->GetSize();		
					//���ö�����VerteciesAdded�е�id��ӵ�
					//�ཻ�ڸö���ĸ�����Ƭ��Ӧ��VerticesInPatch��
					//int vid=VerteciesAdded.GetSize()-1;
					int npid;
					for(i=0;i<pPatchesPassV->GetSize();i++)
					{
						npid=pPatchesPassV->GetAt(i);
						if(!InArray(npid,pExistPIV))			//�����npidû�г��ֹ��������vid
							VerticesInPatch[npid]->Add(ExistPIV);
					}
					
					delete pExistPIV;		//�ͷ��ڴ�
				}

			}
			else
				delete pPatchesPassV;			//��ǰ������Ч������ǰ�������ڴ�

		}//�޽������ݲ�����
	}// end of while-loop

	//���û�кϷ����½��㣬������Ӧ�÷���false��
	//�������ԣ�Ӧ�ò�̫�س��������������Ϊ�ڼ�����С���ʱ
	//��Ӧ���ų������޽���������˻���һ����С������ͱ��������һ���Ե�����
	if(VerteciesAdded.GetSize()==0)
	{
		//�ͷ��ڴ�

		for(i=0;i<PatchesIntersectV.GetSize();i++)
			delete PatchesIntersectV[i];
		for(i=0;i<VerticesInPatch.GetSize();i++)
			delete VerticesInPatch[i];

		return false;
	}

	/////////////////////////////////////////////////////////////////////
	// ��Polytope�в����½��㲿�֣�
	/////////////////////////////////////////////////////////////////////
	//��¼����ǰPolytope���еĶ�����Ŀ���½��������ȫ��id������Ϊ��g_vid,g_vid+1,g_vid+2...
	//��˿ɹ��Ժ������Ƭ�����򶥵��б�ʹ��
	int g_vid=m_pConvexPolytope->GetVNum();	
	for(i=0;i<VerteciesAdded.GetSize();i++)
	{
		VertexInfo vInfo;
		vInfo.cDegree=VDegree[i];
		m_pConvexPolytope->AddV(VerteciesAdded[i],vInfo);
	}
	
	
	//����֮ǰӦ�Ƚ���ǰ��������Ƭ���ݣ���һ�����Կ��Ƿ���CPolytope����Removed()����
	//ǰ���У�

	/////////////////////////////////////////////////////////////////////
	// ����������Ƭ�������б�(���������������Ƭ)
	/////////////////////////////////////////////////////////////////////
	CPatch* pCurNP;			//��ǰ��������Ƭ
	int g_vid1,g_vid2;		//��ǰ��������Ƭ��Ӧ�Ķ��㣬���ڱ��ϵ���һ�����㣨����ȫ��id��
	/////////////////////////////////////////////////////////////////////
	// ��������������ע������������Ƭ�У��ߵķ��������෴����������
	//
	//				 ��ǰ��ƬP
	//             \  ---->   /
	//	��Ӧ(Pnk) v1\________/ v2 ��Ӧ������Ƭ(Pn2)
	//              /        \
	//             / <-----   \
	//				  ������Ƭ(Pn1)
	//
	/////////////////////////////////////////////////////////////////////
	int next,pre;
	AArray<int,int> VSorted;				//���ն�������˳�����Ӻõ�����
	AArray<int,int>* pArrVertices;
	int vid,lastvid;
	for(i=0;i<npCount;i++)					//���ѭ������ÿһ��������Ƭ�ֱ��ǣ�ѭ��������һ��������Ƭ�Ĵ���
	{
		VSorted.RemoveAll();				//����ϴ�ѭ���еĶ��������б�

		next=(i+1<npCount)?i+1:0;			//��һ��neighbor���������������������ʾ��״�ṹ�����Ҫ��һ������
		pre=(i-1<0)?npCount-1:i-1;			//��һ��neighbor������

		pCurNP=m_arrNeighbors[i].pNeighborPatch;
		g_vid1=m_arrNeighbors[i].vid;
		g_vid2=m_arrNeighbors[next].vid;
		pArrVertices=VerticesInPatch[i];
		
		/////////////////////////////////////////////////////////////////////
		// ��ʼ��������
		// ע������Ķ��������㷨�ƺ�û�б�Ȼ�ó���ȷ����ı���
		// ��ˣ���һ��ҲӦ��Ϊ���Ժ͵��Ե��ص㣡
		/////////////////////////////////////////////////////////////////////

		//�Ȳ�����vid2�ڽӵĶ���
		int j(0);
		for(j=0;j<pArrVertices->GetSize();j++)
		{
			vid=pArrVertices->GetAt(j);
			if(InArray(i,PatchesIntersectV[vid]) &&		//vid1��Ӧ��������Ƭ(Pn1)��vid
			   InArray(next,PatchesIntersectV[vid]))	//vid2��Ӧ��������Ƭ(Pn2)��vid
			{
				//˵��vid��v2���ڽ�
				VSorted.Add(vid);		//����������
				break;					//�ҵ��ˣ�����
			}
		}
		//ȷ���ҵ���
		ASSERT_IN_REMOVE_PATCH(VSorted.GetSize()==1);	

		//Ȼ������Ѱ�Ҳ��������ڵ��������㣡
		
		//lastvid��־���һ���ѱ����ӵĶ���
		//����llvid���־�����ڶ��������ӵĶ��㣬����ı�����Ϊ�˱����ظ����ӣ�
		int llvid;	
		int counter=1;			//����counter��ֹ��ѭ��
		while(counter<pArrVertices->GetSize())
		{
			counter++;

			lastvid=VSorted[VSorted.GetSize()-1];
			if(VSorted.GetSize()<2)
				llvid=-1;		//һ����Чֵ����Ϊ��ʱ��û�е����ڶ���
			else
				llvid=VSorted[VSorted.GetSize()-2];

			for(j=0;j<pArrVertices->GetSize();j++)
			{
				vid=pArrVertices->GetAt(j);
				//����ط�Ӧ�������жϣ��������ظ����ӣ�
				if((vid!=lastvid) && (vid!=llvid) && IsVAdjacent(PatchesIntersectV[lastvid],PatchesIntersectV[vid]))	//����
				{
					VSorted.Add(vid);
					break;
				}
			} // end of for-loop
		} // end of while-loop
		
		ASSERT_IN_REMOVE_PATCH(counter==VSorted.GetSize());
		
		lastvid=VSorted[VSorted.GetSize()-1];
		
		//ȷ�����һ������lastvidӦ���Ǻ�v1�ڽӵ�
		ASSERT_IN_REMOVE_PATCH(InArray(pre,PatchesIntersectV[lastvid]) && 
							   InArray(i,PatchesIntersectV[lastvid]) );
			   
		/////////////////////////////////////////////////////////////////////
		// ȷ���������ӹ�ϵ������Կ�ʼ������Ƭ�Ķ�����������
		/////////////////////////////////////////////////////////////////////
		//�ж�v1��v2�Ƿ�Ӧ�ôӵ�ǰ������Ƭ��Neighbor��ɾ����
		//�жϷ�������v2Ϊ���������v2����Ƭ����P,Pn1,Pn2����v2����ɾ���ˣ�����Լ�Ϊ��������Ƿ�Ϊ3��>3
		AArray<VPNeighbor,VPNeighbor>& Neighbors=pCurNP->GetNeighbors();
		//Ѱ��v2�ڵ�ǰ������Ƭ�е�λ��
		for(j=0;j<Neighbors.GetSize();j++)
		{
			int tmp=Neighbors[j].vid;
			if(g_vid2==Neighbors[j].vid)
				break;
		}
		//ѭ��������j�����˵�ǰ��v2λ��
		int v2pos=j;
		ASSERT_IN_REMOVE_PATCH(v2pos<Neighbors.GetSize());				//ȷ��һ���ҵ�
		ASSERT_IN_REMOVE_PATCH(g_vid1==pCurNP->GetNextV(v2pos));		//��ȷ��v2����һ����Ӷ�����v1
		
		bool bv2Last=(v2pos==Neighbors.GetSize()-1);	//v2�Ƿ���Neighbor�����һ��Ԫ�أ�

		//�ȴ���v1����Ϊ����Ĳ����Ƚϼ򵥣�ֱ��ɾ���������ɣ�
		//�ж�v1�Ƿ�Ӧ�ôӵ�ǰ������Ƭ��Neighbor��ɾ��
		VertexInfo& v1Info=m_pConvexPolytope->GetVInfo(g_vid1);
		if(v1Info.cDegree==3)
		{
			//ɾ��������v1�ڵ�ǰpatch�е�����ΪpCurNP->GetNext(j)
			Neighbors.RemoveAt(pCurNP->GetNext(v2pos));	
			if(bv2Last)		//˵��v1�ǵ�һ��Ԫ�أ���ˣ�ɾ��v1��Ӧ��v2pos�Լ�
				v2pos--;
		}
		int InsertPos;				//��������ӵ�λ��

		//����v2�Ƿ�ɾ��������
		VertexInfo& v2Info=m_pConvexPolytope->GetVInfo(g_vid2);
		if(v2Info.cDegree==3)
		{
			//��Ӧɾ��v2
			Neighbors.RemoveAt(v2pos);
			InsertPos=v2pos;						//v2�ѱ�ɾ����ֱ�Ӵ�v2pos����
		}
		else
		{
			//��ɾ��v2���������޸���pNeighborPatch
			Neighbors[v2pos].pNeighborPatch=m_arrNeighbors[next].pNeighborPatch;	//�޸����ӦNeighborPatchΪPn2!
			InsertPos=pCurNP->GetNext(v2pos);		//�������v2pos�����һ��Ԫ����
		}		
		
		//������Ҫ��ӵ�����
		AArray<VPNeighbor,VPNeighbor> NeighborsToAdd;		//��Ҫ��ӵ�����
		VPNeighbor vpn;
		for(j=0;j<VSorted.GetSize();j++)					
		{
			vpn.vid =VSorted[j]+g_vid;						//ע�⣬����Ҫ��ȫ��id�����Ӧ����g_vid
			if(j==VSorted.GetSize()-1)	
			{
				//���һ������ڵ�Ҫ���⴦��
				//��������ƬӦ���ڱ���Ƭ����һ��������Ƭ
				vpn.pNeighborPatch=m_arrNeighbors[pre].pNeighborPatch;	
			}
			else
			{
				//Ҫ��һ�������㷨���ҳ���Ӧ�ڸõ��������Ƭ��
				for(int k=0;k<npCount;k++)
				{
					if(k!=i)		//���ǵ�ǰ���ڿ������Ƭ
					{
						AArray<int,int>* pVertecies=VerticesInPatch[k];
						if(InArray(VSorted[j],pVertecies) && InArray(VSorted[j+1],pVertecies) )
						{
							//��VSorted[j,j+1]���ɵı߳�����pVertecies��
							//˵����k��������Ƭ��Ӧ�˶���VSorted[j]
							vpn.pNeighborPatch=m_arrNeighbors[k].pNeighborPatch;
						}

					}
				}
			}
			ASSERT_IN_REMOVE_PATCH(vpn.pNeighborPatch);
			NeighborsToAdd.Add(vpn);
		}

		//�����µ�����
		Neighbors.InsertAt(InsertPos,NeighborsToAdd);
		
		//��ǰ������ƬpCurNP���µ�������OK����Ҫ������и���ɾ����������
		pCurNP->UpdateRemovedError();

	} // end of for-loop
	
	//ѭ��������ǰ��Ƭ��ÿһ�����㣬����������Լ�1
	int curvid;
	for(i=0;i<npCount;i++)
	{
		curvid=m_arrNeighbors[i].vid;
		VertexInfo& curVInfo=m_pConvexPolytope->GetVInfo(curvid);
		curVInfo.cDegree--;
	}

	//�ͷ��ڴ�
	for(i=0;i<PatchesIntersectV.GetSize();i++)
		delete PatchesIntersectV[i];
	for(i=0;i<VerticesInPatch.GetSize();i++)
		delete VerticesInPatch[i];

	return true;		//ɾ���ɹ�

//�����쳣
ExceptionOccur:
	//�ͷ��ڴ�
	for(i=0;i<PatchesIntersectV.GetSize();i++)
		delete PatchesIntersectV[i];
	for(i=0;i<VerticesInPatch.GetSize();i++)
		delete VerticesInPatch[i];
	
	//��ɾ�������Ϊ��Ч��
	m_fRemovedError=-1.0f;

	//�׳��쳣
	m_pConvexPolytope->ThrowException();
	
	//���أ�ɾ��ʧ��
	return false;
}

//////////////////////////////////////////////////////////////
// �ж�n[0,1,2]������Ԫ���Ƿ��Ѿ������ڶ����������ཻ��һ��
// ��ͬ���б��С�
// ����ǣ��������¼���������id��Ӧ����Ƭ�Ľ����ˣ�
// ��Removed()���е���
//////////////////////////////////////////////////////////////
bool CPatch::Processed(int n[], AArray<AArray<int,int>*,AArray<int,int>*>& CoSolutionList)
{
	bool bInArray[3];
	AArray<int,int>* pArr;

	for(int i=0;i<CoSolutionList.GetSize();i++)
	{
		pArr=CoSolutionList[i];
		assert(pArr->GetSize()>=3);
		if(pArr->GetSize()==3)			//��׼�������������һ���⣬ֱ������
			continue;
		
		bInArray[0]=false;				//��ʼ��
		bInArray[1]=false;				//��ʼ��
		bInArray[2]=false;				//��ʼ��

		//������������ͬ������
		for(int j=0;j<pArr->GetSize();j++)
		{
			if(n[0]==pArr->GetAt(j)) bInArray[0]=true;
			if(n[1]==pArr->GetAt(j)) bInArray[1]=true;
			if(n[2]==pArr->GetAt(j)) bInArray[2]=true;
		}

		if(bInArray[0] && bInArray[1] && bInArray[2])
			return true;
	}
	
	return false;
}

bool CPatch::InArray(int pid, AArray<int,int>* pArr)
{
	for(int i=0;i<pArr->GetSize();i++)
		if(pid==pArr->GetAt(i))
			return true;
	return false;
}

//////////////////////////////////////////////////////////////
// �ж��¼�������������Ƿ��ڽ�
// �䷽��Ϊ���ж������������Ӧ����Ƭ�����У��Ƿ���������ȫ��ͬ��
// ��Removed()���е���
//////////////////////////////////////////////////////////////
bool CPatch::IsVAdjacent(AArray<int,int>* pArr1, AArray<int,int>* pArr2)
{
	int count=0;
	for(int i=0;i<pArr1->GetSize();i++)
		if(InArray(pArr1->GetAt(i),pArr2))
		{
			count++;
			if(count==2) return true;
		}
	return false;
}

//�ж�pPatch�Ƿ��ڵ�ǰ��Ƭ��������Ƭ֮��
bool CPatch::InNeighbors(CPatch *pPatch)
{
	for(int i=0;i<m_arrNeighbors.GetSize();i++)
	{
		if(pPatch==m_arrNeighbors[i].pNeighborPatch)
			return true;
	}
	
	return false;

}

//////////////////////////////////////////////////////////////
// ��дһ�����ƹ��캯��������ȱʡ�ĸ��ƹ��캯��
// �Ӷ������ڿ���ʱ��m_arrNeighbors��ֱ�Ӹ�ֵ
// ������ָ��Ŀ������Ӷ������ڴ��ɾ������
//////////////////////////////////////////////////////////////
CPatch::CPatch(const CPatch &patch)
{
	m_vNormal=patch.m_vNormal;
	m_d=patch.m_d;
	m_fRemovedError=patch.m_fRemovedError;
	m_pConvexPolytope=patch.m_pConvexPolytope;

	//ע����һ��Ĳ�ͬ
	int nSize=patch.m_arrNeighbors.GetSize();
	for(int i=0;i<nSize;i++)
	{
		VPNeighbor vpNB=patch.m_arrNeighbors[i];
		m_arrNeighbors.Add(patch.m_arrNeighbors[i]);
	}

}

//////////////////////////////////////////////////////////////
// ��дһ����ֵ�������Ӷ�����ָ�����
// �ǳ�������������д�ĸ��ƹ��캯���������в�ͬ
//////////////////////////////////////////////////////////////
const CPatch& CPatch::operator=(const CPatch& patch)
{
	if(this!=&patch)	//��ֹ����ֵ
	{
		m_vNormal=patch.m_vNormal;
		m_d=patch.m_d;
		m_fRemovedError=patch.m_fRemovedError;
		m_pConvexPolytope=patch.m_pConvexPolytope;

		//ע����һ��Ĳ�ͬ
		int nSize=patch.m_arrNeighbors.GetSize();

		m_arrNeighbors.RemoveAll();			//�����һ���������
		
		for(int i=0;i<nSize;i++)
		{
			VPNeighbor vpNB=patch.m_arrNeighbors[i];
			m_arrNeighbors.Add(patch.m_arrNeighbors[i]);
		}

	}
	
	return *this;
}

//////////////////////////////////////////////////////////////
// Added by wf,04-10-09
// Ϊ��֤���㽻���һ���Զ���ӵĺ���
// ������ǰ�������ÿһ���ཻ��һ���ƽ��Ƭ���������Ƿ���
// ���԰����ڵ�ǰ������Ľ���һ���ƽ��Ƭ��PIntersectV
// ����У��򷵻�����PatchesIntersectV������id
// ���򣬷���-1
//////////////////////////////////////////////////////////////
int CPatch::HasPIntersectVExist(AArray<int,int>* pPIntersectV,const AArray<AArray<int,int>*,AArray<int,int>*>& PatchesIntersectV)
{
	if(pPIntersectV->GetSize()==3)		
		return -1;			//ֻ������Ԫ�أ���˲����ڰ�����ϵ��ֱ�ӷ��أ�1

	AArray<int,int>* pPIV;
	int pid;
	bool bIn;
	for(int i=0;i<PatchesIntersectV.GetSize();i++)
	{
		bIn=true;
		pPIV=PatchesIntersectV[i];
		for(int j=0;j<pPIV->GetSize();j++)
		{
			pid=pPIV->GetAt(j);
			if(!InArray(pid,pPIntersectV))
			{
				bIn=false;
				break;
			}
		}
		if(bIn)
			return i;
	}

	return -1;

}
///////////////////////////////////////////////////
// ������Ƭ�����ж���Ķ�����1
// �⽫������Ƭ��ɾ�������ָ���ʱ��
///////////////////////////////////////////////////
void CPatch::IncreVDegree()
{
	int vid;
	for(int i=0;i<m_arrNeighbors.GetSize();i++)
	{
		vid=m_arrNeighbors[i].vid;
		m_pConvexPolytope->GetVInfo(vid).cDegree++;
	}
}

///////////////////////////////////////////////////
// ������Ƭ�����ж���Ķ�����1
// �⽫������Ƭ��ɾ���ָ���������ɾ����ʱ��
///////////////////////////////////////////////////
void CPatch::DecreVDegree()
{
	int vid;
	for(int i=0;i<m_arrNeighbors.GetSize();i++)
	{
		vid=m_arrNeighbors[i].vid;
		m_pConvexPolytope->GetVInfo(vid).cDegree--;
	}

}

A3DVECTOR3 CPatch::GetVertex(int vid) 
{ 
	return m_pConvexPolytope->GetV(m_arrNeighbors[vid].vid);
}

void CPatch::GetEdge(int id,A3DVECTOR3& v1,A3DVECTOR3& v2)
{
	v1=m_pConvexPolytope->GetV(m_arrNeighbors[id].vid);
	v2=m_pConvexPolytope->GetV(GetNextV(id));
}

}	// end namespace