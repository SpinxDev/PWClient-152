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
	bool IsRandomDyeJustUsed();				// �Ƿ�ո�ʹ�������Ⱦɫ��
	void SetRandomDyeUsed(bool bFlag);		// ����ʹ�������Ⱦɫ��
	virtual void OnTick();
	void StartShowTip(const ACString& strColor);
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void Resize();
private:
	bool m_bUsedRandomDye;			// ֮ǰ�Ƿ�ʹ�������Ⱦɫ��
	DWORD m_dwRandomDyeUsedTime;	// ���Ⱦɫ��ʹ�õ�ʱ��
	DWORD m_dwStartTime;			// ��ʼ������ʾ��ʱ��

	AUILabel* m_pLblColorName;
	AUILabel* m_pLblText;
	AUIImagePicture* m_pImgGfx;
};

#endif