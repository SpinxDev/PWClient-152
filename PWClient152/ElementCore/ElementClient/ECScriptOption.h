/********************************************************************
	created:	2005/09/07
	created:	7:9:2005   15:52
	file name:	ECScriptOption.h
	author:		yaojun
	
	purpose:	option of help system
*********************************************************************/

#pragma once

#include <hashmap.h>

class CECScriptContext;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef void (*PONRECEIVEOPTIONS) (const void * pData, int iSize);

class CECScriptOption
{
private:
	typedef abase::hash_map<int, bool> CActiveStateMap;
	CActiveStateMap m_scriptStateMap;
	CActiveStateMap m_typeStateMap;
	bool m_bDisableHelp;

	bool m_bOptionLoaded;
	bool m_bOldScriptShrinked;
	CECScriptContext * m_pContext;
	DWORD m_dwSaveTime;
	bool m_bTickSave;
private:
	bool IsActive(int nKey, CActiveStateMap & map);
	void SetActive(int nKey, bool bActive, CActiveStateMap & map);
	void SetActiveAll(bool bActive, CActiveStateMap & map);

	virtual void BufferToOption(const void *pBuffer, int iSize);
	virtual void Option2Buffer(void *pBuffer, int &iSize);

	static void OnLoadFromServer(const void * pData, int iSize);
	static CECScriptOption * s_pThis;

protected:
	virtual void LoadOption(PONRECEIVEOPTIONS pfnCallBack) = 0;
	virtual void SaveOption(const void* pDataBuf, int iSize) = 0;
public:
	CECScriptOption();
	virtual ~CECScriptOption();

	bool Init(CECScriptContext *pContext);
	
	bool IsOptionLoaded();

	void SaveToServer();
	void LoadFromServer();	
	void TickSave();

	bool IsScriptActive(int nID);
	void SetScriptActive(int nID, bool bActive);
	void SetScriptActiveAll(bool bActive);
	bool IsScriptInOption(int nID);
	void RegisterScript(int nID);
	void ShrinkOldScriptInOption();

	bool IsTypeActive(int nType);
	void SetTypeActive(int nType, bool bActive);
	void SetTypeActiveAll(bool bActive);

	bool IsHelpDisabled();
	void SetDisableHelp(bool bDisable);
};


