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
	if(Vertices.size()<=3) return;		//至少要有3个顶点
	int vNum=Vertices.size();

	//添加顶点
	int i(0);
	for(i=0;i<vNum;i++)
	{
		A3DVECTOR3 v(Vertices[i]);
		v.y=0.0f;
		AddVertex(v);
		v.y=fHeight;
		AddVertex(v);
	}

	//添加底面
	CFace face;
	face.SetNormal(A3DVECTOR3(0.0f,-1.0f,0.0f));		//法向为-Y
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
	
	//添加顶面
	face.Reset();
	face.SetNormal(A3DVECTOR3(0.0f,1.0f,0.0f));		//法向为+Y
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

	//添加各个侧面
	for(i=0;i<vNum;i++)
	{
		//每个侧面均为一个矩形！
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

	//依次添加膨胀向量
	
	//构造2*2线性方程组
	CMatrix	mtxCoef(2,2);		// 系数矩阵
	CMatrix mtxConst(2,1);		// 常数矩阵
	CMatrix	mtxResult(2,1);		// 结果	
	
	//初始化常数矩阵
	mtxConst.SetElement(0,0,1.0);
	mtxConst.SetElement(1,0,1.0);

	CFace *pf1,*pf2;
	for(i=0;i<vNum;i++)
	{
		pf1=GetFaceByVID(2*i,CLOCKWISE);
		pf2=GetFaceByVID(2*i,ANTICLOCKWISE);
		if(pf1->GetNormal()==pf2->GetNormal())
		{
			//如果两面片的法向相同，则膨胀向量即为该法向量
			m_arrInflateVecs.Add(pf1->GetNormal());
			continue;
		}
		//其他情况，求解方程组以计算膨胀向量
		mtxCoef.SetElement(0,0,pf1->GetNormal().x);
		mtxCoef.SetElement(0,1,pf1->GetNormal().z);

		mtxCoef.SetElement(1,0,pf2->GetNormal().x);
		mtxCoef.SetElement(1,1,pf2->GetNormal().z);

		CLEquations le(mtxCoef,mtxConst);
		if(!le.GetRootsetGauss(mtxResult))
			assert(1);				//确保求解成功
		A3DVECTOR3 v((float)mtxResult.GetElement(0,0),0.0f,(float)mtxResult.GetElement(1,0));
		m_arrInflateVecs.Add(v);

	}

}

void C2DObstruct::GetCRInfo(CFace* pCDFace,const A3DVECTOR3& Dir,int& RotateDir,int& RelayVID,int CurRotateDir)
{
	if(!pCDFace) return;
	
	if(CurRotateDir>0)
	{
		//当前已处于绕行状态
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
	
	//根据运动方向和碰撞面的切线决定绕向
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
	//注意：这里的vid应该是在棱拄中的id，而不是二维点集的id，因此必须
	//对vid作一个>>1+2的操作！

	// 顺时针方向的后继点
	int nextVID=GetNextVID(vid,CLOCKWISE);
	
	// 如果该点在vid对应的面片外侧，则说明该点是一个凹点
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
	
	//消除一个换行
	fscanf(InFile,"\n");
	
	return true;
}

bool C2DObstruct::SaveToStream(FILE* OutFile) const
{
	if(!CConvexHullData::SaveToStream(OutFile))
		return false;
	
	if(!OutFile) return false;

	//保存膨胀向量
	fprintf(OutFile,"\n");							//换行
	fprintf(OutFile,"Inflate Vectors:\n");	  //标志

	for(int i=0;i<m_arrInflateVecs.GetSize();i++)
		fprintf(OutFile,"  %f  %f  %f\n",m_arrInflateVecs[i].x,m_arrInflateVecs[i].y,m_arrInflateVecs[i].z);
	
	fprintf(OutFile,"\n");							//换行

	return true;
}

/************************************************************************
// Removed by wenfeng, 05-3-28
// 避免引擎中出现std::fstream的库，由于其所需要VC的支持！

bool C2DObstruct::LoadFromStream(std::ifstream& InFile)
{
	if(!CConvexHullData::LoadFromStream(InFile))
		return false;
	
	if(!InFile.is_open()) return false;

	char buf[MAX_LEN];
	//换行及标志
	InFile.getline(buf,MAX_LEN);
	InFile.getline(buf,MAX_LEN);

	int vecNum=GetVertexNum()>>1;
	
	for(int i=0;i<vecNum;i++)
	{
		InFile.getline(buf,MAX_LEN);		//读出一行字符串
		A3DVECTOR3 v;
		sscanf(buf,"%f %f %f",&v.x,&v.y,&v.z);
		m_arrInflateVecs.Add(v);
	}

	InFile.getline(buf,MAX_LEN);		//消除一个换行

	return true;
}

bool C2DObstruct::SaveToStream(std::ofstream& OutFile) const
{
	using namespace std;

	if(!CConvexHullData::SaveToStream(OutFile))
		return false;

	if(!OutFile.is_open())
		return false;
	
	//换行及标志
	OutFile<<"\n";
	OutFile<<"Inflate Vectors:\n";

	for(int i=0;i<m_arrInflateVecs.GetSize();i++)
		OutFile<<"  "<<m_arrInflateVecs[i].x<<"  "<<m_arrInflateVecs[i].y<<"  "<<m_arrInflateVecs[i].z<<endl;

	OutFile<<"\n";						//换行

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

	//再写入膨胀向量
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
	
	//读出膨胀向量
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
	//调用父类方法
	CConvexHullData::Transform(mtxTrans);

	//对膨胀向量进行变换
	//由于膨胀向量仅由面片的法向决定，因此只需用变换中的旋转分量对其进行变换即可
	A3DMATRIX4 mtxRotate(mtxTrans);
	mtxRotate.SetRow(3,A3DVECTOR3(0.0f,0.0f,0.0f));
	float fScale=mtxTrans.GetCol(0).Magnitude();
	mtxRotate/=fScale;

	for(int i=0;i<m_arrInflateVecs.GetSize();i++)
		m_arrInflateVecs[i]=m_arrInflateVecs[i]*mtxRotate;


}

}	// end namespace

