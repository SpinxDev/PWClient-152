//----------------------------------------------------------
// Filename	: TerrainStretchBrush.h
// Creator	: QingFeng Xin
// Date		: 2004.7.21
// Desc		: 画刷拓展类，用于地形高度图的修改操作
//-----------------------------------------------------------

#ifndef _TERRAIN_STRETCH_BRUSH_H_
#define _TERRAIN_STRETCH_BRUSH_H_


#include "Brush.h"


class CTerrainStretchBrush : public CElementBrush
{
public:

	CTerrainStretchBrush()
	{ 
		m_nBrushHard = 20; 
		m_fBrushHeight = 1.0f; 
		m_nGray = 255;
		m_bSmooth = true;

		//Noise
		m_nWaveLength = 1;
		m_nPersistence = 50;
		m_nAmplitude = 10;
		m_nOctaveNum = 1;
	};
	virtual ~CTerrainStretchBrush(){};
	
	virtual void Render(A3DViewport* pA3DViewport);
	virtual void Tick(DWORD dwTimeDelta);
	virtual void SetCenter(float x, float z);

	int GetBrushHard(){ return m_nBrushHard; };
	float GetBrushHeight(){ return m_fBrushHeight; };
	bool GetBrushSmoothSort(){ return m_bSmooth; };
	int GetBrushGray() { return m_nGray; };

	int GetNoiseWaveLenght(){ return m_nWaveLength; };
	int GetNoisePersistence(){ return m_nPersistence; };
	int GetNoiseAmplitude(){ return m_nAmplitude; };
	int GetNoiseOctaveNum(){ return m_nOctaveNum; };


	void SetBrushHard( int nHard){ m_nBrushHard = nHard; };
	void SetBrushHeight( float fHeight){ m_fBrushHeight = fHeight; };
	void SetBrushSmoothSort(bool bSmooth){ m_bSmooth = bSmooth; };
	void SetBrushGray( int nGray) { m_nGray = nGray; };

	void SetNoiseWaveLenght(int waveLength){ m_nWaveLength = waveLength; };
	void SetNoisePersistence( int persistence){ m_nPersistence = persistence; };
	void SetNoiseAmplitude( int amplitude){ m_nAmplitude = amplitude; };
	void SetNoiseOctaveNum( int octaveNum){ m_nOctaveNum = octaveNum; };
	

private:
	int m_nBrushHard;
	float m_fBrushHeight;
	
	//Only use to paint mask map
	int m_nGray;

	//Only use to noise
	int m_nWaveLength;
	int m_nPersistence;
	int m_nAmplitude;
	int m_nOctaveNum;

	bool m_bSmooth;

};

#endif// _TERRAIN_STRETCH_BRUSH_H_