// Face.cpp: implementation of the CFace class.
//
//////////////////////////////////////////////////////////////////////

#include "Face.h"
#include "ConvexHullData.h"
#include "LEquations.h"

namespace CHBasedCD
{

	// �������ߵķ�����< AngleAcuteThreshʱ����Ϊ�����߼�һ����ǣ�
	// ȱʡ����ֵΪ 30 deg�� ��ˣ�ֵΪ-sqrt(3)/2
	float CFace::AngleAcuteThresh=-0.866f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFace::CFace()
{

}

CFace::~CFace()
{

}

/************************************************************************
// Removed by wenfeng, 05-3-28
// ���������г���std::fstream�Ŀ⣬����������ҪVC��֧�֣�

bool CFace::LoadFromStream(std::ifstream& InFile)
{
	char buf[MAX_LEN];
	InFile.getline(buf,MAX_LEN);	
	sscanf(buf,"  Face Parameter: %f %f %f %f",&m_vNormal.x,&m_vNormal.y,&m_vNormal.z,&m_d);
	
	int Num;
	InFile.getline(buf,MAX_LEN);	
	sscanf(buf,"  Element Num: %d",&Num);

	for(int i=0;i<Num;i++)
	{
		int id;
		CHalfSpace hs;
		A3DVECTOR3 n;
		float d;
		
		InFile.getline(buf,MAX_LEN);	
		sscanf(buf,"%d %f %f %f %f",&id,&n.x,&n.y,&n.z,&d);
		hs.SetD(d);
		hs.SetNormal(n);
		
		AddElement(id,hs);
	}

	//�������һ������
	InFile.getline(buf,MAX_LEN);

	return true;
}

bool CFace::SaveToStream(std::ofstream& OutFile) const
{
	using namespace std;

	OutFile<<"  Face Parameter: "<<m_vNormal.x<<" "<<m_vNormal.y<<" "<<m_vNormal.z<<" "<<m_d<<endl;
	OutFile<<"  Element Num: "<<GetEdgeNum()<<endl;
	for(int i=0;i<GetEdgeNum();i++ )
	{
		OutFile<<"    "<<m_arrVIDs[i]<<" ";
		A3DVECTOR3 n=m_arrEdgeHSs[i].GetNormal();
		OutFile<<n.x<<" "<<n.y<<" "<<n.z<<" "<<m_arrEdgeHSs[i].GetDist()<<endl;
	}

	OutFile<<endl;
	
	return true;
}
/************************************************************************/

bool CFace::LoadFromStream(FILE* InFile)
{
	if(!InFile) return false;

	fscanf(InFile,"  Face Parameter: %f %f %f %f\n",&m_vNormal.x,&m_vNormal.y,&m_vNormal.z,&m_d);
	int Num;
	fscanf(InFile,"  Element Num: %d\n",&Num);

	for(int i=0;i<Num;i++)
	{
		int id;
		CHalfSpace hs;
		A3DVECTOR3 n;
		float d;
		
		fscanf(InFile,"%d %f %f %f %f\n",&id,&n.x,&n.y,&n.z,&d);
		hs.SetD(d);
		hs.SetNormal(n);
		
		AddElement(id,hs);
	}

	//�������һ������
	fscanf(InFile,"\n"); 
	
	return true;
}

bool CFace::SaveToStream(FILE* OutFile) const
{
	if(!OutFile) return false;
	fprintf(OutFile,"  Face Parameter: %f %f %f %f\n",m_vNormal.x,m_vNormal.y,m_vNormal.z,m_d);
	fprintf(OutFile,"  Element Num: %d\n",GetEdgeNum());
	for(int i=0;i<GetEdgeNum();i++ )
	{
		A3DVECTOR3 n=m_arrEdgeHSs[i].GetNormal();
		fprintf(OutFile,"    %d %f %f %f %f\n",m_arrVIDs[i],n.x,n.y,n.z,m_arrEdgeHSs[i].GetDist());
	}

	//��һ������
	fprintf(OutFile,"\n");
	
	return true;
}
//////////////////////////////////////////////////////////////
// ��дһ�����ƹ��캯��������ȱʡ�ĸ��ƹ��캯��
// �Ӷ������ڿ���ʱ��m_arrVIDs,m_arrEdgeHSs��ֱ�Ӹ�ֵ
// ������ָ��Ŀ������Ӷ������ڴ��ɾ������
//////////////////////////////////////////////////////////////
CFace::CFace(const CFace& face)
{
	SetNormal(face.m_vNormal);
	SetD(face.m_d);
		
	int i(0);
	for(i=0;i<face.GetVNum();i++)
	{
		int vid=face.GetVID(i);
		CHalfSpace hs=face.GetEdgeHalfSpace(i);
		AddElement(face.GetVID(i),face.GetEdgeHalfSpace(i));
	}
	
	for(i=0;i<face.GetExtraHSNum();i++)
	{
		AddExtraHS(face.GetExtraHalfSpace(i));
	}

}

const CFace& CFace::operator=(const CFace& face)
{
	SetNormal(face.m_vNormal);
	SetD(face.m_d);

	int i(0);
	for(i=0;i<face.GetVNum();i++)
	{
		int vid=face.GetVID(i);
		CHalfSpace hs=face.GetEdgeHalfSpace(i);
		AddElement(face.GetVID(i),face.GetEdgeHalfSpace(i));
	}

	for(i=0;i<face.GetExtraHSNum();i++)
	{
		AddExtraHS(face.GetExtraHalfSpace(i));
	}

	return *this;
}

void CFace::Transform(const A3DMATRIX4& mtxTrans)
{
	//���ø��෽���任����ƽ��ķ���
	CHalfSpace::Transform(mtxTrans);

	int i(0);
	for(i=0;i<m_arrEdgeHSs.GetSize();i++)
	{
		m_arrEdgeHSs[i].Transform(mtxTrans);
	}

	for(i=0;i<m_arrExtraHSs.GetSize();i++)
	{
		m_arrExtraHSs[i].Transform(mtxTrans);
	}

}

void CFace::ComputeExtraHS()
{
	if(m_arrEdgeHSs.GetSize()==0) return;

	A3DVECTOR3 curN,NextN;
	int vi;		//��������
	for(int i=0; i<m_arrEdgeHSs.GetSize();i++)
	{
		curN=m_arrEdgeHSs[i].GetNormal();
		if(i==m_arrEdgeHSs.GetSize()-1)
		{
			NextN=m_arrEdgeHSs[0].GetNormal();
			vi=0;
		}
		else
		{
			NextN=m_arrEdgeHSs[i+1].GetNormal();
			vi=i+1;
		}
			
		if(DotProduct(curN,NextN) < AngleAcuteThresh)
			AddExtraHS(vi);
	}
}

void CFace::AddExtraHS(int i)
{
	A3DVECTOR3 vCurVertex(m_pCHData->GetVertex(GetVID(i)));
	int prei=(i==0)?GetVNum()-1:i-1;
	
	// ��ⷽ�̣�

	//����3*3���Է�����
	CMatrix	mtxCoef(3,3);		// ϵ������
	CMatrix mtxConst(3,1);		// ��������
	CMatrix	mtxResult(3,1);		// ���		

	mtxCoef.SetElement(0,0,m_arrEdgeHSs[prei].GetNormal().x);
	mtxCoef.SetElement(0,1,m_arrEdgeHSs[prei].GetNormal().y);
	mtxCoef.SetElement(0,2,m_arrEdgeHSs[prei].GetNormal().z);

	mtxCoef.SetElement(1,0,m_arrEdgeHSs[i].GetNormal().x);
	mtxCoef.SetElement(1,1,m_arrEdgeHSs[i].GetNormal().y);
	mtxCoef.SetElement(1,2,m_arrEdgeHSs[i].GetNormal().z);

	mtxCoef.SetElement(2,0,GetNormal().x);
	mtxCoef.SetElement(2,1,GetNormal().y);
	mtxCoef.SetElement(2,2,GetNormal().z);

	//��ʼ����������
	mtxConst.SetElement(0,0,m_arrEdgeHSs[prei].GetDist()+1.0f);
	mtxConst.SetElement(1,0,m_arrEdgeHSs[i].GetDist()+1.0f);
	mtxConst.SetElement(2,0,GetDist());
	
	CLEquations le(mtxCoef,mtxConst);
	if(!le.GetRootsetGauss(mtxResult))
		assert(1);				//ȷ�����ɹ�	
	
	A3DVECTOR3 v((float)mtxResult.GetElement(0,0), (float)mtxResult.GetElement(1,0), (float)mtxResult.GetElement(2,0));
	v-=vCurVertex;
	CHalfSpace hs;
	hs.SetNV(v,vCurVertex);
	AddExtraHS(hs);
}

CHalfSpace CFace::GetNearestHS(const A3DVECTOR3& vPos ) const
{
	float fMinDist=1000.0f, fDist;
	int iHS=-1;
	bool bHSInEdge = true;

	int i(0);
	for(i=0; i<m_arrEdgeHSs.GetSize(); i++ )
	{
		fDist = fabs(m_arrEdgeHSs[i].DistV2Plane(vPos));
		if(  fDist < fMinDist)
		{
			fMinDist = fDist;
			iHS = i;
		}
	}

	for( i=0; i<m_arrExtraHSs.GetSize(); i++ )
	{
		fDist = fabs(m_arrExtraHSs[i].DistV2Plane(vPos));
		if(  fDist < fMinDist)
		{
			fMinDist = fDist;
			iHS = i;
			bHSInEdge = false;
		}
	}
	
	CHalfSpace hs;
	if(iHS>=0)
	{
		hs = (bHSInEdge)? m_arrEdgeHSs[iHS] : m_arrExtraHSs[iHS];
	}
	return hs;
}

}	// end namespace