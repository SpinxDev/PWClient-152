// Filename	: DlgLevel2UpgradeShow.h
// Creator	: Xu Wenbin
// Date		: 2013/10/13

#pragma once

#include "DlgBase.h"
#include "EC_TimeSafeChecker.h"

class CECLoginPlayer;
class AUIImagePicture;
class CDlgLevel2UpgradeShow: public CDlgBase
{		
public:
	CDlgLevel2UpgradeShow();

	void SetNewLevel2(int nLevel2);
	
protected:
	CECLoginPlayer	*	m_pPlayer;
	AUIImagePicture *	m_pImg_Char;
	int					m_nNewLevel2;
	CECTimeSafeChecker	m_pointJustReadyGfxPlayTimer;
	bool				m_pointJustReadyGfxPlayed;
	CECTimeSafeChecker	m_hideTimer;
	bool				m_bFitWindowNextTick;

protected:	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool Release();
	virtual void OnTick();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);

	void LoadPlayer();
	void ReleasePlayer();

	void UpdateRenderCallback();
	bool CanShow()const;
	bool CanHide()const;
	DWORD GetCameraParameter()const;
	void FitWindow();

	void PlayGfx();

	void PlayBaseRankGfx();
	void PlayGodRankGfx();
	void PlayEvilRankGfx();

	void PlayPointReadyGfxAtLevel(int iLevel2);
	void PlayPointJustReadyGfxAtLevel(int iLevel2);
	void PlayPointNotReadyGfxAtLevel(int iLevel2);

	void PlayLineReadyGfxToLevel(int iLevel2);
	void PlayLineNotReadyGfxToLevel(int iLevel2);
	void PlayLineJustReadyGfxToLevel(int iLevel2);

	AUIImagePicture * GetPointControlAtLevel(int iLevel2);
	AUIImagePicture * GetLineControlToLevel(int iLevel2);

	static int GetPointControlIndexForLevel(int iLevel2);
	static int GetLineControlIndexForLevel(int iLevel2);

	static const char * GetPointReadyGfxFile();
	static const char * GetPointNotReadyGfxFile();
	static const char * GetPointJustReadyGfxFile();

	static const char * GetLineReadyGfxFile(int iLevel2);
	static const char * GetLineNotReadyGfxFile(int iLevel2);
	static const char * GetLineJustReadyGfxFile(int iLevel2);
};