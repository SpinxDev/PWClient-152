// TerrainObstructGenerator.cpp: implementation of the CTerrainObstructGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TerrainObstructGenerator.h"
#include "EDTmpl.h"
#include "Matrix.h"

#define IMAGE_PIXEL(x,y)	m_pHeightMapBuf[(y)*m_nWidth+x]
#define RESULT_PIXEL(x,y)	m_pResultBuf[(y)*m_nWidth+x]

#define IMAGE_WIDTH m_nWidth
#define IMAGE_HEIGHT m_nHeight


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Some global functions
//////////////////////////////////////////////////////////////////////
void BubbleSort(int *Nums,int num)
{

	for(int i=0;i<num;i++)
	{
		int curMax=Nums[i];
		int Maxid=i;
		for(int j=i+1;j<num;j++)
		{
			if(Nums[j]>curMax)
			{
				Maxid=j;
				curMax=Nums[j];
			}
		}

		//交换
		Nums[Maxid]=Nums[i];
		Nums[i]=curMax;
	}	
}

// 判断两象素是否在阈值内邻接
bool IsAdjacent(CPoint pt1,CPoint pt2, int DistThresh=1)
{
	if(abs(pt1.x-pt2.x)<=DistThresh && abs(pt1.y-pt2.y)<=DistThresh )
		return true;
	else
		return false;	
}

// 合并轮廓。 若Outline与MergedOutlines中的一条轮廓邻接，则合并，否则直接添加。
void MergeOutline(vector< vector<CPoint> > & MergedOutlines, vector<CPoint>& Outline,int DistThresh=3)
{
	if(Outline.size()==0) return;

	for(int i=0;i<MergedOutlines.size();i++)
	{
		//根据不同的邻接情况，进行相应的插入操作
		if(IsAdjacent(MergedOutlines[i].back(),Outline.front(),DistThresh))
		{
			
			MergedOutlines[i].insert(MergedOutlines[i].end(),Outline.begin(),Outline.end());
			return;
		}
		if(IsAdjacent(MergedOutlines[i].back(),Outline.back(),DistThresh))
		{
			//对Outline中的元素从后向前插入到MergedOutlines[i]中
			vector<CPoint>::reverse_iterator ri=Outline.rbegin();
			while(ri!=Outline.rend())
			{
				MergedOutlines[i].insert(MergedOutlines[i].end(),*ri);
				ri++;
			}
			return;
		}
		
		if(IsAdjacent(MergedOutlines[i].front(),Outline.front(),DistThresh))
		{
			//两个轮廓的头相接，此时的处理比较麻烦
			
			//构造一个新的轮廓，并依次向其中添加
			vector<CPoint> newOutline;		
			for(vector<CPoint>::reverse_iterator ri=Outline.rbegin();ri!=Outline.rend();ri++)
				newOutline.push_back((*ri));
			for(vector<CPoint>::iterator it=MergedOutlines[i].begin();it!=MergedOutlines[i].end();it++)
				newOutline.push_back((*it));

			MergedOutlines[i]=newOutline;
			return;
		}
		
		if(IsAdjacent(MergedOutlines[i].front(),Outline.back(),DistThresh))
		{
			Outline.insert(Outline.end(),MergedOutlines[i].begin(),MergedOutlines[i].end());
			MergedOutlines[i]=Outline;
			return;
		}
	}

	//如果没有任何邻接的情况，直接插入到MergedOutlines中
	MergedOutlines.push_back(Outline);
	
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTerrainObstructGenerator::CTerrainObstructGenerator()
{
	m_pHeightMapBuf=NULL;
	m_pResultBuf=NULL;

}

CTerrainObstructGenerator::~CTerrainObstructGenerator()
{
	if(m_pResultBuf) delete [] m_pResultBuf;
}

void CTerrainObstructGenerator::Init(BYTE* pHeightMap, int Width, int Height)
{
	m_pHeightMapBuf=pHeightMap;
	m_nWidth=Width;
	m_nHeight=Height;

	if(m_pResultBuf) delete [] m_pResultBuf;
	m_pResultBuf=new BYTE[m_nWidth*m_nHeight];

	m_Outlines.clear();
	m_LineFitOutlines.clear();
}

void CTerrainObstructGenerator::MedianFilter()
{
	int arr[9];
	
	for(int i=1;i<IMAGE_HEIGHT-1;i++)			//outer loop: y
		for(int j=1;j<IMAGE_WIDTH-1;j++)		//inner loop: x
		{
			int sx=j-1,sy=i-1;
			int cx,cy;
			int counter=0;
			for(int m=0;m<3;m++)
			{
				for(int n=0;n<3;n++)
				{
					cx=sx+n;
					cy=sy+m;
					arr[counter]=IMAGE_PIXEL(cx,cy);
					counter++;
				}	
			}
			
			RESULT_PIXEL(j,i)=GetMedian(arr,9);
		}

}

int CTerrainObstructGenerator::ProcessByTemplate(BYTE* pData,int x,int y,char* pTemplate, int tSize)
{
	int sx=x-tSize/2,sy=y-tSize/2;
	int cx,cy;
	int res=0;
	for(int i=0;i<tSize;i++)
		for(int j=0;j<tSize;j++)
		{
			cx=sx+j;
			cy=sy+i;
			res+=pData[cy*IMAGE_WIDTH+cx]*pTemplate[i*tSize+j];
		}
	
	return abs(res);	
}

void CTerrainObstructGenerator::EdgeDetection(int DiffThresh)
{
	//最外一圈的象素不处理
	for(int i=1;i<IMAGE_HEIGHT-1;i++)			//outer loop: y
		for(int j=1;j<IMAGE_WIDTH-1;j++)		//inner loop: x
		{
			bool bEdge=false;
			//依次检测四个方向的边
			int res=ProcessByTemplate(m_pHeightMapBuf,j,i,Sobel_tmpl_Horizon_Edge);
			if(res>DiffThresh)
				bEdge=true;
			else
			{
				res=ProcessByTemplate(m_pHeightMapBuf,j,i,Sobel_tmpl_Vertical_Edge);
				if(res>DiffThresh)
					bEdge=true;
				else
				{
					res=ProcessByTemplate(m_pHeightMapBuf,j,i,Sobel_tmpl_Slash_Edge);
					if(res>DiffThresh)
						bEdge=true;
					else
					{
						res=ProcessByTemplate(m_pHeightMapBuf,j,i,Sobel_tmpl_Backslash_Edge);
						if(res>DiffThresh)
							bEdge=true;
					}
				}
				
			}
			RESULT_PIXEL(j,i)=(bEdge)?EDGE_GREY:0;
		}
}

void CTerrainObstructGenerator::ProcessEdge(int Thresh)
{
	//阈值为一个0--25间的数
	if(Thresh>25 || Thresh < 0) return;

	//备份数据
	int PixelSize=IMAGE_WIDTH*IMAGE_HEIGHT;
	BYTE * bufBak= new BYTE[PixelSize];
	memcpy(bufBak,m_pResultBuf,PixelSize);

	//使用一个5×5的模板
	for(int i=2;i<IMAGE_HEIGHT-2;i++)			//outer loop: y
		for(int j=2;j<IMAGE_WIDTH-2;j++)		//inner loop: x
		{
			int counter=0;
			int sx=j-2,sy=i-2;
			int cx,cy;
			for(int m=0;m<5;m++)
			{
				for(int n=0;n<5;n++)
				{
					cx=sx+n;
					cy=sy+m;
					if(bufBak[cy*IMAGE_WIDTH+cx]==EDGE_GREY)
						counter++;						
				}
				
			}
			
			RESULT_PIXEL(j,i)=(counter>Thresh)?EDGE_GREY:0;
					
		}
	delete [] bufBak;
}

bool CTerrainObstructGenerator::IsPixelOnBorder(BYTE* Points, int x,int y,int RegionColor)
{
	if(Points[y*IMAGE_WIDTH+x]!=RegionColor)
		return false;

	// 检测4邻域
	if(Points[y*IMAGE_WIDTH+x-1]!=RegionColor)
		return true;
	if(Points[y*IMAGE_WIDTH+x+1]!=RegionColor)
		return true;

	if(Points[(y-1)*IMAGE_WIDTH+x]!=RegionColor)
		return true;
	if(Points[(y+1)*IMAGE_WIDTH+x]!=RegionColor)
		return true;

	return false;	
}

bool CTerrainObstructGenerator::TrackOutline(BYTE* OriginImg, BYTE* OutlineImg,int sx,int sy)
{
	//当前位置置为轮廓点
	OutlineImg[sy*IMAGE_WIDTH+sx]=EDGE_GREY;
	
	vector<CPoint> Outline;
	Outline.push_back(CPoint(sx,sy));

	//定义8个方向
	int Dir[8][2]={{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}};
	int cx=sx,cy=sy;
	int x,y;
	bool bNextBorder;
	do {
		
		bNextBorder=false;

		for(int i=0;i<8;i++)
		{
			x=cx+Dir[i][0];
			y=cy+Dir[i][1];

			if( x>1 && x< IMAGE_WIDTH-2 && y>1 && y <IMAGE_HEIGHT-2 &&				//x,y值有效
				IsPixelOnBorder(OriginImg,x,y) && !OutlineImg[y*IMAGE_WIDTH+x])	//是没有考察的轮廓点
			{
				cx=x;
				cy=y;
				bNextBorder=true;
				OutlineImg[cy*IMAGE_WIDTH+cx]=EDGE_GREY;
				Outline.push_back(CPoint(cx,cy));
				break;
			}
		}
	} while( bNextBorder);
	
	
	if(Outline.size()>3)		//轮廓尺寸阈值
		m_Outlines.push_back(Outline);
	
	return true;

}

void CTerrainObstructGenerator::SearchAllOutlines(int SizeThresh,int DistThresh)
{
	
	m_Outlines.clear();
	
	//构造一个buf，并对其清0
	int PixelSize=IMAGE_WIDTH*IMAGE_HEIGHT;
	BYTE * bufBak= new BYTE[PixelSize];
	memset(bufBak,0,PixelSize);
	
	for(int i=2;i<IMAGE_HEIGHT-2;i++)			//outer loop: y
		for(int j=2;j<IMAGE_WIDTH-2;j++)		//inner loop: x
		{
			//逐象素遍历
			if(IsPixelOnBorder(m_pResultBuf,j,i) &&		//位于边界
			   !bufBak[i*IMAGE_WIDTH+j])						//且没有被处理
			{
				TrackOutline(m_pResultBuf,bufBak,j,i);
			}

		}

	//结果拷贝回去
	memcpy(m_pResultBuf,bufBak,PixelSize);
	delete [] bufBak;

	
	//开始对轮廓线进行初始合并
	vector< vector<CPoint> > MergedOutlines;
	while(!m_Outlines.empty())
	{
		vector<CPoint>& curOutline=m_Outlines.back();
		MergeOutline(MergedOutlines,curOutline);		//用缺省的阈值合并
		m_Outlines.pop_back();
	}
	
	//删除掉较短的Outline
	m_Outlines.clear();
	vector< vector<CPoint> >::iterator it;
	for(it=MergedOutlines.begin();it!=MergedOutlines.end();it++)
	{
		if(it->size()>SizeThresh)
			m_Outlines.push_back((*it));
	}
	
	
	//再进行一遍合并，注意设定新的阈值
	MergedOutlines.clear();
	for(it=m_Outlines.begin();it!=m_Outlines.end();it++)
	{
		MergeOutline(MergedOutlines,*it,DistThresh);		//使用一个稍大的阈值，从而使得结果更连续
	}
	m_Outlines=MergedOutlines;
	
}

void CTerrainObstructGenerator::LineFitOutline(vector<CPoint>& Outline, vector<CPoint>& FittedLines, float FitThresh)
{
	if(Outline.empty())	
		return;
	
	//插入端点
	FittedLines.push_back(Outline[0]);

	float error;
	
	//构造协方差矩阵
	int sqX_sum,sqY_sum,x_sum,y_sum,xy_sum;
	float x_mean,y_mean,sqX_mean,sqY_mean,xy_mean;
	sqX_sum=sqY_sum=x_sum=y_sum=xy_sum=0;
	
	//添加首元素
	x_sum+=Outline[0].x;
	y_sum+=Outline[0].y;
	sqX_sum+=Outline[0].x*Outline[0].x;
	sqY_sum+=Outline[0].y*Outline[0].y;
	xy_sum+=Outline[0].x*Outline[0].y;
	int sum=1;
	vector<CPoint>::iterator it=Outline.begin();
	for(it = it + 1;it!=Outline.end();it++)
	{
		x_sum+=(*it).x;
		y_sum+=(*it).y;
		sqX_sum+=(*it).x*(*it).x;
		sqY_sum+=(*it).y*(*it).y;
		xy_sum+=(*it).x*(*it).y;
		sum++;

		//开始拟合
		x_mean=x_sum/(float)sum;
		y_mean=y_sum/(float)sum;
		sqX_mean=sqX_sum/(float)sum;
		sqY_mean=sqY_sum/(float)sum;
		xy_mean=xy_sum/(float)sum;
		
		CMatrix m(2,2);
		m.SetElement(0,0,sqX_mean-x_mean*x_mean);
		m.SetElement(0,1,xy_mean-x_mean*y_mean);
		m.SetElement(1,0,xy_mean-x_mean*y_mean);
		m.SetElement(1,1,sqY_mean-y_mean*y_mean);
		
		double eValue[2];
		CMatrix eVectors(2,2);
		m.JacobiEigenv(eValue,eVectors);
		
		error=(eValue[0]>eValue[1])?eValue[1]:eValue[0];
		//error/=sum;

		if(error>FitThresh)			//拟合失败的阈值
		{
			FittedLines.push_back(*(it-1));
			sqX_sum=sqY_sum=x_sum=y_sum=xy_sum=0;
			sum=0;
			x_sum+=(*it).x;
			y_sum+=(*it).y;
			sqX_sum+=(*it).x*(*it).x;
			sqY_sum+=(*it).y*(*it).y;
			xy_sum+=(*it).x*(*it).y;
			sum++;
		}

	}

	//最后一个元素插入到折线系列中
	FittedLines.push_back(*(it-1));
	
}

void CTerrainObstructGenerator::FitAllOutlines(float FitThresh)
{
	m_LineFitOutlines.clear();

	for(int i=0;i<m_Outlines.size();i++)
	{
		vector<CPoint> FittedLines;
		LineFitOutline(m_Outlines[i],FittedLines,FitThresh);
		m_LineFitOutlines.push_back(FittedLines);
	}
}