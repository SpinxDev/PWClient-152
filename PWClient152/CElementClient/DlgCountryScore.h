// Filename	: DlgCountryScore.h
// Creator	: Han Guanghui
// Date		: 2012/8/4

#ifndef _DLGCOUNTRYSCORE_H_
#define _DLGCOUNTRYSCORE_H_

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "A2DSprite.h"
#include "globaldataman.h"
#include "AUILabel.h"
class CDlgCountryScore : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgCountryScore();
	virtual ~CDlgCountryScore();

	void OnCommandCancel(const char * szCommand);

	void UpdateScore();
	void UpdateCountryImg();
	void OnCountryEnterWar();

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	
	PAUIIMAGEPICTURE		m_pDigit[2][2];
	PAUIIMAGEPICTURE		m_pPeopleCount[2][2];
	PAUIIMAGEPICTURE		m_pImgCountryDefense;
	PAUIIMAGEPICTURE		m_pImgCountryInvader;
	A2DSprite*				m_pSpriteCountry[DOMAIN2_INFO::COUNTRY_COUNT];
};

#endif // _DLGCOUNTRYSCORE_H_