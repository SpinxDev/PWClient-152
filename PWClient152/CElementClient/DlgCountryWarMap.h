// Filename	: DlgCountryWarMap.h
// Creator	: Han Guanghui
// Date		: 2012/7/31

#ifndef _DLGCOUNTRYWARMAP_H_
#define _DLGCOUNTRYWARMAP_H_ 

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUIlabel.h"
#include "AUICheckBox.h"
#include "EC_DomainCountry.h"
#include <vector>

class A3DGFXEx;


class CDlgCountryWarMap : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()
public:
	CDlgCountryWarMap();
	virtual ~CDlgCountryWarMap();

	void OnEventLButtonUp_CountryMap(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUp_CountryMap(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDBClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventRButtonDBClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventMButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventMButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventMButtonDBClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void OnCommandCancel(const char * szCommand);
	void OnCommandRefresh(const char * szCommand);
	void OnCommandOpenCountryMap(const char * szCommand);
	void OnCommandLeave(const char * szCommand);

	void OnGetPersonalScore();
	void OnGetMapInfo();

	void UpdateLiveShowImg();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);

protected:
	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();

	virtual bool Render();
	bool RenderFlag();
	bool RenderFlagDestination();
	void RenderStrongHoldState();
	void RenderRank();
	void RenderDeath();
	void TickRank();

	void UpdateBattleInfo();
	typedef std::vector<A3DGFXEx*> ImageContainer;
	void ReleaseImages(ImageContainer& container);
	AString	GetRankImage(bool offense, int rank);
	int GetSkullAlpha(int time_stamp, int current_time);

	PAUIIMAGEPICTURE		m_pImgWarMap;
	PAUILABEL				m_pTxtCombatTime;
	PAUILABEL				m_pTxtWarTime;
	PAUILABEL				m_pTxtKillCount;
	PAUILABEL				m_pTxtDeathCount;
	PAUILABEL				m_pTxtCountryKill;
	PAUILABEL				m_pTxtCountryDeath;
	PAUILABEL				m_pTxtComradesCount;
	PAUILABEL				m_pTxtEnemysCount;
	A2DSprite*				m_pImgFlag;
	A2DSprite*				m_pImgFlagDest;
	CSequenceImageAnimator	m_AnimFlagDest;
	ImageContainer			m_LiveShowRankImages[2];
	A2DSprite*				m_LiveShowDeathImages[2];
	int						m_iUpdateLiveShowTime;
	PAUICHECKBOX			m_pCheckRank;
	PAUICHECKBOX			m_pCheckDeath;
};




#endif //_DLGCOUNTRYWARMAP_H_