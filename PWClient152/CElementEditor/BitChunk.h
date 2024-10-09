// BitChunk.h: interface for the CBitChunk class.
// Creator: QingFeng Xin
// Desc: 用位来存储二维数组,其值只能表示0或是1，目的是为了减少占用空间
// Date: 2004/9/10
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITCHUNK_H__25926671_1F46_411C_8589_81808EEA9409__INCLUDED_)
#define AFX_BITCHUNK_H__25926671_1F46_411C_8589_81808EEA9409__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBitChunk  
{
public:
	CBitChunk();
	virtual ~CBitChunk();

	//设置这个2维数组的尺寸，注意：设置尺寸时要破坏原有数据
	//数据都被初始化为0
	void SetSize(int width,int height);
	
	//设置二维数组的值，value 只能是0或1，如果大于0就是1，如果小于等于0就是0
	void SetValue( int w, int h, int value);

	//取回二维数组的值
	int GetVaule( int w, int h);

	//取得内存块的大小
	int GetChunkWidth(){ return m_nWidth; };
	
	//取得内存块的大小
	int GetChunkHeight(){ return m_nHeight; };

	//释放数据
	void Release();

	int GetByteNum(){ return m_nByteNum; };

	BYTE* GetData(){ return m_pData; };

private:
	int m_nWidth;
	int m_nHeight;
	BYTE *m_pData;
	int m_nByteNum;
};

#endif // !defined(AFX_BITCHUNK_H__25926671_1F46_411C_8589_81808EEA9409__INCLUDED_)
