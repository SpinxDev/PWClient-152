 // 2DObstruct.cpp: implementation of the C2DObstruct class.
//
//////////////////////////////////////////////////////////////////////

#include "2DObstruct.h"
#include "LEquations.h"
#include "ALog.h"

//#include <fstream>

extern ALog g_Log;

namespace CHBasedCD
{
	
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2DObstruct::C2DObstruct()
{

}

C2DObstruct::~C2DObstruct()
{

}

void C2DObstruct::Init(const vector<A3DVECTOR3>& Vertices, float fHeight)
{
	if(Vertices.size()<=3) return;		//����Ҫ��3������
	int vNum=Vertices.size();

	//��Ӷ���
	int i(0);
	for(i=0;i<vNum;i++)
	{
		A3DVECTOR3 v(Vertices[i]);
		v.y=0.0f;
		AddVertex(v);
		v.y=fHeight;
		AddVertex(v);
	}

	//��ӵ���
	CFace face;
	face.SetNormal(A3DVECTOR3(0.0f,-1.0f,0.0f));		//����Ϊ-Y
	face.SetD(0.0f);
	for(i=0;i<vNum;i++)
	{
		int vid=2*i;
		CHalfSpace hs;
		if(i!=vNum-1)	
			hs.Set(m_arrVertices[vid],m_arrVertices[vid+2],m_arrVertices[vid+2]+face.GetNormal());
		else
			hs.Set(m_arrVertices[vid],m_arrVertices[0],m_arrVertices[0]+face.GetNormal());
		face.AddElement(vid,hs);
	}
	AddFace(face);
	
	//��Ӷ���
	face.Reset();
	face.SetNormal(A3DVECTOR3(0.0f,1.0f,0.0f));		//����Ϊ+Y
	face.SetD(fHeight);
	for(i=vNum-1;i>=0;i--)
	{
		int vid=2*i+1;
		CHalfSpace hs;
		if (i!=0)
			hs.Set(m_arrVertices[vid],m_arrVertices[vid-2],m_arrVertices[vid-2]+face.GetNormal());
		else
			hs.Set(m_arrVertices[vid],m_arrVertices[vNum-1],m_arrVertices[vNum-1]+face.GetNormal());
		face.AddElement(vid,hs);
	}
	AddFace(face);

	//��Ӹ�������
	for(i=0;i<vNum;i++)
	{
		//ÿ�������Ϊһ�����Σ�
		int vid1,vid2,vid3,vid4;
		if(i!=vNum-1)
		{
			vid1=2*i;
			vid2=2*i+1;
			vid3=2*i+3;
			vid4=2*i+2;
		}
		else
		{
			vid1=2*i;
			vid2=2*i+1;
			vid3=1;
			vid4=0;
		}
		
		face.Reset();
		face.Set(m_arrVertices[vid1],m_arrVertices[vid2],m_arrVertices[vid3]);
		CHalfSpace hs;

		hs.Set(m_arrVertices[vid1],m_arrVertices[vid2],m_arrVertices[vid2]+face.GetNormal());
		face.AddElement(vid1,hs);

		hs.Set(m_arrVertices[vid2],m_arrVertices[vid3],m_arrVertices[vid3]+face.GetNormal());
		face.AddElement(vid2,hs);

		hs.Set(m_arrVertices[vid3],m_arrVertices[vid4],m_arrVertices[vid4]+face.GetNormal());
		face.AddElement(vid3,hs);

		hs.Set(m_arrVertices[vid4],m_arrVertices[vid1],m_arrVertices[vid1]+face.GetNormal());
		face.AddElement(vid4,hs);

		AddFace(face);
	}	

	//���������������
	
	//����2*2���Է�����
	CMatrix	mtxCoef(2,2);		// ϵ������
	CMatrix mtxConst(2,1);		// ��������
	CMatrix	mtxResult(2,1);		// ���	
	
	//��ʼ����������
	mtxConst.SetElement(0,0,1.0);
	mtxConst.SetElement(1,0,1.0);

	CFace *pf1,*pf2;
	for(i=0;i<vNum;i++)
	{
		pf1=GetFaceByVID(2*i,CLOCKWISE);
		pf2=GetFaceByVID(2*i,ANTICLOCKWISE);
		if(pf1->GetNormal()==pf2->GetNormal())
		{
			//�������Ƭ�ķ�����ͬ��������������Ϊ�÷�����
			m_arrInflateVecs.Add(pf1->GetNormal());
			continue;
		}
		//�����������ⷽ�����Լ�����������
		mtxCoef.SetElement(0,0,pf1->GetNormal().x);
		mtxCoef.SetElement(0,1,pf1->GetNormal().z);

		mtxCoef.SetElement(1,0,pf2->GetNormal().x);
		mtxCoef.SetElement(1,1,pf2->GetNormal().z);

		CLEquations le(mtxCoef,mtxConst);
		if(!le.GetRootsetGauss(mtxResult))
			assert(1);				//ȷ�����ɹ�
		A3DVECTOR3 v((float)mtxResult.GetElement(0,0),0.0f,(float)mtxResult.GetElement(1,0));
		m_arrInflateVecs.Add(v);

	}

}

void C2DObstruct::GetCRInfo(CFace* pCDFace,const A3DVECTOR3& Dir,int& RotateDir,int& RelayVID,int CurRotateDir)
{
	if(!pCDFace) return;
	
	if(CurRotateDir>0)
	{
		//��ǰ�Ѵ�������״̬
		RotateDir=CurRotateDir;
		RelayVID=(RotateDir==ANTICLOCKWISE )?pCDFace->GetVID(3):pCDFace->GetVID(0);
		return;
	}
	///////////////////////////////////////////////
	//			v1 ----> v2
	//		   /|\		 |
	//		     |	     \|/
	//			v0 <---- v3
	///////////////////////////////////////////////
	
	//�����˶��������ײ������߾�������
	A3DVECTOR3 vAnticlockwise=GetVertex(pCDFace->GetVID(3))-GetVertex(pCDFace->GetVID(0));
	if(DotProduct(Dir,vAnticlockwise)>0.0f)
	{
		RotateDir=ANTICLOCKWISE;
		RelayVID=pCDFace->GetVID(3);
	}
	else
	{
		RotateDir=CLOCKWISE ;
		RelayVID=pCDFace->GetVID(0);
	}		
}

bool C2DObstruct::IsVertexConcave(int vid)
{
	//ע�⣺�����vidӦ�����������е�id�������Ƕ�ά�㼯��id����˱���
	//��vid��һ��>>1+2�Ĳ�����

	// ˳ʱ�뷽��ĺ�̵�
	int nextVID=GetNextVID(vid,CLOCKWISE);
	
	// ����õ���vid��Ӧ����Ƭ��࣬��˵���õ���һ������
	int fid=(vid>>1)+2;
	return m_arrFaces[fid]->Outside(m_arrVertices[nextVID]);
}

bool C2DObstruct::LoadFromStream(FILE* InFile)
{
	if(!CConvexHullData::LoadFromStream(InFile))
		return false;
	
	if(!InFile) return false;
	
	fscanf(InFile,"\n");
	fscanf(InFile,"Inflate Vectors:\n");
	
	int vecNum=GetVertexNum()>>1;
	
	for(int i=0;i<vecNum;i++)
	{
		A3DVECTOR3 v;
		fscanf(InFile,"%f %f %f\n",&v.x,&v.y,&v.z);
		m_arrInflateVecs.Add(v);
	}
	
	//����һ������
	fscanf(InFile,"\n");
	
	return true;
}

bool C2DObstruct::SaveToStream(FILE* OutFile) const
{
	if(!CConvexHullData::SaveToStream(OutFile))
		return false;
	
	if(!OutFile) return false;

	//������������
	fprintf(OutFile,"\n");							//����
	fprintf(OutFile,"Inflate Vectors:\n");	  //��־

	for(int i=0;i<m_arrInflateVecs.GetSize();i++)
		fprintf(OutFile,"  %f  %f  %f\n",m_arrInflateVecs[i].x,m_arrInflateVecs[i].y,m_arrInflateVecs[i].z);
	
	fprintf(OutFile,"\n");							//����

	return true;
}

/************************************************************************
// Removed by wenfeng, 05-3-28
// ���������г���std::fstream�Ŀ⣬����������ҪVC��֧�֣�

bool C2DObstruct::LoadFromStream(std::ifstream& InFile)
{
	if(!CConvexHullData::LoadFromStream(InFile))
		return false;
	
	if(!InFile.is_open()) return false;

	char buf[MAX_LEN];
	//���м���־
	InFile.getline(buf,MAX_LEN);
	InFile.getline(buf,MAX_LEN);

	int vecNum=GetVertexNum()>>1;
	
	for(int i=0;i<vecNum;i++)
	{
		InFile.getline(buf,MAX_LEN);		//����һ���ַ���
		A3DVECTOR3 v;
		sscanf(buf,"%f %f %f",&v.x,&v.y,&v.z);
		m_arrInflateVecs.Add(v);
	}

	InFile.getline(buf,MAX_LEN);		//����һ������

	return true;
}

bool C2DObstruct::SaveToStream(std::ofstream& OutFile) const
{
	using namespace std;

	if(!CConvexHullData::SaveToStream(OutFile))
		return false;

	if(!OutFile.is_open())
		return false;
	
	//���м���־
	OutFile<<"\n";
	OutFile<<"Inflate Vectors:\n";

	for(int i=0;i<m_arrInflateVecs.GetSize();i++)
		OutFile<<"  "<<m_arrInflateVecs[i].x<<"  "<<m_arrInflateVecs[i].y<<"  "<<m_arrInflateVecs[i].z<<endl;

	OutFile<<"\n";						//����

	return true;
}

/************************************************************************/

int C2DObstruct::ComputeBufSize() const
{
	int BufSize=CConvexHullData::ComputeBufSize();
	BufSize+=3*sizeof(float)*m_arrInflateVecs.GetSize();
	return BufSize;
}

bool C2DObstruct::WriteToBuf(char* buf) const
{
	if(!CConvexHullData::WriteToBuf(buf))
		return false;

	//��д����������
	const int FloatSize=sizeof(float);
	int cur=CConvexHullData::ComputeBufSize();
	float fWrite;

	for(int i=0;i<m_arrInflateVecs.GetSize();i++)
	{
		fWrite=m_arrInflateVecs[i].x;
		memcpy(buf+cur,&fWrite,FloatSize);
		cur+=FloatSize;

		fWrite=m_arrInflateVecs[i].y;
		memcpy(buf+cur,&fWrite,FloatSize);
		cur+=FloatSize;

		fWrite=m_arrInflateVecs[i].z;
		memcpy(buf+cur,&fWrite,FloatSize);
		cur+=FloatSize;		
	}
	
	return true;
}

bool C2DObstruct::ReadFromBuf(char* buf)
{
	if(!CConvexHullData::ReadFromBuf(buf))
		return false;
	
	//������������
	int vecNum=GetVertexNum()>>1;
	
	const int FloatSize=sizeof(float);	
	int cur=CConvexHullData::ComputeBufSize();
	float fRead;
	
	for(int i=0;i<vecNum;i++)
	{
		A3DVECTOR3 v;
		memcpy(&fRead,buf+cur,FloatSize);
		cur+=FloatSize;
		v.x=fRead;

		memcpy(&fRead,buf+cur,FloatSize);
		cur+=FloatSize;
		v.y=fRead;

		memcpy(&fRead,buf+cur,FloatSize);
		cur+=FloatSize;
		v.z=fRead;

		m_arrInflateVecs.Add(v);
	}	
	
	return true;
}

void C2DObstruct::Transform(const A3DMATRIX4& mtxTrans)
{
	//���ø��෽��
	CConvexHullData::Transform(mtxTrans);

	//�������������б任
	//������������������Ƭ�ķ�����������ֻ���ñ任�е���ת����������б任����
	A3DMATRIX4 mtxRotate(mtxTrans);
	mtxRotate.SetRow(3,A3DVECTOR3(0.0f,0.0f,0.0f));
	float fScale=mtxTrans.GetCol(0).Magnitude();
	mtxRotate/=fScale;

	for(int i=0;i<m_arrInflateVecs.GetSize();i++)
		m_arrInflateVecs[i]=m_arrInflateVecs[i]*mtxRotate;


}

}	// end namespace

