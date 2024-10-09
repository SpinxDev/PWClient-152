// BitChunk.h: interface for the CBitChunk class.
// Creator: QingFeng Xin
// Desc: ��λ���洢��ά����,��ֵֻ�ܱ�ʾ0����1��Ŀ����Ϊ�˼���ռ�ÿռ�
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

	//�������2ά����ĳߴ磬ע�⣺���óߴ�ʱҪ�ƻ�ԭ������
	//���ݶ�����ʼ��Ϊ0
	void SetSize(int width,int height);
	
	//���ö�ά�����ֵ��value ֻ����0��1���������0����1�����С�ڵ���0����0
	void SetValue( int w, int h, int value);

	//ȡ�ض�ά�����ֵ
	int GetVaule( int w, int h);

	//ȡ���ڴ��Ĵ�С
	int GetChunkWidth(){ return m_nWidth; };
	
	//ȡ���ڴ��Ĵ�С
	int GetChunkHeight(){ return m_nHeight; };

	//�ͷ�����
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
