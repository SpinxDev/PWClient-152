/********************************************************************
	created:	2005/09/09
	created:	9:9:2005   14:47
	file name:	ECScriptGlobalResource.h
	author:		yaojun
	
	purpose:	manager for global resource
*********************************************************************/

#pragma once

#include <AString.h>
class AUIObject;

class IECScriptGlobalResource
{
public:
	virtual ~IECScriptGlobalResource() {}; 
	virtual void ReleaseAllResource() = 0;
	virtual void FlashItem(AUIObject * pItem, bool bFlash) = 0;
	virtual void RemoveFlashItemAll() = 0;
	virtual void PlayVoice(AString strFileName) = 0;
	virtual void StopVoice(bool bFadeOut) = 0;
};