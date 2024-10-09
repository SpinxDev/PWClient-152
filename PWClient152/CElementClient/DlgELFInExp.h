// Filename	: DlgELFInExp.h
// Creator	: Chen Zhixin
// Date		: October 07, 2008

#pragma once

#include "DlgBase.h"
#include "EC_RoleTypes.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIProgress.h"
#include "EC_HostGoblin.h"
#include "EC_IvtrItem.h"

class CECIvtrGoblinExpPill;

class CDlgELFInExp : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgELFInExp();
	virtual ~CDlgELFInExp();
	virtual bool Render(void);
	virtual void OnTick(void);
	
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_OK(const char * szCommand);
	void OnCommand_Reset(const char * szCommand);
	void OnCommand_QuickSet(const char * szCommand);
	void OnCommand_PillInject(const char * szCommand);
	void OnCommand_HostInject(const char * szCommand);
	void OnCommand_SPInject(const char * szCommand);

	void OnEventRemovePill(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void SetItem(CECIvtrItem *pItem1, int nSlot);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	
	PAUIIMAGEPICTURE	m_pImg_ELF;
	PAUILABEL			m_pTxt_CurrenExp;
	PAUILABEL			m_pTxt_Level;
	PAUIPROGRESS		m_pPgs_Inject;

	int					m_iSlot;
	
	unsigned int		m_iBase;
	unsigned int		m_iBasePill;
	unsigned int		m_iGoblinBase;
	unsigned int		m_iGoblinBasePill;
	unsigned int		m_iBaseSP;
	unsigned int		m_iGoblinBaseSP;
	bool				m_bInjecting;
	DWORD				m_dwStartTime;
	DWORD				m_dwLastTime;
	PAUIOBJECT			m_pButton;
	int					m_iInjectSrc;
	DWORD				m_dwFreezeTime;
	float				m_fRange;
	bool				m_bCanInject;
	void RefreshHostDetails();
	virtual bool OnInitDialog();

	CECIvtrGoblinExpPill * GetValidExpPill();

	// return false to cancel the change action on current object
	virtual bool OnChangeLayout(PAUIOBJECT pMine, PAUIOBJECT pTheir);
};