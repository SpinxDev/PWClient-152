/*
 * FILE: DlgAutoNote.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/2/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_AUTO_NOTE_H_
#define _DLG_AUTO_NOTE_H_

#include "DlgHomeBase.h"

class CDlgAutoNote : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoNote();
	virtual ~CDlgAutoNote();

};

#endif