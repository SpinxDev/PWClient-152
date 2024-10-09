/*
 * FILE: PassMapGenerator.h
 *
 * DESCRIPTION:  A class which generates the pass map of NPCs
 *							The pass map will be loaded in the server for NPCs' moving strategy
 *
 * CREATED BY: He wenfeng, 2005/3/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _PASSMAPGENERATOR_H_
#define _PASSMAPGENERATOR_H_

#include "math.h"
#include "A3DVector.h"

#include "BitImage.h"
#include "BlockImage.h"

typedef unsigned char UCHAR;

#define FLOATTOINT(x) ((int) floor((x)+0.5f))						// ��������

#define STANDARD_PASSMAP_SAMPLE_STEP 1.0f
#define STANDARD_PASSMAP_SIZE 1024

struct SAMPLE_POS {
	int u;				// X
	int v;				// Z
	float h;		  // Y
};

class CPassMapGenerator
{
public:
	CPassMapGenerator();
	virtual ~CPassMapGenerator();	

// Interface
	bool ExportMaps(CBitImage* pReachableImage, CBlockImage<FIX16>* pDeltaHImage);
	bool ImportMaps(const CBitImage* pReachableImage, const CBlockImage<FIX16>* pDeltaHImage);

	void ComputePassMap();

	void ExpandPassRegion(const SAMPLE_POS& posSeed);		//��һ�����ӵ����ſɴ�����
	
	// it seems we haven't employ this function
	void Reset()
	{
		int iSize=m_iWidth*m_iLength;
		// ��ʼ������ ����m_arrPassMap���ԣ�0��ʾδ���㣬1��ʾ�ɴ2��ʾ���ɴ�
		memset(m_arrPassMap, 0, iSize*sizeof(UCHAR));
		memset(m_arrHeightMap, 0, iSize*sizeof(float));
	}

	void Release();
	void SetSampleInfo(float fss =STANDARD_PASSMAP_SAMPLE_STEP, int iTWidth=STANDARD_PASSMAP_SIZE, int iTLength=STANDARD_PASSMAP_SIZE);
	void SetPFSphereInfo(float fr=0.3f, float fsh=1.0f)
	{
		m_fPFRadius=fr;
		m_fPFStepHeight=fsh;
	}
	
	void GetMapSize(int& iWidth, int& iLength) const { iWidth=m_iWidth; iLength=m_iLength; }
	UCHAR* GetPassMap(){ return m_arrPassMap;}
	FIX16 * GetDeltaHeightMap() { return m_arrHeightMap; }
	
	// uv ����ϵ��������ά����ϵ��ת��
	void Transf2DTo3D(const SAMPLE_POS& sPos, A3DVECTOR3& vPos);
	void Transf3DTo2D(const A3DVECTOR3& vPos, SAMPLE_POS& sPos);
	
	void FillUnreachableArea();				// ��δ����������Ϊ���ɴ�����

	void InflateUnreachableArea();			// ���Ͳ��ɴ�����Ŀǰ�ò����Ѿ���ֹ�Ǽ򵥵����ͣ������˲��ٵ�ͼ��������

	// ֱ�Ӷ����ز�����
	void SetPosReachable(const SAMPLE_POS& sPos);
	void SetPosUnreachable(const SAMPLE_POS& sPos);

	bool IsPosReachable(const SAMPLE_POS& sPos);
	float GetPosDeltaHeight(const SAMPLE_POS& sPos);
	
	bool IsPosReachable(const A3DVECTOR3 & vPos)
	{
		SAMPLE_POS sPos;
		Transf3DTo2D(vPos, sPos);
		return IsPosReachable(sPos);
	}

	float GetPosDeltaHeight(const A3DVECTOR3 & vPos)
	{
		SAMPLE_POS sPos;
		Transf3DTo2D(vPos, sPos);
		return GetPosDeltaHeight(sPos);
	}
	

protected:

// Methods
	bool CanPFMove(const SAMPLE_POS& posFrom, SAMPLE_POS& posTo);

	// Tell whether the seed pos is a valid seed for ExpandPassRegion 
	bool IsSeedPosValid(SAMPLE_POS& posSeed);

	SAMPLE_POS GenSeedPos();
	
	bool SearchValidSeed(int iTryTimes, SAMPLE_POS& seed);


// Attributes
	
	// Terrain sample step and resolution
	float m_fSampleStep;
	int m_iWidth;
	int m_iLength;
	
	// Path finder info: a path finder is a moving sphere 
	// which deal with the CD in the scene and then get the pass info
	float m_fPFRadius;
	float m_fPFStepHeight;

	// Pass Map: a 2D bit set, but by now stored as a  char array
	UCHAR *m_arrPassMap;
	FIX16 *m_arrHeightMap;	
};

#endif