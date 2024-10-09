#include "ECScriptGlobalResourceInGame.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EL_BackMusic.h"
#include "AUIObject.h"

#define VOICE_PATH "Help\\Voice\\"

CECScriptGlobalResourceInGame::~CECScriptGlobalResourceInGame()
{
	ReleaseAllResource();
}


void CECScriptGlobalResourceInGame::ReleaseAllResource()
{
	RemoveFlashItemAll();
	StopVoice(true);
}

void CECScriptGlobalResourceInGame::FlashItem(AUIObject * pItem, bool bFlash)
{
	pItem->SetFlash(bFlash);
	if( bFlash )
		m_vecFlashItem.push_back(pItem);
	else
	{
		int i;
		for( i = 0; i < (int)m_vecFlashItem.size(); i++ )
			if( m_vecFlashItem[i] == pItem )
			{
				m_vecFlashItem.erase(m_vecFlashItem.begin() + i);
				i--;
			}
	}
}

void CECScriptGlobalResourceInGame::RemoveFlashItemAll()
{
	int i;
	for( i = 0; i < (int)m_vecFlashItem.size(); i++ )
		m_vecFlashItem[i]->SetFlash(false);
	m_vecFlashItem.clear();
}

void CECScriptGlobalResourceInGame::PlayVoice(AString strFileName)
{
	g_pGame->GetBackMusic()->PlayCommentator(AString(VOICE_PATH) + strFileName);
}

void CECScriptGlobalResourceInGame::StopVoice(bool bFadeOut)
{
	g_pGame->GetBackMusic()->StopCommentator(bFadeOut);
}

