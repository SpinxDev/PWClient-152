#include "A3DWaterFFT.h"

void MTRandInt32::GenState()
{
	for (int i = 0; i < (n - m); ++i)
		m_nState[i] = m_nState[i + m] ^ Twiddle(m_nState[i], m_nState[i + 1]);
	for (int i = n - m; i < (n - 1); ++i)
		m_nState[i] = m_nState[i + m - n] ^ Twiddle(m_nState[i], m_nState[i + 1]);
	m_nState[n - 1] = m_nState[m - 1] ^ Twiddle(m_nState[n - 1], m_nState[0]);
	p = 0; // reset position
}

void MTRandInt32::Seed(uint32 s)
{
	for (int i = 0; i < n; ++i)
		m_nState[i]=0x0UL;
	m_nState[0] = s;
	for (int i = 1; i < n; ++i)
	{
		m_nState[i] = 1812433253UL * (m_nState[i - 1] ^ (m_nState[i - 1] >> 30)) + i;
	}
	p = n;
}

void MTRandInt32::Seed(const uint32* array, int size)
{
	Seed(19650218UL);
	int i = 1, j = 0;
	for (int k = ((n > size) ? n : size); k; --k) {
		m_nState[i] = (m_nState[i] ^ ((m_nState[i - 1] ^ (m_nState[i - 1] >> 30)) * 1664525UL))
			+ array[j] + j; // non linear
		++j; j %= size;
		if ((++i) == n) { m_nState[0] = m_nState[n - 1]; i = 1; }
	}
	for (int k = n - 1; k; --k)
	{
		m_nState[i] = (m_nState[i] ^ ((m_nState[i - 1] ^ (m_nState[i - 1] >> 30)) * 1566083941UL)) - i;
		if ((++i) == n) { m_nState[0] = m_nState[n - 1]; i = 1; }
	}
	m_nState[0] = 0x80000000UL;
	p = n;
}

unsigned int MtRand()
{
	return g_random_generator.Generate() & RAND_MAX;
}

MTRandInt32 g_random_generator;


#include <fstream>
using namespace std;

float A3DWaterFFT::FRandGaussian( float m/*=0.0f*/, float s/*=1.0f*/ ) /* m- mean, s - standard deviation */
{
	float x1, x2, w, y1;
	if(gaussian_use_last)  // use value from previous call
	{
		y1 = gaussian_y2;
		gaussian_use_last = 0;
	}
	else
	{
		do 
		{
			x1 = 2.0f * FRand() - 1.0f;
			x2 = 2.0f * FRand() - 1.0f;

			w = x1 * x1 + x2 * x2;
		}
		while( w >= 1.0f);

		w = sqrtf((-2.0f * logf(w)) / w);
		y1 = x1 * w;
		gaussian_y2 = x2 * w;

		gaussian_use_last = 1;
	}

	return (m + y1 * s);
}

void A3DWaterFFT::ComputeFFT1D( int nDir, float *pReal, float *pImag )
{
	// reference: "2 Dimensional FFT" - by Paul Bourke
	long nn, i, i1, j, k, i2, l, l1, l2;
	float c1, c2, fReal, fImag, t1, t2, u1, u2, z;

	nn= GRID_SIZE;
	float fRecipNN = 1.0f / (float) nn;

	// Do the bit reversal
	i2= nn >> 1;
	j= 0;
	for(i= 0; i<nn - 1; ++i)
	{
		if(i<j)
		{
			fReal= pReal[i];
			fImag= pImag[i];
			pReal[i]= pReal[j];
			pImag[i]= pImag[j];
			pReal[j]= fReal;
			pImag[j]= fImag;
		}

		k= i2;
		while(k<=j)
		{
			j-= k;
			k>>= 1;
		}

		j+= k;
	}

	// FFT computation
	c1= -1.0f;
	c2= 0.0f;
	l2= 1;
	for(l= 0; l<GRID_LOG_SIZE; ++l)
	{
		l1= l2;
		l2<<= 1;
		u1= 1.0;
		u2= 0.0;
		for(j= 0; j<l1; ++j)
		{
			for(i= j; i<nn; i+= l2)
			{
				i1= i + l1;
				t1= u1 * pReal[i1] - u2 * pImag[i1];
				t2= u1 * pImag[i1] + u2 * pReal[i1];
				pReal[i1]= pReal[i] - t1;
				pImag[i1]= pImag[i] - t2;
				pReal[i]+= t1;
				pImag[i]+= t2;
			}

			z=  u1 * c1 - u2 * c2;
			u2= u1 * c2 + u2 * c1;
			u1= z;
		}

		c2= sqrtf((1.0f - c1) * 0.5f );

		if(nDir==1)
		{
			c2= -c2;
		}

		c1= sqrtf(( 1.0f + c1 ) * 0.5f );
	}

	// Scaling for forward transform
	if(nDir==1)
	{
		for(i = 0; i < nn; ++i)
		{
			pReal[i] *= fRecipNN;
			pImag[i] *= fRecipNN;
		}
	}
}

void A3DWaterFFT::ComputeFFT2D( int nDir, complexF*const __restrict c )
{
	// reference: "2 Dimensional FFT" - by Paul Bourke
	float pReal[ GRID_SIZE ];
	float pImag[ GRID_SIZE ];

	// Transform the rows
	for(int y(0); y<GRID_SIZE; ++y)
	{
		for(int x(0); x<GRID_SIZE; ++x)
		{
			int nGridOffset = GetGridOffset(x, y);
			const complexF& curC = c[ nGridOffset ];
			pReal[x]= curC.real();
			pImag[x]= curC.imag();
		}

		ComputeFFT1D(nDir, pReal, pImag );

		for(int x(0); x<GRID_SIZE; ++x)
		{
			int nGridOffset = GetGridOffset(x, y);
			c[nGridOffset] = complexF(pReal[x], pImag[x] );
		}
	}

	// Transform the columns
	for(int x(0); x<GRID_SIZE; ++x)
	{
		for(int y(0); y<GRID_SIZE; ++y)
		{
			int nGridOffset = GetGridOffset(x, y);
			const complexF& curC = c[ nGridOffset ];
			pReal[y]= curC.real();
			pImag[y]= curC.imag();
		}

		ComputeFFT1D(nDir, pReal, pImag);

		for(int y(0); y<GRID_SIZE; ++y)
		{
			int nGridOffset = GetGridOffset(x, y);
			c[ nGridOffset ] = complexF(pReal[y], pImag[y]);
		}
	}
}

float A3DWaterFFT::GetTermAngularFreq( float k )
{
	// reference: "Simulating Ocean Water" - by Jerry Tessendorf (3.2)
	return sqrtf(k * m_fG);
}

float A3DWaterFFT::ComputePhillipsSpec( const D3DXVECTOR2 &pK, const D3DXVECTOR2 &pW ) const
{
	float fK2= pK.x * pK.x + pK.y * pK.y; 

	if(fK2 == 0)
	{
		return 0.0f;
	}

	float fW2= pW.x * pW.x + pW.y * pW.y;
	float fL= fW2/m_fG;
	float fL2= Sqr(fL);

	// reference: "Simulating Ocean Water" - by Jerry Tessendorf (3.3)
	float fPhillips= m_fA* (expf(-1.0f / (fK2 * fL2)) / Sqr(fK2)) 
		* (Sqr(pK.x * pW.x + pK.y * pW.y/*pK.Dot(pW)*/) /fK2 * fW2);

	//assert(fPhillips>=0);
	return fPhillips;
}

void A3DWaterFFT::InitTableK()
{
	for(int y = 0; y < GRID_SIZE; ++y)
	{
		float fKy=GetIndexToWorldY(y);
		for(int x = 0; x<GRID_SIZE; ++x)
		{
			float fKx = GetIndexToWorldX(x);
			float fKLen = sqrtf(fKx * fKx + fKy * fKy);
			float fAngularFreq = GetTermAngularFreq(fKLen);
			m_pLUTK[GetGridOffset(x, y)] = A3DVECTOR4(fKx, fKy, fKLen, fAngularFreq);
		}
	}
}



void A3DWaterFFT::InitFourierAmps()
{
	static bool bFirstRun = true;
	static complexF s_gaussianRandom[GRID_SIZE][GRID_SIZE];

	const float recipSqrt2= 1.0f / sqrtf(2.0f);

	D3DXVECTOR2 pW(cosf(m_fWindDir), sinf(m_fWindDir));
	pW = -pW; // meh - match with regular water animation
	//pW *= m_fWindScale;

	for(int y = 0; y < GRID_SIZE; ++y)
	{
		//float fKy=GetIndexToWorldY(y);
		for(int x = 0; x < GRID_SIZE; ++x)
		{
			complexF e;
			if(bFirstRun)
			{
				e = complexF(FRandGaussian(), FRandGaussian());
				s_gaussianRandom[y][x] = e;
			}
			else
				e = s_gaussianRandom[y][x];

			int nGridOffset = GetGridOffset(x, y);

			A3DVECTOR4 pLookupK = m_pLUTK[nGridOffset];
			D3DXVECTOR2 pK = D3DXVECTOR2(pLookupK.x, pLookupK.y);

			// reference: "Simulating Ocean Water" - by Jerry Tessendorf (3.4)
			e *= recipSqrt2 * sqrtf(ComputePhillipsSpec(pK, pW));
			m_pFourierAmps[nGridOffset] = e;
		}
	}

	bFirstRun = false;
}

void A3DWaterFFT::Update( int nFrameID, float fTime, bool bOnlyHeight /*= false*/ )
{
	if( m_nFrameID == nFrameID )
		return;

	m_nFrameID = nFrameID;


	// Optimization: only half grid is required to update, since we can use conjugate to the other half
	float fHalfY=(GRID_SIZE>>1)+1;
	for(int y = 0; y<fHalfY; ++y)
	{
		for(int x = 0; x<GRID_SIZE; ++x)
		{
			int offset = GetGridOffset(x, y);
			int offsetMirrowed = GetMirrowedGridOffset(x, y);

			A3DVECTOR4 pK = m_pLUTK[offset];

			float fKLen = pK.z; //pK.GetLength();
			float fAngularFreq = pK.w * fTime; //GetTermAngularFreq(fKLen)  //return cry_sqrtf_fast(k*m_fG);

			float fAngularFreqSin = 0, fAngularFreqCos = 0;
			SinCosf( (float)fAngularFreq, (float*) &fAngularFreqSin, (float*)&fAngularFreqCos);

			complexF ep( fAngularFreqCos, fAngularFreqSin );
			complexF em = conj(ep);

			// reference: "Simulating Ocean Water" - by Jerry Tessendorf (3.4) 
			complexF currWaveField= m_pFourierAmps[offset] * ep + conj(m_pFourierAmps[offsetMirrowed]) * em;

			m_pHeightField[offset]= currWaveField;
			if( !bOnlyHeight && fKLen)
			{
				//Crysis的做法
				m_pDisplaceFieldX[offset] = currWaveField * complexF(0, (-pK.x - pK.y) / fKLen);   
				m_pDisplaceFieldY[offset] = currWaveField * complexF(0, -pK.y / fKLen);    

				//我的修改, 看不出什么区别
				m_pDisplaceFieldX[offset] = currWaveField * complexF(0, (-pK.x - pK.y) / fKLen);   
				m_pDisplaceFieldY[offset] = currWaveField * complexF(0, (-pK.y + pK.x * 0.5f) / fKLen); 

			}
			else
			{
				m_pDisplaceFieldX[offset] = 0;
				m_pDisplaceFieldY[offset] = 0;
			}

			// Set upper half using conjugate
			if (y != fHalfY-1)
			{
				m_pHeightField[offsetMirrowed]= conj(m_pHeightField[offset]);
				if( !bOnlyHeight )
				{
					m_pDisplaceFieldX[offsetMirrowed] = conj(m_pDisplaceFieldX[offset]);                  
					m_pDisplaceFieldY[offsetMirrowed] = conj(m_pDisplaceFieldY[offset]);
				}
			}
		}
	}

// 	D3DLOCKED_RECT lockrect;
// 	m_pTexFFTInputH->LockRect(0, &lockrect, NULL, 0);
// 	BYTE* p = (BYTE*)lockrect.pBits;
// 	for(int i = 0; i < NY; i++)
// 	{
// 		float* pBits = (float*)p;
// 		for(int j = 0; j < NX; j++)
// 		{
// 			//random numbers between -1 and 1
// 			pBits[j * 4] =		m_pHeightField[GetGridOffset(j,i)].real() * 10e5f;
// 			pBits[j * 4 + 1] =	m_pHeightField[GetGridOffset(j,i)].imag() * 10e5f;
// 			pBits[j * 4 + 2] =	0;		//klen
// 			pBits[j * 4 + 3] = 1;
// 		}
// 		p += lockrect.Pitch;
// 	}
// 	m_pTexFFTInputH->UnlockRect(0);

// 	m_pTexFFTInputX->LockRect(0, &lockrect, NULL, 0);
// 	p = (BYTE*)lockrect.pBits;
// 	for(int i = 0; i < NY; i++)
// 	{
// 		float* pBits = (float*)p;
// 		for(int j = 0; j < NX; j++)
// 		{
// 			//random numbers between -1 and 1
// 			pBits[j * 4] =		m_pDisplaceFieldX[GetGridOffset(j,i)].real() * 10e5f;		//kx
// 			pBits[j * 4 + 1] =	m_pDisplaceFieldX[GetGridOffset(j,i)].imag() * 10e5f;		//ky
// 			pBits[j * 4 + 2] =	0;		//klen
// 			pBits[j * 4 + 3] = 1;
// 			//fout<< pBits[j * 4] << "  " << pBits[j * 4] <<endl;
// 		}
// 		p += lockrect.Pitch;
// 	}
// 	m_pTexFFTInputX->UnlockRect(0);

// 	m_pTexFFTInputZ->LockRect(0, &lockrect, NULL, 0);
// 	p = (BYTE*)lockrect.pBits;
// 	for(int i = 0; i < NY; i++)
// 	{
// 		float* pBits = (float*)p;
// 		for(int j = 0; j < NX; j++)
// 		{
// 			//random numbers between -1 and 1
// 			pBits[j * 4] =		m_pDisplaceFieldY[GetGridOffset(j,i)].real() * 10e5f;		//kx
// 			pBits[j * 4 + 1] =	m_pDisplaceFieldY[GetGridOffset(j,i)].imag() * 10e5f;		//ky
// 			pBits[j * 4 + 2] =	0;		//klen
// 			pBits[j * 4 + 3] = 1;
// 		}
// 		p += lockrect.Pitch;
// 	}
// 	m_pTexFFTInputZ->UnlockRect(0);

	ComputeFFT2D(-1, m_pHeightField);
	if( !bOnlyHeight )
	{
		ComputeFFT2D(-1, m_pDisplaceFieldX);
		ComputeFFT2D(-1, m_pDisplaceFieldY);
	}

	for(int y = 0; y<GRID_SIZE; ++y)
	{
		for(int x = 0; x<GRID_SIZE; ++x)
		{
			int offset = GetGridOffset(x, y);
			int currPowNeg1 = PowNeg1(x+y);

			m_pHeightField[offset]*= currPowNeg1*m_fMaxWaveSize;
			if( !bOnlyHeight )
			{
				m_pDisplaceFieldX[offset] *= currPowNeg1 * m_fChoppyWaveScale;
				m_pDisplaceFieldY[offset] *= currPowNeg1 * m_fChoppyWaveScale;
			}

			m_pDisplaceGrid[offset] = A3DVECTOR4(
				m_pDisplaceFieldX[offset].real(), 
				m_pDisplaceFieldY[offset].real(),  
				-m_pHeightField[offset].real(), 
				0.0f); // store normal ?
		}
	}

}

void A3DWaterFFT::Create(float fG, float fA, float fWindDir, float fWorldSizeX, float fWorldSizeY ) /* Create/Initialize water simulation */
{
	m_fG = fG;
	m_fA = fA;
	m_fWindDir = fWindDir;
	m_fWorldSizeX = fWorldSizeX;
	m_fWorldSizeY = fWorldSizeY;
	m_fWorldSizeXInv = 1.f / m_fWorldSizeX;
	m_fWorldSizeYInv = 1.f / m_fWorldSizeY;
	InitTableK();
	InitFourierAmps();


	//创建并填充测试纹理
// 	dev->CreateTexture(NX, NY, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, 
// 		D3DPOOL_DEFAULT, &m_pTexTestInitK, NULL);
// 	dev->CreateTexture(NX, NY, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, 
// 		D3DPOOL_DEFAULT, &m_pTexTestInitH0, NULL);
// 	dev->CreateTexture(NX, NY, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, 
// 		D3DPOOL_DEFAULT, &m_pTexFFTInputH, NULL);
// 	dev->CreateTexture(NX, NY, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, 
// 		D3DPOOL_DEFAULT, &m_pTexFFTInputX, NULL);
// 	dev->CreateTexture(NX, NY, 1, D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, 
// 		D3DPOOL_DEFAULT, &m_pTexFFTInputZ, NULL);

// 	D3DLOCKED_RECT lockrect;
// 	m_pTexTestInitK->LockRect(0, &lockrect, NULL, 0);
// 	BYTE* p = (BYTE*)lockrect.pBits;
// 	for(int i = 0; i < NY; i++)
// 	{
// 		float* pBits = (float*)p;
// 		for(int j = 0; j < NX; j++)
// 		{
// 			//random numbers between -1 and 1
// 			pBits[j * 4] =		m_pLUTK[GetGridOffset(j,i)].x;		//kx
// 			pBits[j * 4 + 1] =	m_pLUTK[GetGridOffset(j,i)].y;		//ky
// 			pBits[j * 4 + 2] =	m_pLUTK[GetGridOffset(j,i)].z;		//klen
// 			pBits[j * 4 + 3] = 1;
// 		}
// 		p += lockrect.Pitch;
// 	}
// 	m_pTexTestInitK->UnlockRect(0);


	//ofstream fout;
	//fout.open("CryWaterH0.txt");

// 	m_pTexTestInitH0->LockRect(0, &lockrect, NULL, 0);
// 	p = (BYTE*)lockrect.pBits;
// 	for(int i = 0; i < NY; i++)
// 	{
// 		float* pBits = (float*)p;
// 		for(int j = 0; j < NX; j++)
// 		{
// 			//random numbers between -1 and 1
// 			pBits[j * 4] =		m_pFourierAmps[GetGridOffset(j,i)].real() * 10e5f;
// 			pBits[j * 4 + 1] =	m_pFourierAmps[GetGridOffset(j,i)].imag() * 10e5f;
// 			pBits[j * 4 + 2] =	0;
// 			pBits[j * 4 + 3] = 1;
// 			//fout << pBits[j * 4] << "  " << pBits[j * 4 + 1] << endl;
// 		}
// 		p += lockrect.Pitch;
// 	}
// 	m_pTexTestInitH0->UnlockRect(0);

	//fout.close();
	//D3DXSaveTextureToFileA("CryWaterH0.dds", D3DXIFF_DDS, m_pTexTestInitH0, NULL);
}

void A3DWaterFFT::Release()
{
	unsigned int nSize = GRID_SIZE * GRID_SIZE * sizeof(std::complex<float>);
	memset(&m_pFourierAmps[0], 0, nSize );
	memset(&m_pHeightField[0], 0, nSize );
	memset(&m_pDisplaceFieldX[0], 0, nSize );
	memset(&m_pDisplaceFieldY[0], 0, nSize );

	nSize = GRID_SIZE * GRID_SIZE * sizeof(A3DVECTOR4);
	memset(m_pDisplaceGrid, 0, nSize );
	memset(m_pLUTK, 0, nSize );
}








