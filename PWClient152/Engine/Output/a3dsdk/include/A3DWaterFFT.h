/*
 * FILE: A3DWaterFFT.h
 *
 * DESCRIPTION: FFT生成波动纹理
 *
 * CREATED BY: Pan Yupin, 2012/6/21
 *
 * HISTORY:
 *
 * Copyright (c) 2012 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DWATERFFT_H_
#define _A3DWATERFFT_H_

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DMaterial.h"
#include "A3DMaterial.h"
#include "A3DGeometry.h"
#include "AArray.h"
#include "AAssist.h"
#include "A3DMacros.h"

#include <stdlib.h>
#include <complex>

typedef unsigned int uint32;
//////////////////////////////////////////////////////////////////////////
class MTRandInt32 
{ 
	// Mersenne Twister random number generator
public:
	// default constructor
	MTRandInt32() { Seed(5489UL); }
	// constructor with 32 bit int as seed
	MTRandInt32(uint32 s) { Seed(s); }
	// constructor with array of size 32 bit ints as seed
	MTRandInt32(const uint32* array, int size) { Seed(array, size); }
	// the two seed functions
	void Seed(uint32); // seed with 32 bit integer
	void Seed(const uint32*, int size); // seed with array
	// overload operator() to make this a generator (functor)
	//uint32 operator()() { return rand_int32(); }
	uint32 Generate() { return RandInt32(); }

	// generates random floating number in the closed interval [0,1].
	float GenerateFloat() { return (float)RandInt32()*(1.0f/4294967295.0f); }

	~MTRandInt32() {} // destructor

protected: // used by derived classes, otherwise not accessible; use the ()-operator
	//uint32 rand_int32(); // generate 32 bit random integer
	uint32 RandInt32() 
	{ // generate 32 bit random int
		if (p >= n) GenState(); // new m_nState vector needed
		// gen_state() is split off to be non-inline, because it is only called once
		// in every 624 calls and otherwise irand() would become too big to get inlined
		uint32 x = m_nState[p++];
		x ^= (x >> 11);
		x ^= (x << 7) & 0x9D2C5680UL;
		x ^= (x << 15) & 0xEFC60000UL;
		return x ^ (x >> 18);
	}
private:
	static const int n = 624, m = 397; // compile time constants

	// the variables below are static (no duplicates can exist)
	uint32 m_nState[n+1]; // m_nState vector array
	int p; // position in m_nState array
	// private functions used to generate the pseudo random numbers
	uint32 Twiddle(uint32 u, uint32 v) 
	{
		return (((u & 0x80000000UL) | (v & 0x7FFFFFFFUL)) >> 1)
			^ ((v & 1UL) ? 0x9908B0DFUL : 0x0UL);
	}
	void GenState(); // generate new m_nState
	// make copy constructor and assignment operator unavailable, they don't make sense
	MTRandInt32(const MTRandInt32&); // copy constructor not defined
	void operator=(const MTRandInt32&); // assignment operator not defined
};

extern MTRandInt32 g_random_generator;

unsigned int MtRand();


#ifndef PI
#define PI A3D_PI
#endif
//if enlarging it, spu handling must change
#define GRID_SIZE 64
#define GRID_LOG_SIZE 6

static float gaussian_y2;
static int gaussian_use_last = 0;

class A3DWaterFFT
{
	typedef std::complex<float> complexF;
public: 
	A3DWaterFFT():
	  m_fA(1.0f), m_fWindDir(0.0f), m_fWorldSizeX(1.0f), m_fWorldSizeY(1.0f),
		  m_fWorldSizeXInv(1.f), m_fWorldSizeYInv(1.0f),
		  m_fMaxWaveSize( 200.0f ), m_fChoppyWaveScale( 400.0f ), m_nFrameID(0)
	  {
		  unsigned int nSize = GRID_SIZE * GRID_SIZE * sizeof(complexF);
		  memset(&m_pFourierAmps[0], 0, nSize );
		  memset(&m_pHeightField[0], 0, nSize );
		  memset(&m_pDisplaceFieldX[0], 0, nSize );
		  memset(&m_pDisplaceFieldY[0], 0, nSize );

		  nSize = GRID_SIZE * GRID_SIZE * sizeof(A3DVECTOR4);
		  memset(m_pDisplaceGrid, 0, nSize );
		  memset(m_pLUTK, 0, nSize );

	  }

	  ~A3DWaterFFT()
	  {
		  Release();
	  }

	  float GetWind() { return m_fWindDir; }
	  float GetGravity(){ return m_fG; }
	  float GetAConstant(){ return m_fA; }

	  virtual void Create(float fG, float fA, float fWindDir,
		  float fWorldSizeX, float fWorldSizeY); // Create/Initialize water simulation;

	  virtual void Release();

	  // Update simulation
	  void Update(int nFrameID, float fTime, bool bOnlyHeight = false);

	  A3DVECTOR3 GetPositionAt(int x, int y) const
	  {
		  A3DVECTOR4 pPos = m_pDisplaceGrid[ GetGridOffsetWrapped(x, y) ];
		  return A3DVECTOR3(pPos.x, pPos.y, pPos.z);

	  }

	  float GetHeightAt(int x, int y) const
	  {
		  return m_pDisplaceGrid[ GetGridOffsetWrapped(x, y) ].z;
	  }

	  A3DVECTOR4 *GetDisplaceGrid() 
	  {
		  return m_pDisplaceGrid;
	  }

public:
// 	A3DTexture* m_pTexTestInitK;
// 	A3DTexture* m_pTexTestInitH0;
// 	A3DTexture* m_pTexFFTInputH;
// 	A3DTexture* m_pTexFFTInputX;
// 	A3DTexture* m_pTexFFTInputZ;

private:
	static void SinCosf (float angle, float* pSin, float* pCos)
	{
		*pSin = float(sin(angle));
		*pCos = float(cos(angle));
	}

	static float Sqr(float f)
	{
		return f*f;
	}

	float FRand()
	{
		return ((float) MtRand())/((float) RAND_MAX);
	}

	float SFRand()
	{
		return (MtRand() * 2.0f / (float) RAND_MAX) - 1.0f;
	}

	// Returns (-1)^n
	int PowNeg1(int n)
	{
		int pow[2] = { 1, -1 };
		return pow[n&1];
	}

	// gaussian random number, using box muller technique
	float FRandGaussian(float m=0.0f, float s=1.0f);  // m- mean, s - standard deviation;

	inline int GetGridOffset(const int &x, const int &y) const
	{
		return y * GRID_SIZE + x;
	}

	int GetMirrowedGridOffset(const int &x, const int &y) const
	{
		return GetGridOffset((GRID_SIZE-x)&(GRID_SIZE-1), (GRID_SIZE-y)&(GRID_SIZE-1));
	}

	int GetGridOffsetWrapped(const int &x, const int &y) const
	{
		return GetGridOffset(x&(GRID_SIZE-1), y&(GRID_SIZE-1));
	}

	void ComputeFFT1D(int nDir, float *pReal, float *pImag);
	void ComputeFFT2D(int nDir, complexF*const __restrict c);

	float GetIndexToWorldX(int x)
	{
		static const float PI2ByWorldSizeX=(2.0f*PI)/m_fWorldSizeX;
		return (float(x)-((float)GRID_SIZE/2.0f))*PI2ByWorldSizeX;
	}

	float GetIndexToWorldY(int y)
	{
		static const float PI2ByWorldSizeY=(2.0f*PI)/m_fWorldSizeY;
		return (float(y)-((float)GRID_SIZE/2.0f))*PI2ByWorldSizeY;
	}

	float GetTermAngularFreq(float k);

	// a - constant, pK - wave dir (2d), pW - wind dir (2d)
	float ComputePhillipsSpec(const D3DXVECTOR2 &pK, const D3DXVECTOR2 &pW) const;

	void InitTableK();

	// Initialize Fourier amplitudes table
	void InitFourierAmps();

	A3DVECTOR4 m_pLUTK[ GRID_SIZE * GRID_SIZE ]; // pre-computed K table
	A3DVECTOR4 m_pDisplaceGrid[ GRID_SIZE * GRID_SIZE ]; // Current displace grid

	// Simulation constants
	complexF m_pFourierAmps[GRID_SIZE*GRID_SIZE];			// Fourier amplitudes at time 0 (aka. H0)
	complexF m_pHeightField[GRID_SIZE*GRID_SIZE];			// Current Fourier amplitudes height field
	complexF m_pDisplaceFieldX[GRID_SIZE*GRID_SIZE];	// Current Fourier amplitudes displace field 
	complexF m_pDisplaceFieldY[GRID_SIZE*GRID_SIZE];	// Current Fourier amplitudes displace field 

	float m_fG;
	float m_fA; // constant value
	float m_fWindDir; // window direction angle
	float m_fWorldSizeX; // world dimensions
	float m_fWorldSizeY; // world dimensions

	float m_fWorldSizeXInv; // 1.f / world dimensions
	float m_fWorldSizeYInv; // 1.f / world dimensions

	float m_fMaxWaveSize;
	float m_fChoppyWaveScale;

	int m_nFrameID;

};






#endif