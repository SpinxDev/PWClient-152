// File		: EC_ContinueLoad.h
// Creator	: Xu Wenbin
// Date		: 2014/10/11

#ifndef _ELEMENTCLIENT_EC_CONTINUELOAD_H_
#define _ELEMENTCLIENT_EC_CONTINUELOAD_H_

#include "EC_Global.h"

//	连续加载时显示合并信息
class CECContinueLoad{
	int		m_iSaveBackImage;	//	连续加载时上次加载背景图
	int		m_iSaveTipIndex;	//	连续加载时上次提示下标
	float	m_fSaveLoadPos;		//	连续加载时上次加载进度条位置

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