#ifndef _BLOCKIMAGE_H_
#define _BLOCKIMAGE_H_

#include <stdio.h>
#include <memory.h>

#include <vector.h>
#include <AFileImage.h>

using namespace abase;

// Define a 16bits (10.6)  fix dot number type 
typedef short FIX16;

#define AM_FLOATTOINT(x) ((int) floor((x)+0.5f))						// 四舍五入

#define AM_FLOATTOFIX16(x) (FIX16)AM_FLOATTOINT(x*64.0f)
#define AM_FIX16TOFLOAT(x) ( (x)/64.0f )		

#define NULL_ID -1

// Version Info (DWORD)

// Current Version
#define BLOCKIMAGE_VER 0x00000002	

// Old Version

// class laid under the namespace NPCMoveMap
namespace AutoMove
{

typedef unsigned char UCHAR;


template<class T>
class CBlockImage
{

public:
	
	CBlockImage(T* defaultVal = NULL, int iExp = 4) 
	{ 
		m_BlockIDs=NULL; 
		SetBlockSizeExp(iExp); // default size is set to 8*8
		if (defaultVal)
		{
			m_defaultVal = *defaultVal;
		}
		else
		{
			m_defaultVal = T(0);
		}
		m_pDefaultBlock=NULL;
	}			
	virtual ~CBlockImage() { Release(); }
	

	
	void Release()
	{
		if(m_BlockIDs) 
		{
			delete [] m_BlockIDs;
			m_BlockIDs=NULL;
		}
		
		for(DWORD i=0;i<m_arrBlocks.size();i++)
			delete m_arrBlocks[i];
		
		m_arrBlocks.swap(vector<T* >());
		if (m_pDefaultBlock) 
		{
			delete[] m_pDefaultBlock;
			m_pDefaultBlock = NULL;
		}
	}

	T GetPixel(int u, int v)
	{		
		if (u>=m_iImageWidth || u<0 || v>=m_iImageLength || v<0) 
		{
			assert(false);
			return m_defaultVal;
		}
		int uBlkID=u>>m_iBlockSizeExp;
		int vBlkID=v>>m_iBlockSizeExp;
		int BlkID = m_BlockIDs[vBlkID*m_iWidth+uBlkID];
		if(BlkID == NULL_ID)
		{
			return m_defaultVal;
		}
		
		//int uBlkOffset= u- (uBlkID<<m_iBlockSizeExp);
		//int vBlkOffset= v- (vBlkID<<m_iBlockSizeExp);
		int uBlkOffset= u & (m_iBlockSize -1);
		int vBlkOffset= v & (m_iBlockSize -1);
		
		//return m_arrBlocks[BlkID][vBlkOffset*m_iBlockSize+uBlkOffset];
		return m_arrBlocks[BlkID][(vBlkOffset<<m_iBlockSizeExp)+uBlkOffset];
	}

	bool SetPixel(int u, int v, T value);
	
	void Init(T* pImage, int iWidth, int iLength, float fPixelSize = 1.0f);
	inline void InitDafault(int iWidth, int iLength, float fPixelSize = 1.0f);
	void InitZero(int iWidth, int iLength, float fPixelSize = 1.0f)
	{
		SetDefaultValue(T(0));
		InitDafault( iWidth, iLength, fPixelSize);
	}

	void GetImageSize( int& width, int& length)
	{
		width = m_iImageWidth;
		length = m_iImageLength;
	}
	
	float GetPixelSize()
	{
		return m_fPixelSize;
	}

	T GetDefaultValue()
	{
		return m_defaultVal;
	}

	void SetDefaultValue(T defaultVal)
	{
		m_defaultVal = defaultVal;
		Release();
	}

	// Load and Save, using FILE
	bool Save( FILE *pFileToSave );
	bool Load( FILE *pFileToLoad );

	bool Load(const char * szName);
	bool Save(const char * szName);

protected:
    void DeleteBlock(int uBlkID,int vBlkID);
	T*   GetDefaultBlock();
	void SetBlockSizeExp(int iExp) 
	{ 
		m_iBlockSizeExp=iExp;
		m_iBlockSize=1 << iExp;
	}
protected:
	vector<T* > m_arrBlocks;
	int* m_BlockIDs;
	int m_iBlockSize;				// the size of the block, only the width or length, while not the width*length
	int m_iBlockSizeExp;		  // 以2为底的m_iBlockSize的指数
	int m_iWidth;					  // blocks in width
	int m_iLength;					  // blocks in length

	float m_fPixelSize;					// Each pixel of the image is a square area
	int m_iImageWidth;				// Image width ( in pixels)
	int m_iImageLength;				// Image length ( in pixels)

	T   m_defaultVal;

private:
	T*  m_pDefaultBlock;           //default block, like zero. only get this by function T* GetDefaultBlock();
};

// typedef CBlockImage<FIX16> FIX16BlockImage;

template<class T>
T* CBlockImage<T>::GetDefaultBlock()
{
	if (!m_pDefaultBlock) 
	{
		int iPixelsNumInBlock=m_iBlockSize*m_iBlockSize;
		T* Block=new T[iPixelsNumInBlock];
		for (int i=0;i<iPixelsNumInBlock;++i)
		{
			memcpy(Block+i,&m_defaultVal, sizeof(T));
		}
		m_pDefaultBlock = Block;
	}
	return m_pDefaultBlock;
}

template<class T>
bool CBlockImage<T>::SetPixel(int u, int v, T value)
{
	if (u>=m_iImageWidth || u<0 || v>=m_iImageLength || v<0) 
	{
		assert(false);
		return false;
	}
	int uBlkID=u>>m_iBlockSizeExp;
	int vBlkID=v>>m_iBlockSizeExp;
	int BlkID = m_BlockIDs[vBlkID*m_iWidth+uBlkID];
	if(BlkID == NULL_ID)
	{
		if (m_defaultVal !=value) 
		{
			int iPixelsNumInBlock=m_iBlockSize*m_iBlockSize;
			T* Block=new T[iPixelsNumInBlock];
			memcpy(Block,GetDefaultBlock(), iPixelsNumInBlock*sizeof(T));

			int uBlkOffset= u & (m_iBlockSize -1);
			int vBlkOffset= v & (m_iBlockSize -1);
			Block[(vBlkOffset<<m_iBlockSizeExp)+uBlkOffset] = value;

			m_arrBlocks.push_back(Block);
			m_BlockIDs[vBlkID*m_iWidth+uBlkID]=m_arrBlocks.size()-1;
		}
	}else
	{
		int uBlkOffset= u & (m_iBlockSize -1);
		int vBlkOffset= v & (m_iBlockSize -1);	
		m_arrBlocks[BlkID][(vBlkOffset<<m_iBlockSizeExp)+uBlkOffset] = value;
		if (value==m_defaultVal) 
		{
			int iPixelsNumInBlock=m_iBlockSize*m_iBlockSize;
			if(!memcmp(GetDefaultBlock(), m_arrBlocks[BlkID], iPixelsNumInBlock* sizeof(T)))
			{
				
				DeleteBlock(uBlkID,vBlkID);
			}
		}
	}
	return true;
}

template<class T>
void CBlockImage<T>::DeleteBlock(int uBlkID,int vBlkID)
{
	int BlkIndex = vBlkID*m_iWidth+uBlkID;
	int BlkID = m_BlockIDs[BlkIndex];

	int lastBlockID = m_arrBlocks.size()-1;
	if (NULL_ID==BlkID || BlkID<0 || BlkID>lastBlockID) 
	{
		return ;
	}

	if (BlkID == lastBlockID) 
	{
		T* nowBlock  = m_arrBlocks[BlkID];
		m_arrBlocks.pop_back();

		m_BlockIDs[BlkIndex] = NULL_ID;
		delete[] nowBlock;
	}else
	{
		int lastBlockIndex = -1;
		int nBlocks = m_iWidth*m_iLength;
		for (int i=0;i<nBlocks;++i) 
		{
			if (lastBlockID==m_BlockIDs[i]) 
			{
				lastBlockIndex = i;
				break;
			}
		}
		if (lastBlockIndex>=0) 
		{
			T* nowBlock  = m_arrBlocks[BlkID];

			m_arrBlocks[BlkID] = m_arrBlocks[lastBlockID];
			m_BlockIDs[lastBlockIndex] = BlkID;
			m_arrBlocks.pop_back();

            m_BlockIDs[BlkIndex] = NULL_ID;
			delete[] nowBlock;
		}else
		{
			//this should not happen
			assert(0);
		}
	}

}

template<class T>
void CBlockImage<T>::Init(T* pImage, int iWidth, int iLength,float fPixelSize)
{
	// Firstly, we release myself
	Release();

	m_iImageWidth=iWidth;
	m_iImageLength=iLength;
	m_fPixelSize=fPixelSize;

	m_iWidth=iWidth >> m_iBlockSizeExp;
	//int iLastBlkWidth=iWidth - (m_iWidth << m_iBlockSizeExp);
	int iLastBlkWidth=iWidth & (m_iBlockSize - 1);
	if(iLastBlkWidth!=0)
		m_iWidth++;
	else
		iLastBlkWidth=m_iBlockSize;		

	m_iLength=iLength >> m_iBlockSizeExp;
	//int  iLastBlkLength= iLength - (m_iLength << m_iBlockSizeExp) ;
	int  iLastBlkLength= iLength & (m_iBlockSize - 1);
	if(iLastBlkLength!=0)	
		m_iLength++;
	else
		iLastBlkLength=m_iBlockSize;

	m_BlockIDs= new int[m_iWidth*m_iLength];
	int iPixelsNumInBlock=m_iBlockSize*m_iBlockSize;

	int iCurCpyWidth, iCurCpyLength;
	for(int i=0; i < m_iLength; i++)
		for(int j=0; j < m_iWidth; j++)
		{
			iCurCpyLength=(i==m_iLength-1)?iLastBlkLength:m_iBlockSize;
			iCurCpyWidth=(j==m_iWidth-1)?iLastBlkWidth:m_iBlockSize;
			T* Block=new T[iPixelsNumInBlock];
			memcpy(Block, GetDefaultBlock(), iPixelsNumInBlock*sizeof(T));
			
			// copy the data to the block
			int iStartCpyPos= ( i * iWidth + j ) * m_iBlockSize;	// In fact it is i*m_iBlockSize*iWidth + j* m_iBlockSize;
			int iDestCpyPos=0;
			for(int l=0; l<iCurCpyLength; l++)
			{
				memcpy(Block+iDestCpyPos, pImage+iStartCpyPos, iCurCpyWidth*sizeof(T));
				iStartCpyPos+= iWidth;
				iDestCpyPos+=m_iBlockSize;
			}

			if(memcmp(GetDefaultBlock(), Block, iPixelsNumInBlock* sizeof(T)))
			{
				// not identical
				m_arrBlocks.push_back(Block);
				m_BlockIDs[i*m_iWidth+j]=m_arrBlocks.size()-1;
			}
			else
			{
				// identical which means the Block is a default Block
				delete [] Block;
				m_BlockIDs[i*m_iWidth+j]=NULL_ID;
			}
		}
		
}

template<class T>
inline void CBlockImage<T>::InitDafault(int iWidth, int iLength, float fPixelSize)
{
	/*
	int iSize = iWidth * iLength;
	T* pImage = new T[iSize];
	memset(pImage, 1, iSize*sizeof(T));
	Init(pImage, iWidth, iLength, fPixelSize);
	delete [] pImage;
	*/

	// Firstly, we release myself
	Release();
	
	m_iImageWidth=iWidth;
	m_iImageLength=iLength;
	m_fPixelSize=fPixelSize;

	m_iWidth=iWidth >> m_iBlockSizeExp;
	if(iWidth & (m_iBlockSize - 1))	m_iWidth++;

	m_iLength=iLength >> m_iBlockSizeExp;
//	int  iLastBlkLength= iLength & (m_iBlockSize - 1);
	if(iLength & (m_iBlockSize - 1))	m_iLength++;

	int iSize = m_iWidth * m_iLength;
	m_BlockIDs= new int[iSize];
	for(int i=0; i< iSize; i++)
		m_BlockIDs[i]=NULL_ID;
	//*/
}
template<class T>
bool CBlockImage<T>::Save(const char * szName)
{
	FILE * fp = fopen(szName, "wb");
	if (!fp)
	{
		return false;
	}

	bool bRet = Save(fp);
	fflush(fp);
	fclose(fp);
	return bRet;
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
	if (BLOCKIMAGE_VER==0x00000002)
	{
		BufSize= BufSize + sizeof(DWORD) + sizeof(T);
	}
	dwWrite= BufSize;
	dwWriteLen = fwrite(&dwWrite, 1, sizeof(DWORD), pFileToSave);
	if(dwWriteLen != sizeof(DWORD))
		return false;

	// write the following info in order
	// T's size          (BLOCKIMAGE_VER==0x00000002)
	// default T value   (BLOCKIMAGE_VER==0x00000002)
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
	if (BLOCKIMAGE_VER==0x00000002) 
	{
		* (DWORD *) (buf+cur) = sizeof(T);
		cur+=sizeof(DWORD);
		
		* (T *) (buf+cur) = m_defaultVal;
		cur+=sizeof(T);	
	}

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

	for(DWORD i=0; i<m_arrBlocks.size(); i++)
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
bool CBlockImage<T>::Load( FILE *pFileToLoad )
{
	if(!pFileToLoad) return false;
	
	DWORD dwRead, dwReadLen;

	// Read the Version
	dwReadLen = fread(&dwRead, 1, sizeof(DWORD), pFileToLoad);
	if(dwReadLen != sizeof(DWORD))
		return false;

	if(dwRead<=BLOCKIMAGE_VER)
	{
		// Current Version or old vertion
		DWORD dwFileVertion = dwRead;
		// Read the buf size
		dwReadLen = fread(&dwRead, 1, sizeof(DWORD), pFileToLoad);
		if(dwReadLen != sizeof(DWORD))
			return false;

		int BufSize=dwRead;
		UCHAR * buf = new UCHAR[BufSize];

		// Read the data
		dwReadLen = fread(buf, 1, BufSize, pFileToLoad);
		if( dwReadLen != (DWORD)BufSize )
		{
			delete [] buf;
			return false;
		}

		int cur=0;
		if (0x00000002 == BLOCKIMAGE_VER) 
		{
			if (dwFileVertion==BLOCKIMAGE_VER) 
			{
				DWORD TSize=* (DWORD *) (buf+cur);
				cur+=sizeof(DWORD);
				if (TSize!=sizeof(T)) 
				{
					delete [] buf;
					return false;
				}
				m_defaultVal =*(T*)(buf+cur);
				cur+=sizeof(T);
			}else if(0x00000001 == dwFileVertion)
			{
				m_defaultVal = T(0);
			}
		}
		
		Release();			// Release the old data
		
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

		m_arrBlocks.reserve(arrBlkSize);
		for(int i=0; i<arrBlkSize; i++)
		{
			T* pTBlock= (T *) (new UCHAR[BlkSize]);
			memcpy(pTBlock, buf+cur, BlkSize);
			m_arrBlocks.push_back(pTBlock);
			cur+=BlkSize;
		}

		// Added by wenfeng, 05-9-12
		// How can you forget this???
		delete [] buf;			

		return true;
	}
    else
	{
		return false;
	}
	
}

template<class T>
bool CBlockImage<T>::Load( const char * szName )
{
	AFileImage   fileimg;
	if (!fileimg.Open(szName, AFILE_BINARY|AFILE_OPENEXIST))
	{
		return false;
	}
	unsigned long readlen;
	DWORD flag;
	
	fileimg.Read(&flag, sizeof(DWORD), &readlen);
	
	if (flag > BLOCKIMAGE_VER)
	{
		return false;
	}
	DWORD BufSize;
	fileimg.Read(&BufSize, sizeof(DWORD), &readlen);
	UCHAR * buf = new UCHAR[BufSize];
	// Read the data
	fileimg.Read(buf, sizeof(UCHAR) * BufSize, &readlen);
	if (readlen != BufSize)
	{
		delete[]  buf;
		return false;
	}

	int cur=0;
	if (0x00000002 == BLOCKIMAGE_VER) 
	{
		if (flag==BLOCKIMAGE_VER) 
		{
			DWORD TSize=* (DWORD *) (buf+cur);
			cur+=sizeof(DWORD);
			if (TSize!=sizeof(T)) 
			{
				delete [] buf;
				return false;
			}
			m_defaultVal =*(T*)(buf+cur);
			cur+=sizeof(T);
		}else if(0x00000001 == flag)
		{
			m_defaultVal = T(0);
		}
	}

	Release();			// Release the old data
		
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
	m_arrBlocks.reserve(arrBlkSize);

	for(int i=0; i<arrBlkSize; i++)
	{
		T* pTBlock= (T *) (new UCHAR[BlkSize]);
		memcpy(pTBlock, buf+cur, BlkSize);
		m_arrBlocks.push_back(pTBlock);
		cur+=BlkSize;
	}

	// Added by wenfeng, 05-9-12
	// How can you forget this???
	delete [] buf;			
	return true;
}


}	// end of the namespace

#endif
