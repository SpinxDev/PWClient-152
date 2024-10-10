/*
 * FILE: DlgAutoGenLightMapProgress.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/2/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_GEN_LIGHT_MAP_PROGRESS_H_
#define _DLG_AUTO_GEN_LIGHT_MAP_PROGRESS_H_

#include "DlgHomeBase.h"

class AUIProgress;


enum GenLMProgress
{
	GLMP_O,
	GLMP_1,
	GLMP_2,
	GLMP_3,
	GLMP_4,
	GLMP_5
};

class CDlgAutoGenLightMapProgress: public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgAutoGenLightMapProgress();
	virtual ~CDlgAutoGenLightMapProgress();
	
	
	void SetRunLightMap(bool i_bRunLightMap) { m_bRunLightMap = i_bRunLightMap;}

protected:

	virtual void OnShowDialog() ;
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	
	virtual void OnTick();
	void SetProgress(int i_nProgress);
	
	void OnCommandCancel(const char* pszCommand);
	
protected:
	AUIProgress* m_pProgressGenLightMap;
	bool m_bRunLightMap; 
	int m_nStep;
};

#endif // _DLG_AUTO_GEN_LIGHT_MAP_PROGRESS_H_