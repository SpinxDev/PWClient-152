// Filename	: DlgRedSpark.h
// Creator	: zhangyitian
// Date		: 2014/8/6

#ifndef _ELEMENTCLIENT_DLGREDSPARK_H_
#define _ELEMENTCLIENT_DLGREDSPARK_H_

#include "DlgBase.h"

class AUIImagePicture;

// 表示屏幕周围红色闪烁特效的对话框
class CDlgRedSpark : public CDlgBase {
public:
	CDlgRedSpark();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);
protected:
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool OnInitDialog();
	virtual void OnChangeLayoutEnd(bool bAllDone);
private:
	void SetView();
private:
	AUIImagePicture* m_pImgLeft;
	AUIImagePicture* m_pImgRight;
	AUIImagePicture* m_pImgTop;
	AUIImagePicture* m_pImgBottom;
};

#endif