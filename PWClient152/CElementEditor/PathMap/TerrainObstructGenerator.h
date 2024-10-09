//////////////////////////////////////////////////////////////////////
// TerrainObstructGenerator.h: interface for the CTerrainObstructGenerator class.
// Created by He Wenfeng, 2005-01-06
// A class describes a Obstruct Generator from a terrain 
// which is represented by a height map.
//////////////////////////////////////////////////////////////////////

#ifndef _TERRAINOBSTRUCTGENERATOR_H_
#define _TERRAINOBSTRUCTGENERATOR_H_

#define EDGE_GREY 255				//�߽�ĻҶȼ���

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
		
	//��ֵ�˲���ȥ�룬����3*3�ߴ�
	void MedianFilter();		
	
	//��Ե��⣬��ʹ��Sobel���ӣ���ֵΪ���ػҶȵĲ����ֵ
	void EdgeDetection(int DiffThresh=10);	//0-255

	//����ȡ�ı�Ե���ֵ�ָ��������д�������5*5������ˣ���ֵ��ʾ�����б߽����صļ�����ֵ
	void ProcessEdge(int Thresh=8);        //0-25

	//���������е������ߣ�SizeThreshΪ�趨�����������������ص���С����
	// DistThresh��ֵΪ�����������ӵľ�����ֵ
	void SearchAllOutlines(int SizeThresh=10,int DistThresh=8);		//5-20, 5-10

	void FitAllOutlines(float FitThresh=0.8f);		//0.5-1.0

protected:

// protected functions
	int ProcessByTemplate(BYTE* pData,int x,int y,char* pTemplate, int tSize=3);
	
	bool IsPixelOnBorder(BYTE* Points, int x,int y,int RegionColor=EDGE_GREY);

	// �ӵ�(sx,sy)����ʼ��������
	bool TrackOutline(BYTE* OriginImg, BYTE* OutlineImg,int sx,int sy);

	//���ڽ����ر�ʾ������������ϣ�����FitThresh���������Ч������ֵ
	void LineFitOutline(std::vector<CPoint>& Outline, std::vector<CPoint>& FittedLines, float FitThresh);

// Attributes
	BYTE* m_pHeightMapBuf;
	BYTE* m_pResultBuf;

	int m_nWidth;
	int m_nHeight;

	// Output: a set of vectors which describe outlines of obstruct edge
	std::vector< std::vector<CPoint> > m_Outlines;				//�����ڽӵ����ص������ı�Ե����
	
	std::vector< std::vector<CPoint> > m_LineFitOutlines;			//��m_Outlines��Ϻ������߶α�ʾ������

	// some thresholds to control the result
	//int m_SobelThresh;

	
};

// ð������
void BubbleSort(int *Nums,int num);	

// ����һ��������ֵ
inline int GetMedian(int *Nums,int num)
{
	//�ȶ�nums��һ������
	BubbleSort(Nums,num);
	return Nums[num/2+1];
}

#endif // #define _TERRAINOBSTRUCTGENERATOR_H_
