/********************************************************************
	created:	2012/1/22
	created:	12:11:2012   16:31
	file base:	DlgMeridians
	file ext:	h
	author:		zhougaomin01305
	
*********************************************************************/


#pragma once

#include "DlgBase.h"
#include <vector>
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"
#include "AUITextArea.h"
#include "EC_Meridians.h"

// �����л��з���\r���ַ�����������ʾ�ַ�
class CTextAppearAnimator
{
public:
	CTextAppearAnimator(){}
	~CTextAppearAnimator(){}
	CTextAppearAnimator(const ACString& linesoftext, PAUITEXTAREA pTextArea, int timeInterval);
	void Tick();
	void Reset();
	void SetText(const ACString& strLines);
	void SetTextArea(PAUITEXTAREA pTextArea){ m_pTextArea = pTextArea; }
	void SetTimeInterval(int timeInterval);

protected:
	PAUITEXTAREA	m_pTextArea;
	DWORD			m_dwTimeIntervalBetweenLines;

	int				m_iLines;
	DWORD			m_dwCurrentLine;
	typedef abase::vector<ACString> LINESOFTEXT;
	LINESOFTEXT		m_vecLinesOfText;
	DWORD			m_dwLastTick;
};
// ��������
class CDlgMeridians : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP();
protected:
	typedef std::vector<PAUISTILLIMAGEBUTTON> MeridiansBtnVec;

	
	enum
	{
		IMAGE_IMPACTED,			// �ѳ�
		IMAGE_TO_IMPACT,		// �ɳ�
		IMAGE_WAIT_IMPACT,		// δ��
		IMAGE_TO_IMPACT_HOVER,  // �ɳ����
		IMAGE_TO_IMPACT_DOWN,   // �ɳ尴��
		IMAGE_NUM,				
	};
	char*				m_pbtnImages[IMAGE_NUM];

	MeridiansBtnVec		m_btns[CECMeridians::MeridiansLevelLayer + 1];
	PAUIIMAGEPICTURE	m_pMeridiansGfx;
	PAUIIMAGEPICTURE	m_pLevelGfx;
	PAUIIMAGEPICTURE	m_pTextGfx;
	int					m_iLastLayer;
protected:

	virtual void OnShowDialog();
	virtual bool OnInitDialog();

	void SetBtnState(int level);
	void SetLayerInfo(int level);

	virtual bool OnChangeLayoutBegin(){m_iLastLayer = -1;return true;}
	virtual void OnChangeLayoutEnd(bool bAllDone);

public:
	virtual void OnTick();
	void OnCommandImpact(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void OnEventLButtonUp_LevelGfx(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void OnMeridiansProcess();


	CDlgMeridians();
	virtual ~CDlgMeridians();
};
