               // ConvexHullData.cpp: implementation of the CConvexHullData class.
//
//////////////////////////////////////////////////////////////////////

#include "ConvexHullData.h"
#include <A3DGDI.h>
#include <A3DFlatCollector.h>
#include <A3DTypes.h>
#include <AFile.h>
#include <A3DCollision.h>

#include "CDWithCH.h"

// #include <fstream>

#define FILE_HEAD_TAG	"==========Convex_Hull_Data_File=========="

// 文件输出的版本控制！字符串的长度必须相同！
// old versions
#define CHDATA_HEAD_TAG_V00	"==========Convex_Hull_Data_Start========="
#define CHDATA_HEAD_TAG_V01	"V0.1======Convex_Hull_Data_Start========="

// current version
#define CHDATA_HEAD_TAG	       "V0.2======Convex_Hull_Data_Start========="


static const int IntSize=sizeof(int);
static const int FloatSize=sizeof(float);

namespace CHBasedCD
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConvexHullData::CConvexHullData()
{
	m_dwFlags=0;
}

CConvexHullData::~CConvexHullData()
{
	for(int i=0;i<m_arrFaces.GetSize();i++)
		delete m_arrFaces[i];
}

/************************************************************************
	
// Removed by wenfeng, 05-3-28
// 避免引擎中出现std::fstream的库，由于其需要VC的支持！

bool CConvexHullData::LoadFromFile(const char* szFileName)
{
	using namespace std;
	ifstream InFile(szFileName);
	if(!InFile)
		return false;

	//读FILE_HEAD_TAG
	char buf[MAX_LEN];
	InFile.getline(buf,MAX_LEN);
	if(strcmp(buf,FILE_HEAD_TAG))		//确认文件头标志
		return false;
	InFile.getline(buf,MAX_LEN);		//消除一个换行

	return LoadFromStream(InFile);
}

bool CConvexHullData::SaveToFile(const char* szFileName) const
{
	using namespace std;

	ofstream OutFile(szFileName);
	if(!OutFile)
		return false;
	
	//这里要进行简单的格式控制
	//文件头
	OutFile<<FILE_HEAD_TAG<<endl;
	OutFile<<endl;					//加一个换行
	
	return SaveToStream(OutFile);

	//析构时自动调用
	//OutFile.close();
}



bool CConvexHullData::LoadFromStream(std::ifstream& InFile)
{
	if(!InFile.is_open())
		return false;
	
	char buf[MAX_LEN];
	InFile.getline(buf,MAX_LEN);
	if(strcmp(buf,CHDATA_HEAD_TAG))	//确认数据头标志
		return false;
	
	//开始读顶点
	InFile.getline(buf,MAX_LEN);		//读出一行字符串
	int vNum;
	sscanf(buf,"Vertex Number: %d",&vNum);
	for(int i=0;i<vNum;i++)
	{
		InFile.getline(buf,MAX_LEN);		//读出一行字符串
		A3DVECTOR3 v;
		sscanf(buf,"%f %f %f",&v.x,&v.y,&v.z);
		m_arrVertices.Add(v);
	}
	
	InFile.getline(buf,MAX_LEN);		//消除一个换行

	//开始读取面信息
	InFile.getline(buf,MAX_LEN);		//读出一行字符串
	int fNum;
	sscanf(buf,"Face Number: %d",&fNum);
	for(i=0;i<fNum;i++)
	{
		CFace *pFace = new CFace;
		pFace->LoadFromStream(InFile);
		m_arrFaces.Add(pFace);
	}
	
	return true;
}

bool CConvexHullData::SaveToStream(std::ofstream& OutFile) const
{
	using namespace std;

	if(!OutFile.is_open())
		return false;

	OutFile<<CHDATA_HEAD_TAG<<endl;		//数据头开始标志
	//输出顶点
	OutFile<<"Vertex Number: "<< GetVertexNum()<<endl;
	for(int i=0;i<GetVertexNum();i++)
		OutFile<<"  "<<m_arrVertices[i].x<<"  "<<m_arrVertices[i].y<<"  "<<m_arrVertices[i].z<<endl;
	
	//换行
	OutFile<<endl;
	
	//输出Faces
	OutFile<<"Face Number: "<<GetFaceNum()<<endl;
	for(i=0;i<GetFaceNum();i++)
		m_arrFaces[i]->SaveToStream(OutFile);

	return true;
}

/************************************************************************/

bool CConvexHullData::LoadFromStream(FILE* InFile)
{
	if(!InFile)	return false;
	
	char buf[MAX_LEN];
	fscanf(InFile,"%s\n",buf);
	if(strcmp(buf,CHDATA_HEAD_TAG))	//确认数据头标志
		return false;
	
	//开始读顶点
	int vNum;
	fscanf(InFile,"Vertex Number: %d\n",&vNum);
	int i(0);
	for(i=0;i<vNum;i++)
	{
		A3DVECTOR3 v;
		fscanf(InFile,"%f %f %f\n",&v.x,&v.y,&v.z);
		m_arrVertices.Add(v);
	}
	
	//消除一个换行
	fscanf(InFile,"\n");

	//开始读取面信息
	int fNum;
	fscanf(InFile,"Face Number: %d\n",&fNum);
	for(i=0;i<fNum;i++)
	{
		CFace *pFace = new CFace;
		pFace->LoadFromStream(InFile);
		m_arrFaces.Add(pFace);
	}
	
	return true;
}

bool CConvexHullData::SaveToStream(FILE* OutFile) const
{
	if(!OutFile) return false;

	fprintf(OutFile,CHDATA_HEAD_TAG);		//数据头开始标志
	fprintf(OutFile,"\n");					//换行
	
	//输出顶点
	fprintf(OutFile,"Vertex Number: %d\n",GetVertexNum());
	int i(0);
	for(i=0;i<GetVertexNum();i++)
		fprintf(OutFile,"  %f  %f  %f\n",m_arrVertices[i].x,m_arrVertices[i].y,m_arrVertices[i].z);
	
	//换行
	fprintf(OutFile,"\n");
	
	//输出Faces
	fprintf(OutFile,"Face Number: %d\n",GetFaceNum());
	for(i=0;i<GetFaceNum();i++)
		m_arrFaces[i]->SaveToStream(OutFile);
	
	return true;
}

void CConvexHullData::Render(A3DFlatCollector* pFC,  bool bRenderV,CFace* pSpecialFace, DWORD dwVColor,DWORD dwFColor, const A3DVECTOR3& vRayDir, const A3DMATRIX4* pTransMatrix)
{
	if(!pFC) return;

	if( m_arrVertices.GetSize() == 0 ) return;

	//绘制顶点
	if(bRenderV)
	{
		A3DVECTOR3 v;
		for(int i=0;i<m_arrVertices.GetSize();i++)
		{
			if(pTransMatrix)
				v=m_arrVertices[i]*(*pTransMatrix);
			else
				v=m_arrVertices[i];
			g_pA3DGDI->Draw3DPoint(v,dwVColor,3.0f);
		}
	}

	//绘制面片
	A3DVECTOR3 RayDir;
	//保留面片色彩dwFColor的alpha值
	DWORD ALPHA=dwFColor & 0xff000000;

	Normalize(vRayDir,RayDir);

	for(int i=0;i<m_arrFaces.GetSize();i++)
	{
		int VNum=m_arrFaces[i]->GetVNum();
		A3DVECTOR3* Vs=new A3DVECTOR3[VNum];
		int IDNum=(VNum-2)*3;
		WORD* IDs=new WORD[IDNum];
		int cur=0;
		//构造顶点和三角形索引
		for(int j=0;j<VNum;j++)
		{
			if(pTransMatrix)
				Vs[j]=m_arrVertices[m_arrFaces[i]->GetVID(j)]*(*pTransMatrix);
			else
				Vs[j]=m_arrVertices[m_arrFaces[i]->GetVID(j)];
			if(j<VNum-2)
			{
				IDs[cur]=0;
				IDs[cur+1]=j+1;
				IDs[cur+2]=j+2;
				cur+=3;
			}
		}
		
		//利用光照计算颜色
		A3DCOLORVALUE faceColorV(dwFColor);
		//双向光源，为了区分不同的面片！by wenfeng
		//float fWeight=float(fabs(DotProduct(m_arrFaces[i]->GetNormal(),RayDir)*0.8f));
		// 改为单向光源。  By kuiwu. [21/3/2006]
		float fdot =DotProduct(m_arrFaces[i]->GetNormal(),RayDir);
		float fWeight = a_Max(0.0f, fdot) * 0.8f;

		faceColorV*=0.2f+fWeight;
		DWORD faceColor;
		if(pSpecialFace==m_arrFaces[i])
		{
			faceColor=faceColorV.ToRGBAColor();
			faceColor=0xffffffff-faceColor;
			faceColor=faceColor & 0x00ffffff  | ALPHA;
		}
		else
			faceColor=faceColorV.ToRGBAColor() & 0x00ffffff  | ALPHA;

		//绘制
		pFC->AddRenderData_3D(Vs,VNum,IDs,IDNum,faceColor);

		delete [] IDs;
		delete [] Vs;
	}
}

bool CConvexHullData::LoadBinaryData(AFile* InFile, bool bLoadVertices)
{
	if(!InFile || !InFile->IsBinary())
		return false;
	
	//读出标志，从而检验！
	char *szCHDataFlag=CHDATA_HEAD_TAG;
	char szRead[60];
	memset(szRead,0,60);
	int iFlagLen=strlen(szCHDataFlag);
	DWORD RealLen;
	InFile->Read(szRead,iFlagLen,&RealLen);


	
	//判断标志
	if(!strcmp(szRead,szCHDataFlag))	
	{
		//为最新版本：CHDATA_HEAD_TAG，版本2
		m_iVer=2;
	}
	else
	{
		szCHDataFlag=CHDATA_HEAD_TAG_V01;		
		if(!strcmp(szRead,szCHDataFlag))
		{
			//为版本：CHDATA_HEAD_TAG_V01
			m_iVer=1;
		}
		else
		{
			szCHDataFlag=CHDATA_HEAD_TAG_V00;		
			if(!strcmp(szRead,szCHDataFlag))
			{
				//为版本：CHDATA_HEAD_TAG_V00
				m_iVer=0;
			}
			else
				m_iVer=-1;
		}
	}

	switch(m_iVer) {

	case 2:

	case 1:
		//读出标志位
		InFile->Read(&m_dwFlags,sizeof(int),&RealLen);
		break;

	case 0:
		m_dwFlags=0;
		break;

	case -1:
		return false;
		break;
	}
		
	//读出缓冲区大小
	int BufSize;
	InFile->Read(&BufSize,sizeof(int),&RealLen);
	
	//开始读出数据
	char * buf= new char[BufSize];
	InFile->Read(buf,BufSize,&RealLen);
	if((int)RealLen!=BufSize)
	{
		delete [] buf;
		return false;
	}
	
	ReadFromBuf(buf);
	delete [] buf;

	// build the convex hull's aabb
	BuildAABB();
	
	// clear the vertices to save memory!
	if(!bLoadVertices)
		m_arrVertices.RemoveAll();

	return true;
}

bool CConvexHullData::SaveBinaryData(AFile* OutFile) const
{
	if(!OutFile || !OutFile->IsBinary())
		return false;

	//写标志，从而检验！
	char szCHDataFlag[]=CHDATA_HEAD_TAG;
	DWORD RealLen;
	OutFile->Write(szCHDataFlag,strlen(szCHDataFlag),&RealLen);
	
	//-----------------------------------
	// 写入额外的数据，为了扩展性的考虑
	//-----------------------------------
	OutFile->Write((void *)&m_dwFlags,sizeof(int),&RealLen);		//写入标志信息
	
	//----------------------------------
	// 开始写入基本的数据
	//----------------------------------
	
	//计算缓冲区大小并写入文件
	int BufSize=ComputeBufSize();
	OutFile->Write(&BufSize,sizeof(int),&RealLen);
	
	//开始写入数据
	char* buf=new char[BufSize];
	if(!WriteToBuf(buf)) return false;
	
	//将buf写入OutFile
	OutFile->Write(buf,BufSize,&RealLen);
	delete [] buf;

	if((int)RealLen!=BufSize)
		return false;
	
	return true;
}

int CConvexHullData::ComputeBufSize() const
{

	int BufSize=0;
	
	BufSize+=IntSize;		//顶点数量
	BufSize+=3*FloatSize*m_arrVertices.GetSize();	//顶点的坐标

	BufSize+=IntSize;		//面片数量
	for(int i=0;i<m_arrFaces.GetSize();i++)
	{
		BufSize+=4*FloatSize;		//面片对应的平面方程参数
		BufSize+=IntSize;				//面片所含的顶点数量
		BufSize+=(IntSize+4*FloatSize)*m_arrFaces[i]->GetVNum();

		BufSize+=IntSize;				// 额外的面片数量
		BufSize+=4*FloatSize*m_arrFaces[i]->GetExtraHSNum();	// 额外的面片信息
	}	

	return BufSize;
}

bool CConvexHullData::WriteToBuf(char* buf) const
{
	if(!buf) return false;
	
	int iWrite;
	int cur=0;
	float fWrite;
	
	//写入顶点数量
	iWrite=m_arrVertices.GetSize();
	//memcpy(buf+cur,&iWrite,IntSize);
	* (int* ) (buf+cur)=iWrite;
	cur+=IntSize;

	//写入顶点坐标信息
	int i(0);
	for(i=0;i<m_arrVertices.GetSize();i++)
	{
		fWrite=m_arrVertices[i].x;
		//memcpy(buf+cur,&fWrite,FloatSize);
		* (float* ) (buf+cur)=fWrite;
		cur+=FloatSize;

		fWrite=m_arrVertices[i].y;
		//memcpy(buf+cur,&fWrite,FloatSize);
		* (float* ) (buf+cur)=fWrite;
		cur+=FloatSize;

		fWrite=m_arrVertices[i].z;
		//memcpy(buf+cur,&fWrite,FloatSize);
		* (float* ) (buf+cur)=fWrite;
		cur+=FloatSize;
	}
	
	//写入面片数量
	iWrite=m_arrFaces.GetSize();
	//memcpy(buf+cur,&iWrite,IntSize);
	* (int* ) (buf+cur)=iWrite;
	cur+=IntSize;
	//写入面片信息
	for(i=0;i<m_arrFaces.GetSize();i++)
	{
		//写入面片的平面方程参数
		fWrite=m_arrFaces[i]->GetNormal().x;
		//memcpy(buf+cur,&fWrite,FloatSize);
		* (float* ) (buf+cur)=fWrite;
		cur+=FloatSize;

		fWrite=m_arrFaces[i]->GetNormal().y;
		//memcpy(buf+cur,&fWrite,FloatSize);
		* (float* ) (buf+cur)=fWrite;
		cur+=FloatSize;
		fWrite=m_arrFaces[i]->GetNormal().z;
		//memcpy(buf+cur,&fWrite,FloatSize);
		* (float* ) (buf+cur)=fWrite;
		cur+=FloatSize;

		fWrite=m_arrFaces[i]->GetDist();
		//memcpy(buf+cur,&fWrite,FloatSize);
		* (float* ) (buf+cur)=fWrite;
		cur+=FloatSize;

		//写入该面片所含的顶点数量
		iWrite=m_arrFaces[i]->GetVNum();
		//memcpy(buf+cur,&iWrite,IntSize);
		* (int* ) (buf+cur)=iWrite;
		cur+=IntSize;

		//再写入面片的顶点和边平面信息
		int j(0);
		for(j=0;j<m_arrFaces[i]->GetVNum();j++)
		{
			//写入顶点id
			iWrite=m_arrFaces[i]->GetVID(j);
			//memcpy(buf+cur,&iWrite,IntSize);
			* (int* ) (buf+cur)=iWrite;
			cur+=IntSize;
			
			//写入边平面的平面方程参数
			fWrite=m_arrFaces[i]->GetEdgeHalfSpace(j).GetNormal().x;
			//memcpy(buf+cur,&fWrite,FloatSize);
			* (float* ) (buf+cur)=fWrite;
			cur+=FloatSize;

			fWrite=m_arrFaces[i]->GetEdgeHalfSpace(j).GetNormal().y;
			//memcpy(buf+cur,&fWrite,FloatSize);
			* (float* ) (buf+cur)=fWrite;
			cur+=FloatSize;
			
			fWrite=m_arrFaces[i]->GetEdgeHalfSpace(j).GetNormal().z;
			//memcpy(buf+cur,&fWrite,FloatSize);
			* (float* ) (buf+cur)=fWrite;
			cur+=FloatSize;

			fWrite=m_arrFaces[i]->GetEdgeHalfSpace(j).GetDist();
			//memcpy(buf+cur,&fWrite,FloatSize);
			* (float* ) (buf+cur)=fWrite;
			cur+=FloatSize;
		}

		// 写入当前面片的额外面片数量和信息
		iWrite=m_arrFaces[i]->GetExtraHSNum();
		//memcpy(buf+cur,&iWrite,IntSize);
		* (int* ) (buf+cur)=iWrite;
		cur+=IntSize;
		
		for(j=0;j<m_arrFaces[i]->GetExtraHSNum();j++)
		{
			fWrite=m_arrFaces[i]->GetExtraHalfSpace(j).GetNormal().x;
			//memcpy(buf+cur,&fWrite,FloatSize);
			* (float* ) (buf+cur)=fWrite;
			cur+=FloatSize;

			fWrite=m_arrFaces[i]->GetExtraHalfSpace(j).GetNormal().y;
			//memcpy(buf+cur,&fWrite,FloatSize);
			* (float* ) (buf+cur)=fWrite;
			cur+=FloatSize;
			
			fWrite=m_arrFaces[i]->GetExtraHalfSpace(j).GetNormal().z;
			//memcpy(buf+cur,&fWrite,FloatSize);
			* (float* ) (buf+cur)=fWrite;
			cur+=FloatSize;

			fWrite=m_arrFaces[i]->GetExtraHalfSpace(j).GetDist();
			//memcpy(buf+cur,&fWrite,FloatSize);
			* (float* ) (buf+cur)=fWrite;
			cur+=FloatSize;
		}

	}
	return true;
}

bool CConvexHullData::ReadFromBuf(char* buf)
{
	if(!buf) return false;

	int iRead;
	int cur=0;
	float fRead;
	
	//读出顶点数量
	//memcpy(&iRead,buf+cur,IntSize);
	iRead=* (int *) (buf+cur);
	cur+=IntSize;
	int vNum=iRead;

	//读出顶点坐标信息
	int i(0);
	for(i=0;i<vNum;i++)
	{
		A3DVECTOR3 v;
		//memcpy(&fRead,buf+cur,FloatSize);
		fRead=* (float *) (buf+cur);
		cur+=FloatSize;
		v.x=fRead;

		//memcpy(&fRead,buf+cur,FloatSize);
		fRead=* (float *) (buf+cur);
		cur+=FloatSize;
		v.y=fRead;

		//memcpy(&fRead,buf+cur,FloatSize);
		fRead=* (float *) (buf+cur);
		cur+=FloatSize;
		v.z=fRead;

		m_arrVertices.Add(v);
	}

	//读出面片数量
	//memcpy(&iRead,buf+cur,IntSize);
	iRead=* (int *) (buf+cur);
	cur+=IntSize;
	int fNum=iRead;
	for(i=0;i<fNum;i++)
	{
		CFace *pFace = new CFace;
		
		//读出面片的平面方程参数
		A3DVECTOR3 n;
		float d;
		//memcpy(&fRead,buf+cur,FloatSize);
		fRead=* (float *) (buf+cur);
		cur+=FloatSize;
		n.x=fRead;

		//memcpy(&fRead,buf+cur,FloatSize);
		fRead=* (float *) (buf+cur);
		cur+=FloatSize;
		n.y=fRead;

		//memcpy(&fRead,buf+cur,FloatSize);
		fRead=* (float *) (buf+cur);
		cur+=FloatSize;
		n.z=fRead;

		//memcpy(&fRead,buf+cur,FloatSize);
		fRead=* (float *) (buf+cur);
		cur+=FloatSize;
		d=fRead;

		pFace->SetNormal(n);
		pFace->SetD(d);

		//读出顶点的数量
		//memcpy(&iRead,buf+cur,IntSize);
		iRead=* (int *) (buf+cur);
		cur+=IntSize;
		int fvNum=iRead;
		
		int j(0);
		for(j=0;j<fvNum;j++)
		{
			int id;
			CHalfSpace hs;
			A3DVECTOR3 n;
			float d;
			
			//读出顶点id
			//memcpy(&iRead,buf+cur,IntSize);
			iRead=* (int *) (buf+cur);
			cur+=IntSize;
			id=iRead;
			
			//读出相应的边平面方程
			//memcpy(&fRead,buf+cur,FloatSize);
			fRead=* (float *) (buf+cur);
			cur+=FloatSize;
			n.x=fRead;

			//memcpy(&fRead,buf+cur,FloatSize);
			fRead=* (float *) (buf+cur);
			cur+=FloatSize;
			n.y=fRead;

			//memcpy(&fRead,buf+cur,FloatSize);
			fRead=* (float *) (buf+cur);
			cur+=FloatSize;
			n.z=fRead;

			//memcpy(&fRead,buf+cur,FloatSize);
			fRead=* (float *) (buf+cur);
			cur+=FloatSize;
			d=fRead;

			hs.SetNormal(n);
			hs.SetD(d);
			
			pFace->AddElement(id,hs);
		}
		
		if(m_iVer>1)
		{
			// 版本2以上的，将包含额外面片信息
			
			// 读出额外面片的数量
			iRead=* (int *) (buf+cur);
			cur+=IntSize;
			int iExtraFaceNum=iRead;
			
			// 读出额外面片信息
			for(j=0; j<iExtraFaceNum; j++)
			{
				CHalfSpace hs;
				A3DVECTOR3 n;
				float d;
				
				//读出相应的边平面方程
				//memcpy(&fRead,buf+cur,FloatSize);
				fRead=* (float *) (buf+cur);
				cur+=FloatSize;
				n.x=fRead;

				//memcpy(&fRead,buf+cur,FloatSize);
				fRead=* (float *) (buf+cur);
				cur+=FloatSize;
				n.y=fRead;

				//memcpy(&fRead,buf+cur,FloatSize);
				fRead=* (float *) (buf+cur);
				cur+=FloatSize;
				n.z=fRead;

				//memcpy(&fRead,buf+cur,FloatSize);
				fRead=* (float *) (buf+cur);
				cur+=FloatSize;
				d=fRead;

				hs.SetNormal(n);
				hs.SetD(d);
				
				pFace->AddExtraHS(hs);
			}
			
		}

		m_arrFaces.Add(pFace);
	}

	return true;
}

void CConvexHullData::Transform(const A3DMATRIX4& mtxTrans)
{
	// 变换顶点
	int i(0);
	for(i=0;i<m_arrVertices.GetSize();i++)
	{
		m_arrVertices[i]=m_arrVertices[i]*mtxTrans;
	}

	// 变换面片
	for(i=0;i<m_arrFaces.GetSize();i++)
	{
		m_arrFaces[i]->Transform(mtxTrans);
	}

	// after transformation, we should rebuild the convex hull's aabb.
	BuildAABB();
}

CConvexHullData::CConvexHullData(const CConvexHullData& CHData)
{
	int i(0);
	for(i=0;i<CHData.GetVertexNum();i++)
		AddVertex(CHData.m_arrVertices[i]);
	for(i=0;i<CHData.GetFaceNum();i++)
		AddFace(CHData.m_arrFaces[i]);
	
	m_dwFlags=CHData.m_dwFlags;
}

const CConvexHullData& CConvexHullData::operator=(const CConvexHullData& CHData)
{
	Reset();
	int i(0);
	for(i=0;i<CHData.GetVertexNum();i++)
		AddVertex(CHData.m_arrVertices[i]);
	for(i=0;i<CHData.GetFaceNum();i++)
		AddFace(CHData.m_arrFaces[i]);
	
	m_dwFlags=CHData.m_dwFlags;

	return (*this);
}

void CConvexHullData::ComputeFaceExtraHS()
{
	for(int i=0; i< m_arrFaces.GetSize(); i++)
		m_arrFaces[i]->ComputeExtraHS();
}

void CConvexHullData::Import(const A3DOBB* pOBB, bool bImportVertices)
{
	if(!pOBB) return ;
	
	Reset();

	A3DVECTOR3 Vertices[8];
	WORD wIndices[24];
	pOBB->GetVertices(Vertices, wIndices, true);
	
	// Note: the order of the Vertices is (x,y,z)
	// (-, +, +) (+, +, +) (+,+,-) (-, +, -) (-, -, +) (+, -, +) (+,-,-) (-, -, -)

	if(bImportVertices)
	{
		for( int i =0; i<8; ++i)
			AddVertex(Vertices[i]);

	}

	CHalfSpace hsXPos, hsXNeg;
	CHalfSpace hsYPos, hsYNeg;
	CHalfSpace hsZPos, hsZNeg;
	
	hsXPos.SetNV(pOBB->XAxis, pOBB->Center + pOBB->ExtX);
	hsXNeg.SetNV(-pOBB->XAxis, pOBB->Center - pOBB->ExtX);
	hsYPos.SetNV(pOBB->YAxis, pOBB->Center + pOBB->ExtY);
	hsYNeg.SetNV(-pOBB->YAxis, pOBB->Center - pOBB->ExtY);
	hsZPos.SetNV(pOBB->ZAxis, pOBB->Center + pOBB->ExtZ);
	hsZNeg.SetNV(-pOBB->ZAxis, pOBB->Center - pOBB->ExtZ);

	CFace *pFace;
	
	// 依次添加6个面

	// positive-X face
	pFace= new CFace;
	pFace->SetCHData(this);
	pFace->SetHS(hsXPos);
	pFace->AddElement(2, hsYPos);
	pFace->AddElement(1, hsZPos);
	pFace->AddElement(5, hsYNeg);
	pFace->AddElement(6, hsZNeg);
	m_arrFaces.Add(pFace);

	// negative-X face
	pFace= new CFace;
	pFace->SetCHData(this);
	pFace->SetHS(hsXNeg);
	pFace->AddElement(0, hsYPos);
	pFace->AddElement(3, hsZNeg);
	pFace->AddElement(7, hsYNeg);
	pFace->AddElement(4, hsZPos);
	m_arrFaces.Add(pFace);

	// positive-Y face
	pFace= new CFace;
	pFace->SetCHData(this);
	pFace->SetHS(hsYPos);
	pFace->AddElement(0, hsZPos);
	pFace->AddElement(1, hsXPos);
	pFace->AddElement(2, hsZNeg);
	pFace->AddElement(3, hsXNeg);
	m_arrFaces.Add(pFace);

	// negative-Y face
	pFace= new CFace;
	pFace->SetCHData(this);
	pFace->SetHS(hsYNeg);
	pFace->AddElement(6, hsXPos);
	pFace->AddElement(5, hsZPos);
	pFace->AddElement(4, hsXNeg);
	pFace->AddElement(7, hsZNeg);
	m_arrFaces.Add(pFace);

	// positive-Z face
	pFace= new CFace;
	pFace->SetCHData(this);
	pFace->SetHS(hsZPos);
	pFace->AddElement(1, hsYPos);
	pFace->AddElement(0, hsXNeg);
	pFace->AddElement(4, hsYNeg);
	pFace->AddElement(5, hsXPos);
	m_arrFaces.Add(pFace);

	// negative-Z face
	pFace= new CFace;
	pFace->SetCHData(this);
	pFace->SetHS(hsZNeg);
	pFace->AddElement(3, hsYPos);
	pFace->AddElement(2, hsXPos);
	pFace->AddElement(6, hsYNeg);
	pFace->AddElement(7, hsXNeg);
	m_arrFaces.Add(pFace);

}

bool CConvexHullData::Import(const A3DVECTOR3 Vertices[3], float fThickness)
{
	if(!Vertices) return false;
	
	A3DVECTOR3 e01(Vertices[1]-Vertices[0]),e12(Vertices[2]-Vertices[1]),e20(Vertices[0]-Vertices[2]);

	A3DVECTOR3 vNTop = CrossProduct(e01,e20);
	float fDNTop = vNTop.Normalize();
	
	// Collinear test
	if(fDNTop < 1e-5) return false;		
	
	Reset();

	// Compute all 6 vertices of convex hull;
	A3DVECTOR3 v[6];
	float fHalfThickness = 0.5f * fThickness;
	// top 3 vertices
	v[0] = Vertices[0] + fHalfThickness * vNTop;
	v[1] = Vertices[1] + fHalfThickness * vNTop;
	v[2] = Vertices[2] + fHalfThickness * vNTop;
	
	// bottom 3 vertices
	v[3] = Vertices[0] - fHalfThickness * vNTop;
	v[4] = Vertices[1] - fHalfThickness * vNTop;
	v[5] = Vertices[2] - fHalfThickness * vNTop;

	// Add all vertices
	for( int i =0; i<6; ++i)
		AddVertex(v[i]);

	// Compute the top and the bottom halfspaces
	float fDTop = DotProduct(Vertices[0],vNTop);
	CHalfSpace hsTop;
	hsTop.SetNormal(vNTop);
	hsTop.SetD(fDTop);
	CHalfSpace hsBottom(hsTop);
	hsBottom.Inverse();

	// Translate these two halfspaces
	hsTop.Translate(fHalfThickness);
	hsBottom.Translate(fHalfThickness);

	
	// Compute the left 3 side halfspaces
	CHalfSpace hs01,hs12,hs20;
	hs01.SetNV(CrossProduct(vNTop,e01),Vertices[0]);
	hs12.SetNV(CrossProduct(vNTop,e12),Vertices[1]);
	hs20.SetNV(CrossProduct(vNTop,e20),Vertices[2]);
	
	// Add all faces
	// ******** Noted by wenfeng, 05-10-10
	// For now, considering speed,the side-halfspace's edge-halfspace is not correct!
	// But it will work for CCDBrush. Fix or let it be???

	// Top face
	CFace* pFace;

	pFace= new CFace;
	pFace->SetCHData(this);
	pFace->SetHS(hsTop);
	pFace->AddElement(0, hs20);
	pFace->AddElement(2, hs12);
	pFace->AddElement(1, hs01);
	m_arrFaces.Add(pFace);

	// Bottom face
	pFace= new CFace;
	pFace->SetCHData(this);
	pFace->SetHS(hsBottom);
	pFace->AddElement(3, hs01);
	pFace->AddElement(4, hs12);
	pFace->AddElement(5, hs20);
	m_arrFaces.Add(pFace);

	// Side face 01
	pFace= new CFace;
	pFace->SetCHData(this);
	pFace->SetHS(hs01);
	pFace->AddElement(0, hsTop);
	pFace->AddElement(1, hs12);				// Note: not correct here!!!
	pFace->AddElement(4, hsBottom);
	pFace->AddElement(3, hs20);				// Note: not correct here!!!
	m_arrFaces.Add(pFace);

	// Side face 12
	pFace= new CFace;
	pFace->SetCHData(this);
	pFace->SetHS(hs12);
	pFace->AddElement(1, hsTop);
	pFace->AddElement(2, hs20);				// Note: not correct here!!!
	pFace->AddElement(5, hsBottom);
	pFace->AddElement(4, hs01);				// Note: not correct here!!!
	m_arrFaces.Add(pFace);

	// Side face 20
	pFace= new CFace;
	pFace->SetCHData(this);
	pFace->SetHS(hs20);
	pFace->AddElement(2, hsTop);
	pFace->AddElement(0, hs01);				// Note: not correct here!!!
	pFace->AddElement(3, hsBottom);
	pFace->AddElement(5, hs12);				// Note: not correct here!!!
	m_arrFaces.Add(pFace);

	return true;
}

int CConvexHullData::ConvexHullOverlapTest(const CConvexHullData& AnotherCH) const
{
	//	AABB check at first
	//	Add by dyx 2006.11.30
	A3DAABB aabbOther = AnotherCH.GetAABB();
	if (!CLS_AABBAABBOverlap(m_aabb.Center, m_aabb.Extents, aabbOther.Center, aabbOther.Extents))
		return 0;

	bool bCurVOutside = false, bLastVOutside;

	int i(0);
	for(i=0; i<GetVertexNum(); ++i)
	{
		bLastVOutside = bCurVOutside;
		bCurVOutside = IsVertexOutsideCH( m_arrVertices[i], AnotherCH);

		if( i>0 && bCurVOutside != bLastVOutside )
			return 1;		// some vertices are inside AnotherCH while others are outside
	}

	if( bCurVOutside )
	{
		// all vertices of mine are outside AnotherCH
		for(i=0; i<AnotherCH.GetVertexNum(); ++i)
		{
			bLastVOutside = bCurVOutside;
			bCurVOutside = IsVertexOutsideCH(AnotherCH.m_arrVertices[i], *this);

			if( i>0 && bCurVOutside != bLastVOutside )
				return 1;		// some vertices are inside AnotherCH while others are outside
		}

		if( bCurVOutside)
		{
			// further test whether we will intersect each other
			A3DVECTOR3 vStart, vDelta, vCollidePos;
			const CFace* pCDFace = NULL;
			float fraction;

			// test whether each edge of mine will intersect AnotherCH!
			for( i =0; i<GetFaceNum(); i++)
			{
				for(int j=0; j< m_arrFaces[i]->GetVNum(); j++)
				{
					vStart = m_arrVertices[ m_arrFaces[i]->GetVID(j) ];
					if(j == m_arrFaces[i]->GetVNum()-1)
						vDelta = m_arrVertices[ m_arrFaces[i]->GetVID(0) ];
					else
						vDelta = m_arrVertices[ m_arrFaces[i]->GetVID(j+1) ];

					vDelta -= vStart;
					

					if(RayIntersectWithCH(vStart, vDelta, AnotherCH, &pCDFace, vCollidePos,fraction))
						return 1;
				}
			}

			// test whether each edge of AnotherCH will intersect me!
			for( i =0; i<AnotherCH.GetFaceNum(); i++)
			{
				for(int j=0; j< AnotherCH.m_arrFaces[i]->GetVNum(); j++)
				{
					vStart = AnotherCH.m_arrVertices[ AnotherCH.m_arrFaces[i]->GetVID(j) ];
					if(j == AnotherCH.m_arrFaces[i]->GetVNum()-1)
						vDelta = AnotherCH.m_arrVertices[ AnotherCH.m_arrFaces[i]->GetVID(0) ];
					else
						vDelta = AnotherCH.m_arrVertices[ AnotherCH.m_arrFaces[i]->GetVID(j+1) ];

					vDelta -= vStart;
					

					if(RayIntersectWithCH(vStart, vDelta, *this, &pCDFace, vCollidePos,fraction))
						return 1;
				}
			}

			return 0;

		}
		else
			return 3;		// all vertices of AnotherCH are inside me, so we return 3

	}
	else
		return 2;		// all vertices of mine are inside AnotherCH, so we return 2
}

void CConvexHullData::BuildAABB()
{
	m_aabb.Build( m_arrVertices.GetData(), m_arrVertices.GetSize());	
}

}	// end namespace