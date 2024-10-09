#include "ECScriptOption.h"
#include <ABaseDef.h>
#include <AMemory.h>
#include "ECScriptContext.h"
#include "ECScriptController.h"
#include "ECScript.h"
#include <time.h>

#define TYPE_COUNT 7	// must less or equal than 15
#define WORD_BIT(n) (((WORD)1) << n)
#define OPTION_VERSION 1
#define OPTION_SERVER_TIMEOUT 10000


CECScriptOption * CECScriptOption::s_pThis = NULL;

CECScriptOption::CECScriptOption() :
	m_bOptionLoaded(false),
	m_pContext(NULL),
	m_bOldScriptShrinked(false),
	m_dwSaveTime(0),
	m_bTickSave(false)
{
}

CECScriptOption::~CECScriptOption()
{
}

bool CECScriptOption::IsOptionLoaded()
{
	return m_bOptionLoaded;
}

bool CECScriptOption::Init(CECScriptContext *pContext)
{
	m_pContext = pContext;
	
	return true;
}

void CECScriptOption::TickSave()
{
	// need to save ?
	if (!m_bTickSave)
		return;

	DWORD dwCurTime = time(NULL);
	// do not save in the first interval
	if (0 == m_dwSaveTime) 
		m_dwSaveTime = dwCurTime;
	// check interval
	if (dwCurTime - m_dwSaveTime < 15)
		return;
	m_dwSaveTime = dwCurTime;


	// do save
	int iSize = 0;
	Option2Buffer(NULL, iSize);
	ASSERT(iSize > 0);
	
	void *pBuffer = a_malloctemp(iSize);
	Option2Buffer(pBuffer, iSize);
	
	SaveOption(pBuffer, iSize);
	
	a_freetemp(pBuffer);

	// save finished
	m_bTickSave = false;
}


void CECScriptOption::SaveToServer()
{
	m_bTickSave = true;
}

void CECScriptOption::LoadFromServer()
{
	s_pThis = this;
	m_bOptionLoaded = false;
	LoadOption(OnLoadFromServer);
}

void CECScriptOption::OnLoadFromServer(const void * pData, int iSize)
{
	bool bUseDefault = (pData == NULL);
	WORD *pBuffer = NULL;
	if (bUseDefault)
	{
		// prepare default setting
		iSize = sizeof(WORD) +						// space for version number
			sizeof(WORD) +							// space for count variable
			sizeof(WORD);							// space for type option and autopop
		pBuffer = reinterpret_cast<WORD *>(a_malloctemp(iSize));

		*(pBuffer) = OPTION_VERSION;
		*(pBuffer + 1) = 0;
		*(pBuffer + 2) = static_cast<WORD>(-1) & 0x7fff;
	}
	else
	{
		pBuffer = static_cast<WORD *>(const_cast<void *>(pData));
	}

	// convert to option
	s_pThis->BufferToOption(pBuffer, iSize);

	// clean
	if (bUseDefault)
	{
		a_freetemp(pBuffer);
	}

	s_pThis->m_bOptionLoaded = true;
}

void CECScriptOption::BufferToOption(const void *pBuffer, int iSize)
{
	const WORD *pData = static_cast<const WORD *>(pBuffer);
	// get version
	WORD wVersion = *(pData++);
	// get count of active script
	WORD wScriptCount = *(pData++);
	// insert script
	m_scriptStateMap.clear();
	while (wScriptCount-- > 0)
	{
		WORD wID = *(pData++);
		bool bActive = wID & WORD_BIT(15) ? true : false;
		wID &= 0x7fff;
		m_scriptStateMap.insert(CActiveStateMap::value_type(wID, bActive));
	}
	// set type option
	m_typeStateMap.clear();
	WORD wTypeOption = *(pData++);
	for (int i = 0; i < TYPE_COUNT; i++)
	{
		m_typeStateMap.insert(CActiveStateMap::value_type(
			i, (wTypeOption & WORD_BIT(i) ? true : false)));
	}

	// disable help option
	m_bDisableHelp = wTypeOption & WORD_BIT(15) ? true : false;

	ASSERT(reinterpret_cast<const BYTE *>(pData) - static_cast<const BYTE *>(pBuffer) == iSize);
}

void CECScriptOption::Option2Buffer(void *pBuffer, int &iSize)
{
	if (!pBuffer)
	{
		iSize = sizeof(WORD) +								// space for version number
			sizeof(WORD) +									// space for count variable
			sizeof(WORD) * (int)m_scriptStateMap.size() +	// space for each active id
			sizeof(WORD);									// space for type option and autopop
	}
	else
	{
		WORD *pData = static_cast<WORD *>(pBuffer);

		// set version
		*(pData++) = OPTION_VERSION;
		
		// save script state
		*(pData++) = static_cast<WORD>(m_scriptStateMap.size());
		CActiveStateMap::iterator iter = m_scriptStateMap.begin();
		for (; iter != m_scriptStateMap.end(); ++iter)
		{
			// save script id
			*pData = static_cast<WORD>(iter.value()->first);
			// user highest bit to indicate whetcher the
			// script is active or not
			if (iter.value()->second)
			{
				*pData |= WORD_BIT(15);
			}
			++pData;
		}

		// type option
		WORD wTypeOption = 0;
		for (int i = 0; i < TYPE_COUNT; i++)
		{
			if (IsTypeActive(i))
				wTypeOption |= WORD_BIT(i);
		}

		// disable help option
		if (m_bDisableHelp)	wTypeOption |= WORD_BIT(15);

		*(pData++) = wTypeOption;

		ASSERT(reinterpret_cast<BYTE *>(pData) - static_cast<const BYTE *>(pBuffer) == iSize);
	}
}

bool CECScriptOption::IsActive(int nKey, CActiveStateMap & map)
{
	CActiveStateMap::iterator iter = map.find(nKey);
	// 如果表中没有，可能是策划新增的类别，直接返回true
	if (iter == map.end()) {
		return true;
	}
	ASSERT(iter != map.end());
	return iter->second;
}

void CECScriptOption::SetActive(int nKey, bool bActive, CActiveStateMap & map)
{
	CActiveStateMap::iterator iter = map.find(nKey);
	ASSERT(iter != map.end());
	
	iter->second = bActive;
}

void CECScriptOption::SetActiveAll(bool bActive, CActiveStateMap & map)
{
	CActiveStateMap::iterator iter = map.begin();
	for (; iter != map.end(); ++iter)
	{
		iter->second = bActive;
	}
}


bool CECScriptOption::IsScriptActive(int nID)
{
	return IsActive(nID, m_scriptStateMap);
}

void CECScriptOption::SetScriptActive(int nID, bool bActive)
{
	SetActive(nID, bActive, m_scriptStateMap);
}

void CECScriptOption::SetScriptActiveAll(bool bActive)
{
	SetActiveAll(bActive, m_scriptStateMap);
}

bool CECScriptOption::IsTypeActive(int nType)
{
	return IsActive(nType, m_typeStateMap);
}

void CECScriptOption::SetTypeActive(int nType, bool bActive)
{
	SetActive(nType, bActive, m_typeStateMap);
}

void CECScriptOption::SetTypeActiveAll(bool bActive)
{
	SetActiveAll(bActive, m_typeStateMap);
}


bool CECScriptOption::IsHelpDisabled()
{
	return m_bDisableHelp;
}

void CECScriptOption::SetDisableHelp(bool bDisable)
{
	m_bDisableHelp = bDisable;
}

bool CECScriptOption::IsScriptInOption(int nID)
{
	CActiveStateMap::iterator iter = m_scriptStateMap.find(nID);
	return (iter != m_scriptStateMap.end());
}

void CECScriptOption::RegisterScript(int nID)
{
	CActiveStateMap::iterator iter = m_scriptStateMap.find(nID);
	ASSERT(iter == m_scriptStateMap.end());

	m_scriptStateMap.insert(CActiveStateMap::value_type(nID, true));
}

void CECScriptOption::ShrinkOldScriptInOption()
{
	if (m_bOldScriptShrinked)
		return;

	CActiveStateMap::iterator iter = m_scriptStateMap.begin();
	while (iter != m_scriptStateMap.end())
	{
		if (!m_pContext->GetController()->IsScriptLoaded(iter->first))
		{
			m_scriptStateMap.erase(iter);
			iter = m_scriptStateMap.begin();
		}
		else
		{
			++iter;
		}
	}
	m_bOldScriptShrinked = true;
	SaveToServer();
}

