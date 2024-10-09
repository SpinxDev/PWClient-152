// Filename	: DlgScriptTip.h
// Creator	: zhangyitian
// Date		: 2014/07/24

#ifndef _ELEMENTCLIENT_DLGSCRIPTTIP_H_
#define _ELEMENTCLIENT_DLGSCRIPTTIP_H_

#include "DlgBase.h"

class CECScriptTipWorkMan;
class AUILabel;
class AUIImagePicture;

class CDlgScriptTip : public CDlgBase {

	friend class CECScriptTipWorkSetTitleAndContent;
	friend class CECScriptTipWorkMove;
	friend class CECScriptTipWorkGfx;

public:
	CDlgScriptTip();
	virtual ~CDlgScriptTip();
	virtual void DoDataExchange(bool bSave);
	CECScriptTipWorkMan* GetTipWorkMan();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);

	void SetTitleAndContent(const ACString& strTitle, const ACString& strContent);
	void ShowTip(int iGfxTime, int iInTime, int iStayTime, int iOutTime);
	bool IsFree();

protected:
	virtual void OnTick();
	virtual void OnShowDialog();
private:
	void SetTitle(const ACString& strTitle);		// 设置标题
	void SetContent(const ACString& strContent);	// 设置内容
	void SetShowPercent(float fShowPercent);		// 设置显示在屏幕上的百分比，0表示不显示，1表示完全显示
	void PlayGfx();
	void StopGfx();
private:
	AUILabel* m_pLblTitle;
	AUILabel* m_pLblContent;
	AUIImagePicture* m_pImgPic;
	float m_fCurShowPercent;
	CECScriptTipWorkMan* m_pTipWorkMan;
};

#endif