// Filename	: DlgWikiEquipmentDrop.h
// Creator	: Feng Ning
// Date		: 2010/04/22

#pragma once

#include "DlgWikiMonsterList.h"

class CDlgWikiEquipmentDrop : public CDlgWikiMonsterList
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWikiEquipmentDrop();
	
	virtual void SetSearchDataPtr(void* ptr);
};