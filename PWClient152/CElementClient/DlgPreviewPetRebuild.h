// Filename	: DlgPreviewPetRebuild.h
// Creator	: Wang Yongdong
// Date		: 2012/12/04

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIEditBox.h"
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"

class A3DGFXEx;

class CDlgPreviewPetRebuild: public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgPreviewPetRebuild();

	virtual void Show(bool bShow, bool bModal = false, bool bActive = true);
	void OnServerNotify(int cmd,void* pData);
	int GetPetIdx() const { return m_iPetIdx;}

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void ShowGfxWhenProducing();
	void ShowGfxWhenProduced();
	void ShowGfxWhenSelected(bool bNew);
	void AnimationStart();
	void AnimationTick();
	void ThinkingTimeEnd();

	void OnCommandReproduce(const char *szCommand);
	void OnCommandSelectOldOne(const char *szCommand);
	void OnCommandSelectNewOne(const char *szCommand);
	void OnCommandCANCEL(const char * szCommand);

	bool IsNature() const { return m_iType == 0;};
	bool IsInherit() const { return m_iType == 1;};
	void SwitchType(bool bNature);

	void SetOldInfo(int idx,bool bNature);
	void SetPreviewInfo(void* pData);
	void OnProduceEnd();

	// return false to cancel the whole change action
	virtual bool OnChangeLayoutBegin() ;
	// for notify user the layout changing is over
	virtual void OnChangeLayoutEnd(bool bAllDone);

private:
	int m_iStopThinkingTime;
	bool m_bThinking; // 收到培训或者洗髓的新信息后，开始思考。

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

	int				 m_iType; // -1: invalid; 0: nature; 1: inherit
	int				 m_iPetIdx;

	int m_iUseTime; // 读条时间

	PAUIIMAGEPICTURE m_pImgSkill[4];
	PAUILABEL		 m_pSkillName[4];

	bool			m_bMannualSel; // 手动选择

	bool			m_bGfxVisible[5]; // 保存gfx可见状态，在切换界面风格用
	char			m_szGfxFileOld[MAX_PATH];
	char			m_szGfxFileNew[MAX_PATH];
};