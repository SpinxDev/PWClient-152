// File		: EC_ContinueLoad.h
// Creator	: Xu Wenbin
// Date		: 2014/10/11

#ifndef _ELEMENTCLIENT_EC_CONTINUELOAD_H_
#define _ELEMENTCLIENT_EC_CONTINUELOAD_H_

#include "EC_Global.h"

//	��������ʱ��ʾ�ϲ���Ϣ
class CECContinueLoad{
	int		m_iSaveBackImage;	//	��������ʱ�ϴμ��ر���ͼ
	int		m_iSaveTipIndex;	//	��������ʱ�ϴ���ʾ�±�
	float	m_fSaveLoadPos;		//	��������ʱ�ϴμ��ؽ�����λ��

	ELEMENTCLIENT_DECLARE_SINGLETON(CECContinueLoad);

public:
	bool IsInMergeLoad()const;
	bool IsContinueLastLoad()const;
	void SetSaveLoadPos(float pos){ m_fSaveLoadPos = pos; }
	float GetSaveLoadPos()const{ return m_fSaveLoadPos; }
	void SetSaveBackImage(int index){ m_iSaveBackImage = index; }
	int  GetSaveBackImage()const{ return m_iSaveBackImage; }
	void SetSaveTipIndex(int index){ m_iSaveTipIndex = index; }
	int  GetSaveTipIndex()const{ return m_iSaveTipIndex; }
};

#endif