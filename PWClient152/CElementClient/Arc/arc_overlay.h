
#pragma once

#include <windows.h>
#include "CC_Datatype.h"
#include "CC_Error.h"

class AWString;

namespace CC_SDK {

	extern "C"{
	typedef struct _GAMEWINDOWMESSAGE
	{
		HWND hWnd;
		DWORD dwMsgId;
	}GAMEWINDOWMESSAGE, *PGAMEWINDOWMESSAGE;
	typedef CC_HANDLE (*CC_Init)(unsigned int nPartnerId, unsigned int nAppId, PGAMEWINDOWMESSAGE pAppWnd);

	typedef HRESULT (*CC_UnInit)(const CC_HANDLE hSDK);
	typedef HRESULT (*CC_GotoUrlInOverlay)(const CC_HANDLE hSDK, wchar_t* pszUrl);
	typedef HRESULT (*CC_GetAccountName)(const CC_HANDLE hSDK, wchar_t* pszAccount,unsigned int nAccountLen);
	typedef HRESULT (*CC_GetToken)(const CC_HANDLE hSDK, wchar_t* pszAccount, wchar_t* pszToken, unsigned int nTokenLen, unsigned long dwTimeout = 60000);
	}

	class ArcOverlay
	{
	public:
		static ArcOverlay& Instance();
		~ArcOverlay();		
		bool Init();
		bool GotoURL(const wchar_t *pszUrl);
		bool GetAccountName(AWString &strAccount);
		bool GetToken(const wchar_t* pszAccount, AWString &strToken);
		void UnInit();
		bool IsLoaded()const{ return m_bLoaded; }
		
	private:
		ArcOverlay();

		ArcOverlay(const ArcOverlay&);
		void operator=(const ArcOverlay&);
		
		HMODULE				m_hModule;
		CC_HANDLE			m_hCCSDK;
		CC_Init				m_init;
		CC_UnInit			m_uninit;
		CC_GotoUrlInOverlay	m_gotoURL;
		CC_GetAccountName	m_getAccountName;
		CC_GetToken			m_getToken;
		volatile bool		m_bLoaded;
	};

}  // namespace CC_SDK