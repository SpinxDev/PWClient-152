/*
 * FILE: PerlinNoise2D.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Jiang Dalong, 2005/1/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "PerlinNoise2D.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPerlinNoise2D::CPerlinNoise2D()
{
	m_pNoise = NULL;
	m_bHasGetMaxMin = false;
}

CPerlinNoise2D::~CPerlinNoise2D()
{
	Release();
}

/*
 * Spline(...)
 *
 *	interpolate a 1d spline
 *	taken from _Texturing & Modeling: A Procedural Approach_
 *		chapter 2 - by Darwyn Peachey
 */
float CPerlinNoise2D::Spline( float x, int nknots, float *knot )
{
	// basis matrix for spline interpolation
	float CR00 =	-0.5;
	float CR01 = 	1.5;
	float CR02 =	-1.5;
	float CR03 =	0.5;
	float CR10 =	1.0;
	float CR11 =	-2.5;
	float CR12 =	2.0;
	float CR13 =	-0.5;
	float CR20 =	-0.5;
	float CR21 =	0.0;
	float CR22 =	0.5;
	float CR23 =	0.0;
	float CR30 =	0.0;
	float CR31 =	1.0;
	float CR32 =	0.0;
	float CR33 =	0.0;
	
	int span;
	int nspans = nknots - 3;
	float c0, c1, c2, c3;	// coefficients of the cubic

	if( nspans < 1)
	{
		// illegal
		return 0;
	}

	// find the appropriate 4-point span of the spline
	x = (x < 0 ? 0 : (x > 1 ? 1 : x)) * nspans;
	span = (int) x;
	if( span >= nknots -3 )
		span = nknots - 3;
	x -= span;
	knot += span;

	// evaluate the span cubic at x using horner's rule
	c3 = CR00*knot[0] + CR01*knot[1]
		+ CR02*knot[2] + CR03*knot[3];
	c2 = CR10*knot[0] + CR11*knot[1]
		+ CR12*knot[2] + CR13*knot[3];
	c1 = CR20*knot[0] + CR21*knot[1]
		+ CR22*knot[2] + CR23*knot[3];
	c0 = CR30*knot[0] + CR31*knot[1]
		+ CR32*knot[2] + CR33*knot[3];

	return ((c3*x + c2)*x + c1)*x + c0;
}


/*
 * FracSynthPass(...)
 *
 *	generate basic points
 *	interpolate along spline & scale
 *	add to existing hbuffer
 */
void CPerlinNoise2D::FracSynthPass( float *hbuf, float freq, float zscale, int xres, int zres )
{

	int i;
	int x, z;
	float *val;
	int max;
	float dfx, dfz;
	float *zknots, *knots;
	float xk, zk;
	float *hlist;
	float *buf;

	if (freq <= 1) freq = 2.f;	// Add by jdl 2006.05.15. To ensure the result is valid.

	// how many to generate (need 4 extra for smooth 2d spline interpolation)
	max = int(freq + 2 + 0.5);
	// delta x and z - pixels per spline segment
	dfx = xres / (freq-1);
	dfz = zres / (freq-1);

	// the generated values - to be equally spread across buf
	val = (float*)calloc( sizeof(float)*max*max, 1 );
	// intermediately calculated spline knots (for 2d)
	zknots = (float*)calloc( sizeof(float)*max, 1 );
	// horizontal lines through knots
	hlist = (float*)calloc( sizeof(float)*max*xres, 1 );
	// local buffer - to be added to hbuf
	buf = (float*)calloc( sizeof(float)*xres*zres, 1 );

	// start at -dfx, -dfz - generate knots
	for( z=0; z<max; z++ )
	{
		for( x=0;x<max;x++ )
		{
			if (m_bUpZero)
				val[z*max+x] = MRANDOM;
			else
				val[z*max+x] = SRANDOM;
		}
	}

	// interpolate horizontal lines through knots
	for( i=0;i<max;i++ )
	{
		knots = &val[i*max];
		xk = 0;
		for( x=0;x<xres;x++ )
		{
			hlist[i*xres+x] = Spline( xk/dfx, 4, knots );
			xk += 1;
			if( xk >= dfx )
			{
				xk -= dfx;
				knots++;
			}
		}
	}

	// interpolate all vertical lines
	for( x=0;x<xres;x++ )
	{
		zk = 0;
		knots = zknots;
		// build knot list
		for( i=0;i<max;i++ )
		{
			knots[i] = hlist[i*xres+x];
		}
		for( z=0;z<zres;z++ )
		{
			buf[z*xres+x] = Spline( zk/dfz, 4, knots ) * zscale;
			zk += 1;
			if( zk >= dfz )
			{
				zk -= dfz;
				knots++;
			}
		}
	}

	// update hbuf
	for( z=0;z<zres;z++ )
		for( x=0;x<xres;x++ )
			hbuf[z*xres+x] += buf[z*xres+x];

	free( val );
	free( buf );
	free( hlist );
	free( zknots );

}


/*
 * Init(...)
 *
 *	set remaining values
 *	use fracSynth Pass to generate height values
 *	create arrays for glDrawElements
 */
bool CPerlinNoise2D::Init(int nWidth, int nHeight, float fAmp, float fFre, float fPer, int nPass, int seed, bool bUpZero)
{
	// Save parameters
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_fAmplitude = fAmp;
	m_fFrequency = (float)(int)fFre;
	m_fPersistence = fPer;
	m_nPass = nPass;
	m_bUpZero = bUpZero;
	if (seed>=0)
	{
		m_nSeed = seed;
		srand( seed );
	}

	Release();
	m_pNoise = (float*)malloc(nWidth * nHeight * sizeof(float));
	if (NULL == m_pNoise)
		return false;
	memset(m_pNoise, 0, nWidth*nHeight*sizeof(float));

	// generate height values
	for(int i=0;i<nPass;i++ )
	{
		FracSynthPass( m_pNoise, fFre, fAmp, nWidth, nHeight );
		fFre *= 2;
		fAmp *= fPer;
	}

	m_bHasGetMaxMin = false;

	return true;
}

void CPerlinNoise2D::Release()
{
	if (NULL != m_pNoise)
	{
		free(m_pNoise);
		m_pNoise = NULL;
	}
}

// Get a point's value
float CPerlinNoise2D::GetValue(int x, int y)
{
//	ASSERT(x>=0 && x<m_nWidth);
//	ASSERT(y>=0 && y<m_nHeight);

//	ASSERT(m_pNoise);

	return m_pNoise[y*m_nWidth+x];
}

// Get all noise value
bool CPerlinNoise2D::CopyAllNoise(float* pDst)
{
	if (NULL == pDst)
		return false;
	if (NULL == m_pNoise)
		return false;

	memcpy(pDst, m_pNoise, m_nWidth*m_nHeight*sizeof(float));

	return true;
}

void CPerlinNoise2D::GetMaxAndMinValue()
{
	m_fMinValue = m_pNoise[0];
	m_fMaxValue = m_pNoise[0];

	float fTemp;
	for (int i=0; i<m_nHeight; i++)
	{
		for (int j=0; j<m_nWidth; j++)
		{
			fTemp = m_pNoise[i*m_nWidth+j];
			if (fTemp < m_fMinValue)
				m_fMinValue = fTemp;
			if (fTemp > m_fMaxValue)
				m_fMaxValue = fTemp;
		}
	}
}

// Get max value
float CPerlinNoise2D::GetMaxValue()
{
	if (!m_bHasGetMaxMin)
	{
		GetMaxAndMinValue();
		m_bHasGetMaxMin = true;
	}

	return m_fMaxValue;
}

// Get min value
float CPerlinNoise2D::GetMinValue()
{
	if (!m_bHasGetMaxMin)
	{
		GetMaxAndMinValue();
		m_bHasGetMaxMin = true;
	}

	return m_fMinValue;
}