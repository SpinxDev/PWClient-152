/*
 * FILE: DlgCustomize.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/8/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_CUSTOMIZE_H_
#define _DLG_CUSTOMIZE_H_

#ifdef _WIN32
	#pragma once
#endif

#include "DlgCustomizeBase.h"

class CDlgCustomize:public CDlgCustomizeBase
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgCustomize();
	virtual ~CDlgCustomize();

	void OnCommandOnCancel(const char* szCommand);
	void OnCommandOnOK(const char* szCommand);

	void OnTick();
protected:
	void RandomExpression();
};

#endif