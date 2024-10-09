// Filename	: DlgPrewviewProduce.h
// Creator	: Han Guanghui
// Date		: 2012/05/02

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIEditBox.h"
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"

class A3DGFXEx;

class CDlgPreviewProduce: public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgPreviewProduce();

	void OnCommandReproduce(const char *szCommand);
	void OnCommandSelectOldOne(const char *szCommand);
	void OnCommandSelectNewOne(const char *szCommand);
	void OnCommandCANCEL(const char * szCommand);

	void SetOldInfo(CECIvtrItem *pItem, unsigned int itemID, bool inheritRefine, bool inheritHole, bool inheritStone,bool inheritEngrave);
	void SetPreviewInfo(void* pData);
	void OnCannotProduce();
	void ONCanProduce(){m_bCannotProduce = false;}
	bool OnProduceStart(int type);
	bool OnProduceEnd();
	
	virtual void Show(bool bShow, bool bModal = false, bool bActive = true);

protected:
	virtual bool OnInitDialog();
	virtual void OnTick();

	void ShowGfxWhenProducing();
	void ShowGfxWhenProduced();
	void ShowGfxWhenSelected(bool bNew);
	void ShowGfxBeforeProduce();
	void AnimationStart();
	void AnimationTick();
	void AnimationStop();
	void ThinkingTimeEnd();
	void EnableReproduce(bool bEnable);

private:
	int m_iStopThinkingTime;
	bool m_bInheritRefine;
	bool m_bInheritHole;
	bool m_bInheritStone;
	bool m_bInheritEngrave;

	bool m_bThinking;
	PAUIOBJECT			m_pObjClose;
	PAUISTILLIMAGEBUTTON m_pButtonSelectOld;
	PAUISTILLIMAGEBUTTON m_pButtonSelectNew;
	PAUISTILLIMAGEBUTTON m_pButtonReproduce;
	PAUITEXTAREA	m_pTextAttributeBefore;
	PAUITEXTAREA	m_pTextAttributeAfter;
	PAUILABEL		m_pLabelTime;
	PAUIIMAGEPICTURE m_pImgOld;
	PAUIIMAGEPICTURE m_pImgNew;
	A3DGFXEx*		 m_pGfxLightLeft;
	A3DGFXEx*		 m_pGfxLightRight;
	A3DGFXEx*		 m_pGfxCore;
	DWORD			 m_iAnimationTickStart;
	bool			 m_bAnimationPlaying;
	bool			 m_bCannotProduce;
};