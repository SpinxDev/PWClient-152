/********************************************************************
	created:	2005/09/09
	created:	9:9:2005   14:49
	file name:	ECScriptGlobalResourceInGame.h
	author:		yaojun
	
	purpose:	implement of IECScriptGlobalResource for game
*********************************************************************/

#pragma once

#include <vector.h>
#include "ECScriptGlobalResource.h"

class CECScriptGlobalResourceInGame : public IECScriptGlobalResource
{
	abase::vector<AUIObject*> m_vecFlashItem;
public:
	virtual ~CECScriptGlobalResourceInGame();
	virtual void ReleaseAllResource();
	virtual void FlashItem(AUIObject * pItem, bool bFlash);
	virtual void RemoveFlashItemAll();
	virtual void PlayVoice(AString strFileName);
	virtual void StopVoice(bool bFadeOut);
};