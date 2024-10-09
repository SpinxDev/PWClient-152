//////////////////////////////////////////////////////////////////////
// TerrainObstructGenerator.h: interface for the CTerrainObstructGenerator class.
// Created by He Wenfeng, 2005-01-06
// A class describes a Obstruct Generator from a terrain 
// which is represented by a height map.
//////////////////////////////////////////////////////////////////////

#ifndef _TERRAINOBSTRUCTGENERATOR_H_
#define _TERRAINOBSTRUCTGENERATOR_H_

#define EDGE_GREY 255				//边界的灰度级别

#include <vector>
using namespace std;

class CTerrainObstructGenerator  
{
public:
	CTerrainObstructGenerator();
	virtual ~CTerrainObstructGenerator();

// Interface
	void Init(BYTE* pHeightMap, int Width=513, int Height=513);
	
	BYTE* GetHeightMapBuf() { return m_pHeightMapBuf;}
	BYTE* GetResultBuf() { return m_pResultBuf;}
	int GetWidth() { return m_nWidth;}
	int GetHeight() { return m_nHeight;}

	const std::vector< std::vector<CPoint> >& GetOutlines() {return m_Outlines;}
	const std::vector< std::vector<CPoint> >& GetLineFitOutlines() {return m_LineFitOutlines;}
		
	//中值滤波，去噪，采用3*3尺寸
	void MedianFilter();		
	
	//边缘检测，现使用Sobel算子，阈值为象素灰度的差分阈值
	void EdgeDetection(int DiffThresh=10);	//0-255

	//对提取的边缘出现的指纹现象进行处理，采用5*5邻域过滤，阈值表示邻域中边界象素的计数阈值
	void ProcessEdge(int Thresh=8);        //0-25

	//搜索出所有的轮廓线，SizeThresh为设定的轮廓线所包含象素的最小数量
	// DistThresh阈值为两轮廓线连接的距离阈值
	void SearchAllOutlines(int SizeThresh=10,int DistThresh=8);		//5-20, 5-10

	void FitAllOutlines(float FitThresh=0.8f);		//0.5-1.0

protected:

// protected functions
	int ProcessByTemplate(BYTE* pData,int x,int y,char* pTemplate, int tSize=3);
	
	bool IsPixelOnBorder(BYTE* Points, int x,int y,int RegionColor=EDGE_GREY);

	// 从点(sx,sy)处开始跟踪轮廓
	bool TrackOutline(BYTE* OriginImg, BYTE* OutlineImg,int sx,int sy);

	//对邻接象素表示的轮廓进行拟合，参数FitThresh定义了拟合效果的阈值
	void LineFitOutline(std::vector<CPoint>& Outline, std::vector<CPoint>& FittedLines, float FitThresh);

// Attributes
	BYTE* m_pHeightMapBuf;
	BYTE* m_pResultBuf;

	int m_nWidth;
	int m_nHeight;

	// Output: a set of vectors which describe outlines of obstruct edge
	std::vector< std::vector<CPoint> > m_Outlines;				//有序并邻接的象素点描述的边缘轮廓
	
	std::vector< std::vector<CPoint> > m_LineFitOutlines;			//对m_Outlines拟合后用折线段表示的轮廓

	// some thresholds to control the result
	//int m_SobelThresh;

	
};

// 冒泡排序
void BubbleSort(int *Nums,int num);	

// 计算一组数的中值
inline int GetMedian(int *Nums,int num)
{
	//先对nums作一个排序
	BubbleSort(Nums,num);
	return Nums[num/2+1];
}

#endif // #define _TERRAINOBSTRUCTGENERATOR_H_
