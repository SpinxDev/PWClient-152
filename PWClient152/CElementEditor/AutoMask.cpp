#include "stdafx.h"
#include <MATH.H>
#include "AutoMask.h"


inline bool GetPixelPara(float* pHeightMap, A3DVECTOR3* aNormals, int N,
						 int posx, int posz, float sx, float sz,
						 float& height, float& slope, int& axis)
{
	float ha, hb, hc;
	A3DVECTOR3 dir;

	// 1st position in heightmap
	int pos = posz*N+posx;
	
	// calculate pixel's height & slope
	height = 0.0f;
	slope = 0.0f;

	hb = pHeightMap[pos+1];
	hc = pHeightMap[pos+N];

	A3DVECTOR3& dirb = aNormals[pos+1];
	A3DVECTOR3& dirc = aNormals[pos+N];

	if(sx+sz<1)
	{
		ha = pHeightMap[pos];
		height = ha+sx*(hb-ha)+sz*(hc-ha);

		A3DVECTOR3& dira = aNormals[pos];
		dir = (1-sx-sz)*dira+sx*dirb+sz*dirc;
	}
	else
	{
		ha = pHeightMap[pos+N+1];
		height = ha+(1-sx)*(hb-ha)+(1-sz)*(hc-ha);

		A3DVECTOR3& dira = aNormals[pos+N+1];
		sx = 1-sx;
		sz = 1-sz;
		dir = (1-sx-sz)*dira+sx*dirb+sz*dirc;
	}

	slope = (float)(1.0-dir.y);

	// calculate pixel's axis direction
	axis = 0;
	if(dir.x>0.577f || dir.x<-0.577f)
		axis |= AXIS_X;
	if(dir.y>0.577f || dir.y<-0.577f)
		axis |= AXIS_Y;
	if(dir.z>0.577f || dir.z<-0.577f)
		axis |= AXIS_Z;

	return true;

}

bool GenerateMask(int N, float* pHeightMap, A3DVECTOR3* aNormals, float fCellSize,
				  float fAltitudeStart, float fAltitudeEnd,
				  float fSlopeStart, float fSlopeEnd,
				  int axisUsed, int M, BYTE* pMaskMap,
				  float fSmoothRange)
{
	A3DVECTOR3 pt;

	BYTE* pOldMask = pMaskMap;

	float delta = (float)(N-1)/(float)M;
	int i, j;
	
	float x=0, z=0;
	int coordx, coordz;
	float sx, sz;

	float height, slope;
	int axisdir;
	
	fSlopeStart = (float)sin(fSlopeStart);
	fSlopeEnd = (float)sin(fSlopeEnd);
	fSlopeStart *= fSlopeStart;
	fSlopeEnd *= fSlopeEnd;
	
	z = 0.5f*delta;
	for(j=0; j<M; j++)
	{
		x = 0.5f*delta;
		for(i=0; i<M; i++)
		{
			x += delta;

			coordx = (int)x;
			coordz = (int)z;

			sx = x-coordx;
			sz = z-coordz;
	
			height = 0.0f;
			slope = 0.0f;
			axisdir = 0;
			GetPixelPara(pHeightMap, aNormals, N, coordx, coordz, sx, sz,
							height, slope, axisdir);

			if(height>=fAltitudeStart && height<=fAltitudeEnd
				&& slope>=fSlopeStart && slope<=fSlopeEnd)
			{
				if(axisdir & axisUsed)
					*pMaskMap = 255;
				else
					*pMaskMap = 0;
			}
			else
				*pMaskMap = 0;

			pMaskMap++;

		}
		z += delta;

	}
	
	//////////////////////////////////////////////////////////////////////////
	pMaskMap = pOldMask;
	if(fSmoothRange != 0.0)
		GaussianBlur(pMaskMap, M, fSmoothRange);
	
	return true;
}

static int build_gaussian(double *output, double size)
{
    double	sigma2;
    double	l;
    int		length, n, i;

    sigma2 = -(size + 1) * (size + 1) / log(1.0 / 255.0);
    l = size;

    n = int(ceil(l) * 2.0);
    if( (n % 2) == 0 ) n++;
	length = n / 2;

    for(i=0; i<=length; i++)
	    output[i] = exp(-(i * i) / sigma2);
    
    return length;
}

void GaussianBlur(BYTE * pImage, int nSize, double vBlurRange)
{
	BYTE * pLine = new BYTE[nSize];

	double gaussian[32]; // max blur radius is 32 pixel

	// first setup blur factors
	int		gwidth, kwidth;
	gwidth = build_gaussian(gaussian, vBlurRange);
    kwidth = gwidth * 2 + 1;

	int i, j, k;
	double total = 0.0;
	// now normalize these factors
	for(i=0; i<kwidth; i++) 
	{
        int xpos = abs(i - gwidth);
        total += gaussian[xpos];
    }

	for(i=0; i<=gwidth; i++)
		gaussian[i] /= total;
	
	double	result;
	int		index;
	BYTE	value;

	int index1, index2;

	// first do horizontal blur and put into a temp line buffer first, then copy it to result
	for(i=0; i<nSize; i++) 
	{
		for(j=0; j<gwidth; j++)
		{
			result = 0.0;
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				if( j + k - gwidth < 0 )
					value = pImage[i * nSize];
				else
					value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = (BYTE)result;
		}
		for(j=gwidth; j<nSize-gwidth; j++)
		{
			result = 0.0;
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = (BYTE)result;
        }
		for(j=nSize-gwidth; j<nSize; j++)
		{
			result = 0.0;
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				if( j + k - gwidth >= nSize )
					value = pImage[i * nSize + nSize - 1];
				else
					value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = (BYTE)result;
		}
		memcpy(pImage + i * nSize, pLine, nSize);
    }

	// then transpose the image to make cache works
	for(i=0; i<nSize; i++)
	{
		index1 = i * nSize;
		index2 = i;
		for(j=0; j<i; j++)
		{
			BYTE temp = pImage[index1];
			pImage[index1] = pImage[index2];
			pImage[index2] = temp;
			index1 ++;
			index2 += nSize;
		}
	}

	// then do blur horizontal again.
	for(i=0; i<nSize; i++) 
	{
		for(j=0; j<gwidth; j++)
		{
			result = 0.0;
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				if( j + k - gwidth < 0 )
					value = pImage[i * nSize];
				else
					value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = (BYTE)result;
		}
		for(j=gwidth; j<nSize-gwidth; j++)
		{
			result = 0.0;
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = (BYTE)result;
        }
		for(j=nSize-gwidth; j<nSize; j++)
		{
			result = 0.0;
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				if( j + k - gwidth >= nSize )
					value = pImage[i * nSize + nSize - 1];
				else
					value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = (BYTE)result;
		}
		memcpy(pImage + i * nSize, pLine, nSize);
    }

	// then transpose the image back
	for(i=0; i<nSize; i++)
	{
		index1 = i * nSize;
		index2 = i;
		for(j=0; j<i; j++)
		{
			BYTE temp = pImage[index1];
			pImage[index1] = pImage[index2];
			pImage[index2] = temp;
			index1 ++;
			index2 += nSize;
		}
	}

	delete [] pLine;
}