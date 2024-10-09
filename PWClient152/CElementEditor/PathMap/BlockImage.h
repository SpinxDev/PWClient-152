 /*
 * FILE: BlockImage.h
 *
 * DESCRIPTION:   A  class to realize a model to save, load and 
 *							quickly look up a sparse large-size image.
 *							The basic idea is divide the original image into set of blocks!
 *
 * CREATED BY: He wenfeng, 2005/3/31
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _BLOCKIMAGE_H_
#define _BLOCKIMAGE_H_

#include <stdio.h>
#include "vector.h"
//using namespace abase;

//#include <vector>
//using namespace std;

// Define a 16bits (10.6)  fix dot number type 
typedef unsigned short FIX16;

#define FLOATTOINT(x) ((int) floor((x)+0.5f))						// 四舍五入

#define FLOATTOFIX16(x) (FIX16)FLOATTOINT(x*64.0f)
#define FIX16TOFLOAT(x) ( (x)/64.0f )		

#define NULL_ID -1

// Version Info (DWORD)

// Current Version
#define BLOCKIMAGE_VER 0x00000001	

// Old Version


template<class T>
class CBlockImage
{

public:
	
	CBlockImage() { m_BlockIDs=NULL; SetBlockSizeExp(4); }			// default size is set to 16*16，使用较小块利于减小存储空间
	virtual ~CBlockImage() { Release(); }
	
	void SetBlockSizeExp(int iExp) 
	{ 
		m_iBlockSizeExp=iExp;
		m_iBlockSize=1 << iExp;
	}
	
	void Release()
	{
		if(m_BlockIDs) 
		{
			delete [] m_BlockIDs;
			m_BlockIDs=NULL;
		}
		
		for(int i=0;i<m_arrBlocks.size();i++)
			delete m_arrBlocks[i];
		
		m_arrBlocks.clear();
	}

	T GetPixel(int u, int v) const
	{
		int uBlkID=u>>m_iBlockSizeExp;
		int vBlkID=v>>m_iBlockSizeExp;
		int BlkID=m_BlockIDs[vBlkID*m_iWidth+uBlkID];
		if(BlkID==NULL_ID)
		{
			T tZero;
			memset(&tZero, 0, sizeof(T));
			return tZero;
		}
		
		int uBlkOffset= u- (uBlkID<<m_iBlockSizeExp);
		int vBlkOffset= v- (vBlkID<<m_iBlockSizeExp);
		
		return m_arrBlocks[BlkID][vBlkOffset*m_iBlockSize+uBlkOffset];
	}

	void Init(T* pImage, int iWidth, int iLength, float fPixelSize = 1.0f);
	
	void GetImageSize( int& width, int& length) const
	{
		width = m_iImageWidth;
		length = m_iImageLength;
	}
	
	float GetPixelSize() const
	{
		return m_fPixelSize;
	}

	// Load and Save, using FILE
	bool Save( FILE *pFileToSave );
	bool Load( FILE *pFileToLoad );
	bool Load(const char *szFile);
	bool Save(const char *szFile);

protected:
	abase::vector<T* > m_arrBlocks;
	int* m_BlockIDs;
	int m_iBlockSize;				// the size of the block, only the width or length, while not the width*length
	int m_iBlockSizeExp;		  // 以2为底的m_iBlockSize的指数
	int m_iWidth;					  // blocks in width
	int m_iLength;					  // blocks in length

	float m_fPixelSize;					// Each pixel of the image is a square area
	int m_iImageWidth;				// Image width ( in pixels)
	int m_iImageLength;				// Image length ( in pixels)

};

// typedef CBlockImage<FIX16> FIX16BlockImage;

template<class T>
void CBlockImage<T>::Init(T* pImage, int iWidth, int iLength,float fPixelSize)
{
	m_iImageWidth=iWidth;
	m_iImageLength=iLength;
	m_fPixelSize=fPixelSize;

	m_iWidth=iWidth >> m_iBlockSizeExp;
	int iLastBlkWidth=iWidth - (m_iWidth << m_iBlockSizeExp);
	if(iLastBlkWidth!=0)
		m_iWidth++;
	else
		iLastBlkWidth=m_iBlockSize;		

	m_iLength=iLength >> m_iBlockSizeExp;
	int  iLastBlkLength= iLength - (m_iLength << m_iBlockSizeExp) ;
	if(iLastBlkLength!=0)	
		m_iLength++;
	else
		iLastBlkLength=m_iBlockSize;

	m_BlockIDs= new int[m_iWidth*m_iLength];
	int iPixelsNumInBlock=m_iBlockSize*m_iBlockSize;
	T* ZeroBlock=new T[iPixelsNumInBlock];
	memset(ZeroBlock, 0, iPixelsNumInBlock* sizeof(T));

	int iCurCpyWidth, iCurCpyLength;
	for(int i=0; i < m_iLength; i++)
		for(int j=0; j < m_iWidth; j++)
		{
			iCurCpyLength=(i==m_iLength-1)?iLastBlkLength:m_iBlockSize;
			iCurCpyWidth=(j==m_iWidth-1)?iLastBlkWidth:m_iBlockSize;
			T* Block=new T[iPixelsNumInBlock];
			memset(Block, 0, iPixelsNumInBlock* sizeof(T));
			
			// copy the data to the block
			int iStartCpyPos= ( i * iWidth + j ) * m_iBlockSize;	// In fact it is i*m_iBlockSize*iWidth + j* m_iBlockSize;
			int iDestCpyPos=0;
			for(int l=0; l<iCurCpyLength; l++)
			{
				memcpy(Block+iDestCpyPos, pImage+iStartCpyPos, iCurCpyWidth*sizeof(T));
				iStartCpyPos+= iWidth;
				iDestCpyPos+=m_iBlockSize;
			}

			if(memcmp(ZeroBlock, Block, iPixelsNumInBlock* sizeof(T)))
			{
				// not identical
				m_arrBlocks.push_back(Block);
				m_BlockIDs[i*m_iWidth+j]=m_arrBlocks.size()-1;
			}
			else
			{
				// identical which means the Block is a Zero Block
				delete [] Block;
				m_BlockIDs[i*m_iWidth+j]=NULL_ID;
			}
		}

	delete [] ZeroBlock;
		
}

template<class T>
bool CBlockImage<T>::Save(const char *szFile)
{
	FILE* sFile = fopen(szFile,"wb");
	if (sFile != NULL){
		bool bSave = Save(sFile);
		fclose(sFile);
		return bSave;
	}
	return false;
}

template<class T>
bool CBlockImage<T>::Save( FILE *pFileToSave )
{
	if(!pFileToSave) return false;
	
	DWORD dwWrite, dwWriteLen;
	
	// write the Version info
	dwWrite=BLOCKIMAGE_VER;
	dwWriteLen = fwrite(&dwWrite, 1, sizeof(DWORD), pFileToSave);
	if(dwWriteLen != sizeof(DWORD))
		return false;

	// write the buf size
	int BlkIDSize= m_iWidth*m_iLength*sizeof(int);
	int BlkSize=m_iBlockSize*m_iBlockSize*sizeof(T);
	DWORD BufSize= 5*sizeof(int)+sizeof(float) + BlkIDSize+sizeof(int)+m_arrBlocks.size()*BlkSize;
	dwWrite= BufSize;
	dwWriteLen = fwrite(&dwWrite, 1, sizeof(DWORD), pFileToSave);
	if(dwWriteLen != sizeof(DWORD))
		return false;

	// write the following info in order
	// 1. m_iWidth
	// 2. m_iLength
	// 3. m_iBlockSizeExp
	// 4. m_iImageWidth
	// 5. m_iImageLength
	// 6. m_fPixelSize
	// 7. data in m_BlockIDs
	// 8. size of m_arrBlocks
	// 9. data in m_arrBlocks
	
	UCHAR *buf=new UCHAR[BufSize];
	int cur=0;
	* (int *) (buf+cur) = m_iWidth;
	cur+=sizeof(int);

	* (int *) (buf+cur) = m_iLength;
	cur+=sizeof(int);	

	* (int *) (buf+cur) = m_iBlockSizeExp;
	cur+=sizeof(int);	

	* (int *) (buf+cur) = m_iImageWidth;
	cur+=sizeof(int);	

	* (int *) (buf+cur) = m_iImageLength;
	cur+=sizeof(int);	

	* (float *) (buf+cur) = m_fPixelSize;
	cur+=sizeof(float);	

	memcpy(buf+cur, m_BlockIDs, BlkIDSize);
	cur+=BlkIDSize;
	
	* (int *) (buf+cur) = m_arrBlocks.size();
	cur+=sizeof(int);

	for(int i=0; i<m_arrBlocks.size(); i++)
	{
		memcpy(buf+cur, m_arrBlocks[i], BlkSize);
		cur+=BlkSize;
	}

	dwWriteLen = fwrite(buf, 1, BufSize, pFileToSave);
	if(dwWriteLen != BufSize)
	{
		delete [] buf;
		return false;
	}	

	delete [] buf;
	return true;
}

template<class T>
bool CBlockImage<T>::Load(const char *szFile)
{
	FILE* sFile = fopen(szFile,"rb");
	if (sFile != NULL){
		bool bLoad = Load(sFile);
		fclose(sFile);
		return bLoad;
	}
	return false;
}

template<class T>
bool CBlockImage<T>::Load( FILE *pFileToLoad )
{
	if(!pFileToLoad) return false;
	
	DWORD dwRead, dwReadLen;

	// Read the Version
	dwReadLen = fread(&dwRead, 1, sizeof(DWORD), pFileToLoad);
	if(dwReadLen != sizeof(DWORD))
		return false;

	if(dwRead==BLOCKIMAGE_VER)
	{
		// Current Version
		// Read the buf size
		dwReadLen = fread(&dwRead, 1, sizeof(DWORD), pFileToLoad);
		if(dwReadLen != sizeof(DWORD))
			return false;

		int BufSize=dwRead;
		UCHAR * buf = new UCHAR[BufSize];

		// Read the data
		dwReadLen = fread(buf, 1, BufSize, pFileToLoad);
		if( dwReadLen != BufSize )
		{
			delete [] buf;
			return false;
		}
		
		Release();			// Release the old data
		
		int cur=0;
		m_iWidth=* (int *) (buf+cur);
		cur+=sizeof(int);
		m_iLength=* (int *) (buf+cur);
		cur+=sizeof(int);
		m_iBlockSizeExp=* (int *) (buf+cur);
		cur+=sizeof(int);
		m_iBlockSize = 1<<m_iBlockSizeExp;
		m_iImageWidth=* (int *) (buf+cur);
		cur+=sizeof(int);
		m_iImageLength=* (int *) (buf+cur);
		cur+=sizeof(int);
		m_fPixelSize=* (float *) (buf+cur);
		cur+=sizeof(float);
		
		int BlkIDSize= m_iWidth*m_iLength*sizeof(int);
		int BlkSize=m_iBlockSize*m_iBlockSize*sizeof(T);
		
		m_BlockIDs=(int *)(new UCHAR[BlkIDSize]);

		memcpy(m_BlockIDs, buf+cur, BlkIDSize);
		cur+=BlkIDSize;

		int arrBlkSize=* (int *) (buf+cur);
		cur+=sizeof(int);

		for(int i=0; i<arrBlkSize; i++)
		{
			T* pTBlock= (T *) (new UCHAR[BlkSize]);
			memcpy(pTBlock, buf+cur, BlkSize);
			m_arrBlocks.push_back(pTBlock);
			cur+=BlkSize;
		}
		delete [] buf;
		return true;
	}
	else
		return false;
	
}

#endif