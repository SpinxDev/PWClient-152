/*
 * FILE: DlgHomeBase.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_HOME_BASE_H_
#define _DLG_HOME_BASE_H_

#include "aui/AUIDialog.h"
#include "AutoScene.h"
#include "EC_HomeDlgsMgr.h"

#include "AUIManager.h"
#include "A3DMacros.h"

//Util Marc


#ifdef _DEBUG
	#define PROFILE_BEGIN() \
	unsigned int uBegin = GetTickCount(); 

	#define PROFILE_END() \
	unsigned int uEnd = GetTickCount(); \
	ACHAR szText[MAX_PATH]; \
	a_sprintf(szText, _AL("%d ms"), uEnd - uBegin); \
	GetAUIManager()->MessageBox("Report", szText, MB_OK, A3DCOLORRGBA(255, 255, 255, 160)); 
#else
	#define PROFILE_BEGIN()
	#define PROFILE_END()
#endif


class CECHomeDlgsMgr;
class AUIStillImageButton;
class AutoScene;
class CAutoParameters;
class AUIListBox;
class CECGame;

class CDlgHomeBase: public AUIDialog
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
	
public:
	CDlgHomeBase();
	virtual ~CDlgHomeBase();
	
	bool InitContext(CECHomeDlgsMgr *pECHomeDlgsMgr);
	
	virtual AUIStillImageButton * GetSwithButton() { ASSERT(false);  return NULL; }
	
	int GetActionCount() { return m_nActionCount;}
	void SetActionCount(int i_nVal) {  m_nActionCount = i_nVal;}
	
protected:
	CECGame * GetGame();

	CECHomeDlgsMgr* GetHomeDlgsMgr() { return m_pECHomeDlgsMgr;}

	virtual bool OnInitContext() { return true; }
	virtual void OnShowDialog();

	void OnCommandSwitchDialog(const char * szCommand);
	void OnCommandCancel(const char* szCommand);

	inline AUIManager* GetAUIManager() const{return m_pECHomeDlgsMgr->GetAUIManager();}

	CAutoScene* GetAutoScene() const;
	CAutoParameters* GetAutoParam() const;

	int GetOperationType() const;
	void SetOperationType(int nType);
	
	//Utils 
	bool IsValidIdx(int a_nIdx, AUIListBox* a_pListBox);
	int Clamp(int &i_nVal, const int i_nUpper, const int i_nLower);
	
protected:
	CECHomeDlgsMgr* m_pECHomeDlgsMgr;
	
	int m_nActionCount;
};

#endif 