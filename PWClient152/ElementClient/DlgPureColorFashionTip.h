// Filename	: DlgPureColorFashionTip.h
// Creator	: zhangyitian
// Date		: 2014/8/26

#ifndef _ELEMENTCLIENT_DLGPURECOLORFASHIONTIP_H_
#define _ELEMENTCLIENT_DLGPURECOLORFASHIONTIP_H_

#include "DlgBase.h"
#include <AAssist.h>

class AUILabel;
class AUIImagePicture;

class CDlgPureColorFashionTip : public CDlgBase {
public:
	CDlgPureColorFashionTip();
	bool IsRandomDyeJustUsed();				// 是否刚刚使用了随机染色剂
	void SetRandomDyeUsed(bool bFlag);		// 设置使用了随机染色剂
	virtual void OnTick();
	void StartShowTip(const ACString& strColor);
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void Resize();
private:
	bool m_bUsedRandomDye;			// 之前是否使用了随机染色剂
	DWORD m_dwRandomDyeUsedTime;	// 随机染色剂使用的时间
	DWORD m_dwStartTime;			// 开始播放提示的时间

	AUILabel* m_pLblColorName;
	AUILabel* m_pLblText;
	AUIImagePicture* m_pImgGfx;
};

#endif