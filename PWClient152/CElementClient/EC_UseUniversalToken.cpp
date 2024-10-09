// File		: EC_UseUniversalToken.cpp
// Creator	: Xu Wenbin
// Date		: 2014/4/11

#include "EC_UseUniversalToken.h"
#include "EC_UniversalTokenVisitHTTPCommand.h"
#include "EC_IvtrConsume.h"

#include "ExpTypes.h"

#include <windows.h>
#include <MMSystem.h>

//	class CECUseUniversalTokenCommandManager::UseCommandConfig
CECUseUniversalTokenCommandManager::UseCommandConfig::UseCommandConfig(CECUseUniversalTokenCommand *pCmd, DWORD maxUseTime)
: m_pCommand(pCmd)
, m_maxUseTime(maxUseTime)
{
	m_useTime = ::timeGetTime();
}

bool CECUseUniversalTokenCommandManager::UseCommandConfig::IsTimeUp(DWORD currentTime)const
{
	return currentTime >= (m_useTime + m_maxUseTime);
}

void CECUseUniversalTokenCommandManager::UseCommandConfig::DeleteCommand()
{
	delete m_pCommand;
	m_pCommand = NULL;
}

//	class CECUseUniversalTokenCommandManager
CECUseUniversalTokenCommandManager & CECUseUniversalTokenCommandManager::Instance()
{
	static CECUseUniversalTokenCommandManager s_instance;
	return s_instance;
}

CECUseUniversalTokenCommandManager::CECUseUniversalTokenCommandManager()
{
}

void CECUseUniversalTokenCommandManager::HandleRequest(const CECSSOTicketHandler::Request *p)
{
	bool bProcessed(false);
	for (InUseCommands::iterator it = m_inUseCommands.begin(); it != m_inUseCommands.end(); ++ it){
		UseCommandConfig & config = *it;
		int usageIndex = CECIvtrUniversalToken::MaskToUsageIndex(UTCS_VISIT_HTTP_WITH_TOKEN);
		if (config.m_pCommand->FeedBack(usageIndex, p)){
			config.DeleteCommand();
			m_inUseCommands.erase(it);
			bProcessed = true;
			break;
		}
	}
	if (!bProcessed){
		CECSSOTicketHandler::HandleRequest(p);
	}
}

bool CECUseUniversalTokenCommandManager::Use(CECIvtrUniversalToken *pItem, int usageIndex, DWORD maxUseTime/*=5000*/)
{
	bool bOK(false);
	if (pItem && usageIndex >= 0 && usageIndex < ELEMENTDATA_MAX_UNIVERSAL_TOKEN_USAGE && pItem->HasUsage(1 << usageIndex)){
		if (CECUseUniversalTokenCommand *pCommand = Create(pItem->GetDBEssence(), usageIndex)){
			if (pCommand->UseAndNeedFeedBack()){
				m_inUseCommands.push_back(UseCommandConfig(pCommand, maxUseTime));
			}else{
				delete pCommand;
			}
			bOK = true;
		}
	}else{
		ASSERT(false);
	}
	return bOK;
}

void CECUseUniversalTokenCommandManager::Clear()
{
	for (InUseCommands::iterator it = m_inUseCommands.begin(); it != m_inUseCommands.end(); ++ it){
		UseCommandConfig & config = *it;
		config.DeleteCommand();
	}
	m_inUseCommands.clear();
}

void CECUseUniversalTokenCommandManager::Tick()
{
	if (m_inUseCommands.empty()){
		return;
	}
	DWORD currentTime = ::timeGetTime();
	InUseCommands::iterator it = m_inUseCommands.begin();
	while (it != m_inUseCommands.end()){
		UseCommandConfig & config = *it;
		if (config.IsTimeUp(currentTime)){
			config.DeleteCommand();
			it = m_inUseCommands.erase(it);
		}else{
			++ it;
		}
	}
}

CECUseUniversalTokenCommand * CECUseUniversalTokenCommandManager::Create(const UNIVERSAL_TOKEN_ESSENCE *pDBEssence, int usageIndex)
{
	CECUseUniversalTokenCommand * pCommand = NULL;
	if (pDBEssence && usageIndex >= 0 && usageIndex < ELEMENTDATA_MAX_UNIVERSAL_TOKEN_USAGE){
		unsigned int usageMask = CECIvtrUniversalToken::UsageIndexToMask(usageIndex);
		switch (usageMask){
		case UTCS_VISIT_HTTP_WITH_TOKEN:
			pCommand = new CECUniversalTokenVisitHTTPCommand(pDBEssence, usageIndex);
			break;
		}
	}
	return pCommand;
}