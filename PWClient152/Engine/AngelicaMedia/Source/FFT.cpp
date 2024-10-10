// FFT.cpp: implementation of the CFFT class.
//
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <math.h>
#include "FFT.h"
#include "AMemory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFFT::CFFT()
{
	m_N = 0;
	m_M = 0;
	m_pBuffer = NULL;
	m_pW = NULL;
	m_pWInverse = NULL;
	m_pInverseIndex = NULL;
}

CFFT::~CFFT()
{

}

bool CFFT::Init(int nLength)
{
	m_N = 1;	
	m_M = 0;
	while( nLength > m_N )
	{
		m_N *= 2;
		m_M ++;
	}

	m_pBuffer = (PCOMPLEX)a_malloc( sizeof(COMPLEX) * m_N );
	if( NULL == m_pBuffer )
		return false;
	ZeroMemory( m_pBuffer, sizeof(COMPLEX) * m_N );

	m_pW = (PCOMPLEX) a_malloc( sizeof(COMPLEX) * m_N / 2 * m_M );
	if( NULL == m_pW )
		return false;
	ZeroMemory( m_pW, sizeof(COMPLEX) * m_N / 2 * m_M );

	m_pWInverse = (PCOMPLEX) a_malloc( sizeof(COMPLEX) * m_N / 2 * m_M );
	if( NULL == m_pWInverse )
		return false;
	ZeroMemory( m_pWInverse, sizeof(COMPLEX) * m_N / 2 * m_M );

	//Initialize W table here;
	//First, the last line;
	for(int k=0; k<m_N/2; k++)
	{
		//vDelta;
		float vDelta = -3.14159265f * 2.0f / m_N * k; // -2PI/N * k
		m_pW[ (m_M - 1) * m_N / 2 + k ].a = (float) cos(vDelta);
		m_pW[ (m_M - 1) * m_N / 2 + k ].b = (float) sin(vDelta);
		m_pWInverse[ (m_M - 1) * m_N / 2 + k ].a = (float) cos(-vDelta);
		m_pWInverse[ (m_M - 1) * m_N / 2 + k ].b = (float) sin(-vDelta); 
	}
	//Then, other lines;
	int nLevelUnitNum = m_N / 2;
	for(int nLevel=m_M-2; nLevel>=0; nLevel--)
	{
		nLevelUnitNum /= 2;
		for(int nNum=0; nNum<nLevelUnitNum; nNum++)
		{
			m_pW[nLevel * m_N / 2 + nNum] =  
				m_pW[(nLevel + 1) * m_N / 2 + 2 * nNum];
			m_pWInverse[nLevel * m_N / 2 + nNum] =  
				m_pWInverse[(nLevel + 1) * m_N / 2 + 2 * nNum];
		}
	}

	//Inverse index;
	m_pInverseIndex = (int *)a_malloc( sizeof(int) * m_N );
	if( NULL == m_pInverseIndex )
		return false;
	int	nInverseBits;
	int	nBits[32];
	int nBit;
	for(int n=0; n<m_N; n++)
	{
		for(nBit=0; nBit<m_M; nBit++)
			nBits[nBit] = (n >> nBit) & 0x1;
		nInverseBits = 0;
		for(nBit=0; nBit<m_M; nBit++)
			nInverseBits |= nBits[m_M - 1 - nBit] << nBit;
		m_pInverseIndex[n] = nInverseBits;
	}

	return true;
}

bool CFFT::Release()
{
	if( m_pBuffer )
	{
		a_free( m_pBuffer );
		m_pBuffer = NULL;
	}
	if( m_pW )
	{
		a_free( m_pW );
		m_pW = NULL;
	}
	if( m_pWInverse )
	{
		a_free( m_pWInverse );
		m_pWInverse = NULL;
	}
	if( m_pInverseIndex )
	{
		a_free( m_pInverseIndex );
		m_pInverseIndex = NULL;
	}
	return true;
}
/*
bool CFFT::FFT()
{
	COMPLEX res;
	COMPLEX temp;
	COMPLEX	W;
	COMPLEX vFuck[4096];
	memcpy( vFuck, m_pBuffer, sizeof(COMPLEX) * m_N );
	float vAlpha;
	for( int n=0; n<m_N; n++ )
	{
		res.a = res.b = 0.0f;
		for( int m=0; m<m_N; m++ )
		{
			vAlpha = -3.14159265f * 2.0f / m_N * m * n;
			W.a = cos( vAlpha );
			W.b = sin( vAlpha );
			temp = CMul(vFuck[m], W);
			res.a += temp.a;
			res.b += temp.b;
		}
		m_pBuffer[n].a = res.a;
		m_pBuffer[n].b = res.b;
	}

	return true;
} */
bool CFFT::FFT()
{
	int		nRelateUnitGap = 1;
	int		nGroupNums = 0;
	int		nGroupUnitNums = 0;
	int		nUnitNum;
	COMPLEX	mul;
	COMPLEX	keep;

	for(int nLevel=0; nLevel<m_M; nLevel++)
	{
		nGroupUnitNums = nRelateUnitGap * 2;
		nGroupNums = m_N / nGroupUnitNums;
		for(int nGroup=0; nGroup<nGroupNums; nGroup++)
		{
			for(int nUnit=0; nUnit<nGroupUnitNums / 2; nUnit++)
			{
				//Butterfly calculation;
				//Leftup corner unit no.;
				nUnitNum = nGroup * nGroupUnitNums + nUnit;
				keep = m_pBuffer[nUnitNum];
				//Multiplication;
				//Left bottom corner mulitply with W;
				mul = CMul(m_pBuffer[nUnitNum + nRelateUnitGap],
					m_pW[nLevel * m_N / 2 + nUnit]);
				//Add to right up;
				m_pBuffer[nUnitNum].a = keep.a + mul.a;
				m_pBuffer[nUnitNum].b = keep.b + mul.b;
				//-W;
				mul.a *= -1.0f;
				mul.b *= -1.0f;
				//Add to right bottom;
				m_pBuffer[nUnitNum + nRelateUnitGap].a = keep.a + mul.a;
				m_pBuffer[nUnitNum + nRelateUnitGap].b = keep.b + mul.b;
			}
		}
		nRelateUnitGap *= 2;
	}
	return true;
}
							  
bool CFFT::SetBuffer(float *pInBuffer, int nBufferUnitNum)
{
	int n;
	for(n=0; n<m_N; n++ )
	{
		m_pBuffer[n].a = pInBuffer[m_pInverseIndex[n]];
		m_pBuffer[n].b = 0.0f;
	}

	return true;
}

bool CFFT::GetBuffer(float *pOutBuffer, int nBufferUnitNum)
{
	double s;
	for( int n=0; n<nBufferUnitNum; n++ )
	{
		s = sqrt(m_pBuffer[n].a * m_pBuffer[n].a +
				m_pBuffer[n].b * m_pBuffer[n].b);
		if( s > 0 )
		{
			pOutBuffer[n] = (float)log(s);
			if( pOutBuffer[n] < 0 )
				pOutBuffer[n] = 0.0f;		
		}
		else pOutBuffer[n] = 0.0f;
	}

	return true;
}

bool CFFT::IFFT()
{
	int		nRelateUnitGap = 1;
	int		nGroupNums = 0;
	int		nGroupUnitNums = 0;
	int		nUnitNum;
	COMPLEX	mul;
	COMPLEX	keep;

	for(int nLevel=0; nLevel<m_M; nLevel++)
	{
		nGroupUnitNums = nRelateUnitGap * 2;
		nGroupNums = m_N / nGroupUnitNums;
		for(int nGroup=0; nGroup<nGroupNums; nGroup++)
		{
			for(int nUnit=0; nUnit<nGroupUnitNums / 2; nUnit++)
			{
				//Butterfly calculation;
				//Leftup corner unit no.;
				nUnitNum = nGroup * nGroupUnitNums + nUnit;
				keep = m_pBuffer[nUnitNum];
				//Multiplication;
				//Left bottom corner mulitply with W;
				mul = CMul(m_pBuffer[nUnitNum + nRelateUnitGap],
					m_pWInverse[nLevel * m_N / 2 + nUnit]);
				//Add to right up;
				m_pBuffer[nUnitNum].a = keep.a + mul.a;
				m_pBuffer[nUnitNum].b = keep.b + mul.b;
				//-W;
				mul.a *= -1.0f;
				mul.b *= -1.0f;
				//Add to right bottom;
				m_pBuffer[nUnitNum + nRelateUnitGap].a = keep.a + mul.a;
				m_pBuffer[nUnitNum + nRelateUnitGap].b = keep.b + mul.b;
				m_pBuffer[nUnitNum + nRelateUnitGap].a *= 1.0f / m_N;
				m_pBuffer[nUnitNum + nRelateUnitGap].b *= 1.0f / m_N;
			}
		}
		nRelateUnitGap *= 2;
	}
	return true;
}

bool CFFT::GetComplexBuffer(COMPLEX *pBuffer)
{
	for(int n=0; n<m_N; n++)
	{
		pBuffer[n].a = m_pBuffer[n].a;
		pBuffer[n].b = m_pBuffer[n].b;
	}
	return true;
}

bool CFFT::SetComplexBuffer(COMPLEX *pBuffer)
{
	int n;
	for(n=0; n<m_N; n++ )
	{
		m_pBuffer[n].a = pBuffer[m_pInverseIndex[n]].a;
		m_pBuffer[n].b = pBuffer[m_pInverseIndex[n]].b;
	}

	return true;
}
 
